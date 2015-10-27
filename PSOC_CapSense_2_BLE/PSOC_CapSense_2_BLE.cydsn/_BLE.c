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

// This flag is used to let application update the CCCD value of the CapSense
// sensors characteristic.
uint8 updateCCCDreq;


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
    updateCCCDreq = FALSE;
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
            
            // Reset CapSense notification flag to prevent further notifications
			// being sent to Central device after next connection.
			sendDataNotifications = FALSE;
            sendDataIndications = FALSE;
			
			// Reset the CCCD value to disable notifications.
			updateCCCDreq = TRUE;
			UpdateCCCD();
            
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
				updateCCCDreq = TRUE;
            }
            
            // The peripheral has received a write command on the Control
            // Characteristic.
            else if(CYBLE_CAPSENSE_CONTROL_CHAR_HANDLE ==
                wrReqParam->handleValPair.attrHandle)
            {
                _BLE_acquireData = wrReqParam->handleValPair.value.val[CONTROL_ACQUIRE_DATA];
                _BLE_sendData = wrReqParam->handleValPair.value.val[CONTROL_SEND_DATA];
            }
            
            // The peripheral has received a write command on the Status
            // Characteristic CCCD.
            else if(CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE ==
                wrReqParam->handleValPair.attrHandle)
            {
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_NTF_BIT_MASK )
                    sendStatusNotifications = TRUE;
                else
                    sendStatusNotifications = FALSE;
                
                if( wrReqParam->handleValPair.value.val[CYBLE_CAPSENSE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] & CCCD_IND_BIT_MASK )
				    sendStatusIndications = TRUE;
                else
                    sendStatusIndications = FALSE;
            }
            
            // Update control flags
            _BLE_sendData = sendDataNotifications | sendDataIndications;
            _BLE_sendStatus = sendStatusNotifications | sendStatusIndications;
            
            // Send the response to the write request received.
			CyBle_GattsWriteRsp(connectionHandle);
            
            break;
    }
    
}


/*****************************************************************************
* Function Name: UpdateCCCD()
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
    // Update notification/indication attribute only when there has been a
    // write request.
    if(updateCCCDreq)
	{
    	// Local variable to store the wanted CCCD value.
    	uint8 CCCDvalue[2];
    	
    	// Handle value to update the CCCD.
    	CYBLE_GATT_HANDLE_VALUE_PAIR_T CCCDhandle;
        
		// Reset the flag.
		updateCCCDreq = FALSE;
	
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
*   None.
*
* Note:
*   
*****************************************************************************/
void _BLE_sendCapSenseData(void)
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

//    while (!vectorIsEmpty())
//    {
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
//    }
}


/* [] END OF FILE */