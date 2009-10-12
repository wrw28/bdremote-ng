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

/** @defgroup LIRC LIRC

 *  This group contains a function used to change the UID/GID of the
 *  application to one with less priviledges.

 *  @{
 */

/*! \file lirc_srv.h
    \brief LIRC server.

    This file contains a LIRC server. It's purpose is to translate
    bluetooth events received using a queue from the bluetooth
    interface into key press events which are understood by LIRC.

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
  
  /** Mutex used. */
  pthread_mutex_t dataMutex;
  
  /** Queue used to communicate BT events to lirc clients.
   * Thread safe.
   */
  queue qu;
  
  /** Thread used to receive bluetooth events (keypresses) and
      sending them to LIRC in a format it understands.. */
  pthread_t thread;

} lirc_data;

/** Init data used by the LIRC server part of this application. */
void initLircData(lirc_data* _ld, const configuration* _config);

/** Start a thread used to receive bluetooth events (keypresses) and
    sending them to LIRC in a format it understands. */
void startLircThread(lirc_data* _ld);

/** Wait for the LIRC thread to terminate. */
void waitForLircThread(lirc_data* _ld);

/** Run a LIRC server. */
int lirc_server(configuration* _config, lirc_data* _lircdata);

/** Broadcast a message to all connected LIRC sockets. */
void broadcast_message(lirc_data* _lircdata, const char* _message);

/** Destroy LIRC data used by the LIRC server part of this application. */
void destroyLircData(lirc_data* _ld);

/** Write a message to a socket. */
int write_socket(int _fd, const char* _buf, int _len);

/** Close sockets gracefully. */
void nolinger(int sock);

/** Remote a LIRC client from the list of clients receiving our messages. */
void remove_client(lirc_data* _lircdata, int fd);

/** Struct used to keep track of pressed key and repeat state. */
typedef struct
{
  /** Indicates if a key is down. */
  int keyDown;
  /** Last sent key. */
  int lastKey;
  /** Clock used to keep time. */
  clock_t cl0;
  /** Clock used to keep time. */
  clock_t cl1;

  /** Number of elapsed miliseconds. */
  unsigned long elapsed;

  /** Last number of elapsed miliseconds. */
  unsigned long elapsed_last;

  /** Number of repeat keys already sent.*/
  int repeat_sent;

  /** Repeat counter. */
  int repeat_count;

} keyState;

/** Start keeping track of time. */
void initTime(keyState* _ks);

/** Update time counter. To avoid overflows, reset the counter
    often. */
void updateTime(keyState* _ks);

#endif /* BD_LIRC_SRV_H */


