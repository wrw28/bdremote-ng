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

#ifndef BD_CAPTUREIF_H
#define BD_CAPTUREIF_H

#include <globaldefs.h>
#include <bdrcfg.h>

/* Struct given to the function which is supposed to capture data from
 * a BT interface.
 */
typedef struct
{
#if BDREMOTE_DEBUG
   int magic0;
#endif /* BDREMOTE_DEBUG */

   /* Configuration. */
   const configuration* config;
   /* Context pointer. */
   void* p;
   /* Device address. */
   char* bt_dev_address;
   /* BT Address of the remote sending keypresses. */
   char* dest_address;
   /* Timeout in seconds. */
   int timeout;
   /* Sockets. */
   int sockets[3];
} captureData;

/* Function used to init the data used by this interface.
 * p - context pointer.
 */
void InitCaptureData(captureData* _cd,
		     const configuration* _config,
		     void* _p,
		     const char* _dest_address,
		     const int _timeout);

/* Release any data used by this interface. */
void DestroyCaptureData(captureData* _cd);

/*
 * Callbacks.
 */

/* A remote was connected. */
void RemoteConnected(void* _p);

/* Remote sent some data.
 * p - context pointer.
 */
void DataInd(void* p, const char* _data, const int _size);

/* Remote disconnected. */
void RemoteDisconnected(void* _p);

/* Setup the data needed to start capturing.
 * Called before captureLoop(..), after which the daemon
 * will change UID:GID.
 */
int InitcaptureLoop(captureData* _capturedata);

/* Main capture loop.
 * 
 * The idea is to run this as a thread and call the above functions
 * when a change is detected.
 * 
 * returns: -1 on error.
 */
int captureLoop(captureData* _capturedata);

#endif /* BD_CAPTUREIF_H */

