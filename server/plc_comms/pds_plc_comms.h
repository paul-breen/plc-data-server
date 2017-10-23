/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_plc_comms.h                                                   *
* PURPOSE:  Header file for the PLC comms functions module                    *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#ifndef __PDS_PLC_COMMS_H
#define __PDS_PLC_COMMS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/fcntl.h>
#include <termios.h>

/******************************************************************************
* Defines and macros                                                          *
******************************************************************************/

/* Defines for serial comms ascii control characters */
#define PDS_NUL			0x00
#define PDS_SOH			0x01
#define PDS_STX			0x02
#define PDS_ETX			0x03
#define PDS_EOT			0x04
#define PDS_ENQ			0x05
#define PDS_ACK			0x06
#define PDS_LF			0x0a
#define PDS_CR			0x0d
#define PDS_DLE			0x10
#define PDS_NAK			0x15
#define PDS_SYN			0x16

#define PDS_TTY_RD_PAUSE	5000

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to open a TCP/IP socket connection                                 *
*                                                                             *
* Pre-condition:  Host name (or IP address) and port number are passed to the *
*                 function                                                    *
* Post-condition: Socket connection is established with host, socket file     *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_plc_socket(char *host, unsigned short port);

/******************************************************************************
* Function to connect to PLC network socket                                   *
*                                                                             *
* Pre-condition:  Host info and port are passed to function                   *
* Post-condition: Socket file descriptor is returned or -1 on error           *
******************************************************************************/
int create_plc_network_socket(struct hostent *hostinfo, unsigned short port);

/******************************************************************************
* Function to open a serial TTY device port                                   *
*                                                                             *
* Pre-condition:  TTY device & a value/result struct containing the TTY       *
*                 device's desired settings are passed to the function        *
* Post-condition: TTY port is opened with the passed settings in tio.  The    *
*                 port's original settings are also stored & returned in tio. *
*                 Port file descriptor is returned or -1 on error             *
******************************************************************************/
int open_plc_tty(char *dev, struct termios *tio);

/******************************************************************************
* Function to close a serial TTY device port                                  *
*                                                                             *
* Pre-condition:  A valid TTY port file descriptor & the port's original      *
*                 settings are passed to the function                         *
* Post-condition: TTY port's settings are restored & the port is closed.  If  *
*                 an error occurrs a -1 is returned                           *
******************************************************************************/
int close_plc_tty(int ttyfd, struct termios *old);

/******************************************************************************
* Function to read data from a serial TTY device port                         *
*                                                                             *
* Pre-condition:  TTY device port fd, a buffer for storage and length of the  *
*                 buffer are passed to the function                           *
* Post-condition: Data is read from the port and stored in the buffer.  On    *
*                 error a -1 is returned                                      *
******************************************************************************/
int read_plc_tty(int ttyfd, unsigned char *buf, int blen);

/******************************************************************************
* Function to write data to a serial TTY device port                          *
*                                                                             *
* Pre-condition:  TTY device port fd, data to be written and length of data   *
*                 are passed to the function                                  *
* Post-condition: Data is written on the port.  On error a -1 is returned     *
******************************************************************************/
int write_plc_tty(int ttyfd, unsigned char *buf, int blen);

/******************************************************************************
* Function to double-stuff occurrences of a given byte in a byte array        *
*                                                                             *
* Pre-condition:  The original byte array, storage for the double-stuffed     *
*                 byte array, a pointer to the length of the original array   *
*                 and the byte to be stuffed in the array are passed to the   *
*                 function                                                    *
* Post-condition: If stuff_byte appears in the original array it is double    *
*                 -stuffed in the double-stuff array and the array length is  *
*                 incremented.  All other bytes are copied verbatim.  A count *
*                 of double-stuffed bytes is returned                         *
******************************************************************************/
int double_stuff_byte(unsigned char *before, unsigned char *after,
                      unsigned short int *len, unsigned char stuff_byte);

/******************************************************************************
* Function to remove double-stuffed occurrences of a given byte in a byte     *
* array                                                                       *
*                                                                             *
* Pre-condition:  The double-stuffed byte array, storage for the unstuffed    *
*                 byte array, a pointer to the length of the stuffed array    *
*                 and the byte to be removed in the array are passed to the   *
*                 function                                                    *
* Post-condition: If stuff_byte appears in the stuffed array twice adjacently *
*                 1 of them is removed from the unstuffed array and the array *
*                 length is decremented.  All other bytes are copied          *
*                 verbatim.  A count of removed bytes is returned             *
******************************************************************************/
int remove_double_stuff_byte(unsigned char *before, unsigned char *after,
                             unsigned short int *len,
                             unsigned char stuff_byte);

#endif

