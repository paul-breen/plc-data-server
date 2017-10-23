/******************************************************************************
* PROJECT:  Network comms library                                             *
* MODULE:   nw_comms.c                                                        *
* PURPOSE:  Header file for nw_comms.c                                        * 
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#ifndef __NW_COMMS_H
#define __NW_COMMS_H

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

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define NW_COMMS_TMO_SECS	15
#define NW_COMMS_TMO_USECS	0

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to open a TCP/IP client socket connection                          *
*                                                                             *
* Pre-condition:  The server host name (or IP address) and port number are    *
*                 passed to the function                                      *
* Post-condition: Socket connection is established with server, socket file   *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_client_socket(char *host, unsigned short int port);

/******************************************************************************
* Function to open a TCP/IP server socket connection                          *
*                                                                             *
* Pre-condition:  The server host name (or IP address) and port number are    *
*                 passed to the function                                      *
* Post-condition: Socket is bound to the server address, socket file          *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_server_socket(char *host, unsigned short int port);

/******************************************************************************
* Function to connect client to network server socket                         *
*                                                                             *
* Pre-condition:  A valid server host info struct and server port are passed  *
*                 to the function                                             *
* Post-condition: A connected socket file descriptor to the server or a -1 on *
*                 error is returned                                           *
******************************************************************************/
int create_client_network_socket(struct hostent *hostinfo,
                                 unsigned short int port);

/******************************************************************************
* Function to create and name a network server socket                         *
*                                                                             *
* Pre-condition:  A valid server host info struct and server port are passed  *
*                 to the function                                             *
* Post-condition: A named socket file descriptor for the server or a -1 on    *
*                 error is returned                                           *
******************************************************************************/
int create_server_network_socket(struct hostent *hostinfo,
                                 unsigned short int port);

/******************************************************************************
* Function to display host information on stdout                              *
*                                                                             *
* Pre-condition:  Host info struct is passed to the function                  *
* Post-condition: Contents of the host info struct are displayed on stdout    *
******************************************************************************/
void display_hostinfo(struct hostent *hostinfo);

/******************************************************************************
* Function to read data on a particular socket fd                             *
*                                                                             *
* Pre-condition:  Socket fd, a valid buffer, and the length to read are       *
*                 passed to the function                                      *
* Post-condition: Data is stored in buffer, and the number of bytes read is   *
*                 returned or -1 on error                                     *
******************************************************************************/
int socket_read(int fd, unsigned char *buf, long int len);

/******************************************************************************
* Function to write data on a particular socket fd                            *
*                                                                             *
* Pre-condition:  Socket fd, a valid buffer, and the length to write are      *
*                 passed to the function                                      *
* Post-condition: Data from buffer is written on the socket, and the number   *
*                 of bytes written is returned or -1 on error                 *
******************************************************************************/
int socket_write(int fd, unsigned char *buf, long int len);

#endif

