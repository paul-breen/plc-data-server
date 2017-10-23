/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pdsnp_api.c                                                       *
* PURPOSE:  The PDS network protocol API module                               *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#include "pdsnp_api.h"

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
pdsconn* PDSNPconnect(const char *host, unsigned short int port)
{
  pdsconn *conn = (pdsconn *) malloc(sizeof(pdsconn));
  const char *phost = NULL;

  if(!conn)
  {
    return (pdsconn *) NULL;
  }

  memset(conn, 0, sizeof(pdsconn));
 
  /* Set the connection status initially to error */
  conn->conn_status = PDS_CONN_CONNERR;

  /* Do sanity checks on host - if no host, use default */
  phost = ((host == NULL) || (strcmp(host, "") == 0)) ? PDSNP_DEF_HOST : host;

  /* Connect to the PDS network stub */
  if((conn->fd = open_client_socket(phost, port)) == -1)
  {
    return conn;
  }
  else
  {
    /* Assign the network stub host and port to the connection struct */
    strncpy(conn->ip_addr, phost, PDS_IP_ADDR_LEN - 1);
    conn->port = port;

    /* Finally set the connection status to OK */
    conn->conn_status = PDS_CONN_OK;
  }

  return conn;
}



/******************************************************************************
* Function to disconnect a client from the server                             *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurrs a -1 is returned                              *
******************************************************************************/
int PDSNPdisconnect(pdsconn *conn)
{
  if(conn)
  {
    /* Disconnect from the PDS network stub */
    close(conn->fd);
    free(conn);
    return 0;
  }
  else
    return -1;
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
int PDSNPget_tag(pdsconn *conn, const char *tagname, char *tagvalue)
{
  pdscomms comms;
  int retval = -1;

  if(conn)
  {
    memset(&comms, 0, sizeof(pdscomms));

    PDSNP_SET_BUF_LEN(comms.buf, PDSNP_LEN);
    PDSNP_SET_VER(comms.buf, PDSNP_VER);
    PDSNP_SET_FUNC_ID(comms.buf, PDSNP_GET_TAG_FUNC_ID);
    PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_OK);
    PDSNP_SET_TAGNAME(comms.buf, tagname);
    PDSNP_SET_TAGVALUE(comms.buf, "0");

    comms.conn = conn;

    /* Send the request to get the value for the given tag */
    if(comms_write(conn->fd, &comms) < 0)
    {
      PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_WR_ERR);
      conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);
    }
    else
    {
      if(comms_read(conn->fd, &comms) < 0)
      {
        PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_RD_ERR);
        conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);
      }
      else
      {
        /* Propagate comms status to the client */
        conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);

        /* Store the tagvalue in the string */
        PDSNP_GET_TAGVALUE(tagvalue, comms.buf);

        retval = 0;
      }
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
int PDSNPset_tag(pdsconn *conn, const char *tagname, short int ntags,
                 const short int *tagvalues)
{
  pdscomms comms;
  char tagvalue[PDSNP_TAGVALUE_LEN+1] = "\0";
  int retval = -1;

  if(conn)
  {
    memset(&comms, 0, sizeof(pdscomms));

    PDSNP_SET_BUF_LEN(comms.buf, PDSNP_LEN);
    PDSNP_SET_VER(comms.buf, PDSNP_VER);
    PDSNP_SET_FUNC_ID(comms.buf, PDSNP_SET_TAG_FUNC_ID);
    PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_OK);
    PDSNP_SET_TAGNAME(comms.buf, tagname);
    sprintf(tagvalue, "%u", *tagvalues);
    PDSNP_SET_TAGVALUE(comms.buf, tagvalue);

    comms.conn = conn;

    /* Send the request to set the value for the given tag(s) */
    if(comms_write(conn->fd, &comms) < 0)
    {
      PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_WR_ERR);
      conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);
    }
    else
    {
      if(comms_read(conn->fd, &comms) < 0)
      {
        PDSNP_SET_EX_CODE(comms.buf, PDSNP_COMMS_RD_ERR);
        conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);
      }
      else
      {
        /* Propagate comms status to the client */
        conn->plc_status = PDSNP_GET_EX_CODE(comms.buf);

        retval = 0;
      }
    }
  }

  return retval;
}

