/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   plcmm.h                                                           *
* PURPOSE:  Header file for plcmm.c                                           *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#ifndef __PLCMM_H
#define __PLCMM_H 

#include <stdio.h>

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME		"plcmm"
#define VERSION			"Version 1.3"
#define CREATED			"Created on " __DATE__ " at " __TIME__

#define DEFAULT_FIELDS		"ivr"
#define DEFAULT_SECTIONS	"hldsf"
#define DEFAULT_MAX_TAGLEN	20

#define UNDERLINE(c)\
{\
  int __i = 0;\
  for(__i = 0; __i < (c); __i++) putchar('-');\
  putchar('\n');\
}

#define CHECK_PRINT(s, c)	strchr((const char *) (s), (int) (c))

#define COL_SEP(l)		((l) > 0 ? '|' : ' ')
#define PRINT_EOL(f)		((f) > 0 ? "<br>" : "\n")

#define DATA_LEN                512

#define TMSTAMP_FMT     	"%Y-%m-%dT%H:%M:%S"
#define TMSTAMP_LEN     	25

/******************************************************************************
* plcmm's command line arguments struct definition                            *
******************************************************************************/
typedef struct plcmm_args_rec
{
  char fields[32];                /* The specifiers for output fields */
  char sections[32];              /* The specifiers for report sections */
  char cgi_mode;                  /* CGI runmode flag */
  unsigned char max_taglen;       /* Length of the longest tagname */
} plcmm_args;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int parse_plcmm_cmdln(int argc, char *argv[], plcmm_args *args);

/******************************************************************************
* Function to print the user specified field headers of a data tag            *
*                                                                             *
* Pre-condition:  An output stream & the cmdln args struct containing the     *
*                 field specifiers to be printed are passed to the function   *
* Post-condition: The data tags heading is printed to the output stream.      *
*                 The length of the header is returned or -1 on error         *
******************************************************************************/
int print_data_tag_headers(FILE *os, plcmm_args *args);

/******************************************************************************
* Callback function to print the user specified fields of a data tag          *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & the cmdln  *
*                 args struct cast as a (void *) are passed to the function.  *
*                 If the output stream is NULL, a default is used             *
* Post-condition: The specified fields of the tag (in the fields member of    *
*                 the args struct - aux) are printed to the output stream.    *
*                 On error a -1 is returned                                   *
******************************************************************************/
int print_data_tag(FILE *os, pdstag *tag, void *aux);

/******************************************************************************
* Function to print the user specified field headers of a status tag          *
*                                                                             *
* Pre-condition:  An output stream & the cmdln args struct containing the     *
*                 field specifiers to be printed are passed to the function   *
* Post-condition: The status tags heading is printed to the output stream.    *
*                 The length of the header is returned or -1 on error         *
******************************************************************************/
int print_status_tag_headers(FILE *os, plcmm_args *args);

/******************************************************************************
* Callback function to print the user specified fields of a status tag        *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & the cmdln  *
*                 args struct cast as a (void *) are passed to the function.  *
*                 If the output stream is NULL, a default is used             *
* Post-condition: The specified fields of the tag (in the fields member of    *
*                 the args struct - aux) are printed to the output stream.    *
*                 On error a -1 is returned                                   *
******************************************************************************/
int print_status_tag(FILE *os, pdstag *tag, void *aux);

/******************************************************************************
* Callback function to print shared memory statistics in a default manner     *
*                                                                             *
* Pre-condition:  An output stream, the connection struct containing summary  *
*                 data to be printed & any auxilliary data are passed to the  *
*                 function.  If the output stream is NULL, a default is used  *
* Post-condition: The statistics are printed to the output stream.  On error  *
*                 a -1 is returned                                            *
******************************************************************************/
int print_footer(FILE *os, pdsconn *conn, void *aux);

/******************************************************************************
* Callback function to calc. max. tagname length of all tags in shared mem.   *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be processed & the      *
*                 args struct cast as a (void *) are passed to the function.  *
*                 The output stream is not used                               *
* Post-condition: The tagname length of this tag is compared to the current   *
*                 max. tagname length & the longer of the two is stored       *
*                 appropriately in the args struct.  The length of the        *
*                 tagname is returned or -1 on error                          *
******************************************************************************/
int calc_max_taglen(FILE *os, pdstag *tag, void *aux);

#endif

