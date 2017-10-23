/******************************************************************************
* PROJECT:  PLC data server library (Tcl interface)                           * 
* MODULE:   pds_tcl_id.h                                                      *
* PURPOSE:  Header file for the PLC data server Tcl ID API module             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-03-27                                                        *
******************************************************************************/

#ifndef __PDS_TCL_ID_H
#define __PDS_TCL_ID_H

#include "tcl.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <pds_api.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/
#define PKGNAME		"PDS"
#define PKGVERSION	"1.3"

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The 'PLC data server API connection structure' Tcl wrapper structure        *
******************************************************************************/
typedef struct pdsconn_id_rec
{
  char id[33];                    /* The Tcl identifier */
  pdsconn *conn;                  /* A PDS connection structure */
} pdsconn_id;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to create and register a new Tcl channel for a PDS connection      *
*                                                                             *
* Pre-condition:  The Tcl interpreter and a valid PDS connection struct are   *
*                 passed to the function                                      *
* Post-condition: The PDS connection struct is registered as a Tcl channel    *
*                 and identified by the connection ID                         *
******************************************************************************/
void pdsSetConnectionId(Tcl_Interp *interp, pdsconn *conn);

/******************************************************************************
* Function to retrieve a PDS connection given it's ID                         *
*                                                                             *
* Pre-condition:  The Tcl interpreter, the connection's ID and a pointer to a *
*                 PDS connection ID struct pointer are passed to the function *
* Post-condition: The PDS connection struct is retrieved from Tcl via it's ID *
*                 and returned                                                *
******************************************************************************/
pdsconn* pdsGetConnectionId(Tcl_Interp *interp, char *id,
                            pdsconn_id **pconnid);

/******************************************************************************
* Function to delete and unregister a Tcl channel for a PDS connection        *
*                                                                             *
* Pre-condition:  Any client data and the Tcl interpreter are passed to the   *
*                 function                                                    *
* Post-condition: The client program is disconnected from the PDS and the Tcl *
*                 channel for this connection is unregistered                 *
******************************************************************************/
int pdsDelConnectionId(ClientData cData, Tcl_Interp *interp);

#endif

