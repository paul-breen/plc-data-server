/******************************************************************************
* PROJECT:  Error message handling library                                    * 
* MODULE:   error.c                                                           *
* PURPOSE:  Library of functions to handle output/logging of error messages   *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-19                                                        *
******************************************************************************/

#include "error.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
static FILE *err_fp = (FILE *) 0;      /* Global error log file handle */

/******************************************************************************
* Function to open an error message log file                                  *
*                                                                             *
* Pre-condition:  The name of the log file and it's open mode are passed to   *
*                 the function                                                *
* Post-condition: An attempt is made to open the log file.  If successful the *
*                 file handle is returned, if not a null is returned          *
******************************************************************************/
FILE* err_openlog(const char *filepath, const char *mode)
{
  if(!(err_fp = fopen(filepath, mode)))
  {
    return (FILE *) NULL;
  }
  setbuf(err_fp, NULL);           /* Make all logged output unbuffered */

  return err_fp;
}



/******************************************************************************
* Function to close an error message log file                                 *
*                                                                             *
* Pre-condition:  The function is called                                      *
* Post-condition: The log file pointed to by the global error log file handle *
*                 is closed.  On error a -1 is returned                       *
******************************************************************************/
int err_closelog(void)
{
  return fclose(err_fp);
}



/******************************************************************************
* Function to write an error message to a log file                            *
*                                                                             *
* Pre-condition:  The file handle and the message to log are passed to the    *
*                 function                                                    *
* Post-condition: An attempt is made to write the log file.  On error a -1 is *
*                 returned                                                    *
******************************************************************************/
int err_writelog(FILE *fp, const char *msg)
{
  if(fwrite(msg, strlen(msg), 1, fp) < 1)
    return -1;
  else
    return 0;
}



/******************************************************************************
* Function to read an error message from a log file                           *
*                                                                             *
* Pre-condition:  The file handle, storage for the message and the length of  *
*                 the message string are passed to the function               *
* Post-condition: An attempt is made to read the log file.  On error a -1 is  *
*                 returned                                                    *
******************************************************************************/
int err_readlog(FILE *fp, char *msg, size_t msglen)
{
  if(fread(msg, msglen, 1, fp) < 1)
    return -1;
  else
    return 0;
}



/******************************************************************************
* Function to print to stderr OR log to file, an error message                *
*                                                                             *
* Pre-condition:  An integer to indicate the destination of this message,     *
*                 standard printf format string and variable arguments are    *
*                 passed to the function                                      *
* Post-condition: An attempt is made to write the message to the specified    *
*                 destination.  On error a -1 is returned                     *
******************************************************************************/
int err(int msgdest, const char *fmt, ...)
{
  int i = 0, retval = -1;
  char string[ERR_BUF_LEN+1] = "\0", timestamp[ERR_TMSTAMP_LEN+1] = "\0";
  va_list ap;

  /* Copy variable arg list to string using format */
  va_start(ap, fmt);
  i = vsnprintf(string, ERR_BUF_LEN, fmt, ap);
  va_end(ap);
  
  switch(msgdest)
  {
    case ERR_PRN :                /* Write message to stderr (default) */
    default :
      retval = fputs(string, stderr); 
    break;

    case ERR_LOG :                /* Write timestamped message to log file */
      if(!err_fp)
      {
        /* Open log if not already open */
        if(!(err_fp = err_openlog(ERR_FILENAME, ERR_FILEMODE)))
        {
          retval = -1;
        }
        else
        {
          retval = err_writelog(err_fp, err_timestamp(timestamp));

          if(retval != -1)
            retval = err_writelog(err_fp, string);
        }
      }
      else
      {
        retval = err_writelog(err_fp, err_timestamp(timestamp));

        if(retval != -1)
          retval = err_writelog(err_fp, string);
      }
    break;
  }

  return retval;
}



/******************************************************************************
* Function to return a timestamp string                                       *
*                                                                             *
* Pre-condition:  A string pointer for storage of the timestamp is passed to  *
*                 the function                                                *
* Post-condition: String is filled with current date/time evaluated in the    *
*                 local timezone, and formatted as ERR_TMSTAMP_FMT.  Pointer  *
*                 to timestamp is returned                                    *
******************************************************************************/
char* err_timestamp(char *str)
{
  time_t clock = 0;

  clock = time(NULL);
  strftime(str, ERR_TMSTAMP_LEN, ERR_TMSTAMP_FMT " ", localtime(&clock)); 

  return str;
}

