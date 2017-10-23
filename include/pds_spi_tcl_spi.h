/******************************************************************************
* PROJECT:  PLC data server SPI library (Tcl interface)                       * 
* MODULE:   pds_spi_tcl_spi.h                                                 *
* PURPOSE:  Header file for the PLC data server SPI Tcl SPI module            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-03                                                        *
******************************************************************************/

#ifndef __PDS_SPI_TCL_SPI_H
#define __PDS_SPI_TCL_SPI_H

#include "pds_spi_tcl_id.h"

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to connect to the PDS SPI                                          *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: An attempt is made to connect the client program to the PDS *
*                 SPI and if successful the connection ID is returned.  If an *
*                 error occurs a stack trace is produced and a TCL_ERROR is   *
*                 returned                                                    *
******************************************************************************/
extern int pds_spi_tcl_connect(ClientData cData, Tcl_Interp *interp, int argc,
                               char *argv[]);

/******************************************************************************
* Function to disconnect from the PDS SPI                                     *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: An attempt is made to disconnect the client program from    *
*                 the PDS SPI and if successful the connection is closed.  If *
*                 an error occurs a stack trace is produced and a TCL_ERROR   *
*                 is returned                                                 *
******************************************************************************/
extern int pds_spi_tcl_disconnect(ClientData cData, Tcl_Interp *interp,
                                  int argc, char *argv[]);

/******************************************************************************
* Function to get an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The SPI tag's value is returned.  If an error occurs        *
*                 a stack trace is produced and a TCL_ERROR is returned       *
******************************************************************************/
extern int pds_spi_tcl_get_tag(ClientData cData, Tcl_Interp *interp, int argc,
                               char *argv[]);

/******************************************************************************
* Function to set an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The SPI tag value is set.  If an error occurs a             *
*                 stack trace is produced and a TCL_ERROR is returned else a  *
*                 flag is returned indicating whether the value was set       *
******************************************************************************/
extern int pds_spi_tcl_set_tag(ClientData cData, Tcl_Interp *interp, int argc,
                               char *argv[]);

/******************************************************************************
* Function to initialise this Tcl extension library                           *
*                                                                             *
* Pre-condition:  The Tcl interpreter is passed to the function               *
* Post-condition: The package and all its commands are registered.  If an     *
*                 error occurs an error code is returned indicating the error *
******************************************************************************/
extern int Pds_spi_tcl_Init(Tcl_Interp *interp);

#endif

