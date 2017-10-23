/******************************************************************************
* PROJECT:  Debug library                                                     *
* MODULE:   debug.h                                                           *
* PURPOSE:  Header file for debug library code functions                      *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-03-23                                                        *
******************************************************************************/

#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

extern int __dbgflag;             /* Declared in the debug module */
extern int __dbglvl;              /* Declared in the debug module */

/******************************************************************************
* Defines & macros                                                            *
******************************************************************************/
#define DBG_MAXLEVELS	4

/* Macros to act as accessors for the debug flag & level */
#define GET_DBG_FLAG		(__dbgflag)
#define GET_DBG_LEVEL		(__dbglvl)
#define SET_DBG_FLAG(f)		(__dbgflag = (f))
#define SET_DBG_LEVEL(l)	(__dbglvl = (l))

/* Outputs string s dependent on debug level */
#define dbgmsg1(s)	if(__dbgflag && __dbglvl == 1) (fputs((s), stdout))
#define dbgmsg2(s)	if(__dbgflag && __dbglvl == 2) (fputs((s), stdout))
#define dbgmsg3(s)	if(__dbgflag && __dbglvl == 3) (fputs((s), stdout))
#define dbgmsg4(s)	if(__dbgflag && __dbglvl == 4) (fputs((s), stdout))

/* Variable argument messages dependent on debug level */
#define printd1		if(__dbgflag && __dbglvl == 1) (printf)
#define printd2		if(__dbgflag && __dbglvl == 2) (printf)
#define printd3		if(__dbgflag && __dbglvl == 3) (printf)
#define printd4		if(__dbgflag && __dbglvl == 4) (printf)

/* Debug messages independent of debug level */
#define dbgmsg(s)	if(__dbgflag) (fputs((s), stdout))
#define printd		if(__dbgflag) (printf)

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to set debug options                                               *
*                                                                             *
* Pre-condition:  Debug on/off switch and level are passed to the function    *
* Post-condition: Program output is handled accordingly.  Debug level is      *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_options(int dbgswitch, int dbglevel);

/******************************************************************************
* Function to set debug on/off                                                *
*                                                                             *
* Pre-condition:  Debug on/off switch is passed to the function               *
* Post-condition: Program output is handled accordingly.  Debug flag is       *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_switch(int dbgswitch);

/******************************************************************************
* Function to set debug level                                                 *
*                                                                             *
* Pre-condition:  Debug level is passed to the function                       *
* Post-condition: Program output is handled accordingly.  Debug level is      *
*                 returned or -1 on error                                     *
******************************************************************************/
int set_debug_level(int dbglevel);

#endif

