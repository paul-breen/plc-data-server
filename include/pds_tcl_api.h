/******************************************************************************
* PROJECT:  PLC data server library (Tcl interface)                           * 
* MODULE:   pds_tcl_api.h                                                     *
* PURPOSE:  Header file for the PLC data server Tcl API module                *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-03-27                                                        *
******************************************************************************/

#ifndef __PDS_TCL_API_H
#define __PDS_TCL_API_H

#include "pds_tcl_id.h"

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to connect to the PDS                                              *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: An attempt is made to connect the client program to the PDS *
*                 and if successful the connection ID is returned.  If an     *
*                 error occurs a stack trace is produced and a TCL_ERROR is   *
*                 returned                                                    *
******************************************************************************/
extern int pds_tcl_connect(ClientData cData, Tcl_Interp *interp, int argc,
                           char *argv[]);

/******************************************************************************
* Function to disconnect from the PDS                                         *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: An attempt is made to disconnect the client program from    *
*                 the PDS and if successful the connection is closed.  If an  *
*                 error occurs a stack trace is produced and a TCL_ERROR is   *
*                 returned                                                    *
******************************************************************************/
extern int pds_tcl_disconnect(ClientData cData, Tcl_Interp *interp, int argc,
                              char *argv[]);

/******************************************************************************
* Function to get a tag's value                                               *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tag's value in the PLC is returned.  If an error occurs *
*                 a stack trace is produced and a TCL_ERROR is returned       *
******************************************************************************/
extern int pds_tcl_get_tag(ClientData cData, Tcl_Interp *interp, int argc,
                           char *argv[]);

/******************************************************************************
* Function to get a string of tags' value                                     *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tags' value in the PLC are returned as a single item of *
*                 data.  If an error occurs a stack trace is produced and a   *
*                 TCL_ERROR is returned                                       *
******************************************************************************/
extern int pds_tcl_get_strtag(ClientData cData, Tcl_Interp *interp, int argc,
                              char *argv[]);

/******************************************************************************
* Function to set tag value(s)                                                *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tag value(s) is set in the PLC.  If an error occurs a   *
*                 stack trace is produced and a TCL_ERROR is returned         *
******************************************************************************/
extern int pds_tcl_set_tag(ClientData cData, Tcl_Interp *interp, int argc,
                           char *argv[]);

/******************************************************************************
* Internal function to validate a tag's data format specifier                 *
*                                                                             *
* Pre-condition:  The tag's data format specifier is passed to the function   *
* Post-condition: If the specifier is invalid a -1 is returned                *
******************************************************************************/
static int _pds_tcl_check_tag_formatspec(const char fmt);

/******************************************************************************
* Function to initialise this Tcl extension library                           *
*                                                                             *
* Pre-condition:  The Tcl interpreter is passed to the function               *
* Post-condition: The package and all its commands are registered.  If an     *
*                 error occurs an error code is returned indicating the error *
******************************************************************************/
extern int Pds_tcl_Init(Tcl_Interp *interp);

#endif

