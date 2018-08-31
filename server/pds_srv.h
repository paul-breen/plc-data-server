/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_srv.h                                                         *
* PURPOSE:  Header file for the PLC data server                               *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#ifndef __PDS_SRV_H
#define __PDS_SRV_H 

#include <pds_config.h>

#include <stdio.h>
#include <signal.h>

#include <daemon.h>
#include <debug.h>
#include <error.h>

#include <pds_defs.h>
#include <pds_ipc.h>
#include <pds_functions.h>
#include <pds_protocols.h>
#include <pds_utils.h>
#include <pds_spi.h>

#include "plc_comms/pds_plc_comms.h"
#include "plc_config_scanner/pds_plc_cnf.h"

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PROGNAME	"pdsd"
#ifdef  PDS_VERSION
#define VERSION		"Version " PDS_VERSION
#else
#define VERSION		"Version Unknown!"
#endif
#define CREATED		"Created on " __DATE__ " at " __TIME__

#define PDS_LOGFILE	"pds.log"
#define PDS_LOGMODE	"a"

#define PDS_RDPAUSE_ALL		500000 /* usec refresh pause (read all) */
#define PDS_RDPAUSE_BLOCK	0      /* usec refresh pause (read block) */
#define PDS_WRPAUSE		100000 /* usec poll pause (write) */
#define PDS_DBGPAUSE		2      /* Debug pause (secs.) */
#define PDS_ONLINE		1      /* PDS online/offline status (bool) */
#define PDS_ONLINE_PAUSE	10     /* Online status check pause (secs.) */

#define PDS_L1_ERRX		10     /* Max. error counts */
#define PDS_L2_ERRX		5 
#define PDS_L3_ERRX		2

#define PDS_CONNERR_LIMIT(s, x) 	(((s) & PDS_PLC_CONNERR) &&\
                                         ((x) >= PDS_L1_ERRX))
#define PDS_OFFLINE_LIMIT(s, x)		(((s) & PDS_PLC_OFFLINE) &&\
                                         ((x) >= PDS_L1_ERRX))
#define PDS_COMMSERR_LIMIT(s, x)	(((s) & PDS_PLC_COMMSERR) &&\
                           		 ((x) >= PDS_L2_ERRX))
#define PDS_RESPERR_LIMIT(s, x) 	(((s) & PDS_PLC_RESPERR) &&\
                                         ((x) >= PDS_L3_ERRX))

#define PDS_RM_REFRESH_ALL		0x01
#define PDS_RM_REFRESH_BLOCK		0x02
#define PDS_RM_REFRESH_DEFAULT		PDS_RM_REFRESH_ALL

#define PDS_RM_REFRESH_BITMASK		0x03
#define PDS_GET_RM_REFRESH(r)		((r) & PDS_RM_REFRESH_BITMASK)

#define PDS_RM_QUERY_STATUS		0x04

#define PDS_RM_STATUS_BITMASK		0x04
#define PDS_GET_RM_STATUS(r)		((r) & PDS_RM_STATUS_BITMASK)

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The server's command line options struct definition                         *
******************************************************************************/
typedef struct pds_cmdln_rec
{
  char *dir;                      /* The server's data directory */
  char *cnf_filename;             /* The server's configuration file */
  char *log_dir;                  /* The server's log directory */
  char *log_filename;             /* The server's log file */
  key_t key;                      /* The server's connection key */
  unsigned int runmode;           /* The server's runmode */
} pds_cmdln;

#ifdef _SEM_SEMUN_UNDEFINED

/* N.B.:  This is the comment and union definition from
          '/usr/include/bits/sem.h' */

/* Previous versions of this file used to define this union but this is
   incorrect.  One can test the macro _SEM_SEMUN_UNDEFINED to see whether
   one must define the union or not.  */

/******************************************************************************
* The semaphore union for arguments to 'semctl'                               *
******************************************************************************/
union semun
{
  int val;                        /* Value for SETVAL */
  struct semid_ds *buf;           /* Buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;      /* Array for GETALL & SETALL */
  struct seminfo *__buf;          /* Buffer for IPC_INFO */
};

#endif

/******************************************************************************
* The server's query struct definition                                        *
******************************************************************************/
typedef struct pdsquery_rec
{
  unsigned short int protocol;         /* Comms protocol */
  unsigned short int block_id;         /* Block ID */
  char ip_addr[PDS_IP_ADDR_LEN];       /* IP address */
  unsigned short int port;             /* TCP port */
  char tty_dev[PDS_TTY_DEV_LEN];       /* TTY device */
  char path[PDS_PLC_PATH_LEN];         /* Routing path of PLC */
  int pollrate;                        /* Block's poll rate (in usecs) */
  unsigned char query[PDS_MAXBUFLEN];  /* Query */
  short int qlen;                      /* Query length */
  unsigned short int *status;          /* Status word pointer */
  unsigned short int errx;             /* Error counter */
} pdsquery;

/******************************************************************************
* The server's queries struct definition                                      *
******************************************************************************/
typedef struct pdsqueries_rec
{
  int nqueries;                        /* No. of queries in this structure */
  pdsquery *queries;                   /* Array of query structures */
} pdsqueries;

/******************************************************************************
* The server's transaction struct definition                                  *
******************************************************************************/
typedef struct pdstrans_rec
{
  unsigned short int protocol;              /* Comms protocol */
  unsigned short int block_id;              /* Block ID */
  pdstag *block_start;                      /* Pointer to 1st tag in block */
  int pollrate;                             /* Block's poll rate (in usecs) */

  unsigned char query[PDS_MAXBUFLEN];       /* Query */
  short int qlen;                           /* Query length */

  unsigned char response[PDS_MAXBUFLEN];    /* Response */
  short int rlen;                           /* Response length */

  unsigned char buf[PDS_MAXBUFLEN];         /* Buffer */
  short int blen;                           /* Buffer length */

  unsigned short int *status;               /* Status word pointer */
  unsigned short int *errx;                 /* Error counter pointer */

  unsigned short int trans_id;              /* Transaction ID */
} pdstrans;

/******************************************************************************
* The server's SPI default configuration settings                             *
******************************************************************************/
static pds_spi_tag __spi_tags[] =
{
  {"PDS_RDPAUSE_ALL", PDS_RDPAUSE_ALL, PDS_SPI_PERM_RDWR},
  {"PDS_RDPAUSE_BLOCK", PDS_RDPAUSE_BLOCK, PDS_SPI_PERM_RDWR},
  {"PDS_WRPAUSE", PDS_WRPAUSE, PDS_SPI_PERM_RDWR},
  {"PDS_DBGPAUSE", PDS_DBGPAUSE, PDS_SPI_PERM_RDWR},
  {"PDS_ONLINE", PDS_ONLINE, PDS_SPI_PERM_RDWR}
};

static pds_spi_tag_list __spi_tag_list =
{(sizeof(__spi_tags) / sizeof(pds_spi_tag)), __spi_tags};

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to clean up before terminating the program                         *
*                                                                             *
* Pre-condition:  Quit flag is true                                           *
* Post-condition: Program is cleaned up and terminated                        *
******************************************************************************/
void terminate(void);

/******************************************************************************
* Function to register signals to be handled                                  *
*                                                                             *
* Pre-condition:  Signals are handled in the default manner                   *
* Post-condition: Registered signals have specific handler functions          *
******************************************************************************/
void install_signal_handler(void);

/******************************************************************************
* Function to handle quit signals                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Quit flag is set, signal handler is re-installed            *
******************************************************************************/
void set_quit(int sig);

/******************************************************************************
* Function to handle child signal                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Child process is prevented from becoming a zombie process,  *
*                 signal handler is re-installed                              *
******************************************************************************/
void cleanup_child(int sig);

/******************************************************************************
* Function to parse the server's command line arguments                       *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure.  If an error occurs a  *
*                 -1 is returned                                              *
******************************************************************************/
int parse_pds_cmdln(int argc, char *argv[], pds_cmdln *args);

/******************************************************************************
* Function to get the server log filename                                     *
*                                                                             *
* Pre-condition:  A filled command line arguments structure is passed to the  *
*                 function                                                    *
* Post-condition: The log filename is returned or if an error occurred, a     *
*                 null is returned                                            *
******************************************************************************/
char* get_log_filename(pds_cmdln args);

/******************************************************************************
* Function to get a PLC cnf filename                                          *
*                                                                             *
* Pre-condition:  A filled command line arguments structure is passed to the  *
*                 function                                                    *
* Post-condition: The cnf filename is returned or if an error occurred, a     *
*                 null is returned                                            *
******************************************************************************/
char* get_plc_cnf_filename(pds_cmdln args);

/******************************************************************************
* Function to construct a file path from directory and filename               *
*                                                                             *
* Pre-condition:  The directory, the filename, and the directory separator    *
*                 character are passed to the function                        *
* Post-condition: The full file path is returned or if an error occurred, a   *
*                 null is returned                                            *
******************************************************************************/
char* construct_file_path(char *dir, char *filename, char dirsep);

/******************************************************************************
* Function to initialise the server connection                                *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection struct are  *
*                 passed to the function                                      *
* Post-condition: The server connection is initialised and the configuration  *
*                 file tags are mapped to variables in shared memory.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int init_server_connection(plc_cnf *conf, pdsconn *conn);

/******************************************************************************
* Function to release the server connection                                   *
*                                                                             *
* Pre-condition:  A valid connection struct is passed to the function         *
* Post-condition: The server connection's resources are released.  If an      *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int release_server_connection(pdsconn *conn);

/******************************************************************************
* Function to setup the semaphore(s)                                          *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The semaphores are setup and the semaphore members of the   *
*                 connection struct are initialised accordingly.  If an error *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int init_semaphores(pdsconn *conn);

/******************************************************************************
* Function to setup the shared memory segment                                 *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The shared memory segment is setup and the shared memory    *
*                 members of the connection struct are initialised            *
*                 accordingly.  If an error occurs a -1 is returned           *
******************************************************************************/
int init_shared_mem(pdsconn *conn);

/******************************************************************************
* Function to setup the message queue                                         *
*                                                                             *
* Pre-condition:  The partially initialised connection struct is passed to    *
*                 the function                                                *
* Post-condition: The message queue is setup and the message queue members    *
*                 of the connection struct are initialised accordingly.  If   *
*                 an error occurs a -1 is returned                            *
******************************************************************************/
int init_msg_queue(pdsconn *conn);

/******************************************************************************
* Function to initialise the SPI server connection                            *
*                                                                             *
* Pre-condition:  The SPI tag list and the SPI connection struct are passed   *
*                 to the function                                             *
* Post-condition: The SPI server connection is initialised and the SPI tags   *
*                 are mapped to variables in shared memory.  If an error      *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int init_SPI_server_connection(pds_spi_tag_list *tag_list, pds_spi_conn *conn);

/******************************************************************************
* Function to release the SPI server connection                               *
*                                                                             *
* Pre-condition:  A valid SPI connection struct is passed to the function     *
* Post-condition: The SPI server connection's resources are released.  If an  *
*                 error occurs a -1 is returned                               *
******************************************************************************/
int release_SPI_server_connection(pds_spi_conn *conn);

/******************************************************************************
* Function to setup the SPI shared memory segment                             *
*                                                                             *
* Pre-condition:  The partially initialised SPI connection struct is passed   *
*                 to the function                                             *
* Post-condition: The shared memory segment is setup and the shared memory    *
*                 members of the connection struct are initialised            *
*                 accordingly.  If an error occurs a -1 is returned           *
******************************************************************************/
int init_SPI_shared_mem(pds_spi_conn *conn);

/******************************************************************************
* Function to setup a shared memory segment                                   *
*                                                                             *
* Pre-condition:  The shared memory key, size (in bytes), flags and an int to *
*                 hold the id are passed to the function                      *
* Post-condition: The shared memory segment is created and 'this' process is  *
*                 attached to it.  A pointer to the 'zero initialised' memory *
*                 is returned or a null pointer if an error occurs            *
******************************************************************************/
void* setup_shm(key_t shmkey, size_t shmsize, int shmflags, int *shmid);

/******************************************************************************
* Function to release a shared memory segment                                 *
*                                                                             *
* Pre-condition:  A valid shared memory pointer and valid shared memory id    *
*                 are passed to the function                                  *
* Post-condition: The shared memory segment is released (detached and freed). *
*                 If an error occurred a -1 is returned                       *
******************************************************************************/
int release_shm(void *shm, int shmid);

/******************************************************************************
* Function to map PLC addresses to memory variable tags in shared memory      *
*                                                                             *
* Pre-condition:  The PLC configuration struct and a valid connection struct  *
*                 containing a shared memory pointer are passed to the        *
*                 function                                                    *
* Post-condition: The tags are created as memory variables (in shared memory) *
*                 and map directly to PLC addresses.  The total count of tags *
*                 is returned or -1 if an error occurs                        *
******************************************************************************/
int map_shm(plc_cnf *conf, pdsconn *conn);

/******************************************************************************
* Function to set the value of a semaphore                                    *
*                                                                             *
* Pre-condition:  A valid semaphore ID, the value for the operation and the   *
*                 semaphore set array number are passed to the function       *
* Post-condition: The semaphore is set with the passed value.  If an error    *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int semset(int id, int op, int snum);

/******************************************************************************
* Function to map SPI tags to memory variable tags in shared memory           *
*                                                                             *
* Pre-condition:  The SPI tag list and a valid SPI connection struct          *
*                 containing a shared memory pointer are passed to the        *
*                 function                                                    *
* Post-condition: The tags are created as memory variables (in shared memory) *
*                 and map directly to SPI tags.  If an error occurs, a -1 is  *
*                 returned                                                    *
******************************************************************************/
int map_SPI_shm(pds_spi_tag_list *tag_list, pds_spi_conn *conn);

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
int pds_server(plc_cnf *conf, pdsconn *conn, pds_spi_tag_list *spi_tag_list,
               pds_spi_conn *spi_conn);

/******************************************************************************
* Function to handle read requests                                            *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection structs are *
*                 passed to the function                                      *
* Post-condition: All read queries for this configuration are setup and run   *
*                 against the PLC.  If an error occurs a -1 is returned       *
******************************************************************************/
int handle_read_requests(plc_cnf *conf, pdsconn *conn, pds_spi_conn *spi_conn);

/******************************************************************************
* Function to setup all read/mapped-write block queries in this configuration *
*                                                                             *
* Pre-condition:  The PLC configuration struct and the connection struct are  *
*                 passed to the function                                      *
* Post-condition: A query is constructed for each read/mapped-write block in  *
*                 this configuration and a pointer to the queries struct is   *
*                 returned.  If an error occurs a null is returned            *
******************************************************************************/
pdsqueries* setup_read_queries(plc_cnf *conf, pdsconn *conn);

/******************************************************************************
* Function to free memory for all read queries                                *
*                                                                             *
* Pre-condition:  The queries struct is passed to the function                *
* Post-condition: Memory is freed for the queries struct.  If an error occurs *
*                 a -1 is returned                                            *
******************************************************************************/
int free_read_queries(pdsqueries *queries);

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
                         pdsqueries *queries);

/******************************************************************************
* Function to read data from a PLC                                            *
*                                                                             *
* Pre-condition:  The connection struct & a transaction struct containing the *
*                 query & storage for the response are passed to the function *
* Post-condition: The query is ran against the PLC.  The response is checked  *
*                 for exceptions and returned in the response buffer.  The    *
*                 no. of bytes in the response is returned or a -1 on error   *
******************************************************************************/
int read_from_plc(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to handle - write data to PLC/client initialisation - requests     *
*                                                                             *
* Pre-condition:  The connection structs are passed to the function           *
* Post-condition: Client requests to write data to the PLC and a client's     *
*                 initial request to connect to the server are handled.  If   *
*                 an error occurs a -1 is returned                            *
******************************************************************************/
int handle_write_requests(pdsconn *conn, pds_spi_conn *spi_conn);

/******************************************************************************
* Function to write client data to the PLC                                    *
*                                                                             *
* Pre-condition:  The connection struct and the message struct containing     *
*                 data to write to the PLC are passed to the function         *
* Post-condition: The PLC query is constructed with data in the message and   *
*                 then written to the PLC.  If an error occurs a -1 is        *
*                 returned                                                    *
******************************************************************************/
int write_to_plc(pdsconn *conn, pdsmsg *msg);

/******************************************************************************
* Function to set tag's status words                                          *
*                                                                             *
* Pre-condition:  The connection struct and the PLC's status value are passed *
*                 to the function                                             *
* Post-condition: All tags on this PLC have their status words set to the     *
*                 PLC's status value.  A count of updated tags is returned    *
******************************************************************************/
int set_tags_status(pdsconn *conn, unsigned short int status);

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
                      unsigned short int *errx);

#endif

