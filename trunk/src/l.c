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

#include "l.h"

#include <stdio.h>

#include <globaldefs.h>

FILE* printStream = NULL;

void setDefaultLog()
{
  printStream = stdout;
}

int setLogFile(const configuration* _config)
{
  if (_config->log_filename_set)
    {
      FILE* f = fopen(_config->log_filename, "a");
  
      if (f == NULL)
	{
	  printf("Unable to open log file '%s'.\n", _config->log_filename);
	  return BDREMOTE_FAIL;
	}

      printStream = f;
    }

  return BDREMOTE_OK;
}

void closeLogFile()
{
  if (printStream != stdout)
    {
      fclose(printStream);
      printStream = stdout;
    }
}
