/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_types.h                                                       *
* PURPOSE:  Header file for the PDS types                                     *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-01-09                                                        *
******************************************************************************/

#ifndef __PDS_TYPES_H
#define __PDS_TYPES_H 

#include <pds_functions.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* Defines for data type code mapping */
#define PDS_UNKNOWN_TYPE	0      /* Invalid type */
#define PDS_BIT			1      /* Bit */
#define PDS_INT8		2      /* Byte */
#define PDS_INT16		3      /* 16 bit word */
#define PDS_INT32		4      /* 32 bit word */
#define PDS_FLOAT32		5      /* 32 bit float */
#define PDS_FLOAT64		6      /* 64 bit float */

/* Print the data type */
#define PDS_PRINT_DATATYPE(t)\
((t) == PDS_BIT ? "Bit" : (t) == PDS_INT8 ? "Byte" :\
 (t) == PDS_INT16 ? "16 Bit Integer" : (t) == PDS_INT32 ? "32 Bit Integer" :\
 (t) == PDS_FLOAT32 ? "32 Bit Float" : (t) == PDS_FLOAT64 ? "64 Bit Float" :\
 "Unknown!")

/* Get the data type that maps to this function code */
#define PDS_FUNC_TYPE_MAP(f)\
((f) == PDS_BREAD || (f) == PDS_BWRITE || (f) == PDS_BRDWR ? PDS_BIT :\
 (f) == PDS_CREAD || (f) == PDS_CWRITE || (f) == PDS_CRDWR ? PDS_INT8 :\
 (f) == PDS_WREAD || (f) == PDS_WWRITE || (f) == PDS_WRDWR ? PDS_INT16 :\
 (f) == PDS_LREAD || (f) == PDS_LWRITE || (f) == PDS_LRDWR ? PDS_INT32 :\
 (f) == PDS_FREAD || (f) == PDS_FWRITE || (f) == PDS_FRDWR ? PDS_FLOAT32 :\
 (f) == PDS_DREAD || (f) == PDS_DWRITE || (f) == PDS_DRDWR ? PDS_FLOAT64 : 0)

#endif

