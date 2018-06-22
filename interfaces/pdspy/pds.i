%module pds
%{
#include "pds.h"
%}

typedef int key_t;
typedef long int time_t;

%include "pds_defs.h"
%include "pds_api.h" 
%include "pds_ipc.h"
%include "pds_utils.h"
%include "pds_functions.h"
%include "pds_protocols.h"
%include "pds_types.h"

