/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_api.h                                                         *
* PURPOSE:  Header file for the PLC data server C API module                  *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-09-23                                                        *
******************************************************************************/

#ifndef __PDS_API_H
#define __PDS_API_H

#include <pds_defs.h>
#include <pds_ipc.h>
#include <pds_utils.h>
#include <pds_protocols.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PDS_MAX_NTRYS		3
#define PDS_CONN_PAUSE		100000

#define PDS_DEFAULT_PRINT_DATA_TAG_FUNC		_default_print_data_tag
#define PDS_DEFAULT_PRINT_STATUS_TAG_FUNC	_default_print_status_tag
#define PDS_DEFAULT_PRINT_FOOTER_FUNC		_default_print_footer
#define PDS_DEFAULT_PRINT_DATA_TAG_STREAM	stdout
#define PDS_DEFAULT_PRINT_STATUS_TAG_STREAM	stdout
#define PDS_DEFAULT_PRINT_FOOTER_STREAM		stdout

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Internal function to get a string of tags' value (formatted)                *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname (base), the number   *
*                 of tagvalues to read, a string for storage of the tags'     *
*                 value and a data format specifier are passed to the         *
*                 function.  The semaphore should be held before calling      *
* Post-condition: The tagnames are accessed in the shared memory segment and  *
*                 their value's as a string are returned formatted as the     *
*                 specified type.  On error a -1 is returned                  *
******************************************************************************/
/* static int _get_strtagf(pdsconn *conn, const char *tagname, int ntags,
                        char *tagvalue, const char fmt); */

/******************************************************************************
* Internal function to set the value of a semaphore                           *
*                                                                             *
* Pre-condition:  A valid semaphore ID, the value for the operation and the   *
*                 semaphore set array number are passed to the function       *
* Post-condition: The semaphore is set with the passed value.  If an error    *
*                 occurs a -1 is returned                                     *
******************************************************************************/
/* static int _semset(int id, int op, int snum); */

/******************************************************************************
* Function to connect a client to the server                                  *
*                                                                             *
* Pre-condition:  A server connection key is passed to the function.  If the  *
*                 connection key is NULL or zero, then a default is used      *
* Post-condition: The client program is connected to the server which is      *
*                 identified by the connection key.  The server connection    *
*                 structure is returned which is used in all subsequent       *
*                 calls to the server.  On return the connection structure    *
*                 should be interrogated to determine if the connection was   *
*                 successful or if an error occurred.  If memory cannot be    *
*                 allocated for the connection structure a null pointer is    *
*                 returned                                                    *
******************************************************************************/
pdsconn* PDSconnect(key_t connkey);

/******************************************************************************
* Function to disconnect a client from the server                             *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int PDSdisconnect(pdsconn *conn);

/******************************************************************************
* Function to get a tag's value                                               *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and a string for     *
*                 storage of the tag's value are passed to the function       *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int PDSget_tag(pdsconn *conn, const char *tagname, char *tagvalue);

/******************************************************************************
* Function to get a tag's value (formatted)                                   *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname, a string for        *
*                 storage of the tag's value and a data format specifier      *
*                 are passed to the function                                  *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned formatted as the        *
*                 specified type.  On error a -1 is returned                  *
******************************************************************************/
int PDSget_tagf(pdsconn *conn, const char *tagname, char *tagvalue,
                const char fmt);

/******************************************************************************
* Function to get a string of tags' value (formatted)                         *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname (base), the number   *
*                 of tagvalues to read, a string for storage of the tags'     *
*                 value and a data format specifier are passed to the         *
*                 function                                                    *
* Post-condition: The tagnames are accessed in the shared memory segment and  *
*                 their value's as a string are returned formatted as the     *
*                 specified type.  On error a -1 is returned                  *
******************************************************************************/
int PDSget_strtag(pdsconn *conn, const char *tagname, int ntags,
                  char *tagvalue, const char fmt);

/******************************************************************************
* Function to set tag value(s)                                                *
*                                                                             *
* Pre-condition:  A valid server connection, the base tagname, the no. of     *
*                 tags to write and the tag value(s) as an integer pointer    *
*                 are passed to the function                                  *
* Post-condition: A message is sent to the server requesting that the tag     *
*                 value(s) be written to the PLC.  On error a -1 is returned  *
******************************************************************************/
int PDSset_tag(pdsconn *conn, const char *tagname, short int ntags,
               const unsigned short int *tagvalues);

/******************************************************************************
* Function to set a string of tags' value                                     *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname (base) and the tags' *
*                 value represented as a string are passed to the function    *
* Post-condition: A message is sent to the server requesting that the tag     *
*                 value be written to the PLC.  On error a -1 is returned     *
******************************************************************************/
int PDSset_strtag(pdsconn *conn, const char *tagname, const char *tagvalue);

/******************************************************************************
* Function to get a flag's state                                              *
*                                                                             *
* Pre-condition:  A valid server connection and the tagname of the flag to    *
*                 get are passed to the function                              *
* Post-condition: The flag's value is returned.  On error a -1 is returned    *
******************************************************************************/
int PDSget_flag_state(pdsconn *conn, char *tagname);

/******************************************************************************
* Function to set a flag's state                                              *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname of the flag to set   *
*                 and the value to set are passed to the function             *
* Post-condition: The flag's value is set.  On error a -1 is returned         *
******************************************************************************/
int PDSset_flag_state(pdsconn *conn, char *tagname,
                      unsigned short int tagvalue);

/******************************************************************************
* Function to get tag data from the PLC                                       *
*                                                                             *
* Pre-condition:  A valid server connection and a PLC taglist structure       *
*                 containing an array of tag structs to get data for are      *
*                 passed to the function                                      *
* Post-condition: The tags' data are read into the value strings of the       *
*                 tag structs.  A count is returned of number of tags read    *
*                 from the PLC.  On error a -1 is returned                    *
******************************************************************************/
int PDSget_taglist(pdsconn *conn, plctaglist *taglist);

/******************************************************************************
* Function to get a bit's state within a word                                 *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and the bit number   *
*                 within the word (0-15) to get are passed to the function    *
* Post-condition: The bit's value is returned.  On error a -1 is returned     *
******************************************************************************/
int PDSget_wordbit_state(pdsconn *conn, char *tagname, int bitno);

/******************************************************************************
* Function to set a bit's state within a word                                 *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname, the tagvalue, the   *
*                 bit number within the word (0-15) to set and the bit value  *
*                 to set the bit to are passed to the function                *
* Post-condition: The tag value is modified with the bit value and the tag is *
*                 set.  On error a -1 is returned                             *
******************************************************************************/
int PDSset_wordbit_state(pdsconn *conn, char *tagname,
                         unsigned short int *tagvalue, int bitno,
                         int bitvalue);

/******************************************************************************
* Function to get a tag's PLC status                                          *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and a pointer to     *
*                 store the tag's status are passed to the function           *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its status is stored in status.  On error a -1 is returned  *
******************************************************************************/
int PDSget_tag_status(pdsconn *conn, const char *tagname,
                      unsigned short int *status);

/******************************************************************************
* Function to print a complete memory map of data tags in shared memory       *
*                                                                             *
* Pre-condition:  An output stream, valid connection struct, a callback       *
*                 function to handle output of the tags & any auxilliary data *
*                 are passed to the function.  If any of the parameters       *
*                 (except 'conn') are NULL, defaults are used                 *
* Post-condition: The shared memory is cycled through & each tag, along with  *
*                 the output stream & any auxilliary data, is passed to the   *
*                 function pointed to by 'func'.  On error a -1 is returned   *
******************************************************************************/
int PDSprint_data_memmap(FILE *os, pdsconn *conn,
                         int (*func)(FILE *, pdstag *, void *), void *aux);

/******************************************************************************
* Function to print a complete memory map of status tags in shared memory     *
*                                                                             *
* Pre-condition:  An output stream, valid connection struct, a callback       *
*                 function to handle output of the tags & any auxilliary data *
*                 are passed to the function.  If any of the parameters       *
*                 (except 'conn') are NULL, defaults are used                 *
* Post-condition: The shared memory is cycled through & each tag, along with  *
*                 the output stream & any auxilliary data, is passed to the   *
*                 function pointed to by 'func'.  On error a -1 is returned   *
******************************************************************************/
int PDSprint_status_memmap(FILE *os, pdsconn *conn,
                           int (*func)(FILE *, pdstag *, void *), void *aux);

/******************************************************************************
* Function to print a summary footer for the memory map of shared memory      *
*                                                                             *
* Pre-condition:  An output stream, valid connection struct, a callback       *
*                 function to handle output of the data & any auxilliary data *
*                 are passed to the function.  If any of the parameters       *
*                 (except 'conn') are NULL, defaults are used                 *
* Post-condition: The connection struct (containing summary data), along with *
*                 the output stream & any auxilliary data, is passed to the   *
*                 function pointed to by 'func'.  On error a -1 is returned   *
******************************************************************************/
int PDSprint_footer_memmap(FILE *os, pdsconn *conn,
                           int (*func)(FILE *, pdsconn *, void *), void *aux);

/******************************************************************************
* Internal function to print a data tag's data in a default manner            *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & any        *
*                 auxilliary data are passed to the function.  If the output  *
*                 stream is NULL, a default is used                           *
* Post-condition: The tag's data is printed to the output stream.  The        *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_data_tag(FILE *os, pdstag *tag, void *aux);

/******************************************************************************
* Internal function to print a status tag's data in a default manner          *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & any        *
*                 auxilliary data are passed to the function.  If the output  *
*                 stream is NULL, a default is used                           *
* Post-condition: The tag's data is printed to the output stream.  The        *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_status_tag(FILE *os, pdstag *tag, void *aux);

/******************************************************************************
* Internal function to print shared memory statistics in a default manner     *
*                                                                             *
* Pre-condition:  An output stream, the connection struct containing summary  *
*                 data to be printed & any auxilliary data are passed to the  *
*                 function.  If the output stream is NULL, a default is used  *
* Post-condition: The statistics are printed to the output stream.  The       *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_footer(FILE *os, pdsconn *conn, void *aux);

/******************************************************************************
* Function to get a tag object                                                *
*                                                                             *
* Pre-condition:  A valid server connection & the tagname to search for are   *
*                 passed to the function                                      *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 a pointer to the tag object is returned.  On error a NULL   *
*                 is returned                                                 *
******************************************************************************/
pdstag* PDSget_tag_object(pdsconn *conn, const char *tagname);

#endif

