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
uint8 Status_NoMoreSpace;
uint8 Status_DataAcquired;
uint8 Status_Sending;
uint8 Status_NoMoreData;


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
        
        // Update the Control values.
        _BLE_UpdateControl();
        
        // If asked by client, send the content of the vector containing
        // the CapSense data.
        if ( _BLE_sendData ) {
            Status_DataAcquired = FALSE;
            Status_Sending = TRUE;
            
            // Send one set of values (all sensors).
            if( NO_MORE_DATA == _BLE_sendCapSenseData() ) {
                Status_NoMoreData = TRUE;
                deleteVector();
            }
            else
                Status_NoMoreData = FALSE;
        }
        
        // If asked by client, acquire CapSense data and store it in
        // the vector.
        else if ( _BLE_acquireData ) {
            Status_Ready = FALSE;
            Status_Acquiring = TRUE;
            
            // Scan the CapSense widgets once.
            if( NO_MORE_SPACE == _CapSense_Scan() )
                Status_NoMoreSpace = TRUE;
            else
                Status_NoMoreSpace = FALSE;
        }
        
        // If the 'acquireData' request is not present, but the vector
        // isn't empty, then no more data can be acquired.
        else if ( !_BLE_acquireData && !vectorIsEmpty() ) {
            Status_Acquiring = FALSE;
            Status_NoMoreSpace = FALSE;
            Status_DataAcquired = TRUE;
        }
        
        // If the 'sendData' request is not present, but the vector
        // is empty, then data has been all sent and it's ready to
        // acquire new data.
        else if ( !_BLE_sendData && vectorIsEmpty() ) {
            Status_Sending = FALSE;
            Status_NoMoreData = FALSE;
            Status_Ready = TRUE;
        }
        
        // If asked by client, send the status flags by notification or
        // indication.
        if (_BLE_sendStatus)
            _BLE_sendPSOCStatus();
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
    Status_Ready = TRUE;
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
    
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