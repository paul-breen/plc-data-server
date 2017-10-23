/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   tem.h                                                             *
* PURPOSE:  Header file for tem.c                                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-15                                                        *
******************************************************************************/

#ifndef __TEM_H
#define __TEM_H 

#include <stdio.h>
#include <signal.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"tem"
#define VERSION		"Version 1.0"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define TMSTAMP_FMT	"%Y-%m-%dT%H:%M:%S"
#define TMSTAMP_LEN	25

#define UNDERLINE(c)\
{\
  int i = 0;\
  for(i = 0; i < c; i++) putchar('-');\
  putchar('\n');\
}

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to install a signal handler                                        *
*                                                                             *
* Pre-condition:  Program is running, signal is received                      *
* Post-condition: Signal handler is called                                    *
******************************************************************************/
void install_signal_handler();

/******************************************************************************
* Function to handle quit signals                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Quit flag is set, signal handler is re-installed            *
******************************************************************************/
void set_quit();

#endif

