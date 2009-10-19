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

/** @defgroup Gen Generic
 *  This group contains generic structures and functions used by this application.
 *  @{
 */

/*! \file bdrcfg.h
  \brief Configuration struct and functions used for handling it.

  The configuration struct and the functions in this header files
  are used for storing the configuration of this application.
*/

#ifndef BD_CFG_H
#define BD_CFG_H

/** Common configuration. */
typedef struct
{
  /** Listen to this port for LIRC connections. */
  int listen_port;
  /** Disconnect BT peers after this number of seconds. */
  int disconnect_timeout;
  /** Repeat rate, number of messages per second. */
  int repeat_rate;
  /** Repeat delay - the amount of messages to ignore before
      repeating.*/
  int repeat_delay;
  /** Enable/disable printing of debug messages. */
  int debug;
  /** Indicates if the BT address of the interface to use was set. */
  int interface_addr_set;
  /** BT address of the interface to use. */
  char* interface_addr;
  /** BT address of the PS3 remote. */
  char* remote_addr;
  /** Enable/disable detach from TTY.*/
  int detach;
  /** Change to the UID of this user. */
  char* user;
  /** Change to the GID of this group. */
  char* group;
  /** Indicates if the log filename was set.*/
  int log_filename_set;
  /** Write log to this file name. */
  char* log_filename;
} configuration;

/** Set default configuration. */
void setDefaults(configuration* _config);

/** Set the remote BD address to use. */
void setRemoteAddress(configuration* _config, const char* _address);

/** Set the BD address of the interface to use. */
void setInterfaceAddress(configuration* _config, const char* _address);

/** Set user to change to after opening sockets. */
void setUser(configuration* _config, const char* _user);

/** Set group to change to after opening sockets. */
void setGroup(configuration* _config, const char* _group);

/** Return 1 if both user and group were set. 0 otherwise. */
int userAndGroupSet(const configuration* _config);

/** Set filename used for logging. */
void setLogFilename(configuration* _config, const char* _filename);

/** Destroy the config. */
void destroyConfig(configuration* _config);

/** Print each config item on a line of its own. */
void printConfig(const configuration* _config);

#endif /* BD_CFG_H */

/*@}*/

