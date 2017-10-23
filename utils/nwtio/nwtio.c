/******************************************************************************
* PROJECT:  PDS Utilities                                                     *
* MODULE:   nwtio.c                                                           *
* PURPOSE:  Utility program to connect to the PDS network stub and read/write *
*           a tag                                                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-18                                                        *
******************************************************************************/

#include "nwtio.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  char tagvalue[PDS_TAGVALUE_LEN+1] = "\0"; 
  pdsconn *conn = NULL;
  nwtio_args args;

  if(argc < 3)
  {
    fprintf(stderr, "Usage: %s -h host -p port [-c -f -i] tagname {-g|-s tagvalue}\n", PROGNAME);
    exit(1);
  }
  memset(&args, 0, sizeof(args));

  parse_nwtio_cmdln(argc, argv, &args);

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSNPconnect(args.host, args.port)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    if(args.op == 'g')            /* The get operation (formatted) */
    {
      if(PDSNPget_tag(conn, args.tagname, tagvalue) != -1)
      {
        /* Check the status of the PLC at query time.  If a status
           condition exists on the connection, print the PLC status message */
        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
          fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_plc_status(conn));
        else
          printf("%s\n", tagvalue);
      }
      else
      {
        fprintf(stderr, "%s: failed to get tag %s value\n", PROGNAME, args.tagname);

        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
          fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_plc_status(conn));
      }
    }
    else if(args.op == 's')       /* The set operation */
    {
      if(PDSNPset_tag(conn, args.tagname, 1, &args.oparg) != -1)
      {
        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
          fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_plc_status(conn));
        else
          printf("%d\n", args.oparg);
      }
      else
      {
        fprintf(stderr, "%s: failed to set tag %s value\n", PROGNAME, args.tagname);

        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
          fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_plc_status(conn));
      }
    }
    else
      fprintf(stderr, "%s: invalid option\n", PROGNAME);

    PDSNPdisconnect(conn);
  }
  else
  {
    fprintf(stderr, "%s: error connecting to the PDS\n", PROGNAME);
    fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_conn_status(conn));
    exit(1);
  }

  return 0;
}


 
/******************************************************************************
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure                         *
******************************************************************************/
int parse_nwtio_cmdln(int argc, char *argv[], nwtio_args *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */
  args->host = PDSNP_DEF_HOST;
  args->port = PDSNP_DEF_PORT;

  while((opt = getopt(argc, argv, "ifcgs: :h: :p: :")) != -1)
  {
    switch(opt)
    {
      case 'i' :                  /* The data format specifier */ 
      case 'f' :
      case 'c' :
        args->fmt = (char) opt;
      break; 

      case 'g' :                  /* The get operation */ 
        args->op = (char) opt;
      break; 

      case 's' :                  /* The set operation */ 
        if(optarg)
        {
          args->op = (char) opt;
          args->oparg = (unsigned short int) atoi(optarg);
        }
        else
          args->op = (char) 0;    /* Invalidate this operation */
      break; 

      case 'h' :                  /* The PDS host */ 
        if(optarg)
          args->host = (char *) optarg;
      break; 

      case 'p' :                  /* The PDS port */ 
        if(optarg)
          args->port = (unsigned short int) atoi(optarg);
      break; 

      /* Option should be followed by a command line argument */ 
      case ':' :
        fputs("Option should take an argument\n", stderr);
      break;

      /* Unknown option */
      case '?' :  
        fputs("Unknown option\n", stderr);
      break;
    }
  }   
  args->tagname = argv[optind];   /* Set the tagname to the 1st non-option */

  return 0;
}

