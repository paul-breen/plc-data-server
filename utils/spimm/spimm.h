/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   spimm.h                                                           *
* PURPOSE:  Header file for spimm.c                                           *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-02                                                        *
******************************************************************************/

#ifndef __SPIMM_H
#define __SPIMM_H 

#include <stdio.h>

#include <pds.h>
#include <pds_spi.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"spimm"
#define VERSION		"Version 1.1"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define UNDERLINE(c)\
{\
  int i = 0;\
  for(i = 0; i < c; i++) putchar('-');\
  putchar('\n');\
}

#endif

