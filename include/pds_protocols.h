/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_protocols.h                                                   *
* PURPOSE:  Header file for the PDS protocols                                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-09                                                        *
******************************************************************************/

#ifndef __PDS_PROTOCOLS_H
#define __PDS_PROTOCOLS_H 

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* Defines for comms protocol code mapping */
#define PDS_UNKNOWN_COMMS_PROTOCOL	0   /* Invalid protocol */
#define MB_TCPIP			1   /* ModBus/TCP/IP */
#define MB_SERIAL_TCPIP			2   /* ModBus/serial/TCP/IP */
#define DH_SERIAL_TCPIP			3   /* DataHighway/serial/TCP/IP */
#define MB_SERIAL			4   /* ModBus/serial */
#define DH_SERIAL			5   /* DataHighway/serial */
#define CIP_TCPIP			6   /* CIP/TCP/IP */

/* Defines for comms protocol code types */
#define PDS_UNKNOWN_PROTO_TYPE		0
#define PDS_NWORK_PROTO			1
#define PDS_SERIAL_PROTO		2
#define PDS_SERIAL_NWORK_PROTO		3

/* Get the type of this protocol code */
#define PDS_GET_PROTOTYPE(p)\
((p) == MB_TCPIP || (p) == CIP_TCPIP ? PDS_NWORK_PROTO :\
 (p) == MB_SERIAL_TCPIP || (p) == DH_SERIAL_TCPIP ? PDS_SERIAL_NWORK_PROTO :\
 (p) == MB_SERIAL || (p) == DH_SERIAL ? PDS_SERIAL_PROTO : 0)

/* Print the type of this protocol code */
#define PDS_PRINT_PROTOTYPE(t)\
((t) == PDS_NWORK_PROTO ? "Network" : (t) == PDS_SERIAL_PROTO ? "Serial" :\
 (t) == PDS_SERIAL_NWORK_PROTO ? "Serial/Network" : "Unknown!")

#endif

