/******************************************************************************
* PROJECT:  PLC data server library                                           * 
* MODULE:   pds_spi.h                                                         *
* PURPOSE:  Header file for the PLC data server SPI module                    *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2002-05-01                                                        *
******************************************************************************/

#ifndef __PDS_SPI_H
#define __PDS_SPI_H

#include <pds_api.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define PDS_SPI_TAGNAME_LEN	PDS_TAGNAME_LEN
#define PDS_SPI_IPCKEY		(PDS_IPCKEY + 1)

#define PDS_SPI_PERM_RD		0x01
#define PDS_SPI_PERM_WR		0x02
#define PDS_SPI_PERM_RDWR	(PDS_SPI_PERM_RD | PDS_SPI_PERM_WR)

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The PLC data server SPI tag structure                                       *
******************************************************************************/
typedef struct pds_spi_tag_rec
{
  char name[PDS_SPI_TAGNAME_LEN]; /* The server config. tag name */
  int value;                      /* The server config. tag value */
  unsigned char perms;            /* The server config. tag permissions */
} pds_spi_tag;

/******************************************************************************
* The PLC data server SPI tag list structure                                  *
******************************************************************************/
typedef struct pds_spi_tag_list_rec
{
  int ntags;
  pds_spi_tag *tags;
} pds_spi_tag_list;

/******************************************************************************
* The PLC data server SPI connection structure                                *
******************************************************************************/
typedef struct pds_spi_conn_rec
{
  key_t shmkey;                   /* The shared memory key */
  void *shm;                      /* Pointer to start of shared mem segment */
  int shmsize;                    /* Size of segment */
  int shmflags;                   /* Create flags */
  int shmid;                      /* Shared memory segment ID */

  unsigned short int conn_status; /* The connection status */

  pds_spi_tag *data;              /* Pointer to start of server SPI tags */

  int ndata_tags;                 /* No. of server SPI tags in sh mem */ 

  int febe_proto_ver;             /* Front-end/Back-end protocol version */
   
} pds_spi_conn;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to connect a client to the server SPI                              *
*                                                                             *
* Pre-condition:  A server connection key is passed to the function.  If the  *
*                 connection key is NULL or zero, then a default is used      *
* Post-condition: The client program is connected to the server which is      *
*                 identified by the connection key.  The server connection    *
*                 structure is returned which is used in all subsequent       *
*                 calls to the server.  On return the connection structure    *
*                 should be interrogated to determine if the connection was   *
*                 successful or if an error occurred.  If memory cannot be    *
*                 allocated for the connection structure a null pointer is    *
*                 returned                                                    *
******************************************************************************/
pds_spi_conn* PDS_SPIconnect(key_t connkey);

/******************************************************************************
* Function to disconnect a client from the server SPI                         *
*                                                                             *
* Pre-condition:  A valid server connection struct is passed to the function  *
* Post-condition: The client program is disconnected from the server.  If an  *
*                 error occurrs a -1 is returned                              *
******************************************************************************/
int PDS_SPIdisconnect(pds_spi_conn *conn);

/******************************************************************************
* Function to get an SPI tag's value                                          *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname & an int pointer for *
*                 storage of the tag's value are passed to the function       *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 its corresponding value is returned.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int PDS_SPIget_tag(pds_spi_conn *conn, const char *tagname, int *tagvalue);

/******************************************************************************
* Function to get a pointer to an SPI tag's value                             *
*                                                                             *
* Pre-condition:  A valid server connection & the tagname are passed to the   *
*                 function                                                    *
* Post-condition: The tagname is accessed in the shared memory segment and    *
*                 a pointer to its value is returned.  On error a -1 is       *
*                 returned                                                    *
******************************************************************************/
int* PDS_SPIget_tag_ptr(pds_spi_conn *conn, const char *tagname);

/******************************************************************************
* Function to set an SPI tag value                                            *
*                                                                             *
* Pre-condition:  A valid server connection, the tagname & the tag value      *
*                 are passed to the function                                  *
* Post-condition: The tag value is set to the value passed in the parameter.  *
*                 On error a -1 is returned                                   *
******************************************************************************/
int PDS_SPIset_tag(pds_spi_conn *conn, const char *tagname, int tagvalue);
 
#endif

