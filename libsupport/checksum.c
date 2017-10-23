/******************************************************************************
* PROJECT:  Checksum generating library                                       * 
* MODULE:   checksum.c                                                        *
* PURPOSE:  Library of functions to generate various checksums                *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-11-27                                                        *
******************************************************************************/

#include "checksum.h"

/******************************************************************************
* Function to generate an 8-bit Longitudinal Redundancy Check (LRC) sum       *
*                                                                             *
* Pre-condition:  The data to generate the LRC for as a byte array and the    *
*                 length of the byte array are passed to the function         *
* Post-condition: The LRC is calculated and returned as an 8-bit checksum     *
******************************************************************************/
unsigned char generate_lrc8(const unsigned char *data,
                            unsigned short int len)
{
  unsigned char lrc = 0;

  /* Add each byte to the LRC (no carry), and return 2s complement */
  while(len--)
    lrc += *data++;

  return((unsigned char) (-((char)lrc)));
}



/******************************************************************************
* Function to generate a 16-bit Cyclical Redundancy Check (CRC) sum           *
*                                                                             *
* Pre-condition:  The data to generate the CRC for as a byte array and the    *
*                 length of the byte array are passed to the function         *
* Post-condition: The CRC is calculated and returned as a 16-bit checksum     *
******************************************************************************/
unsigned short int generate_crc16(const unsigned char *data,
                                  unsigned short int len)
{
  unsigned short int crc = 0xffff;
  unsigned int i = 0, j = 0, lsb = 0;

  for(i = 0; i < len; i++)
  {
    crc = crc ^ *data++;

    for(j = 0; j < 8; j++)
    {
      lsb = crc & 0x01;
      crc = (crc >> 1) & 0x7fff;

      if(lsb)
        crc = crc ^ CRC16_POLYNOMIAL;
    }
  }

  return crc;
}

