/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _PrISM.c
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
#include "_PrISM.h"


/*****************************************************************************
* Function Name: _PrISM_Init()
******************************************************************************
* Summary:
*   Initialize the PrISM modules.
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
void _PrISM_Init(void)
{
    // Start the PrISM component for LED control.
    PrISM_1_Start();
    PrISM_2_Start();
    
    // The RGB LED on BLE Pioneer kit are active low. Drive HIGH on 
	// pin for OFF and drive LOW on pin for ON.
	PrISM_1_WritePulse0(RGB_LED_OFF);
	PrISM_1_WritePulse1(RGB_LED_OFF);
	PrISM_2_WritePulse0(RGB_LED_OFF);
	
	// Set Drive mode of output pins from HiZ to Strong.
	RED_SetDriveMode(RED_DM_STRONG);
	GREEN_SetDriveMode(GREEN_DM_STRONG);
	BLUE_SetDriveMode(BLUE_DM_STRONG);
}


/* [] END OF FILE */