/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _LED.c
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
#include "_LED.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// Flags that indicate the current state of each LEDs.
uint8 _LED_Green_On;


/*****************************************************************************
* Function Name: _LED_Init()
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
void _LED_Init(void)
{
	// Set Drive mode of output pins to Strong.
	GREEN_SetDriveMode(GREEN_DM_STRONG);
    
    // Reset the flags.
    _LED_Green_On = FALSE;
}


/*****************************************************************************
* Function Name: _LED_TurnOnLED()
******************************************************************************
* Summary:
*   Turns ON the selected LED.
*
* Parameters:
*   LEDs to turn ON.
*
* Return:
*   None.
*
* Note:
*
*****************************************************************************/
void _LED_TurnOnLED(uint8 Green)
{
    if(Green) {
        // Turn Green LED on.
        GREEN_Write(RGB_LED_ON);
        
        // Set the flag.
        _LED_Green_On = TRUE;
    }
}


/*****************************************************************************
* Function Name: _LED_TurnOffLED()
******************************************************************************
* Summary:
*   Turns OFF the selected LED.
*
* Parameters:
*   LEDs to turn OFF.
*
* Return:
*   None.
*
* Note:
*
*****************************************************************************/
void _LED_TurnOffLED(uint8 Green)
{
    if(Green) {
        // Turn Green LED off.
        GREEN_Write(RGB_LED_OFF);
        
        // Reset the flag.
        _LED_Green_On = FALSE;
    }
}

/* [] END OF FILE */