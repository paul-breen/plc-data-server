/******************************************************************************
* PROJECT:  PLC data server SPI library (Tcl interface)                       * 
* MODULE:   pds_spi_tcl_id.h                                                  *
* PURPOSE:  Header file for the PLC data server SPI Tcl ID SPI module         *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-03                                                        *
******************************************************************************/

#ifndef __PDS_SPI_TCL_ID_H
#define __PDS_SPI_TCL_ID_H

#include "tcl.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pds_api.h"
#include "pds_ipc.h"
#include "pds_spi.h"

/******************************************************************************
* Defines                                                                     *
******************************************************************************/
#define PKGNAME		"PDS_SPI"
#define PKGVERSION	"1.0"

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The 'PLC data server SPI connection structure' Tcl wrapper structure        *
******************************************************************************/
typedef struct pds_spi_conn_id_rec
{
  char id[33];                    /* The Tcl identifier */
  pds_spi_conn *conn;             /* A PDS SPI connection structure */
} pds_spi_conn_id;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to create and register a new Tcl channel for a PDS SPI connection  *
*                                                                             *
* Pre-condition:  The Tcl interpreter and a valid PDS SPI connection struct   *
*                 are passed to the function                                  *
* Post-condition: The PDS SPI connection struct is registered as a Tcl        *
*                 channel and identified by the connection ID                 *
******************************************************************************/
void pds_spi_SetConnectionId(Tcl_Interp *interp, pds_spi_conn *conn);

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
                                      pds_spi_conn_id **pconnid);

/******************************************************************************
* Function to delete and unregister a Tcl channel for a PDS SPI connection    *
*                                                                             *
* Pre-condition:  Any client data and the Tcl interpreter are passed to the   *
*                 function                                                    *
* Post-condition: The client program is disconnected from the PDS SPI and the *
*                 Tcl channel for this connection is unregistered             *
******************************************************************************/
int pds_spi_DelConnectionId(ClientData cData, Tcl_Interp *interp);

#endif

