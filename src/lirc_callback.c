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
#include <unistd.h>
#include <stdlib.h>

static const unsigned int moduleMask = MODULEMASK_LIRC_CB;

/** If enabled, call: <script> <prev> <now>, where <prev> is the
    previous charge in percent, and <now> is the current charge in
    percent. */
void CallBatteryChargeScript(const configuration* _config, int _prev, int _now);

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
               CallBatteryChargeScript(lc->config, lc->charge_percent, _val);
               lc->charge_percent = _val;
            }
      }
   else
      {
         lc->charge_percent     = _val;
         lc->charge_percent_set = 1;

         CallBatteryChargeScript(lc->config, _val, _val);
         fprintf(printStream, "Battery charge %d %%.\n", lc->charge_percent);
      }

}

void RemoteDisconnected(void* _p)
{
   lirc_data* lc = (lirc_data*)_p;

   BDREMOTE_DBG(lc->config->debug, "Remote disconnected.");
}

#define cmd_len 512

void CallBatteryChargeScript(const configuration* _config, int _prev, int _now)
{
   int result = -1;
   char cmd[cmd_len];

   if (!_config->battery_script_set)
      {
         return;
      }
   
   result = access(_config->battery_script, R_OK|X_OK);
   if (result != 0)
      {
         BDREMOTE_DBG(_config->debug, "Unable to call battery script: call to access() failed.")
         return;
      }

   result = snprintf(&cmd[0], cmd_len, "%s '%d' '%d'", _config->battery_script, _prev, _now);
   if (result <= 0)
      {
         BDREMOTE_DBG(_config->debug, "Unable to call battery script: call to sprintf() failed.")
         return;
      }

   result = system(cmd);
   if (result == -1)
      {
         BDREMOTE_DBG(_config->debug, "Unable to call battery script: call to system() failed.")
      }

   BDREMOTE_DBG(_config->debug, "Battery script called successfully.")
}

/*@}*/

