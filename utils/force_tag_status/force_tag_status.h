/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   force_tag_status.h                                                *
* PURPOSE:  Header file for force_tag_status.c                                *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-06-11                                                        *
******************************************************************************/

#ifndef __FORCE_TAG_STATUS_H
#define __FORCE_TAG_STATUS_H 

#include <stdio.h>
#include <signal.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"force_tag_status"
#define VERSION		"Version 1.0"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define FORCE_ON	0xffff         /* Force all bits on */
#define FORCE_OFF	0x0000         /* Force all bits off */
#define FORCE_HI_ON	0x8000         /* Force 16th bit on (32768) */
#define FORCE_HI_OFF	~0x8000        /* Force 16th bit off */

/******************************************************************************
* force_tag_status's command line arguments struct definition                 *
******************************************************************************/
typedef struct force_tag_status_args_rec
{
  char op;                        /* Operation */
  char *tagname;                  /* The tag name to apply operation to */
  unsigned short int tagvalue;    /* Operation's argument */
} force_tag_status_args;

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
int parse_force_tag_status_cmdln(int argc, char *argv[],
                                 force_tag_status_args *args);

#endif

