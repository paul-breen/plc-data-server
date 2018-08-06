/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_mb.c                                                          *
* PURPOSE:  The ModBus specific functions module                              *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#include "pds_mb.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
static unsigned short int __mb_trans_id = 1;

/******************************************************************************
* Function to setup a read PLC query using the configuration file parameters  *
*                                                                             *
* Pre-condition:  Storage for this block's query and the block structure are  *
*                 passed to the function                                      *
* Post-condition: This block's configuration parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int mb_setup_read_query(unsigned char *query, plc_cnf_block *block)
{
  unsigned short int unit = 0, function = 0, ref = 0, nrefs = 0;
  short int qlen = 0;
 
  unit = atoi(block->path);
  function = MB_GET_FUNC(block->function);
  function = MB_WRRD_MAP(function); 
  ref = MB_LO_REF(block->base_addr, block->tags[0].ref);
  nrefs = MB_HI_REF(block->tags[0].ref, block->tags[block->ntags-1].ref);

  /* Construct the PLC query to read this block */
  if((qlen = mb_construct_read_plc_query(query, unit, function, ref, nrefs)) == -1)
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
int mb_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg)
{
  unsigned short int unit = 0, function = 0, ref = 0;
  short int qlen = 0;
 
  function = MB_GET_FUNC(tag->function);
  unit = atoi(tag->path);
  ref = MB_LO_REF(tag->base_addr, tag->ref);

  /* Construct the PLC query to write to this tag(s) */
  if((qlen = mb_construct_write_plc_query(query, unit, function, ref, msg->ntags, msg->tagvalues)) == -1)
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
int mb_setup_status_query(unsigned char *query, pdsconn *conn)
{
  short int qlen = 0;
 
  /* Construct the PLC query to get the status of this PLC */
  if((qlen = mb_construct_status_plc_query(query, atoi(conn->path), MB_SID_STAT)) == -1)
  {
    err(errout, "%s: error constructing status query\n", PROGNAME);
  }

  return qlen;
}



/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 function code, the base reference and the number of         *
*                 references to read are passed to the function               *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_read_plc_query(unsigned char *query, unsigned short int unit,
                                unsigned short int function,
                                unsigned short int ref,
                                unsigned short int nrefs)
{
  short int i = 0;

  /* N.B.:  In ModBus the reference number is always 1 greater than the 
            actual binary representation, e.g., reference 400001 corresponds
            to holding register 0 */

  if(ref < 1)
    return -1;

  if(MB_DATABYTES(function, nrefs) > MB_RDDATA_MAX)
    return -1;

  query[i++] = unit;              /* Unit ID (ModBus PLC node address) */
  query[i++] = function;          /* ModBus hex. function code */
  query[i++] = (ref - 1) >> 8;    /* High order byte of base address */
  query[i++] = (ref - 1) & 0xff;  /* Low order byte of base address */

  query[i++] = nrefs >> 8;        /* High order byte of no. of refs */
  query[i++] = nrefs & 0xff;      /* Low order byte of no. of refs */

  return (int) i;
}



/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 function code, the base reference, number of references and *
*                 a pointer to the value(s) are passed to the function        *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_write_plc_query(unsigned char *query, unsigned short int unit,
                                 unsigned short int function,
                                 unsigned short int ref,
                                 unsigned short int nrefs,
                                 unsigned short int *values)
{
  short int i = 0, j = 0, k = 0;

  /* N.B.:  In ModBus the reference number is always 1 greater than the 
            actual binary representation, e.g., reference 400001 corresponds
            to holding register 0 */

  if(ref < 1)
    return -1;

  if(MB_DATABYTES(function, nrefs) > MB_WRDATA_MAX)
    return -1;

  query[i++] = unit;              /* Unit ID (ModBus PLC node address) */
  query[i++] = function;          /* ModBus hex. function code */
  query[i++] = (ref - 1) >> 8;    /* High order byte of base address */
  query[i++] = (ref - 1) & 0xff;  /* Low order byte of base address */

  query[i++] = nrefs >> 8;        /* High order byte of no. of refs */
  query[i++] = nrefs & 0xff;      /* Low order byte of no. of refs */

  /* Set the no. of bytes in the data buffer according to function */
  query[i++] = MB_DATABYTES(function, nrefs);

  /* Set the data bytes in the data buffer according to function */
  if(function == MB_MC_WRITE)          /* Force Multiple Coils */
  {
    for(j = i, k = 0; k < nrefs; j++, k++)
    {
      query[j] = values[k] & 0xff;     /* Upto MB_BITS_BYTE coils per byte */
    }
  }
  else if(function == MB_MR_WRITE)     /* Preset Multiple Registers */
  {
    for(j = i, k = 0; k < nrefs; j += MB_WORDSIZE, k++)
    {
      query[j] = values[k] >> 8;       /* High order byte */
      query[j+1] = values[k] & 0xff;   /* Low order byte */
    }
  }
 
  return (int) (i + MB_DATABYTES(function, nrefs));
}



/******************************************************************************
* Function to construct a PLC status PLC query                                *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address) and the hex. *
*                 function code {0x07|0x0B|0x0C|0x11} are passed to the       *
*                 function                                                    *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_status_plc_query(unsigned char *query,
                                  unsigned short int unit,
                                  unsigned short int function)
{
  short int i = 0;

  query[i++] = unit;              /* Unit ID (ModBus PLC node address) */
  query[i++] = function;          /* ModBus hex. function code */

  return (int) i;
}



/******************************************************************************
* Function to construct a PLC diagnostics PLC query                           *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 subfunction code and the data (or zero if not applicable)   *
*                 are passed to the function                                  *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_diagnostics_plc_query(unsigned char *query,
                                       unsigned short int unit,
                                       unsigned short int subfunc,
                                       unsigned short int data)
{
  short int i = 0;

  query[i++] = unit;              /* Unit ID (ModBus PLC node address) */
  query[i++] = MB_PLC_DIAG;       /* ModBus hex. function code (diagnostics) */
  query[i++] = subfunc >> 8;      /* High order byte of subfunction */
  query[i++] = subfunc & 0xff;    /* Low order byte of subfunction */

  query[i++] = data >> 8;         /* High order byte of data */
  query[i++] = data & 0xff;       /* Low order byte of data */

  return (int) i;
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
int mb_run_plc_query(int fd, pdstrans *trans)
{
  fd_set fds;
  struct timeval tv;
  int nbytes = 0;

  FD_ZERO(&fds);
  tv.tv_sec = MB_TMO_SECS;
  tv.tv_usec = MB_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the fd is ready to write data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to send on socket\n", PROGNAME);
    return -1;
  }

  /* Send the PLC query request */
  switch(trans->protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
      if((nbytes = send(fd, trans->buf, trans->blen, 0)) < trans->blen)
      {
        err(errout, "%s: error sending on socket\n", PROGNAME);
        err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, trans->blen, nbytes);
        return -1;
      }
    break;

    case MB_SERIAL :
      usleep(MB_SERIAL_PAUSE);    /* At least 3.5 char times between comms */

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
    err(errout, "%s: error selecting to recv on socket\n", PROGNAME);
    return -1;
  }

  trans->blen = 0;
  memset(&trans->buf, 0, MB_MAXBUFLEN);

  /* Receive the query response */
  switch(trans->protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
      return (trans->blen = recv(fd, trans->buf, MB_MAXBUFLEN, 0));
    break;

    case MB_SERIAL :
      usleep(MB_SERIAL_PAUSE);    /* At least 3.5 char times between comms */
      return (trans->blen = read_plc_tty(fd, trans->buf, MB_MAXBUFLEN));
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
short int mb_instantiate_prepared_query(pdstrans *trans)
{
  unsigned short int i = 0, tdp_len = 0, crc = 0;
  unsigned short int function = 0, nrefs = 0;

  /* Set this transaction's ID */
  trans->trans_id = __mb_trans_id++;

  /* Get this query's function code */
  function = trans->query[MB_FUNC_CODE];

  switch(trans->protocol)
  {
    case MB_TCPIP :
      tdp_len = MB_TCPIP_PRELEN;  /* Prefix length */

      for(i = 0; i < 5; i++)      /* Zero the first 5 (0 to 4) elements */ 
        trans->buf[i] = 0;        /* - id id proto-id proto-id hi-len */

      /* The next byte (following no. of bytes in buffer) is determined
         by the type of the query's function (plus nrefs if write query) */
      switch(MB_FUNCTYPE(function))
      {
        case PDS_RD_FUNC :        /* A read function */
          trans->buf[i++] = MB_READ_BYTES_IN_BUF;
        break;

        case PDS_WR_FUNC :        /* A write function */
          nrefs = PDS_MAKEWORD(trans->query[MB_HI_NREFS],
          trans->query[MB_LO_NREFS]);
          trans->buf[i++] = MB_WRITE_BYTES_IN_BUF +
          MB_DATABYTES(function, nrefs);
        break;

        case PDS_STAT_FUNC :      /* A status function */
          trans->buf[i++] = MB_STAT_BYTES_IN_BUF;
        break;
       
        case PDS_DIAG_FUNC :      /* A diagnostics function */
          trans->buf[i++] = MB_DIAG_BYTES_IN_BUF;
        break;
      }

      memcpy(&trans->buf[tdp_len], trans->query, trans->qlen);
      trans->blen = tdp_len + trans->qlen;
    break;

    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      tdp_len = MB_SERIAL_PRELEN; /* Prefix length */

      /* Generate a CRC-16 for the query */
      crc = generate_crc16(trans->query, trans->qlen);

      memcpy(&trans->buf[tdp_len], trans->query, trans->qlen);
      trans->blen = tdp_len + trans->qlen;

      /* Append the CRC-16 */
      trans->buf[trans->blen++] = PDS_GETLOBYTE(crc);
      trans->buf[trans->blen++] = PDS_GETHIBYTE(crc);
    break;
  }

  return trans->trans_id;
}



/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  A transaction struct containing a valid response is passed  *
*                 to the function                                             *
* Post-condition: The response is cleaned (transport dependent prefix         *
*                 removed).  The response length is modified accordingly and  *
*                 returned or -1 on error                                     *
******************************************************************************/
short int mb_clean_plc_response(pdstrans *trans)
{
  unsigned short int tdpre_len = 0, tdpost_len = 0;

  /* Determine the transport-dependent pre/post fix */
  switch(trans->protocol)
  {
    case MB_TCPIP :
      tdpre_len = MB_TCPIP_PRELEN;
      tdpost_len = MB_TCPIP_POSTLEN;
    break;

    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      tdpre_len = MB_SERIAL_PRELEN;
      tdpost_len = MB_SERIAL_POSTLEN;
    break;
  }

  trans->blen -= tdpre_len;       /* Remove tdpre from buffer length */

  if(trans->blen > 0)
  {
    memcpy(&trans->response, &trans->buf[tdpre_len], trans->blen);
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
int mb_check_plc_response(pdstrans *trans) 
{
  int excode = 0;
  unsigned short int function = 0, expected = 0;

  /* Get the hex. function code */
  function = (unsigned short int) trans->response[MB_FUNC_CODE];

  /* Check for an error in the response */
  if(function & MB_EXFLAG)
  {
    excode = (int) trans->response[MB_EXCEPTION_CODE];
  } 
  else if(MB_FUNCTYPE(function) == PDS_RD_FUNC)       /* Read function */
  {
    /* Calculate the expected response message size */
    expected = MB_RDHDR_SIZE + trans->response[MB_RESPONSE_DATABYTES];

    if(trans->rlen != expected)
    {
      excode = (int) MB_INCORRECT_RESPONSE_SIZE;
    }
  } 
  else if(MB_FUNCTYPE(function) == PDS_WR_FUNC)       /* Write function */
  {
    /* Calculate the expected response message size */
    expected = MB_WRHDR_SIZE;

    if(trans->rlen != expected)
    {
      excode = (int) MB_INCORRECT_RESPONSE_SIZE;
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
int mb_refresh_status_tags(pdsconn *conn, pdstrans *trans)
{
  int updated = 0;

  /* If the run-light indicator byte is zero, PLC is offline */
  if(trans->response[MB_RUN_INDICATOR] == 0)
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
int mb_refresh_data_tags(pdsconn *conn, pdstrans *trans)
{
  short int nbytes = 0, ntags = 0, nvalues = 0;
  pdstag *tag = NULL, *base = NULL;
  unsigned short int function = 0;
  register unsigned short int i = 0, j = 0, x = 0, bit = 0;

  /* Get the hex. function code for this query/response */
  function = (unsigned short int) trans->response[MB_FUNC_CODE];

  /* Find the number of tags in this query */
  ntags = PDS_MAKEWORD(trans->query[MB_HI_NREFS], trans->query[MB_LO_NREFS]);

  /* Find the number of values in the response.  If the function was
     read coils/discrete inputs then the number of values is the number
     in the response field with each byte itself holding upto MB_BITS_BYTE
     coils/inputs.  If the function was read holding/input registers
     then the number of values is the number in the response field 
     divided by MB_WORDSIZE */

  if(function == MB_CS_READ || function == MB_IS_READ) 
  {
    nvalues = trans->response[MB_RESPONSE_DATABYTES] * MB_BITS_BYTE;
    nbytes = trans->response[MB_RESPONSE_DATABYTES];
  }
  else if(function == MB_HR_READ || function == MB_IR_READ)
  {
    nvalues = (trans->response[MB_RESPONSE_DATABYTES] > 0 ? trans->response[MB_RESPONSE_DATABYTES] / MB_WORDSIZE : 0);
  }

  /* Get pointer to the beginning of this block in shared memory */
  tag = trans->block_start;

  /* Write the block's configured registers' values into shared memory */

  /* Reference is coil/input, get MB_BITS_BYTE values per byte */
  if(function == MB_CS_READ || function == MB_IS_READ)
  {
    for(i = 0, bit = 0, base = tag; i < nbytes && bit < nvalues; i++) 
    {
      /* Get values of upto MB_BITS_BYTE bits */
      for(x = 0; x < MB_BITS_BYTE; x++)
      {
        if(bit == (tag->ref - base->ref)) 
        { 
          tag->value = PDS_GETBIT(trans->response[MB_HI_DATA+i], x);
          tag->mtime = (time_t) time(NULL);
          tag++;
        }
        bit++;
      }
    }
  }

  /* Reference is holding/input register, make value from MB_WORDSIZE bytes */
  else if(function == MB_HR_READ || function == MB_IR_READ)
  {
    for(i = 0, base = tag; i < nvalues; i++) 
    {
      if(i == (tag->ref - base->ref)) 
      { 
        tag->value = PDS_MAKEWORD(trans->response[MB_HI_DATA+i+i],
        trans->response[MB_LO_DATA+i+i]);
        tag->mtime = (time_t) time(NULL);
        tag++;
      }
    }
  }

  return nvalues;
}



/******************************************************************************
* Function to setup a TTY serial port structure for ModBus serial comms       *
*                                                                             *
* Pre-condition:  A TTY serial port structure for storage is passed to the    *
*                 function                                                    *
* Post-condition: The structure elements are configured for MB serial comms   *
******************************************************************************/
int mb_init_tty_struct(struct termios *tio)
{
  /* Setup the TTY port's serial comms parameters (9600,8,E,1) */
  tio->c_cflag = ((tio->c_cflag & ~CSIZE) | CS8);
  tio->c_cflag = ((tio->c_cflag & ~CSTOPB) | 0);
  tio->c_cflag = ((tio->c_cflag & ~PARENB) | PARENB);
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

