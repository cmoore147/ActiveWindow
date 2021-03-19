///*******************************************************************************
// Copyright (c) 2015, Infineon Technologies AG                                 **
// All rights reserved.                                                         **
//                                                                              **
// Redistribution and use in source and binary forms, with or without           **
// modification,are permitted provided that the following conditions are met:   **
//                                                                              **
// *Redistributions of source code must retain the above copyright notice,      **
// this list of conditions and the following disclaimer.                        **
// *Redistributions in binary form must reproduce the above copyright notice,   **
// this list of conditions and the following disclaimer in the documentation    **
// and/or other materials provided with the distribution.                       **
// *Neither the name of the copyright holders nor the names of its contributors **
// may be used to endorse or promote products derived from this software without**
// specific prior written permission.                                           **
//                                                                              **
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  **
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    **
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   **
// ARE  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   **
// LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         **
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         **
// SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    **
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      **
// CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       **
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   **
// POSSIBILITY OF SUCH DAMAGE.                                                  **
//                                                                              **
// To improve the quality of the software, users are encouraged to share        **
// modifications, enhancements or bug fixes with Infineon Technologies AG       **
// dave@infineon.com).                                                          **
//                                                                              **
//********************************************************************************
//**                                                                            **
//**                                                                            **
//** PLATFORM : Infineon XMC4500 Series                                         **
//**                                                                            **
//** AUTHOR : Application Engineering Team                                      **
//**                                                                            **
//** version 4.0.0  Initial version                                             **
//**         4.1.2  Modified for APP updates                                    **
//**         4.1.4  Modified for APP updates									  **
//**                                                                            **
//** MODIFICATION DATE : OCT, 30, 2015                                          **
//**                                                                            **
//*******************************************************************************/
//
//#include <DAVE.h>			//Declarations from DAVE Code Generation (includes SFR declaration)
//
///*
// * This Example shows transmission and reception of data frames
// * between two CAN nodes of the MultiCAN peripheral in Loop-Back Mode.
// */
//
//
//uint32_t led_state = 0;
//uint32_t button_state = 1;
//
//void React_Node_Handler() {
//	/* Check for Node error */
//	if(CAN_NODE_GetStatus(&React_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
//	{}
//	else if(CAN_NODE_MO_GetStatus(&React_Node_LMO_01_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
//	{
//		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
//
//		CAN_NODE_MO_Receive(&React_Node_LMO_01_Config);
//
//		if (React_Node_LMO_01_Config.mo_ptr->can_data_byte[0] == 0x01) { // Turn on LED
//			DIGITAL_IO_SetOutputHigh(&LED_Indicator);
//		}
//		else { // Turn off LED
//			DIGITAL_IO_SetOutputLow(&LED_Indicator);
//		}
//
//	 /* Switch on LED Pin 3.9  to indicate that the requested message is received*/
//	 //DIGITAL_IO_ToggleOutput(&LED_Indicator);
//	}
//
//}
//
////void Send_CAN_React() {
////
////}
//
//void Read_Button() {
//	// check if state has changed
//	if (button_state != DIGITAL_IO_GetInput(&BoardButton)) {
//		button_state = DIGITAL_IO_GetInput(&BoardButton);
//
//		if (button_state == 0) { // send message to turn led on
//			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x01;
//		} else { // send message to turn led off
//			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x00;
//		}
//
//		uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
//		status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
//	}
//}
//
//int main(void)
//{
//
//  DAVE_Init();		/* Initialization of DAVE APPs  */
//
//  uint32_t TimerId = (uint32_t)SYSTIMER_CreateTimer(100000U,SYSTIMER_MODE_PERIODIC,(void*)Read_Button,NULL);
//  SYSTIMER_StartTimer(TimerId);
//
//  //CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
//
//  while(1U);
//  return 0;
//}
//
/////*
//// * This function is the Interrupt Event Handler for the CAN Node Request
//// *
//// */
////void Request_Node_Interrupt()
////{
////  /* Check for Node error */
////  if(CAN_NODE_GetStatus(&Request_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
////  {}
////  else if(CAN_NODE_MO_GetStatus(&Request_Node_LMO_02_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
////  {
////     /* Read the received Message object and stores in Request_Node_LMO_02_Config*/
////
////	  CAN_NODE_MO_Receive(&Request_Node_LMO_02_Config);
////
////	  if (Request_Node_LMO_02_Config.mo_ptr->can_data_byte[0] == 0x01) { // Turn on LED
////		  DIGITAL_IO_SetOutputHigh(&LED_Indicator);
////	  }
////	  else { // Turn off LED
////		  DIGITAL_IO_SetOutputLow(&LED_Indicator);
////	  }
////
////     /* Switch on LED Pin 3.9  to indicate that the requested message is received*/
////     //DIGITAL_IO_ToggleOutput(&LED_Indicator);
////  }
////
////}
////
///////*
//// * This function is the Interrupt Event Handler for the CAN Node React
//// */

/*******************************************************************************
 Copyright (c) 2015, Infineon Technologies AG                                 **
 All rights reserved.                                                         **
                                                                              **
 Redistribution and use in source and binary forms, with or without           **
 modification,are permitted provided that the following conditions are met:   **
                                                                              **
 *Redistributions of source code must retain the above copyright notice,      **
 this list of conditions and the following disclaimer.                        **
 *Redistributions in binary form must reproduce the above copyright notice,   **
 this list of conditions and the following disclaimer in the documentation    **
 and/or other materials provided with the distribution.                       **
 *Neither the name of the copyright holders nor the names of its contributors **
 may be used to endorse or promote products derived from this software without**
 specific prior written permission.                                           **
                                                                              **
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"  **
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE    **
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   **
 ARE  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   **
 LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         **
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF         **
 SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    **
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      **
 CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       **
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE   **
 POSSIBILITY OF SUCH DAMAGE.                                                  **
                                                                              **
 To improve the quality of the software, users are encouraged to share        **
 modifications, enhancements or bug fixes with Infineon Technologies AG       **
 dave@infineon.com).                                                          **
                                                                              **
********************************************************************************
**                                                                            **
**                                                                            **
** PLATFORM : Infineon XMC4500 Series                                         **
**                                                                            **
** AUTHOR : Application Engineering Team                                      **
**                                                                            **
** version 4.0.0  Initial version                                             **
**         4.1.2  Modified for APP updates                                    **
**         4.1.4  Modified for APP updates									  **
**                                                                            **
** MODIFICATION DATE : OCT, 30, 2015                                          **
**                                                                            **
*******************************************************************************/

#include <DAVE.h>			//Declarations from DAVE Code Generation (includes SFR declaration)
#include <stdio.h>
#include "string.h"
/*
 * This Example shows transmission and reception of data frames
 * between two CAN nodes of the MultiCAN peripheral in Loop-Back Mode.
 */


// uint32_t led_state = 0;


// Globals
uint32_t button_state = 1;

// Function Prototypes
//void initialise_monitor_handles();

void React_Node_Handler() {
	/* Check for Node error */
	if(CAN_NODE_GetStatus(&React_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
	{}
	else if(CAN_NODE_MO_GetStatus(&React_Node_LMO_01_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
	{
		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
		CAN_NODE_MO_Receive(&React_Node_LMO_01_Config);
		char * ID = React_Node_LMO_01_Config.mo_ptr->can_data_byte[0];
		char * function = React_Node_LMO_01_Config.mo_ptr->can_data_byte[1];
		int parameter = React_Node_LMO_01_Config.mo_ptr->can_data_byte[2];

		if (strcmp("window1",ID)==0) { // Turn on LED
			if(strcmp("on",function) == 0){
			DIGITAL_IO_ToggleOutput(&LED_Indicator);
			XMC_DEBUG("4200: Message Received from 4200\n");
			XMC_DEBUG("Received with value: %d \n", receivedBytes);
			XMC_DEBUG("Toggle LED\n\n");

			//React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x01;
			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x03;
			uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
			status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
			}
		}

	}

}

//void Read_Button() {
//	// check if state has changed
//	if (button_state != DIGITAL_IO_GetInput(&BoardButton)) {
//		button_state = DIGITAL_IO_GetInput(&BoardButton);
//
//		if (button_state == 0) { // send message to toggle LED
//			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x01;
//			XMC_DEBUG("Message Sent!\n");
//		}
////		} else { // send message to turn led off
////			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x00;
////		}
//
//
//
//		uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
//		status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
//	}
//}

int main(void)
{

  DAVE_Init();		/* Initialization of DAVE APPs  */

  //uint32_t TimerId = (uint32_t)SYSTIMER_CreateTimer(100000U,SYSTIMER_MODE_PERIODIC,(void*)Read_Button,NULL);
  //SYSTIMER_StartTimer(TimerId);

  //DIGITAL_IO_SetOutputHigh(&LED_Indicator);
  //CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);

  // Function to initialize debug printing
  // initialise_monitor_handles();

  while(1U)
  {
	  if (button_state != DIGITAL_IO_GetInput(&BoardButton)) {
	  		button_state = DIGITAL_IO_GetInput(&BoardButton);

	  		if (button_state == 0) { // send message to turn toggle
	  			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x03;
	  			XMC_DEBUG("4800: Message Sent to 4200 with value 1 (Toggle LED)\n\n");
		  		uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
		  		status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
	  		}
//	  		} else { // send message to turn led off
//	  			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x00;
//	  		}
	  	}
  }
  return 0;
}

///*
// * This function is the Interrupt Event Handler for the CAN Node Request
// *
// */
//void Request_Node_Interrupt()
//{
//  /* Check for Node error */
//  if(CAN_NODE_GetStatus(&Request_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
//  {}
//  else if(CAN_NODE_MO_GetStatus(&Request_Node_LMO_02_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
//  {
//     /* Read the received Message object and stores in Request_Node_LMO_02_Config*/
//
//	  CAN_NODE_MO_Receive(&Request_Node_LMO_02_Config);
//
//	  if (Request_Node_LMO_02_Config.mo_ptr->can_data_byte[0] == 0x01) { // Turn on LED
//		  DIGITAL_IO_SetOutputHigh(&LED_Indicator);
//	  }
//	  else { // Turn off LED
//		  DIGITAL_IO_SetOutputLow(&LED_Indicator);
//	  }
//
//     /* Switch on LED Pin 3.9  to indicate that the requested message is received*/
//     //DIGITAL_IO_ToggleOutput(&LED_Indicator);
//  }
//
//}


