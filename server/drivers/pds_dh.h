/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_dh.h                                                          *
* PURPOSE:  Header for the DataHighway driver module                          *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-02                                                        *
******************************************************************************/

#ifndef __PDS_DH_H
#define __PDS_DH_H

#include "../pds_srv.h"

/******************************************************************************
* Defines and macros                                                          *
******************************************************************************/

/* DataHighway protocol constants */
#define DH_MAXBUFLEN		261    /* Max. length for i/o msg buffers */
#define DH_WORDSIZE		2      /* Word size */
#define DH_BITS_BYTE		8      /* Bits per byte */
#define DH_RDDATA_MAX		244    /* Max. data bytes in read query */
#define DH_WRDATA_MAX		240    /* Max. data bytes in write query */
#define DH_ACK_LEN		2      /* ACK sequence length */
#define DH_PLC_ADDR_LEN		51     /* PLC address length */
#define DH_PLC_STAT_LEN		51     /* PLC status query data length */
#define DH_TMO_SECS		5      /* Select timeout interval (secs) */
#define DH_TMO_USECS		100000 /* Select timeout interval (usecs) */
#define DH_ACK_TIMEOUT		3      /* Receive ACK timeout (secs) */
#define DH_RESP_TIMEOUT		5      /* Receive response timeout (secs) */
#define DH_PLC_ADDR_START	'$'    /* 1st char of PLC address */
#define DH_PLC_ADDR_SEP		':'    /* Separator char of PLC address */
#define DH_PLC_STAT_TEXT	"TID: "
#define DH_PLC_STAT_FORMAT	"%s%05u"

/* DataHighway protocol byte field positions */
#define DH_DF1_START_BYTE	0
#define DH_OB_DST		DH_DF1_START_BYTE
#define DH_OB_SRC		(DH_OB_DST + 1)
#define DH_IB_SRC		DH_DF1_START_BYTE
#define DH_IB_DST		(DH_IB_SRC + 1)
#define DH_CMD_BYTE		(DH_DF1_START_BYTE + 2)
#define DH_STS_BYTE		(DH_DF1_START_BYTE + 3)
#define DH_LO_TNS		(DH_DF1_START_BYTE + 4)
#define DH_HI_TNS		(DH_DF1_START_BYTE + 5)
#define DH_FNC_BYTE		(DH_DF1_START_BYTE + 6)
#define DH_CMD_DATA		(DH_DF1_START_BYTE + 6)
#define DH_EXT_STS_BYTE		(DH_DF1_START_BYTE + 6)
#define DH_LO_DATA		(DH_DF1_START_BYTE + 6)
#define DH_HI_DATA		(DH_DF1_START_BYTE + 7)
#define DH_LO_OFFSET		(DH_DF1_START_BYTE + 7)
#define DH_HI_OFFSET		(DH_DF1_START_BYTE + 8)
#define DH_LO_TTRANS		(DH_DF1_START_BYTE + 9)
#define DH_HI_TTRANS		(DH_DF1_START_BYTE + 10)
#define DH_PLC_ADDR_START_BYTE	(DH_DF1_START_BYTE + 12)

/* DataHighway protocol flags & bitmasks */
#define DH_CMD_REPLY_FLAG	0x40
#define DH_STS_SUCCESS		0x00
#define DH_STS_LOCAL_BITMASK	0x0f
#define DH_STS_REMOTE_BITMASK	0xe0
#define DH_EXT_STS_FLAG		0xf0
#define DH_EXT_STS_BITMASK	0x0000ff00
#define DH_DRV_STS_BITMASK	0x00ff0000

/* DataHighway protocol transport-dependent prefix & postfix lengths */
#define DH_SERIAL_PRELEN	2
#define DH_SERIAL_POSTLEN	3
#define DH_SERIAL_TCPIP_PRELEN	DH_SERIAL_PRELEN
#define DH_SERIAL_TCPIP_POSTLEN	DH_SERIAL_POSTLEN

/* DataHighway hex command/function code mapping.
   Command in the hi byte, function in the lo byte */
#define DH_UNKNOWN_FUNCTION	0x0000 /* Invalid command/function */
#define DH_BIT_READ		0x0000 /* Read Bit */
#define DH_PB_WRITE		0x0200 /* Protected Bit Write */
#define DH_WR_READ		0x0f01 /* Word Range Read */
#define DH_WR_WRITE		0x0f00 /* Word Range Write */
#define DH_ECHO_STAT		0x0600 /* Echo Status */

/* DataHighway local exception codes */
#define DH_DST_NO_BUFFER_SPACE		0x01
#define DH_DST_NO_ACK			0x02
#define DH_DUP_TOKEN_HOLDER		0x03
#define DH_LOCAL_PORT_DISCONNECTED	0x04
#define DH_RESPONSE_TIMEOUT		0x05
#define DH_DUP_NODE			0x06
#define DH_PLC_OFFLINE			0x07
#define DH_HWARE_FAULT			0x08

/* DataHighway remote exception codes */
#define DH_ILLEGAL_COMMAND		0x10
#define DH_REMOTE_PROBLEM		0x20
#define DH_REMOTE_NO_COMMS		0x30
#define DH_REMOTE_HWARE_FAULT		0x40
#define DH_ADDRESS_PROBLEM		0x50
#define DH_FUNC_NOT_ALLOWED		0x60
#define DH_PLC_IN_PROG_MODE		0x70
#define DH_COMPAT_FILE_MISSING		0x80
#define DH_REMOTE_NO_BUFFER_SPACE	0x90
#define DH_REMOTE_WAIT_ACK1		0xa0
#define DH_REMOTE_PROBLEM_DWNLD		0xb0
#define DH_REMOTE_WAIT_ACK2		0xc0

/* DataHighway extended exception codes */
#define DH_ILLEGAL_FIELD_VALUE		(0x01 | DH_EXT_STS_BITMASK)
#define DH_MIN_ADDRESS_FAULT		(0x02 | DH_EXT_STS_BITMASK)
#define DH_MAX_ADDRESS_FAULT		(0x03 | DH_EXT_STS_BITMASK)
#define DH_SYMBOL_NOT_FOUND		(0x04 | DH_EXT_STS_BITMASK)
#define DH_SYMBOL_IMPROPER_FORMAT	(0x05 | DH_EXT_STS_BITMASK)
#define DH_ADDRESS_NOT_USEABLE		(0x06 | DH_EXT_STS_BITMASK)
#define DH_FILE_WRONG_SIZE		(0x07 | DH_EXT_STS_BITMASK)
#define DH_SITUATION_CHANGED		(0x08 | DH_EXT_STS_BITMASK)
#define DH_DATA_TOO_LARGE		(0x09 | DH_EXT_STS_BITMASK)
#define DH_TRANS_ADDRESS_TOO_LARGE	(0xa0 | DH_EXT_STS_BITMASK)
#define DH_ACCESS_DENIED		(0xb0 | DH_EXT_STS_BITMASK)
#define DH_RESOURCE_NOT_AVAIL		(0xc0 | DH_EXT_STS_BITMASK)
#define DH_RESOURCE_ALREADY_AVAIL	(0xd0 | DH_EXT_STS_BITMASK)
#define DH_COMMAND_CANNOT_EXEC		(0xe0 | DH_EXT_STS_BITMASK)
#define DH_HISTOGRAM_OVERFLOW		(0xf0 | DH_EXT_STS_BITMASK)
#define DH_NO_ACCESS			(0x10 | DH_EXT_STS_BITMASK)
#define DH_ILLEGAL_DATA_TYPE		(0x11 | DH_EXT_STS_BITMASK)
#define DH_INVALID_PARAMETER		(0x12 | DH_EXT_STS_BITMASK)
#define DH_ADDRESS_TO_DELETED_AREA	(0x13 | DH_EXT_STS_BITMASK)
#define DH_CMD_EXEC_FAIL_UNKNOWN	(0x14 | DH_EXT_STS_BITMASK)
#define DH_DATA_CONVERSION_ERR		(0x15 | DH_EXT_STS_BITMASK)
#define DH_SCANNER_1771_COMMS_ERR	(0x16 | DH_EXT_STS_BITMASK)
#define DH_TYPE_MISMATCH		(0x17 | DH_EXT_STS_BITMASK)
#define DH_RESPONSE_1771_INVALID	(0x18 | DH_EXT_STS_BITMASK)
#define DH_DUP_LABEL			(0x19 | DH_EXT_STS_BITMASK)
#define DH_REMOTE_RACK_FAULT		(0x22 | DH_EXT_STS_BITMASK)
#define DH_REMOTE_RACK_TIMEOUT		(0x23 | DH_EXT_STS_BITMASK)
#define DH_REMOTE_RACK_UNKNOWN_ERR	(0x24 | DH_EXT_STS_BITMASK)
#define DH_FILE_OWNED_OTHER_NODE	(0x1a | DH_EXT_STS_BITMASK)
#define DH_PROG_OWNED_OTHER_NODE	(0x1b | DH_EXT_STS_BITMASK)
#define DH_DATA_TBL_ELEM_PROTECT	(0x1e | DH_EXT_STS_BITMASK)
#define DH_TMP_INTERNAL_PROBLEM		(0x1f | DH_EXT_STS_BITMASK)

/* DataHighway driver exception codes */
#define DH_TRANS_ID_MISMATCH		(0x01 | DH_DRV_STS_BITMASK)

/* Get the DH command & function from a configuration file function code */
#define DH_GET_CMD_FUNC(f)\
((f) == PDS_BREAD ? DH_BIT_READ :\
 (f) == PDS_BWRITE ? DH_PB_WRITE :\
 (f) == PDS_WREAD ? DH_WR_READ :\
 (f) == PDS_WWRITE ? DH_WR_WRITE : -1)

/* Get the DH command from a configuration file function code */
#define DH_GET_CMD(f)\
((f) == PDS_BREAD ? PDS_GETHIBYTE(DH_BIT_READ) :\
 (f) == PDS_BWRITE ? PDS_GETHIBYTE(DH_PB_WRITE) :\
 (f) == PDS_WREAD ? PDS_GETHIBYTE(DH_WR_READ) :\
 (f) == PDS_WWRITE ? PDS_GETHIBYTE(DH_WR_WRITE) : -1)

/* Get the DH function from a configuration file function code */
#define DH_GET_FUNC(f)\
((f) == PDS_BREAD ? PDS_GETLOBYTE(DH_BIT_READ) :\
 (f) == PDS_BWRITE ? PDS_GETLOBYTE(DH_PB_WRITE) :\
 (f) == PDS_WREAD ? PDS_GETLOBYTE(DH_WR_READ) :\
 (f) == PDS_WWRITE ? PDS_GETLOBYTE(DH_WR_WRITE) : -1)

/* Map a write function to its equivalent read function */
#define DH_WRRD_MAP(f)	((f) == DH_PB_WRITE ? DH_BIT_READ :\
                         (f) == DH_WR_WRITE ? DH_WR_READ : (f))

/* Map a read function to its equivalent write function */
#define DH_RDWR_MAP(f)	((f) == DH_BIT_READ ? DH_PB_WRITE :\
                         (f) == DH_WR_READ ? DH_WR_WRITE : (f))

/* Determine the hi ref of given DataHighway refs */
#define DH_HI_REF(n1, n2)	((n2) - (n1) + 1)

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
int dh_setup_read_query(unsigned char *query, plc_cnf_block *block);

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
int dh_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg);

/******************************************************************************
* Function to setup a status PLC query using the connection parameters        *
*                                                                             *
* Pre-condition:  Storage for the query & the connection struct containing    *
*                 the PLC connection parameters are passed to the function    *
* Post-condition: The connection struct's parameters are used to construct a  *
*                 generic PLC status query.  The length of the query is       *
*                 returned or -1 if an error occurs                           *
******************************************************************************/
int dh_setup_status_query(unsigned char *query, pdsconn *conn);

/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, offset, no. of words   *
*                 to read, PLC address & length and the size in bytes of data *
*                 to read are passed to the function                          *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_read_plc_query(unsigned char *query, unsigned char stn,
                                unsigned char cmd, unsigned short int tns,
                                unsigned char fnc, unsigned short int offset,
                                unsigned short int ttrans, unsigned char *addr,
                                unsigned short int addrlen,
                                unsigned char size);

/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, offset, no. of words   *
*                 to write, PLC address & length, a pointer to the value(s)   *
*                 containing the data to write and the no. of words to write  *
*                 are passed to the function                                  *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_write_plc_query(unsigned char *query, unsigned char stn,
                                 unsigned char cmd, unsigned short int tns,
                                 unsigned char fnc, unsigned short int offset,
                                 unsigned short int ttrans,
                                 unsigned char *addr,
                                 unsigned short int addrlen,
                                 unsigned short int *values,
                                 unsigned short int nvalues);

/******************************************************************************
* Function to construct a status PLC query                                    *
*                                                                             *
* Pre-condition:  Query buffer, the station (slave node address), hex command *
*                 code, transaction ID, function code, data & data length are *
*                 passed to the function                                      *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int dh_construct_status_plc_query(unsigned char *query, unsigned char stn,
                                  unsigned char cmd, unsigned short int tns,
                                  unsigned char fnc, unsigned char *data,
                                  unsigned short int datalen);

/******************************************************************************
* Function to query the PLC                                                   *
*                                                                             *
* Pre-condition:  Open fd and a transaction struct containing the query &     *
*                 storage for the response are passed to the function         *
* Post-condition: Query is run against the PLC, response buffer holds the     *
*                 result of the query.  Number of bytes successfully read is  *
*                 returned or -1 on error                                     *
******************************************************************************/
int dh_run_plc_query(int fd, pdstrans *trans);

/******************************************************************************
* Function to instantiate a prepared PLC query                                *
*                                                                             *
* Pre-condition:  A transaction struct containing a prepared query is passed  *
*                 to the function                                             *
* Post-condition: The runtime variables in the query (trans. ID, checksum)    *
*                 are instantiated.  The trans. ID is returned or -1 on error *
******************************************************************************/
short int dh_instantiate_prepared_query(pdstrans *trans);

/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  A transaction struct containing a valid response is passed  *
*                 to the function                                             *
* Post-condition: The response is cleaned (any double-stuffed bytes removed). *
*                 The response length is modified accordingly and returned or *
*                 -1 on error                                                 *
******************************************************************************/
short int dh_clean_plc_response(pdstrans *trans);

/******************************************************************************
* Function to check the validity of a PLC query response                      *
*                                                                             *
* Pre-condition:  A transaction struct containing the response is passed to   *
*                 the function                                                *
* Post-condition: The response is checked.  If successful, a zero is returned *
*                 else a code is returned indicating the exception            *
******************************************************************************/
int dh_check_plc_response(pdstrans *trans);

/******************************************************************************
* Function to print a PLC exception                                           *
*                                                                             *
* Pre-condition:  An exception code is passed to the function                 *
* Post-condition: The exception string for the given code is returned         *
******************************************************************************/
char* dh_print_plc_exception(int excode);

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
int dh_refresh_status_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to refresh the shared memory data tags with data from a PLC        *
*                                                                             *
* Pre-condition:  The connection struct and a transaction struct containing   *
*                 the query and a valid response are passed to the function   *
* Post-condition: The data in the PLC's response is used to update the data   *
*                 tags in the shared memory segment.  The no. of values in    *
*                 the response is returned or -1 on error                     *
******************************************************************************/
int dh_refresh_data_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to setup a TTY serial port structure for DataHighway serial comms  *
*                                                                             *
* Pre-condition:  A TTY serial port structure for storage is passed to the    *
*                 function                                                    *
* Post-condition: The structure elements are configured for DH serial comms   *
******************************************************************************/
int dh_init_tty_struct(struct termios *tio);

/******************************************************************************
* Function to handle a timeout                                                *
*                                                                             *
* Pre-condition:  Signal is recieved                                          *
* Post-condition: Timeout flag is set to 1.  Handler is re-installed          *
******************************************************************************/
void dh_timeout(int sig);

/******************************************************************************
* Function to register signals to be handled                                  *
*                                                                             *
* Pre-condition:  Signals are handled in the default manner                   *
* Post-condition: Registered signals have specific handler functions          *
******************************************************************************/
void dh_install_signal_handler(void);

#endif

