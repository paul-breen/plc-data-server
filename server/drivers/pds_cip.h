/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_cip.h                                                         *
* PURPOSE:  Header for the CIP driver module                                  *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2003-01-08                                                        *
******************************************************************************/

#ifndef __PDS_CIP_H
#define __PDS_CIP_H

#include "../pds_srv.h"

/******************************************************************************
* Defines and macros                                                          *
******************************************************************************/

/* CIP protocol constants */
#define CIP_MAXBUFLEN		504    /* Max. length for i/o msg buffers */
#define CIP_IOI_SEGLEN		50     /* Max. IOI segment length */
#define CIP_PORT		44818  /* CIP standard TCP port */
#define CIP_BITS_BYTE		8      /* Bits per byte */
#define CIP_TMO_SECS		2      /* Select timeout interval (secs) */
#define CIP_TMO_USECS		100000 /* Select timeout interval (usecs) */
#define CIP_SID_LEN		4      /* Session ID byte length */
#define CIP_DATA_RESP_PREFIX	6      /* No. of bytes in response prefix */
#define CIP_CONN_TICK_TIME	0x07   /* Conn. tick duration */
#define CIP_CONN_TICK_TIMEOUT	0xf9   /* Conn. timeout (ticks * tick time) */

/* CIP protocol byte field positions */
#define CIP_START_BYTE			0
#define CIP_SID_BYTE			(CIP_START_BYTE + 4)
#define CIP_UNCONNECTED_DATA_BYTE	(CIP_START_BYTE + 36)
#define CIP_DATA_LEN			(CIP_START_BYTE + 38)
#define CIP_RESP_SERVICE_CODE_BYTE	(CIP_START_BYTE + 40)
#define CIP_STATUS_BYTE			(CIP_START_BYTE + 42)
#define CIP_EXT_STATUS_LEN		(CIP_START_BYTE + 43)
#define CIP_EXT_STATUS			(CIP_START_BYTE + 44)
#define CIP_TYPE			(CIP_START_BYTE + 44)
#define CIP_DATA			(CIP_START_BYTE + 46)
#define CIP_QUERY_SERVICE_CODE_BYTE	(CIP_START_BYTE + 50)

/* CIP protocol flags & bitmasks */
#define CIP_CMD_REPLY_FLAG	0x80
#define CIP_EXT_STS_BITMASK	0xff010000
#define CIP_DRV_STS_BITMASK	0xffff0000

/* CIP hex command (service) code mapping, encapsulation codes, IOI etc. */
#define CIP_UNKNOWN_FUNCTION	0x00   /* Invalid command */
#define CIP_DATA_READ		0x4c   /* Data Read */
#define CIP_DATA_WRITE		0x4d   /* Data Write */
#define CIP_REGISTER		0x65   /* Register session */
#define CIP_UNREGISTER		0x66   /* Unregister session */
#define CIP_SEND_RR_DATA	0x6f   /* SendRRData (encap header) */
#define CIP_SEND_UNIT_DATA	0x70   /* SendUnitData (encap header) */
#define CIP_UNCONNECTED_SEND	0x52   /* Unconnected send */

#define CIP_CONNECTED_DATA	0xb1   /* Data for a connected send */
#define CIP_UNCONNECTED_DATA	0xb2   /* Data for an unconnected send */

#define CIP_CLASS_SEGMENT1	0x20   /* IOI class segment (1 byte) */
#define CIP_CLASS_SEGMENT2	0x21   /* IOI class segment (2 byte) */
#define CIP_INSTANCE_SEGMENT1	0x24   /* IOI instance segment (1 byte) */
#define CIP_INSTANCE_SEGMENT2	0x25   /* IOI instance segment (2 byte) */
#define CIP_ELEMENT_SEGMENT1	0x28   /* IOI element segment (1 byte) */
#define CIP_ELEMENT_SEGMENT2	0x29   /* IOI element segment (2 byte) */
#define CIP_ELEMENT_SEGMENT4	0x2a   /* IOI element segment (4 byte) */
#define CIP_SYMBOLIC_SEGMENT	0x91   /* IOI symbolic segment */

#define CIP_CM_CLASS_CODE	0x06   /* Connection Manager class code */
#define CIP_CM_INSTANCE_CODE	0x01   /* Connection Manager instance code */

/* CIP data types */
#define CIP_UNKNOWN_TYPE	0x00   /* Invalid type */
#define CIP_BOOL_TYPE		0xc1   /* Boolean type */
#define CIP_SINT_TYPE		0xc2   /* Single int (8 bit) type */
#define CIP_INT_TYPE		0xc3   /* Int (16 bit) type */
#define CIP_DINT_TYPE		0xc4   /* Double int (32 bit) type */
#define CIP_REAL_TYPE		0xca   /* Real (32 bit) type */
#define CIP_BIT_ARRAY_TYPE	0xd2   /* Packed bit array type (32 bit) */
#define CIP_STRUCT_TYPE		0xa002 /* Generic structure type */

/* CIP general status codes */
#define CIP_STATUS_SUCCESS		0x00
#define CIP_CONN_ERR			0x01
#define CIP_RESOURCE_UNAVAIL		0x02
#define CIP_INVAL_PARAM_VAL		0x03
#define CIP_PATH_SEG_ERR		0x04
#define CIP_PATH_DEST_UNKNOWN		0x05
#define CIP_PARTIAL_TRANSFER		0x06
#define CIP_CONN_LOST			0x07
#define CIP_UNSUPPORTED_SERVICE		0x08
#define CIP_INVAL_ATTRIB_VAL		0x09
#define CIP_ATTRIB_LIST_ERR		0x0a
#define CIP_ALREADY_IN_REQ_STATE	0x0b
#define CIP_OBJ_STATE_CONFLICT		0x0c
#define CIP_OBJ_ALREADY_EXISTS		0x0d
#define CIP_ATTRIB_NOT_SETTABLE		0x0e
#define CIP_PRIV_VIOLATION		0x0f
#define CIP_DEV_STATE_CONFLICT		0x10
#define CIP_REPLY_DATA_TOO_LARGE	0x11
#define CIP_PRIMITIVE_FRAGMENTATION	0x12
#define CIP_NOT_ENOUGH_DATA		0x13
#define CIP_UNSUPPORTED_ATTRIB		0x14
#define CIP_TOO_MUCH_DATA		0x15
#define CIP_OBJ_NOT_EXISTS		0x16
#define CIP_SERVICE_FRAG_SEQ_INACTIVE	0x17
#define CIP_UNSAVED_ATTRIB		0x18
#define CIP_SAVE_ATTRIB_ERR		0x19
#define CIP_ROUTING_REQ_PKT_TOO_LARGE	0x1a
#define CIP_ROUTING_RESP_PKT_TOO_LARGE	0x1b
#define CIP_ATTRIB_LIST_ENTRY_MISSING	0x1c
#define CIP_INVAL_ATTRIB_LIST		0x1d
#define CIP_EMBEDDED_SERVICE_ERR	0x1e
#define CIP_VENDOR_SPECIFIC_ERR		0x1f
#define CIP_INVAL_PARAM			0x20
#define CIP_WORM_WRITE_ERR		0x21
#define CIP_INVAL_RESPONSE_RECV		0x22

/* 0x23-0x24 reserved for CIP future extensions */

#define CIP_KEY_PATH_ERR		0x25
#define CIP_INVAL_PATH_SIZE		0x26
#define CIP_UNEXPECTED_ATTRIB_IN_LIST	0x27
#define CIP_INVAL_MEMBER_ID		0x28
#define CIP_MEMBER_NOT_SETTABLE		0x29
#define CIP_G2_SERVER_GENERAL_ERR	0x2a

/* 0x2b-0xcf reserved for CIP future extensions */
/* 0xd0-0xff reserved for object class & service errors */

/* CIP extended exception codes */
#define CIP_TMPL_OFFSET_VIOLATION	(0x0421 | CIP_EXT_STS_BITMASK)
#define CIP_OBJ_ACCESS_VIOLATION	(0x0521 | CIP_EXT_STS_BITMASK)
#define CIP_TYPE_MISMATCH		(0x0721 | CIP_EXT_STS_BITMASK)

/* CIP driver exception codes */
#define CIP_SERVICE_CODE_MISMATCH	(0x0001 | CIP_DRV_STS_BITMASK)

/* Get the CIP function from a configuration file function code */
#define CIP_GET_FUNC(f)\
((f) == PDS_BREAD || (f) == PDS_CREAD || (f) == PDS_WREAD ||\
 (f) == PDS_LREAD || (f) == PDS_FREAD || (f) == PDS_DREAD ? CIP_DATA_READ :\
 (f) == PDS_BWRITE || (f) == PDS_CWRITE || (f) == PDS_WWRITE ||\
 (f) == PDS_LWRITE || (f) == PDS_FWRITE || (f) == PDS_DWRITE ? CIP_DATA_WRITE :\
 0)

/* Get the CIP data type from a configuration file data type code */
#define CIP_GET_TYPE(t)\
((t) == PDS_BIT ? CIP_BOOL_TYPE : (t) == PDS_INT8 ? CIP_SINT_TYPE :\
 (t) == PDS_INT16 ? CIP_INT_TYPE : (t) == PDS_INT32 ? CIP_DINT_TYPE :\
 (t) == PDS_FLOAT32 ? CIP_REAL_TYPE : 0)

/* Map a write function to its equivalent read function */
#define CIP_WRRD_MAP(f)	((f) == CIP_DATA_WRITE ? CIP_DATA_READ : (f))

/* Map a read function to its equivalent write function */
#define CIP_RDWR_MAP(f)	((f) == CIP_DATA_READ ? CIP_DATA_WRITE : (f))

/* Determine the hi ref of given CIP refs */
#define CIP_HI_REF(n1, n2)	((n2) - (n1) + 1)

/* Get the Session ID from a response to a RegisterSession service request */
#define CIP_GET_SID(b, s)	memcpy((s), &(b)[CIP_SID_BYTE], CIP_SID_LEN)

/* Set the Session ID into a query buffer */
#define CIP_SET_SID(s, b)	memcpy(&(b)[CIP_SID_BYTE], (s), CIP_SID_LEN)

#define CIP_ELEMENT_NBYTES(e)\
((e) <= 0xff ? 2 : (e) > 0xff && (e) <= 0xffff ? 4 : 6)

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* Encapsulation Header                                                        *
******************************************************************************/
typedef struct cip_enc_header_rec
{
  unsigned short int command;
  unsigned short int length;
  unsigned int sid;
  unsigned int status;
  unsigned char sender_context[8];
  unsigned int options;
} cip_enc_header;

/******************************************************************************
* Send Request/Response Data                                                  *
******************************************************************************/
typedef struct cip_send_rr_data_rec
{
  /* N.B.: Unfortunately we cannot define this struct in simple terms of
           ints (as we can the other structs) because the struct is padded
           to be 8 bytes (when it needs to be only 6).  That's why the struct
           members are defined as byte arrays! */
/*  unsigned int iid;
  unsigned short int timeout; */

  unsigned char iid[4];
  unsigned char timeout[2];
} cip_send_rr_data;

/******************************************************************************
* Address/Data Item                                                           *
******************************************************************************/
typedef struct cip_item_rec
{
  unsigned short int typeid;
  unsigned short int length;
  /* Data */
} cip_item;

/******************************************************************************
* Common Packet Format                                                        *
******************************************************************************/
typedef struct cip_cpf_rec
{
  unsigned short int count;
  cip_item address;
  cip_item data;
} cip_cpf;

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
int cip_setup_read_query(unsigned char *query, plc_cnf_block *block);

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
int cip_setup_write_query(unsigned char *query, pdstag *tag, pdsmsg *msg);

/******************************************************************************
* Function to setup a status PLC query using the connection parameters        *
*                                                                             *
* Pre-condition:  Storage for the query & the connection struct containing    *
*                 the PLC connection parameters are passed to the function    *
* Post-condition: The connection struct's parameters are used to construct a  *
*                 generic PLC status query.  The length of the query is       *
*                 returned or -1 if an error occurs                           *
******************************************************************************/
int cip_setup_status_query(unsigned char *query, pdsconn *conn);

/******************************************************************************
* Function to construct a register PLC query                                  *
*                                                                             *
* Pre-condition:  The query buffer is passed to the function                  *
* Post-condition: Query buffer is initialised. On success, the total number   *
*                 of bytes in the query is returned, on error a -1 is         *
*                 returned                                                    *
******************************************************************************/
int cip_construct_register_plc_query(unsigned char *query);

/******************************************************************************
* Function to construct an unregister PLC query                               *
*                                                                             *
* Pre-condition:  The query buffer & the PLC assigned session ID are passed   *
*                 to the function                                             *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_unregister_plc_query(unsigned char *query, unsigned int sid);

/******************************************************************************
* Function to construct a read PLC query                                      *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to read *
*                 & the routing path are passed to the function               *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_read_plc_query(unsigned char *query, char *ioi,
                                 unsigned short int size, char *path);

/******************************************************************************
* Function to construct a write PLC query                                     *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to      *
*                 write, the data type, value(s) & the routing path are       *
*                 passed to the function                                      *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_write_plc_query(unsigned char *query, char *ioi,
                                  unsigned short int size,
                                  unsigned short int type,
                                  unsigned short int *values, char *path);

/******************************************************************************
* Function to construct a write (to an array) PLC query                       *
*                                                                             *
* Pre-condition:  The query buffer, the IOI tagname, the no. of words to      *
*                 write, the data type, the value(s), the array element to    *
*                 start writing at & the routing path are passed to the       *
*                 function                                                    *
* Post-condition: Query buffer is initialised with passed data. On success,   *
*                 the total number of bytes in the query is returned, on      *
*                 error a -1 is returned                                      *
******************************************************************************/
int cip_construct_write_array_plc_query(unsigned char *query, char *ioi,
                                        unsigned short int size,
                                        unsigned short int type,
                                        unsigned short int *values,
                                        unsigned int element, char *path);

/******************************************************************************
* Function to query the PLC                                                   *
*                                                                             *
* Pre-condition:  Open fd and a transaction struct containing the query &     *
*                 storage for the response are passed to the function         *
* Post-condition: Query is run against the PLC, response buffer holds the     *
*                 result of the query.  Number of bytes successfully read is  *
*                 returned or -1 on error                                     *
******************************************************************************/
int cip_run_plc_query(int fd, pdstrans *trans);

/******************************************************************************
* Function to instantiate a prepared PLC query                                *
*                                                                             *
* Pre-condition:  Open fd & a transaction struct containing a prepared query  *
*                 are passed to the function                                  *
* Post-condition: The runtime variables in the query (trans. ID, SID)         *
*                 are instantiated.  The trans. ID is returned or -1 on error *
******************************************************************************/
short int cip_instantiate_prepared_query(int fd, pdstrans *trans);

/******************************************************************************
* Function to clean a PLC query response                                      *
*                                                                             *
* Pre-condition:  Open fd & a transaction struct containing a valid response  *
*                 are passed to the function                                  *
* Post-condition: The response is cleaned (encapsulated header etc. removed). *
*                 The response length is modified accordingly and returned or *
*                 -1 on error                                                 *
******************************************************************************/
short int cip_clean_plc_response(int fd, pdstrans *trans);

/******************************************************************************
* Function to check the validity of a PLC query response                      *
*                                                                             *
* Pre-condition:  A transaction struct containing the response is passed to   *
*                 the function                                                *
* Post-condition: The response is checked.  If successful, a zero is returned *
*                 else a code is returned indicating the exception            *
******************************************************************************/
int cip_check_plc_response(pdstrans *trans);

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
int cip_refresh_status_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to refresh the shared memory data tags with data from a PLC        *
*                                                                             *
* Pre-condition:  The connection struct and a transaction struct containing   *
*                 the query and a valid response are passed to the function   *
* Post-condition: The data in the PLC's response is used to update the data   *
*                 tags in the shared memory segment.  The no. of values in    *
*                 the response is returned or -1 on error                     *
******************************************************************************/
int cip_refresh_data_tags(pdsconn *conn, pdstrans *trans);

/******************************************************************************
* Function to parse the PLC routing path                                      *
*                                                                             *
* Pre-condition:  The PDS routing path string & a buffer to store the CIP     *
*                 encoded routing path are passed to the function             *
* Post-condition: The routing path is parsed & encoded as a CIP routing path. *
*                 The length of the routing path in bytes is returned         *
******************************************************************************/
int cip_parse_plc_routing_path(const char *path, unsigned char *pathbuf);

/******************************************************************************
* Function to connect to a PLC (CIP specific)                                 *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: A connection is established with the PLC.  If an error      *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int cip_connect_to_plc(pdsconn *conn);

/******************************************************************************
* Function to disconnect from a PLC (CIP specific)                            *
*                                                                             *
* Pre-condition:  The connection struct is passed to the function             *
* Post-condition: The connection is finished with the PLC.  If an error       *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int cip_disconnect_from_plc(pdsconn *conn);

#endif

