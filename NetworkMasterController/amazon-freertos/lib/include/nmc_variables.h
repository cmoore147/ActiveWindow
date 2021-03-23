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

typedef enum CAN_MESSAGE_TYPE
{
  STATUS_UPDATE = 0x04,
  ADD_DEVICE = 0x05,
  AWS_ACK = 0x02
} CAN_MESSAGE_TYPE;

#endif /* LIB_AWS_INCLUDE_NMC_VARIABLES_H_ */
