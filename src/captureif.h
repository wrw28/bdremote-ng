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

#ifndef _CAPTUREIF_H
#define _CAPTUREIF_H

#include <globaldefs.h>

// Struct given to the function which is supposed to capture data from
// a BT interface.
typedef struct
{
#if BDREMOTE_DEBUG
  int magic0;
#endif // BDREMOTE_DEBUG

  // Context pointer.
  void* p;
  // Device address.
  char* bt_dev_address;
  /// BT Address of the remote sending keypresses.
  char* dest_address;
  // Timeout in seconds.
  int timeout;
} captureData;

// Function used to init the data used by this interface.
// p - context pointer.
void InitCaptureData(captureData* cd,
		     void* _p,
		     const char* _dest_address,
		     const int _timeout);

//
// Callbacks.
//

// A remote was connected.
void RemoteConnected();

// Remote sent some data.
// p - context pointer.
void DataInd(void* p, const char* _data, const int _size);

// Remote disconnected.
void RemoteDisconnected();

// Main capture loop.
// 
// The idea is to run this as a thread and call the above functions
// when a change is detected.
// 
// returns: -1 on error.
int captureLoop(captureData* _capturedata);

#endif // _CAPTUREIF_H

