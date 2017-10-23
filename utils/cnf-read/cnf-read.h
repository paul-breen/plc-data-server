/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   cnf-read.h                                                        *
* PURPOSE:  Header file for cnf-read.c                                        *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#ifndef __CNF_READ_H
#define __CNF_READ_H 

#include <stdio.h>

#include <pds_plc_cnf.h>
#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"cnf-read"
#define VERSION		"Version 1.3"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define NCOLS		79

#define UNDERLINE(c, n)\
do\
{\
  int __i = 0;\
  for(__i = 0; __i < n; __i++) putchar(c);\
  putchar('\n');\
} while(0)

#define DASHEDLINE(c, n)\
do\
{\
  int __i = 0;\
  for(__i = 0; __i < n; __i += 2) putchar(c), putchar(' ');\
  putchar('\n');\
} while(0)

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to print the contents of a PLC cnf file to stdout                  *
*                                                                             *
* Pre-condition:  A PLC configuration structure is passed to the function     *
* Post-condition: The contents of the cnf file is printed on stdout           *
******************************************************************************/
int print_plc_cnf_data(plc_cnf *conf);

#endif

