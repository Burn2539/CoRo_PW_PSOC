/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _Timer.c
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
#include "_Timer.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
uint8 _Timer_DelayDone = FALSE;


/*****************************************************************************
* Function Name: _Timer_Init()
******************************************************************************
* Summary:
*   Initialize the timer and interrupt components.
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
void _Timer_Init(void)
{
    // Set the callback function of the TimerDelay interrupt.
    TimerDelay_Interrupt_StartEx(Delay);
}


/*****************************************************************************
* Interrupt Name: Delay
******************************************************************************
* Summary:
*   Can block the progression of the program if TimerDelay is started.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*   The timer's period is currently: 1ms
*****************************************************************************/
CY_ISR(Delay)
{
    // Allows to wait longer (by multiples of TimerDelay's period).
    static uint16 counter = 0;
    
    // Clear pending interrupt.
    TimerDelay_Interrupt_ClearPending();
    
    if (counter >= 100) {
        // Stop the timer.
        TimerDelay_Stop();
        
        // Set the TimerDelay_Done variable
        _Timer_DelayDone = TRUE;
        
        // Reset the counter
        counter = 0;
    }
    else
        counter++;
}

/* [] END OF FILE */
