/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file			: pwm.c
  *
  * @created		: 18.07.2022 11:00
  *
  * @Author			: Ilmotronics
  ******************************************************************************
  */
/* CODE END Header */

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Inc
#include <_pwm_servo.h>

extern TIM_HandleTypeDef htim3;					// external for Timer PWM
extern ubyte g_ubytDrvInitState;

/* ******************************************************************************
 *  FUNCTION: PWM_init
 *  PARAMETERS:	none
 *  DESCRIPTION: Initialisierung für PWM (servos)
 *  RETURNS: none
 ******************************************************************************** */
void PWM_init(void){
	PWM_Stop_all();
	PWM1_Angle(0);
	PWM2_Angle(0);
	PWM3_Angle(0);
}

/* ******************************************************************************
 *  FUNCTION: PWM1_Stop
 *  PARAMETERS:	none
 *  DESCRIPTION: Timer für PWM1 (Servo1) stoppen
 *  RETURNS: none
 ******************************************************************************** */
void PWM1_Stop(void){
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
}
/* ******************************************************************************
 *  FUNCTION: PWM2_Stop
 *  PARAMETERS:	none
 *  DESCRIPTION: Timer für PWM2 (Servo2) stoppen
 *  RETURNS: none
 ******************************************************************************** */
void PWM2_Stop(void){
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
}

/* ******************************************************************************
 *  FUNCTION: PWM3_Stop
 *  PARAMETERS:	none
 *  DESCRIPTION: Timer für PWM3 (Servo1) stoppen
 *  RETURNS: none
 ******************************************************************************** */
void PWM3_Stop(void){
  HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_3);
}

/* ******************************************************************************
 *  FUNCTION: PWM1_Stop
 *  PARAMETERS:	none
 *  DESCRIPTION: Timer für PWM's (Servos) stoppen
 *  RETURNS: none
 ******************************************************************************** */
void PWM_Stop_all(void){
	PWM1_Stop();
	PWM2_Stop();
	PWM3_Stop();
}
/* ******************************************************************************
 *  FUNCTION: PWM1_Stop
 *  PARAMETERS:	time: Impulsdauer
 *  DESCRIPTION: Überprüfen der Impulsdauer
 *  RETURNS: uint16_t
 ******************************************************************************** */
uint16_t PWM_Time_Lim(uint16_t time){
	if (time<920) time = 920;
	if (time>2120) time = 2120;
	return time;
}

/* ******************************************************************************
 *  FUNCTION: PWM1_Time
 *  PARAMETERS:	time: Impulsdauer
 *  DESCRIPTION: Einstellen der Impulsdauer für PWM (Servo) und damit des Winkels
 *  RETURNS: none
 ******************************************************************************** */
void PWM1_Time(uint16_t time){
	TIM3->CCR1 = PWM_Time_Lim(time);
}

/* ******************************************************************************
 *  FUNCTION: PWM2_Time
 *  PARAMETERS:	time: Impulsdauer
 *  DESCRIPTION: Einstellen der Impulsdauer für PWM (Servo) und damit des Winkels
 *  RETURNS: none
 ******************************************************************************** */
void PWM2_Time(uint16_t time){
	TIM3->CCR2 = PWM_Time_Lim(time);
}

/* ******************************************************************************
 *  FUNCTION: PWM3_Time
 *  PARAMETERS:	time: Impulsdauer
 *  DESCRIPTION: Einstellen der Impulsdauer für PWM (Servo) und damit des Winkels
 *  RETURNS: none
 ******************************************************************************** */
void PWM3_Time(uint16_t time){
	TIM3->CCR3 = PWM_Time_Lim(time);
}

/* ******************************************************************************
 *  FUNCTION: PWM_Angle_to_Time
 *  PARAMETERS:	angle: Winkel des Servos
 *  DESCRIPTION: Umrechung von Winkel zu Impulsdauer
 *  RETURNS: int16_t: Impulsdauer
 ******************************************************************************** */
int16_t PWM_Angle_to_Time(int8_t angle){
	uint16_t time = 1520 + (angle * TIME_PER_DEGREE);
	return time;
}

/* ******************************************************************************
 *  FUNCTION: PWM1_Angle
 *  PARAMETERS:	angle: Winkel des Servos
 *  DESCRIPTION: Einstellen des Winkels für Servo
 *  RETURNS: none
 ******************************************************************************** */
void PWM1_Angle(int8_t angle){
	PWM1_Time(PWM_Angle_to_Time(angle));
}

/* ******************************************************************************
 *  FUNCTION: PWM2_Angle
 *  PARAMETERS:	angle: Winkel des Servos
 *  DESCRIPTION: Einstellen des Winkels für Servo
 *  RETURNS:  none
 ******************************************************************************** */
void PWM2_Angle(int8_t angle){
	PWM2_Time(PWM_Angle_to_Time(angle));
}

/* ******************************************************************************
 *  FUNCTION: PWM3_Angle
 *  PARAMETERS:	angle: Winkel des Servos
 *  DESCRIPTION: Einstellen des Winkels für Servo
 *  RETURNS: none
 ******************************************************************************** */
void PWM3_Angle(int8_t angle){
	PWM3_Time(PWM_Angle_to_Time(angle));
}

/* ******************************************************************************
 *  FUNCTION: PWM1_Start
 *  PARAMETERS: none
 *  DESCRIPTION: Starten des Timers für Servo1
 *  RETURNS: none
 ******************************************************************************** */
void PWM1_Start(void){
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
}

/* ******************************************************************************
 *  FUNCTION: PWM2_Start
 *  PARAMETERS: none
 *  DESCRIPTION: Starten des Timers für Servo2
 *  RETURNS: none
 ******************************************************************************** */
void PWM2_Start(void){
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
}

/* ******************************************************************************
 *  FUNCTION: PWM3_Start
 *  PARAMETERS: none
 *  DESCRIPTION: Starten des Timers für Servo3
 *  RETURNS: none
 ******************************************************************************** */
void PWM3_Start(void){
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
}

/* ******************************************************************************
 *  FUNCTION: PWM_Start_all
 *  PARAMETERS: none
 *  DESCRIPTION: Starten der Timers für alle Servos
 *  RETURNS: none
 ******************************************************************************** */
void PWM_Start_all(void){
	PWM1_Start();
	PWM2_Start();
	PWM3_Start();
}

/* ******************************************************************************
 *  FUNCTION: XYServoSetPWM
 *  PARAMETERS: act_time_x: Impulsdauer für X-Richtung , act_time_y: Impulsdauer für Y-Richtung
 *  DESCRIPTION: XY-Position des Tisches festlegen
 *  RETURNS: none
 ******************************************************************************** */
void XYServoSetPWM(uword act_time_x,uword act_time_y){
	PWM1_Time(act_time_x);
	PWM2_Time(act_time_y);
	PWM1_Start();
	PWM2_Start();
	g_ubytDrvInitState &= 0xfb;
}

/* ******************************************************************************
 *  FUNCTION: XYServoStopp
 *  PARAMETERS: none
 *  DESCRIPTION: Stoppen der Timers für Servos des XY-Tisches
 *  RETURNS: none
 ******************************************************************************** */
void XYServoStopp(void){
	PWM1_Stop();
	PWM2_Stop();
	g_ubytDrvInitState |= 0x04;
}

/* ******************************************************************************
 *  FUNCTION: UNIServoSetPWM
 *  PARAMETERS: act_time: Impulsdauer Uni-Servo
 *  DESCRIPTION: Starten der Timers für UNI-Servos
 *  RETURNS: none
 ******************************************************************************** */
void UNIServoSetPWM(uword act_time){
	PWM3_Time(act_time);
	PWM3_Start();
}

/* ******************************************************************************
 *  FUNCTION: UNIServoStopp
 *  PARAMETERS: none
 *  DESCRIPTION: Stoppen der Timers für UNI-Servos
 *  RETURNS: none
 ******************************************************************************** */
void UNIServoStopp(void){
	PWM3_Stop();
}
/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
