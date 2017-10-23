/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_spi.c                                                         *
* PURPOSE:  The PLC data server SPI module                                    *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-01                                                        *
******************************************************************************/

#include "pds_spi.h" 

/******************************************************************************
* Function to connect a client to the server SPI                              *
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
pds_spi_conn* PDS_SPIconnect(key_t connkey)
{
  pds_spi_conn *conn = (pds_spi_conn *) malloc(sizeof(pds_spi_conn));
  struct shmid_ds shmseg;
  int ntrys = 0;

  if(!conn)
  {
    return (pds_spi_conn *) NULL;
  }

  memset(conn, 0, sizeof(pds_spi_conn));
  memset(&shmseg, 0, sizeof(shmseg));

  /******************** Initialise the connection struct *********************/

  /* Set the Front-end/Back-end protocol version to ensure compatibility */
  conn->febe_proto_ver = PDS_FEBE_PROTO_VER;

  /* Set the connection status initially to error */
  conn->conn_status = PDS_CONN_CONNERR;

  if(connkey)
    conn->shmkey = connkey;
  else
    conn->shmkey = (int) PDS_SPI_IPCKEY; 

  /* Get the ID for the SPI shared memory segment */
  if((conn->shmid = shmget(conn->shmkey, 0, 0)) == -1)
  {
    conn->conn_status |= PDS_CONN_SHMCONN_ERR;
    return conn;                       /* Return error */
  }

  /* Attempt to attach to the server's SPI shared memory segment */
  for(ntrys = 0; ntrys <= PDS_MAX_NTRYS; ntrys++)
  {
    if((conn->shm = shmat(conn->shmid, (void *) 0, 0)) == (void *) -1)
    {
      if(ntrys == PDS_MAX_NTRYS)
      {
        conn->conn_status |= PDS_CONN_SHMCONN_ERR;
        return conn;                   /* Return error */
      }
      else
        usleep(PDS_CONN_PAUSE * ntrys);
    }
    else
      break;
  }

  /* Assign a SPI tag pointer to the start of the shm */
  conn->data = (pds_spi_tag *) conn->shm;

  /* Get info about the shm segment to calculate the number of SPI tags */
  if(shmctl(conn->shmid, IPC_STAT, &shmseg) == -1)
  {
    conn->conn_status |= PDS_CONN_SHMCONN_ERR;
    return conn;                       /* Return error */
  }

  conn->ndata_tags = (shmseg.shm_segsz > 0 ? (shmseg.shm_segsz / sizeof(pds_spi_tag)) : 0);

  /* Finally set the connection status to OK */
  conn->conn_status = PDS_CONN_OK;

  return conn;
}



/******************************************************************************
* Function to disconnect a client from the server SPI                         *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurrs a -1 is returned                              *
******************************************************************************/
int PDS_SPIdisconnect(pds_spi_conn *conn)
{
  if(conn)
  {
    /* Attempt to detach from the server's SPI shared memory segment */
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
* Function to get an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname & an int pointer for *
*                 storage of the tag's value are passed to the function       *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int PDS_SPIget_tag(pds_spi_conn *conn, const char *tagname, int *tagvalue)
{
  pds_spi_tag *tag = NULL;
  int len = strlen(tagname), retval = -1, i = 0;

  if(conn)
  {
    /* Search for tagname ensuring string is not just a substring of tag */
    for(tag = conn->data, i = 0; tag && i < conn->ndata_tags; tag++, i++)
    {
      if(memcmp(tag->name, tagname, len) == 0)
      {
        if(len == strlen(tag->name)) 
        {
          *tagvalue = tag->value;
          retval = 0;
          break;
        }
      }
    }
  }

  return retval;
}



/******************************************************************************
* Function to get a pointer to an SPI tag's value                             *
*                                                                             *
* Pre-condition:  A valid server connection & the tagname are passed to the   *
*                 function                                                    *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 a pointer to its value is returned.  On error a -1 is       *
*                 returned                                                    *
******************************************************************************/
int* PDS_SPIget_tag_ptr(pds_spi_conn *conn, const char *tagname)
{
  pds_spi_tag *tag = NULL;
  int len = strlen(tagname), i = 0;
  int *tagvalue = (int *) -1;

  if(conn)
  {
    /* Search for tagname ensuring string is not just a substring of tag */
    for(tag = conn->data, i = 0; tag && i < conn->ndata_tags; tag++, i++)
    {
      if(memcmp(tag->name, tagname, len) == 0)
      {
        if(len == strlen(tag->name)) 
        {
          tagvalue = &tag->value;
          break;
        }
      }
    }
  }

  return tagvalue;
}



/******************************************************************************
* Function to set an SPI tag value                                            *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname & the tag value      *
*                 are passed to the function                                  *
* Post-condition: The tag value is set to the value passed in the parameter.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int PDS_SPIset_tag(pds_spi_conn *conn, const char *tagname, int tagvalue)
{
  pds_spi_tag *tag = NULL;
  int len = strlen(tagname), retval = -1, i = 0;

  if(conn)
  {
    /* Search for tagname ensuring string is not just a substring of tag */
    for(tag = conn->data, i = 0; tag && i < conn->ndata_tags; tag++, i++)
    {
      if(memcmp(tag->name, tagname, len) == 0)
      {
        if(len == strlen(tag->name)) 
        {
          tag->value = tagvalue;
          retval = 0;
          break;
        }
      }
    }
  }

  return retval;
}

