/**
 * @file State_Machine.c
 * @author Leo Liu
 * @brief control the state of the robot
 * @version 1.0
 * @date 2022-07-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "State_Machine.h"

State_Machine_t State_Machine;

void Remote_Control_Update(void);
void Computer_Update(void);

State_Machine_Func_t State_Machine_Func = State_Machine_Func_GroundInit;
#undef State_Machine_Func_GroundInit

void Remote_Control_Update(void)
{
	/*
	Both switches down for disabling the robot
	Also disable the robot if remote control receives no data
	*/
	if ((DR16_Export_Data.Remote_Control.Left_Switch == SWITCH_DOWN && DR16_Export_Data.Remote_Control.Right_Switch == SWITCH_DOWN) || (DR16_Export_Data.Info_Update_Frame < 1))
	{
		Robot_Control_Func.Robot_Control_Disabled();
		State_Machine.Robot_Ready_Flag = false;
	}

	/*
	Right switch mid, left switch down: follow gimbal mode
	Right switch mid, left switch mid: not follow gimbal mode
	Right switch mid, left switch up: spintop mode
	*/
	else if (DR16_Export_Data.Remote_Control.Right_Switch == SWITCH_MID)
	{
		State_Machine.Control_Source = Remote_Control;

		switch (DR16_Export_Data.Remote_Control.Left_Switch)
		{
		case (SWITCH_DOWN):
		{
			Chassis.Current_Mode = Follow_Gimbal;
			Gimbal.Current_Mode = Follow_Gimbal;

			break;
		}
		case (SWITCH_MID):
		{
			Chassis.Current_Mode = Not_Follow_Gimbal;
			Gimbal.Current_Mode = Not_Follow_Gimbal;

			break;
		}
		case (SWITCH_UP):
		{
			Chassis.Current_Mode = Spin_Top;
			Gimbal.Current_Mode = Spin_Top;

			break;
		}
		}

		if (!Shooting.Fric_Wheel.Turned_On)
		{
			if (DR16_Export_Data.Remote_Control.Dial_Wheel < -50)
			{
				Super_Capacitor.Super_Cap_On = 1;
				Super_Capacitor.Super_Cap_Accel_Rate = fabs((float)DR16_Export_Data.Remote_Control.Dial_Wheel / 1000);
			}
			else
				Super_Capacitor.Super_Cap_On = 0;
		}
	}

	/*
	Right switch up, left switch down: turn on friction wheel
	Right switch up, left switch mid: turn off friction wheel
	Right switch up, left switch up: enter computer control mode
	*/
	else if (DR16_Export_Data.Remote_Control.Right_Switch == SWITCH_UP)
	{
		switch (DR16_Export_Data.Remote_Control.Left_Switch)
		{
		case (SWITCH_DOWN):
		{
			State_Machine.Control_Source = Remote_Control;

			Shooting.Fric_Wheel.Turned_On = 0;
			Shooting.Type.Auto_Aiming = 0;

			break;
		}
		case (SWITCH_MID):
		{
			State_Machine.Control_Source = Remote_Control;
			Shooting.Type.Auto_Aiming = 0;
			Shooting.Fric_Wheel.Turned_On = 1;

			break;
		}
		case (SWITCH_UP):
		{
			// State_Machine.Control_Source = Computer;
			Shooting.Type.Auto_Aiming = 1;

			break;
		}
		}
	}
	Computer_Update();
}

void Computer_Update(void)
{
	/*
	Press R for follow gimbal mode, default mode is follow gimbal mode
	Press F for turn on/off not follow gimbal mode
	Press G for turn on/off spintop mode
	Press B for turn on/off friction wheel
	*/
	if (State_Machine.Control_Source == Computer)
	{
		if (DR16_Export_Data.Keyboard.Press_R.Switch_Flag)
		{
			Chassis.Current_Mode = Follow_Gimbal;
			Gimbal.Current_Mode = Follow_Gimbal;
		}

		else if (DR16_Export_Data.Keyboard.Press_F.Switch_Flag)
		{
			if (Chassis.Current_Mode == Not_Follow_Gimbal && Gimbal.Current_Mode == Not_Follow_Gimbal)
			{
				Chassis.Current_Mode = Follow_Gimbal;
				Gimbal.Current_Mode = Follow_Gimbal;
			}
			else
			{
				Chassis.Current_Mode = Not_Follow_Gimbal;
				Gimbal.Current_Mode = Not_Follow_Gimbal;
			}
		}

		else if (DR16_Export_Data.Keyboard.Press_G.Switch_Flag)
		{
			if (Chassis.Current_Mode == Spin_Top && Gimbal.Current_Mode == Spin_Top)
			{
				Chassis.Current_Mode = Follow_Gimbal;
				Gimbal.Current_Mode = Follow_Gimbal;
			}
			else
			{
				Chassis.Current_Mode = Spin_Top;
				Gimbal.Current_Mode = Spin_Top;
			}
		}

		else if (DR16_Export_Data.Keyboard.Press_B.Switch_Flag)
		{
			if (Shooting.Fric_Wheel.Turned_On)
				Shooting.Fric_Wheel.Turned_On = 0;
			else
				Shooting.Fric_Wheel.Turned_On = 1;
		}

		if (DR16_Export_Data.Keyboard.Press_Shift.Hold_Flag)
		{
			Super_Capacitor.Super_Cap_On = 1;
			Super_Capacitor.Super_Cap_Accel_Rate = 2.0f;
		}
		else if (!DR16_Export_Data.Keyboard.Press_Shift.Hold_Flag)
			Super_Capacitor.Super_Cap_On = 0;
	}
}
