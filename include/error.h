/******************************************************************************
* PROJECT:  Error message handling library                                    * 
* MODULE:   error.h                                                           *
* PURPOSE:  Header file for the implementation file - error.c                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-19                                                        *
******************************************************************************/

#ifndef __ERROR_H
#define __ERROR_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define ERR_PRN			1      /* Output to stderr */
#define ERR_LOG			2      /* Output to log file */

#define ERR_FILENAME		"err.log"
#define ERR_FILEMODE		"a"

#define ERR_TMSTAMP_FMT		"%Y-%m-%dT%H:%M:%S"
#define ERR_TMSTAMP_LEN		25
#define ERR_BUF_LEN		1024

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to open an error message log file                                  *
*                                                                             *
* Pre-condition:  The name of the log file and it's open mode are passed to   *
*                 the function                                                *
* Post-condition: An attempt is made to open the log file.  If successful the *
*                 file handle is returned, if not a null is returned          *
******************************************************************************/
FILE* err_openlog(const char *filepath, const char *mode);

/******************************************************************************
* Function to close an error message log file                                 *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: The log file pointed to by the global error log file handle *
*                 is closed.  On error a -1 is returned                       *
******************************************************************************/
int err_closelog(void);

/******************************************************************************
* Function to write an error message to a log file                            *
*                                                                             *
* Pre-condition:  The file handle and the message to log are passed to the    *
*                 function                                                    *
* Post-condition: An attempt is made to write the log file.  On error a -1 is *
*                 returned                                                    *
******************************************************************************/
int err_writelog(FILE *fp, const char *msg);

/******************************************************************************
* Function to read an error message from a log file                           *
*                                                                             *
* Pre-condition:  The file handle, storage for the message and the length of  *
*                 the message string are passed to the function               *
* Post-condition: An attempt is made to read the log file.  On error a -1 is  *
*                 returned                                                    *
******************************************************************************/
int err_readlog(FILE *fp, char *msg, size_t msglen);

/******************************************************************************
* Function to print to stderr OR log to file, an error message                *
*                                                                             *
* Pre-condition:  An integer to indicate the destination of this message,     *
*                 standard printf format string and variable arguments are    *
*                 passed to the function                                      *
* Post-condition: An attempt is made to write the message to the specified    *
*                 destination.  On error a -1 is returned                     *
******************************************************************************/
int err(int msgdest, const char *fmt, ...);

/******************************************************************************
* Function to return a timestamp string                                       *
*                                                                             *
* Pre-condition:  A string pointer for storage of the timestamp is passed to  *
*                 the function                                                *
* Post-condition: String is filled with current date/time evaluated in the    *
*                 local timezone, and formatted as ERR_TMSTAMP_FMT.  Pointer  *
*                 to timestamp is returned                                    *
******************************************************************************/
char* err_timestamp(char *str);

#endif

