/****************************************************************************
*
* Project Name		: PSoC CapSense 2 BLE
*
* File Name			: vector.c
* Version 			: 1.0
*
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.2
* Compiler    		: ARM GCC 4.8.4, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit
*
* Owner             : A. BERNIER
*
*****************************************************************************
*
* Source:	http://www.sourcetricks.com/2008/07/c-queues.html
*
*****************************************************************************/


/*****************************************************************************
* Included headers
*****************************************************************************/
#include "vector.h"


/*****************************************************************************
* Local global variables
*****************************************************************************/
typedef struct vector {
    uint16 data[VECTOR_MAX_SIZE];
} vector;
    
vector sensor[CapSense_TOTAL_SENSOR_COUNT];

uint8 front;
uint8 rear;


/*****************************************************************************
* Function Name: Vector_Init()
******************************************************************************
* Summary:
*   Initialize the vectors' pointers.
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
void Vector_Init() {
        front = 0;
        rear = 0;
}


/*****************************************************************************
* Function Name: pushInVector()
******************************************************************************
* Summary:
*   Insert the data at the end of the vector and increment the rear pointer.
*
* Parameters:
*   The array of data to insert.
*
* Return:
*   None.
*
* Note:
*   There must be as many data to insert as there are sensors enabled.
*****************************************************************************/
void pushInVector(uint16 *dataIn) {
    uint8 i;
    
    for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
        sensor[i].data[rear] = dataIn[i];
    
    rear++;
    rear = rear % VECTOR_MAX_SIZE;

}


/*****************************************************************************
* Function Name: popOutVector()
******************************************************************************
* Summary:
*   Return the first data of the vector and increment the front pointer.
*
* Parameters:
*   The array that will be containing the first data of the vectors.
*
* Return:
*   None.
*
* Note:
*   'array' must have enough space to receive the data of all the
*   enabled sensors.
*****************************************************************************/
void popOutVector(uint16 *dataOut) {    
    uint8 i;
    
    for (i = 0; i < CapSense_TOTAL_SENSOR_COUNT; i++)
        dataOut[i] = sensor[i].data[front];
        
    front++;
    front = front % VECTOR_MAX_SIZE;

}


/*****************************************************************************
* Function Name: vectorSize()
******************************************************************************
* Summary:
*   Calculate and return the size of the vectors.
*
* Parameters:
*   None.
*
* Return:
*   The size of the vectors.
*
* Note:
*   
*****************************************************************************/
uint16 vectorSize(void) {
    if (rear >= front)
        return (rear - front);
    else
        return (front - rear);
}


/*****************************************************************************
* Function Name: vectorIsEmpty()
******************************************************************************
* Summary:
*   Return true if the vector is empty.
*
* Parameters:
*   None.
*
* Return:
*   Return true if the vector is empty.
*
* Note:
*   
*****************************************************************************/
uint8 vectorIsEmpty(void) {
    return (front == rear) ? TRUE : FALSE;
}


/*****************************************************************************
* Function Name: vectorIsFull()
******************************************************************************
* Summary:
*   Return true if the vector is full.
*
* Parameters:
*   None.
*
* Return:
*   Return true if the vector is full..
*
* Note:
*   
*****************************************************************************/
uint8 vectorIsFull(void) {
    return (vectorSize() == (VECTOR_MAX_SIZE - 1) ) ? TRUE : FALSE;
}


/*****************************************************************************
* Function Name: deleteVector()
******************************************************************************
* Summary:
*   Reset the front and rear pointer.
*
* Parameters:
*   None.
*
* Return:
*   None.
*
* Note:
*   Doesn't delete the actual data.
*****************************************************************************/
void deleteVector(void) {
        front = 0;
        rear = 0;
}


/* [] END OF FILE */
