/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: main.c
* Version 			: 1.0
*
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.3
* Compiler    		: ARM GCC 4.8.4, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
*
* Owner             : A. BERNIER
*
*****************************************************************************
*
* Hardware connection required for testing:
* CapSense      - P2[1], P2[2], P2[3], P2[4], P2[5] 
* Cmod pin      - P4[0] (hard-wired in the PSoC 4 BLE Module)
* R-G-B LED 	- P2[6], P3[6] and P3[7] (hard-wired on the BLE Pioneer kit)
*
*****************************************************************************/


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "crc.h"
#include "_PrISM.h"
#include "_CapSense.h"
#include "_BLE.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// Status flags
uint8 Status_Ready;
uint8 Status_Acquiring;
uint8 Status_DataAcquired;
uint8 Status_Sending;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void InitializeSystem(void);


/*****************************************************************************
* Function Name: main()
******************************************************************************
* Summary:
*   Central function which controls the application flow.
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
int main()
{
    InitializeSystem();
    
    for(;;)
    {
        // Process all the events in the stack.
        CyBle_ProcessEvents();
        
        // Update the CapSense CCCD.
        _BLE_UpdateCCCD();
        
        if (_BLE_sendData)
        {
            _CapSense_Scan();
            _BLE_sendCapSenseData();
        }
        else if (_BLE_acquireData)
            _CapSense_Scan();
    }
    
}


/*****************************************************************************
* Function Name: InitializeSystem()
******************************************************************************
* Summary:
*   Initialize everything here.
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
void InitializeSystem(void)
{
    // Enable global interrupts.
    CyGlobalIntEnable;
    
    // Initialize flags
    Status_Ready = FALSE;
    Status_Acquiring = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    
    // Initialize the table used for CRC calcultation.
    crc_Init();
    
    //Initalize the vectors used for data storage.
    Vector_Init();
    
    //Initialize the PrISM modules.
    _PrISM_Init();
    
    // Initialize the CapSense module.
    _CapSense_Init();
    
    // Initialize the BLE module.
    _BLE_Init();
}