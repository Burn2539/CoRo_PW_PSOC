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
#include "vector.h"
#include "crc.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
// Stores connection parameters.
CYBLE_CONN_HANDLE_T  connectionHandle;

// Status flag for the Stack Busy state. This flag is used to notify the
// application  whether there is stack buffer free to push more data or not.
uint8 busyStatus;

// Connection Parameter update values. This values are used by the BLE 
// component to update the connector parameter, including connection  
// interval, to desired value.
static CYBLE_GAP_CONN_UPDATE_PARAM_T ConnectionParam = {
    CONN_PARAM_UPDATE_MIN_CONN_INTERVAL,  		      
    CONN_PARAM_UPDATE_MAX_CONN_INTERVAL,		       
    CONN_PARAM_UPDATE_SLAVE_LATENCY,			    
    CONN_PARAM_UPDATE_SUPRV_TIMEOUT 			         	
};

// This flag is used to know whether a Central device has been connected.
// This is updated in BLE event callback function.
uint8 deviceConnected;

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

// These flags are used to let application update the CCCD value of the
// different characteristics.
uint8 updateSensorsCCCDreq;
uint8 updateStatusCCCDreq;

// This flag is used to let application update the values of the
// control characteristic.
uint8 updateControlValuesReq;


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
    busyStatus = CYBLE_STACK_STATE_FREE;
    
    // Initialize the connection status flag.
    deviceConnected = FALSE;
    
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
    // Local variable to store the data received as part of the Write request events.
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
    switch(eventCode)
    {
        // This event is received when the module is started.
        case CYBLE_EVT_STACK_ON:
        
            // Start fast advertisement
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        
            break;
        
        
        // This event is generated when the internal stack buffer is full and no more
		// data can be accepted or the stack has buffer available and can accept data.
		// This event is used by application to prevent pushing lot of data to stack.
        case CYBLE_EVT_STACK_BUSY_STATUS:
			
			/* Extract the present stack status */
            busyStatus = * (uint8*)eventParam;
            
            break;
        
        
        // This event is received when device is disconnected or advertising
        // times out.
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        
            // Reset the connection status flag.
            deviceConnected = FALSE;
            
            // Start fast advertisement
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        
            break;
            
        
        // If the current BLE state is Disconnected, then the Advertisement
    	// Start Stop event implies that advertisement has stopped.
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            
            if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
                // Start slow advertisement
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            break;
        
        
        // This event is received when device is connected at GATT level.
        case CYBLE_EVT_GATT_CONNECT_IND:
        
			// Update attribute handle on GATT Connection.
            connectionHandle = *(CYBLE_CONN_HANDLE_T  *)eventParam;
            
            // Send Connection Update request with set Parameter.
		    CyBle_L2capLeConnectionParamUpdateRequest(connectionHandle.bdHandle, &ConnectionParam);
            
            // Set the connection status flag.
            deviceConnected = TRUE;
            
            break;
        
        
        // This event is received when device is disconnected at GATT level.
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            
            // Reset notification flags to prevent further notifications
			// being sent to Central device after next connection.
			sendDataNotifications = FALSE;
            sendDataIndications = FALSE;
            sendStatusNotifications = FALSE;
            sendStatusIndications = FALSE;
            
            // Reset control flags
            _BLE_acquireData = FALSE;
            _BLE_sendData = FALSE;
            updateControlValuesReq = TRUE;
            _BLE_UpdateControl();
			
			// Reset the CCCD value to disable notifications.
			updateSensorsCCCDreq = TRUE;
            updateStatusCCCDreq = TRUE;
			_BLE_UpdateCCCD();
            
            // Start fast advertisement
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            
            break;
            
        
        // This event is received when Central device sends a Write command
        // on an Attribute.
        case CYBLE_EVT_GATTS_WRITE_REQ:
            
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
                    // FALSE possible at any time;
                    // TRUE only if 'Status_Ready' is also TRUE.
                _BLE_acquireData = Status_Ready & wrReqParam->handleValPair.value.val[CONTROL_ACQUIRE_DATA_BYTE_MASK];
                
                // Update 'sendData' flag:
                    // FALSE only if 'Status_NoMoreData' is also TRUE;
                    // TRUE only if 'Status_DataAcquired' is also TRUE.
                if ( !wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_BYTE_MASK] && Status_NoMoreData )
                    _BLE_sendData = FALSE;
                else if ( wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA_BYTE_MASK] && Status_DataAcquired )
                    _BLE_sendData = TRUE;
                
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
    if(updateSensorsCCCDreq)
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
		CyBle_GattsWriteAttributeValue(&CCCDhandle, 0, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
	}
    
    // Update sensors notification/indication attribute only when there has 
    // been a write request.
    if(updateStatusCCCDreq)
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
		CyBle_GattsWriteAttributeValue(&CCCDhandle, 0, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
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
    if(updateControlValuesReq)
    {
		// Reset the flag.
		updateControlValuesReq = FALSE;
        
		// Write the present control values to the local variable.
		ControlValues[CONTROL_ACQUIRE_DATA_BYTE_MASK] = _BLE_acquireData;
		ControlValues[CONTROL_SEND_DATA_BYTE_MASK] = _BLE_sendData;
		
		// Update control values handle.
		ControlHandle.attrHandle = CYBLE_CAPSENSE_CONTROL_CHAR_HANDLE;
		ControlHandle.value.val = ControlValues;
		ControlHandle.value.len = NUM_CONTROLS;
		
		// Report data to BLE component.
		CyBle_GattsWriteAttributeValue(&ControlHandle, 0, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
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
    uint16 rawData[CapSense_TOTAL_SENSOR_COUNT];
    uint32 encodedData[CapSense_TOTAL_SENSOR_COUNT];
    uint8 numBytes = sizeof(encodedData[0]);
    uint8 dataArray[CapSense_TOTAL_SENSOR_COUNT * sizeof(encodedData[0])];
    
    uint8 i;
    uint8 j;
    uint16 arrayIterator;
        
    CYBLE_GATTS_HANDLE_VALUE_NTF_T NotificationHandle;
    CYBLE_GATTS_HANDLE_VALUE_IND_T IndicationHandle;

    // Verify the presence of data to send.
    if ( vectorIsEmpty() )
        return NO_MORE_DATA;

    // Retrieve the front data in the vectors.
    popOutVector(rawData);
    
    // Encode the data with CRC.
    for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
        //encodedData[i] = encodeCRC( rawData[i] );
        encodedData[i] = encodeCRC(rawData[i]);
    
    // Transfer the encoded sensors values into an uint8 array.
    arrayIterator = 0;
    for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
        for (j = 0; j < numBytes; j++)
            dataArray[arrayIterator++] = encodedData[i] >> ((numBytes - 1 - j) * 8) & 0xFF;
    
    // Wait for the stack to be ready.
	while(busyStatus == CYBLE_STACK_STATE_BUSY)
        CyBle_ProcessEvents();
    
    // Send the CapSense characteristic values to BLE client.
    if (sendDataNotifications) {
        NotificationHandle.attrHandle = CYBLE_CAPSENSE_SENSORS_CHAR_HANDLE;
        NotificationHandle.value.val = dataArray;
        NotificationHandle.value.len = CapSense_TOTAL_SENSOR_COUNT * numBytes;
        CyBle_GattsNotification(connectionHandle, &NotificationHandle);
    }
    else if (sendDataIndications) {        
        IndicationHandle.attrHandle = CYBLE_CAPSENSE_SENSORS_CHAR_HANDLE;
        IndicationHandle.value.val = dataArray;
        IndicationHandle.value.len = CapSense_TOTAL_SENSOR_COUNT * numBytes;
        CyBle_GattsIndication(connectionHandle, &IndicationHandle);
    }
    
    // Verify if the vector is now empty.
    if ( vectorIsEmpty() )
        return NO_MORE_DATA;
    
    return SUCCESS;
}


/*****************************************************************************
* Function Name: _BLE_sendPSOCStatus()
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
void _BLE_sendPSOCStatus(void)
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
        
        // Wait for the stack to be ready.
    	while(busyStatus == CYBLE_STACK_STATE_BUSY)
            CyBle_ProcessEvents();
        
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


/* [] END OF FILE */
