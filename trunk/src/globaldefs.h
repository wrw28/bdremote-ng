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

#ifndef BD_GLOBALDEFS_H
#define BD_GLOBALDEFS_H

/** Define used to return errors. */
#ifndef BDREMOTE_FAIL
#  define BDREMOTE_FAIL (-1)
#endif

/** Define used to return success. */
#ifndef BDREMOTE_OK
#  define BDREMOTE_OK (0)
#endif

/** Define this to enable debug. */
#ifndef BDREMOTE_DEBUG
#  define BDREMOTE_DEBUG 1
#endif

#if BDREMOTE_DEBUG
#  include <stdio.h>
#  include <time.h>

int getHour(time_t* _ltime);
int getMinute(time_t* _ltime);
int getSecond(time_t* _ltime);

/** Macro used to print debug output. */
#  define BDREMOTE_DBG(_ENABLED, _x) if (_ENABLED) { time_t ltime = time(NULL); printf("%s:%d -> %d:%d:%d %s\n", __FILE__, __LINE__, getHour(&ltime), getMinute(&ltime), getSecond(&ltime), _x); }
/** Macro used to print error output. */
#  define BDREMOTE_ERR(x) printf("%s:%d -> ERROR: %s\n", __FILE__, __LINE__, x)
#else
/** Macro used to print debug output. */
#  define BDREMOTE_DBG(_ENABLED, _x) if (_ENABLED) { printf("%s\n", _x); }
/** Macro used to print error output. */
#  define BDREMOTE_ERR(x) printf("Error: %s\n", x)
#endif

/** Macro used to copy a string to the config. */
#define SETVAL(CK, S) {                         \
      if (CK != NULL)                           \
         {                                      \
            free(CK);                           \
            CK = NULL;                          \
         }                                      \
      CK =(char *)malloc(strlen(S)+1);          \
      strcpy(CK, S);                            \
   }

/** Macro used to destroy string members of the config. */
#define FREEVAL(CK) {   \
      if (CK != NULL)   \
         {              \
            free(CK);   \
            CK = NULL;  \
         }              \
   }

#endif /* BD_GLOBALDEFS_H */

