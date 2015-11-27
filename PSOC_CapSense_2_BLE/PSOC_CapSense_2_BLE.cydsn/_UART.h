/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _UART.h
* Version 			: 1.0
*
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.3
* Compiler    		: ARM GCC 4.8.4, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
*
* Owner             : A. BERNIER
*
*****************************************************************************/

#ifndef _UART_H
#define _UART_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _UART_Init(void);
void _UART_sendCapSenseData(void);

#endif

/* [] END OF FILE */
