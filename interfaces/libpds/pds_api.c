/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_api.c                                                         *
* PURPOSE:  The PLC data server C API module                                  *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-09-23                                                        *
******************************************************************************/

#include "pds_api.h" 

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
static int _get_strtagf(pdsconn *conn, const char *tagname, int ntags,
                        char *tagvalue, const char fmt)
{
  pdstag *tag = NULL;
  int len = strlen(tagname), retval = -1, i = 0, j = 0;

  if(tagvalue) tagvalue[0] = '\0';

  /* Search for tagname ensuring string is not just a substring of tag */
  for(tag = conn->data, i = 0; tag && i < conn->ttags; tag++, i++)
  {
    if(memcmp(tag->name, tagname, len) == 0)
    {
      if(len == strlen(tag->name)) 
      {
        /* Build the string of tags and set the query status */
        switch(fmt)
        {
          case 'd' :
          case 'i' :
          default  :                   /* Default - format as an integer */
            for(j = 0; j < ntags && tag && i < conn->ttags; tag++, i++, j++)
            {
              sprintf(tagvalue, "%s%u", tagvalue, tag->value);
              conn->plc_status |= tag->status;
            }
          break;

          case 'f' :                   /* Format as a float */
            for(j = 0; j < ntags && tag && i < conn->ttags; tag++, i++, j++)
            {
              sprintf(tagvalue, "%s%f", tagvalue, (float) tag->value);
              conn->plc_status |= tag->status;
            }
          break;

          case 'c' :                   /* Format as a char */
          case 's' :
            for(j = 0; j < ntags && tag && i < conn->ttags; tag++, i++, j++)
            {
              sprintf(tagvalue, "%s%c%c", tagvalue,
              isprint((tag->value >> 8)) ? (char) (tag->value >> 8) : ' ',
              isprint((tag->value & 0xff)) ? (char) (tag->value & 0xff) : ' ');
              conn->plc_status |= tag->status;
            }
          break;
        }
        retval = 0;
        break;

      }
    }
  }

  return retval;
}



/******************************************************************************
* Internal function to set the value of a semaphore                           *
*                                                                             *
* Pre-condition:  A valid semaphore ID, the value for the operation and the   *
*                 semaphore set array number are passed to the function       *
* Post-condition: The semaphore is set with the passed value.  If an error    *
*                 occurs a -1 is returned                                     *
******************************************************************************/
static int _semset(int id, int op, int snum)
{
  struct sembuf sb;

  sb.sem_num = snum;              /* Set semaphore No. in this semaphore set */
  sb.sem_op = op;                 /* Set the value for this operation */
  sb.sem_flg = SEM_UNDO;          /* Ensure 'rollback' if an error occurs */

  return semop(id, &sb, 1);
}



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
pdsconn* PDSconnect(key_t connkey)
{
  pdsconn *conn = (pdsconn *) malloc(sizeof(pdsconn));
  pdsmsg msg;
  long int msgtype = 0;
  int nbytes = 0, ntrys = 0;
  int msgsize = (sizeof(pdsmsg) - sizeof(long int));

  if(!conn)
  {
    return (pdsconn *) NULL;
  }

  memset(conn, 0, sizeof(pdsconn));
  memset(&msg, 0, sizeof(pdsmsg));

  /******************** Initialise the connection struct *********************/

  /* Set the Front-end/Back-end protocol version to ensure compatibility */
  conn->febe_proto_ver = PDS_FEBE_PROTO_VER;

  /* Set the connection status initially to error */
  conn->conn_status = PDS_CONN_CONNERR;

  /* Send a message to the server requesting initialisation data */

  if(connkey)
    conn->msgkey = connkey;
  else
    conn->msgkey = (int) PDS_MSGKEY;

  conn->msgflags = PDS_MSGFLAGS;

  /* Attempt to connect to the server's message queue */
  for(ntrys = 0; ntrys <= PDS_MAX_NTRYS; ntrys++)
  {
    if((conn->msgid = msgget(conn->msgkey, conn->msgflags)) == -1)
    {
      if(ntrys == PDS_MAX_NTRYS)
      {
        conn->conn_status |= PDS_CONN_MSGCONN_ERR;
        return conn;
      }
      else 
        usleep(PDS_CONN_PAUSE * ntrys);
    }
    else
      break;
  }

  msg.msgtype = PDS_INITMSG;

  /* Attempt to send the 'request for init.' message to the server */
  for(ntrys = 0; ntrys <= PDS_MAX_NTRYS; ntrys++)
  {
    if((nbytes = msgsnd(conn->msgid, (void *) &msg, msgsize, 0)) == -1)
    {
      if(ntrys == PDS_MAX_NTRYS)
      {
        conn->conn_status |= PDS_CONN_MSGSEND_ERR;
        return conn;
      }
      else
        usleep(PDS_CONN_PAUSE * ntrys);
    }
    else
      break;
  }

  msgtype = PDS_INITMSG_RESP;

  /* Attempt to receive the server response message */
  for(ntrys = 0; ntrys <= PDS_MAX_NTRYS; ntrys++)
  {
    if((nbytes = msgrcv(conn->msgid, (void *) &msg, msgsize, msgtype, 0)) < msgsize)
    {
      if(ntrys == PDS_MAX_NTRYS)
      {
        conn->conn_status |= PDS_CONN_MSGRECV_ERR;
        return conn;
      }
      else
        usleep(PDS_CONN_PAUSE * ntrys);
    }
    else
      break;
  }

  /* Check compatibility of the client's Front-end/Back-end protocol version
     with that advertised by the server */
  if(conn->febe_proto_ver != msg.febe_proto_ver)
  {
    conn->conn_status |= PDS_CONN_PROTO_ERR;
    return conn;
  }

  /* Set the IPC parameters as returned by the server */
  conn->semid = msg.semid;
  conn->shmid = msg.shmid;
  conn->ndata_tags = msg.ndata_tags;
  conn->nstatus_tags = msg.nstatus_tags;
  conn->ttags = (conn->ndata_tags + conn->nstatus_tags);
  conn->plc_status = PDS_PLC_OK;

  /* Attempt to attach to the server's shared memory segment */
  for(ntrys = 0; ntrys <= PDS_MAX_NTRYS; ntrys++)
  {
    if((conn->shm = shmat(conn->shmid, (void *) 0, 0)) == (void *) -1)
    {
      if(ntrys == PDS_MAX_NTRYS)
      {
        conn->conn_status |= PDS_CONN_SHMCONN_ERR;
        return conn;
      }
      else
        usleep(PDS_CONN_PAUSE * ntrys);
    }
    else
      break;
  }

  /* Assign tag pointers to the start of the data & status tags */
  conn->data = (pdstag *) conn->shm;
  conn->status = (pdstag *) (conn->shm + (conn->ndata_tags * sizeof(pdstag)));

  /* Finally set the connection status to OK */
  conn->conn_status = PDS_CONN_OK;

  return conn;
}



/******************************************************************************
* Function to disconnect a client from the server                             *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int PDSdisconnect(pdsconn *conn)
{
  if(conn)
  {
    /* Detach from the server's shared memory segment */
    if(shmdt(conn->shm) == -1)
    {
      free(conn);
      return -1;
    }
    else
    {
      free(conn);
      return 0;
    }
  }
}



/******************************************************************************
* Function to get a tag's value                                               *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and a string for     *
*                 storage of the tag's value are passed to the function       *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int PDSget_tag(pdsconn *conn, const char *tagname, char *tagvalue)
{
  pdstag *tag = NULL;
  int len = strlen(tagname), retval = -1, i = 0;

  if(conn)
  {
    conn->plc_status = 0;
    if(tagvalue) tagvalue[0] = '\0';

    /* Attempt to hold the semaphore */ 
    if(_semset(conn->semid, PDS_SEMHLD, 0) != -1)
    { 
      /* Search for tagname ensuring string is not just a substring of tag */
      for(tag = conn->data, i = 0; tag && i < conn->ttags; tag++, i++)
      {
        if(memcmp(tag->name, tagname, len) == 0)
        {
          if(len == strlen(tag->name)) 
          {
            /* Copy value into string and set the query's status */
            sprintf(tagvalue, "%u", tag->value);
            conn->plc_status |= tag->status;

            retval = 0;
            break;
          }
        }
      }
      /* Release the semaphore */
      _semset(conn->semid, PDS_SEMREL, 0);
    }
  }

  return retval;
}



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
                const char fmt)
{
  pdstag *tag = NULL;
  int retval = -1;

  if(conn)
  {
    conn->plc_status = 0;

    /* Attempt to hold the semaphore */ 
    if(_semset(conn->semid, PDS_SEMHLD, 0) != -1)
    { 
      /* Get this tag's data (formatted) */
      retval = _get_strtagf(conn, tagname, 1, tagvalue, fmt);

      /* Release the semaphore */
      _semset(conn->semid, PDS_SEMREL, 0);
    }
  }

  return retval;
}



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
                  char *tagvalue, const char fmt)
{
  pdstag *tag = NULL;
  int retval = -1;

  if(!tagvalue)                        /* Check tagvalue is valid storage */
    return retval;

  if(conn)
  {
    conn->plc_status = 0;

    /* Attempt to hold the semaphore */ 
    if(_semset(conn->semid, PDS_SEMHLD, 0) != -1)
    { 
      /* Get this tag's data (formatted) */
      retval = _get_strtagf(conn, tagname, ntags, tagvalue, fmt);

      /* Release the semaphore */
      _semset(conn->semid, PDS_SEMREL, 0);
    }
  }

  return retval;
}



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
               const unsigned short int *tagvalues)
{
  int msgsize = (sizeof(pdsmsg) - sizeof(long int));
  long int msgtype = 0;
  pdsmsg msg;
  int nbytes = 0, retval = -1;

  if(conn)
  {
    memset(&msg, 0, sizeof(pdsmsg));
    conn->plc_status = 0;

    /* Set up the message queue struct */
    msg.msgtype = PDS_WRMSG;
    strncpy(msg.tag.name, tagname, PDS_TAGNAME_LEN - 1);
    msg.ntags = ntags;
    memcpy(msg.tagvalues, tagvalues, (ntags * sizeof(unsigned short int)));

    if(PDSget_tag_status(conn, tagname, &conn->plc_status) != -1)
    {
      /* Only write data to PLC if PLC is ONLINE */
      if((PDS_CHECK_PLC_STATUS(conn) & PDS_PLC_OFFLINE) != PDS_PLC_OFFLINE)
      {
        /* Send the message */
        if((nbytes = msgsnd(conn->msgid, (void *) &msg, msgsize, 0)) != -1)
        {
          memset(&msg, 0, sizeof(pdsmsg));
          msgtype = PDS_WRMSG_RESP;

          /* Receive the server's response (status word is set) */
          nbytes = msgrcv(conn->msgid, (void *) &msg, msgsize, msgtype, 0);

          if(nbytes == msgsize)
          {
            retval = 0;

            /* Return to the client the status of the PLC as returned by the 
               server */
            conn->plc_status = msg.tag.status;
          }
        }
      }
    }
  }

  return retval;
}



/******************************************************************************
* Function to set a string of tags' value                                     *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname (base) and the tags' *
*                 value represented as a string are passed to the function    *
* Post-condition: A message is sent to the server requesting that the tag     *
*                 value be written to the PLC.  On error a -1 is returned     *
******************************************************************************/
int PDSset_strtag(pdsconn *conn, const char *tagname, const char *tagvalue) 
{
  unsigned short int tagvalues[PDS_NTAGVALUES];
  short int ntags = 0, len = strlen(tagvalue);
  register int i = 0, j = 0;

  /* Calculate the number of contiguous tags needed to hold this string
     (each tag holds PDS_WORDSIZE bytes) */
  if(len == 0)
  {
    return -1;
  }
  else
  {
    ((len % PDS_WORDSIZE) == 0) ? (ntags = len / PDS_WORDSIZE) : (ntags = (len / PDS_WORDSIZE) + 1);
  }

  /* Encode the hi and lo byte of each tag as an unsigned short integer */
  for(i = 0, j = 0; i < ntags; i++, j += PDS_WORDSIZE)
    tagvalues[i] = PDS_MAKEWORD(tagvalue[j], tagvalue[j+1]);

  return PDSset_tag(conn, tagname, ntags, tagvalues);
}



/******************************************************************************
* Function to get a flag's state                                              *
*                                                                             *
* Pre-condition:  A valid server connection and the tagname of the flag to    *
*                 get are passed to the function                              *
* Post-condition: The flag's value is returned.  On error a -1 is returned    *
******************************************************************************/
int PDSget_flag_state(pdsconn *conn, char *tagname)
{
  char tagvalue[PDS_TAGVALUE_LEN] = "\0";

  return((PDSget_tag(conn, tagname, tagvalue) == -1) ? -1 : atoi(tagvalue));
}



/******************************************************************************
* Function to set a flag's state                                              *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname of the flag to set   *
*                 and the value to set are passed to the function             *
* Post-condition: The flag's value is set.  On error a -1 is returned         *
******************************************************************************/
int PDSset_flag_state(pdsconn *conn, char *tagname,
                      unsigned short int tagvalue)
{
  return PDSset_tag(conn, tagname, 1, &tagvalue);
}



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
int PDSget_taglist(pdsconn *conn, plctaglist *taglist)
{
  register int i = 0, x = -1;
  plctag *tag = NULL;

  if(conn)
  {
    conn->plc_status = 0;

    /* Attempt to hold the semaphore */ 
    if(_semset(conn->semid, PDS_SEMHLD, 0) != -1)
    { 
      /* Cycle through the taglist and get each tag's value (formatted) */
      for(i = 0, x = 0, tag = taglist->tags; i < taglist->ntags; i++, tag++) 
      {
        if(_get_strtagf(conn, tag->name, 1, tag->value, tag->type) != -1)
        {
          x++;
        }
        else
        {
          x = -1;
          break;
        }
      }
      /* Release the semaphore */
      _semset(conn->semid, PDS_SEMREL, 0);
    }
  }

  return x;
}



/******************************************************************************
* Function to get a bit's state within a word                                 *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and the bit number   *
*                 within the word (0-15) to get are passed to the function    *
* Post-condition: The bit's value is returned.  On error a -1 is returned     *
******************************************************************************/
int PDSget_wordbit_state(pdsconn *conn, char *tagname, int bitno)
{
  char tagvalue[PDS_TAGVALUE_LEN] = "\0";
  unsigned short int nval = 0;
  unsigned char hibyte = 0, lobyte = 0;
  int bitvalue = -1;

  if(bitno >= 0 && bitno < 16)
  {
    if(PDSget_tag(conn, tagname, tagvalue) != -1)
    {
      nval = atoi(tagvalue);
      hibyte = PDS_GETHIBYTE(nval);
      lobyte = PDS_GETLOBYTE(nval);

      /* Get the bit from the appropriate byte */
      if(bitno > 7)
        bitvalue = PDS_GETBIT(hibyte, (bitno - 7));
      else
        bitvalue = PDS_GETBIT(lobyte, bitno);
    }
  }

  return bitvalue;
}



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
                         int bitvalue)
{
  unsigned char hibyte = 0, lobyte = 0;
  int retval = -1;

  if(bitno >= 0 && bitno < 16)
  {
    hibyte = PDS_GETHIBYTE(*tagvalue);
    lobyte = PDS_GETLOBYTE(*tagvalue);

    /* Set or clear the bit in the appropriate byte */
    if(bitno > 7)
    {
      if(bitvalue)
        hibyte = PDS_SETBIT(hibyte, (bitno - 7));
      else
        hibyte = PDS_CLEARBIT(hibyte, (bitno - 7));
    }
    else
    {
      if(bitvalue)
        lobyte = PDS_SETBIT(lobyte, bitno);
      else
        lobyte = PDS_CLEARBIT(lobyte, bitno);
    }

    /* Set the tag with the modified tagvalue parameter */
    *tagvalue = PDS_MAKEWORD(hibyte, lobyte);
    retval = PDSset_tag(conn, tagname, 1, tagvalue);
  }

  return retval;
}



/******************************************************************************
* Function to get a tag's PLC status                                          *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname and a pointer to     *
*                 store the tag's status are passed to the function           *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its status is stored in status.  On error a -1 is returned  *
******************************************************************************/
int PDSget_tag_status(pdsconn *conn, const char *tagname,
                      unsigned short int *status)
{
  pdstag *tag = NULL;
  int len = strlen(tagname), retval = -1, i = 0;

  if(conn)
  {
    /* Attempt to hold the semaphore */ 
    if(_semset(conn->semid, PDS_SEMHLD, 0) != -1)
    { 
      /* Search for tagname ensuring string is not just a substring of tag */
      for(tag = conn->data, i = 0; tag && i < conn->ttags; tag++, i++)
      {
        if(memcmp(tag->name, tagname, len) == 0)
        {
          if(len == strlen(tag->name)) 
          {
            *status |= tag->status;
            retval = 0;
            break;
          }
        }
      }
      /* Release the semaphore */
      _semset(conn->semid, PDS_SEMREL, 0);
    }
  }

  return retval;
}



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
                         int (*func)(FILE *, pdstag *, void *), void *aux)
{
  pdstag *tag = NULL;
  int i = 0, retval = -1;

  if(conn)
  {
    /* Get a pointer to the start of the data tags */
    if((tag = PDSconn_get_data(conn)))
    {
      if(!(func))
        func = PDS_DEFAULT_PRINT_DATA_TAG_FUNC;

      /* Cycle through the tags passing each one to the callback function */
      for(i = 0; i < PDSconn_get_ndata_tags(conn) && tag; i++, tag++)
      {
        func(os, tag, aux);

        /* Store the overall status */
        conn->plc_status |= PDStag_get_status(tag);
      }

      retval = 0;
    }
  }

  return retval;
}



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
                           int (*func)(FILE *, pdstag *, void *), void *aux)
{
  pdstag *tag = NULL;
  int i = 0, retval = -1;

  if(conn)
  {
    /* Get a pointer to the start of the status tags */
    if((tag = PDSconn_get_status(conn)))
    {
      if(!(func))
        func = PDS_DEFAULT_PRINT_STATUS_TAG_FUNC;

      /* Cycle through the tags passing each one to the callback function */
      for(i = 0; i < PDSconn_get_nstatus_tags(conn) && tag; i++, tag++)
        func(os, tag, aux);

      retval = 0;
    }
  }

  return retval;
}



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
                           int (*func)(FILE *, pdsconn *, void *), void *aux)
{
  int retval = -1;

  if(conn)
  {
    if(!(func))
      func = PDS_DEFAULT_PRINT_FOOTER_FUNC;

    /* Pass the connection to the callback function */
    func(os, conn, aux);

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Internal function to print a data tag's data in a default manner            *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & any        *
*                 auxilliary data are passed to the function.  If the output  *
*                 stream is NULL, a default is used                           *
* Post-condition: The tag's data is printed to the output stream.  The        *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_data_tag(FILE *os, pdstag *tag, void *aux)
{
  char tmpstr[256] = "\0";
  int retval = -1;

  if(tag)
  {
    PDS_GET_TAG_FQID(tmpstr, tag);     /* Construct tag's fully-qualified ID */

    if(!os)
      os = PDS_DEFAULT_PRINT_DATA_TAG_STREAM;

    /* Print the tag's data */
    fprintf(os, "%-50s%10u%10d\n", tmpstr, PDStag_get_value(tag), PDStag_get_ref(tag));

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Internal function to print a status tag's data in a default manner          *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & any        *
*                 auxilliary data are passed to the function.  If the output  *
*                 stream is NULL, a default is used                           *
* Post-condition: The tag's data is printed to the output stream.  The        *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_status_tag(FILE *os, pdstag *tag, void *aux)
{
  char tmpstr[256] = "\0";
  int retval = -1;

  if(tag)
  {
    PDS_GET_TAG_FQID(tmpstr, tag);     /* Construct tag's fully-qualified ID */

    if(!os)
      os = PDS_DEFAULT_PRINT_STATUS_TAG_STREAM;

    /* Print the tag's data */
    fprintf(os, "%-50s%10d%10s\n", tmpstr, PDStag_get_status(tag), PDStag_get_path(tag));

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Internal function to print shared memory statistics in a default manner     *
*                                                                             *
* Pre-condition:  An output stream, the connection struct containing summary  *
*                 data to be printed & any auxilliary data are passed to the  *
*                 function.  If the output stream is NULL, a default is used  *
* Post-condition: The statistics are printed to the output stream.  The       *
*                 auxilliary data is unused.  On error a -1 is returned       *
******************************************************************************/
int _default_print_footer(FILE *os, pdsconn *conn, void *aux)
{
  int retval = -1;

  if(conn)
  {
    if(!os)
      os = PDS_DEFAULT_PRINT_FOOTER_STREAM;

    /* Print the summary footer */
    fprintf(os, "Total number of tags: %d\n", PDSconn_get_ttags(conn));
    fprintf(os, "Number of data tags: %d\n", PDSconn_get_ndata_tags(conn));
    fprintf(os, "Number of status tags: %d\n", PDSconn_get_nstatus_tags(conn));
    fprintf(os, "Query status: %s (%d)\n", PDSprint_plc_status(conn), PDScheck_plc_status(conn));

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Function to get a tag object                                                *
*                                                                             *
* Pre-condition:  A valid server connection & the tagname to search for are   *
*                 passed to the function                                      *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 a pointer to the tag object is returned.  On error a NULL   *
*                 is returned                                                 *
******************************************************************************/
pdstag* PDSget_tag_object(pdsconn *conn, const char *tagname)
{
  pdstag *tag = NULL;
  int len = strlen(tagname), found = 0, i = 0;

  if(conn)
  {
    /* Get a pointer to the start of the data tags */
    if((tag = PDSconn_get_data(conn)))
    {
      /* Search for tagname ensuring string is not just a substring of tag */
      for(i = 0; i < PDSconn_get_ttags(conn) && tag; i++, tag++)
      {
        if(memcmp(tag->name, tagname, len) == 0)
        {
          if(len == strlen(tag->name)) 
          {
            found = 1;
            break;
          }
        }
      }
    }
  }

  return (pdstag *) found ? tag : NULL;
}

