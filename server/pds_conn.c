/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_conn.c                                                        *
* PURPOSE:  The server connection management functions module                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-12-21                                                        *
******************************************************************************/

#include "pds_srv.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */

/******************************************************************************
* Function to initialise the server connection                                *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection struct are  *
*                 passed to the function                                      *
* Post-condition: The server connection is initialised and the configuration  *
*                 file tags are mapped to variables in shared memory.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int init_server_connection(plc_cnf *conf, pdsconn *conn)
{
  conn->nblocks = conf->nblocks;            /* No. of blocks in config */
  conn->nplcs = conf->nplcs;                /* No. of PLCs in config */
  conn->ndata_tags = conf->ndata_tags;      /* No. of data tags in config */
  conn->nstatus_tags = conf->nstatus_tags;  /* No. of status tags in config */
  conn->ttags = conf->ttags;                /* Total no. of tags in config */ 

  if(init_semaphores(conn) == -1) /* Setup semaphore for connection */ 
  {
    err(errout, "%s: error initialising semaphore\n", PROGNAME);
    return -1;
  }

  if(init_shared_mem(conn) == -1) /* Setup shared memory for connection */ 
  {
    err(errout, "%s: error initialising shared memory\n", PROGNAME);
    return -1;
  }

  if(init_msg_queue(conn) == -1)  /* Setup message queue for connection */ 
  {
    err(errout, "%s: error initialising message queue\n", PROGNAME);
    return -1;
  }

  /* Map the configuration file tags to memory variables */
  if(map_shm(conf, conn) == -1)
  {
    err(errout, "%s: error mapping shared memory\n", PROGNAME);
    return -1;
  }

  /* Set the server's Front-end/Back-end protocol version.  This ensures that
     any client that connects to the server must have been compiled against
     the same libraries/headers etc. to be able to talk to this server */
  conn->febe_proto_ver = PDS_FEBE_PROTO_VER;

  return 0;
}



/******************************************************************************
* Function to release the server connection                                   *
*                                                                             *
* Pre-condition:  A valid connection struct is passed to the function         *
* Post-condition: The server connection's resources are released.  If an      *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int release_server_connection(pdsconn *conn)
{
  union semun sem_union;

  memset(&sem_union, 0, sizeof(sem_union));

  /* Delete the semaphore */
  if(semctl(conn->semid, IPC_RMID, 0, sem_union) == -1)
  {
    err(errout, "%s: error deleting semaphore\n", PROGNAME);
    return -1;
  }

  /* Detach and free the shared memory */
  if(release_shm(conn->shm, conn->shmid) == -1)
  {
    err(errout, "%s: error releasing shared memory\n", PROGNAME);
    return -1;
  }

  /* Delete the message queue */
  if(msgctl(conn->msgid, IPC_RMID, (struct msqid_ds *) 0) == -1)
  {
    err(errout, "%s: error deleting message queue\n", PROGNAME);
    return -1;
  }

  return 0;
}



/******************************************************************************
* Function to setup the semaphore(s)                                          *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The semaphores are setup and the semaphore members of the   *
*                 connection struct are initialised accordingly.  If an error *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int init_semaphores(pdsconn *conn)
{
  union semun sem_union;

  sem_union.val = 1;              /* Value to initalise the semaphore */

  conn->nsems = 1;
  conn->semflags = PDS_SEMFLAGS | IPC_CREAT | IPC_EXCL;

  /* Create a semaphore */
  if((conn->semid = semget(conn->semkey, conn->nsems, conn->semflags)) == -1)
  {
    err(errout, "%s: error creating semaphore\n", PROGNAME);
    return -1;
  }

  /* Initialise a semaphore */
  if(semctl(conn->semid, 0, SETVAL, sem_union) == -1)
  {
    err(errout, "%s: error setting initial value of semaphore\n", PROGNAME);
    return -1;
  }

  printd("Semaphore initialised using ID %d\n", conn->semid);

  return 0;
}



/******************************************************************************
* Function to setup the shared memory segment                                 *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The shared memory segment is setup and the shared memory    *
*                 members of the connection struct are initialised            *
*                 accordingly.  If an error occurs a -1 is returned           *
******************************************************************************/
int init_shared_mem(pdsconn *conn)
{
  conn->shmsize = conn->ttags * sizeof(pdstag); 
  conn->shmflags = PDS_SHMFLAGS | IPC_CREAT | IPC_EXCL;

  /* Create and attach a shared memory segment */
  if(!(conn->shm = (void *) setup_shm(conn->shmkey, conn->shmsize, conn->shmflags, &conn->shmid)))
  {
    err(errout, "%s: error setting up shared memory\n", PROGNAME);
    return -1;
  }

  /* Assign tag pointers to the start of the data & status tags */
  conn->data = (pdstag *) conn->shm;
  conn->status = (pdstag *) (conn->shm + (conn->ndata_tags * sizeof(pdstag)));

  printd("Shared memory attached at %p, using ID %d\n", (int) conn->shm, conn->shmid);
  printd("No. of tags in segment: %d\n", (conn->shmsize > 0 ? (conn->shmsize / sizeof(pdstag)) : 0));

  return 0;
}



/******************************************************************************
* Function to setup the message queue                                         *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The message queue is setup and the message queue members    *
*                 of the connection struct are initialised accordingly.  If   *
*                 an error occurs a -1 is returned                            *
******************************************************************************/
int init_msg_queue(pdsconn *conn)
{
  conn->msgsize = (sizeof(pdsmsg) - sizeof(long int));
  conn->msgflags = PDS_MSGFLAGS | IPC_CREAT | IPC_EXCL; 

  /* Setup a message queue */
  if((conn->msgid = msgget(conn->msgkey, conn->msgflags)) == -1)
  {
    err(errout, "%s: error creating message queue\n", PROGNAME);
    return -1;
  }

  printd("Message queue initialised using ID %d\n", conn->msgid);

  return 0;
}



/******************************************************************************
* Function to initialise the SPI server connection                            *
*                                                                             *
* Pre-condition:  The SPI tag list and the SPI connection struct are passed   *
*                 to the function                                             *
* Post-condition: The SPI server connection is initialised and the SPI tags   *
*                 are mapped to variables in shared memory.  If an error      *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int init_SPI_server_connection(pds_spi_tag_list *tag_list, pds_spi_conn *conn)
{
  conn->ndata_tags = tag_list->ntags;       /* No. of SPI tags in struct */

  /* Setup shared memory for connection */
  if(init_SPI_shared_mem(conn) == -1)
  {
    err(errout, "%s: error initialising SPI shared memory\n", PROGNAME);
    return -1;
  }

  /* Map the SPI tags to memory variables */
  if(map_SPI_shm(tag_list, conn) == -1)
  {
    err(errout, "%s: error mapping SPI shared memory\n", PROGNAME);
    return -1;
  }

  /* Set the server's Front-end/Back-end protocol version.  This ensures that
     any client that connects to the server must have been compiled against
     the same libraries/headers etc. to be able to talk to this server */
  conn->febe_proto_ver = PDS_FEBE_PROTO_VER;

  return 0;
}



/******************************************************************************
* Function to release the SPI server connection                               *
*                                                                             *
* Pre-condition:  A valid SPI connection struct is passed to the function     *
* Post-condition: The SPI server connection's resources are released.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int release_SPI_server_connection(pds_spi_conn *conn)
{
  /* Detach and free the shared memory */
  if(release_shm(conn->shm, conn->shmid) == -1)
  {
    err(errout, "%s: error releasing SPI shared memory\n", PROGNAME);
    return -1;
  }

  return 0;
}



/******************************************************************************
* Function to setup the SPI shared memory segment                             *
*                                                                             *
* Pre-condition:  The partially initialised SPI connection struct is passed   *
*                 to the function                                             *
* Post-condition: The shared memory segment is setup and the shared memory    *
*                 members of the connection struct are initialised            *
*                 accordingly.  If an error occurs a -1 is returned           *
******************************************************************************/
int init_SPI_shared_mem(pds_spi_conn *conn)
{
  conn->shmsize = conn->ndata_tags * sizeof(pds_spi_tag);
  conn->shmflags = PDS_SHMFLAGS | IPC_CREAT | IPC_EXCL;

  /* Create and attach a shared memory segment */
  if(!(conn->shm = (void *) setup_shm(conn->shmkey, conn->shmsize, conn->shmflags, &conn->shmid)))
  {
    err(errout, "%s: error setting up SPI shared memory\n", PROGNAME);
    return -1;
  }

  /* Assign a tag pointer to the start of the data tags */
  conn->data = (pds_spi_tag *) conn->shm;

  printd("SPI shared memory attached at %p, using ID %d\n", (int) conn->shm, conn->shmid);
  printd("No. of tags in segment: %d\n", (conn->shmsize > 0 ? (conn->shmsize / sizeof(pds_spi_tag)) : 0));

  return 0;
}

