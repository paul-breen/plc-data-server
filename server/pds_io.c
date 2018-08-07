/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_io.c                                                          *
* PURPOSE:  The main (I/O) server functions module                            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#include "pds_srv.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */
extern int dbglvl;                /* Declared in the main file */
extern unsigned int runmode;      /* Declared in the main file */
extern pdstag **block_index;      /* Declared in the mem. management file */

/* Get pointer to 1st tag in specified block from global block index */
#define PDS_GET_BLOCK_START(n)	((pdstag *) block_index[(n)])

/******************************************************************************
* The main (I/O) server function                                              *
*                                                                             *
* Pre-condition:  The PLC configuration struct, the connection struct, the    *
*                 configured SPI tag list & the SPI connection struct are     *
*                 passed to the function                                      *
* Post-condition: The server is configured as per the config file by setting  *
*                 up the necessary process variables in shared memory and     *
*                 connecting to the PLC.  The SPI shared memory is also       *
*                 configured.  If an error occurs a -1 is returned            *
******************************************************************************/
int pds_server(plc_cnf *conf, pdsconn *parent_conn,
               pds_spi_tag_list *spi_tag_list, pds_spi_conn *parent_spi_conn)
{
  pid_t chld = 0;
  pdsconn child_conn;
  pds_spi_conn child_spi_conn;

  memset(&child_conn, 0, sizeof(pdsconn));
  memset(&child_spi_conn, 0, sizeof(pds_spi_conn));

  /* Initialise the server connections */
  if(init_server_connection(conf, parent_conn) == -1)
  {
    err(errout, "%s: error initialising server connection\n", PROGNAME);
    return -1;
  }

  if(init_SPI_server_connection(spi_tag_list, parent_spi_conn) == -1)
  {
    err(errout, "%s: error initialising SPI server connection\n", PROGNAME);
    return -1;
  }

  /* Make copies of the parent process' connections for the child process */
  memcpy(&child_conn, parent_conn, sizeof(pdsconn));
  memcpy(&child_spi_conn, parent_spi_conn, sizeof(pds_spi_conn));

  /* Fork a child process.  The parent handles write requests, the child
     handles read requests thereby giving a concurrent read/write server */

  switch((chld = fork()))
  {
    case -1 :
      err(errout, "%s: error creating child process\n", PROGNAME);
      return -1;
    break;

    case  0 :                     /* The child (read) process */
      dbgmsg("Starting the read process...\n");

      /* Setup all the read/mapped-write blocks for this configuration and sit
         in continuous loop populating the shared memory segment with the data
         in the PLC.  If this function returns, the child process will already
         have been killed so just exit */

      handle_read_requests(conf, &child_conn, &child_spi_conn);

      kill(getpid(), SIGKILL);    /* Kill this process */  
      exit(0);
    break;

    default :                     /* The parent (write) process */
      dbgmsg("Starting the write process...\n");

      /* Handle any write data to PLC/connect to server requests from client
         programs in a continous loop, polling for client messages */

      handle_write_requests(parent_conn, parent_spi_conn);

      kill(chld, SIGTERM);        /* Ensure the child process is terminated */
    break;
  }

  /* Tidy up the server connections */
  if(release_server_connection(parent_conn) == -1)
  {
    err(errout, "%s: error releasing server connection\n", PROGNAME);
    return -1;
  }

  if(release_SPI_server_connection(parent_spi_conn) == -1)
  {
    err(errout, "%s: error releasing SPI server connection\n", PROGNAME);
    return -1;
  }

  return 0;
}



/******************************************************************************
* Function to handle read requests                                            *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection structs are *
*                 passed to the function                                      *
* Post-condition: All read queries for this configuration are setup and run   *
*                 against the PLC.  If an error occurs a -1 is returned       *
******************************************************************************/
int handle_read_requests(plc_cnf *conf, pdsconn *conn, pds_spi_conn *spi_conn)
{
  pdsqueries *queries = NULL;

  /* Setup the queries struct for all read queries in this configuration */
  if((queries = setup_read_queries(conf, conn)) == NULL)
  {
    err(errout, "%s: failed to setup the read queries\n", PROGNAME);
    return -1;
  }

  /* Continuously run the read queries for this configuration */
  execute_read_queries(conn, spi_conn, queries);

  free_read_queries(queries);

  return (!quit_flag) ? -1 : 0;
}



/******************************************************************************
* Function to setup all read/mapped-write block queries in this configuration *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection struct are  *
*                 passed to the function                                      *
* Post-condition: A query is constructed for each read/mapped-write block in  *
*                 this configuration and a pointer to the queries struct is   *
*                 returned.  If an error occurs a null is returned            *
******************************************************************************/
pdsqueries* setup_read_queries(plc_cnf *conf, pdsconn *conn) 
{
  pdsqueries *queries = NULL;
  pdstag *tag = NULL;
  register unsigned short int i = 0, j = 0, b = 0;
 
  if(!(queries = (pdsqueries *) malloc(sizeof(pdsqueries))))
  {
    err(errout, "%s: memory allocation error\n", PROGNAME);
    return NULL;
  }

  /* Array to hold all the read/mapped-write query structures */
  queries->queries = (pdsquery *) calloc(conf->nblocks, sizeof(pdsquery));

  if(!queries->queries)
  {
    err(errout, "%s: memory allocation error\n", PROGNAME);
    return NULL;
  }

  /* Construct the query for each read/mapped-write block */
  for(i = 0, b = 0; b < conf->nblocks; b++)
  {
    queries->queries[i].protocol = conf->blocks[b].protocol;
    queries->queries[i].block_id = b;
    strcpy(queries->queries[i].ip_addr, conf->blocks[b].ip_addr);
    queries->queries[i].port = conf->blocks[b].port;
    strcpy(queries->queries[i].tty_dev, conf->blocks[b].tty_dev);
    strcpy(queries->queries[i].path, conf->blocks[b].path);

    /* Assign a pointer to this query's PLC status word */
    for(j = 0, tag = conn->status; j < conn->nstatus_tags; j++, tag++)
    {
      switch(conf->blocks[b].protocol)
      {
        case MB_TCPIP :
        case MB_SERIAL_TCPIP :
        case DH_SERIAL_TCPIP :
        case CIP_TCPIP :
          if((strcmp(queries->queries[i].ip_addr, tag->ip_addr) == 0) && 
             (queries->queries[i].port == tag->port) &&
             (strcmp(queries->queries[i].path, tag->path) == 0))
          {
            queries->queries[i].status = (unsigned short int *) &tag->status;
          } 
        break;

        case MB_SERIAL :
        case DH_SERIAL :
          if((strcmp(queries->queries[i].tty_dev, tag->tty_dev) == 0) && 
             (strcmp(queries->queries[i].path, tag->path) == 0))
          {
            queries->queries[i].status = (unsigned short int *) &tag->status;
          } 
        break;
      }
    }

    /* Set the initial error count value for this query */
    queries->queries[i].errx = 0;

    /* Set the query and its length */
    switch(conf->blocks[b].protocol)
    {
      case MB_TCPIP :
      case MB_SERIAL_TCPIP :
      case MB_SERIAL :
        queries->queries[i].qlen = mb_setup_read_query(queries->queries[i].query, &conf->blocks[b]);
      break;

      case DH_SERIAL_TCPIP :
      case DH_SERIAL :
        queries->queries[i].qlen = dh_setup_read_query(queries->queries[i].query, &conf->blocks[b]);
      break;

      case CIP_TCPIP :
        queries->queries[i].qlen = cip_setup_read_query(queries->queries[i].query, &conf->blocks[b]);
      break;
    }

    if(queries->queries[i].qlen < 0)
    {
      err(errout, "%s: failed to setup query for block %d\n", PROGNAME, b);
      return NULL;
    } 

    i++;                          /* Increment the query counter */
  }

  queries->nqueries = i;          /* Set the no. of queries */

  return queries;
}



/******************************************************************************
* Function to free memory for all read queries                                *
*                                                                             *
* Pre-condition:  The queries struct is passed to the function                *
* Post-condition: Memory is freed for the queries struct.  If an error occurs *
*                 a -1 is returned                                            *
******************************************************************************/
int free_read_queries(pdsqueries *queries)
{
  int i = -1;

  if(queries)
  {
    if(queries->queries)
    {
      free(queries->queries);
      i = queries->nqueries;
    }  
    free(queries);
  }

  return i;
}
 


/******************************************************************************
* Function to execute all read queries for this configuration                 *
*                                                                             *
* Pre-condition:  The connection structs and the queries struct are passed to *
*                 the function                                                *
* Post-condition: All tags in the data blocks for this configuration are      *
*                 queried from the PLC and their values are placed in memory  *
*                 variables in the shared memory segment.  If an error occurs *
*                 a -1 is returned                                            *
******************************************************************************/
int execute_read_queries(pdsconn *conn, pds_spi_conn *spi_conn,
                         pdsqueries *queries)
{
  register unsigned short int i = 0;
  pdstrans trans, status_trans;
  int *pds_online = NULL, *pds_rdpause_all = NULL;
  int *pds_rdpause_block = NULL, *pds_dbgpause = NULL;

  /* Ensure we have the SPI tags we require */
  if((pds_online = PDS_SPIget_tag_ptr(spi_conn, "PDS_ONLINE")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_ONLINE\n", PROGNAME);
    return -1;
  }

  if((pds_rdpause_all = PDS_SPIget_tag_ptr(spi_conn, "PDS_RDPAUSE_ALL")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_RDPAUSE_ALL\n", PROGNAME);
    return -1;
  }

  if((pds_rdpause_block = PDS_SPIget_tag_ptr(spi_conn, "PDS_RDPAUSE_BLOCK")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_RDPAUSE_BLOCK\n", PROGNAME);
    return -1;
  }

  if((pds_dbgpause = PDS_SPIget_tag_ptr(spi_conn, "PDS_DBGPAUSE")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_DBGPAUSE\n", PROGNAME);
    return -1;
  }

  /* Continuously read data from PLC into shared memory */
  while(!quit_flag)
  {
    /* Check refresh mode.  If 'all', hold semaphore for all blocks */
    if(PDS_GET_RM_REFRESH(runmode) == PDS_RM_REFRESH_ALL)
    {
      if(semset(conn->semid, PDS_SEMHLD, 0) == -1)
        continue;
    }

    /* N.B.: holding the semaphore before checking to see if the PDS has
             been put offline means that any client programs won't be able
             to read the shared memory segment, effectively putting them
             offline as well */

    if(!*pds_online)
    {
      /* Periodically poll the online status */
      err(errout, "%s: PDS has been put offline\n", PROGNAME);

      while(!*pds_online && !quit_flag)
        sleep(PDS_ONLINE_PAUSE);

      err(errout, "%s: PDS has been put online\n", PROGNAME);
    }

    /* Run each query in this configuration in turn */
    for(i = 0; i < queries->nqueries; i++)
    {
      memset(&trans, 0, sizeof(pdstrans));
      memset(&status_trans, 0, sizeof(pdstrans));

      /* Assign this query's properties */
      conn->protocol = queries->queries[i].protocol;
      strcpy(conn->ip_addr, queries->queries[i].ip_addr);
      conn->port = queries->queries[i].port;
      strcpy(conn->tty_dev, queries->queries[i].tty_dev);
      strcpy(conn->path, queries->queries[i].path);

      trans.protocol = queries->queries[i].protocol;
      trans.block_id = queries->queries[i].block_id;
      trans.block_start = PDS_GET_BLOCK_START(trans.block_id);
      memcpy(trans.query, queries->queries[i].query, queries->queries[i].qlen);
      trans.qlen = queries->queries[i].qlen;
      trans.status = queries->queries[i].status; /* N.B.: Already a pointer */
      trans.errx = &queries->queries[i].errx;

      /* Optionally setup a status query */
      if(PDS_GET_RM_STATUS(runmode))
      {
        status_trans.protocol = queries->queries[i].protocol;
        status_trans.block_id = queries->queries[i].block_id;
        status_trans.block_start = PDS_GET_BLOCK_START(status_trans.block_id);
        status_trans.status = queries->queries[i].status;
        status_trans.errx = &queries->queries[i].errx;
      }

      /* Check refresh mode.  If 'block', hold semaphore on per block basis */
      if(PDS_GET_RM_REFRESH(runmode) == PDS_RM_REFRESH_BLOCK)
      {
        if(semset(conn->semid, PDS_SEMHLD, 0) == -1)
          continue;
      }

      /* Check if a status condition exists for this query on this PLC */
      if(*trans.status > 0)
      {
        (*trans.errx)++;

        /* Reset any status bits that have reached their max. error count */
        reset_tags_status(conn, trans.status, trans.errx);

        if(*trans.status > 0)
        {
          /* It's crucial that we release here, otherwise the `continue`
             will allow us to hold the semaphore again, and then we'll
             block ourselves forever */
          if(PDS_GET_RM_REFRESH(runmode) == PDS_RM_REFRESH_BLOCK)
            semset(conn->semid, PDS_SEMREL, 0);

          continue;
        }
      }

      /* Connect the server to the PLC */
      if(connect_to_plc(conn) == -1)
      {
        *trans.status |= PDS_PLC_CONNERR;
        (*trans.errx)++;
        set_tags_status(conn, *trans.status);
      } 
      else
      {
        /* Optionally build a status query */
        if(PDS_GET_RM_STATUS(runmode))
        {
          switch(status_trans.protocol)
          {
            case MB_TCPIP :
            case MB_SERIAL_TCPIP :
            case MB_SERIAL :
              status_trans.qlen = mb_setup_status_query(status_trans.query,
              conn);
            break;

            case DH_SERIAL_TCPIP :
            case DH_SERIAL :
              status_trans.qlen = dh_setup_status_query(status_trans.query,
              conn);
            break;

            case CIP_TCPIP :
              status_trans.qlen = cip_setup_status_query(status_trans.query,
              conn);
            break;
          }
        }

        /* Optionally run a status query */
        if(PDS_GET_RM_STATUS(runmode))
        {
          if(read_from_plc(conn, &status_trans) != -1)
          {
            switch(trans.protocol)
            {
              case MB_TCPIP :
              case MB_SERIAL_TCPIP :
              case MB_SERIAL :
                mb_refresh_status_tags(conn, &status_trans);
              break;

              case DH_SERIAL_TCPIP :
              case DH_SERIAL :
                dh_refresh_status_tags(conn, &status_trans);
              break;

              case CIP_TCPIP :
                cip_refresh_status_tags(conn, &status_trans);
              break;
            }
          }
        }

        /* Only read data from PLC if PLC is ONLINE */
        if((*trans.status & PDS_PLC_OFFLINE) != PDS_PLC_OFFLINE)
        {
          if(read_from_plc(conn, &trans) != -1)
          {
            switch(trans.protocol)
            {
              case MB_TCPIP :
              case MB_SERIAL_TCPIP :
              case MB_SERIAL :
                if(mb_refresh_data_tags(conn, &trans) == -1)
                {
                  *trans.status |= PDS_PLC_RESPERR;
                  (*trans.errx)++;
                  set_tags_status(conn, *trans.status);
                }
              break;

              case DH_SERIAL_TCPIP :
              case DH_SERIAL :
                if(dh_refresh_data_tags(conn, &trans) == -1)
                {
                  *trans.status |= PDS_PLC_RESPERR;
                  (*trans.errx)++;
                  set_tags_status(conn, *trans.status);
                }
              break;

              case CIP_TCPIP :
                if(cip_refresh_data_tags(conn, &trans) == -1)
                {
                  *trans.status |= PDS_PLC_RESPERR;
                  (*trans.errx)++;
                  set_tags_status(conn, *trans.status);
                }
              break;
            }
          }
        } 
      }

      disconnect_from_plc(conn);

      /* Check refresh mode.  If 'block', release semaphore after each block */
      if(PDS_GET_RM_REFRESH(runmode) == PDS_RM_REFRESH_BLOCK)
      {
        semset(conn->semid, PDS_SEMREL, 0);
        usleep(*pds_rdpause_block);
      }

      /* Debug option to pause after each query */
      if(dbglvl == 4) sleep(*pds_dbgpause);
    }

    /* Check refresh mode.  If 'all', release semaphore after all blocks */
    if(PDS_GET_RM_REFRESH(runmode) == PDS_RM_REFRESH_ALL)
    {
      semset(conn->semid, PDS_SEMREL, 0);
      usleep(*pds_rdpause_all);
    }
  }

  return (!quit_flag) ? -1 : 0;
}



/******************************************************************************
* Function to connect to a PLC                                                *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: A connection is established with the PLC.  If an error      *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int connect_to_plc(pdsconn *conn)
{
  /* Connect to this PLC */
  switch(conn->protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case DH_SERIAL_TCPIP :
      if((conn->fd = open_plc_socket(conn->ip_addr, conn->port)) == -1)
      {
        err(errout, "%s: error opening socket to %s:%u:%s\n", PROGNAME, conn->ip_addr, conn->port, conn->path);
        return -1;
      }
    break;

    case CIP_TCPIP :
      if(cip_connect_to_plc(conn) == -1)
        return -1;
    break;

    case MB_SERIAL :
      mb_init_tty_struct(&conn->tio);

      if((conn->fd = open_plc_tty(conn->tty_dev, &conn->tio)) == -1)
      {
        err(errout, "%s: error opening serial port to %s:%s\n", PROGNAME, conn->tty_dev, conn->path);
        return -1;
      }
    break;

    case DH_SERIAL :
      dh_init_tty_struct(&conn->tio);

      if((conn->fd = open_plc_tty(conn->tty_dev, &conn->tio)) == -1)
      {
        err(errout, "%s: error opening serial port to %s:%s\n", PROGNAME, conn->tty_dev, conn->path);
        return -1;
      }
    break;

    default :
      return -1;
    break;
  }

  return 0;
}



/******************************************************************************
* Function to disconnect from a PLC                                           *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: The connection is finished with the PLC.  If an error       *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int disconnect_from_plc(pdsconn *conn)
{
  /* Disconnect from this PLC */
  switch(conn->protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case DH_SERIAL_TCPIP :
      if(close(conn->fd) == -1)
      {
        err(errout, "%s: error closing socket to %s:%u:%s\n", PROGNAME, conn->ip_addr, conn->port, conn->path);
        return -1;
      }
    break;

    case CIP_TCPIP :
      if(cip_disconnect_from_plc(conn) == -1)
        return -1;
    break;

    case MB_SERIAL :
    case DH_SERIAL :
      if(close_plc_tty(conn->fd, &conn->tio) == -1)
      {
        err(errout, "%s: error closing serial port to %s:%s\n", PROGNAME, conn->tty_dev, conn->path);
        return -1;
      }
    break;

    default :
      return -1;
    break;
  }

  return 0;
}



/******************************************************************************
* Function to read data from a PLC                                            *
*                                                                             *
* Pre-condition:  The connection struct & a transaction struct containing the *
*                 query & storage for the response are passed to the function *
* Post-condition: The query is ran against the PLC.  The response is checked  *
*                 for exceptions and returned in the response buffer.  The    *
*                 no. of bytes in the response is returned or a -1 on error   *
******************************************************************************/
int read_from_plc(pdsconn *conn, pdstrans *trans)
{
  short int nbytes = -1;
  int excode = 0;
  char exstr[PDS_EXSTRLEN] = "\0", fqid[PDS_PLC_FQID_LEN] = "\0";

  /* Get this PLC's fully-qualified ID */
  PDS_GET_PLC_FQID(fqid, conn);

  /* Run the query against the PLC */
  switch(trans->protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      mb_instantiate_prepared_query(trans);

      printd("--> Query %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      nbytes = mb_run_plc_query(conn->fd, trans);

      printd("<-- Response %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      mb_clean_plc_response(trans);
    break;

    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      dh_instantiate_prepared_query(trans);

      printd("--> Query %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      nbytes = dh_run_plc_query(conn->fd, trans);

      printd("<-- Response %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      dh_clean_plc_response(trans);
    break;

    case CIP_TCPIP :
      cip_instantiate_prepared_query(conn->fd, trans);

      printd("--> Query %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      nbytes = cip_run_plc_query(conn->fd, trans);

      printd("<-- Response %d, Trans. %d\n", trans->block_id, trans->trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans->buf, trans->blen);

      cip_clean_plc_response(conn->fd, trans);
    break;
  }

  if(nbytes == -1)
  {
    if(!quit_flag)
    {
      err(errout, "%s: error running read query to %s errx %d\n", PROGNAME, fqid, *trans->errx);

      *trans->status |= PDS_PLC_COMMSERR;
      (*trans->errx)++;
      set_tags_status(conn, *trans->status);
    }
  }
  else
  {
    /* Validate the PLC's response to the query */
    switch(trans->protocol)
    {
      case MB_TCPIP :
      case MB_SERIAL_TCPIP :
      case MB_SERIAL :
        if((excode = mb_check_plc_response(trans)) != 0)
          strcpy(exstr, (char *) mb_print_plc_exception(excode));
      break;

      case DH_SERIAL_TCPIP :
      case DH_SERIAL :
        if((excode = dh_check_plc_response(trans)) != 0)
          strcpy(exstr, (char *) dh_print_plc_exception(excode));
      break;

      case CIP_TCPIP :
        if((excode = cip_check_plc_response(trans)) != 0)
          strcpy(exstr, (char *) cip_print_plc_exception(excode));
      break;
    }

    if(excode != 0)
    {
      if(!quit_flag)
      {
        err(errout, "%s: PLC response error - %s - on %s\n", PROGNAME, exstr, fqid);

        *trans->status |= PDS_PLC_RESPERR;
        (*trans->errx)++;
        nbytes = -1;
        set_tags_status(conn, *trans->status);
      }
    }
  } 

  return nbytes;
}



/******************************************************************************
* Function to handle - write data to PLC/client initialisation - requests     *
*                                                                             *
* Pre-condition:  The connection structs are passed to the function           *
* Post-condition: Client requests to write data to the PLC and a client's     *
*                 initial request to connect to the server are handled.  If   *
*                 an error occurs a -1 is returned                            *
******************************************************************************/
int handle_write_requests(pdsconn *conn, pds_spi_conn *spi_conn)
{
  pdsmsg msg;
  int nbytes = 0;
  long int msgtype = -PDS_INITMSG;
  int *pds_online = NULL, *pds_wrpause = NULL;

  /* N.B.: Setting the msgtype to 'minus init message' means that all
           messages with init's priority or higher will be read from the 
           queue and ordered according to priority instead of position in 
           the queue (e.g., a write message will be read from the queue 
           before an init message would).  This is what we want */

  /* Ensure we have the SPI tags we require */
  if((pds_online = PDS_SPIget_tag_ptr(spi_conn, "PDS_ONLINE")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_ONLINE\n", PROGNAME);
    return -1;
  }

  if((pds_wrpause = PDS_SPIget_tag_ptr(spi_conn, "PDS_WRPAUSE")) == (int *) -1)
  {
    err(errout, "%s: failed to get SPI tag PDS_WRPAUSE\n", PROGNAME);
    return -1;
  }

  /* Continuously poll the message queue for client requests */
  while(!quit_flag)
  {
    if(!*pds_online)
    {
      /* Periodically poll the online status */
      err(errout, "%s: PDS has been put offline\n", PROGNAME);

      while(!*pds_online && !quit_flag)
        sleep(PDS_ONLINE_PAUSE);

      err(errout, "%s: PDS has been put online\n", PROGNAME);
    }

    nbytes = 0;
    memset(&msg, 0, sizeof(pdsmsg));

    /* Get the next message in the queue */
    if((nbytes = msgrcv(conn->msgid, (void *) &msg, conn->msgsize, msgtype, 0)) < conn->msgsize)
    {
      err(errout, "%s: error reading from message queue\n", PROGNAME);
      continue;
    }

    switch(msg.msgtype)
    {
      case PDS_WRMSG :            /* Client request to write data to PLC */
        write_to_plc(conn, &msg);

        /* Set the 'write data to PLC response' message type */
        msg.msgtype = PDS_WRMSG_RESP;

        /* Send the 'write data to PLC' response */
        if((nbytes = msgsnd(conn->msgid, (void *) &msg, conn->msgsize, 0)) == -1)
        {
          err(errout, "%s: error writing data response to message queue\n",
          PROGNAME);
          break;
        }
      break;

      case PDS_INITMSG :          /* Client request to connect to server */
        /* Send client the necessary connection data */
        msg.semid = conn->semid;
        msg.shmid = conn->shmid;
        msg.ndata_tags = conn->ndata_tags;
        msg.nstatus_tags = conn->nstatus_tags;
        msg.febe_proto_ver = conn->febe_proto_ver; 

        /* Set the 'request for init. response' message type */
        msg.msgtype = PDS_INITMSG_RESP;

        /* Send the 'request for init.' response */
        if((nbytes = msgsnd(conn->msgid, (void *) &msg, conn->msgsize, 0)) == -1)
        {
          err(errout, "%s: error writing init response to message queue\n",
          PROGNAME);
          break;
        }
      break;
    }

    usleep(*pds_wrpause);
  }

  return (!quit_flag) ? -1 : 0;
}



/******************************************************************************
* Function to write client data to the PLC                                    *
*                                                                             *
* Pre-condition:  The connection struct and the message struct containing     *
*                 data to write to the PLC are passed to the function         *
* Post-condition: The PLC query is constructed with data in the message and   *
*                 then written to the PLC.  If an error occurs a -1 is        *
*                 returned                                                    *
******************************************************************************/
int write_to_plc(pdsconn *conn, pdsmsg *msg)
{
  register unsigned short int i = 0;
  short int len = strlen(msg->tag.name), found = 0, nbytes = 0;
  static unsigned short int errx = 0;
  int excode = 0;
  char exstr[PDS_EXSTRLEN] = "\0", fqid[PDS_PLC_FQID_LEN] = "\0";
  pdstag *tag = NULL;
  pdstrans trans;

  msg->tag.status = 0;
  memset(&trans, 0, sizeof(pdstrans));

  /* Before reading data from shared mem., hold the semaphore */
  while((semset(conn->semid, PDS_SEMHLD, 0) == -1) && (!quit_flag))
    continue;

  /* Search for tagname ensuring string is not just a substring of tag */
  for(i = 0, tag = (pdstag *) conn->data; i < conn->ndata_tags; i++, tag++)
  {
    if(memcmp(tag->name, msg->tag.name, len) == 0)
    {
      if(len == strlen(tag->name)) 
      {
        conn->protocol = tag->protocol;
        strcpy(conn->ip_addr, tag->ip_addr);
        conn->port = tag->port;
        strcpy(conn->tty_dev, tag->tty_dev);
        strcpy(conn->path, tag->path);
        msg->tag.status = tag->status; /* This allows us to return status */
        trans.protocol = tag->protocol;
        trans.block_id = tag->block_id;
        trans.block_start = PDS_GET_BLOCK_START(trans.block_id);
        trans.status = &msg->tag.status;
        trans.errx = &errx;
        found = 1;
        break;
      }
    }
  }

  /* After reading data from shared mem., release the semaphore */
  semset(conn->semid, PDS_SEMREL, 0);

  if(!found)
  {
    err(errout, "%s: error finding tag '%s' to write to\n", PROGNAME, msg->tag.name);
    return -1;
  }

  /* Get this PLC's fully-qualified ID */
  PDS_GET_PLC_FQID(fqid, conn);
 
  /* Don't write data to PLC if PLC is OFFLINE */
  if(msg->tag.status & PDS_PLC_OFFLINE)
  {
    /* N.B.:  No need to write an error log message if PLC is offline.  This
              will be handled by the continuous read queries.  This avoids
              'flooding' the PDS log */
    return -1;
  }

  /* Build the query */
  switch(trans.protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      if((trans.qlen = mb_setup_write_query(trans.query, tag, msg)) == -1)
        return -1;
    break;

    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      if((trans.qlen = dh_setup_write_query(trans.query, tag, msg)) == -1)
        return -1;
    break;

    case CIP_TCPIP :
      if((trans.qlen = cip_setup_write_query(trans.query, tag, msg)) == -1)
        return -1;
    break;
  }

  /* Connect the server to the PLC */
  if(connect_to_plc(conn) == -1)
  {
    msg->tag.status |= PDS_PLC_CONNERR;
    set_tags_status(conn, msg->tag.status);
    return -1;
  } 

  /* Run the query against the PLC */
  switch(trans.protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      mb_instantiate_prepared_query(&trans);

      printd("--> Query %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      nbytes = mb_run_plc_query(conn->fd, &trans);

      printd("<-- Response %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      mb_clean_plc_response(&trans);
    break;

    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      dh_instantiate_prepared_query(&trans);

      printd("--> Query %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      nbytes = dh_run_plc_query(conn->fd, &trans);

      printd("<-- Response %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      dh_clean_plc_response(&trans);
    break;

    case CIP_TCPIP :
      cip_instantiate_prepared_query(conn->fd, &trans);

      printd("--> Query %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      nbytes = cip_run_plc_query(conn->fd, &trans);

      printd("<-- Response %d, Trans. %d\n", trans.block_id, trans.trans_id);
      if(dbgflag) PDS_PRINT_BARRAYX(trans.buf, trans.blen);

      cip_clean_plc_response(conn->fd, &trans);
    break;
  }

  disconnect_from_plc(conn);

  if(nbytes == -1)
  {
    if(!quit_flag)
    {
      err(errout, "%s: error running write query to %s errx %d\n", PROGNAME, fqid, ++errx);

      msg->tag.status |= PDS_PLC_COMMSERR;
      set_tags_status(conn, msg->tag.status);
      return -1;
    }
  }

  /* Validate the PLC's response to the query */
  switch(trans.protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
      if((excode = (int) mb_check_plc_response(&trans)) != 0)
        strcpy(exstr, (char *) mb_print_plc_exception((int) excode));
    break;

    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
      if((excode = (int) dh_check_plc_response(&trans)) != 0)
        strcpy(exstr, (char *) dh_print_plc_exception((int) excode));
    break;

    case CIP_TCPIP :
      if((excode = (int) cip_check_plc_response(&trans)) != 0)
        strcpy(exstr, (char *) cip_print_plc_exception((int) excode));
    break;
  }

  if(excode != 0)
  {
    if(!quit_flag)
    {
      err(errout, "%s: write PLC response error - %s - on %s\n", PROGNAME, exstr, fqid);

      msg->tag.status |= PDS_PLC_RESPERR;
      set_tags_status(conn, msg->tag.status);
      return -1;
    }
  }

  return 0;
}



/******************************************************************************
* Function to set tag's status words                                          *
*                                                                             *
* Pre-condition:  The connection struct and the PLC's status value are passed *
*                 to the function                                             *
* Post-condition: All tags on this PLC have their status words set to the     *
*                 PLC's status value.  A count of updated tags is returned    *
******************************************************************************/
int set_tags_status(pdsconn *conn, unsigned short int status)
{
  register int i = 0, updated = 0;
  pdstag *tag = NULL;

  /* Cycle through ALL tags and match each tag's PLC.  The reason we set the
     status tag with it's status is because a write query doesn't carry a 
     pointer to this status tag so never sets it and because it's never set
     it doesn't get reset after the specified timeout (by the read process) */

  for(i = 0, tag = (pdstag *) conn->data; i < conn->ttags; i++, tag++)
  {
    switch(conn->protocol)
    {
      case MB_TCPIP :
      case MB_SERIAL_TCPIP :
      case DH_SERIAL_TCPIP :
      case CIP_TCPIP :
        if((strcmp(conn->ip_addr, tag->ip_addr) == 0) &&
           (conn->port == tag->port) && (strcmp(conn->path, tag->path) == 0))
        {
          tag->status = status;
          updated++;
        }
      break;

      case MB_SERIAL :
      case DH_SERIAL :
        if((strcmp(conn->tty_dev, tag->tty_dev) == 0) &&
           (strcmp(conn->path, tag->path) == 0))
        {
          tag->status = status;
          updated++;
        }
      break;
    }
  }

  return updated;
}



/******************************************************************************
* Function to reset bits in tags' status words if max. count is reached       *
*                                                                             *
* Pre-condition:  The connection struct, and pointers to the PLC's status     *
*                 value and error count are passed to the function            *
* Post-condition: If a change of status occurs, all tags on this PLC have     *
*                 their status words reset to reflect the change. If the      *
*                 outcome of this is that no status condition exists on the   *
*                 PLC, then the error count is reset to zero.  A count of     *
*                 updated tags is returned                                    *
******************************************************************************/
int reset_tags_status(pdsconn *conn, unsigned short int *status,
                      unsigned short int *errx)
{
  int updated = 0;
  unsigned short int init_status = *status; /* Get the initial status */
  char fqid[PDS_PLC_FQID_LEN] = "\0";

  /* Get this PLC's fully-qualified ID */
  PDS_GET_PLC_FQID(fqid, conn);

  /* If a status condition has reached it's max. count, first log the status,
     then reset it's bit in the status word */

  if(PDS_CONNERR_LIMIT(*status, *errx))
  {
    err(errout, "%s: error on %s - %s (%d) errx %d\n", PROGNAME, fqid, PDS_PRINT_PLC_STATUS(*status), *status, *errx);

    *status &= PDS_PLC_CONNERR_RST;
  }

  if(PDS_COMMSERR_LIMIT(*status, *errx))
  {
    err(errout, "%s: error on %s - %s (%d) errx %d\n", PROGNAME, fqid, PDS_PRINT_PLC_STATUS(*status), *status, *errx);

    *status &= PDS_PLC_COMMSERR_RST;
  }

  if(PDS_RESPERR_LIMIT(*status, *errx))
  {
    err(errout, "%s: error on %s - %s (%d) errx %d\n", PROGNAME, fqid, PDS_PRINT_PLC_STATUS(*status), *status, *errx);

    *status &= PDS_PLC_RESPERR_RST;
  }

  if(PDS_OFFLINE_LIMIT(*status, *errx))
  {
    err(errout, "%s: error on %s - %s (%d) errx %d\n", PROGNAME, fqid, PDS_PRINT_PLC_STATUS(*status), *status, *errx);

    *status &= PDS_PLC_OFFLINE_RST;
  }

  /* Update the status word of this PLC's tags */
  if(*status != init_status)
    updated = set_tags_status(conn, *status);

  if(*status == 0)
    *errx = 0;

  return updated;
}

