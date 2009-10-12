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

void initTime(keyState* _ks)
{
  _ks->cl0     = clock();
  _ks->cl1     = _ks->cl0;
  _ks->elapsed = 0;
}

void updateTime(keyState* _ks)
{
  clock_t temp = 0;
  clock_t e    = 0;
  _ks->cl1     = clock();
  assert(_ks->cl1 >= _ks->cl0);
  temp = ((_ks->cl1 - _ks->cl0) * 1000);
  e = (temp / CLOCKS_PER_SEC);
  _ks->elapsed += e;

  _ks->cl0 = clock();
}

int write_socket(int _fd, const char* _buf, int _len)
{
  int done = 0;
  int todo = _len;
  
  while (todo)
    {
       done = write(_fd, _buf, todo);

      if (done<=0)
         {
            return done;
         }
      _buf  += done;
      todo  -= done;
    }

  return _len;
}

void nolinger(int _sock)
{
  static struct linger  linger = {0, 0};
  int lsize  = sizeof(struct linger);
  setsockopt(_sock, SOL_SOCKET, SO_LINGER, (void *)&linger, lsize);
}

