/*******************************************************************************
* File Name: Delay_1ms.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_Delay_1ms_H)
#define CY_ISR_Delay_1ms_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void Delay_1ms_Start(void);
void Delay_1ms_StartEx(cyisraddress address);
void Delay_1ms_Stop(void);

CY_ISR_PROTO(Delay_1ms_Interrupt);

void Delay_1ms_SetVector(cyisraddress address);
cyisraddress Delay_1ms_GetVector(void);

void Delay_1ms_SetPriority(uint8 priority);
uint8 Delay_1ms_GetPriority(void);

void Delay_1ms_Enable(void);
uint8 Delay_1ms_GetState(void);
void Delay_1ms_Disable(void);

void Delay_1ms_SetPending(void);
void Delay_1ms_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the Delay_1ms ISR. */
#define Delay_1ms_INTC_VECTOR            ((reg32 *) Delay_1ms__INTC_VECT)

/* Address of the Delay_1ms ISR priority. */
#define Delay_1ms_INTC_PRIOR             ((reg32 *) Delay_1ms__INTC_PRIOR_REG)

/* Priority of the Delay_1ms interrupt. */
#define Delay_1ms_INTC_PRIOR_NUMBER      Delay_1ms__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable Delay_1ms interrupt. */
#define Delay_1ms_INTC_SET_EN            ((reg32 *) Delay_1ms__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the Delay_1ms interrupt. */
#define Delay_1ms_INTC_CLR_EN            ((reg32 *) Delay_1ms__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the Delay_1ms interrupt state to pending. */
#define Delay_1ms_INTC_SET_PD            ((reg32 *) Delay_1ms__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the Delay_1ms interrupt. */
#define Delay_1ms_INTC_CLR_PD            ((reg32 *) Delay_1ms__INTC_CLR_PD_REG)



#endif /* CY_ISR_Delay_1ms_H */


/* [] END OF FILE */
