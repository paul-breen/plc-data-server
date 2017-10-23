/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_dh.c                                                          *
* PURPOSE:  The DataHighway specific functions module                         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#include "pds_dh.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
static unsigned char __dh_ack_seq[DH_ACK_LEN] = {PDS_DLE, PDS_ACK};
static unsigned short int __dh_trans_id = 1;
static unsigned short int __dh_timedout = 0;

/******************************************************************************
* Function to setup a read PLC query using the configuration file parameters  *
*                                                                             *
* Pre-condition:  Storage for this block's query and the block structure are  *
*                 passed to the function                                      *
* Post-condition: This block's configuration parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int dh_setup_read_query(unsigned char *query, plc_cnf_block *block)
{
  unsigned char addr[DH_PLC_ADDR_LEN] = "\0";
  unsigned char stn = 0, cmd = 0, fnc = 0, size = 0;
  unsigned short int tns = 0, offset = 0, ttrans = 0, addrlen = 0;
  short int qlen = 0;

  memset(&addr, 0, DH_PLC_ADDR_LEN);

  stn = block->path[0];
  cmd = DH_GET_CMD(block->function);
  fnc = DH_GET_FUNC(block->function);
  addrlen = strlen(block->ascii_addr);
  memcpy(&addr, &block->ascii_addr, addrlen);
  ttrans = DH_HI_REF(0, block->tags[block->ntags-1].ref);
  size = (ttrans * DH_WORDSIZE);

  /* Construct the PLC query to read this block */
  if((qlen = dh_construct_read_plc_query(query, stn, cmd, tns, fnc, offset, ttrans, addr, addrlen, size)) == -1)
  {
    err(errout, "%s: error constructing read query\n", PROGNAME);
  }

  return qlen;
}



/******************************************************************************
* Function to setup a write PLC query using the tag & message parameters      *
*                                                                             *
* Pre-condition:  Storage for the query, the tag struct containing the PLC    *
*                 address to write to and the message struct containing the   *
*                 data to be written are passed to the function               *
* Post-condition: The tag & message struct's parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int dh_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg)
{
  unsigned char addr[DH_PLC_ADDR_LEN] = "\0";
  unsigned char stn = 0, cmd = 0, fnc = 0;
  unsigned short int tns = 0, offset = 0, ttrans = 0, addrlen = 0;
  short int qlen = 0;

  memset(&addr, 0, DH_PLC_ADDR_LEN);
 
  stn = tag->path[0];
  cmd = DH_GET_CMD(tag->function);
  fnc = DH_GET_FUNC(tag->function);
  addrlen = strlen(tag->ascii_addr);
  memcpy(&addr, &tag->ascii_addr, addrlen);

  /* Construct the PLC query to write to this tag(s) */
  if((qlen = dh_construct_write_plc_query(query, stn, cmd, tns, fnc, offset, ttrans, addr, addrlen, msg->tagvalues, msg->ntags)) == -1)
  {
    err(errout, "%s: error constructing write query\n", PROGNAME);
  }

  return qlen;
}



/******************************************************************************
* Function to setup a status PLC query using the connection parameters        *
*                                                                             *
* Pre-condition:  Storage for the query & the connection struct containing    *
*                 the PLC connection parameters are passed to the function    *
* Post-condition: The connection struct's parameters are used to construct a  *
*                 generic PLC status query.  The length of the query is       *
*                 returned or -1 if an error occurs                           *
******************************************************************************/
int dh_setup_status_query(unsigned char *query, pdsconn *conn)
{
  char data[DH_PLC_STAT_LEN] = "\0";
  unsigned char stn = 0, cmd = 0, fnc = 0;
  unsigned short int tns = 0, datalen = 0;
  short int qlen = 0;

  memset(&data, 0, DH_PLC_STAT_LEN);

  stn = conn->path[0];
  cmd = PDS_GETHIBYTE(DH_ECHO_STAT);
  fnc = PDS_GETLOBYTE(DH_ECHO_STAT);
  sprintf(data, DH_PLC_STAT_FORMAT, DH_PLC_STAT_TEXT, __dh_trans_id);
  datalen = strlen(data);

  /* Construct the PLC query to get the status of this PLC */
  if((qlen = dh_construct_status_plc_query(query, stn, cmd, tns, fnc, (unsigned char *) data, datalen)) == -1)
  {
    err(errout, "%s: error constructing status query\n", PROGNAME);
  }

  return qlen;
}



/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, offset, no. of words   *
*                 to read, PLC address & length and the size in bytes of data *
*                 to read are passed to the function                          *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_read_plc_query(unsigned char *query, unsigned char stn,
                                unsigned char cmd, unsigned short int tns,
                                unsigned char fnc, unsigned short int offset,
                                unsigned short int ttrans, unsigned char *addr,
                                unsigned short int addrlen,
                                unsigned char size)
{
  unsigned short int i = 0, j = 0;

  query[i++] = stn;                         /* Transport independent query */
  query[i++] = 0x00;

  query[i++] = cmd;
  query[i++] = 0x00;

  query[i++] = PDS_GETLOBYTE(tns);
  query[i++] = PDS_GETHIBYTE(tns);

  query[i++] = fnc;

  query[i++] = PDS_GETLOBYTE(offset);
  query[i++] = PDS_GETHIBYTE(offset);

  query[i++] = PDS_GETLOBYTE(ttrans);
  query[i++] = PDS_GETHIBYTE(ttrans);

  query[i++] = 0x00;

  for(j = 0; j < addrlen; j++)
    query[i++] = addr[j];

  query[i++] = 0x00;

  query[i++] = size;

  return i;
}



/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, offset, no. of words   *
*                 to write, PLC address & length, a pointer to the value(s)   *
*                 containing the data to write and the no. of words to write  *
*                 are passed to the function                                  *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_write_plc_query(unsigned char *query, unsigned char stn,
                                 unsigned char cmd, unsigned short int tns,
                                 unsigned char fnc, unsigned short int offset,
                                 unsigned short int ttrans,
                                 unsigned char *addr,
                                 unsigned short int addrlen,
                                 unsigned short int *values,
                                 unsigned short int nvalues)
{
  unsigned short int i = 0, j = 0;

  query[i++] = stn;                         /* Transport independent query */
  query[i++] = 0x00;

  query[i++] = cmd;
  query[i++] = 0x00;

  query[i++] = PDS_GETLOBYTE(tns);
  query[i++] = PDS_GETHIBYTE(tns);

  query[i++] = fnc;

  query[i++] = PDS_GETLOBYTE(offset);
  query[i++] = PDS_GETHIBYTE(offset);

  query[i++] = PDS_GETLOBYTE(ttrans);
  query[i++] = PDS_GETHIBYTE(ttrans);

  query[i++] = 0x00;

  for(j = 0; j < addrlen; j++)
    query[i++] = addr[j];

  query[i++] = 0x00;

  for(j = 0; j < nvalues; j++)
  {
    query[i++] = PDS_GETLOBYTE(values[j]);
    query[i++] = PDS_GETHIBYTE(values[j]);
  }

  return i;
}



/******************************************************************************
* Function to construct a status PLC query                                    *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, data & data length are *
*                 passed to the function                                      *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_status_plc_query(unsigned char *query, unsigned char stn,
                                  unsigned char cmd, unsigned short int tns,
                                  unsigned char fnc, unsigned char *data,
                                  unsigned short int datalen)
{
  unsigned short int i = 0;

  query[i++] = stn;                         /* Transport independent query */
  query[i++] = 0x00;

  query[i++] = cmd;
  query[i++] = 0x00;

  query[i++] = PDS_GETLOBYTE(tns);
  query[i++] = PDS_GETHIBYTE(tns);

  query[i++] = fnc;

  memcpy(&query[i], data, datalen);
  i += datalen;

  return i;
}



/******************************************************************************
* Function to query the PLC                                                   *
*                                                                             *
* Pre-condition:  Open fd and a transaction struct containing the query &     *
*                 storage for the response are passed to the function         *
* Post-condition: Query is run against the PLC, response buffer holds the     *
*                 result of the query.  Number of bytes successfully read is  *
*                 returned or -1 on error                                     *
******************************************************************************/
int dh_run_plc_query(int fd, pdstrans *trans)
{
  fd_set fds;
  struct timeval tv;
  unsigned char ack_resp[DH_ACK_LEN] = "\0";
  int nbytes = 0, ack_len = 0, resp_recvd = 0;

  FD_ZERO(&fds);
  tv.tv_sec = DH_TMO_SECS;
  tv.tv_usec = DH_TMO_USECS;
  FD_SET(fd, &fds);

  /* Signal handler for a timeout */
  signal(SIGALRM, dh_timeout);

  /* Check that the fd is ready to write data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to send on socket\n", PROGNAME);
    return -1;
  }

  /* Send the PLC query request */
  switch(trans->protocol)
  {
    case DH_SERIAL_TCPIP :
      if((nbytes = send(fd, trans->buf, trans->blen, 0)) < trans->blen)
      {
        err(errout, "%s: error sending on socket\n", PROGNAME);
        err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, trans->blen, nbytes);
        return -1;
      }
    break;

    case DH_SERIAL :
      if((nbytes = write_plc_tty(fd, trans->buf, trans->blen)) < trans->blen)
      {
        err(errout, "%s: error sending on socket\n", PROGNAME);
        err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, trans->blen, nbytes);
        return -1;
      }
    break;
  }

  FD_SET(fd, &fds);

  /* Wait for response to return on fd */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to recv ACK on socket\n", PROGNAME);
    return -1;
  }

  dbgmsg("Receiving query ACK...\n");

  resp_recvd = 0;
  alarm(DH_ACK_TIMEOUT);          /* Schedule a comms timeout */

  do                              /* First receive an ACK for the query */
  {
    /* Read a single byte at a time from the fd input stream */
    switch(trans->protocol)
    {
      case DH_SERIAL_TCPIP :
        ack_len += recv(fd, &ack_resp[ack_len], 1, 0);
      break;

      case DH_SERIAL :
        ack_len += read_plc_tty(fd, &ack_resp[ack_len], 1);
      break;
    }

    if(__dh_timedout)
    {
      err(errout, "%s: timed out recv query ACK\n", PROGNAME);
      ack_len = 0;
      memset(&ack_resp, 0, DH_ACK_LEN);
      break;
    }

    /* The ACK sequence has been received */
    if((ack_resp[ack_len-1] == PDS_ACK) && (ack_resp[ack_len-2] == PDS_DLE))
      resp_recvd = 1;

    /* Ensure we don't overrun the ACK response buffer */
    if((ack_len >= DH_ACK_LEN) && (!resp_recvd))
    {
      err(errout, "%s: buffer overrun recv query ACK\n", PROGNAME);
      ack_len = 0;
      memset(&ack_resp, 0, DH_ACK_LEN);
      break;                      /* If we haven't got the ACK, give up */
    }
  }
  while(!resp_recvd && !__dh_timedout);

  alarm(0);                       /* Turn off the alarm */
  __dh_timedout = 0;

  /* Query was not acknowledged */
  if(memcmp(ack_resp, __dh_ack_seq, DH_ACK_LEN) != 0)
  {
    err(errout, "%s: failed to recv query ACK\n", PROGNAME);
    return -1;
  }
  else
    dbgmsg("Query ACK received\n");

  FD_SET(fd, &fds);

  /* Wait for response to return on fd */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to recv on socket\n", PROGNAME);
    return -1;
  }

  trans->blen = 0;
  memset(&trans->buf, 0, DH_MAXBUFLEN);

  resp_recvd = 0;
  alarm(DH_RESP_TIMEOUT);         /* Schedule a comms timeout */

  do                              /* Receive the query response */
  {
    /* Read a single byte at a time from the fd input stream */
    switch(trans->protocol)
    {
      case DH_SERIAL_TCPIP :
        trans->blen += recv(fd, &trans->buf[trans->blen], 1, 0);
      break;

      case DH_SERIAL :
        trans->blen += read_plc_tty(fd, &trans->buf[trans->blen], 1);
      break;
    }

    if(__dh_timedout)
    {
      err(errout, "%s: timed out recv response\n", PROGNAME);
      trans->blen = 0;
      memset(&trans->buf, 0, DH_MAXBUFLEN);
      break;
    }

    /* If the end of response sequence has been received, read the checksum */
    if((trans->buf[trans->blen-1] == PDS_ETX) && (trans->buf[trans->blen-2] == PDS_DLE))
    {
      switch(trans->protocol)
      {
        case DH_SERIAL_TCPIP :
          trans->blen += recv(fd, &trans->buf[trans->blen], 1, 0);
        break;

        case DH_SERIAL :
          trans->blen += read_plc_tty(fd, &trans->buf[trans->blen], 1);
        break;
      }

      resp_recvd = 1;
    }

    /* Ensure we don't overrun the transaction buffer */
    if((trans->blen >= DH_MAXBUFLEN) && (!resp_recvd))
    {
      err(errout, "%s: buffer overrun recv response\n", PROGNAME);
      trans->blen = 0;
      memset(&trans->buf, 0, DH_MAXBUFLEN);
      break;                      /* If we haven't got the response, give up */
    }
  }
  while(!resp_recvd && !__dh_timedout);

  alarm(0);                       /* Turn off the alarm */
  __dh_timedout = 0;

  FD_SET(fd, &fds);

  /* Check that the fd is ready to write data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to send response ACK on socket\n", PROGNAME);
    return -1;
  }

  dbgmsg("Sending response ACK...\n");

  /* Send the PLC response ACK */
  switch(trans->protocol)
  {
    case DH_SERIAL_TCPIP :
      if((nbytes = send(fd, __dh_ack_seq, DH_ACK_LEN, 0)) < DH_ACK_LEN)
      {
        err(errout, "%s: error sending response ACK on socket\n", PROGNAME);
        err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, DH_ACK_LEN, nbytes);
        return -1;
      }
      else
        dbgmsg("Response ACK sent\n");
    break;

    case DH_SERIAL :
      if((nbytes = write_plc_tty(fd, __dh_ack_seq, DH_ACK_LEN)) < DH_ACK_LEN)
      {
        err(errout, "%s: error sending response ACK on socket\n", PROGNAME);
        err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, DH_ACK_LEN, nbytes);
        return -1;
      }
      else
        dbgmsg("Response ACK sent\n");
    break;
  }

  return trans->blen;
}



/******************************************************************************
* Function to instantiate a prepared PLC query                                *
*                                                                             *
* Pre-condition:  A transaction struct containing a prepared query is passed  *
*                 to the function                                             *
* Post-condition: The runtime variables in the query (trans. ID, checksum)    *
*                 are instantiated.  The trans. ID is returned or -1 on error *
******************************************************************************/
short int dh_instantiate_prepared_query(pdstrans *trans)
{
  unsigned char data[DH_MAXBUFLEN] = "\0";
  unsigned short int i = 0, stuffed = 0, tdp_len = 0;
  unsigned short int dlen = 0;
  unsigned char lrc = 0;

  memset(&data, 0, DH_MAXBUFLEN);

  /* Set this transaction's ID */
  trans->trans_id = __dh_trans_id++;

  switch(trans->protocol)
  {
    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      tdp_len = DH_SERIAL_PRELEN; /* Prefix length */

      trans->buf[0] = PDS_DLE;    /* Transport-dependent prefix */ 
      trans->buf[1] = PDS_STX;

      memcpy(&data, trans->query, trans->qlen);
      dlen = trans->qlen;

      /* Set the query's transaction ID */
      data[DH_LO_TNS] = PDS_GETLOBYTE(trans->trans_id);
      data[DH_HI_TNS] = PDS_GETHIBYTE(trans->trans_id);

      /* Generate an LRC-8 for the query */
      lrc = generate_lrc8(data, dlen);

      /* Double-stuff any DLE's in the query, copy into transaction buffer */
      stuffed = double_stuff_byte(data, &trans->buf[tdp_len], &dlen, PDS_DLE);

      trans->blen = tdp_len + dlen;         /* Calc. buffer length */

      trans->buf[trans->blen++] = PDS_DLE;  /* Postfix */
      trans->buf[trans->blen++] = PDS_ETX;

      trans->buf[trans->blen++] = lrc;      /* Append the LRC-8 to Postfix */
    break;
  }

  return trans->trans_id;
}



/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  A transaction struct containing a valid response is passed  *
*                 to the function                                             *
* Post-condition: The response is cleaned (any double-stuffed bytes removed). *
*                 The response length is modified accordingly and returned or *
*                 -1 on error                                                 *
******************************************************************************/
short int dh_clean_plc_response(pdstrans *trans)
{
  unsigned short int removed = 0, tdpre_len = 0, tdpost_len = 0;

  /* Determine the transport-dependent pre/post fix */
  switch(trans->protocol)
  {
    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      tdpre_len = DH_SERIAL_PRELEN;
      tdpost_len = DH_SERIAL_POSTLEN;
    break;
  }

  trans->blen -= tdpre_len;       /* Remove tdpre from buffer length */

  if(trans->blen > 0)
  {
    /* Remove any double-stuffed DLE's from the buffer. Result in response */
    removed = remove_double_stuff_byte(&trans->buf[tdpre_len], trans->response, &trans->blen, PDS_DLE);

    /* Set response length */ 
    trans->rlen = (trans->blen - tdpost_len);
  }
  else
    err(errout, "%s: response buffer contained no data\n", PROGNAME);

  return trans->rlen;
}



/******************************************************************************
* Function to check the validity of a PLC query response                      *
*                                                                             *
* Pre-condition:  A transaction struct containing the response is passed to   *
*                 the function                                                *
* Post-condition: The response is checked.  If successful, a zero is returned *
*                 else a code is returned indicating the exception            *
******************************************************************************/
int dh_check_plc_response(pdstrans *trans)
{
  int excode = 0;
  unsigned short int trans_id = 0;
  unsigned char status = 0;

  trans_id = PDS_MAKEWORD(trans->response[DH_HI_TNS], trans->response[DH_LO_TNS]);
  status = trans->response[DH_STS_BYTE];

  /* Check if this response's transaction ID is not the one we expected */
  if(trans_id != trans->trans_id)
  {
    excode = (int) DH_TRANS_ID_MISMATCH;
  }
  /* Check if there is an exception pending on this query's response,
     including any extended status byte */
  else if(status != DH_STS_SUCCESS)
  {
    if(status == DH_EXT_STS_FLAG)
    {
      excode = (int) (trans->response[DH_EXT_STS_BYTE] | DH_EXT_STS_BITMASK);
    }
    else
    {
      excode = (int) status;
    }
  }

  return excode;
}



/******************************************************************************
* Function to refresh the shared memory status tags with data from a PLC      *
*                                                                             *
* Pre-condition:  The connection struct & a transaction struct containing a   *
*                 response and pointer's to this query's status word and      *
*                 error count are passed to the function                      *
* Post-condition: The data in the PLC's response is used to update the status *
*                 tags in the shared memory segment.  A count of updated tags *
*                 is returned                                                 *
******************************************************************************/
int dh_refresh_status_tags(pdsconn *conn, pdstrans *trans)
{
  int updated = 0;
  char expected[DH_PLC_STAT_LEN] = "\0";
  unsigned short int expected_len = 0;

  /* Build the echo return status string that we expect to see in response */
  sprintf(expected, DH_PLC_STAT_FORMAT, DH_PLC_STAT_TEXT, trans->trans_id);
  expected_len = strlen(expected);
  
  if(strncmp(expected, &trans->response[DH_CMD_DATA], expected_len) != 0)
  {
    *trans->status |= PDS_PLC_OFFLINE;
    (*trans->errx)++;

    /* Set the status word for this PLC's tags */
    updated = set_tags_status(conn, *trans->status);
  }

  return updated;
}



/******************************************************************************
* Function to refresh the shared memory data tags with data from a PLC        *
*                                                                             *
* Pre-condition:  The connection struct and a transaction struct containing   *
*                 the query and a valid response are passed to the function   *
* Post-condition: The data in the PLC's response is used to update the data   *
*                 tags in the shared memory segment.  The no. of values in    *
*                 the response is returned or -1 on error                     *
******************************************************************************/
int dh_refresh_data_tags(pdsconn *conn, pdstrans *trans)
{
  short int ntags = 0, nvalues = 0;
  pdstag *tag = NULL, *base = NULL;
  register unsigned short int i = 0;

  /* Find the number of tags in this query */
  ntags = PDS_MAKEWORD(trans->query[DH_HI_TTRANS], trans->query[DH_LO_TTRANS]);

  /* Get pointer to the beginning of this block in shared memory */
  tag = trans->block_start;

  /* Write the block's configured registers' values into shared memory */
  switch((trans->response[DH_CMD_BYTE] ^ DH_CMD_REPLY_FLAG))
  {
    case PDS_GETHIBYTE(DH_WR_READ) :        /* Word Range Read */
      nvalues = ntags;            /* No. of values = no. of tags */

      for(i = 0, base = tag; i < nvalues; i++) 
      {
        if(i == tag->ref) 
        { 
          tag->value = PDS_MAKEWORD(trans->response[DH_HI_DATA+i+i], trans->response[DH_LO_DATA+i+i]);
          tag->mtime = (time_t) time(NULL);
          tag++;
        }
      }
    break;
  }

  return nvalues;
}



/******************************************************************************
* Function to setup a TTY serial port structure for DataHighway serial comms  *
*                                                                             *
* Pre-condition:  A TTY serial port structure for storage is passed to the    *
*                 function                                                    *
* Post-condition: The structure elements are configured for DH serial comms   *
******************************************************************************/
int dh_init_tty_struct(struct termios *tio)
{
  /* Setup the TTY port's serial comms parameters (9600,8,N,1) */
  tio->c_cflag = ((tio->c_cflag & ~CSIZE) | CS8);
  tio->c_cflag = ((tio->c_cflag & ~CSTOPB) | 0);
  tio->c_cflag = ((tio->c_cflag & ~PARENB) | 0);
  tio->c_cflag = ((tio->c_cflag & ~HUPCL) | 0);
  tio->c_cflag = ((tio->c_cflag & ~CLOCAL) | CLOCAL);
  tio->c_cflag = ((tio->c_cflag & ~CRTSCTS) | 0);

  tio->c_lflag = 0;
  tio->c_oflag = 0;
  tio->c_iflag = 0;

  tio->c_cc[VMIN] = 0;
  tio->c_cc[VTIME] = 0;

  /* Set the input & output baud rates */
  cfsetispeed(tio, B9600);
  cfsetospeed(tio, B9600);

  return 0;
}



/******************************************************************************
* Function to handle a timeout                                                *
*                                                                             *
* Pre-condition:  Signal is recieved                                          *
* Post-condition: Timeout flag is set to 1.  Handler is re-installed          *
******************************************************************************/
void dh_timeout(int sig)
{
  __dh_timedout = 1;
  signal(SIGALRM, dh_timeout);
}



/******************************************************************************
* Function to register signals to be handled                                  *
*                                                                             *
* Pre-condition:  Signals are handled in the default manner                   *
* Post-condition: Registered signals have specific handler functions          *
******************************************************************************/
void dh_install_signal_handler(void)
{
  signal(SIGALRM, dh_timeout);
}

