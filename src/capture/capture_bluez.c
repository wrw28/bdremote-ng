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
#include <bluetooth/sdp.h>
#include <bluetooth/hidp.h>

#include <poll.h>

#include <globaldefs.h>
#include <captureif.h>

#include <assert.h>

#define L2CAP_PSM_HIDP_CTRL 0x11
#define L2CAP_PSM_HIDP_INTR 0x13

extern volatile sig_atomic_t __io_canceled;

int l2cap_checkSource(int sock, bdaddr_t* address)
{
  struct sockaddr_l2 addr;
  socklen_t addrlen = sizeof(addr);

  if (getpeername(sock, (struct sockaddr *) &addr, &addrlen) < 0)
    {
      perror("getpeername");
      return -1;
    }

  assert(sizeof(addr.l2_bdaddr) == sizeof(*address));

#if 0
  int i = 0;
  uint8_t* b0 = addr.l2_bdaddr.b;
  uint8_t* b1 = address;

  for (i = 0; i < sizeof(addr.l2_bdaddr); i++)
    {
      printf("byte %d, %d:%d\n", i, (int)b0, (int)b1);
    }
#endif

  if (memcmp(&addr.l2_bdaddr, address, sizeof(*address)) == 0)
    {
      BDREMOTE_DBG("Known device.");
      return 1;
    }

  BDREMOTE_DBG("Unknown device.");
  return 0;
}

int l2cap_connect(bdaddr_t *src, bdaddr_t *dst, unsigned short psm)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.l2_family  = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, src);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
	  {
	    close(sk);
	    return -1;
	  }

	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	memset(&addr, 0, sizeof(addr));
	addr.l2_family  = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, dst);
	addr.l2_psm = htobs(psm);

	if (connect(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(sk);
		return -1;
	}

	return sk;
}

int l2cap_listen(const bdaddr_t *bdaddr, unsigned short psm, int lm, int backlog)
{
	struct sockaddr_l2 addr;
	struct l2cap_options opts;
	int sk;

	if ((sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
	  {
	    BDREMOTE_DBG("Unable to create socket.\n");
	    return -1;
	  }

	memset(&addr, 0, sizeof(addr));
	addr.l2_family = AF_BLUETOOTH;
	bacpy(&addr.l2_bdaddr, bdaddr);
	addr.l2_psm = htobs(psm);

	if (bind(sk, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
	  {
	    perror("bind");
	    close(sk);
	    return -1;
	}

	setsockopt(sk, SOL_L2CAP, L2CAP_LM, &lm, sizeof(lm));

	memset(&opts, 0, sizeof(opts));
	opts.imtu = HIDP_DEFAULT_MTU;
	opts.omtu = HIDP_DEFAULT_MTU;
	opts.flush_to = 0xffff;

	setsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts));

	if (listen(sk, backlog) < 0) 
	  {
	    BDREMOTE_DBG("Listen failed.\n");
	    close(sk);
	    return -1;
	  }

	return sk;
}

int l2cap_accept(int sk, bdaddr_t *bdaddr)
{
	struct sockaddr_l2 addr;
	socklen_t addrlen;
	int nsk;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);

	if ((nsk = accept(sk, (struct sockaddr *) &addr, &addrlen)) < 0)
	  {
	    return -1;
	  }

	if (bdaddr)
	  {
	    bacpy(bdaddr, &addr.l2_bdaddr);
	  }
	return nsk;
}

void run_server(captureData* _capturedata,
		int ctl, int csk, int isk)
{
	struct pollfd p[2];
	sigset_t sigs;
	short events;
	//int err  = -1;
	int ncsk = -1;
	int nisk = -1;

	bdaddr_t destinationAddress;
	if (str2ba(_capturedata->dest_address, &destinationAddress) < 0)
	  {
	    printf("str2ba call failed. Input was '%s'.", _capturedata->bt_dev_address);
	    return;
	  }

	BDREMOTE_DBG("Using destination address:");
	BDREMOTE_DBG(_capturedata->dest_address);

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
	    
	    struct timespec timeout;
	    timeout.tv_sec = 1;
	    timeout.tv_nsec = 0;
	    
	    if (ppoll(p, 2, &timeout, &sigs) < 1)
	      {
		BDREMOTE_DBG("No data captured ..");
		continue;
	      }
	    
	    events = p[0].revents | p[1].revents;
	    
	    if (events & POLLIN) {
	      ncsk = l2cap_accept(csk, NULL);
	      nisk = l2cap_accept(isk, NULL);
	      
	      BDREMOTE_DBG("Accepted client.");
	      
	      if (l2cap_checkSource(ncsk, &destinationAddress))
		{
		  RemoteConnected();
		  
		  int cont = 1;
		  int recv_len = 0;
		  int bufferSize = 1024;
		  char* buffer = malloc(bufferSize);
		  
		  while (cont && (!__io_canceled))
		    {
		      recv_len=recv(nisk, &buffer[0], bufferSize, 0);
		      
		      if (recv_len<=0)
			{
			  cont = 0;
			  BDREMOTE_DBG("Client disconnected.");
			}
		      else
			{
			  BDREMOTE_DBG("Calling DataInd.");
#if BDREMOTE_DEBUG
			  assert(_capturedata->magic0 == 127);
#endif // BDREMOTE_DEBUG
			  DataInd(_capturedata->p, &buffer[0], recv_len);
			}
		      
		    }
		  RemoteDisconnected();
		}
	      
	      
	      
	      close(nisk);
	      sleep(1);
	      close(ncsk);
	    }
	  }
}

int captureLoop(captureData* _capturedata)
{
  bdaddr_t bdaddr;
  int ctl, csk, isk;
  int lm = 0;

  // Find the first interface, using BT primitives.
  struct hci_dev_info devinfo;
  int ret = hci_devinfo(0, &devinfo);

  if (ret < 0)
    {
      perror("hci_devinfo");
      return BDREMOTE_FAIL;
    }

  _capturedata->bt_dev_address = (char*)malloc(127);
  memset(_capturedata->bt_dev_address, 0, 127);
  ret = ba2str(&devinfo.bdaddr, _capturedata->bt_dev_address);

  if (ret < 0)
    {
      perror("ba2str");
      return BDREMOTE_FAIL;
    }

  assert(_capturedata->bt_dev_address != NULL);
  str2ba(_capturedata->bt_dev_address, &bdaddr);
#if BDREMOTE_DEBUG
  printf("Using BT address: %s.\n", _capturedata->bt_dev_address);
#endif

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

  run_server(_capturedata, ctl, csk, isk);

  close(csk);
  close(isk);
  close(ctl);

  return BDREMOTE_OK;
}
