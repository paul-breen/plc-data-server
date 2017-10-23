/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_nwstub_comms.c                                                *
* PURPOSE:  Communications functions for the PDS network stub                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-06-19                                                        *
******************************************************************************/

#include "pds_nwstub.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int comms_err;             /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */
extern int dbglvl;                /* Declared in the main file */

/******************************************************************************
* Function to check if any data is pending on a socket fd                     *
*                                                                             *
* Pre-condition:  The fd currently being checked is passed to the function    *
* Post-condition: The number of bytes pending is returned                     *
******************************************************************************/
int check_data_pending(int fd)
{
  int nread = 0, i = 0, ok_flag = 0;

  while(!ok_flag && (i < PDSNP_CLNTRDTRIES))
  {
    usleep(PDSNP_CLNTRDPAUSE);    /* Allow for network latency */
    ioctl(fd, FIONREAD, &nread);  /* Read no. of bytes in read buffer */

    if(nread > 0)
      ok_flag = 1;
    else
      i++;
  }
  printd("No. of bytes to be read %d\n", nread);
  printd("No. of secs. to read socket %d\n", i && PDSNP_SEC ? i * PDSNP_CLNTRDPAUSE / PDSNP_SEC : 0);

  return nread;
}



/******************************************************************************
* Function to display the comms data (for testing/debugging)                  *
*                                                                             *
* Pre-condition:  A pointer to the comms data is passed to the function       *
* Post-condition: Data in the structure is displayed on stdout                *
******************************************************************************/
int display_comms_data(pdscomms *comms)
{
  char tmpstr[PDSNP_TAGNAME_LEN+1] = "\0";

  if(dbgflag && dbglvl > 1)
  {
    if(dbglvl == 4) PDS_PRINT_BARRAYC(comms->buf, PDSNP_LEN);
    if(dbglvl == 4) PDS_PRINT_BARRAYD(comms->buf, PDSNP_LEN);

    printd("buf_len = %d\n", PDSNP_GET_BUF_LEN(comms->buf));
    printd("ver = %d\n", PDSNP_GET_VER(comms->buf));
    printd("func_id = %d\n", PDSNP_GET_FUNC_ID(comms->buf));
    printd("ex_code = %d\n", PDSNP_GET_EX_CODE(comms->buf));
    PDSNP_GET_TAGNAME(tmpstr, comms->buf);
    printd("tagname = %s\n", tmpstr);
    PDSNP_GET_TAGVALUE(tmpstr, comms->buf);
    printd("tagvalue = %s\n", tmpstr);
  }

  return 0;
}



/******************************************************************************
* Function to display 'comms footer' summary data (for testing/debugging)     *
*                                                                             *
* Pre-condition:  Comms struct ptr, write total and read total are passed to  *
*                 the function                                                *
* Post-condition: Comms summary data is displayed                             *
******************************************************************************/
int display_comms_footer(pdscomms *comms, int writetotal, int readtotal)
{
  if(dbgflag)
  {
    printd("Size of struct: %d\n", sizeof(*comms));
    printd("Size of buffer: %d\n", sizeof(comms->buf));
    printd("Size of total write: %d\n", writetotal);
    printd("Size of total read: %d\n", readtotal);
  }

  return 0;
}

