/******************************************************************************
* PROJECT:  PLC data server library (Tcl interface)                           * 
* MODULE:   pds_tcl_id.c                                                      *
* PURPOSE:  The PLC data server Tcl ID API module                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-03-27                                                        *
******************************************************************************/

#include "pds_tcl_id.h"

/* Define a Tcl Channel for the PDS */
Tcl_ChannelType pdsconn_type =
{
  PKGNAME,                        /* Channel type */
  NULL,                           /* Blockmodeproc */
  pdsDelConnectionId,             /* Closeproc */
  NULL,                           /* Inputproc */
  NULL,                           /* Outputproc */

  /* N.B.:  The additional args can be left NULL, or are initialized
     during a pdsSetConnectionId */
};



/******************************************************************************
* Function to create and register a new Tcl channel for a PDS connection      *
*                                                                             *
* Pre-condition:  The Tcl interpreter and a valid PDS connection struct are   *
*                 passed to the function                                      *
* Post-condition: The PDS connection struct is registered as a Tcl channel    *
*                 and identified by the connection ID                         *
******************************************************************************/
void pdsSetConnectionId(Tcl_Interp *interp, pdsconn *conn)
{
  Tcl_Channel conn_chan;
  pdsconn_id *connid = NULL;

  connid = (pdsconn_id *) ckalloc(sizeof(pdsconn_id));
  connid->conn = conn;

  /* Set this connection's ID - use conn's shared mem ID */
  sprintf(connid->id, "pds%d", conn->shmid);

  /* Create the Tcl channel */
#if TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION == 5
  /* Original signature (only seen in Tcl 7.5) */
  conn_chan = Tcl_CreateChannel(&pdsconn_type, connid->id, NULL, NULL, (ClientData) connid);
#else
  /* Tcl 7.6 and later use this */
  conn_chan = Tcl_CreateChannel(&pdsconn_type, connid->id, (ClientData) connid, TCL_READABLE | TCL_WRITABLE);
#endif

  /* Configure and register the channel */
  Tcl_SetChannelOption(interp, conn_chan, "-buffering", "line");
  Tcl_SetResult(interp, connid->id, TCL_VOLATILE);
  Tcl_RegisterChannel(interp, conn_chan);
}



/******************************************************************************
* Function to retrieve a PDS connection given it's ID                         *
*                                                                             *
* Pre-condition:  The Tcl interpreter, the connection's ID and a pointer to a *
*                 PDS connection ID struct pointer are passed to the function *
* Post-condition: The PDS connection struct is retrieved from Tcl via it's ID *
*                 and returned                                                *
******************************************************************************/
pdsconn* pdsGetConnectionId(Tcl_Interp *interp, char *id,
                            pdsconn_id **pconnid)
{
  Tcl_Channel conn_chan;
  pdsconn_id *connid = NULL;

  /* Validate the channel for this ID - check its type */
  conn_chan = Tcl_GetChannel(interp, id, 0);

  if(conn_chan == NULL || Tcl_GetChannelType(conn_chan) != &pdsconn_type)
  {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, id, " is not a valid PDS connection", 0);
    return (pdsconn *) NULL;
  }

  /* Get the connection ID struct that this channel describes */
  connid = (pdsconn_id *) Tcl_GetChannelInstanceData(conn_chan);

  if(pconnid)
    *pconnid = connid;

  return (pdsconn *) connid->conn;
}



/******************************************************************************
* Function to delete and unregister a Tcl channel for a PDS connection        *
*                                                                             *
* Pre-condition:  Any client data and the Tcl interpreter are passed to the   *
*                 function                                                    *
* Post-condition: The client program is disconnected from the PDS and the Tcl *
*                 channel for this connection is unregistered                 *
******************************************************************************/
int pdsDelConnectionId(ClientData cData, Tcl_Interp *interp)
{
  pdsconn_id *connid = NULL;

  connid = (pdsconn_id *) cData;

  if(connid)                      /* Close the PDS connection */ 
    PDSdisconnect(connid->conn);

  connid->conn = NULL;

  return 0;
}

