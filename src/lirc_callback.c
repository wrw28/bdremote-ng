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

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

void RemoteConnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote connected.");
}

/* Received some data from the ps3 remote. 
 * Forward it to LIRC clients.
 * Note: no threads are used, so no need for locking.
 */
void DataInd(void* _p, const char* _data, const int _size)
{
  lirc_data* lc = (lirc_data*)_p;
#if BDREMOTE_DEBUG
  assert(lc->magic0 == 0x15);
#endif /* BDREMOTE_DEBUG */

  queueData* qd = queueDataInit(_data, _size);
  queueAdd(&lc->qu, qd);
}

void RemoteDisconnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote disconnected.");
}

	
