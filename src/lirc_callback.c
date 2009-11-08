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

/*! \file lirc_callback.c
  \brief Capture device, callback implementation.

  This file implements the callback mechanism specified by the capture
  interface. Look in the captureif.h header file.

*/

#include "lirc_srv.h"
#include <globaldefs.h>

#include <stdint.h>
#include <stdio.h>
#include <assert.h>

static const unsigned int moduleMask = MODULEMASK_LIRC_CB;

void RemoteConnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote connected.");
}

/* Received some data from the ps3 remote.
 * Forward it to LIRC clients.
 */
void DataInd(void* _p, const char* _data, const int _size)
{
  lirc_data* lc = (lirc_data*)_p;
  queueData* qd = 0;
#if BDREMOTE_DEBUG
  assert(lc->magic0 == 0x15);
#endif /* BDREMOTE_DEBUG */

  qd = queueDataInit(_data, _size);
  queueAdd(&lc->qu, qd);
}

void RemoteBatteryCharge(void* _p, int _val)
{
  lirc_data* lc = (lirc_data*)_p;

  if (lc->charge_percent_set)
    {
      if (lc->charge_percent != _val)
	{
	  fprintf(printStream, "Battery charge changed, from %d %% to %d %%.\n", lc->charge_percent, _val);
	  lc->charge_percent = _val;
	}
    }
  else
    {
      lc->charge_percent     = _val;
      lc->charge_percent_set = 1;

      fprintf(printStream, "Battery charge %d %%.\n", lc->charge_percent);
    }

}

void RemoteDisconnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote disconnected.");
}

/*@}*/

