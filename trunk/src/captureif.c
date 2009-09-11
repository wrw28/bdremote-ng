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

#include "captureif.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void InitCaptureData(captureData* _cd,
		     const configuration* _config,
		     void* _p,
		     const char* _dest_address,
		     const int _timeout )
{
#if BDREMOTE_DEBUG
  _cd->magic0 = 127;
#endif // BDREMOTE_DEBUG
  assert(_config != NULL);
  assert(_cd != NULL);
  assert(_p != NULL);
  assert(_dest_address != NULL);
  assert(_timeout > 0);

  _cd->config = _config;
  _cd->p = _p;
  _cd->bt_dev_address = NULL;

  const int addrLen = strlen(_dest_address)+1;
  _cd->dest_address   = (char*)malloc(addrLen);
  memset(&_cd->dest_address[0], 0, addrLen);

  strcpy(_cd->dest_address, _dest_address);

  _cd->timeout = _timeout;
  memset(&_cd->sockets[0], 0, 2);
}

void DestroyCaptureData(captureData* _cd)
{
  _cd->p = NULL;

  free(_cd->bt_dev_address);
  _cd->bt_dev_address = NULL;

  free(_cd->dest_address);
  _cd->dest_address = NULL;
}
