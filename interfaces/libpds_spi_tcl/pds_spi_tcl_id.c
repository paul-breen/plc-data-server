/******************************************************************************
* PROJECT:  PLC data server SPI library (Tcl interface)                       * 
* MODULE:   pds_tcl_id.c                                                      *
* PURPOSE:  The PLC data server SPI Tcl ID SPI module                         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-03                                                        *
******************************************************************************/

#include "pds_spi_tcl_id.h"

/* Define a Tcl Channel for the PDS SPI */
Tcl_ChannelType pds_spi_conn_type =
{
  PKGNAME,                        /* Channel type */
  NULL,                           /* Blockmodeproc */
  pds_spi_DelConnectionId,        /* Closeproc */
  NULL,                           /* Inputproc */
  NULL,                           /* Outputproc */

  /* N.B.:  The additional args can be left NULL, or are initialized
     during a pds_spi_SetConnectionId */
};



/******************************************************************************
* Function to create and register a new Tcl channel for a PDS SPI connection  *
*                                                                             *
* Pre-condition:  The Tcl interpreter and a valid PDS SPI connection struct   *
*                 are passed to the function                                  *
* Post-condition: The PDS SPI connection struct is registered as a Tcl        *
*                 channel and identified by the connection ID                 *
******************************************************************************/
void pds_spi_SetConnectionId(Tcl_Interp *interp, pds_spi_conn *conn)
{
  Tcl_Channel conn_chan;
  pds_spi_conn_id *connid = NULL;

  connid = (pds_spi_conn_id *) ckalloc(sizeof(pds_spi_conn_id));
  connid->conn = conn;

  /* Set this SPI connection's ID - use conn's shared mem ID */
  sprintf(connid->id, "pds_spi_%d", conn->shmid);

  /* Create the Tcl channel */
#if TCL_MAJOR_VERSION == 7 && TCL_MINOR_VERSION == 5
  /* Original signature (only seen in Tcl 7.5) */
  conn_chan = Tcl_CreateChannel(&pds_spi_conn_type, connid->id, NULL, NULL, (ClientData) connid);
#else
  /* Tcl 7.6 and later use this */
  conn_chan = Tcl_CreateChannel(&pds_spi_conn_type, connid->id, (ClientData) connid, TCL_READABLE | TCL_WRITABLE);
#endif

  /* Configure and register the channel */
  Tcl_SetChannelOption(interp, conn_chan, "-buffering", "line");
  Tcl_SetResult(interp, connid->id, TCL_VOLATILE);
  Tcl_RegisterChannel(interp, conn_chan);
}



/******************************************************************************
* Function to retrieve a PDS SPI connection given it's ID                     *
*                                                                             *
* Pre-condition:  The Tcl interpreter, the connection's ID and a pointer to a *
*                 PDS SPI connection ID struct pointer are passed to the      *
*                 function                                                    *
* Post-condition: The PDS SPI connection struct is retrieved from Tcl via     *
*                 it's ID and returned                                        *
******************************************************************************/
pds_spi_conn* pds_spi_GetConnectionId(Tcl_Interp *interp, char *id,
                                      pds_spi_conn_id **pconnid)
{
  Tcl_Channel conn_chan;
  pds_spi_conn_id *connid = NULL;

  /* Validate the channel for this ID - check its type */
  conn_chan = Tcl_GetChannel(interp, id, 0);

  if(conn_chan == NULL || Tcl_GetChannelType(conn_chan) != &pds_spi_conn_type)
  {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, id, " is not a valid PDS SPI connection", 0);
    return (pds_spi_conn *) NULL;
  }

  /* Get the connection ID struct that this channel describes */
  connid = (pds_spi_conn_id *) Tcl_GetChannelInstanceData(conn_chan);

  if(pconnid)
    *pconnid = connid;

  return (pds_spi_conn *) connid->conn;
}



/******************************************************************************
* Function to delete and unregister a Tcl channel for a PDS SPI connection    *
*                                                                             *
* Pre-condition:  Any client data and the Tcl interpreter are passed to the   *
*                 function                                                    *
* Post-condition: The client program is disconnected from the PDS SPI and the *
*                 Tcl channel for this connection is unregistered             *
******************************************************************************/
int pds_spi_DelConnectionId(ClientData cData, Tcl_Interp *interp)
{
  pds_spi_conn_id *connid = NULL;

  connid = (pds_spi_conn_id *) cData;

  if(connid)                           /* Close the PDS SPI connection */ 
    PDS_SPIdisconnect(connid->conn);

  connid->conn = NULL;

  return 0;
}

