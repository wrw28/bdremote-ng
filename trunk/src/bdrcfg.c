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

#include "bdrcfg.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Macro used to copy a string to the config.
#define SETVAL(C, K, S) {                       \
      if (C->K != NULL)                         \
         {                                      \
            free(C->K);                         \
            C->K = NULL;                        \
         }                                      \
      C->K =(char *)malloc(strlen(S)+1);        \
      strcpy(C->K, S);                          \
   }

// Macro used to destroy string members of the config.
#define FREEVAL(C, K) { \
   if (C->K != NULL)    \
      {                 \
         free(C->K);    \
         C->K = NULL;   \
      }                 \
   }

void setDefaults(configuration* _config)
{
  _config->listen_port = 8888;
  _config->disconnect_timeout = 60;
  _config->repeat_rate = 10;
  _config->debug       = 0;
  _config->remote_addr = NULL;
  _config->detach      = 1;
  _config->user        = NULL;
  _config->group       = NULL;
}

void setRemoteAddress(configuration* _config, const char* _address)
{
   SETVAL(_config, remote_addr, _address);
}

void setUser(configuration* _config, const char* _user)
{
   SETVAL(_config, user, _user);
}

void setGroup(configuration* _config, const char* _group)
{
   SETVAL(_config, group, _group);
}

void destroyConfig(configuration* _config)
{
   FREEVAL(_config, remote_addr);
   FREEVAL(_config, user);
   FREEVAL(_config, group);
}

void printConfig(configuration* _config)
{
  printf("Configuration:\n");
  printf(" - listen port: %d.\n", _config->listen_port);
  printf(" - timeout    : %d.\n", _config->disconnect_timeout);
  printf(" - repeat rate: %d per secound.\n", _config->repeat_rate);
  printf(" - debug      : %d.\n", _config->debug);
  printf(" - remote addr: %s.\n", _config->remote_addr);
  printf(" - detach     : %d.\n", _config->detach);
  if (_config->user == NULL)
     {
        printf(" - user       : not set.\n");
     }
  else
     {
        printf(" - user       : %s.\n", _config->user);
     }
  if (_config->group == NULL)
     {
        printf(" - group      : not set.\n");
     }
  else
     {
        printf(" - group      : %s.\n", _config->group);
     }
}

int userAndGroupSet(configuration* _config)
{
   int status = 1;

   if (_config->user == NULL)
     {
        status = 0;
     }
  if (_config->group == NULL)
     {
        status = 0;
     }

  return status;
}
