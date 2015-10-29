/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: main.h
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

#ifndef _MAIN_H
#define _MAIN_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>
#include "vector.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/
#define FALSE   0u
#define TRUE    1u
    
// Return values of functions
#define SUCCESS         0u
#define NO_MORE_SPACE   1u
#define NO_MORE_DATA    2u


/*****************************************************************************
* Extern global variables
*****************************************************************************/
extern uint8 Status_Ready;
extern uint8 Status_Acquiring;
extern uint8 Status_NoMoreSpace;
extern uint8 Status_DataAcquired;
extern uint8 Status_Sending;
extern uint8 Status_NoMoreData;

#endif

/* [] END OF FILE */
