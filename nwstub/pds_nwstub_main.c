/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_nwstub.c                                                      *
* PURPOSE:  Server network stub for n/w socket communications to the PDS      *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-06-19                                                        *
******************************************************************************/

#include "pds_nwstub.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
int quit_flag = 0;                /* Flag to quit the program cleanly */
int errout = 0;                   /* Indicates where to send error messages */
int dbgflag;                      /* Debug flag */
int dbglvl;                       /* Debug level */

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pdscomms comms;
  nwstub_args args;

  memset(&comms, 0, sizeof(pdscomms));
  memset(&args, 0, sizeof(nwstub_args));

  /* Parse the network stub's command line arguments */
  parse_nwstub_cmdln(argc, argv, &args);

  /* Get the runtime parameters */
  dbgflag = GET_DBG_FLAG;
  dbglvl = GET_DBG_LEVEL;

  /* Connect to the server */
  if(!(comms.conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(comms.conn) == PDS_CONN_OK)
  {
    /* Handle client connections on the network stub listening socket */
    nwstub_main(&args, &comms);

    PDSdisconnect(comms.conn);
  }
  else
  {
    fprintf(stderr, "%s: error connecting to the PDS\n", PROGNAME);
    fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_conn_status(comms.conn));
    exit(1);
  }

  terminate();
}



/******************************************************************************
* Function to clean up before terminating the program                         *
*                                                                             *
* Pre-condition:  Quit flag is true                                           *
* Post-condition: Program is cleaned up and terminated                        *
******************************************************************************/
void terminate(void)
{
  int status = 0;

  fprintf(stderr, "%s: cleaning up and terminating\n", PROGNAME);

  wait(&status);                  /* Prevents child from being a zombie */

  exit(0);
}



/******************************************************************************
* Function to register signals to be handled                                  *
*                                                                             *
* Pre-condition:  Signals are handled in the default manner                   *
* Post-condition: Registered signals have specific handler functions          *
******************************************************************************/
void install_signal_handler(void)
{
  signal(SIGTERM, set_quit);
  signal(SIGINT, set_quit);
  signal(SIGQUIT, set_quit);

  signal(SIGCHLD, cleanup_child);
}



/******************************************************************************
* Function to handle quit signals                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Quit flag is set, signal handler is re-installed            *
******************************************************************************/
void set_quit(int sig)
{
  quit_flag = 1;
  install_signal_handler();
}



/******************************************************************************
* Function to handle child signal                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Child process is prevented from becoming a zombie process,  *
*                 signal handler is re-installed                              *
******************************************************************************/
void cleanup_child(int sig)
{
  int status = 0;

  wait(&status);                  /* Prevents child from being a zombie */
  install_signal_handler();
}


 
/******************************************************************************
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure                         *
******************************************************************************/
int parse_nwstub_cmdln(int argc, char *argv[], nwstub_args *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */
  args->host = PDSNP_DEF_HOST;
  args->port = PDSNP_DEF_PORT;

  while((opt = getopt(argc, argv, "h: :p: :d::v")) != -1)
  {
    switch(opt)
    {
      case 'h' :                  /* The PDS host */ 
        if(optarg)
          args->host = (char *) optarg;
      break; 

      case 'p' :                  /* The PDS port */ 
        if(optarg)
          args->port = (unsigned short int) atoi(optarg);
      break; 

      /* Debug switch.  Global debug flag is set */
      case 'd' :
        puts("Started in debug mode");
        SET_DBG_FLAG(1);

        if(optarg)
        {
          /* Optional global debug level */
          set_debug_options(1, atoi(optarg));
        }
      break;

      /* Version switch.  Prints program name, version, date & time and then
         exits */
      case 'v' :
        printf("%s : %s %s\n", argv[0], VERSION, CREATED);
        exit(0);
      break; 

      /* Option should be followed by a command line argument */ 
      case ':' :
        fputs("Option should take an argument\n", stderr);
        return -1;
      break;

      /* Unknown option */
      case '?' :  
        fputs("Unknown option\n", stderr);
        return -1;
      break;
    }
  }   

  return 0;
}



/******************************************************************************
* Function to encapsulate the nwstub's core functionality                     *
*                                                                             *
* Pre-condition:  The command line args struct & the comms struct containing  *
*                 a valid PDS connection are passed to the function           *
* Post-condition: The nwstub listens on its well-known port & accepts         *
*                 incoming client connections.  It acts as a network gateway  *
*                 to the PDS.  On error a -1 is returned                      *
******************************************************************************/
int nwstub_main(nwstub_args *args, pdscomms *comms)
{
  int serverfd = 0, clientfd = 0, clientlen = 0;
  struct sockaddr_in clientaddr;
  pid_t childpid = 0;

  if(!dbgflag)
    daemonise();                  /* Daemonise the program */

  if(dbgflag)
  { 
    struct hostent *hostinfo = NULL;

    if((hostinfo = (struct hostent *) gethostbyname(args->host)))
      display_hostinfo(hostinfo);
    printd("On port: %d\n", args->port);
  }

  install_signal_handler();       /* Handle various signals */ 

  /* Create a server socket and name it */
  if((serverfd = open_server_socket(args->host, args->port)) == -1)
  {
    fprintf(stderr, "%s: cannot create named server socket\n", PROGNAME);
    return -1;
  }

  /* Create a connection queue and wait for clients to attempt connection */
  if(listen(serverfd, PDSNP_SOCKQ) == -1)
  {
    fprintf(stderr, "%s: cannot listen on named server socket\n", PROGNAME);
    return -1;
  }

  quit_flag = 0;

  while(!quit_flag)
  {
    dbgmsg("\nServer waiting\n");

    /* Accept a client connection or block if none pending */
    clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &clientlen);

    if(clientfd != -1)
    {
      printd("Adding client on fd %d\n", clientfd);

      /* Fork a new backend server process to handle this client connection */
      if((childpid = fork()) == 0)
      {
        close(serverfd);          /* New backend closes listening conn. */

        /* New backend services this client until client disconnects */
        new_backend_main(clientfd, comms);
        close(clientfd);
        printd("Removing client on fd %d\n", clientfd);

        exit(0);                  /* Kill the new backend server process */
      }
      close(clientfd);            /* Listening server closes client conn. */
    }
  }

  close(serverfd);

  return 0;
}



/******************************************************************************
* Function to encapsulate the new backend process's core functionality        *
*                                                                             *
* Pre-condition:  The connected client socket & the comms struct containing   *
*                 a valid PDS connection are passed to the function           *
* Post-condition: The new backend server process services this client until   *
*                 the client disconnects.  On error a -1 is returned          *
******************************************************************************/
int new_backend_main(int fd, pdscomms *comms)
{
  int retval = -1;
  int nread = 0, readtotal = 0, writetotal = 0;
  fd_set fds;

  while(!quit_flag)
  {
    nread = readtotal = writetotal = 0;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    /****************** Communicate with the client socket *******************/

    /* Check for socket activity.  Block if none pending */
    if((select((fd + 1), &fds, NULL, NULL, NULL)) < 1)
      return -1;
 
    /* Ensure that there's data to be read on the socket.  If there isn't any
       it's probably because the client has closed the socket.  This is not
       an error condition */
    if((nread = check_data_pending(fd)) == 0)
      return 0;

    printd("Reading client on fd %d\n", fd);

    if((readtotal = comms_read(fd, comms)) < 0)
      return -1;

    /* Process the data & write the response back to the client */
    if(process_comms_data(comms))
    {
      printd("Writing client on fd %d\n", fd);
      display_comms_data(comms);

      if((writetotal = comms_write(fd, comms)) < 0)
        return -1;
    }

    display_comms_footer(comms, writetotal, readtotal);
    printd("Comms exception code: %d\n", PDSNP_GET_EX_CODE(comms->buf));
  }

  return retval;
}



/******************************************************************************
* Function to process the received comms data                                 *
*                                                                             *
* Pre-condition:  A comms struct is passed to the function                    *
* Post-condition: Data in the structure is processed, a flag is returned      *
*                 indicating whether a write operation is necessary or not    *
******************************************************************************/
int process_comms_data(pdscomms *comms)
{
  int write_flag = 0;
  char tagname[PDSNP_TAGNAME_LEN+1] = "\0";
  char tagvalue[PDSNP_TAGVALUE_LEN+1] = "\0";
  unsigned short int value = 0;

  dbgmsg("Processing the comms data\n");

  /* Determine which API function the client wishes to call */
  switch(PDSNP_GET_FUNC_ID(comms->buf))
  {
    case PDSNP_GET_TAG_FUNC_ID :
      PDSNP_GET_TAGNAME(tagname, comms->buf);

      /* Query PDS for this tag's value & return it to client */
      if(PDSget_tag(comms->conn, tagname, tagvalue) != -1)
      {
        printd("PDSget_tag(): %s = %s\n", tagname, tagvalue);

        PDSNP_SET_TAGVALUE(comms->buf, tagvalue);
        PDSNP_SET_EX_CODE(comms->buf, comms->conn->plc_status);
      }
      else
      {
        PDSNP_SET_EX_CODE(comms->buf, (comms->conn->plc_status | PDSNP_COMMS_APP_ERR));

        fprintf(stderr, "%s: PDSget_tag(): failed to get value of %s\n", PROGNAME, tagname);
        fprintf(stderr, "%s: PDSget_tag(): plc_status = %s (%d)\n", PROGNAME, PDSprint_plc_status(comms->conn), PDScheck_plc_status(comms->conn));
      }
      write_flag = 1;             /* Write back to the client */
    break;

    case PDSNP_SET_TAG_FUNC_ID :
      PDSNP_GET_TAGNAME(tagname, comms->buf);
      PDSNP_GET_TAGVALUE(tagvalue, comms->buf);
      sscanf(tagvalue, "%u", &value);

      /* Ask PDS to set this tag's value & return confirmation to client */
      if(PDSset_tag(comms->conn, tagname, 1, &value) != -1)
      {
        printd("PDSset_tag(): %s = %u\n", tagname, value);

        PDSNP_SET_EX_CODE(comms->buf, comms->conn->plc_status);
      }
      else
      {
        PDSNP_SET_EX_CODE(comms->buf, (comms->conn->plc_status | PDSNP_COMMS_APP_ERR));

        fprintf(stderr, "%s: PDSset_tag(): failed to set value of %s to %u\n", PROGNAME, tagname, value);
        fprintf(stderr, "%s: PDSset_tag(): plc_status = %s (%d)\n", PROGNAME, PDSprint_plc_status(comms->conn), PDScheck_plc_status(comms->conn));
      }
      write_flag = 1;             /* Write back to the client */
    break;

    default :
      PDSNP_GET_TAGNAME(tagname, comms->buf);
      PDSNP_SET_EX_CODE(comms->buf, (comms->conn->plc_status | PDSNP_COMMS_FUNC_ERR));

      fprintf(stderr, "%s: unknown function ID (%d)\n", PROGNAME, PDSNP_GET_FUNC_ID(comms->buf));
      fprintf(stderr, "%s: tagname = %s\n", PROGNAME, tagname);

      write_flag = 1;             /* Write back to the client */
    break;
  }

  return write_flag;
}

