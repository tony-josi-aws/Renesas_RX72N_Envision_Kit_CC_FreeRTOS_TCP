/***********************************************************************
*
*  FILE        : Renesas_RX72N_Envision_Kit_FreeRTOS_TCP.c
*  DATE        : 2023-07-22
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/

#include <string.h>

#include "r_smc_entry.h"

#include "FreeRTOS.h"
#include "task.h"
#include "r_gpio_rx_if.h"
#include "r_sci_rx_pinset.h"
#include "r_sci_rx_if.h"
#include "r_dtc_rx_if.h"
#include "demo_specific_io.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

void main(void);

/* Start user code for adding. Do not edit comment generated here */

#include "serial.h"
#include "logging.h"

#define BASIC_SYS_TEST_DEMO					1
#define USE_FREERTOS_TCP_STACK				1

/* Logging module configuration. */
#define mainLOGGING_TASK_STACK_SIZE         256
#define mainLOGGING_TASK_PRIORITY           (tskIDLE_PRIORITY + 1)
#define mainLOGGING_QUEUE_LENGTH            100

/* Board Support Data Structures. */
sci_hdl_t xSerialSciHandle;
dtc_transfer_data_t xSerialTxDtcInfo;

void R_FIT_Board_Support_Settings(void);
void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength );

void vPrintStringToUart( const char *x )
{
	vSerialPutString(0, x, strlen(x));
}

/* End user code. Do not edit comment generated here */

void main_task(void *pvArgs)
{
	BaseType_t xRet;
	UBaseType_t uxCounter = 0;

	R_FIT_Board_Support_Settings();
	R_BSP_InterruptsEnable();

    xRet = xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                                   mainLOGGING_TASK_PRIORITY,
                                   mainLOGGING_QUEUE_LENGTH );

#if USE_FREERTOS_TCP_STACK
	extern void app_main(void);
	//app_main();
#endif /* USE_FREERTOS_TCP_STACK */

#if BASIC_SYS_TEST_DEMO
	while(1)
	{
		LED0 = !LED0;
		vTaskDelay(pdMS_TO_TICKS(100));
		//R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS); /* Example use of busy delay */
		configPRINTF(("Print test: %u\n\r", uxCounter++));
	}
#endif /* BASIC_SYS_TEST_DEMO */

}

void main(void)
{

}

void R_FIT_Board_Support_Settings(void)
{
    /* Do not call any functions which enables generating any interrupt requests. */

    /* GPIO for LED */
    R_GPIO_PinWrite(U_GPIO_PIN_LED0, (gpio_level_t)LED_OFF); // for the initial level after input --> output
    R_GPIO_PinDirectionSet(U_GPIO_PIN_LED0, GPIO_DIRECTION_OUTPUT);

    /* GPIO for SW */
    R_GPIO_PinDirectionSet(U_GPIO_PIN_SW1, GPIO_DIRECTION_INPUT );

    /* FreeRTOS CLI Command Console */
    U_SCI_UART_CLI_PINSET();
    sci_cfg_t xSerialSciConfig;
    xSerialSciConfig.async.baud_rate    = 115200;
    xSerialSciConfig.async.clk_src      = SCI_CLK_INT;
    xSerialSciConfig.async.data_size    = SCI_DATA_8BIT;
    xSerialSciConfig.async.parity_en    = SCI_PARITY_OFF;
    xSerialSciConfig.async.parity_type  = SCI_EVEN_PARITY;
    xSerialSciConfig.async.stop_bits    = SCI_STOPBITS_1;
    xSerialSciConfig.async.int_priority = 1; /* lowest at first. */
    R_SCI_Open(U_SCI_UART_CLI_SCI_CH, SCI_MODE_ASYNC, &xSerialSciConfig, vSerialSciCallback, &xSerialSciHandle);
    R_DTC_Open();
    R_DTC_Control(DTC_CMD_DTC_START, NULL, NULL);
}
