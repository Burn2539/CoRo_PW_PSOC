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
#include "_Timer.h"
#include "_CapSense.h"
#include "_BLE.h"
#include "_Watchdog.h"


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
void ResetSystem(void);


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
        
        // To execute only if a device is connected
        if (_BLE_deviceConnected) {
            
            // Update the CapSense CCCD.
            _BLE_UpdateCCCD();
            
            // Update the Control values.
            _BLE_UpdateControl();
            
            // If asked by client, send the content of the vector containing
            // the CapSense data.
            if ( _BLE_sendData ) {
                Status_DataAcquired = FALSE;
                Status_Sending = TRUE;
                
                // Delay to let the computer process the previous set of data.
                TimerDelay_Start();
                if (_Timer_DelayDone) {
                    _Timer_DelayDone = FALSE;
                
                    // Send as many sets of values as possible (for all sensors).
                    if( NO_MORE_DATA == _BLE_sendCapSenseData() )
                        Status_NoMoreData = TRUE;
                    else
                        Status_NoMoreData = FALSE;
                }
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
                
                // If asked by client, send data synchronously
                if( _BLE_sendDataSynchronously ) {
                    Status_Sending = TRUE;
                    _BLE_sendCapSenseData();
                }
            }
            
            // If the 'acquireData' request is not present, but the vector
            // isn't empty, then no more data can be acquired.
            else if ( !_BLE_acquireData && !vectorIsEmpty() ) {
                Status_Acquiring = FALSE;
                Status_NoMoreSpace = FALSE;
                Status_DataAcquired = TRUE;
            }
            
            // If the 'sendData' and 'acquireData' requests are not present,
            // but the vector is empty, then data has been all sent and
            // it's ready to acquire new data.
            else if ( !_BLE_sendData && !_BLE_acquireData && vectorIsEmpty() ) {
                Status_Acquiring = FALSE;
                Status_NoMoreSpace = FALSE;
                Status_Sending = FALSE;
                Status_NoMoreData = FALSE;
                Status_Ready = TRUE;
                clearVector();
            }
            
            // If asked by client, send the status flags by notification or
            // indication.
            if (_BLE_sendStatus)
                _BLE_sendStatusFlags();
        }
                    
        // Start advertisement if the flag is set in the BLE event handler.
        if (_BLE_restartAdvertisement) {
            // Reset the flag.
            _BLE_restartAdvertisement = FALSE;
            // Start advertisement.
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        }
        
        // If the client is disconnected, reset everything as the program
        // would be is first started.
        if (_BLE_resetNeeded) {
            // Reset the flag.
            _BLE_resetNeeded = FALSE;
            // Reset everything that needs to.
            ResetSystem();
        }
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
    
    // Initialize the table used for CRC calcultation.
    crc_Init();
    
    //Initalize the vectors used for data storage.
    Vector_Init();
    
    //Initialize the PrISM modules.
    _PrISM_Init();
    
    // Initialize the Timer and Interrupt components.
    _Timer_Init();
    
    // Initialize the CapSense module.
    _CapSense_Init();
    
    // Initialize the BLE module.
    _BLE_Init();
    
    // Initialize the watchdog.
    _Watchdog_Init();
    
    // Initialize status flags
    Status_Ready = TRUE;
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
}


/*****************************************************************************
* Function Name: ResetSystem()
******************************************************************************
* Summary:
*   Reset everything here.
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
void ResetSystem(void)
{
    //Reset the vectors used for data storage.
    clearVector();
    
    // Reset status flags
    Status_Ready = TRUE;
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
    
    // Initialize the BLE module.
    _BLE_Reset();
}

/* [] END OF FILE */