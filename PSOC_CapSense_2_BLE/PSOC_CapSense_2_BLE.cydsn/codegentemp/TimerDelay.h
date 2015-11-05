/*******************************************************************************
* File Name: TimerDelay.h
* Version 2.70
*
*  Description:
*     Contains the function prototypes and constants available to the timer
*     user module.
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_Timer_v2_60_TimerDelay_H)
#define CY_Timer_v2_60_TimerDelay_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 TimerDelay_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Timer_v2_70 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define TimerDelay_Resolution                 16u
#define TimerDelay_UsingFixedFunction         0u
#define TimerDelay_UsingHWCaptureCounter      0u
#define TimerDelay_SoftwareCaptureMode        0u
#define TimerDelay_SoftwareTriggerMode        0u
#define TimerDelay_UsingHWEnable              0u
#define TimerDelay_EnableTriggerMode          0u
#define TimerDelay_InterruptOnCaptureCount    0u
#define TimerDelay_RunModeUsed                0u
#define TimerDelay_ControlRegRemoved          0u

#if defined(TimerDelay_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG)
    #define TimerDelay_UDB_CONTROL_REG_REMOVED            (0u)
#elif  (TimerDelay_UsingFixedFunction)
    #define TimerDelay_UDB_CONTROL_REG_REMOVED            (0u)
#else 
    #define TimerDelay_UDB_CONTROL_REG_REMOVED            (1u)
#endif /* End TimerDelay_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG */


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Wakeup Backup structure for Timer Component
 *************************************************************************/
typedef struct
{
    uint8 TimerEnableState;
    #if(!TimerDelay_UsingFixedFunction)

        uint16 TimerUdb;
        uint8 InterruptMaskValue;
        #if (TimerDelay_UsingHWCaptureCounter)
            uint8 TimerCaptureCounter;
        #endif /* variable declarations for backing up non retention registers in CY_UDB_V1 */

        #if (!TimerDelay_UDB_CONTROL_REG_REMOVED)
            uint8 TimerControlRegister;
        #endif /* variable declaration for backing up enable state of the Timer */
    #endif /* define backup variables only for UDB implementation. Fixed function registers are all retention */

}TimerDelay_backupStruct;


/***************************************
*       Function Prototypes
***************************************/

void    TimerDelay_Start(void) ;
void    TimerDelay_Stop(void) ;

void    TimerDelay_SetInterruptMode(uint8 interruptMode) ;
uint8   TimerDelay_ReadStatusRegister(void) ;
/* Deprecated function. Do not use this in future. Retained for backward compatibility */
#define TimerDelay_GetInterruptSource() TimerDelay_ReadStatusRegister()

#if(!TimerDelay_UDB_CONTROL_REG_REMOVED)
    uint8   TimerDelay_ReadControlRegister(void) ;
    void    TimerDelay_WriteControlRegister(uint8 control) ;
#endif /* (!TimerDelay_UDB_CONTROL_REG_REMOVED) */

uint16  TimerDelay_ReadPeriod(void) ;
void    TimerDelay_WritePeriod(uint16 period) ;
uint16  TimerDelay_ReadCounter(void) ;
void    TimerDelay_WriteCounter(uint16 counter) ;
uint16  TimerDelay_ReadCapture(void) ;
void    TimerDelay_SoftwareCapture(void) ;

#if(!TimerDelay_UsingFixedFunction) /* UDB Prototypes */
    #if (TimerDelay_SoftwareCaptureMode)
        void    TimerDelay_SetCaptureMode(uint8 captureMode) ;
    #endif /* (!TimerDelay_UsingFixedFunction) */

    #if (TimerDelay_SoftwareTriggerMode)
        void    TimerDelay_SetTriggerMode(uint8 triggerMode) ;
    #endif /* (TimerDelay_SoftwareTriggerMode) */

    #if (TimerDelay_EnableTriggerMode)
        void    TimerDelay_EnableTrigger(void) ;
        void    TimerDelay_DisableTrigger(void) ;
    #endif /* (TimerDelay_EnableTriggerMode) */


    #if(TimerDelay_InterruptOnCaptureCount)
        void    TimerDelay_SetInterruptCount(uint8 interruptCount) ;
    #endif /* (TimerDelay_InterruptOnCaptureCount) */

    #if (TimerDelay_UsingHWCaptureCounter)
        void    TimerDelay_SetCaptureCount(uint8 captureCount) ;
        uint8   TimerDelay_ReadCaptureCount(void) ;
    #endif /* (TimerDelay_UsingHWCaptureCounter) */

    void TimerDelay_ClearFIFO(void) ;
#endif /* UDB Prototypes */

/* Sleep Retention APIs */
void TimerDelay_Init(void)          ;
void TimerDelay_Enable(void)        ;
void TimerDelay_SaveConfig(void)    ;
void TimerDelay_RestoreConfig(void) ;
void TimerDelay_Sleep(void)         ;
void TimerDelay_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Timer__CaptureModes, Used in Capture Mode */
#define TimerDelay__B_TIMER__CM_NONE 0
#define TimerDelay__B_TIMER__CM_RISINGEDGE 1
#define TimerDelay__B_TIMER__CM_FALLINGEDGE 2
#define TimerDelay__B_TIMER__CM_EITHEREDGE 3
#define TimerDelay__B_TIMER__CM_SOFTWARE 4



/* Enumerated Type B_Timer__TriggerModes, Used in Trigger Mode */
#define TimerDelay__B_TIMER__TM_NONE 0x00u
#define TimerDelay__B_TIMER__TM_RISINGEDGE 0x04u
#define TimerDelay__B_TIMER__TM_FALLINGEDGE 0x08u
#define TimerDelay__B_TIMER__TM_EITHEREDGE 0x0Cu
#define TimerDelay__B_TIMER__TM_SOFTWARE 0x10u


/***************************************
*    Initialial Parameter Constants
***************************************/

#define TimerDelay_INIT_PERIOD             23999u
#define TimerDelay_INIT_CAPTURE_MODE       ((uint8)((uint8)0u << TimerDelay_CTRL_CAP_MODE_SHIFT))
#define TimerDelay_INIT_TRIGGER_MODE       ((uint8)((uint8)0u << TimerDelay_CTRL_TRIG_MODE_SHIFT))
#if (TimerDelay_UsingFixedFunction)
    #define TimerDelay_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << TimerDelay_STATUS_TC_INT_MASK_SHIFT)) | \
                                                  ((uint8)((uint8)0 << TimerDelay_STATUS_CAPTURE_INT_MASK_SHIFT)))
#else
    #define TimerDelay_INIT_INTERRUPT_MODE (((uint8)((uint8)0u << TimerDelay_STATUS_TC_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << TimerDelay_STATUS_CAPTURE_INT_MASK_SHIFT)) | \
                                                 ((uint8)((uint8)0 << TimerDelay_STATUS_FIFOFULL_INT_MASK_SHIFT)))
#endif /* (TimerDelay_UsingFixedFunction) */
#define TimerDelay_INIT_CAPTURE_COUNT      (2u)
#define TimerDelay_INIT_INT_CAPTURE_COUNT  ((uint8)((uint8)(1u - 1u) << TimerDelay_CTRL_INTCNT_SHIFT))


/***************************************
*           Registers
***************************************/

#if (TimerDelay_UsingFixedFunction) /* Implementation Specific Registers and Register Constants */


    /***************************************
    *    Fixed Function Registers
    ***************************************/

    #define TimerDelay_STATUS         (*(reg8 *) TimerDelay_TimerHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define TimerDelay_STATUS_MASK    (*(reg8 *) TimerDelay_TimerHW__SR0 )
    #define TimerDelay_CONTROL        (*(reg8 *) TimerDelay_TimerHW__CFG0)
    #define TimerDelay_CONTROL2       (*(reg8 *) TimerDelay_TimerHW__CFG1)
    #define TimerDelay_CONTROL2_PTR   ( (reg8 *) TimerDelay_TimerHW__CFG1)
    #define TimerDelay_RT1            (*(reg8 *) TimerDelay_TimerHW__RT1)
    #define TimerDelay_RT1_PTR        ( (reg8 *) TimerDelay_TimerHW__RT1)

    #if (CY_PSOC3 || CY_PSOC5LP)
        #define TimerDelay_CONTROL3       (*(reg8 *) TimerDelay_TimerHW__CFG2)
        #define TimerDelay_CONTROL3_PTR   ( (reg8 *) TimerDelay_TimerHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define TimerDelay_GLOBAL_ENABLE  (*(reg8 *) TimerDelay_TimerHW__PM_ACT_CFG)
    #define TimerDelay_GLOBAL_STBY_ENABLE  (*(reg8 *) TimerDelay_TimerHW__PM_STBY_CFG)

    #define TimerDelay_CAPTURE_LSB         (* (reg16 *) TimerDelay_TimerHW__CAP0 )
    #define TimerDelay_CAPTURE_LSB_PTR       ((reg16 *) TimerDelay_TimerHW__CAP0 )
    #define TimerDelay_PERIOD_LSB          (* (reg16 *) TimerDelay_TimerHW__PER0 )
    #define TimerDelay_PERIOD_LSB_PTR        ((reg16 *) TimerDelay_TimerHW__PER0 )
    #define TimerDelay_COUNTER_LSB         (* (reg16 *) TimerDelay_TimerHW__CNT_CMP0 )
    #define TimerDelay_COUNTER_LSB_PTR       ((reg16 *) TimerDelay_TimerHW__CNT_CMP0 )


    /***************************************
    *    Register Constants
    ***************************************/

    /* Fixed Function Block Chosen */
    #define TimerDelay_BLOCK_EN_MASK                     TimerDelay_TimerHW__PM_ACT_MSK
    #define TimerDelay_BLOCK_STBY_EN_MASK                TimerDelay_TimerHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* Interrupt Count - Not valid for Fixed Function Block */
    #define TimerDelay_CTRL_INTCNT_SHIFT                  0x00u
    /* Trigger Polarity - Not valid for Fixed Function Block */
    #define TimerDelay_CTRL_TRIG_MODE_SHIFT               0x00u
    /* Trigger Enable - Not valid for Fixed Function Block */
    #define TimerDelay_CTRL_TRIG_EN_SHIFT                 0x00u
    /* Capture Polarity - Not valid for Fixed Function Block */
    #define TimerDelay_CTRL_CAP_MODE_SHIFT                0x00u
    /* Timer Enable - As defined in Register Map, part of TMRX_CFG0 register */
    #define TimerDelay_CTRL_ENABLE_SHIFT                  0x00u

    /* Control Register Bit Masks */
    #define TimerDelay_CTRL_ENABLE                        ((uint8)((uint8)0x01u << TimerDelay_CTRL_ENABLE_SHIFT))

    /* Control2 Register Bit Masks */
    /* As defined in Register Map, Part of the TMRX_CFG1 register */
    #define TimerDelay_CTRL2_IRQ_SEL_SHIFT                 0x00u
    #define TimerDelay_CTRL2_IRQ_SEL                      ((uint8)((uint8)0x01u << TimerDelay_CTRL2_IRQ_SEL_SHIFT))

    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        /* As defined by Verilog Implementation */
        #define TimerDelay_CTRL_MODE_SHIFT                 0x01u
        #define TimerDelay_CTRL_MODE_MASK                 ((uint8)((uint8)0x07u << TimerDelay_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Control3 Register Bit Locations */
        #define TimerDelay_CTRL_RCOD_SHIFT        0x02u
        #define TimerDelay_CTRL_ENBL_SHIFT        0x00u
        #define TimerDelay_CTRL_MODE_SHIFT        0x00u

        /* Control3 Register Bit Masks */
        #define TimerDelay_CTRL_RCOD_MASK  ((uint8)((uint8)0x03u << TimerDelay_CTRL_RCOD_SHIFT)) /* ROD and COD bit masks */
        #define TimerDelay_CTRL_ENBL_MASK  ((uint8)((uint8)0x80u << TimerDelay_CTRL_ENBL_SHIFT)) /* HW_EN bit mask */
        #define TimerDelay_CTRL_MODE_MASK  ((uint8)((uint8)0x03u << TimerDelay_CTRL_MODE_SHIFT)) /* Run mode bit mask */

        #define TimerDelay_CTRL_RCOD       ((uint8)((uint8)0x03u << TimerDelay_CTRL_RCOD_SHIFT))
        #define TimerDelay_CTRL_ENBL       ((uint8)((uint8)0x80u << TimerDelay_CTRL_ENBL_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define TimerDelay_RT1_SHIFT                       0x04u
    /* Sync TC and CMP bit masks */
    #define TimerDelay_RT1_MASK                        ((uint8)((uint8)0x03u << TimerDelay_RT1_SHIFT))
    #define TimerDelay_SYNC                            ((uint8)((uint8)0x03u << TimerDelay_RT1_SHIFT))
    #define TimerDelay_SYNCDSI_SHIFT                   0x00u
    /* Sync all DSI inputs with Mask  */
    #define TimerDelay_SYNCDSI_MASK                    ((uint8)((uint8)0x0Fu << TimerDelay_SYNCDSI_SHIFT))
    /* Sync all DSI inputs */
    #define TimerDelay_SYNCDSI_EN                      ((uint8)((uint8)0x0Fu << TimerDelay_SYNCDSI_SHIFT))

    #define TimerDelay_CTRL_MODE_PULSEWIDTH            ((uint8)((uint8)0x01u << TimerDelay_CTRL_MODE_SHIFT))
    #define TimerDelay_CTRL_MODE_PERIOD                ((uint8)((uint8)0x02u << TimerDelay_CTRL_MODE_SHIFT))
    #define TimerDelay_CTRL_MODE_CONTINUOUS            ((uint8)((uint8)0x00u << TimerDelay_CTRL_MODE_SHIFT))

    /* Status Register Bit Locations */
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define TimerDelay_STATUS_TC_SHIFT                 0x07u
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define TimerDelay_STATUS_CAPTURE_SHIFT            0x06u
    /* As defined in Register Map, part of TMRX_SR0 register */
    #define TimerDelay_STATUS_TC_INT_MASK_SHIFT        (TimerDelay_STATUS_TC_SHIFT - 0x04u)
    /* As defined in Register Map, part of TMRX_SR0 register, Shared with Compare Status */
    #define TimerDelay_STATUS_CAPTURE_INT_MASK_SHIFT   (TimerDelay_STATUS_CAPTURE_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define TimerDelay_STATUS_TC                       ((uint8)((uint8)0x01u << TimerDelay_STATUS_TC_SHIFT))
    #define TimerDelay_STATUS_CAPTURE                  ((uint8)((uint8)0x01u << TimerDelay_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on TC */
    #define TimerDelay_STATUS_TC_INT_MASK              ((uint8)((uint8)0x01u << TimerDelay_STATUS_TC_INT_MASK_SHIFT))
    /* Interrupt Enable Bit-Mask for interrupt on Capture */
    #define TimerDelay_STATUS_CAPTURE_INT_MASK         ((uint8)((uint8)0x01u << TimerDelay_STATUS_CAPTURE_INT_MASK_SHIFT))

#else   /* UDB Registers and Register Constants */


    /***************************************
    *           UDB Registers
    ***************************************/

    #define TimerDelay_STATUS              (* (reg8 *) TimerDelay_TimerUDB_rstSts_stsreg__STATUS_REG )
    #define TimerDelay_STATUS_MASK         (* (reg8 *) TimerDelay_TimerUDB_rstSts_stsreg__MASK_REG)
    #define TimerDelay_STATUS_AUX_CTRL     (* (reg8 *) TimerDelay_TimerUDB_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define TimerDelay_CONTROL             (* (reg8 *) TimerDelay_TimerUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    
    #if(TimerDelay_Resolution <= 8u) /* 8-bit Timer */
        #define TimerDelay_CAPTURE_LSB         (* (reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TimerDelay_CAPTURE_LSB_PTR       ((reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TimerDelay_PERIOD_LSB          (* (reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TimerDelay_PERIOD_LSB_PTR        ((reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TimerDelay_COUNTER_LSB         (* (reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define TimerDelay_COUNTER_LSB_PTR       ((reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
    #elif(TimerDelay_Resolution <= 16u) /* 8-bit Timer */
        #if(CY_PSOC3) /* 8-bit addres space */
            #define TimerDelay_CAPTURE_LSB         (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TimerDelay_CAPTURE_LSB_PTR       ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TimerDelay_PERIOD_LSB          (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TimerDelay_PERIOD_LSB_PTR        ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TimerDelay_COUNTER_LSB         (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define TimerDelay_COUNTER_LSB_PTR       ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define TimerDelay_CAPTURE_LSB         (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define TimerDelay_CAPTURE_LSB_PTR       ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_F0_REG )
            #define TimerDelay_PERIOD_LSB          (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define TimerDelay_PERIOD_LSB_PTR        ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_D0_REG )
            #define TimerDelay_COUNTER_LSB         (* (reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
            #define TimerDelay_COUNTER_LSB_PTR       ((reg16 *) TimerDelay_TimerUDB_sT16_timerdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(TimerDelay_Resolution <= 24u)/* 24-bit Timer */
        #define TimerDelay_CAPTURE_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TimerDelay_CAPTURE_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
        #define TimerDelay_PERIOD_LSB          (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TimerDelay_PERIOD_LSB_PTR        ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
        #define TimerDelay_COUNTER_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
        #define TimerDelay_COUNTER_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
    #else /* 32-bit Timer */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define TimerDelay_CAPTURE_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TimerDelay_CAPTURE_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__F0_REG )
            #define TimerDelay_PERIOD_LSB          (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TimerDelay_PERIOD_LSB_PTR        ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__D0_REG )
            #define TimerDelay_COUNTER_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
            #define TimerDelay_COUNTER_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define TimerDelay_CAPTURE_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define TimerDelay_CAPTURE_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_F0_REG )
            #define TimerDelay_PERIOD_LSB          (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define TimerDelay_PERIOD_LSB_PTR        ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_D0_REG )
            #define TimerDelay_COUNTER_LSB         (* (reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
            #define TimerDelay_COUNTER_LSB_PTR       ((reg32 *) TimerDelay_TimerUDB_sT16_timerdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */ 
    #endif

    #define TimerDelay_COUNTER_LSB_PTR_8BIT       ((reg8 *) TimerDelay_TimerUDB_sT16_timerdp_u0__A0_REG )
    
    #if (TimerDelay_UsingHWCaptureCounter)
        #define TimerDelay_CAP_COUNT              (*(reg8 *) TimerDelay_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define TimerDelay_CAP_COUNT_PTR          ( (reg8 *) TimerDelay_TimerUDB_sCapCount_counter__PERIOD_REG )
        #define TimerDelay_CAPTURE_COUNT_CTRL     (*(reg8 *) TimerDelay_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
        #define TimerDelay_CAPTURE_COUNT_CTRL_PTR ( (reg8 *) TimerDelay_TimerUDB_sCapCount_counter__CONTROL_AUX_CTL_REG )
    #endif /* (TimerDelay_UsingHWCaptureCounter) */


    /***************************************
    *       Register Constants
    ***************************************/

    /* Control Register Bit Locations */
    #define TimerDelay_CTRL_INTCNT_SHIFT              0x00u       /* As defined by Verilog Implementation */
    #define TimerDelay_CTRL_TRIG_MODE_SHIFT           0x02u       /* As defined by Verilog Implementation */
    #define TimerDelay_CTRL_TRIG_EN_SHIFT             0x04u       /* As defined by Verilog Implementation */
    #define TimerDelay_CTRL_CAP_MODE_SHIFT            0x05u       /* As defined by Verilog Implementation */
    #define TimerDelay_CTRL_ENABLE_SHIFT              0x07u       /* As defined by Verilog Implementation */

    /* Control Register Bit Masks */
    #define TimerDelay_CTRL_INTCNT_MASK               ((uint8)((uint8)0x03u << TimerDelay_CTRL_INTCNT_SHIFT))
    #define TimerDelay_CTRL_TRIG_MODE_MASK            ((uint8)((uint8)0x03u << TimerDelay_CTRL_TRIG_MODE_SHIFT))
    #define TimerDelay_CTRL_TRIG_EN                   ((uint8)((uint8)0x01u << TimerDelay_CTRL_TRIG_EN_SHIFT))
    #define TimerDelay_CTRL_CAP_MODE_MASK             ((uint8)((uint8)0x03u << TimerDelay_CTRL_CAP_MODE_SHIFT))
    #define TimerDelay_CTRL_ENABLE                    ((uint8)((uint8)0x01u << TimerDelay_CTRL_ENABLE_SHIFT))

    /* Bit Counter (7-bit) Control Register Bit Definitions */
    /* As defined by the Register map for the AUX Control Register */
    #define TimerDelay_CNTR_ENABLE                    0x20u

    /* Status Register Bit Locations */
    #define TimerDelay_STATUS_TC_SHIFT                0x00u  /* As defined by Verilog Implementation */
    #define TimerDelay_STATUS_CAPTURE_SHIFT           0x01u  /* As defined by Verilog Implementation */
    #define TimerDelay_STATUS_TC_INT_MASK_SHIFT       TimerDelay_STATUS_TC_SHIFT
    #define TimerDelay_STATUS_CAPTURE_INT_MASK_SHIFT  TimerDelay_STATUS_CAPTURE_SHIFT
    #define TimerDelay_STATUS_FIFOFULL_SHIFT          0x02u  /* As defined by Verilog Implementation */
    #define TimerDelay_STATUS_FIFONEMP_SHIFT          0x03u  /* As defined by Verilog Implementation */
    #define TimerDelay_STATUS_FIFOFULL_INT_MASK_SHIFT TimerDelay_STATUS_FIFOFULL_SHIFT

    /* Status Register Bit Masks */
    /* Sticky TC Event Bit-Mask */
    #define TimerDelay_STATUS_TC                      ((uint8)((uint8)0x01u << TimerDelay_STATUS_TC_SHIFT))
    /* Sticky Capture Event Bit-Mask */
    #define TimerDelay_STATUS_CAPTURE                 ((uint8)((uint8)0x01u << TimerDelay_STATUS_CAPTURE_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TimerDelay_STATUS_TC_INT_MASK             ((uint8)((uint8)0x01u << TimerDelay_STATUS_TC_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TimerDelay_STATUS_CAPTURE_INT_MASK        ((uint8)((uint8)0x01u << TimerDelay_STATUS_CAPTURE_SHIFT))
    /* NOT-Sticky FIFO Full Bit-Mask */
    #define TimerDelay_STATUS_FIFOFULL                ((uint8)((uint8)0x01u << TimerDelay_STATUS_FIFOFULL_SHIFT))
    /* NOT-Sticky FIFO Not Empty Bit-Mask */
    #define TimerDelay_STATUS_FIFONEMP                ((uint8)((uint8)0x01u << TimerDelay_STATUS_FIFONEMP_SHIFT))
    /* Interrupt Enable Bit-Mask */
    #define TimerDelay_STATUS_FIFOFULL_INT_MASK       ((uint8)((uint8)0x01u << TimerDelay_STATUS_FIFOFULL_SHIFT))

    #define TimerDelay_STATUS_ACTL_INT_EN             0x10u   /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define TimerDelay_AUX_CTRL_FIFO0_CLR             0x01u   /* As defined by Register map */
    #define TimerDelay_AUX_CTRL_FIFO1_CLR             0x02u   /* As defined by Register map */
    #define TimerDelay_AUX_CTRL_FIFO0_LVL             0x04u   /* As defined by Register map */
    #define TimerDelay_AUX_CTRL_FIFO1_LVL             0x08u   /* As defined by Register map */
    #define TimerDelay_STATUS_ACTL_INT_EN_MASK        0x10u   /* As defined for the ACTL Register */

#endif /* Implementation Specific Registers and Register Constants */

#endif  /* CY_Timer_v2_30_TimerDelay_H */


/* [] END OF FILE */
