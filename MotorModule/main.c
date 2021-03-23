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

// Globals
uint32_t top_button_state = 1;
uint32_t bottom_button_state = 1;
int MOTOR_MODULE_ID = 0x01;	//ID of this motor module device
int isRegistered = 0;		//Initially 0, will be 1 once registered with NMC
int closedToOpenTime = 0;	//Initially 0, will be set by homing routine

//Time Variable
int time = 0;

//State Variables
int WindowState;			//Current state of window (0 = closed, 11 = fully open), TODO : Set in homing routine
int stateTimingVariables[11];	//Contains variables for time to take to move to certain location

// Function Prototypes
//void initialise_monitor_handles();
void RegisterDevice();
int HomeWindow();
int MoveWindow(int percentage);
int CheckTopLimitSwitch();
int CheckBottomLimitSwitch();

void React_Node_Handler() {
	/* Check for Node error */
	uint32_t status;
	if(CAN_NODE_GetStatus(&React_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
	{}
	else if(CAN_NODE_MO_GetStatus(&React_Node_LMO_01_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
	{
		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
		CAN_NODE_MO_Receive(&React_Node_LMO_01_Config);
		int requestedID = React_Node_LMO_01_Config.mo_ptr->can_data_byte[0];	//grab 0th byte

		if(requestedID == MOTOR_MODULE_ID)		//Check if ID requested is equal to this motor module
		{
			int requestedCommand = React_Node_LMO_01_Config.mo_ptr->can_data_byte[1];	//grab 1st byte

			if(requestedCommand == 0x01)		//operate window
			{
				int requestedPercentage = React_Node_LMO_01_Config.mo_ptr->can_data_byte[2];	//grab 2nd byte
				requestedPercentage = (requestedPercentage / 10);
				int windowMovementStatus = MoveWindow(requestedPercentage);
				if(windowMovementStatus == 0)
				{
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = MOTOR_MODULE_ID;
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[1] = 0x02;	//Successful Command
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[3] = 1;	//
					status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
					status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
				}
				else if(windowMovementStatus == 1)
				{
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = MOTOR_MODULE_ID;
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[1] = 0x02;	//Successful Command
					React_Node_LMO_02_Config.mo_ptr->can_data_byte[3] = 0;	//
					status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
					status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
				}
			}
			else if(requestedCommand == 0x02)	//Acknowledgment to our registration
			{
				isRegistered = 1;
			}
			else if(requestedCommand == 0x03)	//state request
			{
				int WindowStatePercentage = (WindowState * 10);

				React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = MOTOR_MODULE_ID;
				React_Node_LMO_02_Config.mo_ptr->can_data_byte[1] = 0x04;	//Status update
				React_Node_LMO_02_Config.mo_ptr->can_data_byte[2] = WindowStatePercentage;
				status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
				status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
			}
		}
	}
}

void TimerHandler()
{
	time++;
}

int main(void)
{

  DAVE_Init();		/* Initialization of DAVE APPs  */

  // Startup functions
  //RegisterDevice();
  HomeWindow();	// TODO : Uncomment when done

  // Function to initialize debug printing
  // initialise_monitor_handles();

  MoveWindow(5);	// 50%
  MoveWindow(0);	//0%

  while(1U)
  {

  }
  return 0;
}

// Sends message to NMC on startup to register device
void RegisterDevice()
{
	React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = MOTOR_MODULE_ID;
	React_Node_LMO_02_Config.mo_ptr->can_data_byte[1] = 0x05;	//device registration
	uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
	status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);

	//TODO : Add delay until registered, retransmit after delay if not

}

//Homes the motor to acquire constants needed for operation. Also for tuning
int HomeWindow()
{
	// TODO : Home window to find constants needed for proper window operation
	int BottomLimitSwitchState = CheckBottomLimitSwitch();

	if (BottomLimitSwitchState == 1)		// TODO : Change to 0 bc 0 means button pressed
	{
		// TODO : verify that starting PWM before direction is ok
		TIMER_Start(&TIMER_Motor);
		while(CheckTopLimitSwitch() == 1)	//drive motor until top limit switch is reached
		{
			PWM_Start(&PWM_Motor);
			DIGITAL_IO_SetOutputLow(&Motor_Direction); //Move up
		}
		PWM_Stop(&PWM_Motor);
		TIMER_Stop(&TIMER_Motor);
		closedToOpenTime = time;
		while(CheckBottomLimitSwitch() == 1)
		{
			PWM_Start(&PWM_Motor);
			DIGITAL_IO_SetOutputHigh(&Motor_Direction);
		}
		PWM_Stop(&PWM_Motor);	//Turn off PWM once bottom reached
	}
	WindowState = 0;	// Set initial state to zero
	time = 0;			// reset global timer variable
	// Create State table
	stateTimingVariables[0]  = 0;								//0%
	stateTimingVariables[1]  = (closedToOpenTime * (0.1));		//10%
	stateTimingVariables[2]  = (closedToOpenTime * (0.2));		//20%
	stateTimingVariables[3]  = (closedToOpenTime * (0.3));		//30%
	stateTimingVariables[4]  = (closedToOpenTime * (0.4));		//40%
	stateTimingVariables[5]  = (closedToOpenTime * (0.5));		//50%
	stateTimingVariables[6]  = (closedToOpenTime * (0.6));		//60%
	stateTimingVariables[7]  = (closedToOpenTime * (0.7));		//70%
	stateTimingVariables[8]  = (closedToOpenTime * (0.8));		//80%
	stateTimingVariables[9]  = (closedToOpenTime * (0.9));		//90%
	stateTimingVariables[10] = (closedToOpenTime);		        //100%
	return 1;
}

//Function to move window to desired percentage
// NOTE: HIGH = DOWM; LOW = UP !!!
int MoveWindow(int percentage)
{
	// TODO : Move motor to requested percentage. return 1 for success, 0 for failure
	/*
	 -----Methodology-----
	 - Key into global time array using requested percentage
	 - Take current state, subtract requested state from it
	 - use + or - value of the number to know which direction to move, and magnitude for how much time to drive motor
	 - drive motor and update timer variable with timer interrupt until motor has been driven appropriately
	 - stop timer and update current state variable
	 - return with success and smile :)
	*/
	int requestedPercentage = percentage;
	int timeToMove = (stateTimingVariables[requestedPercentage] - stateTimingVariables[WindowState]);		// Subtract current state (time) to requested state.
																										// Difference will be time needed to drive motor,
																										// sign will be direction (+ = up, - = down)

	if (timeToMove > 0)			// Positive, so need to drive up
	{
		TIMER_Start(&TIMER_Motor);
		while(time < timeToMove)	//drive motor until time is equal to state
		{
			PWM_Start(&PWM_Motor);
			DIGITAL_IO_SetOutputLow(&Motor_Direction); //Move up
		}
		PWM_Stop(&PWM_Motor);
		time = 0;
		WindowState = requestedPercentage;
	}
	else if (timeToMove < 0)	// Negative, need to drive down
	{
		timeToMove = abs(timeToMove);
		TIMER_Start(&TIMER_Motor);
		while(time < timeToMove)	//drive motor until time is equal to state
		{
			PWM_Start(&PWM_Motor);
			DIGITAL_IO_SetOutputHigh(&Motor_Direction); //Move down
		}
		PWM_Stop(&PWM_Motor);
		time = 0;
		WindowState = requestedPercentage;
	}
	else if (timeToMove == 0)
	{
		// Requesting same state, so do nothing
	}
	else
	{
		return 0;
	}
	return 1;
}

int CheckTopLimitSwitch()
{
	int TopSwitchState = DIGITAL_IO_GetInput(&LIMIT_SWITCH_TOP);
	if (TopSwitchState != top_button_state)
	{
		DIGITAL_IO_SetOutputHigh(&LED_Indicator);
		return 0;	//Top switch pressed
	}

	return 1; 	//Base case that neither is pressed
	// TODO : Make this meaningful
}
int CheckBottomLimitSwitch()
{
	int BottomSwitchState = DIGITAL_IO_GetInput(&LIMIT_SWITCH_BOTTOM);
	if (BottomSwitchState != bottom_button_state)
	{
		DIGITAL_IO_SetOutputLow(&LED_Indicator);
		return 0; 	//Bottom switch pressed
	}

	return 1; 	//Base case that neither is pressed
	// TODO : Make this meaningful
}

void ManualOverrideSwitches()
{

}

//*****************************************************************//
//*****************************************************************//
// Code that may be useful for reuse later

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

//int React_Node_Handler() {
//	/* Check for Node error */
//	if(CAN_NODE_GetStatus(&React_Node) & XMC_CAN_NODE_STATUS_LAST_ERROR_CODE)
//	{}
//	else if(CAN_NODE_MO_GetStatus(&React_Node_LMO_01_Config) & XMC_CAN_MO_STATUS_RX_PENDING)
//	{
//		/* Read the received Message object and stores in Request_Node_LMO_02_Config*/
//		CAN_NODE_MO_Receive(&React_Node_LMO_01_Config);
//		int receivedBytes = React_Node_LMO_01_Config.mo_ptr->can_data_byte[0];
//		if (receivedBytes == 0x01) { // Turn on LED
//			DIGITAL_IO_ToggleOutput(&LED_Indicator);
//			XMC_DEBUG("4200: Message Received from 4200\n");
//			XMC_DEBUG("Received with value: %d \n", receivedBytes);
//			XMC_DEBUG("Toggle LED\n\n");
//
//			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x01;
//			React_Node_LMO_02_Config.mo_ptr->can_data_byte[0] = 0x03;
//			uint32_t status = (CAN_NODE_STATUS_t)XMC_CAN_MO_UpdateData(React_Node_LMO_02_Config.mo_ptr);
//			status = CAN_NODE_MO_Transmit(&React_Node_LMO_02_Config);
//		}
//		else if (receivedBytes == 0x03)
//		{
//			XMC_DEBUG("4200: Message Received from 4200\n");
//			XMC_DEBUG("Received with value: %d \n", receivedBytes);
//			XMC_DEBUG("ACK\n\n");
//		}
//	}
//
//}
