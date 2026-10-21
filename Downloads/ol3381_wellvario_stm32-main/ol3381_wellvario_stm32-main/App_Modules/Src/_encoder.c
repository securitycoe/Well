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
#include <_encoder.h>

extern TIM_HandleTypeDef htim1;					// external for Timer ENCODER 1
extern TIM_HandleTypeDef htim4;					// external for Timer ENCODER 2
extern TIM_HandleTypeDef htim8;					// external for Timer ENCODER 3

extern volatile int16_t HrzEncoder;				// Enc.1 (was T2) replacements for old (C167CS) hardware registers
extern volatile int16_t VertEncoder;			// Enc.2 (was T3)
extern volatile int16_t	T4Encoder; 				// Enc.3 (Was T4) nur bei Drehtisch (Disk) angeschlossen,initialisiert,aber nicht genutzt

/* ******************************************************************************
 *  FUNCTION: ENCODER_start
 *  PARAMETERS:	none
 *  DESCRIPTION: Starte Timer für Encoder
 *  RETURNS: none
 ******************************************************************************** */
void ENCODER_start(void){
  HAL_TIM_Encoder_Start_IT(&htim1,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start_IT(&htim4,TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start_IT(&htim8,TIM_CHANNEL_ALL);
}

/* ******************************************************************************
 *  FUNCTION: ENCODER_update
 *  PARAMETERS:	none
 *  DESCRIPTION: Encoderwerte manuell aktualisieren
 *  RETURNS: none
 ******************************************************************************** */
void ENCODER_update(void)
{
	volatile uint32_t counter = __HAL_TIM_GET_COUNTER(&htim1);
	__HAL_TIM_SET_COUNTER(&htim1,0);
	volatile int16_t count	 = (int16_t) counter;		// 1600 Imp/U
	HrzEncoder	+= count;
	counter = __HAL_TIM_GET_COUNTER(&htim4);
	__HAL_TIM_SET_COUNTER(&htim4,0);
	count	 = (int16_t) counter;
	VertEncoder	+= count;
	counter = __HAL_TIM_GET_COUNTER(&htim8);
	__HAL_TIM_SET_COUNTER(&htim8,0);
	count	 = (int16_t) counter;
	T4Encoder	+= count;
}

/* ******************************************************************************
 *  FUNCTION: HAL_TIM_IC_CaptureCallback
 *  PARAMETERS:	none
 *  DESCRIPTION: Interrupt durch Encoder
 *  RETURNS: none
 ******************************************************************************** */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
//	int speed		 = 0;
	uint32_t counter = __HAL_TIM_GET_COUNTER(htim);
	__HAL_TIM_SET_COUNTER(htim,0);
	int16_t count	 = (int16_t) counter;		// 1600 Imp/U
//	int16_t position = count/4;					// 400/U ?
	if (htim == &htim1) HrzEncoder	+= count;
	if (htim == &htim4) VertEncoder	+= count;
	if (htim == &htim8) T4Encoder	+= count;

#ifdef test_printf_encoder
	printf("hor:%6d  ver:%6d  en3:%6d\r\n",HrzEncoder,VertEncoder,T4Encoder);
#endif
}
/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
