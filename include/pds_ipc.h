/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_ipc.h                                                         *
* PURPOSE:  Header file for the PLC data server Interprocess Communications   *
*           module                                                            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-22                                                        *
******************************************************************************/

#ifndef __PDS_IPC_H
#define __PDS_IPC_H

#ifndef _SVID_SOURCE              /* Define for SysV Interface Definition */
#define _SVID_SOURCE
#endif

#include <sys/ipc.h>
#include <sys/sem.h> 
#include <sys/shm.h>
#include <sys/msg.h>

#include "pds_defs.h"

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PDS_IPCKEY		1423779
#define PDS_IPCFLAGS		0666

#define PDS_SEMKEY		PDS_IPCKEY
#define PDS_SEMFLAGS		PDS_IPCFLAGS

#define PDS_SHMKEY		PDS_IPCKEY
#define PDS_SHMFLAGS		PDS_IPCFLAGS

#define PDS_MSGKEY		PDS_IPCKEY
#define PDS_MSGFLAGS		PDS_IPCFLAGS

#define PDS_SEMHLD		-1     /* Hold the semaphore */
#define PDS_SEMREL		1      /* Release the semaphore */

#define PDS_WRMSG		100    /* Message priority (1 = high) */
#define PDS_WRMSG_RESP		110
#define PDS_INITMSG		200
#define PDS_INITMSG_RESP	210
#define PDS_RDMSG		300
#define PDS_RDMSG_RESP		310

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The PLC data server IPC message queue structure                             *
******************************************************************************/
typedef struct pdsmsg_rec
{
  long int msgtype;               /* The message type */

  /*********************** For writing data to the PLC ***********************/

  pdstag tag;                     /* The PLC data server tag structure */
                                  /* Array of tag values */
  unsigned short int tagvalues[PDS_NTAGVALUES];
  short int ntags;                /* No. of tags in the query */

  /*********************** For connecting to the server **********************/
  
  int semid;                      /* The semaphore ID */
  int shmid;                      /* The shared memory segment ID */

  int ndata_tags;                 /* No. of data tags in sh mem segment */
  int nstatus_tags;               /* No. of status tags in sh mem segment */

  int febe_proto_ver;             /* Front-end/Back-end protocol version */

} pdsmsg;

#endif

