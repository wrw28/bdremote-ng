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

/** @defgroup Test Tests
 *  This group contains tests and test cases.
 *  @{
 */

/*! \file qtest.c
  \brief Test queue.

  Test application which tests if the queue used to send messages
  between the BT thread and the LIRC thread is working as expected.
*/

#include <q.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#define LOOP 20

void* producer (void *args);
void* consumer (void *args);

unsigned int globalLogMask = 
  MODULEMASK_LIRC_THR | MODULEMASK_LIRC_SOCK | 
  MODULEMASK_LIRC_CB | MODULEMASK_BT_IF | MODULEMASK_BT_IMPL | 
  MODULEMASK_QUEUE | MODULEMASK_SPARE | MODULEMASK_MAIN;


static const unsigned int moduleMask = MODULEMASK_MAIN;

int main(int argc, char *argv[])
{
  queue fifo;
  pthread_t pro, con;
  
  int res = queueInit(&fifo);
  if (res ==  Q_ERR)
    {
      fprintf (stderr, "main: Queue Init failed.\n");
      exit(1);
    }

  pthread_create (&pro, NULL, producer, &fifo);
  pthread_create (&con, NULL, consumer, &fifo);
  pthread_join (pro, NULL);
  pthread_join (con, NULL);
  queueDeinit (&fifo);

  return 0;
}

void* producer (void* q)
{
  queue *fifo = (queue *)q;
  int i;
  char msg[100];
  queueData* qd = NULL;

  for (i = 0; i < LOOP; i++)
    {
      sprintf(msg, "Test 1: %d", i);
      qd = queueDataInit(msg, strlen(msg)+1);
      queueAdd (fifo, qd);
      printf ("producer: added %s.\n", msg);
      usleep (100000);
    }
  for (i = 0; i < LOOP; i++)
    {
      sprintf(msg, "Test 2: %d", i);
      qd = queueDataInit(msg, strlen(msg)+1);
      queueAdd (fifo, qd);
      printf ("producer: added %s.\n", msg);
      usleep (200000);
    }
  return (NULL);
}

void* consumer (void* q)
{
  queue* fifo = (queue *)q;
  int i;
  queueData* d = 0;
  
  for (i = 0; i < LOOP; i++)
    {
      queueRemBlock(fifo, &d);
      printf ("consumer: recieved %s.\n", d->buffer);
      queueDataDeInit(d);
      d = NULL;
      usleep(200000);
    }
  for (i = 0; i < LOOP; i++)
    {
      queueRemBlock(fifo, &d);
      printf ("consumer: recieved %s.\n", d->buffer);
      queueDataDeInit(d);
      d = NULL;
      usleep (50000);
    }
  return (NULL);
}

/*\@}*/

