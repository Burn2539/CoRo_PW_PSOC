/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _CapSense.c
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


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "_CapSense.h"
#include "_Watchdog.h"
#include "crc.h"
#include "vector.h"


/*****************************************************************************
* Function Name: _CapSense_Init()
******************************************************************************
* Summary:
*   Initialize the CapSense module.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*
*****************************************************************************/
void _CapSense_Init(void)
{
    // Initialize and enables all the sensors.
    CapSense_Start();
}


/*****************************************************************************
* Function Name: _CapSense_Scan()
******************************************************************************
* Summary:
*   Scan the CapSense module.
*
* Parameters:
*   None.
*
* Return:
*   'SUCCESS' if it succeeded.
*   'NO_MORE_SPACE' if the vector is full.
*
* Note:
*
*****************************************************************************/
uint8 _CapSense_Scan(void)
{
    // Verify if there is space in the vector.
    if ( vectorIsFull() )
        return NO_MORE_SPACE;
    
    
    // Scan the Widgets.
    CapSense_ScanEnabledWidgets();
    
    // Disable watchdog interrupts
    _Watchdog_PauseInt();
    
    // Wait for CapSense scanning to be complete.
	while(CapSense_IsBusy()) {
		CySysPmSleep();
    }
    
    // Enable watchdog interrupts
    _Watchdog_ResumeInt();
        
    // Store the sensors values into the vectors.
    pushInVector(CapSense_SensorRaw);
    
    
    // Verify if the vector is now full.
    if ( vectorIsFull() )
        return NO_MORE_SPACE;
    
    
    return SUCCESS;
}


/*****************************************************************************
* Function Name: _CapSense_Sleep()
******************************************************************************
* Summary:
*   Put the Capsense module to sleep.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*
*****************************************************************************/
void _CapSense_Sleep(void)
{
    CapSense_Sleep();
    CapSense_SetDriveModeAllPins(CY_SYS_PINS_DM_ALG_HIZ);
}


/*****************************************************************************
* Function Name: _CapSense_Wakeup()
******************************************************************************
* Summary:
*   Put the Capsense module to sleep.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*
*****************************************************************************/
void _CapSense_Wakeup(void)
{
     CapSense_RestoreDriveModeAllPins();
    CapSense_Wakeup();
}

/* [] END OF FILE */
