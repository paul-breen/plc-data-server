/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   addrmm.h                                                          *
* PURPOSE:  Header file for addrmm.c                                          *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#ifndef __ADDRMM_H
#define __ADDRMM_H 

#include <stdio.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"addrmm"
#define VERSION		"Version 1.1"
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define STATUS_OPT	"-s"

#define UNDERLINE(c)\
{\
  int i = 0;\
  for(i = 0; i < c; i++) putchar('-');\
  putchar('\n');\
}

#endif

