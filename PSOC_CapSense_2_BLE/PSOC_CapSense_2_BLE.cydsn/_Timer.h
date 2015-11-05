/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _Timer.h
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

#ifndef _TIMER_H
#define _TIMER_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/


/*****************************************************************************
* Extern global variables
*****************************************************************************/
extern uint8 _Timer_DelayDone;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _Timer_Init(void);
CY_ISR_PROTO(Delay);

#endif

/* [] END OF FILE */
