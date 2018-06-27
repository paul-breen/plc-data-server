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

/* Ensure functions such as PDSget_tag recognise the char pointer as a string,
   and return it from the function */
%cstring_bounded_output(char* tagvalue, 256);

%include "typemaps.i"

/* Map the tagvalues short int array to a Python list */
%typemap(in) (short int ntags, const unsigned short int *tagvalues) {
  int i;

  if(!PyList_Check($input)) {
    PyErr_SetString(PyExc_ValueError, "Expecting a list");
    return NULL;
  }
  $1 = PyList_Size($input);
  $2 = (short int *) malloc(($1) * sizeof(short int));

  for(i = 0; i < $1; i++) {
    PyObject *s = PyList_GetItem($input, i);

    if(!PyInt_Check(s)) {
        free($2);
        PyErr_SetString(PyExc_ValueError, "List items must be integers");
        return NULL;
    }
    $2[i] = PyInt_AsLong(s);
  }
}

%typemap(freearg) (short int ntags, const unsigned short int *tagvalues) {
   if($2) free($2);
}

/* Map the tagvalue in/out argument as short pointer, rather than string */
%apply unsigned short int *INOUT {unsigned short int *tagvalue};
%inline %{
extern int PDSset_wordbit_state(pdsconn *conn, char *tagname, unsigned short int *tagvalue, int bitno, int bitvalue);
%}

%include "pds_defs.h"
%include "pds_api.h" 
%include "pds_ipc.h"
%include "pds_utils.h"
%include "pds_functions.h"
%include "pds_protocols.h"
%include "pds_types.h"

