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
#include "timers.h"


/* MQTT includes. */
#include "aws_mqtt_agent.h"
#include "aws_wifi.h"
#include "aws_secure_sockets.h"

/* Credentials includes. */
#include "aws_clientcredential.h"

/* Demo includes. */
#include "aws_demo_config.h"
#include "aws_hello_world.h"

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


/*
 * Sock Variables
*/
int socketStatus = 1;
Socket_t xLocalSocket;
SocketsSockaddr_t xRemoteAddress;
char * MsgParse[4];
uint8_t phoneIP[5] = { 192, 168, 8, 224 };
int phonePort = 9002;

/*
 * Connection Checking variables
 */
uint8_t UmassIP[5] = {128,119,245,12};
int TCP_HeartBeat = 10;
int InterNetPing = 2;


/**
 * @brief Dimension of the character array buffers used to hold data (strings in
 * this case) that is published to and received from the MQTT broker (in the cloud).
 */
#define statusMAX_DATA_LENGTH      28
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
static BaseType_t prvCreateClientAndConnectToBroker( int delOldHand );

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


/**
 * @brief Subscribes to the windowCommandTOPIC .
 *
 * @return pdPASS if subscribe operation is successful, pdFALSE otherwise.
 */

static BaseType_t prvSubscribe( const uint8_t * Topic);

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


/*
 * Determines current communication channel available
 * Connects and re-establishes AWS connection
 */
void CheckOrChangeConnection( int count);
void connect2AWS( int delOldHand );

/*
 * Sends MQTT messages to AWS
 */
MQTTAgentReturnCode_t prvSend2AWS(char *cDataBuffer, const uint8_t * CANmsg);

/*
 * Message handler for CAN communication
 * AWS or TCP_IP string --> CAN msg format
 */

int prvSendCAN( char * message);
int char2Int(char * data);

/* *
 * Socket Functions
 * */
void sendSocket(char * cDataBuffer, const size_t xTotalLengthToSend);
void listenSocket(int blocking,int count);
void setupSocket();
int closeSocket();


/*-----------------------------------------------------------*/
/*
 * Used to connect NMC to AWS IOT core
 */
static BaseType_t prvCreateClientAndConnectToBroker( int delOldHand )
{
	MQTTAgentReturnCode_t xReturned;
	BaseType_t xReturn = pdFAIL;
	//configPRINTF( ( "My Client ID is [%s]\r\n", thing_mac_address) );
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
	if(delOldHand){
		xReturned = MQTT_AGENT_Disconnect(xMQTTHandle,1000);
		if(xReturned == eMQTTAgentSuccess){

			if(MQTT_AGENT_Delete( xMQTTHandle ) == eMQTTAgentSuccess){
				configPRINTF( ( "Deleted old MQTT handler %d.\r\n", xReturned ) );
				xReturn = pdPASS;
			}

		}else{
			configPRINTF(("ERROR: could not Delete %x",xReturned));
		}
		return xReturn;
	}

	else if(xMQTTHandle != NULL)
	{
		configPRINTF(("oldMQTT Handler still there\r\n"));
		xConnectParameters.usClientIdLength = ( uint16_t ) strlen( ( const char * ) echoCLIENT_ID );

		        /* Connect to the broker. */
		        configPRINTF( ( "attempting to connect to %s.\r\n", clientcredentialMQTT_BROKER_ENDPOINT ) );
		        xReturned = MQTT_AGENT_Connect( xMQTTHandle,
		                                        &xConnectParameters,
		                                        democonfigMQTT_ECHO_TLS_NEGOTIATION_TIMEOUT );

		        if( xReturned != eMQTTAgentSuccess )
		        {
		            /* Could not connect, so delete the MQTT client. */
		        	xReturned = MQTT_AGENT_Disconnect(xMQTTHandle,1000);
		        	if(xReturned == eMQTTAgentSuccess){

		        		( void ) MQTT_AGENT_Delete( xMQTTHandle );
		        		configPRINTF( ( "ERROR:  failed to connect with error %d.\r\n", xReturned ) );

		        	}else{
		        		configPRINTF(("ERROR: could not Reconnect %x",xReturned));
		        	}
		        }
		        else
		        {
		            configPRINTF( ( "connected.\r\n" ) );
		            xReturn = pdPASS;
		        }
		return xReturn;
	}


    /* Check this function has not already been executed. */
    //configASSERT( xMQTTHandle == NULL );

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
/*
 * Function listens on socket for incoming commands and returns immediately if there is no message
 */
void listenSocket(int blocking,int count)
{
		if(blocking){
			//char * block = "ACK:Message:Dropped\r\n";
			//sendSocket(block,sizeof(block));
			return;
			}

		size_t xbytes;
		char cBuffer[commandMAX_DATA_LENGTH -1];
		uint32_t uBytesToCopy = ( commandMAX_DATA_LENGTH - 1 );
		socketStatus = ENABLED;


		xbytes = SOCKETS_Recv(xLocalSocket,cBuffer,sizeof(cBuffer),0);

		/*============Checks Connection========*/
		if(count%TCP_HeartBeat == 0 ){
			if(cBuffer[0] == '$'){
			configPRINTF(("TCP_IP: Connected\r\n"));
			return;
			}
			else{
				configPRINTF(("TCP_IP: Disconnected\r\n"));
				closeSocket();
				NMC_mode = DISCONNECTED;
				return;
			}
		}
		if(cBuffer[0] == '#'){
			configPRINTF(("TCP_IP: Phone Disconnected\r\n"));
			if(closeSocket() == 0){
				NMC_mode = DISCONNECTED;
			}

			return;
		}/*====================================*/

		else if(xbytes > sizeof(cBuffer)-sizeof(cBuffer) &&  xbytes<= uBytesToCopy)
				{

					( void ) xMessageBufferSend( xCommandMsgBuffer, cBuffer,
										uBytesToCopy + ( size_t ) 1,
										echoDONT_BLOCK );

					memset(cBuffer,0x00, sizeof(cBuffer));
				}
}
/*-----------------------------------------------------------*/
/*
 * Closes socket for TCP/IP connection
 */
int closeSocket(){
	int xReturn = -1;

	xReturn = SOCKETS_Shutdown( xLocalSocket, SOCKETS_SHUT_RD );
	vTaskDelay(500);

			if(xReturn == 0 )
				{
				xReturn = SOCKETS_Close(xLocalSocket);
				if( xReturn == 0){
					//vTaskDelay(1000);
					configPRINTF((" Closing Socket \r\n"));
					socketStatus=DISABLED;
					return xReturn;
				}
				else{configPRINTF((" ERROR: Closing socket failed %d\r\n",xReturn));}
			}
			else
				{
				configPRINTF(("ERROR: Socket failed to shutdown with error %d\r\n",xReturn));
				return xReturn;
				}

}
/*-----------------------------------------------------------*/
/*
 * Sets up connection with phone in TCP/IP mode
 */
int setUpSocket()
{
	int xReturn = 0;
		static const TickType_t xTimeOut = pdMS_TO_TICKS( 2000 );

		if(socketStatus == DISABLED)
		{
				xRemoteAddress.ucSocketDomain = SOCKETS_AF_INET;
				xRemoteAddress.usPort = SOCKETS_htons( phonePort );
				xRemoteAddress.ulAddress = SOCKETS_inet_addr_quick(phoneIP[0],phoneIP[1],
																	phoneIP[2],phoneIP[3]);
				xRemoteAddress.ucLength = sizeof(xRemoteAddress);

				/* Attempt to open the socket. */
				xLocalSocket = SOCKETS_Socket( SOCKETS_AF_INET,
													SOCKETS_SOCK_STREAM,  /* SOCK_STREAM for TCP. */
													SOCKETS_IPPROTO_TCP );

				/* Check the socket was created. */
				configASSERT( xLocalSocket != SOCKETS_INVALID_SOCKET );
				SOCKETS_SetSockOpt(xLocalSocket,0,SOCKETS_SO_RCVTIMEO,&xTimeOut,xTimeOut);

				socketStatus = ENABLED_NOTCONN;
		}
		if (socketStatus == ENABLED_NOTCONN){

			xReturn = SOCKETS_Connect(xLocalSocket,&xRemoteAddress, xRemoteAddress.ucLength );

			if(xReturn != 0){
				configPRINTF((" ERROR: Socket Connection failed with error %d\r\n", xReturn));
				return -1;

			}else{
				configPRINTF(("Socket Connected...\r\n"));
				socketStatus = ENABLED;
				NMC_mode = TCP_IP;
				configPRINTF(("Socket Listening...\r\n"));
				//SOCKETS_SetSockOpt(xLocalSocket,0,SOCKETS_SO_NONBLOCK,NULL,NULL);
				return 0;

			}
		}
}
/*-----------------------------------------------------------*/
/*
 * Socket sending function for TCP/IP connection
 */
void sendSocket(char * cDataBuffer, const size_t xTotalLengthToSend){

	size_t xLenToSend;

	BaseType_t xAlreadyTransmitted = 0, xBytesSend = 0;
	configPRINTF(("Socket Sending: %s\n",cDataBuffer));

	while( xAlreadyTransmitted < xTotalLengthToSend)
	{

		xLenToSend = xTotalLengthToSend - xAlreadyTransmitted;
		xBytesSend = SOCKETS_Send(xLocalSocket,
								&cDataBuffer[xAlreadyTransmitted],
								xLenToSend,
								0);
		if( xBytesSend >= 0)
		{
			xAlreadyTransmitted += xBytesSend;
		}
		else
		{
			break;
		}

	}
}
/*-----------------------------------------------------------*/
/*
 * Brain of the NMC functionality, indefinitely loops through this function
 */
static void prvNMCmain()
{
	int count = 0;
	size_t xBytesReceived;
	int waitforResponseFlag = 0;
	char ack [statusMAX_DATA_LENGTH];

	char cDataBuffer[ statusMAX_DATA_LENGTH ];
	int CANmsg[ statusMAX_DATA_LENGTH ];


	/* Priority Connection is AWS */
	connect2AWS(0);

    for( ; ; )
    {
    	memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
    	configPRINTF(("%d\r\n",count));


    	//vTaskDelay(1000);
    	count+=1;
    	CheckOrChangeConnection(count);
    	vTaskDelay(500);

    	if(NMC_mode == DISCONNECTED){count = 0;}

    	if(NMC_mode == TCP_IP)
    		{
    	  	listenSocket(waitforResponseFlag,count);
    		}


    	xBytesReceived = xMessageBufferReceive( xCommandMsgBuffer,cDataBuffer,
    	    	  								sizeof( cDataBuffer ),0x0500);

				/* ~Message from AWS or TCP/IP~ */
				if(waitforResponseFlag == 0 && xBytesReceived > ( sizeof(cDataBuffer)-sizeof(cDataBuffer) ) )
				{

					count = 0;
					configPRINTF( ( "Message from APP %s\r\n", cDataBuffer ) );



					configPRINTF(("Sending CAN Message \r\n"));


					if(prvSendCAN(cDataBuffer) != CAN_NODE_STATUS_SUCCESS)
						{
						configPRINTF((" Error: Sending CAN message failed \r\n "));
						}

					//vTaskDelay(1*1000);

					/* Send ACK to APP */
					char* ackmsg = "ACK";

					if(NMC_mode == TCP_IP){
						snprintf(ack,statusMAX_DATA_LENGTH,"NMC:0:%s:0\r\n",ackmsg);

						configPRINTF(("%s",ack));

						sendSocket(ack,statusMAX_DATA_LENGTH);
					}

					else{prvSend2AWS(ackmsg,status_TOPIC);}

					/* Block any more Commands until MM Response */
					waitforResponseFlag = 0;//TODO Change to 1 after debugging


				}

				/* ~Message from CAN~ */
				else if(uxQueueMessagesWaiting(xCANqueue))
				{
				memset( cDataBuffer, 0x00, sizeof( cDataBuffer ) );
				//configPRINTF(("got message from CAN\r\n"));
				int delayTime = 0x00;
				xQueueReceive(xCANqueue,&CANmsg,delayTime);




					/* ~Local Button Operated~ */
					if(CANmsg[1]== STATUS_UPDATE)
					{
						configPRINTF((" Status Update: %d, %d, %d \r\n",CANmsg[0],CANmsg[1],CANmsg[2]));
						waitforResponseFlag = 0;
					}

					/* ~Device need to be added to System~ */
					else if(CANmsg[1]== ADD_DEVICE)
					{
						configPRINTF((" Device: %d added to system: Status: %d \r\n",CANmsg[0], CANmsg[1]));

					}
					else
					{
						configPRINTF(("Invalid msg\r\n"));
						continue;

					}

				/* ~ Send to APP as STRING ~ */
				if(NMC_mode == TCP_IP){
					snprintf(cDataBuffer,statusMAX_DATA_LENGTH,"ID:%d:Status:%u\n\r",CANmsg[0],CANmsg[2]);

					sendSocket(cDataBuffer,statusMAX_DATA_LENGTH);
				}
				/* ~ Send to APP in JSON format ~ */
				else{
					snprintf(cDataBuffer,statusMAX_DATA_LENGTH,
														 "{\"ID\":\"%d\","
														 "\"Status\":\"%d\""
														 "}",CANmsg[0],CANmsg[2]);
					prvSend2AWS(cDataBuffer,status_TOPIC);
				}


			}
    	}

}
/*-----------------------------------------------------------*/
/*
 * Subscribe the NMC to AWS message stream
 */
static BaseType_t prvSubscribe( const uint8_t * Topic)

{
	MQTTAgentReturnCode_t xReturned;
    BaseType_t xReturn;
    MQTTAgentSubscribeParams_t xSubscribeParams;

    /* Setup subscribe parameters to subscribe to commandTOPIC_NAME topic. */
    xSubscribeParams.pucTopic = Topic;
    xSubscribeParams.pvPublishCallbackContext = NULL;
    xSubscribeParams.pxPublishCallback = prvMQTTCallback;
   xSubscribeParams.usTopicLength = ( uint16_t ) strlen( ( const char * ) Topic);
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

/*
 * Used for incoming AWS messages
 */
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


void vStartMQTTEchoDemo()
{
    configPRINTF( ( "Starting NMC...\r\n" ) );

    LED_Initialize();



    xTaskCreate(prvNMCmain,
    	             "AWS",
    	             democonfigMQTT_ECHO_TASK_STACK_SIZE, /* Size of the stack to allocate for the task, in words not bytes! */
    	             NULL,                                /* The task parameter is not used. */
    	             tskIDLE_PRIORITY+1,                    /* Runs at the lowest priority. */
    	             NULL);                 /* The handle is stored so the created task can be deleted again at the end of the demo. */



}
/*-----------------------------------------------------------*/
/*
 * Function that is called in every iteration of the main loop to testing connection
 */
void CheckOrChangeConnection(int count)
{
	/*
	 * LED 1 = TCP_IP
	 * LED 0 = AWS
	 */

	if(NMC_mode == AWS){
		LED_On(0);
		LED_Off(1);

		if(count%InterNetPing == 0){

			if(WIFI_Ping(UmassIP,1,200 /*wait .5 sec for response*/ )==eWiFiSuccess){
				NMC_mode = AWS;

			}else{
				/*Calling connect2AWS will delete the task for old aws connection*/
				configPRINTF((" AWS: Disconnected \r\n"));
				LED_Off(0);
				connect2AWS(1);
				NMC_mode = DISCONNECTED;
			}
		}
	}

	else if(NMC_mode == DISCONNECTED){
		LED_Off(1);
		LED_On(0);

		if(socketStatus == DISABLED)
			{
				setUpSocket();
				LED_On(1);
			}
		else if(socketStatus == ENABLED_NOTCONN)
			{
				if(WIFI_Ping(UmassIP,1,500)==eWiFiSuccess){
					if(closeSocket()==0){
						connect2AWS(0);
						}
				}
				else{
					LED_Off(0);/*AWS still trying to connect after failure*/
					configPRINTF(("ERROR: Couldn't Reach AWS \r\n"));
					/*tries to connect again*/
					setUpSocket();
					count = 0;
					LED_On(1);// will blink until gets a connection
					}
			}
	}
	else if(NMC_mode == TCP_IP){

		LED_On(1); /*TCP_IP LED stays on*/
		LED_Off(0);

		if(count%TCP_HeartBeat == 0){
			char * temp = "$\r\n";
			sendSocket(temp,sizeof(temp));

		}
		/* Checks AWS connection every other multiple of 5 */
		else if(count%InterNetPing == 0){
			if(WIFI_Ping(UmassIP,2,200)==eWiFiSuccess){
				configPRINTF(("Connecting to AWS\r\n"));
				LED_Off(1);//LED for TCP_IP mode

				char * temp = "#\r\n";//TODO change msg?
				sendSocket(temp,sizeof(temp));

				vTaskDelay(1000);
				if(closeSocket() == 0){
					connect2AWS(1);
					NMC_mode = DISCONNECTED;
				}
			}
		}
	}
	return;
}

/*-----------------------------------------------------------*/
/*
 * Function used to either establish initial AWS connection or delete old connection
 */
void connect2AWS( int delOldHand )
{
	BaseType_t xReturned1;
	MQTTAgentReturnCode_t xReturned;

	prvMacForHumans(thing_mac_address);// used to identify the identity of thing connecting to AWS
	//( void ) MQTT_AGENT_Delete( xMQTTHandle );
	//xMQTTHandle = NULL;

	xReturned1 = prvCreateClientAndConnectToBroker(delOldHand);


	if(delOldHand){return;}

	if(xCommandMsgBuffer == NULL){
		xCommandMsgBuffer = xMessageBufferCreate( ( size_t ) commandMAX_DATA_LENGTH + sizeof( size_t ) );
		configASSERT( xCommandMsgBuffer );
//
//		xConnectionBuffer = xMessageBufferCreate( ( size_t ) 3 + sizeof( size_t ) );
//		configASSERT( xConnectionBuffer );
	}


	xConnectionBuffer = xMessageBufferCreate( ( size_t ) 3 + sizeof( size_t ) );
	configASSERT( xConnectionBuffer );

	/* Check this task has not already been created. */
	//configASSERT( xMQTTHandle != NULL );
	//configASSERT( xCommandMsgBuffer != NULL );

	if( xReturned1 == pdPASS )
		{

		xReturned = prvSubscribe(command_TOPIC);

		if( xReturned == pdPASS )
			{
			NMC_mode = AWS;
			//configPRINTF(("NMCmode = AWS %d \r\n",NMC_mode));
			prvSend2AWS("NMC Connected to AWS",status_TOPIC);
			return;
			}
		}
	else{
		//vTaskDelay(2*1000);
		NMC_mode = DISCONNECTED;
		return;
	}
}
/*-----------------------------------------------------------*/
/*
 * Function that sends messages to AWS
 */
MQTTAgentReturnCode_t prvSend2AWS(char * cDataBuffer, const uint8_t *Topic){

	MQTTAgentPublishParams_t xPublishParameters;
	MQTTAgentReturnCode_t xReturned;

	xPublishParameters.pucTopic = Topic;
	xPublishParameters.pvData = cDataBuffer;
	xPublishParameters.usTopicLength = ( uint16_t ) strlen( ( const char * ) Topic );
	//xPublishParameters.ulDataLength = ( uint32_t ) strlen( &CANmsg );
	xPublishParameters.ulDataLength = ( uint32_t ) strlen( cDataBuffer );
	xPublishParameters.xQoS = eMQTTQoS1;

	/* Publish the message. */
	xReturned = MQTT_AGENT_Publish( xMQTTHandle, &( xPublishParameters ), democonfigMQTT_TIMEOUT );

	if( xReturned == eMQTTAgentSuccess )
		{

		configPRINTF( ( "Successfully published to '%s': '%s'\r\n", Topic,cDataBuffer ) );

		return xReturned;
		}
	else
		{

			configPRINTF( ( "ERROR:  Failed to publish to '%s': '%s'\r\n", Topic, cDataBuffer  ) );
			return xReturned;
		}
}
/*-----------------------------------------------------------*/
/*
 * Converts Command string into a single integer value for CAN
 */
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
/*-----------------------------------------------------------*/
/*
 * Function handling transmission of CAN message to Motor Module
 */
int prvSendCAN(char * command)
{

	/* Chars from Command */
	char * id = strtok( command, ":" ) ;
	char * function = strtok( NULL,":");
	char * temp1= strtok( NULL,":");
	strncpy(temp1,temp1,(strlen(temp1)-1));
	//parameter = temp1;
	/*--------------------*/

	/* Convert for CAN */
	int ID = char2Int( id );
	int Function = char2Int( function );
	int Parameter = atoi(temp1);
	/*-----------------*/
//

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
