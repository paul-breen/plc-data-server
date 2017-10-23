/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_plc_comms.c                                                   *
* PURPOSE:  The PLC comms functions module                                    *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#include "pds_plc_comms.h"

extern int errout;                /* Declared in the main file */

/******************************************************************************
* Function to open a TCP/IP socket connection                                 *
*                                                                             *
* Pre-condition:  Host name (or IP address) and port number are passed to the *
*                 function                                                    *
* Post-condition: Socket connection is established with host, socket file     *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_plc_socket(char *host, unsigned short port)
{
  int sockfd = -1;
  struct hostent *hostinfo = NULL;
  extern int h_errno;

  /* Create and connect to socket */
  if((sockfd = open_client_socket(host, port)) == -1)
  {
    err(errout, "error opening client socket to PLC '%s'\n", host);

    if(h_errno != 0)
    {
      err(errout, "error resolving host info for host '%s'\n", host);
    }
  }

  return sockfd;
}



/******************************************************************************
* Function to open a serial TTY device port                                   *
*                                                                             *
* Pre-condition:  TTY device & a value/result struct containing the TTY       *
*                 device's desired settings are passed to the function        *
* Post-condition: TTY port is opened with the passed settings in tio.  The    *
*                 port's original settings are also stored & returned in tio. *
*                 Port file descriptor is returned or -1 on error             *
******************************************************************************/
int open_plc_tty(char *dev, struct termios *tio)
{
  int ttyfd = -1;
  struct termios tio_copy;

  /* Save the passed comms parameters into a local copy.  This is because
     tio is used to return the current settings to the caller */
  memcpy(&tio_copy, tio, sizeof(struct termios));

  /* Open the TTY port for reading/writing */
  if((ttyfd = open(dev, O_RDWR)) < 0)
  {
    err(errout, "error opening device %s\n", dev);
    return -1; 
  }

  /* Setup the TTY port */
  if(ioctl(ttyfd, TIOCEXCL, 0) < 0)
  {
    err(errout, "error setting up device %s\n", dev);
    close(ttyfd);
    return -1; 
  }   

  /* Get the TTY port's current info struct (return to caller) */
  if(tcgetattr(ttyfd, tio) < 0)
  {
    err(errout, "error getting attributes of device %s\n", dev);
    close(ttyfd);
    return -1; 
  }   

  /* Set the TTY port's new info struct (comms parameters) */
  if(tcsetattr(ttyfd, TCSANOW, &tio_copy) < 0)
  {
    err(errout, "error setting attributes of device %s\n", dev);
    close(ttyfd);
    return -1; 
  }   

  return ttyfd;
}



/******************************************************************************
* Function to close a serial TTY device port                                  *
*                                                                             *
* Pre-condition:  A valid TTY port file descriptor & the port's original      *
*                 settings are passed to the function                         *
* Post-condition: TTY port's settings are restored & the port is closed.  If  *
*                 an error occurrs a -1 is returned                           *
******************************************************************************/
int close_plc_tty(int ttyfd, struct termios *old)
{
  /* Reset the TTY port's original info struct (comms parameters) */
  if(tcsetattr(ttyfd, TCSANOW, old) < 0)
  {
    err(errout, "error resetting attributes on ttyfd %d\n", ttyfd);
    close(ttyfd);
    return -1; 
  }   
  close(ttyfd);

  return 0;
}



/******************************************************************************
* Function to read data from a serial TTY device port                         *
*                                                                             *
* Pre-condition:  TTY device port fd, a buffer for storage and length of the  *
*                 buffer are passed to the function                           *
* Post-condition: Data is read from the port and stored in the buffer.  On    *
*                 error a -1 is returned                                      *
******************************************************************************/
int read_plc_tty(int ttyfd, unsigned char *buf, int blen)
{
  int ret = 1, len = 0, nread = 0;
   
  /* Check that data is available & that the buffer is not overflowed */
  while((ret > 0) && (len < blen))
  {
    /* Determine how many bytes are pending on the serial port */
    ioctl(ttyfd, FIONREAD, &nread);

    if(nread < 1)
      usleep(PDS_TTY_RD_PAUSE);

    /* Read a byte at a time from the serial port */
    ret = read(ttyfd, &buf[len], 1);

    if(ret > -1)
      len += ret;
    else
      len = ret;                  /* Set byte count to error */
  } 

  return len;
}



/******************************************************************************
* Function to write data to a serial TTY device port                          *
*                                                                             *
* Pre-condition:  TTY device port fd, data to be written and length of data   *
*                 are passed to the function                                  *
* Post-condition: Data is written on the port.  On error a -1 is returned     *
******************************************************************************/
int write_plc_tty(int ttyfd, unsigned char *buf, int blen)
{
  /* Write the data to the serial port */
  if(write(ttyfd, buf, blen) != blen)
  {
    return -1;
  } 
  tcdrain(ttyfd);                 /* Wait for all data to be written */

  return blen;
}



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
                      unsigned short int *len, unsigned char stuff_byte)
{
  unsigned short int i = 0, stuff_count = 0;

  for(i = 0; i < *len; i++)
  {
    if(*before != stuff_byte)
    {
      *after++ = *before++;       /* Copy straight into the new array */
    }
    else
    {
      *after++ = *before++;       /* Copy the byte then double up the */
      *after++ = stuff_byte;      /* next byte in the new array */
      stuff_count++;
    }
  }
  *len += stuff_count;

  return stuff_count;
}



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
                             unsigned char stuff_byte)
{
  unsigned short int i = 0, remove_count = 0;

  for(i = 0; i < *len; i++)
  {
    if(*before != stuff_byte)
    {
      *after++ = *before++;       /* Copy straight into the new array */
    }
    else
    {
      *after++ = *before++;       /* Copy the byte */

      if(*before == stuff_byte)
      {
        *before++;                /* Move pointer beyond double-stuffed byte */
        remove_count++;
      }
    }
  }
  *len -= remove_count;

  return remove_count;
}

