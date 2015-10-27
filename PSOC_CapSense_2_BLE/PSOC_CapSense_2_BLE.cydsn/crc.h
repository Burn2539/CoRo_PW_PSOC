/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: crc.h
* Version 			: 1.0
*
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
* Compiler    		: ARM GCC 4.8.4, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit
*
* Owner             : A. BERNIER
*
*****************************************************************************
*
* Source:	Programming Embedded Systems in C and C++
*			By Michael Barr, p.76 to p.79
*
*****************************************************************************/

#ifndef CRC_H
#define CRC_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>

    
/*****************************************************************************
* Macros and constants
*****************************************************************************/
// The CRC parameters. Currently configured for CCITT.
// Simply modify these to switch to another CRC standard.
#define POLYNOMIAL			0x1021
#define INTIAL_REMAINDER	0xFFFF
#define FINAL_XOR_VALUE		0x0000

// The width of the CRC calculation and result.
// Modify the typedef for an 8 or 32-bit CRC standard.
typedef unsigned short width;

#define WIDTH	(8 * sizeof(width))
#define TOPBIT	(1 << (WIDTH - 1))


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void crc_Init(void);
width crcCompute(unsigned char * message, unsigned int nBytes);
uint32 encodeCRC(uint16 value);
uint8 verifyCRC(uint16 value, uint16 good_crc);

#endif

/* [] END OF FILE */