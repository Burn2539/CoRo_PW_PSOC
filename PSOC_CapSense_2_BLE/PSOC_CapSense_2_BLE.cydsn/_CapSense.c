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
#include "crc.h"


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
    
    // Initialize all CapSense Baseline.
	CapSense_InitializeAllBaselines();
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
*   None.
*
* Note:
*
*****************************************************************************/
void _CapSense_Scan(void)
{
    if (!vectorIsFull())
    {
        // Scan the Widgets.
        CapSense_ScanEnabledWidgets();
        
        // Wait for CapSense scanning to be complete.
    	while(CapSense_IsBusy())
    		CySysPmSleep();
        
        // Update CapSense Baseline.
        CapSense_UpdateEnabledBaselines();
            
        // Store the sensors values into the vectors.
        pushInVector(CapSense_sensorRaw);
    }
}


/* [] END OF FILE */
