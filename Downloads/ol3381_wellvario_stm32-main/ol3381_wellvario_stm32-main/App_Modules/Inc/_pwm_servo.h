/* CODE BEGIN Header */
/**
 ******************************************************************************
 * @file			: _pwm_servo.h
 * @brief			: Header for pwm.c file.
 *					  This file contains the common defines of the application.
 * @created		: 18.07.2022 11:00
 *
 * @Author			: Ilmotronics
 ******************************************************************************
 */
/* CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC__PWM_SERVO_H_
#define INC__PWM_SERVO_H_

#include "typedefs.h"

/* Exported functions prototypes ---------------------------------------------*/
void PWM_init(void);

void PWM1_Stop(void);
void PWM2_Stop(void);
void PWM3_Stop(void);
void PWM_Stop_all(void);

uint16_t PWM_Time_Lim(uint16_t time);	// 920...1520...2120 [µs]
void PWM1_Time(uint16_t time);
void PWM2_Time(uint16_t time);
void PWM3_Time(uint16_t time);

#define TIME_PER_DEGREE	600/60			// [µs/°] X/Y-Servo S3115: -60...0...60 [°] -> 60°/600µs
int16_t PWM_Angle_to_Time(int8_t angle);
void PWM1_Angle(int8_t angle);
void PWM2_Angle(int8_t angle);
void PWM3_Angle(int8_t angle);

void PWM1_Start(void);
void PWM2_Start(void);
void PWM3_Start(void);
void PWM_Start_all(void);

void XYServoSetPWM(uword act_time_x, uword act_time_y);
void XYServoStopp(void);
void UNIServoSetPWM(uword act_time);
void UNIServoStopp(void);

#endif /* INC__PWM_SERVO_H_ */

/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
