/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: crc.c
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


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "crc.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// An array containing the pre-computed intermediate result for each
// possible byte of input. This is used to speed up the computation.
width crcTable[256];



/*****************************************************************************
* Function Name: crcInit()
******************************************************************************
* Summary:
*   Initialize the CRC lookup table. This table is used
*	by crcCompute() to make CRC computation faster.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*   The mod-2 binary long division is implemented here.
*****************************************************************************/
void crc_Init(void)
{
	width	remainder;
	width	dividend;
	int		bit;

	// Perform binary long division, a bit at a time.
	for (dividend = 0; dividend < 256; dividend++)
	{
		// Initialize the remainder.
		remainder = dividend << (WIDTH - 8);

		//Shift and XOR with the polynomial.
		for (bit = 0; bit < 8; bit++)
		{
			//Try to divide the current data bit.
			if (remainder & TOPBIT)
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = remainder << 1;
			}
		}

		// Save the result in the table.
		crcTable[dividend] = remainder;
	}

}


/*****************************************************************************
* Function Name: crcCompute()
******************************************************************************
* Summary:
*   Compute the CRC checksum of a binary message block.
*
* Parameters:
*   Pointer to the message.
*	Number of bytes of the message.
*
* Return:
*   The CRC of the data.
*
* Note:
*   This function expects that crcInit() has been called
*	first to intialize the CRC lookup table.
*****************************************************************************/
width crcCompute(unsigned char * message, unsigned int nBytes)
{
	unsigned int	offset;
	unsigned char	byte;
	width			remainder = INTIAL_REMAINDER;

	// Divide the message by the polynomial, a byte at a time.
	for (offset = 0; offset < nBytes; offset++)
	{
		byte = (remainder >> (WIDTH - 8)) ^ message[offset];
		remainder = crcTable[byte] ^ (remainder << 8);
	}

	// The final remainder is the CRC result.
	return (remainder ^ FINAL_XOR_VALUE);

}


/*****************************************************************************
* Function Name: encodeCRC()
******************************************************************************
* Summary:
*   Add the CRC at the end of the message.
*
* Parameters:
*   Value to encode.
*
* Return:
*   32 bits message. Message (16 bits) followed by
*	the CRC (16 bits).
*
* Note:
*   Works with a original message of 16 bits.
*****************************************************************************/
uint32 encodeCRC(uint16 value)
{
	const int		SizeOfMessage = sizeof(value);
	unsigned char 	message[SizeOfMessage];
	width			crc;
	uint32		    message_with_crc;
	int				i;

	// Convert the uint16 into a array of unsigned chars.
	for (i = 0; i < SizeOfMessage; i++)
	{
		message[SizeOfMessage - 1 - i] = (value >> (i * 8) & 0xFF);
	}

	// Calculate the CRC.
	crc = crcCompute(message, SizeOfMessage);

	// Concatenate the original message with the CRC.
	message_with_crc = value << 8 * SizeOfMessage | crc;

	return message_with_crc;

}


/*****************************************************************************
* Function Name: verifyCRC()
******************************************************************************
* Summary:
*   Verify that the message is valid according to the CRC.
*
* Parameters:
*   Message to verify.
*	CRC of the message ot verify.
*
* Return:
*   FALSE if checkSUM is bad.
*	TRUE if checkSUM is good.
*
* Note:
*   
*****************************************************************************/
uint8 verifyCRC(uint16 value, uint16 good_crc)
{
	const int		SizeOfMessage = sizeof(value);
	unsigned char 	message[SizeOfMessage];
	width			crc;
	int				i;

	/* Convert the uint16 into a array of unsigned chars. */
	for (i = 0; i < SizeOfMessage; i++)
	{
		message[SizeOfMessage - 1 - i] = (value >> (i * 8) & 0xFF);
	}

	/* Calculate the CRC. */
	crc = crcCompute(message, SizeOfMessage);

	/* Validate the CRC received with the CRC calulated. */
	return (crc == good_crc);

}


/* [] END OF FILE */