/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   pds_ctl.c                                                         *
* PURPOSE:  Utility program to control the PDS at runtime via the SPI         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-02                                                        *
******************************************************************************/

#include "pds_ctl.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  int tagvalue = 0; 
  pds_spi_conn *conn = NULL;
  pds_ctl_args args;

  if(argc < 3)
  {
    fprintf(stderr, "Usage: %s tagname {-g|-s} [tagvalue]\n", PROGNAME);
    exit(1);
  }
  parse_pds_ctl_cmdln(argc, argv, &args);

  /* Connect to the PDS via the SPI */
  if(!(conn = (pds_spi_conn *) PDS_SPIconnect(PDS_SPI_IPCKEY)))
  {
    fprintf(stderr, "PDS SPI memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    if(args.op == 'g')                 /* The get operation */
    {
      if(PDS_SPIget_tag(conn, args.tagname, &tagvalue) != -1)
        printf("%d\n", tagvalue);
      else
        fprintf(stderr, "Get tag: tag %s not found!!\n", args.tagname);
    }
    else if(args.op == 's')            /* The set operation */
    {
      if(PDS_SPIset_tag(conn, args.tagname, args.oparg) != -1)
        printf("%d\n", args.oparg);
      else
        fprintf(stderr, "Set tag: tag %s not found!!\n", args.tagname);
    }
    else
      fprintf(stderr, "%s: invalid option\n", PROGNAME);

    PDS_SPIdisconnect(conn);
  }
  else
  {
    fprintf(stderr, "%s: error connecting to the PDS SPI\n", PROGNAME);
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
int parse_pds_ctl_cmdln(int argc, char *argv[], pds_ctl_args *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */

  while((opt = getopt(argc, argv, "gs: :")) != -1)
  {
    switch(opt)
    {
      case 'g' :                  /* The get operation */ 
        args->op = (char) opt;
      break; 

      case 's' :                  /* The set operation */ 
        if(optarg)
        {
          args->op = (char) opt;
          args->oparg = atoi(optarg);
        }
        else
          args->op = (char) 0;    /* Invalidate this operation */
      break; 

      /* Option should be followed by a command line argument */ 
      case ':' :
        fprintf(stderr, "\nOption should take an argument\n");
      break;

      /* Unknown option */
      case '?' :  
        fprintf(stderr, "\nUnknown option\n");
      break;
    }
  }   
  args->tagname = argv[optind];   /* Set the tagname to the 1st non-option */

  return 0;
}

