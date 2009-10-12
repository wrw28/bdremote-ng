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

#include "globaldefs.h"

#if BDREMOTE_DEBUG
#  include <assert.h>

#  define getPart(PART) {                       \
    struct tm* Tm = localtime(_ltime);          \
    return Tm->PART;                            \
  }

int getHour(time_t* _ltime)
{
  getPart(tm_hour);
}

int getMinute(time_t* _ltime)
{
  getPart(tm_min);
}

int getSecond(time_t* _ltime)
{
  getPart(tm_sec);
}

const char* slashOffset(const char* _filenameWithSlashes)
{
  int offset = 0;
  int len    = 0;
  int i      = 0;
  assert(_filenameWithSlashes != NULL);

  len = strlen(_filenameWithSlashes);

  for (i = len; i >= 0; i--)
    {
      if (_filenameWithSlashes[i] == '/')
	{
	  offset = i;

	  if ((offset+1) < len)
	    {
	      offset++;
	    }
	  break;
	}
      
    }

  return _filenameWithSlashes+offset;
}

#endif /* BDREMOTE_DEBUG */

