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

/** \ingroup Test
 *  @{
 */

/*! \file ttest.c
  \brief Test timers.

  Test application which tests if simple timers behave as expected.
*/

#include <globaldefs.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <lirc_srv.h>

unsigned int globalLogMask = 
  MODULEMASK_LIRC_THR | MODULEMASK_LIRC_SOCK | 
  MODULEMASK_LIRC_CB | MODULEMASK_BT_IF | MODULEMASK_BT_IMPL | 
  MODULEMASK_QUEUE | MODULEMASK_SPARE | MODULEMASK_MAIN;


static const unsigned int moduleMask = MODULEMASK_MAIN;

int main(int argc, char *argv[])
{
  int i = 0;
  keyState ks;

  if (argc > 1)
    {
      printf("Arguments are not supported.\n");

      for (i = 1; i < argc; i++)
	{
	  printf("Unhandled argument: %s.\n", argv[i]);
	}
      return -1;
    }

  initTime(&ks);

  /* sleep(1); */
  i = 0;
  while (i < 50)
    {
      printf("Sleep #%d.\n", i);

      usleep(10000);

      updateTime(&ks);

      printf("Elapsed ms: %lu\n.", ks.elapsed);

      i++;
    }

  return 0;
}

/*\@}*/

