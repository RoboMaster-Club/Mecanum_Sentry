/**
 * @file Gimbal_Control.h
 * @author Leo Liu
 * @brief header file for gimbal control
 * @version 1.0
 * @date 2022-07-11
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __GIMBAL_CONTROL_H
#define __GIMBAL_CONTROL_H

#include "DR16_Remote.h"
#include "User_Defined_Math.h"
#include "PID.h"
#include "Robot_Control.h"
#include "Board_A_IMU.h"
#include "MPU6050_IMU.h"
#include "Jetson_Tx2.h"

#define YAW_DIRECTION 1			  // Yaw motor direction
#define YAW_MID_MECH_ANGLE 2691	  // This is measured for yaw origin
#define PITCH_DIRECTION 1		  // Pitch motor direction
#define PITCH_MID_MECH_ANGLE 5241 // This is measured for pitch origin
#define PITCH_LOWER_LIMIT -23.0f  // Pitch cannot rotate 360 degree, so these are the limits in mechanical angle
#define PITCH_UPPER_LIMIT 30.0f

#define Gimbal_Func_GroundInit        \
	{                                 \
		&Gimbal_Init,                 \
			&Gimbal_Control_Get_Data, \
			&Gimbal_Processing,       \
	}

typedef struct
{
	int Current_Mode;
	int Prev_Mode;

	float Target_Yaw;		 // deg
	float Lag_Target_Yaw;	 // deg
	float Last_Yaw;			 // deg
	float Current_Yaw;		 // deg
	float Target_Yaw_Speed;	 // deg/s
	float Current_Yaw_Speed; // deg/s

	float Target_Pitch;
	float Current_Pitch;
	float Target_Pitch_Speed;  // deg/s
	float Current_Pitch_Speed; // deg/s
	float Angle_Difference;
	float Pitch_Angle_Output;

	uint8_t Gimbal_Back_Mid_Flag;
	uint8_t Gimbal_Offline_Flag;
	uint8_t On_Command;
	uint8_t Last_On_Command;
} Gimbal_t;

typedef struct
{
	void (*Gimbal_Init)(void);
	void (*Gimbal_Control_Get_Data)(Gimbal_t *Gimbal);
	void (*Gimbal_Processing)(Gimbal_t *Gimbal);
} Gimbal_Func_t;

void SpeedCalc(void const *argument);

extern Gimbal_Func_t Gimbal_Func;
extern Gimbal_t Gimbal;
extern float yawBuffer[11];
#endif
