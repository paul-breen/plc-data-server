/******************************************************************************
* PROJECT:  PDS Utilities                                                     *
* MODULE:   tio.h                                                             *
* PURPOSE:  Header file for tio.c                                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-18                                                        *
******************************************************************************/

#ifndef __TIO_H
#define __TIO_H

#include <stdio.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"tio"
#define VERSION		"Version 1.3"
#define CREATED		"Created on " __DATE__ " at " __TIME__

/******************************************************************************
* tio's command line arguments struct definition                              *
******************************************************************************/
typedef struct tio_args_rec
{
  char fmt;                       /* Data format specifier */
  char op;                        /* Operation */
  unsigned short int oparg[2];    /* Operation's argument */
  short int nargs;                /* No. of arguments */
  char *tagname;                  /* The tag name to apply operation to */
} tio_args;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure                         *
******************************************************************************/
int parse_tio_cmdln(int argc, char *argv[], tio_args *args);

#endif

