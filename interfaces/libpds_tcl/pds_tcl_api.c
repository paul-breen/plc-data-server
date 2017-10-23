/******************************************************************************
* PROJECT:  PLC data server library (Tcl interface)                           * 
* MODULE:   pds_tcl_api.c                                                     *
* PURPOSE:  The PLC data server Tcl API module                                *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-03-27                                                        *
******************************************************************************/

#include "pds_tcl_api.h"

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
int pds_tcl_connect(ClientData cData, Tcl_Interp *interp, int argc,
                    char *argv[])
{
  key_t connkey = 0;
  pdsconn *conn = NULL;

  if(argc != 2)
  {
    Tcl_AppendResult(interp, "pdsconnect: wrong # of arguments\n", "pdsconnect connkey", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the connkey arg */
  if((Tcl_GetInt(interp, argv[1], &connkey) != TCL_OK)) 
  {
    Tcl_AppendResult(interp, "pdsconnect: error getting connkey as an int ", (char *) NULL);
    return TCL_ERROR;
  }

  /* Attempt to connect to the PDS using the connection key */
  if(!(conn = (pdsconn *) PDSconnect(connkey)))
  {
    Tcl_AppendResult(interp, "pdsconnect: PDS memory allocation error", (char *) NULL);
    return TCL_ERROR;
  }
  else
  {
    if(PDScheck_conn_status(conn) != PDS_CONN_OK)
    {
      Tcl_AppendResult(interp, "pdsconnect: error connecting to PDS: ", PDSprint_conn_status(conn), (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      /* Set the ID for this PDS connection */
      pdsSetConnectionId(interp, conn);
      return TCL_OK;
    }
  }
}



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
int pds_tcl_disconnect(ClientData cData, Tcl_Interp *interp, int argc,
                       char *argv[])
{
  Tcl_Channel conn_chan;

  if(argc != 2)
  {
    Tcl_AppendResult(interp, "pdsdisconnect: wrong # of arguments\n", "pdsdisconnect conn", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the Tcl channel for this PDS connection */
  conn_chan = Tcl_GetChannel(interp, argv[1], 0);

  if(conn_chan == NULL)
  {
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, argv[1], " is not a valid PDS connection", (char *) NULL);
    return TCL_ERROR;
  }

  /* Unregister the Tcl channel for this PDS connection */
  return Tcl_UnregisterChannel(interp, conn_chan);
}



/******************************************************************************
* Function to get a tag's value                                               *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tag's value in the PLC is returned.  If an error occurs *
*                 a stack trace is produced and a TCL_ERROR is returned       *
******************************************************************************/
int pds_tcl_get_tag(ClientData cData, Tcl_Interp *interp, int argc,
                    char *argv[])
{
  char tagvalue[PDS_TAGVALUE_LEN] = "\0";
  pdsconn *conn = NULL;
  pdsconn_id *connid = NULL;

  if(argc != 3)
  {
    Tcl_AppendResult(interp, "pdsget_tag: wrong # of arguments\n", "pdsget_tag conn tagname", (char *) NULL);
    return TCL_ERROR;
  }

  if((conn = (pdsconn *) pdsGetConnectionId(interp, argv[1], &connid)))
  {
    /* Call the 'C' API function to get a tag's value */
    if(PDSget_tag(conn, argv[2], tagvalue) == -1)
    {
      Tcl_AppendResult(interp, "pdsget_tag: error getting value for ", argv[2], (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, tagvalue, (char *) NULL);
      return TCL_OK;
    }
  }
  else
  {
    Tcl_AppendResult(interp, "pdsget_tag: error getting PDS connection", " from ID ", argv[1], (char *) NULL);
    return TCL_ERROR;
  }
}



/******************************************************************************
* Function to get a string of tags' value                                     *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tags' value in the PLC are returned as a single item of *
*                 data.  If an error occurs a stack trace is produced and a   *
*                 TCL_ERROR is returned                                       *
******************************************************************************/
int pds_tcl_get_strtag(ClientData cData, Tcl_Interp *interp, int argc,
                       char *argv[])
{
  int ntags = 0;
  char tagvalue[PDS_STR_TAGVALUE_LEN] = "\0";
  char fmt = ' ';
  pdsconn *conn = NULL;
  pdsconn_id *connid = NULL;

  if(argc != 5)
  {
    Tcl_AppendResult(interp, "pdsget_strtag: wrong # of arguments\n", "pdsget_strtag conn tagname ntags format", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the ntags arg */
  if((Tcl_GetInt(interp, argv[3], &ntags) != TCL_OK)) 
  {
    Tcl_AppendResult(interp, "pdsget_strtag: error getting ntags as an int ", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the data format specifier (char) and validate it */
  fmt = argv[4][0];

  if(_pds_tcl_check_tag_formatspec(fmt) == -1)
  {
    Tcl_AppendResult(interp, "pdsget_strtag: bad data format specifier - ", argv[4], (char *) NULL);
    return TCL_ERROR;
  }
 
  if((conn = (pdsconn *) pdsGetConnectionId(interp, argv[1], &connid)))
  {
    /* Call the 'C' API function to get a string of tags' value */
    if(PDSget_strtag(conn, argv[2], ntags, tagvalue, fmt) == -1)
    {
      Tcl_AppendResult(interp, "pdsget_strtag: error getting value for ", argv[2], (char *) NULL);
      return TCL_ERROR;
    }
    else
    {
      Tcl_ResetResult(interp);
      Tcl_AppendResult(interp, tagvalue, (char *) NULL);
      return TCL_OK;
    }
  }
  else
  {
    Tcl_AppendResult(interp, "pdsget_strtag: error getting PDS connection", " from ID ", argv[1], (char *) NULL);
    return TCL_ERROR;
  }
}



/******************************************************************************
* Function to set tag value(s)                                                *
*                                                                             *
* Pre-condition:  The standard Tcl function parameters are passed to the      *
*                 function                                                    *
* Post-condition: The tag value(s) is set in the PLC.  If an error occurs a   *
*                 stack trace is produced and a TCL_ERROR is returned         *
******************************************************************************/
int pds_tcl_set_tag(ClientData cData, Tcl_Interp *interp, int argc,
                    char *argv[])
{
  int ntags = 0, retval = -1;
  register int i = 0;
  unsigned short int tagvalues[PDS_NTAGVALUES];
  char retstr[5] = "\0";
  pdsconn *conn = NULL;
  pdsconn_id *connid = NULL;

  memset(tagvalues, 0, (PDS_NTAGVALUES * sizeof(unsigned short int)));

  if(argc < 4)
  {
    Tcl_AppendResult(interp, "pdsset_tag: wrong # of arguments\n", "pdsset_tag conn tagname tagvalue [tagvalue ...]", (char *) NULL);
    return TCL_ERROR;
  }

  /* Get the variable number of tagvalues */
  for(i = 3, ntags = 0; i < argc && i <= PDS_NTAGVALUES; i++)
  {
    if((Tcl_GetInt(interp, argv[i], (int *) &tagvalues[i - 3]) != TCL_OK)) 
    {
      Tcl_AppendResult(interp, "pdsset_tag: error getting tagvalue as an int ", (char *) NULL);
      return TCL_ERROR;
    }
    ntags++;
  }

  if((conn = (pdsconn *) pdsGetConnectionId(interp, argv[1], &connid)))
  {
    /* Call the 'C' API function to set tag value(s) */
    if((retval = PDSset_tag(conn, argv[2], ntags, tagvalues)) == -1)
    {
      Tcl_AppendResult(interp, "pdsset_tag: error setting value for ", argv[2], (char *) NULL);
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
    Tcl_AppendResult(interp, "pdsset_tag: error getting PDS connection", " from ID ", argv[1], (char *) NULL);
    return TCL_ERROR;
  }
}



/******************************************************************************
* Internal function to validate a tag's data format specifier                 *
*                                                                             *
* Pre-condition:  The tag's data format specifier is passed to the function   *
* Post-condition: If the specifier is invalid a -1 is returned                *
******************************************************************************/
static int _pds_tcl_check_tag_formatspec(const char fmt)
{
  switch(fmt)
  {
    case 'd' :                    /* Specifier is valid */
    case 'i' :
    case 'f' :
    case 'c' :
    case 's' :
      return 0;
    break;

    default :                     /* Specifier is invalid */
      return -1;
    break;
  }
}



/******************************************************************************
* Function to initialise this Tcl extension library                           *
*                                                                             *
* Pre-condition:  The Tcl interpreter is passed to the function               *
* Post-condition: The package and all its commands are registered.  If an     *
*                 error occurs an error code is returned indicating the error *
******************************************************************************/
int Pds_tcl_Init(Tcl_Interp *interp)
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

  /******************* Register the PDS Tcl API's commands *******************/

  Tcl_CreateCommand(interp, "pdsconnect", (Tcl_CmdProc *) pds_tcl_connect, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pdsdisconnect", (Tcl_CmdProc *) pds_tcl_disconnect, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pdsget_tag", (Tcl_CmdProc *) pds_tcl_get_tag, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pdsget_strtag", (Tcl_CmdProc *) pds_tcl_get_strtag, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp, "pdsset_tag", (Tcl_CmdProc *) pds_tcl_set_tag, (ClientData) 0, (Tcl_CmdDeleteProc *) NULL);

  return TCL_OK;
}

