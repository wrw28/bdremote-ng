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

#ifndef BD_LIRC_SRV_H
#define BD_LIRC_SRV_H

#include <globaldefs.h>

#include <bdrcfg.h>
#include <stdint.h>

#include <pthread.h>
#include <q.h>

/** The number of LIRC clients that can connect at the same time. */
#define MAX_CLIENTS 16

/** Information used by the LIRC server. */
typedef struct
{
#if BDREMOTE_DEBUG
   /** Magic value, used for asserting. */
   int magic0;
#endif /* BDREMOTE_DEBUG */
   /** Configuration. */
   const configuration* config;
   /** Socked used to accept new LIRC clients. */
   int sockinet;
   
   /** Client sockets. */
   int clis[MAX_CLIENTS]; 
   /** Number of client sockets. */
   int clin;
   
   /* State information. */

   /** Last state. */
   int laststate;

   /** Last code. */
   unsigned char lastcode;

   /** Last mask. */
   uint32_t lastmask;

   /** Last received key from the remote. */
   int lastkey;

   /** Last sent key. */
   int lastsend;
  
  /** Mutex used. */
  pthread_mutexattr_t mutex;

  /** Queue used to communicate BT events to lirc clients.
   * Thread safe.
   */
  queue qu;

  pthread_t thread;

} lirc_data;

/** Init data used by the LIRC server part of this application. */
void initLircData(lirc_data* _ld, const configuration* _config);

void startLircThread(lirc_data* _ld, const configuration* _config);

void waitForLircThread(lirc_data* _ld);

/** Run a LIRC server. */
int lirc_server(configuration* _config, lirc_data* _lircdata);

/** Broadcast a message to all connected LIRC sockets. */
void broadcast_message(lirc_data* _lircdata, const char* _message);

/** Destroy LIRC data used by the LIRC server part of this application. */
void destroyLircData(lirc_data* _ld);

#endif /* BD_LIRC_SRV_H */


