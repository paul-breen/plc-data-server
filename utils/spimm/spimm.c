/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   spimm.c                                                           *
* PURPOSE:  Utility program to read a shared memory segment with its data     *
*           populated from the PDS's SPI (Server Programming Interface)       *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-02                                                        *
******************************************************************************/

#include "spimm.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pds_spi_conn *conn = NULL;
  register int i = 0;
  pds_spi_tag *p = NULL;

  /* Connect to the PDS via the SPI */
  if(!(conn = (pds_spi_conn *) PDS_SPIconnect(PDS_SPI_IPCKEY)))
  {
    fprintf(stderr, "PDS SPI memory allocation error\n", PROGNAME);
    exit(1);
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    printf("%-59s|%9s|%10s\n", "TAG", "VALUE", "PERMS");
    UNDERLINE(80);

    /* Print the SPI data tags data */
    for(i = 0, p = (pds_spi_tag *) conn->data; i < conn->ndata_tags; i++, p++)
    {
      printf("%-59s|%9d|%#10x\n", p->name, p->value, p->perms);
    }
    UNDERLINE(80);

    /* Print the summary footer */
    printf("Number of SPI tags: %d\n", conn->ndata_tags);

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

