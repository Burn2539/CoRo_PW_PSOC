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
// Bit masks for notification and indication bit in CCCD.
#define CCCD_NTF_BIT_MASK   0x01
#define CCCD_IND_BIT_MASK   0x02

// Byte masks for the status characteristic.
#define STATUS_READY_BYTE_MASK          0x00
#define STATUS_ACQUIRING_BYTE_MASK      0x01
#define STATUS_DATA_ACQUIRED_BYTE_MASK  0x02
#define STATUS_SENDING_BYTE_MASK        0x03
    
// Byte masks for the control characteristic.
#define CONTROL_ACQUIRE_DATA    0x00
#define CONTROL_SEND_DATA       0x01

// Client Characteristic Configuration descriptor data length. This is defined
// as per BLE spec.
#define CCC_DATA_LEN   2

// Connection Update Parameter values to modify connection interval. These 
// values are sent as part of CyBle_L2capLeConnectionParamUpdateRequest() 
// which requests Client to update the existing Connection Interval to new  
// value. Increasing connection interval will reduce data rate but will also 
// reduce power consumption. These numbers will influence power consumption.
// Minimum connection interval = CONN_PARAM_UPDATE_MIN_CONN_INTERVAL * 1.25 ms.
#define CONN_PARAM_UPDATE_MIN_CONN_INTERVAL 40
// Maximum connection interval = CONN_PARAM_UPDATE_MAX_CONN_INTERVAL * 1.25 ms.
#define CONN_PARAM_UPDATE_MAX_CONN_INTERVAL	42
// Slave latency = Number of connection events.
#define CONN_PARAM_UPDATE_SLAVE_LATENCY		4
// Supervision timeout = CONN_PARAM_UPDATE_SUPRV_TIMEOUT * 10.
#define CONN_PARAM_UPDATE_SUPRV_TIMEOUT		200


/*****************************************************************************
* Extern global variables
*****************************************************************************/
// These flags control the behavior of the main function.
extern uint8 _BLE_sendData;
extern uint8 _BLE_acquireData;
extern uint8 _BLE_sendStatus;


/*****************************************************************************
* Function Declarations
*****************************************************************************/
void _BLE_Init(void);
void EventHandler(uint32 event, void *eventParam);
void _BLE_UpdateCCCD(void);
void _BLE_sendCapSenseData(void);

#endif

/* [] END OF FILE */
