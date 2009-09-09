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

#include "bdremoteng.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <pthread.h>

#include <globaldefs.h>
#include <bdrcfg.h>
#include <captureif.h>
#include <lirc_srv.h>

static void sig_hup(int sig);
static void sig_term(int sig);

void* listener(void* _p);

int main(int argc, char *argv[])
{
  pthread_t bt_thread;
  struct sigaction sa;
  int opt;
  
  configuration config;
  setDefaults(&config);
  
  while ((opt=getopt(argc,argv,"+p:t:a:r:d:nh"))!=-1)
    {
      switch(opt)
	{
	case 'p':
	  config.listen_port=atoi(optarg);
	  break;
	case 't':
	  config.disconnect_timeout=atoi(optarg);
	  break;
	case 'a':
	  setRemoteAddress(&config, optarg);
	  break;
	case 'r':
	  config.repeat_rate=atoi(optarg);
	  break;
	case 'd':
	  config.debug = atoi(optarg);
	  break;
	case 'n':
	  config.detach=0;
	  break;
	case 'h':
	  usage();
	  exit(0);
	  break;
	default:
	  exit(0);
	}
    }
  
  if (config.remote_addr == NULL)
    {
      usage();
      printf("\nPlease specify a remote BD address using the -a switch.\n");

      exit(0);
    }

  if (config.debug)
    {
      printConfig(&config);
    }

  lirc_data ldata;
  initLircData(&ldata, &config);
  
  captureData cdata;
  InitCaptureData(&cdata,
		  &config,
		  &ldata,
		  config.remote_addr,
		  config.disconnect_timeout);

  if (config.detach==1)
    {
      if (daemon(0, 0))
	{
	  perror("Can't start daemon");
	  exit(1);                                                
	};
    };

  nice(-4);

  // Start listening for BT clients.
  if (pthread_create(&bt_thread, NULL, listener, &cdata) != 0)
    {
      perror("Could not create BT client thread");
      exit(1);                                                
    }

  memset(&sa, 0, sizeof(sa));
  sa.sa_flags = SA_NOCLDSTOP;
  sa.sa_handler = sig_term;
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT,  &sa, NULL);
  sa.sa_handler = sig_hup;
  sigaction(SIGHUP, &sa, NULL);
  
  sa.sa_handler = SIG_IGN;
  sigaction(SIGCHLD, &sa, NULL);
  sigaction(SIGPIPE, &sa, NULL);

  
  // Start handling LIRC clients and forwarding data.
  lirc_server(&config, &ldata);

  BDREMOTE_DBG(config.debug, "Terminating.");

  pthread_kill (bt_thread, SIGTERM);
  BDREMOTE_DBG(config.debug, "Waiting for threads to finish.");
  pthread_join(bt_thread, NULL);

  DestroyCaptureData(&cdata);
  destroyLircData(&ldata);

  destroyConfig(&config);

  return EXIT_SUCCESS;
}

void* listener(void* _p)
{
  captureData* cd = (captureData*)_p;

  BDREMOTE_DBG(cd->config->debug, "Started listener thread.");

  int ret = captureLoop(cd);
  if (ret < 0)
    {
      BDREMOTE_DBG(cd->config->debug, "captureLoop failed.");
    }

  return 0;
}

void usage(void)                                                         
{                                                                               
	printf("bdremoteng - Sony BD Remote helper daemon version %s\n\n", VERSION);
	printf("Usage:\n"                                                       
		"\tbdremoteng [options]\n"                                 
		"\n");
	printf("Options:\n"                                                     
		"\t-p <port>            Set port number for incoming LIRCD connections\n"       
		"\t-t <timeout>         Set disconnect timeout for BD remote (in minutes)\n"        
		"\t-a <address>         BT addres of remote.\n"
		"\t                     For example: -a 00:19:C1:5A:F1:3F \n"
		"\t-r <rate>            Key repeat rate. Generate <rate> repeats per\n"
		"\t                     second, when key is pressed\n"    
		"\t-d                   Enable debug.\n"     				
		"\t-n                   Don't fork daemon to background\n"      
		"\t-h, --help           Display help\n"                         
		"\n");                                                          

}

void sig_hup(int sig)
{
  // BDREMOTE_DBG("Not handling HUP.");
}

void sig_term(int sig)
{
  extern volatile sig_atomic_t __io_canceled;
  __io_canceled = 1;
}
