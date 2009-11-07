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

/** @defgroup bluezif Bluez capture interface implemenation

 *  This group contains the Bluez capture interface implementation.

 *  @{
 */

/*! \file capture_bluez.h
  \brief Bluez capture interface.
*/

#ifndef BD_CAPTURE_BLUEZ_H
#define BD_CAPTURE_BLUEZ_H

#include <globaldefs.h>
#include <captureif.h>

int InitcaptureLoop(captureData* _capturedata);
int captureLoop(captureData* _capturedata);

#endif /* BD_CAPTURE_BLUEZ_H */

/*@}*/

