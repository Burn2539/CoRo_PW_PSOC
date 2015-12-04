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
* CapSense      - P2[2], P2[3], P2[4], P2[5], P2[6],
*                 P2[7], P2[1], P1[7], P1[6], P1[5]
* Cmod pin      - P4[0] (hard-wired in the PSoC 4 BLE Module)
* Green LED 	- P3[7]
*
*****************************************************************************/


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "crc.h"
#include "vector.h"
#include "_LED.h"
#include "_CapSense.h"
#include "_Watchdog.h"
#include "_BLE.h"
#include "LowPowerMode.h"
#include "_Timer.h"


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

// Count of connection intervals used to wait between each send command.
uint8 numConnInterval;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void InitializeSystem(void);
void ReadyToAcquireData(void);
void AcquireData(void);
void ReadyToSendData(void);
void SendData(void);
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
    // Init.
    InitializeSystem();

    // Wait for BLE component to finish initialization.
    while (CyBle_GetState() == CYBLE_STATE_INITIALIZING) {
        CyBle_ProcessEvents();
    } 

    //////////////////////////////////////////////////////////////////////////
    // Main Loop
    //////////////////////////////////////////////////////////////////////////
    for(;;)
    {

        // Process all the events in the stack.
        CyBle_ProcessEvents();


        #if LOWPOWERMODE_ENABLED
            // Put the BLESS in deepest sleep possible.
            CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        #endif

        //////////////////////////////////////////////////////////////////////
        // Device connected
        //////////////////////////////////////////////////////////////////////
        if (_BLE_deviceConnected) {
            
            // Update the CapSense CCCD.
            _BLE_UpdateCCCD();
            
            // Update the Control values.
            _BLE_UpdateControl();
            
            // If asked by client, send the status flags by notification or
            // indication.
            if (_BLE_sendStatus)
                _BLE_sendStatusFlags();
                

            #if LOWPOWERMODE_ENABLED
                // Handle the Capsense low power mode.
                LowPowerMode_Capsense();
            #endif
            
            // If asked by client, send the content of the vector containing
            // the CapSense data.
            if (_BLE_sendData) {
                SendData();
                // Allow DeepSleep between connection intervals.
                LowPowerMode_System_DeepSleepAllowed = TRUE;
            }
            
            // If asked by client, acquire CapSense data and store it in
            // the vector.
            else if (_BLE_acquireData || TEST_USB) {
                AcquireData();
                // Keep system in Active mode during scanning.
                LowPowerMode_System_DeepSleepAllowed = FALSE;
            }

            // If the 'acquireData' request is not present, but the vector
            // isn't empty, then no more data can be acquired.
            else if (!_BLE_acquireData && !vectorIsEmpty()) {
                ReadyToSendData();
                // Allow DeepSleep between connection intervals.
                LowPowerMode_System_DeepSleepAllowed = TRUE;
            }
            
            // If the 'sendData' and 'acquireData' requests are not present,
            // but the vector is empty, then data has been all sent and
            // it's ready to acquire new data.
            else if (!_BLE_sendData && !_BLE_acquireData && vectorIsEmpty()) {
                ReadyToAcquireData();
                // Allow DeepSleep between connection intervals.
                LowPowerMode_System_DeepSleepAllowed = TRUE;
            }
        }
        
        else
            // Allow DeepSleep between advertisement intervals.
            LowPowerMode_System_DeepSleepAllowed = TRUE;
        
        //////////////////////////////////////////////////////////////////////
        // Advertisement
        //////////////////////////////////////////////////////////////////////
        // Start advertisement if the flag is set in the BLE event handler.
        if (_BLE_restartAdvertisement) {
            // Reset the flag.
            _BLE_restartAdvertisement = FALSE;
            // Start advertisement.
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        }
        
        //////////////////////////////////////////////////////////////////////
        // Reset
        //////////////////////////////////////////////////////////////////////
        // If the client is disconnected, reset everything as the program
        // would be is first started.
        if (_BLE_resetNeeded) {
            // Reset the flag.
            _BLE_resetNeeded = FALSE;
            // Reset everything that needs to.
            ResetSystem();
        }

        //////////////////////////////////////////////////////////////////////
        // Low Power Mode (Sleep)
        //////////////////////////////////////////////////////////////////////
        #if LOWPOWERMODE_ENABLED
            // Put the system in the deepest sleep possible.
            LowPowerMode_Sleep();
            // Reset flag.
            LowPowerMode_System_DeepSleepAllowed = FALSE;
        #endif
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
    
    //Initialize the LED module.
    _LED_Init();
    
    // Initialize the CapSense module.
    _CapSense_Init();

    // Initialize the BLE module.
    _BLE_Init();

    // Initialize the watchdog.
    _Watchdog_Init();
    
    #if LOWPOWERMODE_ENABLED
        // Initialize the Low Power module.
        LowPowerMode_Init();
    #else
        // Initialize the Timer and related Interrupt components.
        _Timer_Init();
    #endif
    
    // Initialize status flags
    Status_Ready = TRUE;
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
    
    // Initialize counters
    numConnInterval = 0u;
}


/*****************************************************************************
* Function Name: ReadyToAcquireData()
******************************************************************************
* Summary:
*   Operations to indicate that we are ready to acquire the Capsense data.
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
void ReadyToAcquireData(void)
{
    // Set flags.
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
    Status_Ready = TRUE;
    
    // Clear previous data.
    clearVector();
}


/*****************************************************************************
* Function Name: AcquireData()
******************************************************************************
* Summary:
*   Operations to acquire the Capsense data.
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
void AcquireData(void)
{
    // Set flags.
    Status_Ready = FALSE;
    Status_Acquiring = TRUE;
    
    // Scan the CapSense widgets once.
    if(NO_MORE_SPACE == _CapSense_Scan())
        Status_NoMoreSpace = TRUE;
    else
        Status_NoMoreSpace = FALSE;

    // If asked by client, send data synchronously
    if(_BLE_sendDataSynchronously) {
        Status_Sending = TRUE;
        _BLE_sendCapSenseData();
    }
}


/*****************************************************************************
* Function Name: ReadyToSendData()
******************************************************************************
* Summary:
*   Operations to indicate that we are ready to send the Capsense data.
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
void ReadyToSendData(void)
{
    // Set flags.
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = TRUE;
}


/*****************************************************************************
* Function Name: SendData()
******************************************************************************
* Summary:
*   Operations to send the Capsense data.
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
void SendData(void)
{
    // Set flags.
    Status_DataAcquired = FALSE;
    Status_Sending = TRUE;
    
    #if LOWPOWERMODE_ENABLED
        // Delay between each send command.
        if (numConnInterval == CONN_INTERVAL_DELAY) {
            numConnInterval = 0u;

    #else
        // Delay so the Event stack doesn't get overloaded.
        TimerDelay_Start();
        if (_Timer_DelayDone) {
            TimerDelay_Stop();
            _Timer_DelayDone = FALSE;
    #endif
    
        // Send as many sets of values as possible (for all sensors).
        if(NO_MORE_DATA == _BLE_sendCapSenseData())
            Status_NoMoreData = TRUE;
        else
            Status_NoMoreData = FALSE;
    }
    
    #if LOWPOWERMODE_ENABLED
        else
            numConnInterval++;
    #endif

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
    // Clear previous data.
    clearVector();
    
    // Reset status flags.
    Status_Ready = TRUE;
    Status_Acquiring = FALSE;
    Status_NoMoreSpace = FALSE;
    Status_DataAcquired = FALSE;
    Status_Sending = FALSE;
    Status_NoMoreData = FALSE;
    
    // Reset the BLE module.
    _BLE_Reset();
}

/* [] END OF FILE */