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
#include <unistd.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"tem"
#define VERSION		"Version 1.1"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define TMSTAMP_FMT	"%Y-%m-%dT%H:%M:%S"
#define TMSTAMP_LEN	25
#define CHECK_PAUSE     1000      /* (usecs) */

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

/******************************************************************************
* Function to print the tag's properties                                      *
*                                                                             *
* Pre-condition:  The tag struct to be printed & the tag's previous value are *
*                 passed to the function.                                     *
* Post-condition: The tag's mod. time, name and current and previous values   *
*                 are printed out                                             *
******************************************************************************/
int print_tag(pdstag *tag, unsigned short int prev_val);

#endif

