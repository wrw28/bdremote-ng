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

#include "lirc_srv.h"
#include <globaldefs.h>

#include <stdint.h>
#include <keydef.h>
#include <stdio.h>
#include <assert.h>

void broadcast(lirc_data* _lc);

void RemoteConnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote connected.");
}

/* Received some data from the ps3 remote. 
 * Forward it to LIRC clients.
 * Note: no threads are used, so no need locking.
 */
void DataInd(void* _p, const char* _data, const int _size)
{
  lirc_data* lc = (lirc_data*)_p;
#if BDREMOTE_DEBUG
  assert(lc->magic0 == 0x15);
#endif /* BDREMOTE_DEBUG */
  int i;
  int num;
  const uint32_t* mask_in = (uint32_t *)(_data+2);
  uint32_t mask     = 0;
  uint32_t mask1    = 0;
  const unsigned char* magic = (const unsigned char*)_data;
  const unsigned char* code  = (const unsigned char*)_data+5;	
  const unsigned char* state = (const unsigned char*)_data+11;

  if ((_size==13) && (*magic==0xa1))
    {
      mask=(*mask_in) & 0xFFFFFF;

      num=-1;

      /* printf("%02X , %02X , %08X\n", *code, *state, mask); */
      switch (*state)
	{
	case 1:
	  {
	    if (*code != 0xFF)
	      {
		switch (lc->laststate)
		  {
		  case 0:
		    BDREMOTE_DBG(lc->config->debug, "single.");			
		    for (i=0;i<ps3remote_num_keys;++i)
		      {
			if (*code==ps3remote_keys[i].code)
			  {
			    num=i;
			    break;
			  }
		      }
		    break;
		  }
		lc->lastcode=*code;
		lc->lastmask=mask;
		lc->laststate=1;
	      }
	    else
	      if (mask!=0x0 && lc->laststate==1 && mask!=lc->lastmask)
		{
		  mask1=mask - lc->lastmask;
		  BDREMOTE_DBG(lc->config->debug, "multiple");
		  /* printf("multiple, %08X, %08X, %08X\n",lc->lastmask, mask, mask1); */
		  for (i=0;i<ps3remote_num_masked;++i)
		    {
		      if (mask1==ps3remote_keys[i].mask)
			{
			  num=i;					
			  break;
			}
		    }
		  lc->lastmask=mask;
		  lc->laststate=2;			
		}
	    if (num>=0)
	      {
		lc->lastkey=num;
		broadcast(lc);
	      }
	    else
	      {
		lc->lastkey=-1;		
		lc->lastsend=-1;
		broadcast(lc);
	    }
	    
	    break;
	  }
	case 0:
	  {
	    lc->laststate = 0;
	    lc->lastkey   = -1;
	    lc->lastsend  = -1;
	    broadcast(lc);
	    break;
	  }
	}
      /* printf("E:%02X , %02X , %08X\n", lc->lastcode, lc->laststate, lc->lastmask); */
    }
}

/* Broadcast the last received key to all connected LIRC clients. */
void broadcast(lirc_data* _lc)
{ 
  int rep=-1;
  char msg[100];	

  if (_lc->lastsend!=_lc->lastkey) 
    {
      rep=-1;
      _lc->lastsend=_lc->lastkey;
    };
  sprintf(msg,"%04X %02d %s %s\n", ps3remote_keys[_lc->lastkey].code, ++rep, ps3remote_keys[_lc->lastkey].name, "SonyBDRemote");
  broadcast_message(_lc, msg);
}

void RemoteDisconnected(void* _p)
{
  lirc_data* lc = (lirc_data*)_p;

  BDREMOTE_DBG(lc->config->debug, "Remote disconnected.");
}

	
