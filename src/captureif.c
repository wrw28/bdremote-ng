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

void InitCaptureData(captureData* cd,
		     void* _p,
		     const char* _dest_address,
		     const int _timeout )
{
#if BDREMOTE_DEBUG
  cd->magic0 = 127;
#endif // BDREMOTE_DEBUG
  assert(cd != NULL);
  assert(_p != NULL);
  assert(_dest_address != NULL);
  assert(_timeout > 0);

  cd->p = _p;
  cd->bt_dev_address = NULL;

  cd->dest_address   = malloc(strlen(_dest_address)+1);
  strcpy(cd->dest_address, _dest_address);

  cd->timeout = _timeout;
}
