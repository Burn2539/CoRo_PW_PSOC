/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _PrISM.h
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

#ifndef _PRISM_H
#define _PRISM_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/
// PrISM density value for LED ON and LED OFF. Note that LED on BLE Pioneer
// kit is active low.
#define RGB_LED_OFF			255
#define RGB_LED_ON			0


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _PrISM_Init(void);

#endif

/* [] END OF FILE */
