/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_nwstub.h                                                      *
* PURPOSE:  Header file of the network stub for the PDS                       *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-06-19                                                        *
******************************************************************************/

#ifndef __PDS_NWSTUB_H
#define __PDS_NWSTUB_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>

#include <daemon.h>
#include <debug.h>
#include <pdsnp_defs.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* General defines */
#define VERSION			"Version 1.0"
#define CREATED			"Created on " __DATE__ " at " __TIME__
#define PROGNAME		"pds_nwstubd"

#define PDS_NWSTUB_DEF_HOST	"localhost"
#define PDS_NWSTUB_DEF_PORT	9574

#define PDS_NWSTUB_SOCKQ	5
#define PDS_NWSTUB_MAXFD	32 
#define PDS_NWSTUB_TMO_SECS	15
#define PDS_NWSTUB_TMO_USECS	0
#define PDS_NWSTUB_SEC		1000000
#define PDS_NWSTUB_CLNTRDPAUSE	(PDS_NWSTUB_SEC / 2)
#define PDS_NWSTUB_CLNTRDTRIES	8

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* nwstub's command line arguments struct definition                           *
******************************************************************************/
typedef struct nwstub_args_rec
{
  char *host;                     /* The host {IP address|hostname} */
  unsigned short int port;        /* The nwstub's well-known port */
} nwstub_args;

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
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure                         *
******************************************************************************/
int parse_nwstub_cmdln(int argc, char *argv[], nwstub_args *args);

/******************************************************************************
* Function to encapsulate the nwstub's core functionality                     *
*                                                                             *
* Pre-condition:  The command line args struct & the comms struct containing  *
*                 a valid PDS connection are passed to the function           *
* Post-condition: The nwstub listens on its well-known port & accepts         *
*                 incoming client connections.  It acts as a network gateway  *
*                 to the PDS.  On error a -1 is returned                      *
******************************************************************************/
int nwstub_main(nwstub_args *args, pdscomms *comms);

/******************************************************************************
* Function to encapsulate the new backend process's core functionality        *
*                                                                             *
* Pre-condition:  The connected client socket & the comms struct containing   *
*                 a valid PDS connection are passed to the function           *
* Post-condition: The new backend server process services this client until   *
*                 the client disconnects.  On error a -1 is returned          *
******************************************************************************/
int new_backend_main(int fd, pdscomms *comms);

/******************************************************************************
* Function to process the received comms data                                 *
*                                                                             *
* Pre-condition:  A comms struct is passed to the function                    *
* Post-condition: Data in the structure is processed, a flag is returned      *
*                 indicating whether a write operation is necessary or not    *
******************************************************************************/
int process_comms_data(pdscomms *comms);

/******************************************************************************
* Function to check if any data is pending on a socket fd                     *
*                                                                             *
* Pre-condition:  The fd currently being checked is passed to the function    *
* Post-condition: The number of bytes pending is returned                     *
******************************************************************************/
int check_data_pending(int fd);

/******************************************************************************
* Function to display the comms data (for testing/debugging)                  *
*                                                                             *
* Pre-condition:  A pointer to the comms data is passed to the function       *
* Post-condition: Data in the structure is displayed on stdout                *
******************************************************************************/
int display_comms_data(pdscomms *comms);

/******************************************************************************
* Function to display 'comms footer' summary data (for testing/debugging)     *
*                                                                             *
* Pre-condition:  Comms struct ptr, write total and read total are passed to  *
*                 the function                                                *
* Post-condition: Comms summary data is displayed                             *
******************************************************************************/
int display_comms_footer(pdscomms *comms, int writetotal, int readtotal);

#endif

