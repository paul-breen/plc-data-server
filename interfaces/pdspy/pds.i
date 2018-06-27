/******************************************************************************
* PROJECT:  PLC data server library (Python interface)                        * 
* MODULE:   pds.i                                                             *
* PURPOSE:  The PLC data server Python API module                             *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2018-06-23                                                        *
******************************************************************************/

%module pds
%{
#include "pds.h"
%}

typedef int key_t;
typedef long int time_t;

%include <cstring.i>
%cstring_bounded_output(char* tagvalue, 256);

%include "typemaps.i"

%apply unsigned short int *INPUT {const unsigned short int *tagvalues};
%inline %{
extern int PDSset_tag(pdsconn *conn, const char *tagname, short int ntags, const unsigned short int *tagvalues);
%}

%include "pds_defs.h"
%include "pds_api.h" 
%include "pds_ipc.h"
%include "pds_utils.h"
%include "pds_functions.h"
%include "pds_protocols.h"
%include "pds_types.h"

