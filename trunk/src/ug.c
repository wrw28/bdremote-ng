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

#include "ug.h"

#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

#include <globaldefs.h>

/* The following function was taken from BTG (btg.berlios.de), also
 * written by me.
 */

int changeUIDAndGID(const char* _user,
                    const char* _group)
{
  int result              = BDREMOTE_OK;
  struct passwd* s_passwd = NULL;
  uid_t uid               = -1;
  struct group* s_group   = NULL;
  gid_t gid               = -1;
  /* Resolve the user and group into uid/gid. */

  /* User. */
  s_passwd = getpwnam(_user);

  if (s_passwd == 0)
    {
      result = BDREMOTE_FAIL;
      return result;
    }

  uid = s_passwd->pw_uid;

  /* Group. */
  s_group = getgrnam(_group);

  if (s_group == 0)
    {
      result = BDREMOTE_FAIL;
      return result;
    }

  gid = s_group->gr_gid;

  /* Do the change. */

  if (setgid(gid) != 0)
    {
      result = BDREMOTE_FAIL;
    }

  if (setuid(uid) != 0)
    {
      result = BDREMOTE_FAIL;
    }

  return result;
}
