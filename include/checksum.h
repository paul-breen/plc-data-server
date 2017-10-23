/******************************************************************************
* PROJECT:  Checksum generating library                                       *
* MODULE:   checksum.h                                                        *
* PURPOSE:  Header file for the implementation file - checksum.c              *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-11-27                                                        *
******************************************************************************/

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

#define CRC16_POLYNOMIAL	0xA001

#define generate_bcc8		generate_lrc8

/******************************************************************************
* Function prototypes                                                         *
******************************************************************************/

/******************************************************************************
* Function to generate an 8-bit Longitudinal Redundancy Check (LRC) sum       *
*                                                                             *
* Pre-condition:  The data to generate the LRC for as a byte array and the    *
*                 length of the byte array are passed to the function         *
* Post-condition: The LRC is calculated and returned as an 8-bit checksum     *
******************************************************************************/
unsigned char generate_lrc8(const unsigned char *data,
                            unsigned short int len);

/******************************************************************************
* Function to generate a 16-bit Cyclical Redundancy Check (CRC) sum           *
*                                                                             *
* Pre-condition:  The data to generate the CRC for as a byte array and the    *
*                 length of the byte array are passed to the function         *
* Post-condition: The CRC is calculated and returned as a 16-bit checksum     *
******************************************************************************/
unsigned short int generate_crc16(const unsigned char *data,
                                  unsigned short int len);

#endif

