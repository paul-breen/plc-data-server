/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   pds_ctl.h                                                         *
* PURPOSE:  Header file for pds_ctl.c                                         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-02                                                        *
******************************************************************************/

#ifndef __PDS_CTL_H
#define __PDS_CTL_H 

#include <stdio.h>

#include <pds.h>
#include <pds_spi.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"pds_ctl"
#define VERSION		"Version 1.1"
#define CREATED		"Created on " __DATE__ " at " __TIME__

/******************************************************************************
* pds_ctl's command line arguments struct definition                          *
******************************************************************************/
typedef struct pds_ctl_args_rec
{
  char op;                        /* Operation */
  int oparg;                      /* Operation's argument */
  char *tagname;                  /* The tag name to apply operation to */
} pds_ctl_args;

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
int parse_pds_ctl_cmdln(int argc, char *argv[], pds_ctl_args *args);

#endif

