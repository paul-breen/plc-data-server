/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   cnf-read.c                                                        *
* PURPOSE:  Utility program to print the contents of a PLC configuration file *
*           to stdout                                                         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#include "cnf-read.h"

int errout = 0;                   /* Indicates where to send error messages */

/******************************************************************************
* The main function                                                           *
******************************************************************************/
int main(int argc, char *argv[])
{
  plc_cnf *conf = NULL;
  char *cnffile = NULL;
  int i = 0;

  if(argc < 2)
  {
    fprintf(stderr, "Usage: %s config-filename\n", PROGNAME);
    exit(1);
  }

  cnffile = argv[1];

  /* Get the config data from the PLC configuration file & print to stdout */
  if((conf = get_plc_cnf_data(cnffile, PLC_CNF_FILEMODE)))
  {
    print_plc_cnf_data(conf);

    if(conf) free(conf);
  }
  conf = (plc_cnf *) NULL;

  return 0;
}



/******************************************************************************
* Function to print the contents of a PLC cnf file to stdout                  *
*                                                                             *
* Pre-condition:  A PLC configuration structure is passed to the function     *
* Post-condition: The contents of the cnf file is printed on stdout           *
******************************************************************************/
int print_plc_cnf_data(plc_cnf *conf)
{
  int i = 0, j = 0;

  /* Print the file header (block, PLC, tag counters etc.) */
  puts("File header");
  printf("No. of Blocks (nblocks) = %d\n", conf->nblocks);
  printf("No. of PLCs (nplcs) = %d\n", conf->nplcs);
  printf("No. of Data Tags (ndata_tags) = %d\n", conf->ndata_tags);
  printf("No. of Status Tags (nstatus_tags) = %d\n", conf->nstatus_tags);
  printf("Total no. of Tags (ttags) = %d\n", conf->ttags);

  UNDERLINE('#', NCOLS);

  /* Print each block header's data */
  for(i = 0; i < conf->nblocks; i++)
  {
    printf("Block %d Header\n", i);
    printf("Protocol Code (protocol) = %d (%s)\n", conf->blocks[i].protocol, PDS_PRINT_PROTOTYPE(PDS_GET_PROTOTYPE(conf->blocks[i].protocol)));
    printf("Function Code (function) = %d (%s)\n", conf->blocks[i].function, PDS_PRINT_FUNCTYPE(PDS_GET_FUNCTYPE(conf->blocks[i].function)));
    printf("Routing Path (path) = %s\n", conf->blocks[i].path);
    printf("IP Address (ip_addr) = %s\n", conf->blocks[i].ip_addr);
    printf("TCP Port (port) = %d\n", conf->blocks[i].port);
    printf("TTY device (tty_dev) = %s\n", conf->blocks[i].tty_dev);
    printf("Base Address (base_addr) = %d\n", conf->blocks[i].base_addr);
    printf("ASCII Address (ascii_addr) = %s\n", conf->blocks[i].ascii_addr);
    printf("Poll Rate (pollrate) = %d\n", conf->blocks[i].pollrate);
    printf("No. of Tags in Block (ntags) = %d\n", conf->blocks[i].ntags);
    UNDERLINE('-', NCOLS);

    /* For each block, print it's tags' data */
    for(j = 0; j < conf->blocks[i].ntags; j++)
    {
      printf("Tag %d in Block %d\n", j, i);
      printf("Tag Name (name) = %s\n", conf->blocks[i].tags[j].name);
      printf("Tag Reference (ref) = %d\n", conf->blocks[i].tags[j].ref);
      printf("Tag ASCII Reference (ascii_ref) = %s\n", conf->blocks[i].tags[j].ascii_ref);
      printf("Tag Bit (bit) = %u\n", conf->blocks[i].tags[j].bit);
      if(j < conf->blocks[i].ntags - 1) DASHEDLINE('-', NCOLS);
    }
    UNDERLINE('#', NCOLS);
  }

  /* Print each PLC's data */
  for(i = 0; i < conf->nplcs; i++)
  {
    printf("PLC %d\n", i);
    printf("Protocol Code (protocol) = %d (%s)\n", conf->plcs[i].protocol, PDS_PRINT_PROTOTYPE(PDS_GET_PROTOTYPE(conf->plcs[i].protocol)));
    printf("IP Address (ip_addr) = %s\n", conf->plcs[i].ip_addr);
    printf("TCP Port (port) = %d\n", conf->plcs[i].port);
    printf("TTY device (tty_dev) = %s\n", conf->blocks[i].tty_dev);
    printf("Routing Path (path) = %s\n", conf->plcs[i].path);
    if(i < conf->nplcs - 1) UNDERLINE('#', NCOLS);
  }

  return 0;
}

