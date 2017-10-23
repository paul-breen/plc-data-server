/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_defs.h                                                        *
* PURPOSE:  Header file for the PLC data server definitions module            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-22                                                        *
******************************************************************************/

#ifndef __PDS_DEFS_H
#define __PDS_DEFS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <termios.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PDS_FEBE_PROTO_VER	6

#define PDS_TAGNAME_LEN		64
#define PDS_TAGVALUE_LEN	16
#define PDS_STR_TAGVALUE_LEN	256
#define PDS_NTAGVALUES		100
#define PDS_IP_ADDR_LEN		24
#define PDS_TTY_DEV_LEN		64
#define PDS_PLC_ADDR_LEN	64
#define PDS_PLC_REF_LEN		64
#define PDS_PLC_PATH_LEN	16
#define PDS_PLC_FQID_LEN	(PDS_TTY_DEV_LEN + PDS_PLC_PATH_LEN + 24)
#define PDS_TAG_FQID_LEN	(PDS_PLC_FQID_LEN + PDS_TAGNAME_LEN)

#define PDS_MAXBUFLEN		504
#define PDS_WORDSIZE		2
#define PDS_BITS_BYTE		8
#define PDS_BITS_WORD		(PDS_BITS_BYTE * PDS_WORDSIZE)
#define PDS_EXSTRLEN		96

#define PDS_PLC_PREFIX		"PLC"
#define PDS_PLC_PATH_SEP	'.'

#define PDS_CONN_OK		0x00   /* PDS connection status OK */
#define PDS_CONN_CONNERR	0x01   /* PDS general connection error */
#define PDS_CONN_MSGCONN_ERR	0x02   /* PDS msg queue connection error */
#define PDS_CONN_MSGSEND_ERR	0x04   /* PDS send request for init. error */
#define PDS_CONN_MSGRECV_ERR	0x08   /* PDS recv request for init. error */
#define PDS_CONN_PROTO_ERR	0x10   /* PDS fe/be protocol error */
#define PDS_CONN_SHMCONN_ERR	0x20   /* PDS shared mem connection error */
 
#define PDS_PLC_OK		0x00   /* PLC status OK */
#define PDS_PLC_CONNERR		0x01   /* PLC connection error */
#define PDS_PLC_COMMSERR	0x02   /* PLC comms error */
#define PDS_PLC_RESPERR		0x04   /* PLC response error */
#define PDS_PLC_OFFLINE		0x08   /* PLC is offline */

#define PDS_PLC_CONNERR_RST	~0x01  /* PLC connection error reset */
#define PDS_PLC_COMMSERR_RST	~0x02  /* PLC comms error reset */
#define PDS_PLC_RESPERR_RST	~0x04  /* PLC response error reset */
#define PDS_PLC_OFFLINE_RST	~0x08  /* PLC is offline reset */

#define PDS_CHECK_PROTO_VER(c)		((c)->febe_proto_ver)
#define PDScheck_proto_ver(c)		PDS_CHECK_PROTO_VER(c)

#define PDS_CHECK_CONN_STATUS(c)	((c)->conn_status)
#define PDScheck_conn_status(c)		PDS_CHECK_CONN_STATUS(c)

#define PDS_PRINT_CONN_STATUS(s)\
(((s) == PDS_CONN_OK) ? "PDS Connection Status OK" :\
 ((s) & PDS_CONN_MSGCONN_ERR) ? "PDS Message Queue Connection Error" :\
 ((s) & PDS_CONN_MSGSEND_ERR) ? "PDS Send Request for Initialisation Error" :\
 ((s) & PDS_CONN_MSGRECV_ERR) ? "PDS Recv Request for Initialisation Error" :\
 ((s) & PDS_CONN_PROTO_ERR) ? "PDS FE/BE Protocol Error" :\
 ((s) & PDS_CONN_SHMCONN_ERR) ? "PDS Shared Memory Connection Error" :\
 ((s) & PDS_CONN_CONNERR) ? "PDS Connection Error" :\
 "PDS Connection Status Unknown!")
#define PDSprint_conn_status(c)		PDS_PRINT_CONN_STATUS((c)->conn_status)

#define PDS_CHECK_PLC_STATUS(c)		((c)->plc_status)
#define PDScheck_plc_status(c)		PDS_CHECK_PLC_STATUS(c)

#define PDS_PRINT_PLC_STATUS(s)\
(((s) == PDS_PLC_OK) ? "PLC Status OK" :\
 ((s) & PDS_PLC_CONNERR) ? "PLC Connection Error" :\
 ((s) & PDS_PLC_COMMSERR) ? "PLC Comms Error" :\
 ((s) & PDS_PLC_RESPERR) ? "PLC Response Error" :\
 ((s) & PDS_PLC_OFFLINE) ? "PLC is Offline" :\
 "PLC Status Unknown!")
#define PDSprint_plc_status(c)		PDS_PRINT_PLC_STATUS((c)->plc_status)

/* Accessor macros for the pdsconn structure */
#define PDSconn_get_conn_status(c)	((c) ? (c)->conn_status : -1)
#define PDSconn_get_protocol(c)		((c) ? (c)->protocol : -1)
#define PDSconn_get_ip_addr(c)		((c) ? (c)->ip_addr : NULL)
#define PDSconn_get_port(c)		((c) ? (c)->port : -1)
#define PDSconn_get_tty_dev(c)		((c) ? (c)->tty_dev : NULL)
#define PDSconn_get_tio(c)		((c) ? &(c)->tio : NULL)
#define PDSconn_get_path(c)		((c) ? (c)->path : NULL)
#define PDSconn_get_fd(c)		((c) ? (c)->fd : -1)
#define PDSconn_get_plc_status(c)	((c) ? (c)->plc_status : -1)
#define PDSconn_get_data(c)		((c) ? (c)->data : NULL)
#define PDSconn_get_status(c)		((c) ? (c)->status : NULL)
#define PDSconn_get_nblocks(c)		((c) ? (c)->nblocks : -1)
#define PDSconn_get_nplcs(c)		((c) ? (c)->nplcs : -1)
#define PDSconn_get_ndata_tags(c)	((c) ? (c)->ndata_tags : -1)
#define PDSconn_get_nstatus_tags(c)	((c) ? (c)->nstatus_tags : -1)
#define PDSconn_get_ttags(c)		((c) ? (c)->ttags : -1)

/* Accessor macros for the pdstag structure */
#define PDStag_get_id(t)		((t) ? (t)->id : -1)
#define PDStag_get_protocol(t)		((t) ? (t)->protocol : -1)
#define PDStag_get_function(t)		((t) ? (t)->function : -1)
#define PDStag_get_block_id(t)		((t) ? (t)->block_id : -1)
#define PDStag_get_base_addr(t)		((t) ? (t)->base_addr : -1)
#define PDStag_get_ascii_addr(t)	((t) ? (t)->ascii_addr : NULL)
#define PDStag_get_ip_addr(t)		((t) ? (t)->ip_addr : NULL)
#define PDStag_get_port(t)		((t) ? (t)->port : -1)
#define PDStag_get_tty_dev(t)		((t) ? (t)->tty_dev : NULL)
#define PDStag_get_path(t)		((t) ? (t)->path : NULL)
#define PDStag_get_ref(t)		((t) ? (t)->ref : -1)
#define PDStag_get_ascii_ref(t)		((t) ? (t)->ascii_ref : NULL)
#define PDStag_get_name(t)		((t) ? (t)->name : NULL)
#define PDStag_get_value(t)		((t) ? (t)->value : -1)
#define PDStag_get_type(t)		((t) ? (t)->type : -1)
#define PDStag_get_status(t)		((t) ? (t)->status : -1)
#define PDStag_get_mtime(t)		((t) ? (t)->mtime : -1)

/* Construct a PLC's fully-qualified ID (dependent on comms protocol) */
#define PDS_GET_PLC_FQID(s, p)\
((PDS_GET_PROTOTYPE(p->protocol) == PDS_SERIAL_PROTO) ?\
 (sprintf((s), "%s:%s", (p)->tty_dev, (p)->path)) :\
 (sprintf((s), "%s:%u:%s", (p)->ip_addr, (p)->port, (p)->path)))

/* Construct a tag's fully-qualified ID (dependent on comms protocol) */
#define PDS_GET_TAG_FQID(s, p)\
((PDS_GET_PROTOTYPE(p->protocol) == PDS_SERIAL_PROTO) ?\
 (sprintf((s), "%s:%s %s", (p)->tty_dev, (p)->path, (p)->name)) :\
 (sprintf((s), "%s:%u:%s %s", (p)->ip_addr, (p)->port, (p)->path, (p)->name)))

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The PLC data server tag structure                                           *
******************************************************************************/
typedef struct pdstag_rec
{
  unsigned int id;                     /* The unique ID for this tag struct */
  unsigned short int protocol;         /* The comms protocol */
  unsigned short int function;         /* The function code */
  unsigned short int block_id;         /* The block's ID */
  unsigned int base_addr;              /* The block's base address */
  char ascii_addr[PDS_PLC_ADDR_LEN];   /* The block's logical address */
  char ip_addr[PDS_IP_ADDR_LEN];       /* The PLC IP address */
  unsigned short int port;             /* The PLC TCP port */
  char tty_dev[PDS_TTY_DEV_LEN];       /* The PLC TTY device */
  char path[PDS_PLC_PATH_LEN];         /* The PLC routing path */
  unsigned int ref;                    /* The tag reference */
  char ascii_ref[PDS_PLC_REF_LEN];     /* The tag logical reference */
  char name[PDS_TAGNAME_LEN];          /* The tag name */
  unsigned short int value;            /* The tag value */
  unsigned short int type;             /* The tag's data type */
  unsigned short int status;           /* The tag's PLC status */
  time_t mtime;                        /* The tag's last modification time */
} pdstag;

/******************************************************************************
* The PLC data server connection structure                                    *
******************************************************************************/
typedef struct pdsconn_rec
{
  key_t semkey;                   /* The semaphore key */
  int nsems;                      /* Number of semaphores */
  int semflags;                   /* Create flags */
  int semid;                      /* Semaphore ID */

  key_t shmkey;                   /* The shared memory key */
  void *shm;                      /* Pointer to start of shared mem segment */
  int shmsize;                    /* Size of segment */
  int shmflags;                   /* Create flags */
  int shmid;                      /* Shared memory segment ID */

  key_t msgkey;                   /* The message queue key */
  int msgsize;                    /* The size of the message data */
  int msgflags;                   /* Create flags */
  int msgid;                      /* The message queue ID */

  unsigned short int conn_status; /* The connection status */

  unsigned short int protocol;    /* The comms protocol */
  char ip_addr[PDS_IP_ADDR_LEN];  /* The IP address */
  unsigned short int port;        /* The port */
  char tty_dev[PDS_TTY_DEV_LEN];  /* The TTY device */
  struct termios tio;             /* The TTY attributes struct */
  char path[PDS_PLC_PATH_LEN];    /* The PLC routing path */
  int fd;                         /* The {socket|tty} fd */

  unsigned short int plc_status;  /* PLC status */

  pdstag *data;                   /* Pointer to start of data tags */
  pdstag *status;                 /* Pointer to start of status tags */

  int nblocks;                    /* No. of blocks in sh mem */ 
  int nplcs;                      /* No. of PLCs in sh mem */ 
  int ndata_tags;                 /* No. of data tags in sh mem */ 
  int nstatus_tags;               /* No. of status tags in sh mem */ 
  int ttags;                      /* Total no. of tags in sh mem */ 

  int febe_proto_ver;             /* Front-end/Back-end protocol version */
   
} pdsconn;

/******************************************************************************
* Generic tag structure definition                                            *
******************************************************************************/
typedef struct plctag_rec
{
  char name[PDS_TAGNAME_LEN];     /* The tag's name */
  char value[PDS_TAGVALUE_LEN];   /* String representation of tag's value */
  char type;                      /* The tag's data type */
} plctag; 

/******************************************************************************
* Generic tag list structure definition                                       *
******************************************************************************/
typedef struct plctaglist_rec
{
  int ntags;                      /* The number of tags in the taglist */
  plctag *tags;                   /* A pointer to the plctag array (taglist) */
} plctaglist; 

#endif

