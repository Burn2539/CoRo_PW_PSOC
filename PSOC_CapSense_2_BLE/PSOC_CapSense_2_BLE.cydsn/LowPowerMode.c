/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: LowPowerMode.c
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
#include "LowPowerMode.h"
#include "_BLE.h"
#include "_CapSense.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// Flags
uint8 LowPowerMode_Capsense_inSleep;
uint8 LowPowerMode_System_DeepSleepAllowed;


/*****************************************************************************
* Function Name: LowPowerMode_Init()
******************************************************************************
* Summary:
*   Initialize the Low Power Mode module.
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
void LowPowerMode_Init(void)
{
    // Stop the ILO to reduce current consumption.
    CySysClkIloStop();
    
    // Configure the divider values for the ECO, so that a 3-MHz ECO divided
    // clock can be provided to the CPU in Sleep mode.
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);
    
    // Init flags.
    LowPowerMode_Capsense_inSleep = FALSE;
    LowPowerMode_System_DeepSleepAllowed = FALSE;
}


/*****************************************************************************
* Function Name: LowPowerMode_Capsense()
******************************************************************************
* Summary:
*   Handles the Capsense low power mode.
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
void LowPowerMode_Capsense(void)
{
    // If we are not acquiring data, put Capsense into sleep.
    if (!_BLE_acquireData && !LowPowerMode_Capsense_inSleep) {
        _CapSense_Sleep();
        LowPowerMode_Capsense_inSleep = TRUE;
    }
    else if (_BLE_acquireData && LowPowerMode_Capsense_inSleep) {
        _CapSense_Wakeup();
        LowPowerMode_Capsense_inSleep = FALSE;
    }
}


/*****************************************************************************
* Function Name: LowPowerMode_Sleep()
******************************************************************************
* Summary:
*   Handles the system's power modes.
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
void LowPowerMode_Sleep(void)
{
    // Stores the current BLESS power mode.
    CYBLE_BLESS_STATE_T blePowerMode;
    
    //  Get current state of BLE sub system.
    blePowerMode =  CyBle_GetBleSsState();
    
    // System can enter DeepSleep only when BLESS and rest of the application
    // are in DeepSleep or equivalent power modes.
    if((blePowerMode == CYBLE_BLESS_STATE_DEEPSLEEP ||
        blePowerMode == CYBLE_BLESS_STATE_ECO_ON) &&
        LowPowerMode_System_DeepSleepAllowed)
    {
        // Put system into Deep-Sleep mode.
        CySysPmDeepSleep();
        // Put the BLESS back into Active mode.
        CyBle_EnterLPM(CYBLE_BLESS_ACTIVE);
    }
    
    // BLESS is not in Deep Sleep mode. Check if it can enter Sleep mode.
    else if(blePowerMode != CYBLE_BLESS_STATE_EVENT_CLOSE)
    {
        // Application can be in DeepSleep, so IMO is not required.
        if(LowPowerMode_System_DeepSleepAllowed)
        {
            // Change HFCLK source from IMO to ECO.
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO);
            // Stop IMO for reducing current consumption.
            CySysClkImoStop();
            // Put the CPU to sleep.
            CySysPmSleep();
            // Put the BLESS back into Active mode.
            CyBle_EnterLPM(CYBLE_BLESS_ACTIVE);
            // After waking up, start IMO.
            CySysClkImoStart();
            // Change HFCLK source back to IMO.
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
        }
    }
}

/* [] END OF FILE */
