/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _LED.h
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

#ifndef _LED_H
#define _LED_H

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
* Extern global variables
*****************************************************************************/
// Flags that indicate the current state of each LEDs.
extern uint8 _LED_Green_On;

/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _LED_Init(void);
void _LED_TurnOnLED(uint8 Green);
void _LED_TurnOffLED(uint8 Green);

#endif

/* [] END OF FILE */
