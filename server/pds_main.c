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

  /* Setup some defaults */
  args->dir = PLC_CNF_DATA_DIR;
  args->cnf_filename = PLC_CNF_FILENAME;
  args->log_dir = PLC_CNF_DATA_DIR;
  args->log_filename = PDS_LOGFILE;
  args->key = (key_t) PDS_IPCKEY;
  args->runmode = 0;

  while((opt = getopt(argc, argv, "D:c:L:l:k:r:S:sd::vh")) != -1)
  {
    switch(opt)
    {
      /* The server's data/auxillary files directory */
      case 'D' :
        if(optarg)
          args->dir = optarg;
      break; 

      /* The server's alternative configuration filename */
      case 'c' :
        if(optarg)
          args->cnf_filename = optarg;
      break; 

      /* The server's log file directory */
      case 'L' :
        if(optarg)
          args->log_dir = optarg;
      break; 

      /* The server's alternative log filename */
      case 'l' :
        if(optarg)
          args->log_filename = optarg;
      break; 

      /* The server's connection key */
      case 'k' :
        if(optarg)
          args->key = atoi(optarg);
      break; 

      /* The server's refresh mode */
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

      /* The server's query status mode */
      case 's' :
        /* Set query status bit in runmode */
        args->runmode |= PDS_RM_QUERY_STATUS;
      break; 

      /* Set initial value for the given SPI tag */
      case 'S' :
        if(optarg)
          init_SPI_tag(optarg);
      break;

      /* The server's command line help */
      case 'h' :
        printf("Usage: %s [OPTIONS]\n"
"Daemon to make PLC data available to client programs\n\n"
"  -D data_dir -- the path to the PDS PLC config dir (default = %s)\n"
"  -c filename -- name of the PDS PLC config file (default = %s)\n"
"  -L log_dir -- the path to the PDS log dir (default = %s)\n"
"  -l filename -- name for the PDS log file (default = %s)\n"
"  -k IPC_key -- an alternative to the standard PDS IPC key (default = %d)\n"
"  -r {all|block} -- the default refresh mode (default = all)\n"
"  the semaphore is released after ALL blocks in the config\n"
"  are refreshed or after each BLOCK in the config\n"
"  -s -- run a PLC status query before each data query\n"
"  -S name=value -- set an initial value for the given SPI tag\n"
"  -d[1-4] -- debug (and optional level)\n"
"  level 4 gives a %d second pause between each read query\n"
"  -h -- print this help text\n",
        PROGNAME, PLC_CNF_DATA_DIR, PLC_CNF_FILENAME, PLC_CNF_DATA_DIR,
        PDS_LOGFILE, PDS_IPCKEY, PDS_DBGPAUSE);
        exit(0);
      break; 

      /* Debug.  Global debug flag is set */
      case 'd' :
        puts("Started in debug mode");
        SET_DBG_FLAG(1);

        if(optarg)
        {
          /* Optional global debug level */
          set_debug_options(1, atoi(optarg));
        }
      break;

      /* Version.  Prints program name, version, date & time and then exits */
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
  return construct_file_path(args.log_dir, args.log_filename, PLC_CNF_DIRSEP);
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
  return construct_file_path(args.dir, args.cnf_filename, PLC_CNF_DIRSEP);
}



/******************************************************************************
* Function to construct a file path from directory and filename               *
*                                                                             *
* Pre-condition:  The directory, the filename, and the directory separator    *
*                 character are passed to the function                        *
* Post-condition: The full file path is returned or if an error occurred, a   *
*                 null is returned                                            *
******************************************************************************/
char* construct_file_path(char *dir, char *filename, char dirsep)
{
  int len = 0;
  char *path = (char *) NULL;

  len = strlen(dir) + strlen(filename) + 2;      /* Path + dirsep + strterm */

  if((path = (char *) malloc(len)))
  {
    memset(path, 0, len);

    /* If dir has no trailing directory separator, then add one */
    if(dir[strlen(dir) - 1] != dirsep)
      snprintf(path, len, "%s%c%s", dir, dirsep, filename);
    else
      snprintf(path, len, "%s%s", dir, filename);
  }

  return path;
}



/******************************************************************************
* Function to split a string into n tokens on a given delimiter               *
*                                                                             *
* Pre-condition:  The string, delimiter and the maximum number of tokens to   *
*                 extract from the string are passed to the function          *
* Post-condition: The array of tokens is returned or if an error occurred, a  *
*                 null is returned                                            *
******************************************************************************/
char** split(char *s, char *delim, int n)
{
  int i = 0;
  char *cp = NULL, **tokens;

  if(!(tokens = malloc(n)))
  {
    err(errout, "%s: memory allocation error\n", PROGNAME);
    return NULL;
  }
  else
  {
    for(i = 0, cp = s; i < n; cp = NULL)
    {
      if(!(tokens[i++] = (char *) strtok(cp, delim)))
        break;
    }
  }

  return tokens;
}



/******************************************************************************
* Function to initialise an SPI tag from a key-value pair string              *
*                                                                             *
* Pre-condition:  The SPI tag key and value are passed as a                   *
*                 delimiter-separated string                                  *
* Post-condition: The SPI tag value is initialised in the global SPI tag list *
******************************************************************************/
int init_SPI_tag(char *kvpair)
{
  int i = 0, found = 0;
  char **kv;

  if((kv = split(kvpair, PDS_SPI_KV_DELIM, PDS_SPI_KV_N_TOKENS)))
  {
    if(kv[0] != NULL && kv[1] != NULL)
    {
      for(i = 0; i < __spi_tag_list.ntags; i++)
      {
        if(strcmp(__spi_tag_list.tags[i].name, kv[0]) == 0)
        {
          err(errout, "%s: initialising SPI tag %s to %d\n", PROGNAME, kv[0], atoi(kv[1]));
          __spi_tag_list.tags[i].value = atoi(kv[1]);
          found = 1;
        }
      }
      if(!found)
        err(errout, "%s: unrecognised SPI tag %s\n", PROGNAME, kv[0]);
    }
    else
    {
      err(errout, "%s: error parsing SPI tag KV pair %s\n", PROGNAME, kvpair);
    }
    free(kv);
  }

  return (found > 0 ? 0 : -1);
}

