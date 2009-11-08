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

/** \ingroup Gen */
/*@{*/

/*! \file q.c
  \brief Thread safe queue implementation.

  Implements a thread safe queue used for communication between
  bluetooth thread and LIRC thread.

*/

#include "q.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

/** Set to 1 to enabled some output using printf. Not recomended,
    unless you really mean to see a lot of debugging output. */
#define Q_DBG 0

static const unsigned int moduleMask = MODULEMASK_QUEUE;

queueData* queueDataInit(const char* _s, int _size)
{
  queueData* qd = (queueData*)malloc(sizeof(queueData));

  assert(_s != NULL);
  assert(_size >= 0);
  qd->buffer = (char*)malloc(_size);
  qd->size   = _size;
  memcpy(qd->buffer, _s, _size);

  return qd;
}

void queueDataDeInit(queueData* _qd)
{
  assert(_qd != NULL);
  assert(_qd->buffer != NULL);
  assert(_qd->size > 0);

  free(_qd->buffer);
  _qd->buffer = 0;
  _qd->size   = 0;
  free(_qd);
  _qd = 0;
}

int queueInit (queue* _q)
{
  assert(_q != NULL);
  if (_q == NULL)
    {
      return Q_ERR;
    }

  _q->empty = 1;
  _q->full = 0;
  _q->head = 0;
  _q->tail = 0;
  _q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (_q->mut, NULL);
  _q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (_q->notFull, NULL);
  _q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (_q->notEmpty, NULL);

  return Q_OK;
}

void queueDeinit (queue* _q)
{
  assert(_q != NULL);

  pthread_mutex_destroy (_q->mut);
  free (_q->mut);
  pthread_cond_destroy (_q->notFull);
  free (_q->notFull);
  pthread_cond_destroy (_q->notEmpty);
  free (_q->notEmpty);
}

void queueAdd (queue* _q, queueData* _in)
{
  assert(_q != NULL);
  assert(_in != NULL);

  pthread_mutex_lock (_q->mut);

  while (_q->full)
    {
#if BDREMOTE_DEBUG
#if Q_DBG
      printf ("Add: queue FULL.\n");
#endif
#endif
      pthread_cond_wait (_q->notFull, _q->mut);
    }

#if BDREMOTE_DEBUG
#if Q_DBG
  printf ("Add: %d.\n", _in->size);
#endif
#endif

  _q->buf[_q->tail] = _in;
  _q->tail++;
  if (_q->tail == QUEUESIZE)
    {
      _q->tail = 0;
    }
  if (_q->tail == _q->head)
    {
      _q->full = 1;
    }
  _q->empty = 0;

  pthread_mutex_unlock (_q->mut);
  pthread_cond_signal (_q->notEmpty);
}

void queueRemBlock(queue* _q, queueData** _out)
{
  assert(_q != NULL);
  assert(*_out == NULL);

  pthread_mutex_lock (_q->mut);

  while (_q->empty)
    {
#if BDREMOTE_DEBUG
#if Q_DBG
      printf ("Rem: queue EMPTY.\n");
#endif
#endif
      pthread_cond_wait (_q->notEmpty, _q->mut);
    }

#if BDREMOTE_DEBUG
#if Q_DBG
      printf ("Rem: done waiting for cond.\n");
#endif
#endif

      *_out = _q->buf[_q->head];

      _q->head++;
      if (_q->head == QUEUESIZE)
	{
	  _q->head = 0;
	}
      if (_q->head == _q->tail)
	{
	  _q->empty = 1;
	}
      _q->full = 0;

      pthread_mutex_unlock (_q->mut);
      pthread_cond_signal (_q->notFull);
}

int queueRemNonBlock(queue* _q, int _n, queueData** _out)
{
  struct timeval tp;
  struct timespec abstime;
  int i = 1;

  assert(_q != NULL);
  assert(*_out == NULL);

  pthread_mutex_lock (_q->mut);

  while (_q->empty)
    {
      gettimeofday(&tp, NULL);
      abstime.tv_sec  = tp.tv_sec;
      abstime.tv_nsec = tp.tv_usec * 1000;
      /* Wait for _n ms. */

      abstime.tv_nsec += (_n * 1000000); /* milliseconds */
      assert(abstime.tv_nsec >= 0);
      // abstime.tv_nsec += 50 000000;/* 50 milliseconds */

      // printf("WTF!? %ld.\n", abstime.tv_nsec);

      if (pthread_cond_timedwait(_q->notEmpty, _q->mut, &abstime) == ETIMEDOUT)
	{
	  /* Timeout. */
	  i = 0;
	  break;
	}
    }

  if (i == 0)
    {
      pthread_mutex_unlock (_q->mut);
      return Q_ERR;
    }

  *_out = _q->buf[_q->head];

  _q->head++;
  if (_q->head == QUEUESIZE)
    {
      _q->head = 0;
    }
  if (_q->head == _q->tail)
    {
      _q->empty = 1;
    }
  _q->full = 0;

  pthread_mutex_unlock (_q->mut);
  pthread_cond_signal (_q->notFull);

  return Q_OK;
}

/*@}*/

