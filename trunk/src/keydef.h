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

/** @defgroup Keys PS3 remote key definition.
 *  This group contains the definition of the keys on the PS3 remote.
 *  @{
 */


/*! \file keydef.h
  \brief PS3 remote keys.

  This file contains the definition of the keys on the PS3 remote
  and some useful defines.

*/

#ifndef BD_KEYDEF_H
#define BD_KEYDEF_H

#include <stdint.h>

/** Key information. */
struct key_info
{
  /** The original name. */
  const char* name_orig;
  /** The LIRC namespace name. */
  const char* name_lirc;
  /** The Linux key event code. */
  unsigned int event_code;
  /** The code. */
  unsigned int code;
  /** The mask. */
  uint32_t mask;
};

#define KEY_ENTER		28
#define KEY_POWER		116
#define KEY_SELECT		0x161
#define BTN_C			0x132
#define BTN_Z			0x135
#define BTN_START		0x13b
#define KEY_UP			103
#define KEY_RIGHT		106
#define KEY_DOWN		108
#define KEY_LEFT		105
#define BTN_B			0x131
#define BTN_Y			0x134
#define BTN_A			0x130
#define BTN_X			0x133
#define KEY_OPTION		0x165
#define KEY_BACK		158
#define KEY_CLEAR		0x163
#define KEY_ZOOM		0x174
#define KEY_1			2
#define KEY_2			3
#define KEY_3			4
#define KEY_4			5
#define KEY_5			6
#define KEY_6			7
#define KEY_7			8
#define KEY_8			9
#define KEY_9			10
#define KEY_0			11
#define KEY_MEDIA_REPEAT	0x1b7
/*#define KEY_CLEAR		0x163*/
#define KEY_EJECTCD		161
#define KEY_MENU		139
#define KEY_TIME		0x167
#define KEY_PREVIOUS		0x19c
#define KEY_NEXT		0x197
#define KEY_PLAY		207
#define KEY_REWIND		168
#define KEY_FORWARD		159
#define KEY_STOP		128
#define KEY_PAUSE		119
#define KEY_CONTEXT_MENU	0x1b6
#define KEY_FRAMEBACK		0x1b4
#define KEY_FRAMEFORWARD	0x1b5
#define KEY_SUBTITLE		0x172
#define KEY_AUDIO		0x188
#define KEY_ANGLE		0x173
#define KEY_INFO		0x166
#define KEY_BLUE		0x191
#define KEY_RED			0x18e
#define KEY_GREEN		0x18f
#define KEY_YELLOW		0x190

/** The code for a key release. */
#define ps3remote_keyup 0xFF
/** Undefined key. */
#define ps3remote_undef -1

/** The max number of keys. */
#define ps3remote_num_keys   51
#define ps3remote_num_masked 18

/** Array of keys. */
static struct key_info ps3remote_keys[] =
  {
    {"enter\0"   , "KEY_ENTER\0"       , KEY_ENTER       , 0x0b, 0x080000},
    {"ps\0"      , "KEY_POWER\0"       , KEY_POWER       , 0x43, 0x010000},
    {"select\0"  , "KEY_SELECT\0"      , KEY_SELECT      , 0x50, 0x000001},
    {"l3\0"      , "BTN_C\0"           , BTN_C           , 0x51, 0x000002},
    {"r3\0"      , "BTN_Z\0"           , BTN_Z           , 0x52, 0x000004},
    {"start\0"   , "BTN_START\0"       , BTN_START       , 0x53, 0x000008},
    {"up\0"      , "KEY_UP\0"          , KEY_UP          , 0x54, 0x000010},
    {"right\0"   , "KEY_RIGHT\0"       , KEY_RIGHT       , 0x55, 0x000020},
    {"down\0"    , "KEY_DOWN\0"        , KEY_DOWN        , 0x56, 0x000040},
    {"left\0"    , "KEY_LEFT\0"        , KEY_LEFT        , 0x57, 0x000080},
    {"l2\0"      , "BTN_B\0"           , BTN_B           , 0x58, 0x000100},
    {"r2\0"      , "BTN_Y\0"           , BTN_Y           , 0x59, 0x000200},
    {"l1\0"      , "BTN_A\0"           , BTN_A           , 0x5a, 0x000400},
    {"r1\0"      , "BTN_X\0"           , BTN_X           , 0x5b, 0x000800},
    {"triangle\0", "KEY_OPTION\0"      , KEY_OPTION      , 0x5c, 0x001000},
    {"circle\0"  , "KEY_BACK\0"        , KEY_BACK        , 0x5d, 0x002000},
    {"cross\0"   , "KEY_CLEAR\0"       , KEY_CLEAR       , 0x5e, 0x004000},
    {"square\0"  , "KEY_ZOOM\0"        , KEY_ZOOM        , 0x5f, 0x008000},
    {"num1\0"    , "KEY_1\0"           , KEY_1           , 0x00, 0x000000},
    {"num2\0"    , "KEY_2\0"           , KEY_2           , 0x01, 0x000000},
    {"num3\0"    , "KEY_3\0"           , KEY_3           , 0x02, 0x000000},
    {"num4\0"    , "KEY_4\0"           , KEY_4           , 0x03, 0x000000},
    {"num5\0"    , "KEY_5\0"           , KEY_5           , 0x04, 0x000000},
    {"num6\0"    , "KEY_6\0"           , KEY_6           , 0x05, 0x000000},
    {"num7\0"    , "KEY_7\0"           , KEY_7           , 0x06, 0x000000},
    {"num8\0"    , "KEY_8\0"           , KEY_8           , 0x07, 0x000000},
    {"num9\0"    , "KEY_9\0"           , KEY_9           , 0x08, 0x000000},
    {"num0\0"    , "KEY_0\0"           , KEY_0           , 0x09, 0x000000},
    {"return\0"  , "KEY_MEDIA_REPEAT\0", KEY_MEDIA_REPEAT, 0x0e, 0x000000},
    {"clear\0"   , "KEY_CLEAR\0"       , KEY_CLEAR       , 0x0f, 0x000000},
    {"eject\0"   , "KEY_EJECTCD\0"     , KEY_EJECTCD     , 0x16, 0x000000},
    {"topmenu\0" , "KEY_MENU\0"        , KEY_MENU        , 0x1a, 0x000000},
    {"time\0"    , "KEY_TIME\0"        , KEY_TIME        , 0x28, 0x000000},
    {"prev\0"    , "KEY_PREVIOUS\0"    , KEY_PREVIOUS    , 0x30, 0x000000},
    {"next\0"    , "KEY_NEXT\0"        , KEY_NEXT        , 0x31, 0x000000},
    {"play\0"    , "KEY_PLAY\0"        , KEY_PLAY        , 0x32, 0x000000},
    {"scanrev\0" , "KEY_REWIND\0"      , KEY_REWIND      , 0x33, 0x000000},
    {"scanfwd\0" , "KEY_FORWARD\0"     , KEY_FORWARD     , 0x34, 0x000000},
    {"stop\0"    , "KEY_STOP\0"        , KEY_STOP        , 0x38, 0x000000},
    {"pause\0"   , "KEY_PAUSE\0"       , KEY_PAUSE       , 0x39, 0x000000},
    {"popup\0"   , "KEY_CONTEXT_MENU\0", KEY_CONTEXT_MENU, 0x40, 0x000000},
    {"steprev\0" , "KEY_FRAMEBACK\0"   , KEY_FRAMEBACK   , 0x60, 0x000000},
    {"stepfwd\0" , "KEY_FRAMEFORWARD\0", KEY_FRAMEFORWARD, 0x61, 0x000000},
    {"subtitle\0", "KEY_SUBTITLE\0"    , KEY_SUBTITLE    , 0x63, 0x000000},
    {"audio\0"   , "KEY_AUDIO\0"       , KEY_AUDIO       , 0x64, 0x000000},
    {"angle\0"   , "KEY_ANGLE\0"       , KEY_ANGLE       , 0x65, 0x000000},
    {"display\0" , "KEY_INFO\0"        , KEY_INFO        , 0x70, 0x000000},
    {"blue\0"    , "KEY_BLUE\0"        , KEY_BLUE        , 0x80, 0x000000},
    {"red\0"     , "KEY_RED\0"         , KEY_RED         , 0x81, 0x000000},
    {"green\0"   , "KEY_GREEN\0"       , KEY_GREEN       , 0x82, 0x000000},
    {"yellow\0"  , "KEY_YELLOW\0"      , KEY_YELLOW      , 0x83, 0x000000},
  };

#endif /* BD_KEYDEF_H */

/*@}*/

