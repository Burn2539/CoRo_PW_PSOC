/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _BLE.c
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
#include "_BLE.h"
#include "LowPowerMode.h"
#include "vector.h"
#include "crc.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// Stores connection parameters.
CYBLE_CONN_HANDLE_T  connectionHandle;

// Used to change the current clock configuration of the BLESS.
CYBLE_BLESS_CLK_CFG_PARAMS_T clockConfig;

// Status flag for the Stack Busy state. This flag is used to notify the
// application  whether there is stack buffer free to push more data or not.
uint8 _BLE_busyStatus;

// This flag is used to know whether a Central device has been connected.
// This is updated in BLE event callback function.
uint8 _BLE_deviceConnected;

// Size of the MTU used by the BLE stack negociated with the client.
uint16 negotiatedMtu;

// These flags are set when the Central device writes to CCCD of the
// CapSense sensors Characteristic to enable notifications or indications.
uint8 sendDataNotifications;
uint8 sendDataIndications;

// These flags are set when the Central device writes to CCCD of the
// Status Characteristic to enable notifications or indications.
uint8 sendStatusNotifications;
uint8 sendStatusIndications;

// These flags control the behavior of the main function.
uint8 _BLE_acquireData;
uint8 _BLE_sendData;
uint8 _BLE_sendStatus;
uint8 _BLE_sendDataSynchronously;
uint8 _BLE_restartAdvertisement;
uint8 _BLE_resetNeeded;

// These flags are used to let application update the CCCD value of the
// different characteristics.
uint8 updateSensorsCCCDreq;
uint8 updateStatusCCCDreq;

// This flag is used to let application update the values of the
// control characteristic.
uint8 updateControlValuesReq;


//////////////////////////////////////////////////////////////////////////////
// DEBUGGING VARIABLES
//////////////////////////////////////////////////////////////////////////////
uint32 eventCodesLog[256];
uint8 pointer = 0;


/*****************************************************************************
* Function Name: _BLE_Init()
******************************************************************************
* Summary:
*   Initialize the Bluetooth Low Energy module.
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
void _BLE_Init(void)
{
    // Enable the BLE module and register the callback function for all the events.
    CyBle_Start(EventHandler);
    
    // Initialize the busy status flag.
    _BLE_busyStatus = CYBLE_STACK_STATE_FREE;
    
    // Initialize the connection status flag.
    _BLE_deviceConnected = FALSE;
    
    // Initialize the MTU used during BLE communication.
    negotiatedMtu = DEFAULT_MTU_SIZE;
    
    // Initialize flags.
    sendDataNotifications = FALSE;
    sendDataIndications = FALSE;
    sendStatusNotifications = FALSE;
    sendStatusIndications = FALSE;
    
    updateSensorsCCCDreq = FALSE;
    updateStatusCCCDreq = FALSE;
    updateControlValuesReq = FALSE;
    
    _BLE_acquireData = FALSE;
    _BLE_sendData = FALSE;
    _BLE_sendStatus = FALSE;
    _BLE_sendDataSynchronously = FALSE;
    _BLE_restartAdvertisement = FALSE;
    _BLE_resetNeeded = FALSE;
}


/*****************************************************************************
* Function Name: _BLE_Reset()
******************************************************************************
* Summary:
*   Reset the BLE variables.
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
void _BLE_Reset(void)
{
    // Reset connection handle.
    connectionHandle.attId = 0;
    connectionHandle.bdHandle = 0;
    
    // Reset the negociated MTU.
    negotiatedMtu = DEFAULT_MTU_SIZE;
    
    // Reset flags.
    sendDataNotifications = FALSE;
    sendDataIndications = FALSE;
    sendStatusNotifications = FALSE;
    sendStatusIndications = FALSE;
    
    updateSensorsCCCDreq = FALSE;
    updateStatusCCCDreq = FALSE;
    updateControlValuesReq = FALSE;
    
    _BLE_acquireData = FALSE;
    _BLE_sendData = FALSE;
    _BLE_sendStatus = FALSE;
    _BLE_sendDataSynchronously = FALSE;
    _BLE_restartAdvertisement = TRUE;
}


/*****************************************************************************
* Function Name: EventHandler()
******************************************************************************
* Summary:
*   Handles the events generated by stack and take necessary actions when
*   event is received.
*
* Parameters:
*   eventCode - the event code
*   *event_params - the event parameters
*
* Return:
*   None.
*
* Note:
*   
*****************************************************************************/
void EventHandler(uint32 eventCode, void *eventParam)
{
    // Local variable to store the data received as part of the Write request
    // events.
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
    // Log all the events.
    eventCodesLog[pointer++] = eventCode;
    
    switch(eventCode)
    {
        //////////////////////////////////////////////////////////////////////
        // General events
        //////////////////////////////////////////////////////////////////////
        // This event is received when the module is started.
        case CYBLE_EVT_STACK_ON: // eventCode == 0x01
            
            #if LOWPOWERMODE_ENABLED
                // Get the configured clock parameters for BLE subsystem.
                CyBle_GetBleClockCfgParam(&clockConfig);

                // Set the device sleep-clock accuracy (SCA) based on the
                // tuned ppm of the WCO.
                clockConfig.bleLlSca = CYBLE_LL_SCA_000_TO_020_PPM;

                // Set the clock parameter of BLESS with updated values.
                CyBle_SetBleClockCfgParam(&clockConfig);
            #endif
        
            // Start fast advertisement
			_BLE_restartAdvertisement = TRUE;
        
            break;
            
        
        // This event is received when there is a timeout and application  
        // needs to handle the event. Timeout reason is defined by 
        // CYBLE_TO_REASON_CODE_T.
        case CYBLE_EVT_TIMEOUT: // eventCode == 0x02
            
            // If triggered by the advertisement timeout.
            if( *(CYBLE_TO_REASON_CODE_T *)eventParam == CYBLE_GAP_ADV_MODE_TO ) {
            }
            
            break;
            
        
        // This event is generated when the internal stack buffer is full 
		// and no more data can be accepted or the stack has buffer available 
		// and can accept data. This event is used by application to prevent 
        // pushing lot of data to stack.
        case CYBLE_EVT_STACK_BUSY_STATUS: // eventCode == 0x05
			
			// Extract the present stack status.
            _BLE_busyStatus = *(uint8*)eventParam;
            
            break;
            
            
        //////////////////////////////////////////////////////////////////////
        // GAP events
        //////////////////////////////////////////////////////////////////////
        // This event is received by Peripheral and Central devices. When it is
        // received by Peripheral, peripheral needs to Call CyBle_GappAuthReqReply()
        // to reply to authentication request from Central.
        case CYBLE_EVT_GAP_AUTH_REQ: // eventCode == 0x21
            
            // Reply to the authentification request.
            CyBle_GappAuthReqReply(connectionHandle.bdHandle, &cyBle_authInfo);
            
            break;
            
        
        // Authentication process failed between two devices. The return value 
        // of type CYBLE_GAP_AUTH_FAILED_REASON_T indicates the reason for failure.
        case CYBLE_EVT_GAP_AUTH_FAILED: // eventCode == 0x25
            
            break;
                
            
        // If the current BLE state is Disconnected, then the Advertisement
    	// Start_Stop event implies that advertisement has stopped.
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP: // eventCode == 0x26
            
            // Restart fast advertisement
            if(CyBle_GetState() != CYBLE_STATE_ADVERTISING)
                _BLE_restartAdvertisement = TRUE;
            
            break;
            
            
        // This event is generated at the GAP Peripheral end after 
        // connection is completed with peer Centra  device.
        case CYBLE_EVT_GAP_DEVICE_CONNECTED: // eventCode == 0x27
            
            break;

        
        // Disconnected from remote device or failed to establish
        // connection. Parameter returned with the event contains pointer to 
        // the reason for disconnection.
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED: // eventCode == 0x28
            
            break;
            
            
        // This event is generated at the GAP Central and the Peripheral end
        // after connection parameter update is requested from the host to 
        // the controller. Event parameter is a pointer to a structure of type
        // CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T.
        case CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE: // eventCode == 0x2A
            
            break;
            
                
        //////////////////////////////////////////////////////////////////////
        // GATT events
        //////////////////////////////////////////////////////////////////////
        // This event is received when device is connected at GATT level.
        case CYBLE_EVT_GATT_CONNECT_IND: // eventCode == 0x41
        
			// Update attribute handle on GATT Connection.
            connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            
            // Set the connection status flag.
            _BLE_deviceConnected = TRUE;
            
            // Set all the flags for the current connection.
            updateSensorsCCCDreq = TRUE;
            updateControlValuesReq = TRUE;
            _BLE_UpdateCCCD();
            _BLE_UpdateControl();
            _BLE_writeStatusFlags();
            
            break;
        
        
        // This event is received when device is disconnected at GATT level.
        case CYBLE_EVT_GATT_DISCONNECT_IND: // eventCode == 0x42
            
            // Reset the connection status flag.
            _BLE_deviceConnected = FALSE;
            
            // Reset the connection status flag.
            _BLE_resetNeeded = TRUE;
            
            break;
            
            
        // 'MTU  Exchange  Request'  received  from  GATT  client  device. 
        // Event parameter contains the MTU size of type CYBLE_GATT_XCHG_MTU_PARAM_T. 
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ: // eventCode == 0x43
            
            // Establish the MTU that is going to be used during communication.
            negotiatedMtu = (((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu < CYBLE_GATT_MTU) ?
                            ((CYBLE_GATT_XCHG_MTU_PARAM_T *)eventParam)->mtu : CYBLE_GATT_MTU;
            
            break;
            
        
        // This event is received when Central device sends a Write command
        // on an Attribute.
        case CYBLE_EVT_GATTS_WRITE_REQ: // eventCode == 0x4C
            
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
			// The peripheral has received a write command on the Sensors
            // Characteristic CCCD.
            if(CYBLE_CAPSENSE_SENSORS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE ==
                wrReqParam->handleValPair.attrHandle)
            {
                // Extract the Write value sent by the Client CCCD.
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_SENSORS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_NTF_BIT_MASK )
                    sendDataNotifications = TRUE;
                else
                    sendDataNotifications = FALSE;
                
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_SENSORS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_IND_BIT_MASK )
				    sendDataIndications = TRUE;
                else
                    sendDataIndications = FALSE;
                
                // Set flag to allow CCCD to be updated.
				updateSensorsCCCDreq = TRUE;
            }
            
            // The peripheral has received a write command on the Control
            // Characteristic.
            else if(CYBLE_CAPSENSE_CONTROL_CHAR_HANDLE ==
                wrReqParam->handleValPair.attrHandle)
            {
                // Update 'acquireData' flag:
                    // FALSE only if 'Status_Acquiring' is TRUE;
                    // TRUE only if 'Status_Ready' is also TRUE.
                if ( Status_Acquiring && !wrReqParam->handleValPair.value.val[CONTROL_ACQUIRE_DATA_BYTE_MASK] )
                    _BLE_acquireData = FALSE;
                else if ( Status_Ready && wrReqParam->handleValPair.value.val[CONTROL_ACQUIRE_DATA_BYTE_MASK] )
                    _BLE_acquireData = TRUE;
                
                // Update 'sendData' flag:
                    // FALSE only if 'Status_NoMoreData' is TRUE;
                    // TRUE only if 'Status_DataAcquired' is also TRUE.
                if ( !wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_BYTE_MASK] && Status_NoMoreData )
                    _BLE_sendData = FALSE;
                else if ( wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_BYTE_MASK] && Status_DataAcquired )
                    _BLE_sendData = TRUE;
                
                // Update 'sendDataSyncronously' flag:
                    // FALSE only if 'Status_Acquiring' OR 'Status_Ready' is TRUE;
                    // TRUE only if 'Status_Ready' is also TRUE.
                if ( (Status_Acquiring || Status_Ready) && !wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_SYNCHRONOUSLY_BYTE_MASK] )
                    _BLE_sendDataSynchronously = FALSE;
                else if ( Status_Ready && wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_SYNCHRONOUSLY_BYTE_MASK] )
                    _BLE_sendDataSynchronously = TRUE;

                
                // Set the flag to allow the control values to be updated.
                updateControlValuesReq = TRUE;
            }
            
            // The peripheral has received a write command on the Status
            // Characteristic CCCD.
            else if(CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE ==
                wrReqParam->handleValPair.attrHandle)
            {
                // Extract the Write value sent by the Client CCCD.
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_NTF_BIT_MASK )
                    sendStatusNotifications = TRUE;
                else
                    sendStatusNotifications = FALSE;
                
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_IND_BIT_MASK )
				    sendStatusIndications = TRUE;
                else
                    sendStatusIndications = FALSE;
                
                // Set flag to allow CCCD to be updated.
				updateStatusCCCDreq = TRUE;
            }
            
            // Update control flags
            _BLE_sendStatus = sendStatusNotifications | sendStatusIndications;
            
            // Send the response to the write request received.
			CyBle_GattsWriteRsp(connectionHandle);
            
            break;
            
            
        // Confirmation to indication response from client device. Event
        // parameter is a pointer to a structure of type CYBLE_CONN_HANDLE_T. 
        case CYBLE_EVT_GATTS_HANDLE_VALUE_CNF: // eventCode == 0x54
            
            break;
            
            
        // Event parameters for characteristic read value access event 
        // generated by BLE Stack upon an access of Characteristic value read  
        // for the characteristic definition which has
        // CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
        case CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ: // eventCode == 0x57
            
            // Make sure all the flags are updated.
            _BLE_writeStatusFlags();
            
            break;
            

        default:
            
            break;
    }
}


/*****************************************************************************
* Function Name: _BLE_UpdateCCCD()
******************************************************************************
* Summary:
*   Update the data handle for notification status and report it to BLE 
*	component so that it can be read by Central device.
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
void _BLE_UpdateCCCD(void)
{
	// Local variable to store the wanted CCCD value.
	uint8 CCCDvalue[CCC_DATA_LEN];
    
    // Handle value to update the CCCD.
    CYBLE_GATT_HANDLE_VALUE_PAIR_T CCCDhandle;

    // Update sensors notification/indication attribute only when there has 
    // been a write request.
    if(updateSensorsCCCDreq && _BLE_deviceConnected)
	{
		// Reset the flag.
		updateSensorsCCCDreq = FALSE;
	
		// Write the present notification/indication status to the local variable.
		CCCDvalue[0] = sendDataNotifications;
		CCCDvalue[1] = sendDataIndications;
		
		// Update CCCD handle with notification status data.
		CCCDhandle.attrHandle = CYBLE_CAPSENSE_SENSORS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
		CCCDhandle.value.val = CCCDvalue;
		CCCDhandle.value.len = CCC_DATA_LEN;
		
		// Report data to BLE component for sending data when read by Central device.
		CyBle_GattsWriteAttributeValue(&CCCDhandle, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
	}
    
    // Update sensors notification/indication attribute only when there has 
    // been a write request.
    if(updateStatusCCCDreq && _BLE_deviceConnected)
	{
		// Reset the flag.
		updateStatusCCCDreq = FALSE;
	
		// Write the present notification/indication status to the local variable.
		CCCDvalue[0] = sendStatusNotifications;
		CCCDvalue[1] = sendStatusIndications;
		
		// Update CCCD handle with notification status data.
		CCCDhandle.attrHandle = CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
		CCCDhandle.value.val = CCCDvalue;
		CCCDhandle.value.len = CCC_DATA_LEN;
		
		// Report data to BLE component for sending data when read by Central device.
		CyBle_GattsWriteAttributeValue(&CCCDhandle, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
	}	
}


/*****************************************************************************
* Function Name: _BLE_UpdateControl()
******************************************************************************
* Summary:
*   Update the values of the 'Control' characteristic.
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
void _BLE_UpdateControl(void)
{
    // Local variable to store the wanted control values.
	uint8 ControlValues[NUM_CONTROLS];
    
    // Handle value to update the control values.
    CYBLE_GATT_HANDLE_VALUE_PAIR_T ControlHandle;

    // Update control attribute only when there has been a write request.
    if(updateControlValuesReq && _BLE_deviceConnected)
    {
		// Reset the flag.
		updateControlValuesReq = FALSE;
        
		// Write the present control values to the local variable.
		ControlValues[CONTROL_ACQUIRE_DATA_BYTE_MASK] = _BLE_acquireData;
		ControlValues[CONTROL_SEND_DATA_BYTE_MASK] = _BLE_sendData;
        ControlValues[CONTROL_SEND_DATA_SYNCHRONOUSLY_BYTE_MASK] = _BLE_sendDataSynchronously;
		
		// Update control values handle.
		ControlHandle.attrHandle = CYBLE_CAPSENSE_CONTROL_CHAR_HANDLE;
		ControlHandle.value.val = ControlValues;
		ControlHandle.value.len = NUM_CONTROLS;
		
		// Report data to BLE component.
		CyBle_GattsWriteAttributeValue(&ControlHandle, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
    }
}


/*****************************************************************************
* Function Name: _BLE_sendCapSenseData()
******************************************************************************
* Summary:
*   Send the whole content of the vectors through BLE notifications
*   or indications.
*
* Parameters:
*   None.
*
* Return:
*   'SUCCESS' if it succeeded.
*   'NO_MORE_DATA' if the vector is empty
*
* Note:
*   
*****************************************************************************/
uint8 _BLE_sendCapSenseData(void)
{
    // Verify the presence of data to send.
    if ( vectorIsEmpty() )
        return NO_MORE_DATA;
    
    
    uint16 rawData[CapSense_TOTAL_SENSOR_COUNT];
    uint32 encodedData[CapSense_TOTAL_SENSOR_COUNT];
    uint8 numBytes_OneData = sizeof(encodedData[0]);
    
    uint8 i;
    uint8 j;
    uint8 k;
    uint16 arrayIterator = 0;
        
    CYBLE_GATTS_HANDLE_VALUE_NTF_T NotificationHandle;
    CYBLE_GATTS_HANDLE_VALUE_IND_T IndicationHandle;
    
    uint16 numIterations;
    
    uint8 buffer[CYBLE_GATT_MTU];
    numIterations = ( (vectorSize() * numBytes_OneData * CapSense_TOTAL_SENSOR_COUNT) < (negotiatedMtu - 3) ) ?
                        vectorSize() : (negotiatedMtu - 3) / numBytes_OneData / CapSense_TOTAL_SENSOR_COUNT;
                    
    uint16 numBytes_AllData = numIterations * numBytes_OneData * CapSense_TOTAL_SENSOR_COUNT;

    for( k = 0; k < numIterations; k++) {
        // Retrieve the front data in the vectors.
        popOutVector(rawData);
        
        // Encode the data with CRC.
        for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
            encodedData[i] = encodeCRC( rawData[i] );
        
        // Transfer the encoded sensors values into an uint8 array.
        for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
            for (j = 0; j < numBytes_OneData; j++)
                buffer[arrayIterator++] = encodedData[i] >> ((numBytes_OneData - 1 - j) * 8) & 0xFF;
    }
    
    // Send the CapSense characteristic values to BLE client.
    if (sendDataNotifications) {
        NotificationHandle.attrHandle = CYBLE_CAPSENSE_SENSORS_CHAR_HANDLE;
        NotificationHandle.value.val = buffer;
        NotificationHandle.value.len = numBytes_AllData;
        CyBle_GattsNotification(connectionHandle, &NotificationHandle);
    }
    else if (sendDataIndications) {        
        IndicationHandle.attrHandle = CYBLE_CAPSENSE_SENSORS_CHAR_HANDLE;
        IndicationHandle.value.val = buffer;
        IndicationHandle.value.len = numBytes_AllData;
        CyBle_GattsIndication(connectionHandle, &IndicationHandle);
    }
    
    
    // Verify if the vector is now empty.
    if ( vectorIsEmpty() )
        return NO_MORE_DATA;
    
    
    return SUCCESS;
}


/*****************************************************************************
* Function Name: _BLE_sendStatusFlags()
******************************************************************************
* Summary:
*   Send all the status flags.
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
void _BLE_sendStatusFlags(void)
{
    static uint8 Previous_Status_Ready = 0xFF;
    static uint8 Previous_Status_Acquiring = 0xFF;
    static uint8 Previous_Status_NoMoreSpace = 0xFF;
    static uint8 Previous_Status_DataAcquired = 0xFF;
    static uint8 Previous_Status_Sending = 0xFF;
    static uint8 Previous_Status_NoMoreData = 0xFF;
    
    uint8 StatusArray[NUM_STATUS];
        
    CYBLE_GATTS_HANDLE_VALUE_NTF_T NotificationHandle;
    CYBLE_GATTS_HANDLE_VALUE_IND_T IndicationHandle;

    // Verify if the status changed since the last notification.
    // If so, send all the current status.
    if( Previous_Status_Ready != Status_Ready ||
        Previous_Status_Acquiring != Status_Acquiring ||
        Previous_Status_NoMoreSpace != Status_NoMoreSpace ||
        Previous_Status_DataAcquired != Status_DataAcquired ||
        Previous_Status_Sending != Status_Sending ||
        Previous_Status_NoMoreData != Status_NoMoreData )
    {
        // Fill the 'StatusArray' with the current status values.
        StatusArray[STATUS_READY_BYTE_MASK] = Status_Ready;
        StatusArray[STATUS_ACQUIRING_BYTE_MASK] = Status_Acquiring;
        StatusArray[STATUS_NO_MORE_SPACE_BYTE_MASK] = Status_NoMoreSpace;
        StatusArray[STATUS_DATA_ACQUIRED_BYTE_MASK] = Status_DataAcquired;
        StatusArray[STATUS_SENDING_BYTE_MASK] = Status_Sending;
        StatusArray[STATUS_NO_MORE_DATA_BYTE_MASK] = Status_NoMoreData;
        
        // Send the Status characteristic values to BLE client.
        if (sendStatusNotifications) {
            NotificationHandle.attrHandle = CYBLE_CAPSENSE_STATUS_CHAR_HANDLE;
            NotificationHandle.value.val = StatusArray;
            NotificationHandle.value.len = NUM_STATUS;
            CyBle_GattsNotification(connectionHandle, &NotificationHandle);
        }
        else if (sendStatusIndications) {        
            IndicationHandle.attrHandle = CYBLE_CAPSENSE_STATUS_CHAR_HANDLE;
            IndicationHandle.value.val = StatusArray;
            IndicationHandle.value.len = NUM_STATUS;
            CyBle_GattsIndication(connectionHandle, &IndicationHandle);
        }
        
        // Set the 'Previous_*' static variables to the current status values.
        Previous_Status_Ready = Status_Ready;
        Previous_Status_Acquiring = Status_Acquiring;
        Previous_Status_NoMoreSpace = Status_NoMoreSpace;
        Previous_Status_DataAcquired = Status_DataAcquired;
        Previous_Status_Sending = Status_Sending;
        Previous_Status_NoMoreData = Status_NoMoreData;
    }
}


/*****************************************************************************
* Function Name: _BLE_writeStatusFlags()
******************************************************************************
* Summary:
*   Write the actual value of all the status flags into the characteristic.
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
void _BLE_writeStatusFlags(void)
{
    uint8 StatusArray[NUM_STATUS];
        
    CYBLE_GATT_HANDLE_VALUE_PAIR_T FlagsHandle;

    // Fill the 'StatusArray' with the current status values.
    StatusArray[STATUS_READY_BYTE_MASK] = Status_Ready;
    StatusArray[STATUS_ACQUIRING_BYTE_MASK] = Status_Acquiring;
    StatusArray[STATUS_NO_MORE_SPACE_BYTE_MASK] = Status_NoMoreSpace;
    StatusArray[STATUS_DATA_ACQUIRED_BYTE_MASK] = Status_DataAcquired;
    StatusArray[STATUS_SENDING_BYTE_MASK] = Status_Sending;
    StatusArray[STATUS_NO_MORE_DATA_BYTE_MASK] = Status_NoMoreData;
    
    // Write the Status values into the characteristic.
    FlagsHandle.attrHandle = CYBLE_CAPSENSE_STATUS_CHAR_HANDLE;
    FlagsHandle.value.val = StatusArray;
    FlagsHandle.value.len = NUM_STATUS;
    CyBle_GattsWriteAttributeValue(&FlagsHandle, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
}

/* [] END OF FILE */