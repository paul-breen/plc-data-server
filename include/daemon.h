/******************************************************************************
* PROJECT:  Daemon library                                                    *
* MODULE:   daemon.h                                                          *
* PURPOSE:  Header file for daemon.c                                          * 
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1998-10-29                                                        *
******************************************************************************/

#ifndef __DAEMON_H
#define __DAEMON_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <paths.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define NOTDAEMON	0              /* Not daemonised */
#define DAEMON		1              /* Daemonised */

#define DAEMON_MAXFD	64

#ifndef _PATH_DEVNULL
#define _PATH_DEVNULL	"/dev/null"
#endif

#ifndef _PATH_CONSOLE
#define _PATH_CONSOLE	"/dev/console"
#endif

#ifndef STDIN_FILENO              /* If std streams' fds not defined... */

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#endif

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to daemonise a process                                             *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: An attempt is made to daemonise the process.  If successful *
*                 the process is daemonised and has no controlling terminal.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int daemonise_process();

/******************************************************************************
* Function to redirect the standard file descriptors (stdin, stdout & stderr) *
*                                                                             *
* Pre-condition:  The file/device path and the flags are passed to function   *
* Post-condition: The std streams are redirected to this file.  On error a    *
*                 -1 is returned                                              *
******************************************************************************/
int redirect_std_fds(char *filepath, int flags);

/******************************************************************************
* Function to close any 'inherited' file descriptors                          *
*                                                                             *
* Pre-condition:  The fd's to close from <start> upto and including <end> are *
*                 passed to the function                                      *
* Post-condition: The fd's are closed                                         *
******************************************************************************/
int close_inherited_fds(int start, int end);

/******************************************************************************
* Wrapper function to daemonise a process                                     *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: An attempt is made to daemonise the process.  If successful *
*                 the process is daemonised and has no controlling terminal.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int daemonise();

/******************************************************************************
* Function to daemonise a process (with output going to syslog)               *
*                                                                             *
* Pre-condition:  The process name (program name) and log message facility    *
*                 are passed to the function                                  *
* Post-condition: The process is daemonised and has no controlling terminal   *
******************************************************************************/
void daemon_init(char *pname, int facility);

/******************************************************************************
* Function to setup a daemonised process (invoked by inetd)                   *
*                                                                             *
* Pre-condition:  The process name (program name) and log message facility    *
*                 are passed to the function                                  *
* Post-condition: The process is setup                                        *
******************************************************************************/
void daemon_inetd(char *pname, int facility);

#endif

