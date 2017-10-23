/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_functions.h                                                   *
* PURPOSE:  Header file for the PDS functions                                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-09                                                        *
******************************************************************************/

#ifndef __PDS_FUNCTIONS_H
#define __PDS_FUNCTIONS_H 

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* Defines for comms function code mapping */
#define PDS_UNKNOWN_FUNCTION	0      /* Invalid function */
#define PDS_BREAD		1      /* Bit read */
#define PDS_BWRITE		2      /* Bit write */
#define PDS_BRDWR		3      /* Bit read/write */
#define PDS_CREAD		4      /* Char (byte) read */
#define PDS_CWRITE		5      /* Char (byte) write */
#define PDS_CRDWR		6      /* Char (byte) read/write */
#define PDS_WREAD		7      /* Word read */
#define PDS_WWRITE		8      /* Word write */
#define PDS_WRDWR		9      /* Word read/write */
#define PDS_LREAD		10     /* Long word read */
#define PDS_LWRITE		11     /* Long word write */
#define PDS_LRDWR		12     /* Long word read/write */
#define PDS_FREAD		13     /* Float read */
#define PDS_FWRITE		14     /* Float write */
#define PDS_FRDWR		15     /* Float read/write */
#define PDS_DREAD		16     /* Double float read */
#define PDS_DWRITE		17     /* Double float write */
#define PDS_DRDWR		18     /* Double float read/write */
#define PDS_STAT		19     /* Status */
#define PDS_DIAG		20     /* Diagnostics */

/* Defines for comms function code operation types */
#define PDS_UNKNOWN_FUNC_TYPE	0
#define PDS_RD_FUNC		1
#define PDS_WR_FUNC		2
#define PDS_RDWR_FUNC		3
#define PDS_STAT_FUNC		4
#define PDS_DIAG_FUNC		5

/* Get the type of this function code */
#define PDS_GET_FUNCTYPE(f)\
((f) == PDS_BREAD || (f) == PDS_CREAD || (f) == PDS_WREAD ||\
 (f) == PDS_LREAD || (f) == PDS_FREAD || (f) == PDS_DREAD ? PDS_RD_FUNC :\
 (f) == PDS_BWRITE || (f) == PDS_CWRITE || (f) == PDS_WWRITE ||\
 (f) == PDS_LWRITE || (f) == PDS_FWRITE || (f) == PDS_DWRITE ? PDS_WR_FUNC :\
 (f) == PDS_BRDWR || (f) == PDS_CRDWR || (f) == PDS_WRDWR ||\
 (f) == PDS_LRDWR || (f) == PDS_FRDWR || (f) == PDS_DRDWR ? PDS_RDWR_FUNC :\
 (f) == PDS_STAT ? PDS_STAT_FUNC :\
 (f) == PDS_DIAG ? PDS_DIAG_FUNC : 0)

/* Print the operation type of this function code */
#define PDS_PRINT_FUNCTYPE(t)\
((t) == PDS_RD_FUNC ? "Read" : (t) == PDS_WR_FUNC ? "Write" :\
 (t) == PDS_RDWR_FUNC ? "ReadWrite" : (t) == PDS_STAT_FUNC ? "Status" :\
 (t) == PDS_DIAG_FUNC ? "Diagnostics" : "Unknown!")

#endif

