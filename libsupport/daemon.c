/******************************************************************************
* PROJECT:  Daemon library                                                    *
* MODULE:   daemon.c                                                          *
* PURPOSE:  Functions to daemonise a process                                  * 
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1998-10-29                                                        *
******************************************************************************/

#include "daemon.h"

/******************************************************************************
* Function to daemonise a process                                             *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: An attempt is made to daemonise the process.  If successful *
*                 the process is daemonised and has no controlling terminal.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int daemonise_process()
{
  pid_t pid = 0;

  if((pid = fork()) != 0)              /* Terminate the parent process, */
    exit(0);                           /* the child process continues */

  setsid();                            /* Child becomes the session leader */
  signal(SIGHUP, SIG_IGN);

  if((pid = fork()) != 0)              /* Terminate the 1st child process, */
    exit(0);                           /* a 2nd child process continues */

  return 0;
}



/******************************************************************************
* Function to redirect the standard file descriptors (stdin, stdout & stderr) *
*                                                                             *
* Pre-condition:  The file/device path and the flags are passed to function   *
* Post-condition: The std streams are redirected to this file.  On error a    *
*                 -1 is returned                                              *
******************************************************************************/
int redirect_std_fds(char *filepath, int flags)
{
  int fd = 0;
  int retval = -1;

  if((fd = open(filepath, flags, 0)) != -1)
  {
    dup2(fd, STDIN_FILENO);            /* Redirect all I/O to <filepath> */  
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Function to close any 'inherited' file descriptors                          *
*                                                                             *
* Pre-condition:  The fd's to close from <start> upto and including <end> are *
*                 passed to the function                                      *
* Post-condition: The fd's are closed                                         *
******************************************************************************/
int close_inherited_fds(int start, int end)
{
  int fd = 0;

  for(fd = start; fd <= end; fd++)
  {
    close(fd);
  }

  return 0; 
}



/******************************************************************************
* Wrapper function to daemonise a process                                     *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: An attempt is made to daemonise the process.  If successful *
*                 the process is daemonised and has no controlling terminal.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int daemonise()
{
  int retval = -1;

  if(daemonise_process() != -1)
  {
    /* Clear file mode creation mask, redirect all std streams to /dev/null,
       and close all fd's after std fd's upto maxfd */
    umask(0);
    retval = redirect_std_fds(_PATH_DEVNULL, O_RDWR);
    close_inherited_fds(3, DAEMON_MAXFD); 
  }

  return retval;
}



/******************************************************************************
* Function to daemonise a process (with output going to syslog)               *
*                                                                             *
* Pre-condition:  The process name (program name) and log message facility    *
*                 are passed to the function                                  *
* Post-condition: The process is daemonised and has no controlling terminal   *
******************************************************************************/
void daemon_init(char *pname, int facility)
{
  if(daemonise() != -1)
    openlog(pname, LOG_PID, facility); /* Write output to system log */
}



/******************************************************************************
* Function to setup a daemonised process (invoked by inetd)                   *
*                                                                             *
* Pre-condition:  The process name (program name) and log message facility    *
*                 are passed to the function                                  *
* Post-condition: The process is setup                                        *
******************************************************************************/
void daemon_inetd(char *pname, int facility)
{
  /* N.B.:  All the daemonisation code is done by inetd upon starting... */

  openlog(pname, LOG_PID, facility);   /* Write output to system log */
}

