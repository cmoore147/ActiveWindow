/*
 * Amazon FreeRTOS V1.4.6
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#include <string.h>

#include <DAVE.h>                 //Declarations from DAVE Code Generation (includes SFR declaration)

#include "console_io.h"
#include "entropy_hardware.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes */
#include "aws_demo_runner.h"

/* AWS library includes. */
#include "aws_system_init.h"
#include "aws_logging_task.h"
#include "aws_wifi.h"
#include "aws_clientcredential.h"
#include "aws_application_version.h"
#include "aws_dev_mode_key_provisioning.h"

/* NMC includes */
#include "nmc_variables.h"

/* Declare the firmware version structure for all to see. */
const AppVersion32_t xAppFirmwareVersion = {
	.u.x.ucMajor = APP_VERSION_MAJOR,
	.u.x.ucMinor = APP_VERSION_MINOR,
	.u.x.usBuild = APP_VERSION_BUILD,
};

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 8 )

/* The task delay for allowing the lower priority logging task to print out Wi-Fi 
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/* The name of the devices for xApplicationDNSQueryHook. */
#define mainDEVICE_NICK_NAME                "XMC4800_IoTKit_Demo"


/**
 * @brief Application task startup hook for applications using Wi-Fi. If you are not 
 * using Wi-Fi, then start network dependent applications in the vApplicationIPNetorkEventHook
 * function. If you are not using Wi-Fi, this hook can be disabled by setting 
 * configUSE_DAEMON_TASK_STARTUP_HOOK to 0.
 */
void vApplicationDaemonTaskStartupHook( void );

/**
 * @brief Connects to Wi-Fi.
 */
int prvWifiConnect( void );

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

/* Ensure the FreeRTOS heap is not crossing SRAM boundaries */
__attribute__((section("ETH_RAM"))) uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];


/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int main( void )
{

    prvMiscInitialization();

    /* Create tasks that are not dependent on the Wi-Fi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the Wi-Fi initialization, is performed in the RTOS daemon task
     * startup hook. */
    vTaskStartScheduler();

    return 0;
}
/*-----------------------------------------------------------*/
/* CAN_Node_Handler()
 * - used to receive command acknowledgment and status messages from window
 */
void CAN_Node_Handler( void ) {

	//portBASE_TYPE status;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE ;


	/* Check for Node error */
	if(CAN_NODE_GetStatus(&CAN_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
	{}
	else if(CAN_NODE_MO_GetStatus(&CAN_Node_LMO_01_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
	{
		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
		CAN_NODE_MO_Receive(&CAN_Node_LMO_01_Config);

		int msg[3] = {CAN_Node_LMO_01_Config.mo_ptr->can_data_word[0],
				CAN_Node_LMO_01_Config.mo_ptr->can_data_word[1],
				CAN_Node_LMO_01_Config.mo_ptr->can_data_word[2]};

		xQueueSendFromISR(xCANqueue,&msg, &xHigherPriorityTaskWoken );

	}
	//portEXIT_SWITCHING_ISR( xHigherPriorityTaskWoken );

}

/*-----------------------------------------------------------*/
static void prvMiscInitialization( void )
{
#if defined(configPRINT_SERIALIF)
	CONSOLE_IO_Init();
#endif
#if defined(configPRINT_DEBUGIF)
	/* To compiler need to specify -specs=rdimon.specs in linker miscellaneous preferences tab, other flags
	 * To work properly enable semihosting in debugger settings */
	//TODO: Figure this out
	extern void initialise_monitor_handles(void);
	initialise_monitor_handles();
#endif

	DAVE_Init();
	ENTROPY_HARDWARE_Init();
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    vDevModeKeyProvisioning();

    /* Initialize the AWS Libraries system. */
    if ( SYSTEM_Init() == pdPASS )
    {
    	/*xQueueCreate( elements allowed in queue, size in bytes of element); */
    	xCANqueue = xQueueCreate( 3, 16 );
    	configASSERT( xCANqueue );


    	prvWifiConnect();

        DEMO_RUNNER_RunDemos();
    }

}
/*-------------------------------------------//
 * Returns:
 * 	 1 (on failing to connect)
 *   0 (with connecting)
 * ----------------*/
int prvWifiConnect( void )
{
    /* FIX ME: Delete surrounding compiler directives when the Wi-Fi library is ported. */
        WIFINetworkParams_t xNetworkParams;
        WIFIReturnCode_t xWifiStatus;
        uint8_t ucTempIp[4] = { 0 };
        int wifi_pass = 1;
        int wifi_fail = 0;

        xWifiStatus = WIFI_On();

        if( xWifiStatus == eWiFiSuccess )
        {
        	uint8_t mac_address[6];
        	WIFI_GetMAC(mac_address);
            configPRINTF( ( "Wi-Fi module initialized (MAC Address: %02x:%02x:%02x:%02x:%02x:%02x).\r\n",
            		         mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5] ) );
            configPRINTF( ( "Connecting to AP...\r\n" ) );
        }
        else
        {
            configPRINTF( ( "Wi-Fi module failed to initialize.\r\n" ) );

            /* Delay to allow the lower priority logging task to print the above status. 
             * The while loop below will block the above printing. */
            vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

            while( 1 )
            {
            }

        }

        /* Setup parameters. */
        xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
        xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
        xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
        xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
        xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;
        xNetworkParams.cChannel = 0;

        xWifiStatus = WIFI_ConnectAP( &( xNetworkParams ) );

        if( xWifiStatus == eWiFiSuccess )
        {
            configPRINTF( ( "Wi-Fi Connected to AP. Creating tasks which use network...\r\n" ) );
            
            xWifiStatus = WIFI_GetIP( ucTempIp );
            if ( eWiFiSuccess == xWifiStatus ) 
            {
                configPRINTF( ( "IP Address acquired %d.%d.%d.%d\r\n",
                                ucTempIp[ 0 ], ucTempIp[ 1 ], ucTempIp[ 2 ], ucTempIp[ 3 ] ) );
            }
            return wifi_pass;
        }
        else
        {
            configPRINTF( ( "Wi-Fi failed to connect to AP.\r\n" ) );
            return wifi_fail;
            /* Delay to allow the lower priority logging task to print the above status. 
             * The while loop below will block the above printing. */
            //vTaskDelay( mainLOGGING_WIFI_STATUS_DELAY );

            //while( 1 )
            //{
            //}
        }
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide memory that is used by the Idle task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() in order to provide memory to
 * the Idle task.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, then application must provide an
 * implementation of vApplicationGetTimerTaskMemory() in order to provide memory
 * to the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
/*-----------------------------------------------------------*/

/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook()
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    char * pcTaskName )
{
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined Idle task function.
 *
 * @note Do not make any blocking operations in this function.
 */
void vApplicationIdleHook( void )
{
    static TickType_t xLastPrint = 0;
    TickType_t xTimeNow;
    const TickType_t xPrintFrequency = pdMS_TO_TICKS( 5000 );

    xTimeNow = xTaskGetTickCount();

    if( ( xTimeNow - xLastPrint ) > xPrintFrequency )
    {
        configPRINT( "." );
        xLastPrint = xTimeNow;
    }
}
/*-----------------------------------------------------------*/

/**
* @brief User defined application hook to process names returned by the DNS server.
*/
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 )
    BaseType_t xApplicationDNSQueryHook( const char * pcName )
    {
        /* FIX ME. If necessary, update to applicable DNS name lookup actions. */

        BaseType_t xReturn;

        /* Determine if a name lookup is for this node.  Two names are given
         * to this node: that returned by pcApplicationHostnameHook() and that set
         * by mainDEVICE_NICK_NAME. */
        if( strcmp( pcName, pcApplicationHostnameHook() ) == 0 )
        {
            xReturn = pdPASS;
        }
        else if( strcmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
        {
            xReturn = pdPASS;
        }
        else
        {
            xReturn = pdFAIL;
        }

        return xReturn;
    }
	
#endif /* if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) */
/*-----------------------------------------------------------*/

/**
 * @brief User defined assertion call. This function is plugged into configASSERT.
 * See FreeRTOSConfig.h to define configASSERT to something different.
 */
void vAssertCalled(const char * pcFile,
	uint32_t ulLine)
{
    /* FIX ME. If necessary, update to applicable assertion routine actions. */

	const uint32_t ulLongSleep = 1000UL;
	volatile uint32_t ulBlockVariable = 0UL;
	volatile char * pcFileName = (volatile char *)pcFile;
	volatile uint32_t ulLineNumber = ulLine;

	(void)pcFileName;
	(void)ulLineNumber;

	printf("vAssertCalled %s, %ld\n", pcFile, (long)ulLine);
	fflush(stdout);

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
	* this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while (ulBlockVariable == 0UL)
		{
			vTaskDelay( pdMS_TO_TICKS( ulLongSleep ) );
		}
	}
	taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/**
 * @brief User defined application hook need by the FreeRTOS-Plus-TCP library.
 */
#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    const char * pcApplicationHostnameHook(void)
    {
        /* FIX ME: If necessary, update to applicable registration name. */

        /* This function will be called during the DHCP: the machine will be registered 
         * with an IP address plus this name. */
        return clientcredentialIOT_THING_NAME;
    }

#endif
