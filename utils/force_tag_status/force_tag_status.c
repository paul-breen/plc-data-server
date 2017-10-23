/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   force_tag_status.c                                                *
* PURPOSE:  Utility program to force a PLC's status to a specific value       *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-06-11                                                        *
******************************************************************************/

#include "force_tag_status.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pdsconn *conn = NULL;
  pdstag *p = NULL;
  force_tag_status_args args;

  if(argc < 3)
  {
    fprintf(stderr, "Usage: %s [-n] tagname {-g|-s|-b|-c tagvalue}\n",
    PROGNAME);
    exit(1);
  }

  memset(&args, 0, sizeof(args));

  parse_force_tag_status_cmdln(argc, argv, &args);

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Get a pointer to the tag.  N.B.:  We search through ALL tags
       (data & status), this means we can force status tags as well as data
       tags */
    if((p = PDSget_tag_object(conn, args.tagname)))
    {
      switch(args.op)
      {
        case 'g' :                /* The get operation */
          printf("%hu\n", PDStag_get_status(p));
        break;

        case 's' :                /* The set (value) operation */
          p->status = args.tagvalue;
          printf("%hu\n", PDStag_get_status(p));
        break;

        case 'b' :                /* The set (bit) operation */
          p->status |= args.tagvalue;
          printf("%hu\n", PDStag_get_status(p));
        break;

        case 'c' :                /* The clear (bit) operation */
          p->status &= args.tagvalue;
          printf("%hu\n", PDStag_get_status(p));
        break;
      }
    }
    else
    {
      fprintf(stderr, "%s: error - tag %s not found!\n", PROGNAME, args.tagname);
    }

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
int parse_force_tag_status_cmdln(int argc, char *argv[],
                                 force_tag_status_args *args)
{
  int opt = 0;
  extern char *optarg;
  extern int opterr, optind;

  opterr = 0;                     /* Turn off getopt()'s error messages */

  while((opt = getopt(argc, argv, "ngs: :b: :c: :")) != -1)
  {
    switch(opt)
    {
      case 'n' :                  /* The tagname */ 
        args->tagname = (char *) optarg;
      break; 

      case 'g' :                  /* The get operation */ 
        args->op = (char) opt;
      break; 

      case 's' :                  /* The set (value) operation */ 
        if(optarg)
        {
          args->op = (char) opt;

          /* Check if the user passed the special values "on" or "off" */
          if(strcasecmp(optarg, "ON") == 0)
            args->tagvalue = (unsigned short int) FORCE_ON;
          else if(strcasecmp(optarg, "OFF") == 0)
            args->tagvalue = (unsigned short int) FORCE_OFF;
          else
            args->tagvalue = (unsigned short int) atoi(optarg);
        }
        else
          args->op = (char) 0;    /* Invalidate this operation */
      break; 


      case 'b' :                  /* The set (bit) operation */ 
        if(optarg)
        {
          args->op = (char) opt;

          /* Check if the user passed the special value "on" */
          if(strcasecmp(optarg, "ON") == 0)
            args->tagvalue = (unsigned short int) FORCE_HI_ON;
          else
            args->tagvalue = (unsigned short int) atoi(optarg);
        }
        else
          args->op = (char) 0;    /* Invalidate this operation */
      break; 

      case 'c' :                  /* The clear (bit) operation */ 
        if(optarg)
        {
          args->op = (char) opt;

          /* Check if the user passed the special value "off" */
          if(strcasecmp(optarg, "OFF") == 0)
            args->tagvalue = (unsigned short int) FORCE_HI_OFF;
          else
            args->tagvalue = (unsigned short int) atoi(optarg);
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

  /* Ensure we have a tagname */
  if(!args->tagname)
    args->tagname = argv[optind];

  return 0;
}

