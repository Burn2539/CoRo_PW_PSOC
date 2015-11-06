/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _BLE.h
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

#ifndef _BLE_H
#define _BLE_H

/*****************************************************************************
* Included headers
*****************************************************************************/
#include "main.h"


/*****************************************************************************
* Macros and constants
*****************************************************************************/
// Byte masks for the status characteristic.
#define STATUS_READY_BYTE_MASK          0
#define STATUS_ACQUIRING_BYTE_MASK      1
#define STATUS_NO_MORE_SPACE_BYTE_MASK  2
#define STATUS_DATA_ACQUIRED_BYTE_MASK  3
#define STATUS_SENDING_BYTE_MASK        4
#define STATUS_NO_MORE_DATA_BYTE_MASK   5
    
// Number of status in the BLE profile.
#define NUM_STATUS  6
    
// Byte masks for the control characteristic.
#define CONTROL_ACQUIRE_DATA_BYTE_MASK              0
#define CONTROL_SEND_DATA_BYTE_MASK                 1
#define CONTROL_SEND_DATA_SYNCHRONOUSLY_BYTE_MASK   2
    
// Number of controls in the BLE profile.
#define NUM_CONTROLS    3

// Bit masks for notification and indication bit in CCCD.
#define CCCD_NTF_BIT_MASK   1
#define CCCD_IND_BIT_MASK   2
    
// Client Characteristic Configuration descriptor data length. This is defined
// as per BLE spec.
#define CCC_DATA_LEN    2
    
//#define BUFFER_MAX_SIZE (CYBLE_GATT_MTU - CYBLE_GATT_MTU_BUF_COUNT)
#define BUFFER_MAX_SIZE 240


/*****************************************************************************
* Extern global variables
*****************************************************************************/
// These flags control the behavior of the main function.
extern uint8 _BLE_sendData;
extern uint8 _BLE_acquireData;
extern uint8 _BLE_sendStatus;
extern uint8 _BLE_sendDataSynchronously;
extern uint8 _BLE_restartAdvertisement;
extern uint8 _BLE_resetNeeded;
    
// Flag that is set when a device is connected through BLE.
extern uint8 _BLE_deviceConnected;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _BLE_Init(void);
void _BLE_Reset(void);
void EventHandler(uint32 event, void *eventParam);
void _BLE_UpdateCCCD(void);
void _BLE_UpdateControl(void);
uint8 _BLE_sendCapSenseData(void);
void _BLE_sendStatusFlags(void);
void _BLE_writeStatusFlags(void);

#endif

/* [] END OF FILE */
