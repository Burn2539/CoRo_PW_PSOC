/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: LowPowerMode.h
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

#ifndef _LOWPOWERMODE_H
#define _LOWPOWERMODE_H


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"
    
    
/*****************************************************************************
* Macros and constants
*****************************************************************************/
// Enables all the low power functionnalities (put to FALSE to disable).
#define LOWPOWERMODE_ENABLED    TRUE


/*****************************************************************************
* Extern global variables
*****************************************************************************/
extern uint8 LowPowerMode_System_DeepSleepAllowed;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void LowPowerMode_Init(void);
void LowPowerMode_IMO(void);
void LowPowerMode_Capsense(void);
void LowPowerMode_Sleep(void);

#endif

/* [] END OF FILE */
