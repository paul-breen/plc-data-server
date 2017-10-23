/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_main.c                                                        *
* PURPOSE:  The main module for the PLC data server                           *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#include "pds_srv.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
int quit_flag = 0;                /* Flag to quit the program cleanly */
int errout = ERR_PRN;             /* Indicates where to send error messages */
int dbgflag = 0;                  /* Debug mode flag */
int dbglvl = 0;                   /* Debug level flag */
unsigned int runmode = 0;         /* Bit-encoded optional runtime parameters */

/******************************************************************************
* The main function                                                           *
******************************************************************************/
int main(int argc, char *argv[])
{
  pds_cmdln args;
  plc_cnf *conf = NULL;
  pdsconn conn;
  pds_spi_conn spi_conn;
  char *cnffile = NULL, *logfile = NULL;

  quit_flag = dbgflag = dbglvl = runmode = 0;
  errout = ERR_PRN;
  memset(&args, 0, sizeof(args));
  memset(&conn, 0, sizeof(conn));
  memset(&spi_conn, 0, sizeof(spi_conn));

  /* Parse the server's command line arguments */
  if(parse_pds_cmdln(argc, argv, &args) == -1)
  {
    fprintf(stderr, "%s: error parsing command line\n", PROGNAME);
    terminate();
  }

  /* Get the runtime parameters */
  dbgflag = GET_DBG_FLAG;
  dbglvl = GET_DBG_LEVEL;
  runmode = args.runmode;

  /* Determine the connection key to use */
  if(args.key)
  {
    conn.shmkey = conn.msgkey = conn.semkey = (key_t) args.key;
    spi_conn.shmkey = (key_t) args.key + 1;
  }
  else
  {
    conn.shmkey = conn.msgkey = conn.semkey = (key_t) PDS_IPCKEY;
    spi_conn.shmkey = (key_t) PDS_SPI_IPCKEY;
  }

  if(!dbgflag)
  {
    daemonise();                  /* Daemonise the program */

    /* Get log filename */
    if(!(logfile = get_log_filename(args)))
    {
      terminate();
    }
    else
    {
      if(!err_openlog(logfile, PDS_LOGMODE))
        terminate();
      else
        errout = ERR_LOG;         /* Send error messages to log */
    }
  }

  /* Log the start time */
  err(errout, "%s: starting up\n", PROGNAME);
  err(errout, "%s: PDS %s (%s)\n", PROGNAME, VERSION, CREATED);

  /* Get PLC config filename */
  if(!(cnffile = get_plc_cnf_filename(args)))
  {
    err(errout, "%s: error getting PLC configuration filename\n", PROGNAME);
    terminate();
  }
  else
  {
    err(errout, "%s: PLC config filename - %s\n", PROGNAME, cnffile);
    err(errout, "%s: connection key - %d\n", PROGNAME, conn.shmkey);
  }

  install_signal_handler();       /* Handle various signals */ 

  /* Get the config data from the PLC configuration file */
  if((conf = (plc_cnf *) get_plc_cnf_data(cnffile, PLC_CNF_FILEMODE)))
  {
    /*********************** Call the server functions ***********************/
    if(pds_server(conf, &conn, &__spi_tag_list, &spi_conn) == -1)
    {
      err(errout, "%s: server error\n", PROGNAME);
    }

    if(conf) free(conf);
    conf = (plc_cnf *) NULL;
  }

  /* Log the end time */
  err(errout, "%s: shutting down\n", PROGNAME);

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

  err(errout, "%s: cleaning up and terminating\n", PROGNAME);

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
* Function to parse the server's command line arguments                       *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure.  If an error occurs a  *
*                 -1 is returned                                              *
******************************************************************************/
int parse_pds_cmdln(int argc, char *argv[], pds_cmdln *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */
  args->runmode = 0;

  while((opt = getopt(argc, argv, "D:k:r:sd::vh")) != -1)
  {
    switch(opt)
    {
      /* The server's data/auxillary files directory switch */
      case 'D' :
        if(optarg)
          args->dir = optarg;
        else
          args->dir = (char *) NULL;
      break; 

      /* The server's connection key switch */
      case 'k' :
        if(optarg)
          args->key = atoi(optarg);
        else
          args->key = 0;
      break; 

      /* The server's refresh mode switch */
      case 'r' :
        if(optarg)
        {
          /* Set refresh bit in runmode according to option */
          if(strcasecmp(optarg, "all") == 0)
            args->runmode |= PDS_RM_REFRESH_ALL;
          else if(strcasecmp(optarg, "block") == 0)
            args->runmode |= PDS_RM_REFRESH_BLOCK;
          else
          {
            fprintf(stderr, "%s: invalid refresh mode '%s'\n",
            PROGNAME, optarg);
            return -1;
          }
        }
      break; 

      /* The server's query status mode switch */
      case 's' :
        /* Set query status bit in runmode */
        args->runmode |= PDS_RM_QUERY_STATUS;
      break; 

      /* The server's command line help switch */
      case 'h' :
        printf("Usage: %s -D data_dir -k IPC_key -r {all|block}"
        " -s -d[1-4] -v -h\n\n"
        "  -D data_dir -- the path to the PDS PLC config file\n"
        "  -k IPC_key -- an alternative to the standard PDS IPC key\n"
        "  (default = %d)\n"
        "  -r {all|block} -- the default refresh mode\n"
        "  the semaphore is released after ALL blocks in the config\n"
        "  are refreshed or after each BLOCK in the config\n"
        "  (default = all)\n"
        "  -s -- switch to run a PLC status query before each data query\n"
        "  -d[1-4] -- debug switch (and optional level)\n"
        "  level 4 gives a %d second pause between each read query\n"
        "  -h -- switch to print this help text\n",
        PROGNAME, PDS_IPCKEY, PDS_DBGPAUSE);
        exit(0);
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
        fprintf(stderr, "Try %s -h\n", PROGNAME);
        return -1;
      break;

      /* Unknown option */
      case '?' :  
        fputs("Unknown option\n", stderr);
        fprintf(stderr, "Try %s -h\n", PROGNAME);
        return -1;
      break;
    }
  }   

  /* Ensure we have mandatory runmode parameters */
  if(PDS_GET_RM_REFRESH(args->runmode) == 0)
    args->runmode |= PDS_RM_REFRESH_DEFAULT;

  return 0;
}



/******************************************************************************
* Function to get the server log filename                                     *
*                                                                             *
* Pre-condition:  A filled command line arguments structure is passed to the  *
*                 function                                                    *
* Post-condition: The log filename is returned or if an error occurred, a     *
*                 null is returned                                            *
******************************************************************************/
char* get_log_filename(pds_cmdln args)
{
  int flen = strlen(PDS_LOGFILE) + 2;       /* Filename + dirsep + strterm */
  int dlen = 0;
  char *filename;

  if(args.dir)
  {
    dlen = strlen(args.dir);

    if(!(filename = (char *) malloc(dlen + flen)))
      return (char *) NULL;

    memset(filename, 0, (dlen + flen));

    /* If dir has no trailing directory separator, then add one */
    if(args.dir[dlen - 1] != PLC_CNF_DIRSEP)
      sprintf(filename, "%s%c%s", args.dir, PLC_CNF_DIRSEP, PDS_LOGFILE);
    else
      sprintf(filename, "%s%s", args.dir, PDS_LOGFILE);

    return filename;
  }
  else
    return (char *) PDS_LOGFILE;
}



/******************************************************************************
* Function to get a PLC cnf filename                                          *
*                                                                             *
* Pre-condition:  A filled command line arguments structure is passed to the  *
*                 function                                                    *
* Post-condition: The cnf filename is returned or if an error occurred, a     *
*                 null is returned                                            *
******************************************************************************/
char* get_plc_cnf_filename(pds_cmdln args)
{
  int flen = strlen(PLC_CNF_FILENAME) + 2;  /* Filename + dirsep + strterm */
  int dlen = 0;
  char *filename;

  if(args.dir)
  {
    dlen = strlen(args.dir);

    if(!(filename = (char *) malloc(dlen + flen)))
      return (char *) NULL;

    memset(filename, 0, (dlen + flen));

    /* If dir has no trailing directory separator, then add one */
    if(args.dir[dlen - 1] != PLC_CNF_DIRSEP)
      sprintf(filename, "%s%c%s", args.dir, PLC_CNF_DIRSEP, PLC_CNF_FILENAME);
    else
      sprintf(filename, "%s%s", args.dir, PLC_CNF_FILENAME);

    return filename;
  }
  else
    return (char *) PLC_CNF_FILENAME;
}

