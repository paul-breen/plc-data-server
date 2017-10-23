/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_mb.h                                                          *
* PURPOSE:  Header for the ModBus driver module                               *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#ifndef __PDS_MB_H
#define __PDS_MB_H

#include "../pds_srv.h"

/******************************************************************************
* Defines and macros                                                          *
******************************************************************************/

/* ModBus protocol constants */
#define MB_MAXBUFLEN	261       /* Max. length for i/o message buffers */
#define MB_WORDSIZE	2         /* Word size */
#define MB_BITS_BYTE	8         /* Bits per byte */
#define MB_RDDATA_MAX	250       /* Max. data bytes in read query */
#define MB_WRDATA_MAX	200       /* Max. data bytes in write query */
#define MB_RDHDR_SIZE	3         /* Read response header size */
#define MB_WRHDR_SIZE	6         /* Write response header size */
#define MB_ASAPORT	502       /* ModBus ASA standard port */
#define MB_TMO_SECS	5         /* Select timeout interval (secs) */
#define MB_TMO_USECS	100000    /* Select timeout interval (usecs) */
#define MB_SERIAL_PAUSE	500       /* Pause between serial comms (usecs) */

/* Following no. of bytes in buffer for ModBus/TCP/IP header */
#define MB_READ_BYTES_IN_BUF	6
#define MB_WRITE_BYTES_IN_BUF	7
#define MB_STAT_BYTES_IN_BUF	2
#define MB_DIAG_BYTES_IN_BUF	6

/* ModBus protocol byte field positions */
#define MB_START_BYTE		0
#define MB_UNIT_ID		MB_START_BYTE
#define MB_FUNC_CODE		(MB_START_BYTE + 1)
#define MB_EXCEPTION_CODE	(MB_START_BYTE + 2)
#define MB_RESPONSE_DATABYTES	(MB_START_BYTE + 2)
#define MB_HI_DATA		(MB_START_BYTE + 3)
#define MB_LO_DATA		(MB_START_BYTE + 4)
#define MB_HI_BASE_ADDR		(MB_START_BYTE + 2)
#define MB_LO_BASE_ADDR		(MB_START_BYTE + 3)
#define MB_HI_SUBFUNC_CODE	(MB_START_BYTE + 2)
#define MB_LO_SUBFUNC_CODE	(MB_START_BYTE + 3)
#define MB_HI_SUBFUNC_DATA	(MB_START_BYTE + 4)
#define MB_LO_SUBFUNC_DATA	(MB_START_BYTE + 5)
#define MB_HI_NREFS		(MB_START_BYTE + 4)
#define MB_LO_NREFS		(MB_START_BYTE + 5)
#define MB_RUN_INDICATOR	(MB_START_BYTE + 4)
#define MB_QUERY_DATABYTES	(MB_START_BYTE + 6)

/* ModBus protocol flags & bitmasks */
#define MB_EXFLAG		0x80        /* Exception response flag */
#define MB_DRV_STS_BITMASK	0x00ff0000  /* Driver status bitmask */

/* ModBus protocol transport-dependent prefix & postfix lengths */
#define MB_SERIAL_PRELEN	0
#define MB_SERIAL_POSTLEN	2
#define MB_SERIAL_TCPIP_PRELEN	MB_SERIAL_PRELEN
#define MB_SERIAL_TCPIP_POSTLEN	MB_SERIAL_POSTLEN
#define MB_TCPIP_PRELEN		6
#define MB_TCPIP_POSTLEN	0

#define MB_CS_BASE	000000    /* Base address of Coils */
#define MB_IS_BASE	100000    /* Base address of Discrete Inputs */
#define MB_HR_BASE	400000    /* Base address of Holding Registers */
#define MB_IR_BASE	300000    /* Base address of Input Registers */

#define MB_SID_MICRO84	0         /* Slave IDs for different ModBus PLCs */
#define MB_SID_484	1         /* (used in device-specific diagnostics) */
#define MB_SID_184_384	2
#define MB_SID_584	3
#define MB_SID_884	8
#define MB_SID_984	9

/* ModBus hex function code mapping */
#define MB_UNKNOWN_FUNCTION	0x00   /* Invalid function */
#define MB_CS_READ		0x01   /* Read Coil Status */
#define MB_IS_READ		0x02   /* Read Input Status */
#define MB_HR_READ		0x03   /* Read Holding Registers */
#define MB_IR_READ		0x04   /* Read Input Registers */

#define MB_SC_WRITE		0x05   /* Force Single Coil */
#define MB_SR_WRITE		0x06   /* Preset Single Register */
#define MB_ES_STAT		0x07   /* Read Exception Status */
#define MB_PLC_DIAG		0x08   /* PLC diagnostics (see subfunctions) */

#define MB_CEC_STAT		0x0b   /* Fetch Comm Event Counter Status */
#define MB_CEL_STAT		0x0c   /* Fetch Comm Event Log Status */

#define MB_MC_WRITE		0x0f   /* Force Multiple Coils */
#define MB_MR_WRITE		0x10   /* Preset Multiple Registers */

#define MB_SID_STAT		0x11   /* Report Slave ID & PLC Status */

#define MB_GR_READ		0x14   /* Read General Reference */
#define MB_GR_WRITE		0x15   /* Write General Reference */
#define MB_MH_WRITE		0x16   /* Mask Write Holding Registers */
#define MB_HR_RDWR		0x17   /* Read/Write Holding Registers */
#define MB_FQ_READ		0x18   /* Read FIFO Queue */

/* ModBus hex subfunction (function 0x08) code mapping */
#define MB_QD_READ		0x00   /* Return Query Data */
#define MB_RS_COMMS		0x01   /* Restart Communications Option */
#define MB_DR_READ		0x02   /* Return Diagnostic Register */
#define MB_ID_WRITE		0x03   /* Change ASCII Input Delimiter */
#define MB_LO_MODE		0x04   /* Force Listen Only Mode */

#define MB_CD_CLEAR		0x0a   /* Clear Counters/Diagnostic Register */
#define MB_BMC_READ		0x0b   /* Return Bus Message Count */
#define MB_BCEC_READ		0x0c   /* Return Bus Comms Error Count */
#define MB_BEEC_READ		0x0d   /* Return Bus Execption Error Count */
#define MB_SMC_READ		0x0e   /* Return Slave Message Count */
#define MB_SNRC_READ		0x0f   /* Return Slave No Response Count */
#define MB_SNAK_READ		0x10   /* Return Slave NAK Count */
#define MB_SBC_READ		0x11   /* Return Slave Busy Count */
#define MB_BCOC_READ		0x12   /* Return Bus Character Overrun Count */
#define MB_IOPOC_READ		0x13   /* Return IOP Overrun Count (884) */
#define MB_OCF_CLEAR		0x14   /* Clear Overrun Counter/Flag (884) */
#define MB_MBP_STATS		0x15   /* Get/Clear ModBus+ Statistics */

/* ModBus protocol exception codes */
#define MB_ILLEGAL_FUNCTION		0x01
#define MB_ILLEGAL_DATA_ADDRESS		0x02
#define MB_ILLEGAL_DATA_VALUE		0x03
#define MB_ILLEGAL_RESPONSE_LENGTH	0x04
#define MB_ACKNOWLEDGE			0x05
#define MB_SLAVE_DEVICE_BUSY		0x06
#define MB_NEGATIVE_ACKNOWLEDGE		0x07
#define MB_MEMORY_PARITY_ERROR		0x08
#define MB_MBP_GW_PATH_UNAVAILABLE	0x0a
#define MB_MBP_GW_DEVICE_NO_RESP	0x0b

/* ModBus driver exception codes */
#define MB_INCORRECT_RESPONSE_SIZE	(0x01 | MB_DRV_STS_BITMASK)

/* Get the MB function from a configuration file function code */
#define MB_GET_FUNC(f)\
((f) == PDS_BREAD ? MB_CS_READ : (f) == PDS_BWRITE ? MB_MC_WRITE :\
 (f) == PDS_WREAD ? MB_HR_READ : (f) == PDS_WWRITE ? MB_MR_WRITE : -1)

/* Map a write function to its equivalent read function */
#define MB_WRRD_MAP(f)	((f) == MB_MC_WRITE ? MB_CS_READ :\
                         (f) == MB_MR_WRITE ? MB_HR_READ : (f))

/* Map a read function to its equivalent write function */
#define MB_RDWR_MAP(f)	((f) == MB_CS_READ ? MB_MC_WRITE :\
                         (f) == MB_HR_READ ? MB_MR_WRITE : (f))

/* Calculate how many bytes are in query based on function and no. of refs */
#define MB_DATABYTES(f, r)\
(((f) == MB_CS_READ || (f) == MB_IS_READ || (f) == MB_MC_WRITE) ?\
(((r) + (MB_BITS_BYTE - 1)) / MB_BITS_BYTE) :\
((f) == MB_HR_READ || (f) == MB_IR_READ || (f) == MB_MR_WRITE) ?\
((r) * MB_WORDSIZE) : 0)

/* Determine the type of this hex function code {read|write|rdwr|stat|diag} */
#define MB_FUNCTYPE(f)\
(((f) == MB_CS_READ || (f) == MB_IS_READ || (f) == MB_HR_READ ||\
(f) == MB_IR_READ || (f) == MB_GR_READ || (f) == MB_FQ_READ) ? PDS_RD_FUNC :\
((f) == MB_SC_WRITE || (f) == MB_SR_WRITE || (f) == MB_MC_WRITE ||\
(f) == MB_MR_WRITE || (f) == MB_GR_WRITE || (f) == MB_MH_WRITE) ?\
PDS_WR_FUNC :\
((f) == MB_HR_RDWR) ? PDS_RDWR_FUNC :\
((f) == MB_ES_STAT || (f) == MB_CEC_STAT || (f) == MB_CEL_STAT ||\
(f) == MB_SID_STAT) ? PDS_STAT_FUNC :\
((f) == MB_PLC_DIAG) ? PDS_DIAG_FUNC : 0)

/* Determine the lo ref, hi ref of given ModBus refs */
#define MB_LO_REF(n1, n2)	((n2) - (n1)) 
#define MB_HI_REF(n1, n2)	((n2) - (n1) + 1)

/* Determine the base address given a ModBus function code */
#define MB_BASE_ADDR(f)\
((f) == MB_CS_READ ? MB_CS_BASE : (f) == MB_IS_READ ? MB_IS_BASE :\
 (f) == MB_HR_READ ? MB_HR_BASE : (f) == MB_IR_READ ? MB_IR_BASE : -1)

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to setup a read PLC query using the configuration file parameters  *
*                                                                             *
* Pre-condition:  Storage for this block's query and the block structure are  *
*                 passed to the function                                      *
* Post-condition: This block's configuration parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int mb_setup_read_query(unsigned char *query, plc_cnf_block *block);

/******************************************************************************
* Function to setup a write PLC query using the tag & message parameters      *
*                                                                             *
* Pre-condition:  Storage for the query, the tag struct containing the PLC    *
*                 address to write to and the message struct containing the   *
*                 data to be written are passed to the function               *
* Post-condition: The tag & message struct's parameters are assigned to the   *
*                 correct array element in the PLC query.  The length of the  *
*                 query is returned or -1 if an error occurs                  *
******************************************************************************/
int mb_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg);

/******************************************************************************
* Function to setup a status PLC query using the connection parameters        *
*                                                                             *
* Pre-condition:  Storage for the query & the connection struct containing    *
*                 the PLC connection parameters are passed to the function    *
* Post-condition: The connection struct's parameters are used to construct a  *
*                 generic PLC status query.  The length of the query is       *
*                 returned or -1 if an error occurs                           *
******************************************************************************/
int mb_setup_status_query(unsigned char *query, pdsconn *conn);

/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 function code, the base reference and the number of         *
*                 references to read are passed to the function               *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_read_plc_query(unsigned char *query, unsigned short int unit,
                                unsigned short int function,
                                unsigned short int ref,
                                unsigned short int nrefs);

/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 function code, the base reference, number of references and *
*                 a pointer to the value(s) are passed to the function        *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_write_plc_query(unsigned char *query, unsigned short int unit,
                                 unsigned short int function,
                                 unsigned short int ref,
                                 unsigned short int nrefs,
                                 unsigned short int *values);

/******************************************************************************
* Function to construct a PLC status PLC query                                *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address) and the hex. *
*                 function code {0x07|0x0B|0x0C|0x11} are passed to the       *
*                 function                                                    *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_status_plc_query(unsigned char *query,
                                  unsigned short int unit,
                                  unsigned short int function);

/******************************************************************************
* Function to construct a PLC diagnostics PLC query                           *
*                                                                             *
* Pre-condition:  Query buffer, the unit ID (slave node address), the hex.    *
*                 subfunction code and the data (or zero if not applicable)   *
*                 are passed to the function                                  *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int mb_construct_diagnostics_plc_query(unsigned char *query,
                                       unsigned short int unit,
                                       unsigned short int subfunc,
                                       unsigned short int data);

/******************************************************************************
* Function to query the PLC                                                   *
*                                                                             *
* Pre-condition:  Open fd and a transaction struct containing the query &     *
*                 storage for the response are passed to the function         *
* Post-condition: Query is run against the PLC, response buffer holds the     *
*                 result of the query.  Number of bytes successfully read is  *
*                 returned or -1 on error                                     *
******************************************************************************/
int mb_run_plc_query(int fd, pdstrans *trans);

/******************************************************************************
* Function to instantiate a prepared PLC query                                *
*                                                                             *
* Pre-condition:  A transaction struct containing a prepared query is passed  *
*                 to the function                                             *
* Post-condition: The runtime variables in the query (trans. ID, checksum)    *
*                 are instantiated.  The trans. ID is returned or -1 on error *
******************************************************************************/
short int mb_instantiate_prepared_query(pdstrans *trans);

/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  A transaction struct containing a valid response is passed  *
*                 to the function                                             *
* Post-condition: The response is cleaned (transport dependent prefix         *
*                 removed).  The response length is modified accordingly and  *
*                 returned or -1 on error                                     *
******************************************************************************/
short int mb_clean_plc_response(pdstrans *trans);

/******************************************************************************
* Function to check the validity of a PLC query response                      *
*                                                                             *
* Pre-condition:  A transaction struct containing the response is passed to   *
*                 the function                                                *
* Post-condition: The response is checked.  If successful, a zero is returned *
*                 else a code is returned indicating the exception            *
******************************************************************************/
int mb_check_plc_response(pdstrans *trans);

/******************************************************************************
* Function to print a PLC exception                                           *
*                                                                             *
* Pre-condition:  An exception code is passed to the function                 *
* Post-condition: The exception string for the given code is returned         *
******************************************************************************/
char* mb_print_plc_exception(int excode);

/******************************************************************************
* Function to refresh the shared memory status tags with data from a PLC      *
*                                                                             *
* Pre-condition:  The connection struct & a transaction struct containing a   *
*                 response and pointer's to this query's status word and      *
*                 error count are passed to the function                      *
* Post-condition: The data in the PLC's response is used to update the status *
*                 tags in the shared memory segment.  A count of updated tags *
*                 is returned                                                 *
******************************************************************************/
int mb_refresh_status_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to refresh the shared memory data tags with data from a PLC        *
*                                                                             *
* Pre-condition:  The connection struct and a transaction struct containing   *
*                 the query and a valid response are passed to the function   *
* Post-condition: The data in the PLC's response is used to update the data   *
*                 tags in the shared memory segment.  The no. of values in    *
*                 the response is returned or -1 on error                     *
******************************************************************************/
int mb_refresh_data_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to setup a TTY serial port structure for ModBus serial comms       *
*                                                                             *
* Pre-condition:  A TTY serial port structure for storage is passed to the    *
*                 function                                                    *
* Post-condition: The structure elements are configured for MB serial comms   *
******************************************************************************/
int mb_init_tty_struct(struct termios *tio);

#endif

