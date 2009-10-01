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

#define getPart(PART) { \
  struct tm* Tm = localtime(_ltime); \
  return Tm->PART;		     \
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

#endif /* BDREMOTE_DEBUG */

