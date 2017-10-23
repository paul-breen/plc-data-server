/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_plc_cnf.h                                                     *
* PURPOSE:  Header file for the pds_plc_cnf.c and pds_plc_cnf_scan.l          *
*           implementation files                                              *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/
 
#ifndef __PDS_PLC_CNF_H
#define __PDS_PLC_CNF_H

#include <stdio.h>

#include <pds_defs.h>
#include <pds_utils.h>
#include <pds_functions.h>
#include <pds_protocols.h>
#include <pds_types.h>

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* Configuration file constants */
#define PLC_CNF_DATA_DIR	"./" 
#define PLC_CNF_DIRSEP		'/'
#define PLC_CNF_FILENAME	"plc.cnf" 
#define PLC_CNF_FILEMODE	"r"

/* Configuration data constants */
#define PLC_CNF_FN_LEN		96
#define PLC_CNF_IP_ADDR_LEN	PDS_IP_ADDR_LEN
#define PLC_CNF_TTY_DEV_LEN	PDS_TTY_DEV_LEN
#define PLC_CNF_DEFAULT_PORT	502
#define PLC_CNF_TAGNAME_LEN	PDS_TAGNAME_LEN
#define PLC_CNF_TAGS_BLK	125
#define PLC_CNF_BLKS		100
#define PLC_CNF_PLCS		PLC_CNF_BLKS
#define PLC_CNF_PLC_ADDR_LEN	PDS_PLC_ADDR_LEN
#define PLC_CNF_PLC_REF_LEN	PDS_PLC_REF_LEN
#define PLC_CNF_PLC_PATH_LEN	PDS_PLC_PATH_LEN

/******************************************************************************
* Stucture definitions                                                        *
******************************************************************************/

/******************************************************************************
* PLC configuration tag structure                                             *
******************************************************************************/
typedef struct plc_cnf_tag_rec
{
  char name[PLC_CNF_TAGNAME_LEN];           /* Tag name for tag/ref mapping */
  unsigned int ref;                         /* Reference for tag/ref mapping */
  char ascii_ref[PLC_CNF_PLC_REF_LEN];      /* Reference for tag/ref mapping */
  unsigned char bit;                        /* Bit in the word to read */
} plc_cnf_tag;

/******************************************************************************
* PLC configuration block structure                                           *
******************************************************************************/
typedef struct plc_cnf_block_rec
{
  /****************** Configuration file block header data *******************/
  unsigned short int protocol;              /* Block's comms protocol */
  unsigned short int function;              /* Block's function */
  unsigned short int type;                  /* Block's data type */
  char path[PLC_CNF_PLC_PATH_LEN];          /* Routing path (string) */
  char ip_addr[PLC_CNF_IP_ADDR_LEN];        /* IP address (string) */
  unsigned short int port;                  /* TCP port */
  char tty_dev[PLC_CNF_TTY_DEV_LEN];        /* TTY device (string) */
  unsigned int base_addr;                   /* Block's base address */
  char ascii_addr[PLC_CNF_PLC_ADDR_LEN];    /* Block's logical address */
  int pollrate;                             /* Block's poll rate (in msecs) */
  unsigned short int ntags;                 /* No. of tags in this block */

  /******************* The tags configured for this block ********************/
  plc_cnf_tag tags[PLC_CNF_TAGS_BLK];       /* Tags array */ 
} plc_cnf_block;

/******************************************************************************
* PLC configuration PLC structure                                             *
******************************************************************************/
typedef struct plc_cnf_plc_rec
{
  unsigned short int protocol;              /* PLC's comms protocol */
  char ip_addr[PLC_CNF_IP_ADDR_LEN];        /* IP address (string) */
  unsigned short int port;                  /* TCP port */
  char tty_dev[PLC_CNF_TTY_DEV_LEN];        /* TTY device (string) */
  char path[PLC_CNF_PLC_PATH_LEN];          /* Routing path (string) */
} plc_cnf_plc;

/******************************************************************************
* PLC configuration file structure                                            *
******************************************************************************/
typedef struct plc_cnf_rec
{
  char filename[PLC_CNF_FN_LEN];       /* Name of this configuration file */
  unsigned short int nblocks;          /* No. of blocks in this file */
  unsigned short int nplcs;            /* No. of unique PLCs in this file */
  unsigned short int ndata_tags;       /* No. of data tags in this file */
  unsigned short int nstatus_tags;     /* No. of status tags in this file */
  unsigned short int ttags;            /* Total no. of tags in this file */

  /******************* The blocks configured for this file *******************/
  plc_cnf_block blocks[PLC_CNF_BLKS];  /* Blocks array */

  /******************** The PLCs configured for this file ********************/
  plc_cnf_plc plcs[PLC_CNF_PLCS];      /* PLCs array */
} plc_cnf;

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to get a file's PLC configuration data                             *
*                                                                             *
* Pre-condition:  The filename and mode are passed to the function            *
* Post-condition: The file is opened and it's configuration data is returned  *
*                 in the struct.  If an error occurred a null is returned     *
******************************************************************************/
plc_cnf* get_plc_cnf_data(char *filename, char *filemode);

/******************************************************************************
* Function to read a PLC configuration file                                   *
*                                                                             *
* Pre-condition:  The successfully opened config file handle is passed to     *
*                 the function                                                *
* Post-condition: Each configured tag is read into a structure.  On success   *
*                 the struct is returned, on error a null is returned         *
******************************************************************************/
plc_cnf* read_plc_cnf_file(FILE *handle);

/******************************************************************************
* Function to translate a block function string into an integer constant      *
*                                                                             *
* Pre-condition:  The block function string is passed to the function         *
* Post-condition: The block function is returned as an integer or 0 if the    *
*                 block function is unknown                                   *
******************************************************************************/
static int get_plc_cnf_block_function(char *str);

/******************************************************************************
* Function to translate a block protocol string into an integer constant      *
*                                                                             *
* Pre-condition:  The block protocol string is passed to the function         *
* Post-condition: The block protocol is returned as an integer or 0 if the    *
*                 block protocol is unknown                                   *
******************************************************************************/
static int get_plc_cnf_block_protocol(char *str);

/******************************************************************************
* Function to add, if not already present, this block's PLC to the PLC array  *
*                                                                             *
* Pre-condition:  The block's array index and the configuration struct are    *
*                 passed to the function                                      *
* Post-condition: If not already present, the block's PLC is added to the PLC *
*                 array                                                       *
******************************************************************************/
static int configure_plcs(int block, plc_cnf *conf);
 
#endif

