/*
 *  bdremoteng - helper daemon for Sony(R) BD Remote Control
 *  Based on bdremoted, written by Anton Starikov <antst@mail.ru>.
 *
 *  Copyright (C) 2009 Paul Bender <pebender@san.rr.com>
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

#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "globaldefs.h"
#include "keydef.h"
#include "event_out.h"

#include <unistd.h>

#ifdef __linux__
#include <linux/input.h>
#include <linux/uinput.h>
#endif

static int event_out_fd = -1;

void event_out_send(int _event_code, 
                    int _event_value)
{
#ifdef __linux__
   struct input_event event;

   if (event_out_fd != -1)
      {
         memset(&event, 0, sizeof(event));
         event.type  = EV_KEY;
         event.code  = _event_code;
         event.value = _event_value;
         if (write(event_out_fd, &event, sizeof(event)) != sizeof(event))
            {
               ;
            }
      }
#endif
}

void event_out_destroy(void)
{
#ifdef __linux__
   if (event_out_fd != -1)
      {
         ioctl(event_out_fd, UI_DEV_DESTROY);
         close(event_out_fd);
         event_out_fd = -1;
      }
#endif
}

void event_out_init(void)
{
#ifdef __linux__
   const char* uinput_devname[] =
      {
         "/dev/uinput",
         "/dev/input/uinput",
         "/dev/misc/uinput",
         NULL
      };
   int i;
   struct uinput_user_dev dev;

   event_out_fd = -1;
   for (i = 0; (event_out_fd == -1) && (uinput_devname[i] != NULL); i++)
      {
         event_out_fd = open(uinput_devname[i], O_WRONLY | O_NDELAY);
      }
   if (event_out_fd == -1)
      {
         return;
      }

   memset(&dev, 0, sizeof(dev));
   strncpy(dev.name, "bdremoteng", sizeof(dev.name));
   dev.name[sizeof(dev.name) - 1] = '\0';

   if (write(event_out_fd, &dev, sizeof(dev)) != sizeof(dev))
      {
         close(event_out_fd);
         event_out_fd = -1;
         return;
      }

   if (ioctl(event_out_fd, UI_SET_EVBIT, EV_KEY) != 0)
      {
         close(event_out_fd);
         event_out_fd = -1;
         return;
      }

   for(i = 0 ; i < ps3remote_num_keys ; i++)
      {
         if (ioctl(event_out_fd, UI_SET_KEYBIT, ps3remote_keys[i].event_code) != 0)
            {
               close(event_out_fd);
               event_out_fd = -1;
               return;
            }
      }

   if (ioctl(event_out_fd, UI_DEV_CREATE) != 0)
      {
         close(event_out_fd);
         event_out_fd = -1;
         return;
      }
#endif
}
