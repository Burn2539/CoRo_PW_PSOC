/*******************************************************************************
* File Name: TimerDelay_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "TimerDelay.h"

static TimerDelay_backupStruct TimerDelay_backup;


/*******************************************************************************
* Function Name: TimerDelay_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TimerDelay_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void TimerDelay_SaveConfig(void) 
{
    #if (!TimerDelay_UsingFixedFunction)
        TimerDelay_backup.TimerUdb = TimerDelay_ReadCounter();
        TimerDelay_backup.InterruptMaskValue = TimerDelay_STATUS_MASK;
        #if (TimerDelay_UsingHWCaptureCounter)
            TimerDelay_backup.TimerCaptureCounter = TimerDelay_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!TimerDelay_UDB_CONTROL_REG_REMOVED)
            TimerDelay_backup.TimerControlRegister = TimerDelay_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: TimerDelay_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TimerDelay_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void TimerDelay_RestoreConfig(void) 
{   
    #if (!TimerDelay_UsingFixedFunction)

        TimerDelay_WriteCounter(TimerDelay_backup.TimerUdb);
        TimerDelay_STATUS_MASK =TimerDelay_backup.InterruptMaskValue;
        #if (TimerDelay_UsingHWCaptureCounter)
            TimerDelay_SetCaptureCount(TimerDelay_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!TimerDelay_UDB_CONTROL_REG_REMOVED)
            TimerDelay_WriteControlRegister(TimerDelay_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: TimerDelay_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TimerDelay_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void TimerDelay_Sleep(void) 
{
    #if(!TimerDelay_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(TimerDelay_CTRL_ENABLE == (TimerDelay_CONTROL & TimerDelay_CTRL_ENABLE))
        {
            /* Timer is enabled */
            TimerDelay_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            TimerDelay_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    TimerDelay_Stop();
    TimerDelay_SaveConfig();
}


/*******************************************************************************
* Function Name: TimerDelay_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  TimerDelay_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void TimerDelay_Wakeup(void) 
{
    TimerDelay_RestoreConfig();
    #if(!TimerDelay_UDB_CONTROL_REG_REMOVED)
        if(TimerDelay_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                TimerDelay_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
