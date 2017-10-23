/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   addrmm.c                                                          *
* PURPOSE:  Utility program to read a shared memory segment with its address  *
*           data populated from a PLC configuration file                      *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#include "addrmm.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pdsconn *conn = NULL;
  register int i = 0;
  pdstag *p = NULL;
  char tmpstr[81] = "\0";

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Print the data tags header */
    printf("%-49s|%19s|%10s\n", "TAG", "BLOCK ADDR", "REF");
    UNDERLINE(80);

    /* Print the data tags data */
    for(i = 0, p = (pdstag *) conn->data; i < conn->ndata_tags; i++, p++)
    {
      /* Build the tag identifier - dependent on comms protocol */
      if(strlen(p->ip_addr) == 0)
        sprintf(tmpstr, "%s:%s %s", p->tty_dev, p->path, p->name);
      else
        sprintf(tmpstr, "%s:%d:%s %s", p->ip_addr, p->port, p->path, p->name);

      /* Print address data dependent on comms protocol */
      switch(p->protocol)
      {
        case MB_TCPIP :
        case MB_SERIAL_TCPIP :
        case MB_SERIAL :
          printf("%-49s|%19d|%10d\n", tmpstr, p->base_addr, p->ref);
        break;

        case DH_SERIAL_TCPIP :
        case DH_SERIAL :
        case CIP_TCPIP :
          printf("%-49s|%19s|%10d\n", tmpstr, p->ascii_addr, p->ref);
        break;
      }
      conn->plc_status |= p->status;   /* Set the query's status */
    }
    UNDERLINE(80);

    /* Print the status tags header */
    printf("%-49s|%19s|%10s\n", "PLC", "STATUS", "PATH");
    UNDERLINE(80);

    /* Print the status tags data */
    for(i = 0, p = (pdstag *) conn->status; i < conn->nstatus_tags; i++, p++)
    {
      /* Build the tag identifier - dependent on comms protocol */
      if(strlen(p->ip_addr) == 0)
        sprintf(tmpstr, "%s:%s %s", p->tty_dev, p->path, p->name);
      else
        sprintf(tmpstr, "%s:%d:%s %s", p->ip_addr, p->port, p->path, p->name);

      printf("%-49s|%19d|%10s\n", tmpstr, p->status, p->path);
    }
    UNDERLINE(80);

    /* Print the summary footer */
    printf("Total number of tags: %d\n", conn->ttags);
    printf("Number of data tags: %d\n", conn->ndata_tags);
    printf("Number of status tags: %d\n", conn->nstatus_tags);
    printf("Query status: %s (%d) \n", PDSprint_plc_status(conn), PDScheck_plc_status(conn));

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

