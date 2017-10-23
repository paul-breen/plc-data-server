/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   nwtio.h                                                           *
* PURPOSE:  Header file for nwtio.c                                           *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-18                                                        *
******************************************************************************/

#ifndef __NWTIO_H
#define __NWTIO_H

#include <stdio.h>

#include <pds.h>
#include <pdsnp.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"nwtio"
#define VERSION		"Version 1.3"
#define CREATED		"Created on " __DATE__ " at " __TIME__

/******************************************************************************
* nwtio's command line arguments struct definition                            *
******************************************************************************/
typedef struct nwtio_args_rec
{
  char fmt;                       /* Data format specifier */
  char op;                        /* Operation */
  unsigned short int oparg;       /* Operation's argument */
  char *tagname;                  /* The tag name to apply operation to */
  char *host;                     /* The PDS host's {IP address|hostname} */
  unsigned short int port;        /* The PDS host's port */
} nwtio_args;

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
int parse_nwtio_cmdln(int argc, char *argv[], nwtio_args *args);

#endif

