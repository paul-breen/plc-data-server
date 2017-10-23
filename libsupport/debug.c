/******************************************************************************
* PROJECT:  Debug library                                                     *
* MODULE:   debug.c                                                           *
* PURPOSE:  Debug library code                                                *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-03-23                                                        *
******************************************************************************/

#include "debug.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
int __dbgflag = 0;                /* Debug flag */
int __dbglvl = 0;                 /* Debug level */

/******************************************************************************
* Function to set debug options                                               *
*                                                                             *
* Pre-condition:  Debug on/off switch and level are passed to the function    *
* Post-condition: Program output is handled accordingly.  Debug level is      *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_options(int dbgswitch, int dbglevel)
{
  int retval = 0;

  if((retval = set_debug_switch(dbgswitch)) != -1)
    retval = set_debug_level(dbglevel);

  return retval;
}


 
/******************************************************************************
* Function to set debug on/off                                                *
*                                                                             *
* Pre-condition:  Debug on/off switch is passed to the function               *
* Post-condition: Program output is handled accordingly.  Debug flag is       *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_switch(int dbgswitch)
{
  if(dbgswitch)
    __dbgflag = 1;
  else
    __dbgflag = 0;

  return __dbgflag;
}



/******************************************************************************
* Function to set debug level                                                 *
*                                                                             *
* Pre-condition:  Debug level is passed to the function                       *
* Post-condition: Program output is handled accordingly.  Debug level is      *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_level(int dbglevel)
{
  if((dbglevel > 0) && (dbglevel <= DBG_MAXLEVELS))
    __dbglvl = dbglevel;
  else
    __dbglvl = -1;

  return __dbglvl;
}

