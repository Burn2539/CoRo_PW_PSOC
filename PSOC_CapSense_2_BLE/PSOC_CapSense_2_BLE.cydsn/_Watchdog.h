/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _Watchdog.h
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

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/
// Watchdog Interrupt Vector number in PSoC 4 BLE. See PSoC 4 BLE TRM for details.
#define WATCHDOG_INT_VEC_NUM    8
											
// Desired Watchdog Interrupt priority.
#define WATCHDOG_INT_VEC_PRIORITY   3
    
// Number of ticks in 1 second for the LFCLK (32.768kHz).
#define WATCHDOG_ONE_SEC_COUNT_VAL  32768

// Number of ticks in 1 millisecond for the LFCLK (32.768kHz).
#define WATCHDOG_ONE_MILLISEC_COUNT_VAL 33
    
// After Resetting or Writing the Counter 0 register with match value, it 
// takes ~3 LFCLK cycles to take effect. Provide a delay of 100us after 
// above actions for changes to take effect.
#define WATCHDOG_REG_UPDATE_WAIT_TIME   100
    
// Counters period
#define WATCHDOG_COUNTER0_PERIOD (WATCHDOG_ONE_MILLISEC_COUNT_VAL * CYBLE_GAPP_CONNECTION_INTERVAL_MIN)
#define WATCHDOG_COUNTER1_PERIOD (WATCHDOG_ONE_MILLISEC_COUNT_VAL * 1000)

    
/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _Watchdog_Init(void);
void _Watchdog_PauseInt(void);
void _Watchdog_ResumeInt(void);
void _Watchdog_Handler(void);

#endif

/* [] END OF FILE */
