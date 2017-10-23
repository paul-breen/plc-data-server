/******************************************************************************
* PROJECT:  PLC data server SPI library (Tcl interface)                       * 
* MODULE:   pds_spi_tcl_spi.c                                                 *
* PURPOSE:  The PLC data server SPI Tcl SPI module                            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-03                                                        *
******************************************************************************/

#include "pds_spi_tcl_spi.h"

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
int pds_spi_tcl_connect(ClientData cData, Tcl_Interp *interp, int argc,
                        char *argv[])
{
  key_t connkey = 0;
  pds_spi_conn *conn = NULL;

  if(argc != 2)
  {
    Tcl_AppendResult(interp, "pds_spiconnect: wrong # of arguments\n", "pds_spiconnect connkey", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the connkey arg */
  if((Tcl_GetInt(interp, argv[1], &connkey) != TCL_OK)) 
  {
    Tcl_AppendResult(interp, "pds_spiconnect: error getting connkey as an int ", (char *) NULL);
    return TCL_ERROR;
  }

  /* Attempt to connect to the PDS SPI using the connection key */
  if(!(conn = (pds_spi_conn *) PDS_SPIconnect(connkey)))
  {
    Tcl_AppendResult(interp, "pds_spiconnect: PDS SPI memory allocation error", (char *) NULL);
    return TCL_ERROR;
  }
  else
  {
    if(PDScheck_conn_status(conn) != PDS_CONN_OK)
    {
      Tcl_AppendResult(interp, "pds_spiconnect: error connecting to PDS SPI: ", PDSprint_conn_status(conn), (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      /* Set the ID for this PDS SPI connection */
      pds_spi_SetConnectionId(interp, conn);
      return TCL_OK;
    }
  }
}



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
int pds_spi_tcl_disconnect(ClientData cData, Tcl_Interp *interp, int argc,
                           char *argv[])
{
  Tcl_Channel conn_chan;

  if(argc != 2)
  {
    Tcl_AppendResult(interp, "pds_spidisconnect: wrong # of arguments\n", "pds_spidisconnect conn", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the Tcl channel for this PDS SPI connection */
  conn_chan = Tcl_GetChannel(interp, argv[1], 0);

  if(conn_chan == NULL)
  {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, argv[1], " is not a valid PDS SPI connection", (char *) NULL);
    return TCL_ERROR;
  }

  /* Unregister the Tcl channel for this PDS SPI connection */
  return Tcl_UnregisterChannel(interp, conn_chan);
}



/******************************************************************************
* Function to get an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The SPI tag's value is returned.  If an error occurs        *
*                 a stack trace is produced and a TCL_ERROR is returned       *
******************************************************************************/
int pds_spi_tcl_get_tag(ClientData cData, Tcl_Interp *interp, int argc,
                        char *argv[])
{
  int tagvalue = 0;
  char tmpstr[16] = "\0";
  pds_spi_conn *conn = NULL;
  pds_spi_conn_id *connid = NULL;

  if(argc != 3)
  {
    Tcl_AppendResult(interp, "pds_spiget_tag: wrong # of arguments\n", "pds_spiget_tag conn tagname", (char *) NULL);
    return TCL_ERROR;
  }

  if((conn = (pds_spi_conn *) pds_spi_GetConnectionId(interp, argv[1], &connid)))
  {
    /* Call the 'C' SPI function to get a tag's value */
    if(PDS_SPIget_tag(conn, argv[2], &tagvalue) == -1)
    {
      Tcl_AppendResult(interp, "pds_spiget_tag: error getting value for ", argv[2], (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      sprintf(tmpstr, "%d", tagvalue);
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, tmpstr, (char *) NULL);
      return TCL_OK;
    }
  }
  else
  {
    Tcl_AppendResult(interp, "pds_spiget_tag: error getting PDS SPI connection", " from ID ", argv[1], (char *) NULL);
    return TCL_ERROR;
  }
}



/******************************************************************************
* Function to set an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The SPI tag value is set.  If an error occurs a             *
*                 stack trace is produced and a TCL_ERROR is returned else a  *
*                 flag is returned indicating whether the value was set       *
******************************************************************************/
int pds_spi_tcl_set_tag(ClientData cData, Tcl_Interp *interp, int argc,
                        char *argv[])
{
  int retval = -1;
  int tagvalue = 0;
  char retstr[5] = "\0";
  pds_spi_conn *conn = NULL;
  pds_spi_conn_id *connid = NULL;

  if(argc != 4)
  {
    Tcl_AppendResult(interp, "pds_spiset_tag: wrong # of arguments\n", "pds_spiset_tag conn tagname tagvalue", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the tagvalue */
  if((Tcl_GetInt(interp, argv[3], &tagvalue) != TCL_OK)) 
  {
    Tcl_AppendResult(interp, "pds_spiset_tag: error getting tagvalue as an int ", (char *) NULL);
    return TCL_ERROR;
  }

  if((conn = (pds_spi_conn *) pds_spi_GetConnectionId(interp, argv[1], &connid)))
  {
    /* Call the 'C' SPI function to set a tag's value */
    if((retval = PDS_SPIset_tag(conn, argv[2], tagvalue)) == -1)
    {
      Tcl_AppendResult(interp, "pds_spiset_tag: error setting value for ", argv[2], (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      sprintf(retstr, "%d", retval);
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, retstr, (char *) NULL);
      return TCL_OK;
    }
  }
  else
  {
    Tcl_AppendResult(interp, "pds_spiset_tag: error getting PDS SPI connection", " from ID ", argv[1], (char *) NULL);
    return TCL_ERROR;
  }
}



/******************************************************************************
* Function to initialise this Tcl extension library                           *
*                                                                             *
* Pre-condition:  The Tcl interpreter is passed to the function               *
* Post-condition: The package and all its commands are registered.  If an     *
*                 error occurs an error code is returned indicating the error *
******************************************************************************/
int Pds_spi_tcl_Init(Tcl_Interp *interp)
{
  int code = 0;

  /* Initialise the Tcl stubs */
  if(Tcl_InitStubs(interp, TCL_VERSION, 1) == NULL)
  {
    return TCL_ERROR;
  }

  /* Register this Tcl extension library's (package) name and version */
  if((code = Tcl_PkgProvide(interp, PKGNAME, PKGVERSION)) != TCL_OK)
  {
    return code;
  }

  /***************** Register the PDS SPI Tcl SPI's commands *****************/

  Tcl_CreateCommand(interp, "pds_spiconnect", (Tcl_CmdProc *) pds_spi_tcl_connect, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pds_spidisconnect", (Tcl_CmdProc *) pds_spi_tcl_disconnect, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pds_spiget_tag", (Tcl_CmdProc *) pds_spi_tcl_get_tag, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pds_spiset_tag", (Tcl_CmdProc *) pds_spi_tcl_set_tag, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

