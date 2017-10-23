/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pdsnp_comms.h                                                     *
* PURPOSE:  Header file for the PDS network protocol comms module             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#ifndef __PDSNP_COMMS_H
#define __PDSNP_COMMS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <pdsnp_defs.h>

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to read comms data on a particular socket fd                       *
*                                                                             *
*                                                                             *
* Pre-condition:  Socket fd and comms struct pointer are passed to the        *
*                 function                                                    *
* Post-condition: Data is stored in structure, and the number of bytes read   *
*                 is returned or -1 on error                                  *
******************************************************************************/
int comms_read(int fd, pdscomms *comms);

/******************************************************************************
* Function to write comms data on a particular socket fd                      *
*                                                                             *
* Pre-condition:  Socket fd and comms struct pointer are passed to the        *
*                 function                                                    *
* Post-condition: Data is written on the socket, and the number of bytes      *
*                 written is returned or -1 on error                          *
******************************************************************************/
int comms_write(int fd, pdscomms *comms);

#endif

