/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pdsnp_defs.h                                                      *
* PURPOSE:  Header file to define the PDS network protocol & constants        *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#ifndef __PDSNP_DEFS_H
#define __PDSNP_DEFS_H 

#include <pds.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PDSNP_VER		1
#define PDSNP_HOST		"localhost"
#define PDSNP_PORT		9574

#define PDSNP_FEBE_PROTO_VER	PDSNP_VER
#define PDSNP_DEF_HOST		PDSNP_HOST
#define PDSNP_DEF_PORT		PDSNP_PORT

#define PDSNP_SOCKQ		5
#define PDSNP_MAXFD		32 
#define PDSNP_TMO_SECS		15
#define PDSNP_TMO_USECS		0
#define PDSNP_SEC		1000000
#define PDSNP_CLNTRDPAUSE	(PDSNP_SEC / 2)
#define PDSNP_CLNTRDTRIES	8

#define PDSNP_GET_TAG_FUNC_ID	1
#define PDSNP_SET_TAG_FUNC_ID	2

/* N.B.: The network protocol extends the PDS exceptions */
#define PDSNP_COMMS_OK		0x00
#define PDSNP_COMMS_RD_ERR	0x10
#define PDSNP_COMMS_WR_ERR	0x20
#define PDSNP_COMMS_APP_ERR	0x40
#define PDSNP_COMMS_FUNC_ERR	0x80

#define PDS_PRINT_NP_STATUS(s)\
(((s) == PDSNP_COMMS_OK) ? "PDSNP Status OK" :\
 ((s) & PDSNP_COMMS_RD_ERR) ? "PDSNP Comms Read Error" :\
 ((s) & PDSNP_COMMS_WR_ERR) ? "PDSNP Comms Write Error" :\
 ((s) & PDSNP_COMMS_APP_ERR) ? "PDSNP Application Error" :\
 ((s) & PDSNP_COMMS_FUNC_ERR) ? "PDSNP Function Error" :\
 "PDSNP Status Unknown!")

/* Override to include the network protocol extended exceptions */
#ifdef PDSprint_plc_status
#undef PDSprint_plc_status
#define PDSprint_plc_status(c)		((c)->plc_status >= PDSNP_COMMS_RD_ERR ? PDS_PRINT_NP_STATUS((c)->plc_status) : PDS_PRINT_PLC_STATUS((c)->plc_status))
#endif

#define PDSNP_BUF_LEN		1
#define PDSNP_VER_LEN		1
#define PDSNP_FUNC_ID_LEN	1
#define PDSNP_EX_CODE_LEN	1
#define PDSNP_TAGNAME_LEN	(PDS_TAGNAME_LEN)
#define PDSNP_TAGVALUE_LEN	(PDS_TAGVALUE_LEN)

#define PDSNP_LEN		(PDSNP_BUF_LEN + PDSNP_VER_LEN + PDSNP_FUNC_ID_LEN + PDSNP_EX_CODE_LEN + PDSNP_TAGNAME_LEN + PDSNP_TAGVALUE_LEN)

#define PDSNP_GET_BUF_LEN(b)	((int) (b)[0])
#define PDSNP_SET_BUF_LEN(b,v)	((b)[0] = (v))
#define PDSNP_GET_VER(b)	((int) (b)[1])
#define PDSNP_SET_VER(b,v)	((b)[1] = (v))
#define PDSNP_GET_FUNC_ID(b)	((int) (b)[2])
#define PDSNP_SET_FUNC_ID(b,v)	((b)[2] = (v))
#define PDSNP_GET_EX_CODE(b)	((int) (b)[3])
#define PDSNP_SET_EX_CODE(b,v)	((b)[3] = (v))
#define PDSNP_GET_TAGNAME(s,b)	(strncpy((s), (char*) &(b)[4], PDSNP_TAGNAME_LEN))
#define PDSNP_SET_TAGNAME(b,v)	(strncpy((char*) &(b)[4], (v), PDSNP_TAGNAME_LEN))
#define PDSNP_GET_TAGVALUE(s,b)	(strncpy((s), (char*) &(b)[68], PDSNP_TAGVALUE_LEN))
#define PDSNP_SET_TAGVALUE(b,v)	(strncpy((char*) &(b)[68], (v), PDSNP_TAGVALUE_LEN))

/******************************************************************************
* Buffer to handle PDS network stub client/server socket communications       *
******************************************************************************/
typedef unsigned char pdsnp1_buf;           /* PDS n/w protocol v. 1 */
typedef pdsnp1_buf pdsnp_buf;               /* Latest protocol version */
 
/******************************************************************************
* Structure to handle PDS network stub client/server socket communications    *
******************************************************************************/
typedef struct pdscomms_rec
{
  pdsconn *conn;                       /* PDS API connection structure */
  pdsnp_buf buf[PDSNP_LEN];            /* PDS n/w protocol buffer */
} pdscomms;

#endif

