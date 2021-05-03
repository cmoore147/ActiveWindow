/*
 * nmc_variables.h
 *
 *  Created on: Feb 24, 2021
 *      Author: Frank
 */

#ifndef LIB_AWS_INCLUDE_NMC_VARIABLES_H_
#define LIB_AWS_INCLUDE_NMC_VARIABLES_H_

#include "aws_demo.h"

int MSG_RCVD;

int WINDOW_STATUS;


QueueHandle_t xCANqueue;

//Receiving messages
typedef enum CAN_MESSAGE_TYPE
{
  STATUS_UPDATE = 0x04,
  ADD_DEVICE = 0x05,
  AWS_ACK = 0x02
} CAN_MESSAGE_TYPE;

typedef enum SOCKET_STATUS
{
  DISABLED = 0x01,
  CLOSE_IT = 0x02,
  ENABLED = 0x03,
  ENABLED_NOTCONN = 0x04
} SOCKET_STATUS;

typedef enum NMC_MODE
{
  AWS = 0x01, 				/* set in socketSetUP*/
  DISCONNECTED = 0x02,		/* set is connect2AWS()*/
  TCP_IP = 0x03 			/* Set in SocketSetUp()*/
} NMC_MODE;

int NMC_mode;


#endif /* LIB_AWS_INCLUDE_NMC_VARIABLES_H_ */
