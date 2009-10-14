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

/*! \file lirc_thr.c
  \brief Thread used to handle messages from capture interface.

  This file implements a thread which is used to receive messages
  (trough a thread safe queue) from the capture interface. This thread
  parses the contents and, if configured to do so, handles repeat -
  that is holding down a key on the remote.

*/

#include "lirc_srv.h"

#include <globaldefs.h>

#include <stdint.h>
#include <keydef.h>
#include <stdio.h>
#include <assert.h>

#include <unistd.h>

#define _GNU_SOURCE
#include <signal.h>

extern FILE* printStream;
extern volatile sig_atomic_t __io_canceled;

/** Thread. */
void* lircThread (void* q);

/** Broadcast an event to LIRC. */
void broadcastToLirc(lirc_data* _lc, const char* _name, int _rep, unsigned int _code);

/** Handle a key press. */
void handleKey(lirc_data* _ld,
               const char* _data, const int _size,
               keyState* _ks);

/** Handle key down. */
void DataInd_keyDown(lirc_data* _lc,
                     unsigned int _code,
                     uint32_t _mask,
                     keyState* _ks);

/** Handle key up. */
void DataInd_keyUp(lirc_data* _lc,
                   unsigned int _code,
                   uint32_t _mask,
                   keyState* _ks);

void startLircThread(lirc_data* _ld)
{
  pthread_create(&_ld->thread, NULL, lircThread, _ld);
}

void waitForLircThread(lirc_data* _ld)
{
  pthread_join(_ld->thread, NULL);
}

/** LIRC thread. */
void* lircThread (void* q)
{
  lirc_data* ld = (lirc_data*)q;
  queueData* qd = NULL;
  int res       = Q_ERR;
  keyState ks;
#if ENABLE_REPEAT
  int rate_mod  = 0;
  unsigned long rate_delay = 100;
  if (ld->config->repeat_delay > 0)
    {
      rate_delay = (unsigned long)ld->config->repeat_delay;
    }

  if (ld->config->repeat_rate > 0)
    {
      rate_mod = (int) (1000 / ld->config->repeat_rate);
    }
  else
    {
      rate_mod = 100;
    }
#endif

#if ENABLE_REPEAT
#  if BDREMOTE_DEBUG
  BDREMOTE_LOG(ld->config->debug,
               fprintf(printStream, "Started LIRC thread:\n");
               fprintf(printStream, " - using repeat rate : %d.\n", ld->config->repeat_rate);
               fprintf(printStream, " - using repeat delay: %lu.\n", rate_delay);
               );
#  endif
#else
#  if BDREMOTE_DEBUG
  BDREMOTE_LOG(ld->config->debug,
               fprintf(printStream, "Started LIRC thread:\n");
               fprintf(printStream, " - repeat disabled.");
               );
#  endif
#endif

  ks.keyDown = 0;
  ks.lastKey = ps3remote_undef;

#if ENABLE_REPEAT
  initTime(&ks);
  ks.elapsed_last = 0;
  ks.repeat_sent  = 0;
  ks.repeat_count = 0;
#endif

  while (!__io_canceled)
    {
      qd  = NULL;
      res = queueRem (&ld->qu, 0 /* No blocking. */, &qd);

      if (res == Q_OK)
        {
          assert(qd->buffer != NULL);
          handleKey(ld, qd->buffer, qd->size, &ks);

          queueDataDeInit(qd);
#if ENABLE_REPEAT
          initTime(&ks);
          ks.elapsed_last = 0;
          ks.repeat_sent  = 0;
          ks.repeat_count = 0;
#endif
        }
#if ENABLE_REPEAT
      if (ks.keyDown == 1)
        {
          updateTime(&ks);

          if (ks.elapsed % rate_mod == 0)
            {
              if (ks.elapsed_last == ks.elapsed)
                {
                  usleep(10);
                  continue;
                }

              ks.elapsed_last = ks.elapsed;

	      if (ks.elapsed >= rate_delay)
                {
                  BDREMOTE_LOG(ld->config->debug,
                               fprintf(printStream, "Key is down (repeat): %lu\n", ks.elapsed);
                               );
                  broadcastToLirc(ld, ps3remote_keys[ks.lastKey].name, 0 /*ks.repeat_sent*/, ps3remote_keys[ks.lastKey].code);
                  /* broadcastToLirc(ld, ps3remote_keys[ks.lastKey].name, 0, 0xFF); */
                  ks.repeat_sent++;
                }
              ks.repeat_count++;
            }
        }
      else
        {
           usleep(100);
        }
#else
      if (ks.keyDown == 1)
         {
            usleep(10);
         }
      else
         {
            usleep(100);
         }
#endif
    }

  BDREMOTE_DBG(ld->config->debug, "Thread terminating ..");

  return (NULL);
}

/** Received some data from the ps3 remote.
 * Forward it to LIRC clients.
 * Note: no threads are used, so no need for locking.
 */
void handleKey(lirc_data* _ld,
               const char* _data, const int _size,
               keyState* _ks)
{
  int num                 = 0;
  const uint32_t* mask_in = (uint32_t *)(_data+2);
  uint32_t mask           = 0;
  const unsigned char* magic = (const unsigned char*)_data;
  const unsigned char* code  = (const unsigned char*)_data+5;
  const unsigned char* state = (const unsigned char*)_data+11;

#if BDREMOTE_DEBUG
  assert(_ld->magic0 == 0x15);
#endif /* BDREMOTE_DEBUG */

  if ((_size==13) && (*magic==0xa1))
    {
      mask=(*mask_in) & 0xFFFFFF;

      num=-1;

      switch (*state)
        {
        case 1:
          {
            DataInd_keyDown(_ld, *code, mask, _ks);
            break;
          }
        case 0:
          {
            DataInd_keyUp(_ld, *code, mask, _ks);
            break;
          }
        }
    }
}

/** Convert a code received from the remote to an index into an array
    of remote keys. */
int codeToIndex(unsigned int _code)
{
  int num = ps3remote_undef;
  int i   = 0;
  for (i=0;i<ps3remote_num_keys;++i)
    {
      if (_code==ps3remote_keys[i].code)
        {
          num=i;
          break;
        }
    }
  return num;
}

void DataInd_keyDown(lirc_data* _ld,
                     unsigned int _code,
                     uint32_t _mask,
                     keyState* _ks)
{
  int num = ps3remote_undef;
  if (_code != ps3remote_keyup)
    {
      /* Key pressed. */
      BDREMOTE_LOG(_ld->config->debug,
                   fprintf(printStream, "key down (single): %02X, %08X\n", _code, _mask);
                   );
      num               = codeToIndex(_code);
      _ks->keyDown      = 1;
      _ks->lastKey      = num;
      _ks->repeat_count = 0;
      _ks->repeat_sent  = 0;

    }
  if (num != ps3remote_undef)
    {
      broadcastToLirc(_ld, ps3remote_keys[num].name, 0, ps3remote_keys[num].code);
    }
}

void DataInd_keyUp(lirc_data* _ld,
                   unsigned int _code,
                   uint32_t _mask,
                   keyState* _ks)
{
  if (_code == ps3remote_keyup)
    {
      /* Key up. */
      BDREMOTE_LOG(_ld->config->debug,
                   fprintf(printStream, "key up (single): %02X, %08X\n", _code, _mask);
                   );
      if (_ks->lastKey != ps3remote_undef)
        {
          /* broadcastToLirc(_ld, ps3remote_keys[_ks->lastKey].name, 0, _code); */

          _ks->keyDown      = 0;
          _ks->lastKey      = ps3remote_undef;
          _ks->repeat_count = 0;
          _ks->repeat_sent  = 0;
        }
    }
}

/** Broadcast the last received key to all connected LIRC clients. */
void broadcastToLirc(lirc_data* _ld, const char* _name, int _rep, unsigned int _code)
{
  int i = 0;
  char msg[100];
  int len = sprintf(msg,"%04X %02d %s %s\n", _code, _rep, _name, "SonyBDRemote");

  BDREMOTE_DBG(_ld->config->debug, msg);

  BDREMOTE_LOG(_ld->config->debug,
               fprintf(printStream, "message=%s.\n", msg);
               fprintf(printStream, "_ld->magic0=%d.\n", _ld->magic0);
               );
#if BDREMOTE_DEBUG
  assert(_ld->magic0 == 0x15);
#endif /* BDREMOTE_DEBUG */
  assert(_ld->clin < MAX_CLIENTS);

  pthread_mutex_lock (&_ld->dataMutex);

  for (i=0; i<_ld->clin; i++)
    {
      if (write_socket(_ld->clis[i], msg, len)<len)
        {
          remove_client(_ld, _ld->clis[i]);
          i--;
        }
#if BDREMOTE_DEBUG
      else
        {
          BDREMOTE_LOG(_ld->config->debug,
                       fprintf(printStream, "broadcast %d bytes to socket id %d.\n", len, _ld->clis[i]);
                       );
        }
#endif /* BDREMOTE_DEBUG */
    }

  pthread_mutex_unlock (&_ld->dataMutex);
}

/*@}*/

