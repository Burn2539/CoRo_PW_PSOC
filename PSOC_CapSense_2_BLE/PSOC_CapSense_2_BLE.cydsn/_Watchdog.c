/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _Watchdog.c
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
#include "_Watchdog.h"
#include "_BLE.h"
#include "_PrISM.h"


/*****************************************************************************
* Function Name: _Watchdog_Init()
******************************************************************************
* Summary:
*   Initialize the watchdog.
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
void _Watchdog_Init(void)
{
    // Unlock the WDT registers for modification.
	CySysWdtUnlock();
    
    // Write Mode for Counters as Interrupt on Match.
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER1, CY_SYS_WDT_MODE_INT);
    
    // Set Clear on Match for Counters.
	CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER0, TRUE);
    CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER1, TRUE);
    
    // Set Watchdog interrupt to lower priority.
	CyIntSetPriority(WATCHDOG_INT_VEC_NUM, WATCHDOG_INT_VEC_PRIORITY);
    
    // Enable Watchdog Interrupt using Interrupt number.
    CyIntEnable(WATCHDOG_INT_VEC_NUM);
    
    // Write the match value in Counters.
	CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, WATCHDOG_COUNTER0_PERIOD);
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER1, WATCHDOG_COUNTER1_PERIOD);
    
    // Enable Counters.
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
	
	// Lock Watchdog to prevent further changes.
    CySysWdtLock();
    
    // Set the Watchdog Interrupt vector to the address of Interrupt routine 
	// _Watchdog_Handler.
	CyIntSetVector(WATCHDOG_INT_VEC_NUM, &_Watchdog_Handler);
}


/*****************************************************************************
* Function Name: _Watchdog_Handler()
******************************************************************************
* Summary:
*   Watchdog interrupt routine.
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
void _Watchdog_Handler(void)
{
    // If the Interrupt source is Counter 0 match, then process.
	if(CySysWdtGetInterruptSource() & CY_SYS_WDT_COUNTER0_INT)
	{
        // Clear Watchdog Interrupt from Counter 0.
		CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
        
        /********************************************************************/
        
        // Process all the events in the stack.
        if(CyBle_GattGetBusyStatus() == CYBLE_STACK_STATE_BUSY)
            CyBle_ProcessEvents();
        
        /********************************************************************/
        
        // Unlock the WDT registers for modification.
		CySysWdtUnlock();
		
		// Disable Counter 0 to allow modifications.
		CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
		
		// Reset Counter 0 and give ~3 LFCLK cycles to take effect.
		CySysWdtResetCounters(CY_SYS_WDT_COUNTER0_RESET);
		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
		
		// Write the Counter 0 match value for 20 millisecond and give 
		// ~3 LFCLK cycles to take effect.
		CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, WATCHDOG_COUNTER0_PERIOD);
		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
		
		// Enable Watchdog Counter 0.
		CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
		
		// Lock Watchdog to prevent any further change.
	    CySysWdtLock();
    }
    
    // If the Interrupt source is Counter 1 match, then process.
	else if(CySysWdtGetInterruptSource() & CY_SYS_WDT_COUNTER1_INT)
	{
        // Clear Watchdog Interrupt from Counter 0.
		CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER1_INT);
        
        /********************************************************************/
        
        // Stores the previous state of the BLE module.
        static uint8 previousState = 0xFF;
        
        // If state changed, turn all the LEDs off.
        if (CyBle_GetState() != previousState)
            _PrISM_TurnOffLED(TRUE, TRUE, TRUE);
            
        // If disconnected, turn all the LEDs off.
        if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED) {
            previousState = CYBLE_STATE_DISCONNECTED;
            
            _PrISM_TurnOffLED(TRUE, TRUE, TRUE);
        }
                
        // If in advertising mode, flash the red LED.
        else if(CyBle_GetState() == CYBLE_STATE_ADVERTISING) {
            previousState = CYBLE_STATE_ADVERTISING;
            
            if(_PrISM_Red_On)
                _PrISM_TurnOffLED(TRUE, FALSE, FALSE);
            else
                _PrISM_TurnOnLED(TRUE, FALSE, FALSE);
        }
        
        // If connected, turn the blue LED on.
        else if(CyBle_GetState() == CYBLE_STATE_CONNECTED) {
            previousState = CYBLE_STATE_CONNECTED;
            
            _PrISM_TurnOffLED(TRUE, TRUE, TRUE);
            _PrISM_TurnOnLED(FALSE, FALSE, TRUE);
        }
        
        /********************************************************************/
        
        // Unlock the WDT registers for modification.
		CySysWdtUnlock();
		
		// Disable Counter 1 to allow modifications.
		CySysWdtDisable(CY_SYS_WDT_COUNTER1_MASK);
		
		// Reset Counter 1 and give ~3 LFCLK cycles to take effect.
		CySysWdtResetCounters(CY_SYS_WDT_COUNTER1_RESET);
		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
		
		// Write the Counter 1 match value for 500 millisecond and give 
		// ~3 LFCLK cycles to take effect.
		CySysWdtWriteMatch(CY_SYS_WDT_COUNTER1, WATCHDOG_COUNTER1_PERIOD);
		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
		
		// Enable Watchdog Counter 1.
		CySysWdtEnable(CY_SYS_WDT_COUNTER1_MASK);
		
		// Lock Watchdog to prevent any further change.
	    CySysWdtLock();
    }
}

/* [] END OF FILE */
