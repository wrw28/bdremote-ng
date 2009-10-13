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

/** \ingroup LIRC */
/*@{*/

/*! \file lirc_srv.c
  \brief Implements a LIRC server.
  
  This file implements a LIRC server. This means listening for a
  socket for connections from LIRC and accepting new clients. Added
  client are broadcast keypresses in other part of this application -
  see lirc_thr.c.

  The lirc_server() function is the one to examine in order to
  understand how this module works.

*/

#include "lirc_srv.h"

#include <globaldefs.h>

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
#include <poll.h>
#include <netinet/in.h>
#include <assert.h>

/** Indicates that a TERM signal was received which interrupted IO. */
volatile sig_atomic_t __io_canceled = 0;

void add_client(lirc_data* _lircdata);
int create_listener(configuration* _config, lirc_data* _lircdata);

void initLircData(lirc_data* _ld, const configuration* _config)
{
  assert(_config != NULL);
  assert(
         pthread_mutex_init (&_ld->dataMutex, NULL) == 0
         );

#if BDREMOTE_DEBUG
  _ld->magic0 = 0x15;
#endif /* BDREMOTE_DEBUG */
  _ld->config   = _config;
  _ld->sockinet = -1;

  pthread_mutex_lock (&_ld->dataMutex);

  memset(&_ld->clis[0], 0, MAX_CLIENTS);
  _ld->clin = 0;

  pthread_mutex_unlock (&_ld->dataMutex);

  assert(queueInit(&_ld->qu) == Q_OK);
}

void destroyLircData(lirc_data* _ld)
{
  queueDeinit(&_ld->qu);

  _ld->config = NULL;

  assert(_ld->clin == 0);
  assert(_ld->sockinet == BDREMOTE_FAIL);

  pthread_mutex_destroy (&_ld->dataMutex);
}

int lirc_server(configuration* _config, lirc_data* _lircdata)
{
  struct pollfd p;
  sigset_t sigs;
  int i = 0;

  if (create_listener(_config, _lircdata) == BDREMOTE_FAIL)
    {
      return BDREMOTE_FAIL;
    }

  sigfillset(&sigs);
  sigdelset(&sigs, SIGCHLD);
  sigdelset(&sigs, SIGPIPE);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGHUP);

  p.fd = _lircdata->sockinet;
  p.events = POLLIN | POLLERR | POLLHUP;

  while (!__io_canceled)
    {
      p.revents = 0;
      if (ppoll(&p, 1, NULL, &sigs) < 1)
        {
          continue;
        }
      if (p.events & POLLIN)
        {
          BDREMOTE_DBG(_config->debug, "new client accepted.");
          add_client(_lircdata);
        }
    }

  pthread_mutex_lock (&_lircdata->dataMutex);

  /* Close all client sockets. */
  for(i=0;i<_lircdata->clin;i++)
    {
      shutdown(_lircdata->clis[i],2);
      close(_lircdata->clis[i]);
    }

  _lircdata->clin = 0;

  pthread_mutex_unlock (&_lircdata->dataMutex);

  shutdown(_lircdata->sockinet,2);
  close(_lircdata->sockinet);

  _lircdata->sockinet = BDREMOTE_FAIL;

  return BDREMOTE_OK;
}

int create_listener(configuration* _config, lirc_data* _lircdata)
{
  int enable=1;
  unsigned short int port=_config->listen_port;
  struct sockaddr_in serv_addr_in;
  /* create socket*/
  _lircdata->sockinet=socket(PF_INET,SOCK_STREAM,IPPROTO_IP);
  if (_lircdata->sockinet==-1)
    {
      perror("socket");
      BDREMOTE_DBG(_config->debug, "Could not create TCP/IP socket.");
      return BDREMOTE_FAIL;
    }
  (void) setsockopt(_lircdata->sockinet,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof(enable));
  serv_addr_in.sin_family      = AF_INET;
  serv_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr_in.sin_port        = htons(port);

  if (bind(_lircdata->sockinet,(struct sockaddr *) &serv_addr_in,
           sizeof(serv_addr_in))==-1)
    {
      BDREMOTE_DBG(_config->debug, "Could not assign address to socket\n");
      perror("bind");
      close(_lircdata->sockinet);
      return BDREMOTE_FAIL;
    }

  listen(_lircdata->sockinet,3);
  nolinger(_lircdata->sockinet);

  return BDREMOTE_OK;
}

void add_client(lirc_data* _lircdata)
{
  struct sockaddr client_addr;
  socklen_t clilen = sizeof(client_addr);
  int flags  = 0;
  int fd     = accept(_lircdata->sockinet,(struct sockaddr *)&client_addr, &clilen);

  if (fd==-1)
    {
      BDREMOTE_DBG(_lircdata->config->debug,
                   "accept() failed for new client.");
      perror("accept");
    };

  if(fd>=FD_SETSIZE || _lircdata->clin>=MAX_CLIENTS)
    {
      BDREMOTE_DBG(_lircdata->config->debug,
                   "Connection rejected.");
      shutdown(fd,2);
      close(fd);
      return;
    }
  nolinger(fd);

  flags=fcntl(fd, F_GETFL, 0);
  if(flags!=-1)
    {
      fcntl(fd,F_SETFL,flags|O_NONBLOCK);
    }
  pthread_mutex_lock (&_lircdata->dataMutex);
  _lircdata->clis[_lircdata->clin++]=fd;
  pthread_mutex_unlock (&_lircdata->dataMutex);
}

/* Only called from LIRC thread, mutex already locked. */
void remove_client(lirc_data* _lircdata, int fd)
{
  int i;
  for(i=0;i<_lircdata->clin;i++)
    {
      if(_lircdata->clis[i] == fd)
        {
          shutdown(_lircdata->clis[i],2);
          close(_lircdata->clis[i]);

          _lircdata->clin--;

          for(;i<_lircdata->clin;i++)
            {
              _lircdata->clis[i]=_lircdata->clis[i+1];
            }
          return;
        }
    }
}

/*@}*/

