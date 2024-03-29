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

/** \ingroup Test */
/*\@{*/

/*! \file capture_test.c
  \brief Test bluetooth capture.

  Test application which should print something when the remote
  connects and sends data.
  Notice the hardcoded destination address.
*/

#include <globaldefs.h>
#include <captureif.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <l.h>

unsigned int globalLogMask = 
  MODULEMASK_LIRC_THR | MODULEMASK_LIRC_SOCK | 
  MODULEMASK_LIRC_CB | MODULEMASK_BT_IF | MODULEMASK_BT_IMPL | 
  MODULEMASK_QUEUE | MODULEMASK_SPARE | MODULEMASK_MAIN;

static const unsigned int moduleMask = MODULEMASK_MAIN;

/** Capture interface implementation. */
void RemoteConnected(void* _p)
{
  assert(_p);
  printf("Remote connected.\n");
}

/** Capture interface implementation. */
void DataInd(void* _p, const char* _data, const int _size)
{
  assert(_p);
  assert(_data);
  printf("Remote data indication, %d bytes.\n", _size);
}

/** Capture interface implementation. */
void RemoteDisconnected(void* _p)
{
  assert(_p);
  printf("Remote disconnected.\n");
}

void RemoteBatteryCharge(void* _p, int _val)
{
  assert(_p);
  printf("Battery charge %d %%.\n", _val);
}

/** Capture test. */
int main(int argc, char *argv[])
{
  /* Address of the remote. */
  const char* destinationAddress = "00:19:C1:58:C3:B7";
  configuration config;
  captureData cd;
  void* p = (void*)0x1; /* Unused here. */
  int res = BDREMOTE_FAIL;
  int i   = 0;

  if (argc > 1)
    {
      printf("Arguments are not supported.\n");
      
      for (i = 1; i < argc; i++)
	{
	  printf("Unhandled argument: %s.\n", argv[i]);
	}
      return -1;
    }
  
  setDefaultLog();
  memset(&config, 0, sizeof(config));
  memset(&cd, 0, sizeof(cd));

  setDefaults(&config);
  setRemoteAddress(&config, destinationAddress);
 
  InitCaptureData(&cd,
                  &config,
                  p);
  
  res = InitcaptureLoop(&cd);
  if (res == BDREMOTE_FAIL)
    {
      BDREMOTE_DBG(config.debug, "InitcaptureLoop failed.");
      return BDREMOTE_FAIL;
    }

  /* Run capture loop. */
  res = captureLoop(&cd);

  if (res == BDREMOTE_FAIL)
    {
      BDREMOTE_ERR("captureLoop failed.");
      return BDREMOTE_FAIL;
    }

  return BDREMOTE_OK;
}

/*\@}*/

