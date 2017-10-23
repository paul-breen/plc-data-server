/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_utils.h                                                       *
* PURPOSE:  Header file for the PDS utilities                                 *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2001-01-09                                                        *
******************************************************************************/

#ifndef __PDS_UTILS_H
#define __PDS_UTILS_H 

/******************************************************************************
* Defines                                                                     *
******************************************************************************/

/* Print an array in a specified format to a specified output stream */
#define PDS_FPRINTF_BARRAY(d, l, f, s)\
do\
{\
  int __i = 0;\
  for(__i = 0; __i < (l); __i++)\
    fprintf((s), (f), (d)[__i]);\
  fputs("\n", (s));\
} while(0)

/* Print 2 digit hex, 3 digit octal & 3 digit decimal arrays */
#define PDS_FPRINT_BARRAYX(d, l, s)	PDS_FPRINTF_BARRAY(d, l, "%02x ", s)
#define PDS_FPRINT_BARRAYO(d, l, s)	PDS_FPRINTF_BARRAY(d, l, "%03o ", s)
#define PDS_FPRINT_BARRAYD(d, l, s)	PDS_FPRINTF_BARRAY(d, l, "%03d ", s)

/* Print 2 digit ascii character array (or a '.' if non-printable) */
#define PDS_FPRINT_BARRAYC(d, l, s)\
do\
{\
  int __i = 0;\
  for(__i = 0; __i < (l); __i++)\
    fprintf((s), "%-02c ", isprint((d)[__i]) ? (d)[__i] : '.');\
  fputs("\n", (s));\
} while(0)

/* Print an array in a specified format */
#define PDS_PRINTF_BARRAY(d, l, f)	PDS_FPRINTF_BARRAY(d, l, f, stdout)

/* Print 2 digit hex, 3 digit octal & 3 digit decimal arrays */
#define PDS_PRINT_BARRAYX(d, l)		PDS_PRINTF_BARRAY(d, l, "%02x ")
#define PDS_PRINT_BARRAYO(d, l)		PDS_PRINTF_BARRAY(d, l, "%03o ")
#define PDS_PRINT_BARRAYD(d, l)		PDS_PRINTF_BARRAY(d, l, "%03d ")

/* Print 2 digit ascii character array (or a '.' if non-printable) */
#define PDS_PRINT_BARRAYC(d, l)		PDS_FPRINT_BARRAYC(d, l, stdout)

/* Get/set bits within bytes, bytes within words nibbles within bytes etc. */
#define PDS_GETBIT(b, n)	(((unsigned char) (b) >> (n)) & 1)
/* #define PDS_SETBIT(b, n)	((unsigned char) (b) = (b) | (1 << (n))) */
#define PDS_SETBIT(b, n)	((b) = (b) | (1 << (n))) 
#define PDS_CLEARBIT(b, n)	(PDS_GETBIT(b, n) ?\
                                 (b) = ((b) ^ (1 << (n))) : (b))
#define PDS_SWITCHBIT(b, n)	(PDS_GETBIT(b, n) ?\
                                 PDS_CLEARBIT(b, n) : PDS_SETBIT(b, n)) 

#define PDS_GETHIBYTE(n)	((n) >> 8)
#define PDS_GETLOBYTE(n)	((n) & 0xff)
#define PDS_MAKEWORD16(h, l)	((h << 8) + (l))

#define PDS_GETBYTE3(n)		((n) >> 24)
#define PDS_GETBYTE2(n)		((n) >> 16)
#define PDS_GETBYTE1(n)		((n) >> 8)
#define PDS_GETBYTE0(n)		((n) & 0xff)
#define PDS_MAKEWORD32(b3, b2, b1, b0)\
(((b3) << 24) + ((b2) << 16) + ((b1) << 8) + (b0))

#define PDS_GETHIWORD(n)	((n) >> 16)
#define PDS_GETLOWORD(n)	((n) & 0xffff)

#define PDS_MAKEDWORD32(h, l)	((h << 16) + (l))

#define PDS_MAKEWORD(h, l)	PDS_MAKEWORD16(h, l)
#define PDS_MAKEDWORD(h, l)	PDS_MAKEDWORD32(h, l)

#define PDS_GETHINIBBLE(b)	((b) >> 4)
#define PDS_GETLONIBBLE(b)	((b) & 0x0f)
#define PDS_MAKEBYTE(h, l)	((h << 4) + (l))

/* Convert binary numbers to ascii & vice versa */
#define PDS_BIN2UASCII(b)	(((b) > 9) ? (b) + 0x37 : (b) + 0x30)
#define PDS_BIN2LASCII(b)	(((b) > 9) ? (b) + 0x57 : (b) + 0x30)
#define PDS_BIN2ASCII(b)	PDS_BIN2UASCII(b)

#define PDS_UASCII2BIN(b)	(((b) > '9') ? (b) - 0x37 : (b) - 0x30)
#define PDS_LASCII2BIN(b)	(((b) > '9') ? (b) - 0x57 : (b) - 0x30)
#define PDS_ASCII2BIN(b)	PDS_UASCII2BIN(b)

#define PDS_LOOKUP_NAME_LEN	96

/******************************************************************************
* Structure definitions                                                       *
******************************************************************************/

/******************************************************************************
* The generic lookup item struct definition                                   *
******************************************************************************/
typedef struct pds_lookup_item_rec
{
  char name[PDS_LOOKUP_NAME_LEN];
  int value;
} pds_lookup_item;

/******************************************************************************
* The generic lookup list struct definition                                   *
******************************************************************************/
typedef struct pds_lookup_list_rec
{
  int nitems;
  pds_lookup_item *items;
} pds_lookup_list;

#endif

