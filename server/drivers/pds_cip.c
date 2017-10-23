/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_cip.c                                                         *
* PURPOSE:  The CIP specific functions module                                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-01-08                                                        *
******************************************************************************/

#include "pds_cip.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
static unsigned short int __cip_trans_id = 1;
static int __cip_fd = 0;
static unsigned int __cip_sid = 0;

/******************************************************************************
* Function to setup a read PLC query using the configuration file parameters  *
*                                                                             *
* Pre-condition:  Storage for this block's query and the block structure are  *
*                 passed to the function                                      *
* Post-condition: This block's configuration parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int cip_setup_read_query(unsigned char *query, plc_cnf_block *block)
{
  unsigned short int size = 0;
  char ioi[CIP_IOI_SEGLEN+1] = "\0";
  short int qlen = 0, ioilen = 0;

  /* If the IOI is an array, remove its [] chars from the end of the string */
  ioilen = strlen(block->ascii_addr);

  if((ioilen > 2) && (block->ascii_addr[ioilen-1] == ']') &&
     (block->ascii_addr[ioilen-2] == '['))
    ioilen -= 2;

  ioilen = (ioilen < CIP_IOI_SEGLEN) ? ioilen : CIP_IOI_SEGLEN;

  strncpy(ioi, block->ascii_addr, ioilen);
  ioi[ioilen] = '\0';

  size = CIP_HI_REF(block->tags[0].ref, block->tags[block->ntags-1].ref);

  /* Construct the PLC query to read this block */
  if((qlen = cip_construct_read_plc_query(query, ioi, size, block->path)) == -1)
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
int cip_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg)
{
  unsigned short int type = 0;
  char ioi[CIP_IOI_SEGLEN+1] = "\0";
  unsigned int element = 0;
  short int qlen = 0, ioilen = 0, isarray = 0;

  /* If the IOI is an array, remove its [] chars from the end of the string */
  ioilen = strlen(tag->ascii_addr);

  if((ioilen > 2) && (tag->ascii_addr[ioilen-1] == ']') &&
     (tag->ascii_addr[ioilen-2] == '['))
  {
    ioilen -= 2;
    isarray = 1;
  }
  ioilen = (ioilen < CIP_IOI_SEGLEN) ? ioilen : CIP_IOI_SEGLEN;

  strncpy(ioi, tag->ascii_addr, ioilen);
  ioi[ioilen] = '\0';

  type = CIP_GET_TYPE(tag->type);
  element = tag->ref;

  /* Construct the PLC query to write to this tag(s) */
  if(isarray)
  {
    if((qlen = cip_construct_write_array_plc_query(query, ioi, msg->ntags, type, msg->tagvalues, element, tag->path)) == -1)
    {
      err(errout, "%s: error constructing write array query\n", PROGNAME);
    }
  }
  else
  {
    if((qlen = cip_construct_write_plc_query(query, ioi, msg->ntags, type, msg->tagvalues, tag->path)) == -1)
    {
      err(errout, "%s: error constructing write query\n", PROGNAME);
    }
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
int cip_setup_status_query(unsigned char *query, pdsconn *conn)
{
  short int qlen = 0;

  /* This PLC doesn't support a status query */

  return qlen;
}



/******************************************************************************
* Function to construct a register PLC query                                  *
*                                                                             *
* Pre-condition:  The query buffer is passed to the function                  *
* Post-condition: Query buffer is initialised. On success, the total number   *
*                 of bytes in the query is returned, on error a -1 is         *
*                 returned                                                    *
******************************************************************************/
int cip_construct_register_plc_query(unsigned char *query)
{
  unsigned short int i = 0, j = 0;

  query[i++] = CIP_REGISTER;
  query[i++] = 0x00;
  query[i++] = 0x04;
  query[i++] = 0x00;

  for(j = 0; j < 8; j++)
    query[i++] = 0x00;

  for(j = 0; j < 8; j++)
    query[i++] = 0x00;

  for(j = 0; j < 4; j++)
    query[i++] = 0x00;

  query[i++] = 0x01;
  query[i++] = 0x00;

  query[i++] = 0x00;
  query[i++] = 0x00;

  return i;
}



/******************************************************************************
* Function to construct an unregister PLC query                               *
*                                                                             *
* Pre-condition:  The query buffer & the PLC assigned session ID are passed   *
*                 to the function                                             *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_unregister_plc_query(unsigned char *query, unsigned int sid)
{
  unsigned short int i = 0, j = 0;

  query[i++] = CIP_UNREGISTER;
  query[i++] = 0x00;
  query[i++] = 0x00;
  query[i++] = 0x00;

  /* Encode the session ID (lobyte-hibyte) */
  query[i++] = PDS_GETBYTE0(sid);
  query[i++] = PDS_GETBYTE1(sid);
  query[i++] = PDS_GETBYTE2(sid);
  query[i++] = PDS_GETBYTE3(sid);

  for(j = 0; j < 4; j++)
    query[i++] = 0x00;

  for(j = 0; j < 8; j++)
    query[i++] = 0x00;

  for(j = 0; j < 4; j++)
    query[i++] = 0x00;

  return i;
}



/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to read *
*                 & the routing path are passed to the function               *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_read_plc_query(unsigned char *query, char *ioi,
                                 unsigned short int size, char *path)
{
  unsigned short int i = 0, j = 0, ioi_sym_bytelen = 0, mr_bytelen = 0;
  unsigned char ioi_sym[CIP_IOI_SEGLEN] = "\0";
  unsigned char ioi_bytelen = 0, ioi_wordlen = 0, path_bytelen = 0;
  unsigned char buf[CIP_MAXBUFLEN] = "\0", pathbuf[PDS_PLC_PATH_LEN] = "\0";
  cip_enc_header header;
  cip_send_rr_data send_rr_data;
  cip_cpf cpf;

  memset(ioi_sym, 0, sizeof(ioi_sym));
  memset(buf, 0, sizeof(buf));
  memset(pathbuf, 0, sizeof(pathbuf));
  memset(&header, 0, sizeof(header));
  memset(&send_rr_data, 0, sizeof(send_rr_data));
  memset(&cpf, 0, sizeof(cpf));

  /* Build up the structures to provide data for the various layers of CIP
     encapsulation.  N.B.: Session ID (sid) is filled in at runtime */
  header.command = CIP_SEND_RR_DATA;
  header.sid = 0;

  /* N.B.: In the cip_send_rr_data struct the 2 struct members are all set to
           zero (interface ID for CIP = 0, timeout = 0 -- use CIP timeout). */

  cpf.count = 0x02;                         /* 1 for address, 1 for data */
  cpf.address.typeid = 0x00;                /* Null address means UCMM msg */
  cpf.address.length = 0x00;
  cpf.data.typeid = CIP_UNCONNECTED_DATA;   /* Data for unconnected send */

  ioi_bytelen = strlen(ioi);                /* Get the IOI string length */

  /* The 1st byte of the IOI symbolic string contains the word count for
     the string.  This is calculated as (byte length of string + pad byte
     if uneven number of bytes + 1 byte for CIP_SYMBOLIC_SEGMENT + 1 byte
     for length of string in bytes).  This is expressed as a word count NOT
     byte count */
  ioi_wordlen = ioi_bytelen;
  ioi_wordlen = ((ioi_wordlen % 2) == 0) ? ioi_wordlen : ioi_wordlen + 1;
  ioi_wordlen += 2;
  ioi_wordlen = ioi_wordlen ? ioi_wordlen / 2 : 0;

  ioi_sym[i++] = ioi_wordlen;
  ioi_sym[i++] = CIP_SYMBOLIC_SEGMENT;
  ioi_sym[i++] = ioi_bytelen;

  /* Copy the IOI tagname into the IOI symbolic segment */
  for(j = 0; j < ioi_bytelen; j++)
    ioi_sym[i++] = ioi[j];

  ioi_sym_bytelen = i;                      /* Save length of IOI symbol */

  ioi_sym_bytelen = ((ioi_bytelen % 2) == 0) ? ioi_sym_bytelen : ioi_sym_bytelen + 1;

  i = 0;

  /* Build a buffer containing a request for an unconnected send */
  buf[i++] = CIP_UNCONNECTED_SEND;
  buf[i++] = 0x02;                          /* 2 words in following IOI seg */

  /* Route msg through the connection manager */
  buf[i++] = CIP_CLASS_SEGMENT1;
  buf[i++] = CIP_CM_CLASS_CODE;
  buf[i++] = CIP_INSTANCE_SEGMENT1;
  buf[i++] = CIP_CM_INSTANCE_CODE;

  /* Encode the connection timeout */
  buf[i++] = CIP_CONN_TICK_TIME;
  buf[i++] = CIP_CONN_TICK_TIMEOUT;

  /* Bytelen of embedded msg = IOI symbol length + byte for CIP_DATA_READ +
     2 bytes for size */
  mr_bytelen = ioi_sym_bytelen + 3;
  buf[i++] = PDS_GETLOBYTE(mr_bytelen);
  buf[i++] = PDS_GETHIBYTE(mr_bytelen);

  /* Embedded MR msg for an unconnected read - CIP Read Data Service */
  buf[i++] = CIP_DATA_READ;

  /* Copy the IOI symbolic segment into the buffer */
  for(j = 0; j < ioi_sym_bytelen; j++)
    buf[i++] = ioi_sym[j];

  /* Encode the size (lobyte-hibyte) */
  buf[i++] = PDS_GETLOBYTE(size);
  buf[i++] = PDS_GETHIBYTE(size);

  /* Parse, & get the size of, routing path (in bytes) */
  path_bytelen = (unsigned char) cip_parse_plc_routing_path(path, pathbuf);

  /* Size of routing path (in words) */
  buf[i++] = (path_bytelen ? path_bytelen / 2 : 0);
  buf[i++] = 0x00;

  /* Encode the routing path */
  memcpy(&buf[i], pathbuf, path_bytelen);
  i += path_bytelen;

  /* Calc. the length of the CPF & the overall encapsulated data */
  cpf.data.length = i;
  header.length = sizeof(send_rr_data) + sizeof(cpf) + i;

  i = 0;

  /* Build a buffer containing a CIP encapsulated query */
  memcpy(query, &header, sizeof(header));
  i = sizeof(header);
  memcpy(&query[i], &send_rr_data, sizeof(send_rr_data));
  i+= sizeof(send_rr_data);
  memcpy(&query[i], &cpf, sizeof(cpf));
  i+= sizeof(cpf);
  memcpy(&query[i], buf, cpf.data.length);
  i+= cpf.data.length;

  return i;
}



/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to      *
*                 write, the data type, value(s) & the routing path are       *
*                 passed to the function                                      *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_write_plc_query(unsigned char *query, char *ioi,
                                  unsigned short int size,
                                  unsigned short int type,
                                  unsigned short int *values, char *path)
{
  unsigned short int i = 0, j = 0, ioi_sym_bytelen = 0, mr_bytelen = 0;
  unsigned char ioi_sym[CIP_IOI_SEGLEN] = "\0";
  unsigned char ioi_bytelen = 0, ioi_wordlen = 0, path_bytelen = 0;
  unsigned char buf[CIP_MAXBUFLEN] = "\0", pathbuf[PDS_PLC_PATH_LEN] = "\0";
  int nwords = 0;
  cip_enc_header header;
  cip_send_rr_data send_rr_data;
  cip_cpf cpf;

  memset(ioi_sym, 0, sizeof(ioi_sym));
  memset(buf, 0, sizeof(buf));
  memset(pathbuf, 0, sizeof(pathbuf));
  memset(&header, 0, sizeof(header));
  memset(&send_rr_data, 0, sizeof(send_rr_data));
  memset(&cpf, 0, sizeof(cpf));

  /* Build up the structures to provide data for the various layers of CIP
     encapsulation.  N.B.: Session ID (sid) is filled in at runtime */
  header.command = CIP_SEND_RR_DATA;
  header.sid = 0;

  /* N.B.: In the cip_send_rr_data struct the 2 struct members are all set to
           zero (interface ID for CIP = 0, timeout = 0 -- use CIP timeout). */

  cpf.count = 0x02;                         /* 1 for address, 1 for data */
  cpf.address.typeid = 0x00;                /* Null address means UCMM msg */
  cpf.address.length = 0x00;
  cpf.data.typeid = CIP_UNCONNECTED_DATA;   /* Data for unconnected send */

  ioi_bytelen = strlen(ioi);                /* Get the IOI string length */

  /* The 1st byte of the IOI symbolic string contains the word count for
     the string.  This is calculated as (byte length of string + pad byte
     if uneven number of bytes + 1 byte for CIP_SYMBOLIC_SEGMENT + 1 byte
     for length of string in bytes).  This is expressed as a word count NOT
     byte count */
  ioi_wordlen = ioi_bytelen;
  ioi_wordlen = ((ioi_wordlen % 2) == 0) ? ioi_wordlen : ioi_wordlen + 1;
  ioi_wordlen += 2;
  ioi_wordlen = ioi_wordlen ? ioi_wordlen / 2 : 0;

  ioi_sym[i++] = ioi_wordlen;
  ioi_sym[i++] = CIP_SYMBOLIC_SEGMENT;
  ioi_sym[i++] = ioi_bytelen;

  /* Copy the IOI tagname into the IOI symbolic segment */
  for(j = 0; j < ioi_bytelen; j++)
    ioi_sym[i++] = ioi[j];

  ioi_sym_bytelen = i;                      /* Save length of IOI symbol */

  ioi_sym_bytelen = ((ioi_bytelen % 2) == 0) ? ioi_sym_bytelen : ioi_sym_bytelen + 1;

  i = 0;

  /* Build a buffer containing a request for an unconnected send */
  buf[i++] = CIP_UNCONNECTED_SEND;
  buf[i++] = 0x02;                          /* 2 words in following IOI seg */

  /* Route msg through the connection manager */
  buf[i++] = CIP_CLASS_SEGMENT1;
  buf[i++] = CIP_CM_CLASS_CODE;
  buf[i++] = CIP_INSTANCE_SEGMENT1;
  buf[i++] = CIP_CM_INSTANCE_CODE;

  /* Encode the connection timeout */
  buf[i++] = CIP_CONN_TICK_TIME;
  buf[i++] = CIP_CONN_TICK_TIMEOUT;

  /* Bytelen of embedded msg = IOI symbol length + byte for CIP_DATA_WRITE +
     2 bytes for type + 2 bytes for size +
     2 bytes for each 16 bit word of each data value (independent of type).
     The PDS API only uses 16 bit words, hence type is irrelevant */
  mr_bytelen = ioi_sym_bytelen + 5 + (2 * size);

  buf[i++] = PDS_GETLOBYTE(mr_bytelen);
  buf[i++] = PDS_GETHIBYTE(mr_bytelen);

  /* Embedded MR msg for an unconnected write - CIP Write Data Service */
  buf[i++] = CIP_DATA_WRITE;

  /* Copy the IOI symbolic segment into the buffer */
  for(j = 0; j < ioi_sym_bytelen; j++)
    buf[i++] = ioi_sym[j];

  /* Encode the type (lobyte-hibyte) */
  buf[i++] = PDS_GETLOBYTE(type);
  buf[i++] = PDS_GETHIBYTE(type);

  /* Determine the number of words (16 bit or 32 bit) dependent on data type */
  nwords = ((type == CIP_BOOL_TYPE || type == CIP_SINT_TYPE || type == CIP_INT_TYPE) ? size : (size ? size / 2 : 0));

  /* Encode the size (lobyte-hibyte) */
  buf[i++] = PDS_GETLOBYTE(nwords);
  buf[i++] = PDS_GETHIBYTE(nwords);

  for(j = 0; j < size; j++)
  {
    /* Encode the data value according to type */
    switch(type)
    {
      case CIP_BOOL_TYPE :
        buf[i++] = values[j] ? 0xff : 0x00;
        buf[i++] = 0x00;
      break;

      case CIP_SINT_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = 0x00;
      break;

      case CIP_INT_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
      break;

      case CIP_DINT_TYPE :
      case CIP_REAL_TYPE :
      case CIP_BIT_ARRAY_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
        buf[i++] = PDS_GETLOBYTE(values[++j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
      break;
    }
  }

  /* Parse, & get the size of, routing path (in bytes) */
  path_bytelen = (unsigned char) cip_parse_plc_routing_path(path, pathbuf);

  /* Size of routing path (in words) */
  buf[i++] = (path_bytelen ? path_bytelen / 2 : 0);
  buf[i++] = 0x00;

  /* Encode the routing path */
  memcpy(&buf[i], pathbuf, path_bytelen);
  i += path_bytelen;

  /* Calc. the length of the CPF & the overall encapsulated data */
  cpf.data.length = i;
  header.length = sizeof(send_rr_data) + sizeof(cpf) + i;

  i = 0;

  /* Build a buffer containing a CIP encapsulated query */
  memcpy(query, &header, sizeof(header));
  i = sizeof(header);
  memcpy(&query[i], &send_rr_data, sizeof(send_rr_data));
  i+= sizeof(send_rr_data);
  memcpy(&query[i], &cpf, sizeof(cpf));
  i+= sizeof(cpf);
  memcpy(&query[i], buf, cpf.data.length);
  i+= cpf.data.length;

  return i;
}



/******************************************************************************
* Function to construct a write (to an array) PLC query                       *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to      *
*                 write, the data type, the value(s), the array element to    *
*                 start writing at & the routing path are passed to the       *
*                 function                                                    *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_write_array_plc_query(unsigned char *query, char *ioi,
                                        unsigned short int size,
                                        unsigned short int type,
                                        unsigned short int *values,
                                        unsigned int element, char *path)
{
  unsigned short int i = 0, j = 0, ioi_sym_bytelen = 0, mr_bytelen = 0;
  unsigned char ioi_sym[CIP_IOI_SEGLEN] = "\0";
  unsigned char ioi_bytelen = 0, ioi_wordlen = 0, path_bytelen = 0;
  unsigned char buf[CIP_MAXBUFLEN] = "\0", pathbuf[PDS_PLC_PATH_LEN] = "\0";
  int nwords = 0;
  cip_enc_header header;
  cip_send_rr_data send_rr_data;
  cip_cpf cpf;

  memset(ioi_sym, 0, sizeof(ioi_sym));
  memset(buf, 0, sizeof(buf));
  memset(pathbuf, 0, sizeof(pathbuf));
  memset(&header, 0, sizeof(header));
  memset(&send_rr_data, 0, sizeof(send_rr_data));
  memset(&cpf, 0, sizeof(cpf));

  /* Build up the structures to provide data for the various layers of CIP
     encapsulation.  N.B.: Session ID (sid) is filled in at runtime */
  header.command = CIP_SEND_RR_DATA;
  header.sid = 0;

  /* N.B.: In the cip_send_rr_data struct the 2 struct members are all set to
           zero (interface ID for CIP = 0, timeout = 0 -- use CIP timeout). */

  cpf.count = 0x02;                         /* 1 for address, 1 for data */
  cpf.address.typeid = 0x00;                /* Null address means UCMM msg */
  cpf.address.length = 0x00;
  cpf.data.typeid = CIP_UNCONNECTED_DATA;   /* Data for unconnected send */

  ioi_bytelen = strlen(ioi);                /* Get the IOI string length */

  /* The 1st byte of the IOI symbolic string contains the word count for
     the string.  This is calculated as (byte length of string + pad byte
     if uneven number of bytes + 1 byte for CIP_SYMBOLIC_SEGMENT + 1 byte
     for length of string in bytes + word length of the element segment).
     This is expressed as a word count NOT byte count */
  ioi_wordlen = ioi_bytelen;
  ioi_wordlen = ((ioi_wordlen % 2) == 0) ? ioi_wordlen : ioi_wordlen + 1;
  ioi_wordlen += 2;
  ioi_wordlen = ioi_wordlen ? ioi_wordlen / 2 : 0;
  ioi_wordlen += CIP_ELEMENT_NBYTES(element) / 2;

  ioi_sym[i++] = ioi_wordlen;
  ioi_sym[i++] = CIP_SYMBOLIC_SEGMENT;
  ioi_sym[i++] = ioi_bytelen;

  /* Copy the IOI tagname into the IOI symbolic segment */
  for(j = 0; j < ioi_bytelen; j++)
    ioi_sym[i++] = ioi[j];

  ioi_sym_bytelen = i;                      /* Save length of IOI symbol */

  ioi_sym_bytelen = ((ioi_bytelen % 2) == 0) ? ioi_sym_bytelen : ioi_sym_bytelen + 1;

  i = 0;

  /* Build a buffer containing a request for an unconnected send */
  buf[i++] = CIP_UNCONNECTED_SEND;
  buf[i++] = 0x02;                          /* 2 words in following IOI seg */

  /* Route msg through the connection manager */
  buf[i++] = CIP_CLASS_SEGMENT1;
  buf[i++] = CIP_CM_CLASS_CODE;
  buf[i++] = CIP_INSTANCE_SEGMENT1;
  buf[i++] = CIP_CM_INSTANCE_CODE;

  /* Encode the connection timeout */
  buf[i++] = CIP_CONN_TICK_TIME;
  buf[i++] = CIP_CONN_TICK_TIMEOUT;

  /* Bytelen of embedded msg = IOI symbol length + byte for CIP_DATA_WRITE +
     2-6 bytes for the array element dependent on element size +
     2 bytes for type + 2 bytes for size +
     2 bytes for each 16 bit word of each data value (independent of type).
     The PDS API only uses 16 bit words, hence type is irrelevant */
  mr_bytelen = ioi_sym_bytelen + 5 + CIP_ELEMENT_NBYTES(element) + (2 * size);

  buf[i++] = PDS_GETLOBYTE(mr_bytelen);
  buf[i++] = PDS_GETHIBYTE(mr_bytelen);

  /* Embedded MR msg for an unconnected write - CIP Write Data Service */
  buf[i++] = CIP_DATA_WRITE;

  /* Copy the IOI symbolic segment into the buffer */
  for(j = 0; j < ioi_sym_bytelen; j++)
    buf[i++] = ioi_sym[j];

  /* Encode the element segment dependent on the size of the element.
     N.B.: The returned byte count includes the IOI segment ID + any padding */
  switch(CIP_ELEMENT_NBYTES(element))
  {
    case 2 :
      /* The IOI element segment (1 byte version) */
      buf[i++] = CIP_ELEMENT_SEGMENT1;
      buf[i++] = PDS_GETBYTE0(element);
    break;

    case 4 :
      /* The IOI element segment (2 byte version) */
      buf[i++] = CIP_ELEMENT_SEGMENT2;
      buf[i++] = 0x00;
      buf[i++] = PDS_GETBYTE0(element);
      buf[i++] = PDS_GETBYTE1(element);
    break;

    case 6 :
      /* The IOI element segment (4 byte version) */
      buf[i++] = CIP_ELEMENT_SEGMENT4;
      buf[i++] = 0x00;
      buf[i++] = PDS_GETBYTE0(element);
      buf[i++] = PDS_GETBYTE1(element);
      buf[i++] = PDS_GETBYTE2(element);
      buf[i++] = PDS_GETBYTE3(element);
    break;
  }

  /* Encode the type (lobyte-hibyte) */
  buf[i++] = PDS_GETLOBYTE(type);
  buf[i++] = PDS_GETHIBYTE(type);

  /* Determine the number of words (16 bit or 32 bit) dependent on data type */
  nwords = ((type == CIP_BOOL_TYPE || type == CIP_SINT_TYPE || type == CIP_INT_TYPE) ? size : (size ? size / 2 : 0));

  /* Encode the size (lobyte-hibyte) */
  buf[i++] = PDS_GETLOBYTE(nwords);
  buf[i++] = PDS_GETHIBYTE(nwords);

  for(j = 0; j < size; j++)
  {
    /* Encode the data value according to type */
    switch(type)
    {
      case CIP_BOOL_TYPE :
        buf[i++] = values[j] ? 0xff : 0x00;
        buf[i++] = 0x00;
      break;

      case CIP_SINT_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = 0x00;
      break;

      case CIP_INT_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
      break;

      case CIP_DINT_TYPE :
      case CIP_REAL_TYPE :
      case CIP_BIT_ARRAY_TYPE :
        buf[i++] = PDS_GETLOBYTE(values[j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
        buf[i++] = PDS_GETLOBYTE(values[++j]);
        buf[i++] = PDS_GETHIBYTE(values[j]);
      break;
    }
  }

  /* Parse, & get the size of, routing path (in bytes) */
  path_bytelen = (unsigned char) cip_parse_plc_routing_path(path, pathbuf);

  /* Size of routing path (in words) */
  buf[i++] = (path_bytelen ? path_bytelen / 2 : 0);
  buf[i++] = 0x00;

  /* Encode the routing path */
  memcpy(&buf[i], pathbuf, path_bytelen);
  i += path_bytelen;

  /* Calc. the length of the CPF & the overall encapsulated data */
  cpf.data.length = i;
  header.length = sizeof(send_rr_data) + sizeof(cpf) + i;

  i = 0;

  /* Build a buffer containing a CIP encapsulated query */
  memcpy(query, &header, sizeof(header));
  i = sizeof(header);
  memcpy(&query[i], &send_rr_data, sizeof(send_rr_data));
  i+= sizeof(send_rr_data);
  memcpy(&query[i], &cpf, sizeof(cpf));
  i+= sizeof(cpf);
  memcpy(&query[i], buf, cpf.data.length);
  i+= cpf.data.length;

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
int cip_run_plc_query(int fd, pdstrans *trans)
{
  fd_set fds;
  struct timeval tv;
  int nbytes = 0;

  /* N.B.: CIP likes to connect (TCP/IP) once, register once & then fire off
           multiple queries on the same connection.  This is different to how
           other drivers work (e.g., MB) which connect/disconnect for each
           query.  For this reason, we ignore the conn struct's fd & simply
           use this driver's global CIP specific fd instead */

  fd = __cip_fd;

  FD_ZERO(&fds);
  tv.tv_sec = CIP_TMO_SECS;
  tv.tv_usec = CIP_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the fd is ready to write data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to send on socket\n", PROGNAME);
    return -1;
  }

  if((nbytes = send(fd, trans->buf, trans->blen, 0)) < trans->blen)
  {
    err(errout, "%s: error sending on socket\n", PROGNAME);
    err(errout, "%s: blen = %d, nbytes = %d\n", PROGNAME, trans->blen,
    nbytes);
    return -1;
  }
  FD_SET(fd, &fds);

  /* Wait for response to return on fd */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
  {
    err(errout, "%s: error selecting to recv on socket\n", PROGNAME);
    return -1;
  }
  trans->blen = 0;
  memset(&trans->buf, 0, CIP_MAXBUFLEN);

  return (trans->blen = recv(fd, trans->buf, CIP_MAXBUFLEN, 0));
}



/******************************************************************************
* Function to instantiate a prepared PLC query                                *
*                                                                             *
* Pre-condition:  Open fd & a transaction struct containing a prepared query  *
*                 are passed to the function                                  *
* Post-condition: The runtime variables in the query (trans. ID, SID)         *
*                 are instantiated.  The trans. ID is returned or -1 on error *
******************************************************************************/
short int cip_instantiate_prepared_query(int fd, pdstrans *trans)
{
  /* Set this transaction's ID */
  trans->trans_id = __cip_trans_id++;

  /* Copy the query into the comms buffer & set any runtime parameters */
  memcpy(trans->buf, trans->query, trans->qlen);
  trans->blen = trans->qlen;
  CIP_SET_SID(&__cip_sid, trans->buf);

  return trans->trans_id;
}



/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  Open fd & a transaction struct containing a valid response  *
*                 are passed to the function                                  *
* Post-condition: The response is cleaned (encapsulated header etc. removed). *
*                 The response length is modified accordingly and returned or *
*                 -1 on error                                                 *
******************************************************************************/
short int cip_clean_plc_response(int fd, pdstrans *trans)
{
  /* N.B.: Whereas with other protocols (ModBus etc.) this is where we'd
           strip off any transport dependent prefix/postfix to leave us
           with just the 'pure' protocol, CIP (because of its encapsulation)
           is by definition transport independent so we just copy the comms
           buffer straight into the response buffer */
  memcpy(trans->response, trans->buf, trans->blen);
  trans->rlen = trans->blen;

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
int cip_check_plc_response(pdstrans *trans)
{
  int excode = 0;

  /* Check that the response service code is the one we expected */
  if(trans->response[CIP_RESP_SERVICE_CODE_BYTE] != (trans->query[CIP_QUERY_SERVICE_CODE_BYTE] | CIP_CMD_REPLY_FLAG))
  {
    excode = CIP_SERVICE_CODE_MISMATCH;
  }
  else
  {
    /* Check for an error in the response, including extended status words */
    if(trans->response[CIP_STATUS_BYTE] != CIP_STATUS_SUCCESS)
    {
      if(trans->response[CIP_EXT_STATUS_LEN] == 0)
      {
        excode = trans->response[CIP_STATUS_BYTE];
      }
      else
      {
        excode = PDS_MAKEWORD32(trans->response[CIP_EXT_STATUS+3], trans->response[CIP_EXT_STATUS+2], trans->response[CIP_EXT_STATUS+1], trans->response[CIP_EXT_STATUS]);
      }
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
int cip_refresh_status_tags(pdsconn *conn, pdstrans *trans)
{
  int updated = 0;

  /* Set the status word for this PLC's tags */
  updated = set_tags_status(conn, *trans->status);

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
int cip_refresh_data_tags(pdsconn *conn, pdstrans *trans)
{
  short int nbytes = 0, nvalues = 0;
  pdstag *tag = NULL;
  register unsigned short int i = 0, type = 0;

  /* Find the number of bytes in the data of this response */
  nbytes = PDS_MAKEWORD(trans->response[CIP_DATA_LEN+1], trans->response[CIP_DATA_LEN]) - CIP_DATA_RESP_PREFIX;

  if(nbytes <= 0)
  {
    err(errout, "%s: response buffer contained no data\n", PROGNAME);
    return -1;
  }

  /* Get pointer to the beginning of this block in shared memory */
  tag = trans->block_start;

  /* Get the data type of the values */
  type = PDS_MAKEWORD(trans->response[CIP_TYPE+1], trans->response[CIP_TYPE]);

  /* Write the block's configured registers' values into shared memory */
  switch(type)
  {
    case CIP_BOOL_TYPE :
      nvalues = nbytes;           /* Each datum is stored in a byte */

      for(i = 0; i < nvalues; i++) 
      {
        if(i == tag->ref) 
        { 
          tag->value = (trans->response[CIP_DATA+i] == 0xff ? 1 : 0);
          tag->mtime = (time_t) time(NULL);
          tag++;
        }
      }
    break;

    case CIP_SINT_TYPE :
      nvalues = nbytes;           /* Each datum is stored in a byte */

      for(i = 0; i < nvalues; i++) 
      {
        if(i == tag->ref) 
        { 
          tag->value = trans->response[CIP_DATA+i];
          tag->mtime = (time_t) time(NULL);
          tag++;
        }
      }
    break;

    case CIP_INT_TYPE :
      nvalues = nbytes / 2;       /* Each datum is stored in 2 bytes */

      for(i = 0; i < nvalues; i++) 
      {
        if(i == tag->ref) 
        { 
          tag->value = PDS_MAKEWORD(trans->response[CIP_DATA+1+i+i],
          trans->response[CIP_DATA+i+i]);
          tag->mtime = (time_t) time(NULL);
          tag++;
        }
      }
    break;

    case CIP_DINT_TYPE :
    case CIP_REAL_TYPE :
      nvalues = nbytes / 4;       /* Each datum is stored in 4 bytes */

      for(i = 0; i < nvalues; i++) 
      {
        /* N.B.: Internal storage in the PDS is 16 bit words so to store a
                 32 bit value it MUST span 2 tags -- hiword loword */
        if(i == tag->ref) 
        { 
          tag->value = PDS_MAKEWORD(trans->response[CIP_DATA+3+i+i+i+i],
          trans->response[CIP_DATA+2+i+i+i+i]);
          tag->mtime = (time_t) time(NULL);
          tag++;

          /* Ensure next tag is configured */
          if(tag && (i == tag->ref))
          {
            tag->value = PDS_MAKEWORD(trans->response[CIP_DATA+1+i+i+i+i],
            trans->response[CIP_DATA+i+i+i+i]);
            tag->mtime = (time_t) time(NULL);
            tag++;
          }
          else
          {
            err(errout, "%s: hiword/loword (32 bit) reference mismatch\n", PROGNAME);
            return -1;
          }
        }
      }
    break;

    case CIP_STRUCT_TYPE :
    case CIP_BIT_ARRAY_TYPE :
    default :
      /* Data type not supported so log error */
      err(errout, "%s: data type %x not supported yet\n", PROGNAME, type);
      return -1;
    break;
  }

  return nvalues;
}



/******************************************************************************
* Function to parse the PLC routing path                                      *
*                                                                             *
* Pre-condition:  The PDS routing path string & a buffer to store the CIP     *
*                 encoded routing path are passed to the function             *
* Post-condition: The routing path is parsed & encoded as a CIP routing path. *
*                 The length of the routing path in bytes is returned         *
******************************************************************************/
int cip_parse_plc_routing_path(const char *path, unsigned char *pathbuf)
{
  int i = 0, len = 0;

  for(i = 0, len = 0; i < strlen(path); i++)
  {
    /* To encode to a CIP routing path, remove the path separators & encode
       the ASCII number representations as proper binary numbers.  Each
       element of the routing path is stored in a single byte */
    if(path[i] != PDS_PLC_PATH_SEP)
      pathbuf[len++] = PDS_ASCII2BIN(path[i]);
  }

  return len;
}



/******************************************************************************
* Function to connect to a PLC (CIP specific)                                 *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: A connection is established with the PLC.  If an error      *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int cip_connect_to_plc(pdsconn *conn)
{
  pdstrans regtrans;

  memset(&regtrans, 0, sizeof(pdstrans));

  /* If socket is not open or an error has occurred, open a new connection */
  if(__cip_fd <= 0 || (PDS_CHECK_PLC_STATUS(conn) == PDS_PLC_COMMSERR || PDS_CHECK_PLC_STATUS(conn) == PDS_PLC_CONNERR))
  {
    /* First close the old socket connection with the PLC */
    if(close(__cip_fd) == -1)
    {
      err(errout, "%s: error closing socket to %s:%u:%s\n", PROGNAME, conn->ip_addr, conn->port, conn->path);
    }

    __cip_fd = 0;
 
    /* Connect the server to the PLC */
    if((__cip_fd = open_plc_socket(conn->ip_addr, conn->port)) == -1)
    {
      err(errout, "%s: error opening socket to %s:%u:%s\n", PROGNAME, conn->ip_addr, conn->port, conn->path);
      return -1;
    }

    /* Construct the PLC query to register with CIP */
    if((regtrans.blen = cip_construct_register_plc_query(regtrans.buf)) == -1)
    {
      err(errout, "%s: error constructing register query\n", PROGNAME);
    }
    else
    {
      if(cip_run_plc_query(__cip_fd, &regtrans) == -1)
      {
        err(errout, "%s: error running register query\n", PROGNAME);
      }
      else
      {
        /* Get the returned SID & store it */
        CIP_GET_SID(regtrans.buf, &__cip_sid);
      }
    }
  }

  return __cip_fd;
}



/******************************************************************************
* Function to disconnect from a PLC (CIP specific)                            *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: The connection is finished with the PLC.  If an error       *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int cip_disconnect_from_plc(pdsconn *conn)
{
  int retval = 0;
  pdstrans regtrans;

  memset(&regtrans, 0, sizeof(pdstrans));

  /* If socket is open and an error has occurred, disconnect */
  if(__cip_fd > 0 && (PDS_CHECK_PLC_STATUS(conn) == PDS_PLC_COMMSERR || PDS_CHECK_PLC_STATUS(conn) == PDS_PLC_CONNERR))
  {
    retval = -1;

    /* Construct the PLC query to unregister with CIP.  If the unregister
       fails, it's not disasterous so we just log the fact */
    if((regtrans.blen = cip_construct_unregister_plc_query(regtrans.buf, __cip_sid)) == -1)
    {
      err(errout, "%s: error constructing unregister query\n", PROGNAME);
    }
    else
    {
      if(cip_run_plc_query(__cip_fd, &regtrans) == -1)
        err(errout, "%s: error running unregister query\n", PROGNAME);
    }

    /* Close the socket connection with the PLC */
    if((retval = close(__cip_fd)) == -1)
    {
      err(errout, "%s: error closing socket to %s:%u:%s\n", PROGNAME, conn->ip_addr, conn->port, conn->path);
    }

    __cip_fd = 0;
  }

  return retval;
}

