/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_cip_err.c                                                     *
* PURPOSE:  The CIP specific error functions module                           *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-01-08                                                        *
******************************************************************************/

#include "pds_cip.h"

/* Create a lookup list of error values with corresponding error strings */
static pds_lookup_item __cip_err_items[] =
{
  {"Connection error!",
   CIP_CONN_ERR},
  {"Resource unavailable!",
   CIP_RESOURCE_UNAVAIL},
  {"Invalid parameter value!",
   CIP_INVAL_PARAM_VAL},
  {"Path segment error!",
   CIP_PATH_SEG_ERR},
  {"Path destination unknown!",
   CIP_PATH_DEST_UNKNOWN},
  {"Partial transfer!",
   CIP_PARTIAL_TRANSFER},
  {"Connection lost!",
   CIP_CONN_LOST},
  {"Service not supported!",
   CIP_UNSUPPORTED_SERVICE},
  {"Invalid attribute value!",
   CIP_INVAL_ATTRIB_VAL},
  {"Attribute list error!",
   CIP_ATTRIB_LIST_ERR},
  {"Object already in requested state!",
   CIP_ALREADY_IN_REQ_STATE},
  {"Object state conflict!",
   CIP_OBJ_STATE_CONFLICT},
  {"Object already exists!",
   CIP_OBJ_ALREADY_EXISTS},
  {"Attribute not settable!",
   CIP_ATTRIB_NOT_SETTABLE},
  {"Privilege violation!",
   CIP_PRIV_VIOLATION},
  {"Device state conflict!",
   CIP_DEV_STATE_CONFLICT},
  {"Reply data too large!",
   CIP_REPLY_DATA_TOO_LARGE},
  {"Fragmentation of a primitive value!",
   CIP_PRIMITIVE_FRAGMENTATION},
  {"Not enough data!",
   CIP_NOT_ENOUGH_DATA},
  {"Attribute not supported!",
   CIP_UNSUPPORTED_ATTRIB},
  {"Too much data!",
   CIP_TOO_MUCH_DATA},
  {"Object does not exist!",
   CIP_OBJ_NOT_EXISTS},
  {"Service fragmentation sequence not in progress!",
   CIP_SERVICE_FRAG_SEQ_INACTIVE},
  {"No stored attribute data!",
   CIP_UNSAVED_ATTRIB},
  {"Store operation error!",
   CIP_SAVE_ATTRIB_ERR},
  {"Routing error, request packet too large!",
   CIP_ROUTING_REQ_PKT_TOO_LARGE},
  {"Routing error, response packet too large!",
   CIP_ROUTING_RESP_PKT_TOO_LARGE},
  {"Missing attribute list entry data!",
   CIP_ATTRIB_LIST_ENTRY_MISSING},
  {"Invalid attribute value list!",
   CIP_INVAL_ATTRIB_LIST},
  {"Embedded service error!",
   CIP_EMBEDDED_SERVICE_ERR},
  {"Vendor specific error!",
   CIP_VENDOR_SPECIFIC_ERR},
  {"Invalid parameter!",
   CIP_INVAL_PARAM},
  {"Write-once value or medium already written!",
   CIP_WORM_WRITE_ERR},
  {"Invalid response received!",
   CIP_INVAL_RESPONSE_RECV},
  {"Key error in path!",
   CIP_KEY_PATH_ERR},
  {"Path size invalid!",
   CIP_INVAL_PATH_SIZE},
  {"Unexpected attribute in list!",
   CIP_UNEXPECTED_ATTRIB_IN_LIST},
  {"Invalid member ID!",
   CIP_INVAL_MEMBER_ID},
  {"Member not settable!",
   CIP_MEMBER_NOT_SETTABLE},
  {"Group 2 only server general error!",
   CIP_G2_SERVER_GENERAL_ERR},
  {"Template offset access violation!",
   CIP_TMPL_OFFSET_VIOLATION},
  {"Object access violation!",
   CIP_OBJ_ACCESS_VIOLATION},
  {"Abbreviated type mismatch!",
   CIP_TYPE_MISMATCH},
  {"Unexpected response service code!",
   CIP_SERVICE_CODE_MISMATCH},
  {"Unknown exception!!",
   0}                             /* Catch all error */
};

static pds_lookup_list __cip_err_list =
{(sizeof(__cip_err_items) / sizeof(pds_lookup_item)), __cip_err_items};

/******************************************************************************
* Function to print a PLC exception                                           *
*                                                                             *
* Pre-condition:  An exception code is passed to the function                 *
* Post-condition: The exception string for the given code is returned         *
******************************************************************************/
char* cip_print_plc_exception(int excode)
{
  register int i = 0;

  /* Check exception code and return message accordingly */
  for(i = 0; i < __cip_err_list.nitems; i++)
  {
    if(excode == __cip_err_list.items[i].value)
      return (char *) __cip_err_list.items[i].name;
  }

  return (char *) "Unknown exception!!";
}

