/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   mcrd.h                                                            *
* PURPOSE:  Header file for mcrd.c                                            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-08                                                        *
******************************************************************************/

#ifndef __MCRD_H
#define __MCRD_H

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pds.h>
#include <curses.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"mcrd"
#define VERSION		"Version 1.1"
#define CREATED		"Created on " __DATE__ " at " __TIME__

/* Command line constants */
#define NPRETAG_ARGS	1
#define MAX_ARGV	(LINES - 3)
#define MCRD_BUFSIZ	(MAX_ARGV * PDS_TAGNAME_LEN)
#define HELP_OPT	"-h"

/* Constants for data presentation in the windows */
#define DW_OFFSET	1
#define DW_TAGLEN	50
#define DW_VALLEN	10
#define DTHDR_FMT	"%Y-%m-%dT%H:%M:%S"
#define DTHDR_LEN	26
#define ERRMSG_PAUSE	1
#define ERRMSG_LGPAUSE	2

/* Common default window constants */
#define DEFP_Y		1
#define DEFP_X		2
#define DEFW_COLS	80
#define DEFW_X		0

/* Error window constants */
#define EW_LINES	3
#define EW_COLS		DEFW_COLS
#define EW_Y		6
#define EW_X		DEFW_X

/* Header window constants */
#define HW_LINES	(LINES - MAX_ARGV)
#define HW_COLS		DEFW_COLS
#define HW_Y		1
#define HW_X		DEFW_X

/* Data window constants */
#define DW_LINES	(1 + (DW_OFFSET * 2))
#define DW_COLS		DEFW_COLS
#define DW_Y		4
#define DW_X		DEFW_X

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to clean up before terminating the program                         *
*                                                                             *
* Pre-condition:  Quit flag is true                                           *
* Post-condition: Program is cleaned up and terminated                        *
******************************************************************************/
void terminate();

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
* Function to setup the main window                                           *
*                                                                             *
* Pre-condition:  Function is called                                          *
* Post-condition: Curses is initialised.  Default program features are setup. *
*                 On success a zero is returned, on error the program is      *
*                 aborted                                                     *
******************************************************************************/
int setup_mainwin();

/******************************************************************************
* Function to setup the error reporting window                                *
*                                                                             *
* Pre-condition:  Curses must be initialised.  Function is called             *
* Post-condition: A window exists for error reporting. On success a zero is   *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_errwin();

/******************************************************************************
* Function to setup the header window                                         *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The error window must exist.   *
*                 Function is called                                          *
* Post-condition: A window exists for header data. On success a zero is       *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_hdrwin();

/******************************************************************************
* Function to setup the data window                                           *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The error window must exist.   *
*                 The number of lines for the window is passed to the         *
*                 function                                                    *
* Post-condition: A window exists for a data table. On success a zero is      *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_datawin(int nlines);

/******************************************************************************
* Function to refresh the header window                                       *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The header window must exist.  *
*                 Function is called                                          *
* Post-condition: The header window is refreshed. On success a zero is        *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int refresh_hdrwin();

/******************************************************************************
* Function to refresh the data window                                         *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The data window must exist.    *
*                 Function is called                                          *
* Post-condition: The data window is refreshed. On success a zero is          *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int refresh_datawin();

/******************************************************************************
* Function to print a message in the error window                             *
*                                                                             *
* Pre-condition:  The error window must exist.  The message to print is       *
*                 passed to the function                                      *
* Post-condition: The message is printed in the error window. On success a    *
*                 zero is returned, on error a -1 is returned                 *
******************************************************************************/
int print_err(char *msg);

/******************************************************************************
* Function to parse the program's command line                                *
*                                                                             *
* Pre-condition:  The count of command line args and the args are passed to   *
*                 function                                                    *
* Post-condition: The program's command line is parsed. On success a zero is  *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int parse_cmdln(int *argc, char **argv);

/******************************************************************************
* Function to print the program's usage message                               *
*                                                                             *
* Pre-condition:  The error window must exist.  The function is called        *
* Post-condition: The program's usage message is printed                      *
******************************************************************************/
void print_usage();

/******************************************************************************
* Function to setup the taglist struct                                        *
*                                                                             *
* Pre-condition:  The count of command line args, the args and storage for    *
*                 the taglist are passed to the function                      *
* Post-condition: The taglist struct is filled with the program's command     *
*                 line args. On success a zero is returned, on error a -1 is  *
*                 returned                                                    *
******************************************************************************/
int setup_taglist(int argc, char **argv, plctaglist *taglist);

/******************************************************************************
* Function to encapsulate the program's main loop                             *
*                                                                             *
* Pre-condition:  The filled taglist struct is passed to the function         *
* Post-condition: The program enters its main loop.  If a signal is caught,   *
*                 it exits the loop                                           *
******************************************************************************/
int mcrd_main(plctaglist *taglist);
 
#endif

