/******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the XMC MCU: DMA M2M Example for
* ModusToolbox. This example demonstrates transfer of data from flash to RAM
* using GPDMA peripheral.
*
* Related Document: See README.md
*
******************************************************************************
*
* Copyright (c) 2015 - 2022, Infineon Technologies AG
* All rights reserved.
*
* Boost Software License - Version 1.0 - August 17th, 2003
*
* Permission is hereby granted, free of charge, to any person or organization
* obtaining a copy of the software and accompanying documentation covered by
* this license (the "Software") to use, reproduce, display, distribute,
* execute, and transmit the Software, and to prepare derivative works of the
* Software, and to permit third-parties to whom the Software is furnished to
* do so, all subject to the following:
*
* The copyright notices in the Software and this entire statement, including
* the above license grant, this restriction and the following disclaimer,
* must be included in all copies of the Software, in whole or in part, and
* all derivative works of the Software, unless such copies or derivative
* works are solely in the form of machine-executable object code generated by
* a source language processor.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
* SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
* FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include "cybsp.h"
#include "cy_utils.h"
#include "cy_retarget_io.h"
#include <stdio.h>

/*******************************************************************************
* Macros
*******************************************************************************/

#define DATA_LENGTH 10    /* Length of the buffer to be transferred */

/* Define macro to enable/disable printing of debug messages */
#define ENABLE_XMC_DEBUG_PRINT          (0)

/*******************************************************************************
* Global Variables
*******************************************************************************/

/* Source buffer */
const uint32_t source_data[DATA_LENGTH] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

/* Destination buffer */
uint32_t destination_data[DATA_LENGTH];

/* Pointers */
uint32_t * ptr_source_data = (uint32_t *) source_data;
uint32_t * ptr_destination_data = (uint32_t *) destination_data;

/* Variable to denote completion of data transfer */
volatile uint8_t dma_complete = 0;

/*******************************************************************************
* Function Name: GPDMA0_INTERRUPT_HANDLER
********************************************************************************
* Summary:
* This is the interrupt handler function for the GPDMA block transfer complete
* event
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void GPDMA0_INTERRUPT_HANDLER(void)
{
    dma_complete = 1;

    /* Clear the DMA interrupt */
    XMC_DMA_CH_ClearEventStatus(DMA_HW, DMA_NUM, XMC_DMA_CH_EVENT_TRANSFER_COMPLETE);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function. This function performs
*  1. initial setup of device
*  2. initialize GPDMA peripheral
*  3. Compares the source and destination array to check if data was transferred
*     successfully.
*  4.  Upon successful data transmission, the USER LED is turned ON. Else, the
*     system is halted.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize printf retarget */
    cy_retarget_io_init(CYBSP_DEBUG_UART_HW);

    #if ENABLE_XMC_DEBUG_PRINT
        printf("Initialization done\r\n");
    #endif

    /* Set the interrupt priority for DMA event and enable the DMA interrupt */
    NVIC_SetPriority(GPDMA0_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 63, 0));
    NVIC_EnableIRQ(GPDMA0_0_IRQn);

    /* Start DMA transfer */
    XMC_DMA_CH_Enable(DMA_HW, DMA_NUM);

    /* Wait till DMA transfer is complete */
    while(dma_complete == 0)
    {

    }
    #if ENABLE_XMC_DEBUG_PRINT
        printf("DMA transfer completed\r\n");
    #endif

    /* Compare the source and destination array */
    if (memcmp(source_data, destination_data, sizeof(source_data)) != 0)
    {
        #if ENABLE_XMC_DEBUG_PRINT
            printf("Source and destination array elements are not equal\r\n");
        #endif
        /* Error - array elements are not equal. Halt CPU execution */
        CY_ASSERT(0);
    }
    else
    {
        /* Turn ON the user LED */
        XMC_GPIO_SetOutputHigh(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        #if ENABLE_XMC_DEBUG_PRINT
            printf("Source and destination array elements are equal\r\n");
        #endif
    }

    for (;;)
    {
        /* Placeholder for user application code */
    }
}

/* [] END OF FILE */
