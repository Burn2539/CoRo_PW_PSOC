/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: vector.h
* Version 			: 1.0
*
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
* Compiler    		: ARM GCC 4.8.4, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit
*
* Owner             : A. BERNIER
*
*****************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include <project.h>


/*****************************************************************************
* Macros and constants
*****************************************************************************/
#define FALSE   0u
#define TRUE    1u

#define VECTOR_MAX_SIZE     256


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void Vector_Init();
void pushInVector(uint16 *dataIn);
void popOutVector(uint16 *dataOut);
uint16 vectorSize(void);
uint8 vectorIsEmpty(void);
uint8 vectorIsFull(void);
void clearVector(void);

#endif

/* [] END OF FILE */
