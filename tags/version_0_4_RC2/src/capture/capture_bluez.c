/*
 *  bdremoteng - helper daemon for Sony(R) BD Remote Control
 *  Based on bdremoted, written by Anton Starikov <antst@mail.ru>.
 *
 *  Copyright (C) 2009  Michael Wojciechowski <wojci@wojci.dk>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/** \ingroup bluezif
 *  @{
 */

/*! \file capture_bluez.c
  \brief Bluez capture interface implementation.
*/

#include "capture_bluez.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hidp.h>

#include <poll.h>
#include <assert.h>

/** PSM corresponding to HID control channel. */
#define L2CAP_PSM_HIDP_CTRL 0x11
/** PSM corresponding to HID interrupt channel. */
#define L2CAP_PSM_HIDP_INTR 0x13

/** The max buffer used for reading from BT sockets. */
#define MAXBUFFERSIZE 1024

static const unsigned int moduleMask = MODULEMASK_BT_IMPL;

extern volatile sig_atomic_t __io_canceled;

/** Read from a socket, until timeout or error.
 * @returns 0 - daemon shutting down becase of signal from outside world. 
 *          1 - client disconnected. 
 *          2 - timeout.
 */
int readFromSocket(captureData* _capturedata, int _socket);

/** Check the source address of a connected socket against the address
 *  given as parameter.
 * 
 * @return 0 - unknown device or 1 - known device.
 */
int l2cap_checkSource(int _sock, const bdaddr_t* _address)
{
  struct sockaddr_l2 addr;
  socklen_t addrlen = (socklen_t)sizeof(addr);

  if (getpeername(_sock, (struct sockaddr *) &addr, &addrlen) < 0)
    {
      perror("getpeername");
      return BDREMOTE_FAIL;
    }

  assert(sizeof(addr.l2_bdaddr) == sizeof(*_address));

#if 0
  int i = 0;
  uint8_t* b0 = addr.l2_bdaddr.b;
  uint8_t* b1 = address;

  for (i = 0; i < sizeof(addr.l2_bdaddr); i++)
    {
      printf("byte %d, %d:%d\n", i, (int)b0, (int)b1);
    }
#endif

  if (memcmp(&addr.l2_bdaddr, _address, sizeof(*_address)) == 0)
    {
      return 1;
    }

  return 0;
}

/** l2cap version of listen(). */
int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog)
{
  struct sockaddr_l2 addr;
  socklen_t addrlen = (socklen_t)sizeof(addr);
  struct l2cap_options opts;
  int sk;

  if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
    {
      perror("socket");
      return BDREMOTE_FAIL;
    }

  memset(&addr, 0, (size_t)addrlen);
  addr.l2_family = AF_BLUETOOTH;
  bacpy(&addr.l2_bdaddr, bdaddr);
  addr.l2_psm = htobs(psm);

  if (bind(sk, (struct sockaddr *) &addr, addrlen) < 0)
    {
      perror("bind");
      (void)close(sk);
      return BDREMOTE_FAIL;
    }

  (void)setsockopt(sk, SOL_L2CAP, L2CAP_LM, &lm,
                   (socklen_t)sizeof(lm));

  memset(&opts, 0, sizeof(opts));
  opts.imtu = HIDP_DEFAULT_MTU;
  opts.omtu = HIDP_DEFAULT_MTU;
  opts.flush_to = 0xffff;

  (void)setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts,
                   (socklen_t)sizeof(opts));

  if (listen(sk, backlog) < 0)
    {
      perror("listen");
      (int)close(sk);
      return BDREMOTE_FAIL;
    }

  return sk;
}

/** l2cap version of accept(). */
int l2cap_accept(int sk, bdaddr_t *bdaddr)
{
  struct sockaddr_l2 addr;
  socklen_t addrlen = (socklen_t)sizeof(addr);
  int nsk;

  memset(&addr, 0, (size_t)addrlen);


  if ((nsk = accept(sk, (struct sockaddr *) &addr, &addrlen)) < 0)
    {
      return BDREMOTE_FAIL;
    }

  if (bdaddr)
    {
      bacpy(bdaddr, &addr.l2_bdaddr);
    }
  return nsk;
}

/** Return an error, if there is no actual device corresponding to the
 *  address given as argument.
 */

/* My one and only comment about the level of bluez documentation:
 * KURWA! 
 */
int bt_device_exists(bdaddr_t* _bdaddr)
{
  int found = 0;
  int ctl   = -1;
  struct hci_dev_info di;
  struct hci_dev_list_req* dl = NULL;
  struct hci_dev_req* dr      = NULL;
  int i = 0;

  if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) 
    {
      perror("Can't open HCI socket.");
      exit(1);
    }

  dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t));
  if (!dl)
    {
      perror("Can't allocate memory");
      return BDREMOTE_FAIL;
    }

  dl->dev_num = HCI_MAX_DEV;
  dr          = dl->dev_req;

  if (ioctl(ctl, HCIGETDEVLIST, (void *) dl) < 0) 
    {
      perror("Can't get device list");
      return BDREMOTE_FAIL;
    }

  for (i = 0; i< dl->dev_num; i++) 
    {
      di.dev_id = (dr+i)->dev_id;

      if (ioctl(ctl, HCIGETDEVINFO, (void *) &di) < 0)
	{
	  continue;
	}

      if (hci_test_bit(HCI_RAW, &di.flags) && !bacmp(&di.bdaddr, BDADDR_ANY)) 
	{
	  int dd = hci_open_dev(di.dev_id);
	  hci_read_bd_addr(dd, &di.bdaddr, 1000);
	  hci_close_dev(dd);
	}

      assert(sizeof(di.bdaddr) == sizeof(bdaddr_t));
      if (memcmp(&di.bdaddr, _bdaddr, sizeof(di.bdaddr)) == 0)
	{
	  found = 1;
	  break;
	}
    }

  close(ctl);
  free(dl);

  if (found)
    {
      return BDREMOTE_OK;
    }
  else
    {
      return BDREMOTE_FAIL;
    }
}

int readFromSocket(captureData* _capturedata, int _socket)
{
  struct pollfd p;
  sigset_t sigs;
  /* Number of seconds since last read which returned data. */
  int numberOfSeconds  = 0;
  int recv_len         = 0;

  char buffer[MAXBUFFERSIZE];
  struct timespec timeout;

  p.fd = _socket;
  p.events = POLLIN | POLLERR | POLLHUP;

  sigfillset(&sigs);
  sigdelset(&sigs, SIGCHLD);
  sigdelset(&sigs, SIGPIPE);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGHUP);

  while (!__io_canceled)
    {
      if (numberOfSeconds > _capturedata->timeout)
        {
          BDREMOTE_DBG(_capturedata->config->debug, "Timeout.");
          return 2;
        }

      p.revents = 0;

      timeout.tv_sec  = 1;
      timeout.tv_nsec = 0;

      if (ppoll(&p, 1, &timeout, &sigs) < 1)
        {
          BDREMOTE_DBG(_capturedata->config->debug, "No BT data captured ..");
          numberOfSeconds++;
          continue;
        }
      numberOfSeconds = 0;

      recv_len=recv(_socket, &buffer[0], MAXBUFFERSIZE, 0);

      if (recv_len <= 0)
        {
          return 1;
        }
      else
        {
          BDREMOTE_DBG(_capturedata->config->debug, "Calling DataInd.");
#if BDREMOTE_DEBUG
          assert(_capturedata->magic0 == 127);
#endif /* BDREMOTE_DEBUG */
          DataInd(_capturedata->p, &buffer[0], recv_len);
        }
    }
  return 0;
}

/** Called by captureLoop to do the actual work. */
void run_server(captureData* _capturedata,
                int csk, int isk)
{
  struct pollfd p[2];
  sigset_t sigs;
  short events;
  int ncsk = -1;
  int nisk = -1;
  bdaddr_t destinationAddress;
  struct timespec timeout;
  int stat = -1;

  if (str2ba(_capturedata->dest_address, &destinationAddress) < 0)
    {
      BDREMOTE_ERR("str2ba call failed.");
      BDREMOTE_LOG(_capturedata->config->debug,
                   fprintf(printStream, "str2ba call failed. Input was '%s'.", _capturedata->bt_dev_address);
                   );
      return;
    }

  BDREMOTE_LOG(_capturedata->config->debug,
               fprintf(printStream, "Using destination address: %s", _capturedata->dest_address);
               );

  sigfillset(&sigs);
  sigdelset(&sigs, SIGCHLD);
  sigdelset(&sigs, SIGPIPE);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGHUP);

  p[0].fd = csk;
  p[0].events = POLLIN | POLLERR | POLLHUP;

  p[1].fd = isk;
  p[1].events = POLLIN | POLLERR | POLLHUP;

  while (!__io_canceled)
    {
      p[0].revents = 0;
      p[1].revents = 0;

      timeout.tv_sec = 1;
      timeout.tv_nsec = 0;

      if (ppoll(p, 2, &timeout, &sigs) < 1)
        {
          BDREMOTE_DBG(_capturedata->config->debug, "No data captured ..");
          continue;
        }

      events = p[0].revents | p[1].revents;

      if (events & POLLIN)
        {
          ncsk = l2cap_accept(csk, NULL);
          nisk = l2cap_accept(isk, NULL);

          BDREMOTE_DBG(_capturedata->config->debug, "Accepted BT client.");

          if (l2cap_checkSource(ncsk, &destinationAddress))
            {
              BDREMOTE_DBG(_capturedata->config->debug, "Known device.");
              RemoteConnected(_capturedata->p);

              stat = readFromSocket(_capturedata, nisk);

              switch (stat)
                {
                case 0:
                  break;
                case 1:
                  BDREMOTE_DBG(_capturedata->config->debug, "BT client disconnect..");
                  break;
                case 2:
                  BDREMOTE_DBG(_capturedata->config->debug, "BT timeout..");
                  break;
                }

              RemoteDisconnected(_capturedata->p);
            }
          else
            {
              BDREMOTE_DBG(_capturedata->config->debug, "Unknown device.");
            }

          close(nisk);
          sleep(1);
          close(ncsk);
        }
    }
}

int InitcaptureLoop(captureData* _capturedata)
{
  bdaddr_t bdaddr;
  int ctl, csk, isk;
  int lm = 0;

  /* Find the first interface, using BT primitives. */
  struct hci_dev_info devinfo;
  int ret = 0;

  if (_capturedata->bt_dev_address == NULL)
    {
      /* Use any BT interface. */
      ret = hci_devinfo(0, &devinfo);

      if (ret < 0)
	{
	  perror("hci_devinfo");
	  return BDREMOTE_FAIL;
	}

      assert(_capturedata->bt_dev_address == NULL);
      _capturedata->bt_dev_address = (char*)malloc(127);
      memset(_capturedata->bt_dev_address, 0, 127);
      ret = ba2str(&devinfo.bdaddr, _capturedata->bt_dev_address);

      if (ret < 0)
	{
	  perror("ba2str");
	  return BDREMOTE_FAIL;
	}
      assert(_capturedata->bt_dev_address != NULL);
    }
  else
    {
      /* Address was set using command line argument. */
      assert(_capturedata->bt_dev_address != NULL);
    }

  ret = str2ba(_capturedata->bt_dev_address, &bdaddr);
  if (ret < 0)
    {
      perror("str2ba");
      return BDREMOTE_FAIL;
    }

  ret = bt_device_exists(&bdaddr);
  if (ret == BDREMOTE_FAIL)
    {
      BDREMOTE_LOG(_capturedata->config->debug,
		   fprintf(printStream, "Unable to use interface address: %s.\n", _capturedata->bt_dev_address);
		   );

      return BDREMOTE_FAIL;
    }

  BDREMOTE_LOG(_capturedata->config->debug,
               fprintf(printStream, "Using BT address: %s.\n", _capturedata->bt_dev_address);
               );

  ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HIDP);
  if (ctl < 0)
    {
      perror("Can't open HIDP control socket");
      return BDREMOTE_FAIL;
    }

  csk = l2cap_listen(&bdaddr, L2CAP_PSM_HIDP_CTRL, lm, 10);
  if (csk < 0)
    {
      perror("Can't listen on HID control channel");
      close(ctl);
      return BDREMOTE_FAIL;
    }

  isk = l2cap_listen(&bdaddr, L2CAP_PSM_HIDP_INTR, lm, 10);
  if (isk < 0)
    {
      perror("Can't listen on HID interrupt channel");
      close(ctl);
      close(csk);
      return BDREMOTE_FAIL;
    }

  _capturedata->sockets[0] = ctl;
  _capturedata->sockets[1] = csk;
  _capturedata->sockets[2] = isk;

  return BDREMOTE_OK;
}

int captureLoop(captureData* _capturedata)
{
  int ctl = _capturedata->sockets[0];
  int csk = _capturedata->sockets[1];
  int isk = _capturedata->sockets[2];

  run_server(_capturedata, csk, isk);

  close(csk);
  close(isk);
  close(ctl);

  return BDREMOTE_OK;
}

/*@}*/

