/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: _UART.c
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
#include "main.h"
#include "vector.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/


/*****************************************************************************
* Function Name: _UART_Init()
******************************************************************************
* Summary:
*   Initialize the UART component.
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
void _UART_Init(void)
{
    // Start the UART component.
    USB_UART_Start();
}


/*****************************************************************************
* Function Name: _UART_sendCapSenseData()
******************************************************************************
* Summary:
*   Send the last CapSense set of data through USB.
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
void _UART_sendCapSenseData(void)
{
    uint8 num_EnabledSensors = CapSense_TOTAL_SENSOR_COUNT;
    uint16 rawData[num_EnabledSensors];
    
    uint8 numBytes_OneData = sizeof(rawData[0]);
    uint16 numBytes_AllData = num_EnabledSensors * numBytes_OneData;
    uint8 buffer[numBytes_AllData];
    
    uint16 i;
    uint16 j;
    uint16 arrayIterator = 0;
    
    // Retrieve the front data in the vectors.
    popOutVector(rawData);
    
    // Transfer the encoded sensors values into an uint8 array.
    for (i = 0; i < num_EnabledSensors; i++)
        for (j = 0; j < numBytes_OneData; j++)
            buffer[arrayIterator++] = rawData[i] >> ((numBytes_OneData - 1 - j) * 8) & 0xFF;
    
    // Send the data through UART component.
    USB_UART_PutArray(buffer, numBytes_AllData);
}

/* [] END OF FILE */
