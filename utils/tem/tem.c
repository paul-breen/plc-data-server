/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   tem.c                                                             *
* PURPOSE:  Utility program to monitor events (change of value) on a tag      *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-15                                                        *
******************************************************************************/

#include "tem.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
int quit_flag = 0;                /* Global quit flag */

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pdsconn *conn = NULL;
  pdstag *p = NULL;
  char tmstamp[TMSTAMP_LEN] = "\0";
  char tagname[PDS_TAGNAME_LEN] = "\0";
  unsigned short int prev_val = 0;

  if(argc < 2)
  {
    fprintf(stderr, "Usage: %s tagname\n", PROGNAME);    
    exit(1);
  }
  strcpy(tagname, argv[1]);

  install_signal_handler();

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Print the header */
    printf("%-19s|%-40s|%9s|%9s\n", "TIME", "TAG", "VALUE", "PREVIOUS");
    UNDERLINE(80);

    /* Get a pointer to the tag.  N.B.:  We search through ALL tags
       (data & status), this means we can monitor changes to status tags as
       well as data tags */
    if((p = PDSget_tag_object(conn, tagname)))
    {
      while(!quit_flag)
      {
        /* If the tagvalue has changed, print the data */
        if(p->value != prev_val)
        {
          /* Construct the date/time stamp */
          strftime(tmstamp, TMSTAMP_LEN, TMSTAMP_FMT, localtime(&p->mtime));

          printf("%-19s|%-40s|%9u|%9u\n", tmstamp, tagname, p->value, prev_val);

          prev_val = p->value;
        }
      }
    }
    else
    {
      fprintf(stderr, "%s: error - tag %s not found!\n", PROGNAME, tagname);
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
* Function to install a signal handler                                        *
*                                                                             *
* Pre-condition:  Program is running, signal is received                      *
* Post-condition: Signal handler is called                                    *
******************************************************************************/
void install_signal_handler()
{
  signal(SIGTERM, set_quit);
  signal(SIGINT, set_quit);
  signal(SIGQUIT, set_quit);
}



/******************************************************************************
* Function to handle quit signals                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Quit flag is set, signal handler is re-installed            *
******************************************************************************/
void set_quit()
{
  quit_flag = 1;
  install_signal_handler();
}

