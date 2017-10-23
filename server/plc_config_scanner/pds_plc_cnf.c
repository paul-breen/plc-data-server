/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_plc_cnf.c                                                     *
* PURPOSE:  Support functions to read a PLC configuration from a file         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#include "pds_plc_cnf.h"

extern int errout;                /* Declared in the main file */

/******************************************************************************
* Function to get a file's PLC configuration data                             *
*                                                                             *
* Pre-condition:  The filename and mode are passed to the function            *
* Post-condition: The file is opened and it's configuration data is returned  *
*                 in the struct.  If an error occurred a null is returned     *
******************************************************************************/
plc_cnf* get_plc_cnf_data(char *filename, char *filemode)
{
  plc_cnf *conf = NULL;
  FILE *fp = NULL;

  /* Open the PLC configuration file */
  if((fp = fopen(filename, filemode)))
  {
    conf = read_plc_cnf_file(fp); /* Read config file */
    fclose(fp);
  }
  else
  {
    err(errout, "error opening PLC config file '%s'\n", filename);
  }

  return conf;
}

