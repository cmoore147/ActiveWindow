/*
 * Amazon FreeRTOS MQTT Echo Demo V1.4.6
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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


/**
 * @file aws_hello_world.c
 * @brief Network Master Controller Main function.
 *
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "queue.h"

/* FreeRTOS+TCP includes. */
//#include "FreeRTOS_IP.h"
#include "C:/Users/moore/Documents/ConnorHW/Spring 2021/ActiveWindow/NetworkMasterController/amazon-freertos/lib/FreeRTOS-Plus-TCP/include/FreeRTOS_Sockets.h"

/* MQTT includes. */
#include "aws_mqtt_agent.h"
#include "aws_wifi.h"
#include "aws_secure_sockets.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "aws_hello_world.h"

//#include "aws_simple_tcp_echo_server.h"
//#include "aws_demo_config.h"

/* peripheral includes */
#include "Board_LED.h"
#include "Board_Buttons.h"

/* DAVE includes */
#include <Dave.h>

/* NMC includes */
#include "nmc_variables.h"

/* The task delay for allowing the lower priority logging task to print out Wi-Fi
 * failure status before blocking indefinitely. */
#define mainLOGGING_WIFI_STATUS_DELAY       pdMS_TO_TICKS( 1000 )

/**
 * @brief MQTT client ID.
 *
 * It must be unique per MQTT broker.
 */
#define echoCLIENT_ID            ( ( const uint8_t * ) "xmc4800" )

/**
 * @brief The topic that the MQTT client both subscribes and publishes to.
 */
#define command_TOPIC		 	 ( ( const uint8_t * ) "windowCommandTopic" )
#define status_TOPIC			 ( ( const uint8_t * )  "windowStatusTopic" )
#define connection_TOPIC		 ( ( const uint8_t * ) "AWSconnectionTopic" )
int NMC_PORT = 1000;
/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 */
#define statusMAX_DATA_LENGTH      25
#define commandMAX_DATA_LENGTH     25

/**
 * @brief A block time of 0 simply means "don't block".
 */
#define echoDONT_BLOCK           ( ( TickType_t ) 0 )

/*-----------------------------------------------------------*/

/**
 * @brief Creates an MQTT client and then connects to the MQTT broker.
 *
 * The MQTT broker end point is set by clientcredentialMQTT_BROKER_ENDPOINT.
 *
 * @return pdPASS if everything is successful, pdFAIL otherwise.
 */
static BaseType_t prvCreateClientAndConnectToBroker( void );

/**
 * @brief The callback registered with the MQTT client to get notified when
 * data is received from the broker.
 *
 * @param[in] pvUserData User data as supplied while registering the callback.
 * @param[in] pxCallbackParams Data received from the broker.
 *
 * @return Indicates whether or not we take the ownership of the buffer containing
 * the MQTT message. We never take the ownership and always return eMQTTFalse.
 */
static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxCallbackParams );

static MQTTBool_t prvConnectionCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxCallbackParams );

/**
 * @brief Subscribes to the echoTOPIC_NAME topic.
 *
 * @return pdPASS if subscribe operation is successful, pdFALSE otherwise.
 */
static BaseType_t prvSubscribe( const uint8_t * Topic, int mode );
/*-----------------------------------------------------------*/
/**
 * @brief The FreeRTOS message buffer that is used to send data from the callback
 * function (see prvMQTTCallback() above) to the task that echoes the data back to
 * the broker.
 */
static MessageBufferHandle_t xCommandMsgBuffer = NULL;

//buffer used for testing AWS connection
static MessageBufferHandle_t xConnectionBuffer = NULL;

/**
 * @ brief The handle of the MQTT client object used by the MQTT echo demo.
 */
static MQTTAgentHandle_t xMQTTHandle = NULL;

/*-----------------------------------------------------------*/


static void prvMacForHumans(uint8_t * humanAddress);
static uint8_t thing_mac_address[ ( wificonfigMAX_BSSID_LEN * 2 ) + 1 ];
int CheckWifi( int count );
int connect2AWS( void );
int prvWifiConnect( void );
MQTTAgentReturnCode_t prvSend2AWS(char *cDataBuffer, const uint8_t * CANmsg);
int prvSendCAN( char * message);

static BaseType_t prvCreateClientAndConnectToBroker( void )
{
	if(xMQTTHandle != NULL)
	{
		configPRINTF(("oldMQTT Handler still there\r\n"));
	}

    MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn = pdFAIL;
    configPRINTF( ( "My Client ID is [%s]\r\n", thing_mac_address) );
    MQTTAgentConnectParams_t xConnectParameters =
    {
        clientcredentialMQTT_BROKER_ENDPOINT, /* The URL of the MQTT broker to connect to. */
        democonfigMQTT_AGENT_CONNECT_FLAGS,   /* Connection flags. */
        pdFALSE,                              /* Deprecated. */
        clientcredentialMQTT_BROKER_PORT,     /* Port number on which the MQTT broker is listening. Can be overridden by ALPN connection flag. */
        thing_mac_address,                        /* Client Identifier of the MQTT client. It should be unique per broker. */
        0,                                    /* The length of the client Id, filled in later as not const. */
        pdFALSE,                              /* Deprecated. */
        NULL,                                 /* User data supplied to the callback. Can be NULL. */
        NULL,                                 /* Callback used to report various events. Can be NULL. */
        NULL,                                 /* Certificate used for secure connection. Can be NULL. */
        0                                     /* Size of certificate used for secure connection. */
    };

    /* Check this function has not already been executed. */
    configASSERT( xMQTTHandle == NULL );

    /* The MQTT client object must be created before it can be used.  The
     * maximum number of MQTT client objects that can exist simultaneously
     * is set by mqttconfigMAX_BROKERS. */
    xReturned = MQTT_AGENT_Create( &xMQTTHandle );

    if( xReturned == eMQTTAgentSuccess )
    {
        /* Fill in the MQTTAgentConnectParams_t member that is not const,
         * and therefore could not be set in the initializer (where
         * xConnectParameters is declared in this function). */
        xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) echoCLIENT_ID );

        /* Connect to the broker. */
        configPRINTF( ( "attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
                                        &xConnectParameters,
                                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

        if( xReturned != eMQTTAgentSuccess )
        {
            /* Could not connect, so delete the MQTT client. */
            ( void ) MQTT_AGENT_Delete( xMQTTHandle );
            configPRINTF( ( "ERROR:  failed to connect with error %d.\r\n", xReturned ) );
        }
        else
        {
            configPRINTF( ( "connected.\r\n" ) );
            xReturn = pdPASS;
        }
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

static void prvNMCmain()
{
	// msg from AWS
	char cDataBuffer[ statusMAX_DATA_LENGTH ];
	size_t xBytesReceived;
	int reconnectFlag = 1;

	//msg send back from Motor Module
	int CANmsg[ statusMAX_DATA_LENGTH ];

	reconnectFlag = connect2AWS();
	int waitforResponseFlag = 0;
	int count = 1;

	//Socket_t xListeningSocket
	//xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET,
	//	                                        FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
	//	                                        FREERTOS_IPPROTO_TCP );

    for( ; ; )
    {
    	if(CheckWifi(count) == 1){
    		if(reconnectFlag == 1){//initially ==1 and will after checkWiFi fails
    			//reconnectFlag = connect2AWS();
    			//vTaskDelay(3*1000);
    		}
    		else
    		{
    			configPRINTF(("%d\r\n",count));
    			count++;
				memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
				//buffer that receives msg from AWS

				xBytesReceived = xMessageBufferReceive( xCommandMsgBuffer,
														cDataBuffer,
														sizeof( cDataBuffer ),
														0x01000);// time to wait until returning
														//portMAX_DELAY );

				/* ~Message from AWS~ */
				if(waitforResponseFlag == 0 && xBytesReceived > ( sizeof(cDataBuffer)-sizeof(cDataBuffer) ) )
				{
					configPRINTF( ( "Message from AWS topic '%s': '%s'\r\n\n",
									command_TOPIC, cDataBuffer ) );


					waitforResponseFlag = 0;
					configPRINT(("Sending CAN Message \r\n"));

						if(prvSendCAN(cDataBuffer) != CAN_NODE_STATUS_SUCCESS)
							{
							configPRINTF((" Error: Sending CAN message failed \r\n "));
							}

					prvSend2AWS("ACK from NMC",status_TOPIC);

				}

				/* ~Message from CAN~ */
				else if(uxQueueMessagesWaiting(xCANqueue))
				{
				memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
				//configPRINTF(("got message from CAN\r\n"));
				int delayTime = 0x00;
				xQueueReceive(xCANqueue,&CANmsg,delayTime);


					/* ~Response to command from AWS~ */
					if(CANmsg[1]== AWS_ACK)
					{
						configPRINTF((" CAN msg: %d, %d, %d \r\n",CANmsg[0],CANmsg[1],CANmsg[2]));
						waitforResponseFlag = 0;
					}

					/* ~Local Button Operated~ */
					else if(CANmsg[1]== STATUS_UPDATE)
					{
						configPRINTF((" Received Status Update\r\n"));
						waitforResponseFlag = 0;
					}

					/* ~Device need to be added to System~ */
					else if(CANmsg[1]== ADD_DEVICE)
					{
						configPRINTF((" Device: %d added to system: Status: %d \r\n",CANmsg[0], CANmsg[1]));
						//update local lookupTable
						waitforResponseFlag = 0;
					}
					else
					{
						configPRINTF(("Not a response or an error somewhere\r\n"));
					}

				/* ~ Send to AWS in JSON format ~ */
				snprintf(cDataBuffer,statusMAX_DATA_LENGTH,
									 "{\"ID\":\"%d\","
									 "\"Status\":\"%u\""
									 "}",CANmsg[0],CANmsg[2]);

				if(prvSend2AWS(cDataBuffer,status_TOPIC) != eMQTTAgentSuccess){
					// encompasses both timeout case and failure case
					//disconnect = 1;
					configPRINTF(("failed\r\n"));
					}

				}

    		}//if(reconnectFlag == 0)

    	}//checkWifi

    	else
    		{
    		//IotMqtt_Disconnect(); <----2/15 this is where I was last google how to handle MQTT reconnection
    		//if(MQTT_AGENT_Disconnect(xMQTTHandle,5*1000) == eMQTTAgentSuccess){
    		//	( void ) MQTT_AGENT_Delete( xMQTTHandle );
    		//	vTaskDelay(3*1000);
    		//	reconnectFlag = prvWifiConnect();

    		//inital Socket setup
    		//prvDirectConnection(NMC_PORT);

    		}

    		vTaskDelay(3*1000);

    		//reconnectFlag = 0;
    		//}
    }//forloop
}
/*-----------------------------------------------------------*/

static BaseType_t prvSubscribe( const uint8_t * Topic, int mode )
{
	MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn;
    MQTTAgentSubscribeParams_t xSubscribeParams;

    /* Setup subscribe parameters to subscribe to commandTOPIC_NAME topic. */
    xSubscribeParams.pucTopic = Topic;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    if(mode ==1){xSubscribeParams.pxPublishCallback = prvMQTTCallback;}
    else{xSubscribeParams.pxPublishCallback = prvConnectionCallback;}
    xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) Topic);
    //xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) echoTOPIC_NAME );
    xSubscribeParams.xQoS = eMQTTQoS1;

    /* Subscribe to the topic. */
    xReturned = MQTT_AGENT_Subscribe( xMQTTHandle,
                                      &xSubscribeParams,
                                      democonfigMQTT_TIMEOUT );

    if( xReturned == eMQTTAgentSuccess )
    {
        configPRINTF( ( "NMC subscribed to %s\r\n", Topic ) );
        xReturn = pdPASS;
    }
    else
    {
        configPRINTF( ( "ERROR:  NMC could not subscribe to %s\r\n", Topic ) );
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/
static MQTTBool_t prvConnectionCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters)
{
	char cBuffer[ 2 ];
	    uint32_t ulBytesToCopy = ( 1 );
	    /* Remove warnings about the unused parameters. */
	    ( void ) pvUserData;

	    /* Don't expect the callback to be invoked for any other topics. */
	    //configASSERT( ( size_t ) ( pxPublishParameters->usTopicLength ) == strlen( ( const char * ) connection_TOPIC ) );
	    //configASSERT( memcmp( pxPublishParameters->pucTopic, connection_TOPIC, ( size_t ) ( pxPublishParameters->usTopicLength ) ) == 0 );


	    if( pxPublishParameters->ulDataLength <= ulBytesToCopy )
	    {
	        ulBytesToCopy = pxPublishParameters->ulDataLength;

	        memset( cBuffer, 0x00, sizeof( cBuffer ) );
	        memcpy( cBuffer, pxPublishParameters->pvData, ( size_t ) ulBytesToCopy );

	        ( void ) xMessageBufferSend( xConnectionBuffer,
	        		cBuffer,
					( size_t ) ulBytesToCopy + ( size_t ) 1, echoDONT_BLOCK );
	    }
	    else
	    {
	        configPRINTF( ( "[WARN]: Dropping received message as it does not fit in the buffer.\r\n" ) );
	    }

	    return eMQTTFalse;
}


//reads form msg buffer for the command
static MQTTBool_t prvMQTTCallback( void * pvUserData,
                                   const MQTTPublishData_t * const pxPublishParameters)
{
    char cBuffer[ commandMAX_DATA_LENGTH ];
    uint32_t ulBytesToCopy = ( commandMAX_DATA_LENGTH - 1 ); /* Bytes to copy initialized to ensure it
                                                                                   * fits in the buffer. One place is left
                                                                                   * for NULL terminator. */

    /* Remove warnings about the unused parameters. */
    ( void ) pvUserData;

    /* Don't expect the callback to be invoked for any other topics. */
    configASSERT( ( size_t ) ( pxPublishParameters->usTopicLength ) == strlen( ( const char * ) command_TOPIC ) );
    configASSERT( memcmp( pxPublishParameters->pucTopic, command_TOPIC, ( size_t ) ( pxPublishParameters->usTopicLength ) ) == 0 );

    /* THe ulBytesToCopy has already been initialized to ensure it does not copy
     * more bytes than will fit in the buffer.  Now check it does not copy more
     * bytes than are available. */
    if( pxPublishParameters->ulDataLength <= ulBytesToCopy )
    {
        ulBytesToCopy = pxPublishParameters->ulDataLength;

        /* Set the buffer to zero and copy the data into the buffer to ensure
         * there is a NULL terminator and the buffer can be accessed as a
         * string. */
        memset( cBuffer, 0x00, sizeof( cBuffer ) );
        memcpy( cBuffer, pxPublishParameters->pvData, ( size_t ) ulBytesToCopy );

        ( void ) xMessageBufferSend( xCommandMsgBuffer,
        		cBuffer,
				( size_t ) ulBytesToCopy + ( size_t ) 1, echoDONT_BLOCK );
    }
    else
    {
        configPRINTF( ( "[WARN]: Dropping received message as it does not fit in the buffer.\r\n" ) );
    }

    /* The data was copied into the FreeRTOS message buffer, so the buffer
     * containing the data is no longer required.  Returning eMQTTFalse tells the
     * MQTT agent that the ownership of the buffer containing the message lies with
     * the agent and it is responsible for freeing the buffer. */
    return eMQTTFalse;
}
/*-----------------------------------------------------------*/

static void prvDirectConnection()
{
	struct freertos_sockaddr xClient, xBindAddress;
	Socket_t xListeningSocket, xConnectedSocket;
	//socklen_t xSize = sizeof( xClient );
	//static const TickType_t xReceiveTimeOut = portMAX_DELAY;
	const BaseType_t xBacklog = 20;
	int BUFFER_SIZE = 10;
	BaseType_t lBytesReceived;
	char cRxedData[ BUFFER_SIZE ];

	    /* Attempt to open the socket. */
	    xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET,
	                                        FREERTOS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
	                                        FREERTOS_IPPROTO_TCP );

	    /* Check the socket was created. */
	    configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );

	    /* If FREERTOS_SO_RCVBUF or FREERTOS_SO_SNDBUF are to be used with
	    FreeRTOS_setsockopt() to change the buffer sizes from their default then do
	    it here!.  (see the FreeRTOS_setsockopt() documentation. */

	    /* If ipconfigUSE_TCP_WIN is set to 1 and FREERTOS_SO_WIN_PROPERTIES is to
	    be used with FreeRTOS_setsockopt() to change the sliding window size from
	    its default then do it here! (see the FreeRTOS_setsockopt()
	    documentation. */

	    /* Set a time out so accept() will just wait for a connection. */
	    //FreeRTOS_setsockopt( xListeningSocket,
	    //                     0,
	    //                     FREERTOS_SO_RCVTIMEO,
	    //                     &xReceiveTimeOut,
	    //                     sizeof( xReceiveTimeOut ) );

	    /* Set the listening port to 10000. */
	    xBindAddress.sin_port = ( uint16_t ) NMC_PORT;
	    //xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );

	    /* Bind the socket to the port that the client RTOS task will send to. */
	    FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );

	    /* Set the socket into a listening state so it can accept connections.
	    The maximum number of simultaneous connections is limited to 20. */
	    FreeRTOS_listen( xListeningSocket, xBacklog );

	    for( ;; )
	    {
	    	lBytesReceived = FreeRTOS_recv( xListeningSocket, &cRxedData, BUFFER_SIZE, 0 );

	    	        if( lBytesReceived > 0 )
	    	        {
	    	            /* Data was received, process it here. */

	    	        }
	    	        else if( lBytesReceived == 0 )
	    	        {
	    	            /* No data was received, but FreeRTOS_recv() did not return an error.
	    	            Timeout? */
	    	        }
	    	        else
	    	        {
	    	            /* Error (maybe the connected socket already shut down the socket?).
	    	            Attempt graceful shutdown. */
	    	            FreeRTOS_shutdown( xListeningSocket, FREERTOS_SHUT_RDWR );
	    	            break;
	    	        }
	    }
	}

void vStartMQTTEchoDemo()
{
    configPRINTF( ( "Starting NMC...\r\n" ) );
    //BaseType_t xReturned;

    //prvMacForHumans(thing_mac_address);// used to identify the identity of thing connecting to AWS
    LED_Initialize();

    //-----------------------------------------------------------
    //xReturned = prvCreateClientAndConnectToBroker();

    /* Create the message buffer used to pass strings from the MQTT callback
         * function to the task that echoes the strings back to the broker.  The
         * message buffer will only ever have to hold one message as messages are only
         * published every 5 seconds.  The message buffer requires that there is space
         * for the message length, which is held in a size_t variable. */
    //xCommandMsgBuffer = xMessageBufferCreate( ( size_t ) commandMAX_DATA_LENGTH + sizeof( size_t ) );
    //configASSERT( xCommandMsgBuffer );



    xTaskCreate(prvNMCmain,
    	             "AWS",
    	             democonfigMQTT_ECHO_TASK_STACK_SIZE, /* Size of the stack to allocate for the task, in words not bytes! */
    	             NULL,                                /* The task parameter is not used. */
    	             tskIDLE_PRIORITY,                    /* Runs at the lowest priority. */
    	             NULL);                 /* The handle is stored so the created task can be deleted again at the end of the demo. */

    xTaskCreate( prvDirectConnection,
                     "TCP/IP",
					 democonfigMQTT_ECHO_TASK_STACK_SIZE,
                     NULL,
					 tskIDLE_PRIORITY,
                     NULL );
    //}
    //---------------------------------------------------------------

}
/*-----------------------------------------------------------*/

int CheckWifi(int count)
{
	int check =0;
	//configPRINTF(("Checking----\r\n"));
	check = WIFI_IsConnected();
	if( check == 0 ){
		configPRINTF(("Disconnected: %d\r\n", check));
		//WIFI_Disconnect();
		return check;
	}
	else{
		configPRINTF((" Connected: %d\r\n",check));
		if(count % 10 == 0)
		{
			prvSend2AWS("1",connection_TOPIC);
		}
		return check;


		//char cAWSconnection[4];
		//size_t xBytesReceived;

		//xBytesReceived = xMessageBufferReceive( xConnectionBuffer,
		//										cAWSconnection,
		//										sizeof( cAWSconnection ),
		//										0x01000);// time to wait until returning
												//portMAX_DELAY );

		//if(xBytesReceived > sizeof(cAWSconnection)-sizeof(cAWSconnection)){
		//	configPRINTF((" Connected\r\n"));
		//	memset(cAWSconnection,0x00, sizeof(cAWSconnection));
		//	return 1;
		//}
		//else{
		//	configPRINTF((" Disconnected to AWS\r\n"));
		//	memset(cAWSconnection,0x00, sizeof(cAWSconnection));
		//	return 0;
		//}

	}
	return 1;
}

/*-----------------------------------------------------------*/

int connect2AWS()
{
	BaseType_t xReturned1;
	MQTTAgentReturnCode_t xReturned;

	prvMacForHumans(thing_mac_address);// used to identify the identity of thing connecting to AWS

	xReturned1 = prvCreateClientAndConnectToBroker();

	xCommandMsgBuffer = xMessageBufferCreate( ( size_t ) commandMAX_DATA_LENGTH + sizeof( size_t ) );
	configASSERT( xCommandMsgBuffer );

	xConnectionBuffer = xMessageBufferCreate( ( size_t ) 3 + sizeof( size_t ) );
	configASSERT( xConnectionBuffer );

	/* Check this task has not already been created. */
	configASSERT( xMQTTHandle != NULL );
	//configASSERT( xCommandMsgBuffer != NULL );

	if( xReturned1 == pdPASS )
		{
		xReturned = prvSubscribe(command_TOPIC,1);

		if( xReturned == pdPASS )
			{
			prvSend2AWS("NMC Connected to AWS",status_TOPIC);
			return 0;
			//xReturned = prvSubscribe(connection_TOPIC,0);
			//configPRINTF(("prvStatus Successfully Subscribed to topic\n"));

			if( xReturned == pdPASS )
				{
				return 0;//reconnectFlag
				}
			}
		}
	return 1;//reconnectFlag
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

MQTTAgentReturnCode_t prvSend2AWS(char * cDataBuffer, const uint8_t *Topic){

	MQTTAgentPublishParams_t xPublishParameters;
	MQTTAgentReturnCode_t xReturned;


	//char * temp =strcat(Topic,"/");
	//temp=strcat(status_TOPIC,CANmsg[0]);
	xPublishParameters.pucTopic = Topic;
	//xPublishParameters.pucTopic = temp;
	xPublishParameters.pvData = cDataBuffer;
	xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) Topic );
	//xPublishParameters.ulDataLength = ( uint32_t ) strlen( &CANmsg );
	xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );
	xPublishParameters.xQoS = eMQTTQoS1;

	/* Publish the message. */
	xReturned = MQTT_AGENT_Publish( xMQTTHandle, &( xPublishParameters ), democonfigMQTT_TIMEOUT );

	if( xReturned == eMQTTAgentSuccess )
		{
		configPRINTF( ( "Successfully published '%s' to '%s'\r\n", cDataBuffer, Topic ) );
		return xReturned;
		}
	else
		{
			configPRINTF( ( "ERROR:  Failed to publish '%s' to '%s'\r\n", cDataBuffer, Topic ) );
			return xReturned;
		}
}

int char2Int(char * data)
{
	//664
	//752
	//percent
    int hashval;
    //configPRINTF(("%s/n",data));
    for (hashval = 0; *data != '\0'; data++){
    	if(*data == '"'){continue;}
    	else{hashval = tolower(*data) + 1 * hashval;}
    }
    return hashval;
}

int prvSendCAN(char * command)
{
	//Need to figure out where number is converted to string
	int ID = char2Int( strtok( command, ":" ) );
	int Function = char2Int( strtok( NULL,":") );
	char * temp1= strtok( NULL,":");
	strncpy(temp1,temp1,(strlen(temp1)-1));

	int Parameter = atoi(temp1);

	configPRINTF(("%d, %d, %d \r\n",ID,Function,Parameter));

	CAN_Node_LMO_02_Config.mo_ptr->can_data_word[0] = ID;
	CAN_Node_LMO_02_Config.mo_ptr->can_data_word[1] = Function;
	CAN_Node_LMO_02_Config.mo_ptr->can_data_word[2] = Parameter;

	uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(CAN_Node_LMO_02_Config.mo_ptr);
	status = CAN_NODE_MO_Transmit(&CAN_Node_LMO_02_Config);

	return status;
}
/*-----------------------------------------------------------*/

static void prvMacForHumans(uint8_t * humanAddress)
 {
     uint8_t buf[ wificonfigMAX_BSSID_LEN ];
     char cbuf[2];
     uint8_t idx = 0;

     WIFI_GetMAC( ( uint8_t * )  buf);

     // get the mac address in a string
     for (idx = 0; idx < wificonfigMAX_BSSID_LEN; idx++)
     {
         sprintf(cbuf, "%x", buf[idx]);
         if ( cbuf[1] == '\0')
         {
             humanAddress[( idx * 2 )] = '0';
             humanAddress[( idx * 2 ) + 1] = cbuf[0];

         }
         else
         {
             humanAddress[( idx * 2 )] = cbuf[0];
             humanAddress[( idx * 2 ) + 1] = cbuf[1];
         }
     }

     humanAddress[ ( wificonfigMAX_BSSID_LEN * 2 ) ] = '\0';
 }
