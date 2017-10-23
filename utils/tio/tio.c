/******************************************************************************
* PROJECT:  PDS Utilities                                                     *
* MODULE:   tio.c                                                             *
* PURPOSE:  Utility program to connect to the PDS and read/write a tag        *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-11-18                                                        *
******************************************************************************/

#include "tio.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  char tagvalue[PDS_TAGVALUE_LEN+1] = "\0"; 
  pdsconn *conn = NULL;
  pdstag *tag = NULL;
  tio_args args;

  if(argc < 3)
  {
    fprintf(stderr, "Usage: %s [-c -f -i] tagname {-g|-s tagvalue}\n", PROGNAME);
    exit(1);
  }
  memset(&args, 0, sizeof(args));

  parse_tio_cmdln(argc, argv, &args);

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Get a pointer to this tag object (for checking metadata) */
    if(!(tag = PDSget_tag_object(conn, args.tagname)))
    {
      fprintf(stderr, "Get tag object: tag %s not found!!\n", args.tagname);

      PDSdisconnect(conn);
      return -1;
    }

    /* Set no. of args (16 bit words) dependent upon tag's data type */
    args.nargs = (tag->type == PDS_INT32 || tag->type == PDS_FLOAT32) ? 2 : 1;

    if(args.op == 'g')            /* The get operation (formatted) */
    {
      if(PDSget_tagf(conn, args.tagname, tagvalue, args.fmt) != -1)
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
      if(PDSset_tag(conn, args.tagname, args.nargs, args.oparg) != -1)
      {
        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
          fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_plc_status(conn));
        else
          printf("%lu\n", PDS_MAKEDWORD(args.oparg[1], args.oparg[0]));
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

    PDSdisconnect(conn);
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
int parse_tio_cmdln(int argc, char *argv[], tio_args *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */

  while((opt = getopt(argc, argv, "ifcgs: :")) != -1)
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
          args->oparg[0] = (unsigned short int) PDS_GETLOWORD(atoi(optarg));
          args->oparg[1] = (unsigned short int) PDS_GETHIWORD(atoi(optarg));
        }
        else
          args->op = (char) 0;    /* Invalidate this operation */
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

