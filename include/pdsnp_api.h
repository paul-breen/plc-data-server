/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pdsnp_api.h                                                       *
* PURPOSE:  Header file for the PDS network protocol API module               *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#ifndef __PDSNP_API_H
#define __PDSNP_API_H

#include <pds.h>
#include <pdsnp_defs.h>

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to connect a client to the server                                  *
*                                                                             *
* Pre-condition:  A server host & port are passed to the function.  If the    *
*                 host is NULL a local connection is made, else a network     *
*                 connection is made                                          *
* Post-condition: The client program is connected to the server which is      *
*                 identified by the host & port.  The server connection       *
*                 structure is returned which is used in all subsequent       *
*                 calls to the server.  On return the connection structure    *
*                 should be interrogated to determine if the connection was   *
*                 successful or if an error occurred.  If memory cannot be    *
*                 allocated for the connection structure a null pointer is    *
*                 returned                                                    *
******************************************************************************/
pdsconn* PDSNPconnect(const char *host, unsigned short int port);

/******************************************************************************
* Function to disconnect a client from the server                             *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurrs a -1 is returned                              *
******************************************************************************/
int PDSNPdisconnect(pdsconn *conn);

/******************************************************************************
* Function to get a tag's value                                               *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and a string for     *
*                 storage of the tag's value are passed to the function       *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int PDSNPget_tag(pdsconn *conn, const char *tagname, char *tagvalue);

/******************************************************************************
* Function to set tag value(s)                                                *
*                                                                             *
* Pre-condition:  A valid server connection, the base tagname, the no. of     *
*                 tags to write and the tag value(s) as an integer pointer    *
*                 are passed to the function                                  *
* Post-condition: A message is sent to the server requesting that the tag     *
*                 value(s) be written to the PLC.  On error a -1 is returned  *
******************************************************************************/
int PDSNPset_tag(pdsconn *conn, const char *tagname, short int ntags,
                 const short int *tagvalues);

#endif

