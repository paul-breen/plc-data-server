/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_dh_err.c                                                      *
* PURPOSE:  The DataHighway specific error functions module                   *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#include "pds_dh.h"

/* Create a lookup list of error values with corresponding error strings */
static pds_lookup_item __dh_err_items[] =
{
  {"DST node is out of buffer space!",
   DH_DST_NO_BUFFER_SPACE},
  {"Cannot guarantee delivery, remote node does not ACK command!",
   DH_DST_NO_ACK},
  {"Duplicate token holder detected!",
   DH_DUP_TOKEN_HOLDER},
  {"Local port is disconnected!",
   DH_LOCAL_PORT_DISCONNECTED},
  {"Application layer timed out waiting for response!",
   DH_RESPONSE_TIMEOUT},
  {"Duplicate node detected!",
   DH_DUP_NODE},
  {"PLC is offline!",
   DH_PLC_OFFLINE},
  {"Hardware fault!",
   DH_HWARE_FAULT},
  {"Illegal command or format!",
   DH_ILLEGAL_COMMAND},
  {"Host has a problem and will not communicate!",
   DH_REMOTE_PROBLEM},
  {"Remote node host is missing, disconnected, or shut down!",
   DH_REMOTE_NO_COMMS},
  {"Host could not complete function due to hardware fault!",
   DH_REMOTE_HWARE_FAULT},
  {"Addressing problem or memory protect rungs!",
   DH_ADDRESS_PROBLEM},	
  {"Function not allowed due to command protection selection!",
   DH_FUNC_NOT_ALLOWED},
  {"Processor is in Program mode!",
   DH_PLC_IN_PROG_MODE},
  {"Compatibility mode file missing or communication zone problem!",
   DH_COMPAT_FILE_MISSING},
  {"Remote node cannot buffer command!",
   DH_REMOTE_NO_BUFFER_SPACE},
  {"Wait ACK (1775-KA buffer full},!",
   DH_REMOTE_WAIT_ACK1},
  {"Remote node problem due to download!",
   DH_REMOTE_PROBLEM_DWNLD},
  {"Wait ACK (1775-KA buffer full},!",
   DH_REMOTE_WAIT_ACK2},
  {"A field has an illegal value!",
   DH_ILLEGAL_FIELD_VALUE},
  {"Less levels specified in address than minimum for any address!",
   DH_MIN_ADDRESS_FAULT},
  {"More levels specified in address than system supports!",
   DH_MAX_ADDRESS_FAULT},
  {"Symbol not found!",
   DH_SYMBOL_NOT_FOUND},
  {"Symbol is of improper format!",
   DH_SYMBOL_IMPROPER_FORMAT},
  {"Address does not point to something useable!",
   DH_ADDRESS_NOT_USEABLE},
  {"File is wrong size!",
   DH_FILE_WRONG_SIZE},
  {"Cannot complete request, situation changed since command started!",
   DH_SITUATION_CHANGED},
  {"Data or file is too large!",
   DH_DATA_TOO_LARGE},
  {"Transaction size plus word address is too large!",
   DH_TRANS_ADDRESS_TOO_LARGE},
  {"Access denied, improper privilege!",
   DH_ACCESS_DENIED},
  {"Condition cannot be generated, resource is not available!",
   DH_RESOURCE_NOT_AVAIL},
  {"Condition already exists, resource is already available!",
   DH_RESOURCE_ALREADY_AVAIL},
  {"Command cannot be executed!",
   DH_COMMAND_CANNOT_EXEC},
  {"Histogram overflow!",
   DH_HISTOGRAM_OVERFLOW},
  {"No access!",
   DH_NO_ACCESS},
  {"Illegal data type!",
   DH_ILLEGAL_DATA_TYPE},
  {"Invalid parameter or invalid data!",
   DH_INVALID_PARAMETER},
  {"Address reference exists to deleted area!",
   DH_ADDRESS_TO_DELETED_AREA},
  {"Command execution failure for unknown reason!",
   DH_CMD_EXEC_FAIL_UNKNOWN},
  {"Data conversion error!",
   DH_DATA_CONVERSION_ERR},
  {"Scanner not able to communicate with 1771 rack adapter!",
   DH_SCANNER_1771_COMMS_ERR},
  {"Type mismatch!",
   DH_TYPE_MISMATCH},
  {"1771 module response was not valid!",
   DH_RESPONSE_1771_INVALID},
  {"Duplicated label!",
   DH_DUP_LABEL},
  {"Remote rack fault!",
   DH_REMOTE_RACK_FAULT},
  {"Remote rack timeout!",
   DH_REMOTE_RACK_TIMEOUT},
  {"Remote rack unknown error!",
   DH_REMOTE_RACK_UNKNOWN_ERR},
  {"File is open, another node owns it!",
   DH_FILE_OWNED_OTHER_NODE},
  {"Another node is the program owner!",
   DH_PROG_OWNED_OTHER_NODE},
  {"Data table element protection violation!",
   DH_DATA_TBL_ELEM_PROTECT},
  {"Temporary internal problem!",
   DH_TMP_INTERNAL_PROBLEM},
  {"Response transaction ID did not match query transaction ID!",
   DH_TRANS_ID_MISMATCH},
  {"Unknown exception!!",
   0}                             /* Catch all error */
};

static pds_lookup_list __dh_err_list =
{(sizeof(__dh_err_items) / sizeof(pds_lookup_item)), __dh_err_items};

/******************************************************************************
* Function to print a PLC exception                                           *
*                                                                             *
* Pre-condition:  An exception code is passed to the function                 *
* Post-condition: The exception string for the given code is returned         *
******************************************************************************/
char* dh_print_plc_exception(int excode)
{
  register int i = 0;

  /* Check exception code and return message accordingly */
  for(i = 0; i < __dh_err_list.nitems; i++)
  {
    if(excode == __dh_err_list.items[i].value)
      return (char *) __dh_err_list.items[i].name;
  }

  return (char *) "Unknown exception!!";
}

