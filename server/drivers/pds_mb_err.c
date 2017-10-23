/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_mb_err.c                                                      *
* PURPOSE:  The ModBus specific error functions module                        *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#include "pds_mb.h"

/* Create a lookup list of error values with corresponding error strings */
static pds_lookup_item __mb_err_items[] =
{
  {"Illegal function!", 
   MB_ILLEGAL_FUNCTION},            
  {"Illegal data address!",
   MB_ILLEGAL_DATA_ADDRESS},
  {"Illegal data value!",
   MB_ILLEGAL_DATA_VALUE},
  {"Illegal response length!",
   MB_ILLEGAL_RESPONSE_LENGTH},
  {"Acknowledge!",
   MB_ACKNOWLEDGE},
  {"Slave device busy!",
   MB_SLAVE_DEVICE_BUSY},
  {"Negative acknowledge!",
   MB_NEGATIVE_ACKNOWLEDGE},
  {"Memory parity error!",
   MB_MEMORY_PARITY_ERROR},
  {"Incorrect response size!",
   MB_INCORRECT_RESPONSE_SIZE},
  {"ModBus+ gateway path unavailable!",
   MB_MBP_GW_PATH_UNAVAILABLE},
  {"ModBus+ gateway target device failed to respond!",
   MB_MBP_GW_DEVICE_NO_RESP},
  {"Unknown exception!!",
   0}                             /* Catch all error */
};

static pds_lookup_list __mb_err_list =
{(sizeof(__mb_err_items) / sizeof(pds_lookup_item)), __mb_err_items};

/******************************************************************************
* Function to print a PLC exception                                           *
*                                                                             *
* Pre-condition:  An exception code is passed to the function                 *
* Post-condition: The exception string for the given code is returned         *
******************************************************************************/
char* mb_print_plc_exception(int excode)
{
  register int i = 0;

  /* Check exception code and return message accordingly */
  for(i = 0; i < __mb_err_list.nitems; i++)
  {
    if(excode == __mb_err_list.items[i].value)
      return (char *) __mb_err_list.items[i].name;
  }

  return (char *) "Unknown exception!!";
}

