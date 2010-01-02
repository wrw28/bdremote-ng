/*
 *  bdremoteng - helper daemon for Sony(R) BD Remote Control
 *  Based on bdremoted, written by Anton Starikov <antst@mail.ru>.
 *
 *  Copyright (C) 2009 Paul Bender <pebender@gmail.com>
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

/** @defgroup uinput Uinput Support
 * 
 *  Output keypresses using event device using the Linux uinput
 *  driver.
 * 
 *  @{
 */

/*! \file event_out.h
  \brief uinput support.
*/

#ifndef EVENT_OUT_H
#define EVENT_OUT_H

#include "bdrcfg.h"

/** Send event. */
void event_out_send(int _code, int _value);

/** Initialize. */
void event_out_init(void);

/** Close sockets etc.. */
void event_out_destroy(void);


#endif
