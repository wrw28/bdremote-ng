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

/* The following was based on some pthread examples written by Andrae
 * Muys.
 */

#ifndef BD_Q_H
#define BD_Q_H

#include <globaldefs.h>
#include <pthread.h>

/** Max queue size. */
#define QUEUESIZE 254

/** OK */
#define Q_OK   0

/** Error. */
#define Q_ERR -1

/** Data stored in this queue. */
typedef struct
{
  /** Data. */
  char* buffer;
  /** Data size. */
  int   size;
} queueData;

/** Queue. */
typedef struct 
{
  /** Array of pointers to the data stored. */
  queueData* buf[QUEUESIZE];
  /** Head of the queue. */
  long             head;
  /** Tail of the queue. */
  long             tail;
  /** Indicates if the queue is full. */
  int              full;
  /** Indicates if the queue is empty. */
  int              empty;
  /** Mutex. */
  pthread_mutex_t* mut;
  /** Condition. */
  pthread_cond_t*  notFull;
  /** Condition. */
  pthread_cond_t*  notEmpty;
} queue;

/** Allocate a struct used for keeping data in this queue. */
queueData* queueDataInit(const char* _s, int _size);

/** Deallocate the struct used for keeping data in this queue. */
void queueDataDeInit(queueData* _qd);

/** Init queue. */
int queueInit (queue* _q);

/** Deinit queue. */
void queueDeinit (queue* _q);

/** Add an element to the queue. Blocks if the queue is full. 
 * Notice that the pointer belongs to the function.
 */
void queueAdd (queue* _q, queueData* _in);

/** Remove an element from the queue. Returns Q_ERR if the queue is empty. 
 * Notice that the pointer should be disposed of by the caller.
*/
int queueRem (queue* _q, int _blockOnEmpty, queueData** _out);

#endif /* BD_Q_H */

