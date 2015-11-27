/*******************************************************************************
* File Name: USB_UART.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_USB_UART_H)
#define CY_UART_USB_UART_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define USB_UART_RX_ENABLED                     (0u)
#define USB_UART_TX_ENABLED                     (1u)
#define USB_UART_HD_ENABLED                     (0u)
#define USB_UART_RX_INTERRUPT_ENABLED           (0u)
#define USB_UART_TX_INTERRUPT_ENABLED           (0u)
#define USB_UART_INTERNAL_CLOCK_USED            (1u)
#define USB_UART_RXHW_ADDRESS_ENABLED           (0u)
#define USB_UART_OVER_SAMPLE_COUNT              (8u)
#define USB_UART_PARITY_TYPE                    (0u)
#define USB_UART_PARITY_TYPE_SW                 (0u)
#define USB_UART_BREAK_DETECT                   (0u)
#define USB_UART_BREAK_BITS_TX                  (13u)
#define USB_UART_BREAK_BITS_RX                  (13u)
#define USB_UART_TXCLKGEN_DP                    (1u)
#define USB_UART_USE23POLLING                   (1u)
#define USB_UART_FLOW_CONTROL                   (0u)
#define USB_UART_CLK_FREQ                       (0u)
#define USB_UART_TX_BUFFER_SIZE                 (4u)
#define USB_UART_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define USB_UART_CONTROL_REG_REMOVED            (0u)
#else
    #define USB_UART_CONTROL_REG_REMOVED            (1u)
#endif /* End USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct USB_UART_backupStruct_
{
    uint8 enableState;

    #if(USB_UART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End USB_UART_CONTROL_REG_REMOVED */

} USB_UART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void USB_UART_Start(void) ;
void USB_UART_Stop(void) ;
uint8 USB_UART_ReadControlRegister(void) ;
void USB_UART_WriteControlRegister(uint8 control) ;

void USB_UART_Init(void) ;
void USB_UART_Enable(void) ;
void USB_UART_SaveConfig(void) ;
void USB_UART_RestoreConfig(void) ;
void USB_UART_Sleep(void) ;
void USB_UART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) )

    #if (USB_UART_RX_INTERRUPT_ENABLED)
        #define USB_UART_EnableRxInt()  CyIntEnable (USB_UART_RX_VECT_NUM)
        #define USB_UART_DisableRxInt() CyIntDisable(USB_UART_RX_VECT_NUM)
        CY_ISR_PROTO(USB_UART_RXISR);
    #endif /* USB_UART_RX_INTERRUPT_ENABLED */

    void USB_UART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void USB_UART_SetRxAddress1(uint8 address) ;
    void USB_UART_SetRxAddress2(uint8 address) ;

    void  USB_UART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 USB_UART_ReadRxData(void) ;
    uint8 USB_UART_ReadRxStatus(void) ;
    uint8 USB_UART_GetChar(void) ;
    uint16 USB_UART_GetByte(void) ;
    uint8 USB_UART_GetRxBufferSize(void)
                                                            ;
    void USB_UART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define USB_UART_GetRxInterruptSource   USB_UART_ReadRxStatus

#endif /* End (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) */

/* Only if TX is enabled */
#if(USB_UART_TX_ENABLED || USB_UART_HD_ENABLED)

    #if(USB_UART_TX_INTERRUPT_ENABLED)
        #define USB_UART_EnableTxInt()  CyIntEnable (USB_UART_TX_VECT_NUM)
        #define USB_UART_DisableTxInt() CyIntDisable(USB_UART_TX_VECT_NUM)
        #define USB_UART_SetPendingTxInt() CyIntSetPending(USB_UART_TX_VECT_NUM)
        #define USB_UART_ClearPendingTxInt() CyIntClearPending(USB_UART_TX_VECT_NUM)
        CY_ISR_PROTO(USB_UART_TXISR);
    #endif /* USB_UART_TX_INTERRUPT_ENABLED */

    void USB_UART_SetTxInterruptMode(uint8 intSrc) ;
    void USB_UART_WriteTxData(uint8 txDataByte) ;
    uint8 USB_UART_ReadTxStatus(void) ;
    void USB_UART_PutChar(uint8 txDataByte) ;
    void USB_UART_PutString(const char8 string[]) ;
    void USB_UART_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void USB_UART_PutCRLF(uint8 txDataByte) ;
    void USB_UART_ClearTxBuffer(void) ;
    void USB_UART_SetTxAddressMode(uint8 addressMode) ;
    void USB_UART_SendBreak(uint8 retMode) ;
    uint8 USB_UART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define USB_UART_PutStringConst         USB_UART_PutString
    #define USB_UART_PutArrayConst          USB_UART_PutArray
    #define USB_UART_GetTxInterruptSource   USB_UART_ReadTxStatus

#endif /* End USB_UART_TX_ENABLED || USB_UART_HD_ENABLED */

#if(USB_UART_HD_ENABLED)
    void USB_UART_LoadRxConfig(void) ;
    void USB_UART_LoadTxConfig(void) ;
#endif /* End USB_UART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USB_UART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    USB_UART_CyBtldrCommStart(void) CYSMALL ;
    void    USB_UART_CyBtldrCommStop(void) CYSMALL ;
    void    USB_UART_CyBtldrCommReset(void) CYSMALL ;
    cystatus USB_UART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus USB_UART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USB_UART)
        #define CyBtldrCommStart    USB_UART_CyBtldrCommStart
        #define CyBtldrCommStop     USB_UART_CyBtldrCommStop
        #define CyBtldrCommReset    USB_UART_CyBtldrCommReset
        #define CyBtldrCommWrite    USB_UART_CyBtldrCommWrite
        #define CyBtldrCommRead     USB_UART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USB_UART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define USB_UART_BYTE2BYTE_TIME_OUT (25u)
    #define USB_UART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define USB_UART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define USB_UART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define USB_UART_SET_SPACE      (0x00u)
#define USB_UART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (USB_UART_TX_ENABLED) || (USB_UART_HD_ENABLED) )
    #if(USB_UART_TX_INTERRUPT_ENABLED)
        #define USB_UART_TX_VECT_NUM            (uint8)USB_UART_TXInternalInterrupt__INTC_NUMBER
        #define USB_UART_TX_PRIOR_NUM           (uint8)USB_UART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* USB_UART_TX_INTERRUPT_ENABLED */

    #define USB_UART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define USB_UART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define USB_UART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(USB_UART_TX_ENABLED)
        #define USB_UART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (USB_UART_HD_ENABLED) */
        #define USB_UART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (USB_UART_TX_ENABLED) */

    #define USB_UART_TX_STS_COMPLETE            (uint8)(0x01u << USB_UART_TX_STS_COMPLETE_SHIFT)
    #define USB_UART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << USB_UART_TX_STS_FIFO_EMPTY_SHIFT)
    #define USB_UART_TX_STS_FIFO_FULL           (uint8)(0x01u << USB_UART_TX_STS_FIFO_FULL_SHIFT)
    #define USB_UART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << USB_UART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (USB_UART_TX_ENABLED) || (USB_UART_HD_ENABLED)*/

#if( (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) )
    #if(USB_UART_RX_INTERRUPT_ENABLED)
        #define USB_UART_RX_VECT_NUM            (uint8)USB_UART_RXInternalInterrupt__INTC_NUMBER
        #define USB_UART_RX_PRIOR_NUM           (uint8)USB_UART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* USB_UART_RX_INTERRUPT_ENABLED */
    #define USB_UART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define USB_UART_RX_STS_BREAK_SHIFT             (0x01u)
    #define USB_UART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define USB_UART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define USB_UART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define USB_UART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define USB_UART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define USB_UART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define USB_UART_RX_STS_MRKSPC           (uint8)(0x01u << USB_UART_RX_STS_MRKSPC_SHIFT)
    #define USB_UART_RX_STS_BREAK            (uint8)(0x01u << USB_UART_RX_STS_BREAK_SHIFT)
    #define USB_UART_RX_STS_PAR_ERROR        (uint8)(0x01u << USB_UART_RX_STS_PAR_ERROR_SHIFT)
    #define USB_UART_RX_STS_STOP_ERROR       (uint8)(0x01u << USB_UART_RX_STS_STOP_ERROR_SHIFT)
    #define USB_UART_RX_STS_OVERRUN          (uint8)(0x01u << USB_UART_RX_STS_OVERRUN_SHIFT)
    #define USB_UART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << USB_UART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define USB_UART_RX_STS_ADDR_MATCH       (uint8)(0x01u << USB_UART_RX_STS_ADDR_MATCH_SHIFT)
    #define USB_UART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << USB_UART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define USB_UART_RX_HW_MASK                     (0x7Fu)
#endif /* End (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) */

/* Control Register definitions */
#define USB_UART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define USB_UART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define USB_UART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define USB_UART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define USB_UART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define USB_UART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define USB_UART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define USB_UART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define USB_UART_CTRL_HD_SEND               (uint8)(0x01u << USB_UART_CTRL_HD_SEND_SHIFT)
#define USB_UART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << USB_UART_CTRL_HD_SEND_BREAK_SHIFT)
#define USB_UART_CTRL_MARK                  (uint8)(0x01u << USB_UART_CTRL_MARK_SHIFT)
#define USB_UART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << USB_UART_CTRL_PARITY_TYPE0_SHIFT)
#define USB_UART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << USB_UART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define USB_UART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define USB_UART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define USB_UART_SEND_BREAK                         (0x00u)
#define USB_UART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define USB_UART_REINIT                             (0x02u)
#define USB_UART_SEND_WAIT_REINIT                   (0x03u)

#define USB_UART_OVER_SAMPLE_8                      (8u)
#define USB_UART_OVER_SAMPLE_16                     (16u)

#define USB_UART_BIT_CENTER                         (USB_UART_OVER_SAMPLE_COUNT - 2u)

#define USB_UART_FIFO_LENGTH                        (4u)
#define USB_UART_NUMBER_OF_START_BIT                (1u)
#define USB_UART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define USB_UART_TXBITCTR_BREAKBITS8X   ((USB_UART_BREAK_BITS_TX * USB_UART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define USB_UART_TXBITCTR_BREAKBITS ((USB_UART_BREAK_BITS_TX * USB_UART_OVER_SAMPLE_COUNT) - 1u)

#define USB_UART_HALF_BIT_COUNT   \
                            (((USB_UART_OVER_SAMPLE_COUNT / 2u) + (USB_UART_USE23POLLING * 1u)) - 2u)
#if (USB_UART_OVER_SAMPLE_COUNT == USB_UART_OVER_SAMPLE_8)
    #define USB_UART_HD_TXBITCTR_INIT   (((USB_UART_BREAK_BITS_TX + \
                            USB_UART_NUMBER_OF_START_BIT) * USB_UART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define USB_UART_RXBITCTR_INIT  ((((USB_UART_BREAK_BITS_RX + USB_UART_NUMBER_OF_START_BIT) \
                            * USB_UART_OVER_SAMPLE_COUNT) + USB_UART_HALF_BIT_COUNT) - 1u)

#else /* USB_UART_OVER_SAMPLE_COUNT == USB_UART_OVER_SAMPLE_16 */
    #define USB_UART_HD_TXBITCTR_INIT   ((8u * USB_UART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define USB_UART_RXBITCTR_INIT      (((7u * USB_UART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      USB_UART_HALF_BIT_COUNT)
#endif /* End USB_UART_OVER_SAMPLE_COUNT */

#define USB_UART_HD_RXBITCTR_INIT                   USB_UART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 USB_UART_initVar;
#if (USB_UART_TX_INTERRUPT_ENABLED && USB_UART_TX_ENABLED)
    extern volatile uint8 USB_UART_txBuffer[USB_UART_TX_BUFFER_SIZE];
    extern volatile uint8 USB_UART_txBufferRead;
    extern uint8 USB_UART_txBufferWrite;
#endif /* (USB_UART_TX_INTERRUPT_ENABLED && USB_UART_TX_ENABLED) */
#if (USB_UART_RX_INTERRUPT_ENABLED && (USB_UART_RX_ENABLED || USB_UART_HD_ENABLED))
    extern uint8 USB_UART_errorStatus;
    extern volatile uint8 USB_UART_rxBuffer[USB_UART_RX_BUFFER_SIZE];
    extern volatile uint8 USB_UART_rxBufferRead;
    extern volatile uint8 USB_UART_rxBufferWrite;
    extern volatile uint8 USB_UART_rxBufferLoopDetect;
    extern volatile uint8 USB_UART_rxBufferOverflow;
    #if (USB_UART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 USB_UART_rxAddressMode;
        extern volatile uint8 USB_UART_rxAddressDetected;
    #endif /* (USB_UART_RXHW_ADDRESS_ENABLED) */
#endif /* (USB_UART_RX_INTERRUPT_ENABLED && (USB_UART_RX_ENABLED || USB_UART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define USB_UART__B_UART__AM_SW_BYTE_BYTE 1
#define USB_UART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define USB_UART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define USB_UART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define USB_UART__B_UART__AM_NONE 0

#define USB_UART__B_UART__NONE_REVB 0
#define USB_UART__B_UART__EVEN_REVB 1
#define USB_UART__B_UART__ODD_REVB 2
#define USB_UART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define USB_UART_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define USB_UART_NUMBER_OF_STOP_BITS    (1u)

#if (USB_UART_RXHW_ADDRESS_ENABLED)
    #define USB_UART_RX_ADDRESS_MODE    (0u)
    #define USB_UART_RX_HW_ADDRESS1     (0u)
    #define USB_UART_RX_HW_ADDRESS2     (0u)
#endif /* (USB_UART_RXHW_ADDRESS_ENABLED) */

#define USB_UART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((0 << USB_UART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << USB_UART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << USB_UART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << USB_UART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << USB_UART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << USB_UART_RX_STS_BREAK_SHIFT) \
                                        | (0 << USB_UART_RX_STS_OVERRUN_SHIFT))

#define USB_UART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << USB_UART_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << USB_UART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << USB_UART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << USB_UART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define USB_UART_CONTROL_REG \
                            (* (reg8 *) USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define USB_UART_CONTROL_PTR \
                            (  (reg8 *) USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(USB_UART_TX_ENABLED)
    #define USB_UART_TXDATA_REG          (* (reg8 *) USB_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define USB_UART_TXDATA_PTR          (  (reg8 *) USB_UART_BUART_sTX_TxShifter_u0__F0_REG)
    #define USB_UART_TXDATA_AUX_CTL_REG  (* (reg8 *) USB_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define USB_UART_TXDATA_AUX_CTL_PTR  (  (reg8 *) USB_UART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define USB_UART_TXSTATUS_REG        (* (reg8 *) USB_UART_BUART_sTX_TxSts__STATUS_REG)
    #define USB_UART_TXSTATUS_PTR        (  (reg8 *) USB_UART_BUART_sTX_TxSts__STATUS_REG)
    #define USB_UART_TXSTATUS_MASK_REG   (* (reg8 *) USB_UART_BUART_sTX_TxSts__MASK_REG)
    #define USB_UART_TXSTATUS_MASK_PTR   (  (reg8 *) USB_UART_BUART_sTX_TxSts__MASK_REG)
    #define USB_UART_TXSTATUS_ACTL_REG   (* (reg8 *) USB_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define USB_UART_TXSTATUS_ACTL_PTR   (  (reg8 *) USB_UART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(USB_UART_TXCLKGEN_DP)
        #define USB_UART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define USB_UART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define USB_UART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define USB_UART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define USB_UART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define USB_UART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define USB_UART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define USB_UART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define USB_UART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define USB_UART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) USB_UART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* USB_UART_TXCLKGEN_DP */

#endif /* End USB_UART_TX_ENABLED */

#if(USB_UART_HD_ENABLED)

    #define USB_UART_TXDATA_REG             (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define USB_UART_TXDATA_PTR             (  (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__F1_REG )
    #define USB_UART_TXDATA_AUX_CTL_REG     (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define USB_UART_TXDATA_AUX_CTL_PTR     (  (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define USB_UART_TXSTATUS_REG           (* (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_REG )
    #define USB_UART_TXSTATUS_PTR           (  (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_REG )
    #define USB_UART_TXSTATUS_MASK_REG      (* (reg8 *) USB_UART_BUART_sRX_RxSts__MASK_REG )
    #define USB_UART_TXSTATUS_MASK_PTR      (  (reg8 *) USB_UART_BUART_sRX_RxSts__MASK_REG )
    #define USB_UART_TXSTATUS_ACTL_REG      (* (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define USB_UART_TXSTATUS_ACTL_PTR      (  (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End USB_UART_HD_ENABLED */

#if( (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) )
    #define USB_UART_RXDATA_REG             (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define USB_UART_RXDATA_PTR             (  (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__F0_REG )
    #define USB_UART_RXADDRESS1_REG         (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define USB_UART_RXADDRESS1_PTR         (  (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__D0_REG )
    #define USB_UART_RXADDRESS2_REG         (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define USB_UART_RXADDRESS2_PTR         (  (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__D1_REG )
    #define USB_UART_RXDATA_AUX_CTL_REG     (* (reg8 *) USB_UART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define USB_UART_RXBITCTR_PERIOD_REG    (* (reg8 *) USB_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define USB_UART_RXBITCTR_PERIOD_PTR    (  (reg8 *) USB_UART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define USB_UART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) USB_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define USB_UART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) USB_UART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define USB_UART_RXBITCTR_COUNTER_REG   (* (reg8 *) USB_UART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define USB_UART_RXBITCTR_COUNTER_PTR   (  (reg8 *) USB_UART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define USB_UART_RXSTATUS_REG           (* (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_REG )
    #define USB_UART_RXSTATUS_PTR           (  (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_REG )
    #define USB_UART_RXSTATUS_MASK_REG      (* (reg8 *) USB_UART_BUART_sRX_RxSts__MASK_REG )
    #define USB_UART_RXSTATUS_MASK_PTR      (  (reg8 *) USB_UART_BUART_sRX_RxSts__MASK_REG )
    #define USB_UART_RXSTATUS_ACTL_REG      (* (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define USB_UART_RXSTATUS_ACTL_PTR      (  (reg8 *) USB_UART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) */

#if(USB_UART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define USB_UART_INTCLOCK_CLKEN_REG     (* (reg8 *) USB_UART_IntClock__PM_ACT_CFG)
    #define USB_UART_INTCLOCK_CLKEN_PTR     (  (reg8 *) USB_UART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define USB_UART_INTCLOCK_CLKEN_MASK    USB_UART_IntClock__PM_ACT_MSK
#endif /* End USB_UART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(USB_UART_TX_ENABLED)
    #define USB_UART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End USB_UART_TX_ENABLED */

#if(USB_UART_HD_ENABLED)
    #define USB_UART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End USB_UART_HD_ENABLED */

#if( (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) )
    #define USB_UART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define USB_UART_WAIT_1_MS      USB_UART_BL_CHK_DELAY_MS   

#define USB_UART_TXBUFFERSIZE   USB_UART_TX_BUFFER_SIZE
#define USB_UART_RXBUFFERSIZE   USB_UART_RX_BUFFER_SIZE

#if (USB_UART_RXHW_ADDRESS_ENABLED)
    #define USB_UART_RXADDRESSMODE  USB_UART_RX_ADDRESS_MODE
    #define USB_UART_RXHWADDRESS1   USB_UART_RX_HW_ADDRESS1
    #define USB_UART_RXHWADDRESS2   USB_UART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define USB_UART_RXAddressMode  USB_UART_RXADDRESSMODE
#endif /* (USB_UART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define USB_UART_initvar                    USB_UART_initVar

#define USB_UART_RX_Enabled                 USB_UART_RX_ENABLED
#define USB_UART_TX_Enabled                 USB_UART_TX_ENABLED
#define USB_UART_HD_Enabled                 USB_UART_HD_ENABLED
#define USB_UART_RX_IntInterruptEnabled     USB_UART_RX_INTERRUPT_ENABLED
#define USB_UART_TX_IntInterruptEnabled     USB_UART_TX_INTERRUPT_ENABLED
#define USB_UART_InternalClockUsed          USB_UART_INTERNAL_CLOCK_USED
#define USB_UART_RXHW_Address_Enabled       USB_UART_RXHW_ADDRESS_ENABLED
#define USB_UART_OverSampleCount            USB_UART_OVER_SAMPLE_COUNT
#define USB_UART_ParityType                 USB_UART_PARITY_TYPE

#if( USB_UART_TX_ENABLED && (USB_UART_TXBUFFERSIZE > USB_UART_FIFO_LENGTH))
    #define USB_UART_TXBUFFER               USB_UART_txBuffer
    #define USB_UART_TXBUFFERREAD           USB_UART_txBufferRead
    #define USB_UART_TXBUFFERWRITE          USB_UART_txBufferWrite
#endif /* End USB_UART_TX_ENABLED */
#if( ( USB_UART_RX_ENABLED || USB_UART_HD_ENABLED ) && \
     (USB_UART_RXBUFFERSIZE > USB_UART_FIFO_LENGTH) )
    #define USB_UART_RXBUFFER               USB_UART_rxBuffer
    #define USB_UART_RXBUFFERREAD           USB_UART_rxBufferRead
    #define USB_UART_RXBUFFERWRITE          USB_UART_rxBufferWrite
    #define USB_UART_RXBUFFERLOOPDETECT     USB_UART_rxBufferLoopDetect
    #define USB_UART_RXBUFFER_OVERFLOW      USB_UART_rxBufferOverflow
#endif /* End USB_UART_RX_ENABLED */

#ifdef USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define USB_UART_CONTROL                USB_UART_CONTROL_REG
#endif /* End USB_UART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(USB_UART_TX_ENABLED)
    #define USB_UART_TXDATA                 USB_UART_TXDATA_REG
    #define USB_UART_TXSTATUS               USB_UART_TXSTATUS_REG
    #define USB_UART_TXSTATUS_MASK          USB_UART_TXSTATUS_MASK_REG
    #define USB_UART_TXSTATUS_ACTL          USB_UART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(USB_UART_TXCLKGEN_DP)
        #define USB_UART_TXBITCLKGEN_CTR        USB_UART_TXBITCLKGEN_CTR_REG
        #define USB_UART_TXBITCLKTX_COMPLETE    USB_UART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define USB_UART_TXBITCTR_PERIOD        USB_UART_TXBITCTR_PERIOD_REG
        #define USB_UART_TXBITCTR_CONTROL       USB_UART_TXBITCTR_CONTROL_REG
        #define USB_UART_TXBITCTR_COUNTER       USB_UART_TXBITCTR_COUNTER_REG
    #endif /* USB_UART_TXCLKGEN_DP */
#endif /* End USB_UART_TX_ENABLED */

#if(USB_UART_HD_ENABLED)
    #define USB_UART_TXDATA                 USB_UART_TXDATA_REG
    #define USB_UART_TXSTATUS               USB_UART_TXSTATUS_REG
    #define USB_UART_TXSTATUS_MASK          USB_UART_TXSTATUS_MASK_REG
    #define USB_UART_TXSTATUS_ACTL          USB_UART_TXSTATUS_ACTL_REG
#endif /* End USB_UART_HD_ENABLED */

#if( (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) )
    #define USB_UART_RXDATA                 USB_UART_RXDATA_REG
    #define USB_UART_RXADDRESS1             USB_UART_RXADDRESS1_REG
    #define USB_UART_RXADDRESS2             USB_UART_RXADDRESS2_REG
    #define USB_UART_RXBITCTR_PERIOD        USB_UART_RXBITCTR_PERIOD_REG
    #define USB_UART_RXBITCTR_CONTROL       USB_UART_RXBITCTR_CONTROL_REG
    #define USB_UART_RXBITCTR_COUNTER       USB_UART_RXBITCTR_COUNTER_REG
    #define USB_UART_RXSTATUS               USB_UART_RXSTATUS_REG
    #define USB_UART_RXSTATUS_MASK          USB_UART_RXSTATUS_MASK_REG
    #define USB_UART_RXSTATUS_ACTL          USB_UART_RXSTATUS_ACTL_REG
#endif /* End  (USB_UART_RX_ENABLED) || (USB_UART_HD_ENABLED) */

#if(USB_UART_INTERNAL_CLOCK_USED)
    #define USB_UART_INTCLOCK_CLKEN         USB_UART_INTCLOCK_CLKEN_REG
#endif /* End USB_UART_INTERNAL_CLOCK_USED */

#define USB_UART_WAIT_FOR_COMLETE_REINIT    USB_UART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_USB_UART_H */


/* [] END OF FILE */
