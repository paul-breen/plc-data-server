/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pdsnp_comms.c                                                     *
* PURPOSE:  The PDS network protocol comms module                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#include "pdsnp_comms.h"

/******************************************************************************
* Function to read comms data on a particular socket fd                       *
*                                                                             *
*                                                                             *
* Pre-condition:  Socket fd and comms struct pointer are passed to the        *
*                 function                                                    *
* Post-condition: Data is stored in structure, and the number of bytes read   *
*                 is returned or -1 on error                                  *
******************************************************************************/
int comms_read(int fd, pdscomms *comms)
{
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  tv.tv_sec = PDSNP_TMO_SECS;
  tv.tv_usec = PDSNP_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the socket is ready for reading and peek at the message
     length byte */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
    return -1;
  else
    recv(fd, comms->buf, 1, MSG_PEEK);

  /* Check that the socket is ready for reading and receive the data */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
    return -1;
  else
    return recv(fd, comms->buf, PDSNP_GET_BUF_LEN(comms->buf), 0);
}



/******************************************************************************
* Function to write comms data on a particular socket fd                      *
*                                                                             *
* Pre-condition:  Socket fd and comms struct pointer are passed to the        *
*                 function                                                    *
* Post-condition: Data is written on the socket, and the number of bytes      *
*                 written is returned or -1 on error                          *
******************************************************************************/
int comms_write(int fd, pdscomms *comms)
{
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  tv.tv_sec = PDSNP_TMO_SECS;
  tv.tv_usec = PDSNP_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the socket is ready for writing and send the data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
    return -1;
  else
    return send(fd, comms->buf, PDSNP_GET_BUF_LEN(comms->buf), 0);
}

