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
* Local global variables
*****************************************************************************/
// Flags that indicate the current state of each LEDs.
uint8 _PrISM_Red_On;
uint8 _PrISM_Green_On;
uint8 _PrISM_Blue_On;


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
	RED_SetDriveMode(RED_DM_ALG_HIZ);
	GREEN_SetDriveMode(GREEN_DM_ALG_HIZ);
	BLUE_SetDriveMode(BLUE_DM_ALG_HIZ);
    
    // Reset the flags.
    _PrISM_Red_On = FALSE;
    _PrISM_Green_On = FALSE;
    _PrISM_Blue_On = FALSE;
}


/*****************************************************************************
* Function Name: _PrISM_TurnOnLED()
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
void _PrISM_TurnOnLED(uint8 Red, uint8 Green, uint8 Blue)
{
    if(Red) {
        // Turn Red LED on.
        PrISM_1_WritePulse0(RGB_LED_ON);
    	RED_SetDriveMode(RED_DM_STRONG);
        
        // Set the flag.
        _PrISM_Red_On = TRUE;
    }
    
    if(Green) {
        // Turn Green LED on.
        PrISM_1_WritePulse1(RGB_LED_ON);
    	GREEN_SetDriveMode(GREEN_DM_STRONG);
        
        // Set the flag.
        _PrISM_Green_On = TRUE;
    }
    
    if(Blue) {
        // Turn Blue LED on.
        PrISM_2_WritePulse0(RGB_LED_ON);
    	BLUE_SetDriveMode(BLUE_DM_STRONG);
        
        // Set the flag.
        _PrISM_Blue_On = TRUE;
    }
}


/*****************************************************************************
* Function Name: _PrISM_TurnOffLED()
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
void _PrISM_TurnOffLED(uint8 Red, uint8 Green, uint8 Blue)
{
    if(Red) {
        // Turn Red LED off.
        PrISM_1_WritePulse0(RGB_LED_OFF);
    	RED_SetDriveMode(RED_DM_ALG_HIZ);
        
        // Reset the flag.
        _PrISM_Red_On = FALSE;
    }
    
    if(Green) {
        // Turn Green LED off.
        PrISM_1_WritePulse1(RGB_LED_OFF);
    	GREEN_SetDriveMode(GREEN_DM_ALG_HIZ);
        
        // Reset the flag.
        _PrISM_Green_On = FALSE;
    }
    
    if(Blue) {
        // Turn Blue LED off.
        PrISM_2_WritePulse0(RGB_LED_OFF);
    	BLUE_SetDriveMode(BLUE_DM_ALG_HIZ);
        
        // Reset the flag.
        _PrISM_Blue_On = FALSE;
    }
}

/* [] END OF FILE */