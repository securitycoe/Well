/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software,it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>							// define bool types
#include <stdio.h>								// for types in printf
#include <string.h>								// for types in printf
#include <math.h>								// für float

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions


// ---------- BASE
#include <ig_define.h>
// ---------- Inc
#include <_encoder.h>
#include <_i2c_fram.h>
#include <_pwm_servo.h>
#include <_spi_eeprom.h>
#include <_tmc5130a.h>
#include <_uart_terminal.h>
// ---------- Master
#include <ch_ig_head.h>							// P2.8
#include <ch_ig_config.h>						// EEPROMReadDeviceType
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
const char HW_Revision[] ={"2.0"};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

I2C_HandleTypeDef hi2c1;

IWDG_HandleTypeDef hiwdg;

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

uword g_uwWaitTime;
uword g_uwKeyRepeatTime;
uword g_uwBeepTime;
uword g_uwTimeout;
uword g_uwHeadSafetyTime;

//gloabl flags for polling
volatile int32_t global_flag_counter_10ms;
volatile int32_t global_counter_10ms;

bool _Diag0o1;
bool _Diag1o1;
bool _Diag0o2;
bool _Diag1o2;
bool _Diag0o3;
bool _Diag1o3;
bool _nReceiveRes1;
bool _nReceiveRes2;
bool _nReceiveRes3;
bool _nWagen_Null_Lage;
bool _nVorwarn;
bool _Oben;
bool _Unten;
bool _nKopf_eingesetzt;
bool _nMechZero1;
bool _nGripper_connected;
bool _nShield_connected;
bool _nGripper_Endlage;
bool _nShield_Endlage;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_DAC_Init(void);
static void MX_IWDG_Init(void);
/* USER CODE BEGIN PFP */

// encoder (old Timer - Register replacement)
extern volatile int16_t HrzEncoder;					// Enc.1 (was T2) replacements for old (C167CS) hardware registers
extern volatile int16_t VertEncoder;				// Enc.2 (was T3)
volatile int16_t 		T4Encoder; 					// Enc.3 (Was T4) nur bei Drehtisch (Disk) angeschlossen,initialisiert,aber nicht genutzt

// ----- tick, Timer, Watchdog
void HAL_IncTick(void)
{								// cycle: 10 ms
	uwTick += uwTickFreq;							// original system function
}

void TimeCounters10ms(void)
{
	if(g_uwTimeout > 0) g_uwTimeout--;
	if(g_uwWaitTime > 0) g_uwWaitTime--;
	if(g_uwBeepTime > 0) g_uwBeepTime--;
	if(g_uwKeyRepeatTime > 0) g_uwKeyRepeatTime--;
}

void SetWaitTime(unsigned short usValue)
{
	g_uwWaitTime = usValue;
}

unsigned short IsWaitTime(void)
{
	return g_uwWaitTime;
}

void ResetWaitTime(void)
{
	g_uwWaitTime = 0;
}

void WDO(void)
{
	HAL_IWDG_Refresh(&hiwdg);						// timeout: 4 s
}

// keyboard
void KeySetRequest(uint16_t KeyNumber)
{
	HAL_GPIO_WritePin(key_out1_GPIO_Port,key_out1_Pin,(KeyNumber & 1));
	HAL_GPIO_WritePin(key_out2_GPIO_Port,key_out2_Pin,(KeyNumber & 2));
	HAL_GPIO_WritePin(key_out3_GPIO_Port,key_out3_Pin,(KeyNumber & 4));
	HAL_GPIO_WritePin(key_out4_GPIO_Port,key_out4_Pin,(KeyNumber & 8));
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();



	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART3_UART_Init();
	MX_USB_OTG_FS_PCD_Init();
	MX_TIM4_Init();
	MX_TIM3_Init();
	MX_TIM8_Init();
	MX_TIM1_Init();
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_USART6_UART_Init();
	MX_SPI2_Init();
	MX_SPI3_Init();
	MX_DAC_Init();
	MX_IWDG_Init();


	//while(HAL_SetTickFreq(HAL_TICK_FREQ_100HZ) != HAL_OK)	WDO();	// init 10 ms Tick
	while(HAL_SetTickFreq(HAL_TICK_FREQ_1KHZ) != HAL_OK)	WDO();	// init  1 ms Tick

	// ----- keyboard
	KeySetRequest(0);		// set Decoder Address to key1

	// ----- Gripper,Shield -----
	Gripper_Stop;
	Gripper_Open;
	Shield_Stop;
	Shield_Open_VcStop;

	// ----- read io state
	for (int i=0; i<5;i++)
	{
		_Diag0o1 = Diag0o1_RD;							//Poll			(reserviert Motorsteuerung)
		_Diag1o1 = Diag1o1_RD;							//Poll		H
		_Diag0o2 = Diag0o2_RD;							//Poll
		_Diag1o2 = Diag1o2_RD;							//Poll		H
		_Diag0o3 = Diag0o3_RD;							//Poll
		_Diag1o3 = Diag1o3_RD;							//Poll		H

		_nReceiveRes1 = nReceiveRes1_RD;				//ExtI0		L	P5_0	DiskFineZero						(ENC1_N Drehtisch_485 - nur bei "Disk" aktiv)
		_nReceiveRes2 = nReceiveRes2_RD;				//ExtI1			P5_4										(ENC2_N Ausheber)
		_nReceiveRes3 = nReceiveRes3_RD;				//ExtI2			P5_8	!DiskRawZeroRequest					(ENC3_N Drehtisch_485 - nur Grobnull)

		_nVorwarn = nVorwarn_RD;						//ExtI4			IN0[1]	VertReadAdvancePos					(Ausheber)
		_Oben = Oben_RD;								//ExtI6		H	IN0[4]	unused								(Ausheber)
		_Unten = Unten_RD;								//ExtI7			IN0[0]	VertReadEndPos						(Ausheber)

		_nKopf_eingesetzt = nKopf_eingesetzt_RD;		//ExtI15	L	P2_8	pos_edge -> UnexpectedHeadRemove()	(XS19/2, mit XS6 verbunden)
		_nMechZero1 = nMechZero1_RD;					//ExtI8		L	P5_2	HeadConnectionPin; PullUp			(XS6, mit XS19/2 verbunden)

		//	nMechZero2 = HIGH;							//Closed		P5_6	IsHeadShieldOpen / JALOUSIE_OPEN	S4 - Abdeckung

		//	nMechZero3 = HIGH;							//Closed		P5_10	HrzZeroRequest3714					S2 - Kopfhaube

		_nWagen_Null_Lage = nWagen_Null_Lage_RD;		//ExtI14				HrzZeroRequestStd					(Wagen)

		//	IsHeadShieldOpen: (IN2[2]==0) (S4 Jalousie bis V1.20), (IN2[3]==0) (S4 Jalousie ab V1.21), (P5_6==0) (S4 Überwachung Abdeckung)

		_nGripper_connected = nGripper_connected_RD;	//Poll			(Parkzange)
		_nGripper_Endlage = nGripper_Endlage_RD;		//ExtI12

		_nShield_connected = nShield_connected_RD;		//Poll			(Schutzschild)
		_nShield_Endlage = nShield_Endlage_RD;			//ExtI13

		HAL_Delay(10);
		WDO();
	}

	// ----- PWM
	PWM_init();			// Stop_all,0° = 1520ms

	// ----- DAC ----- (Ain_Iref)
	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_2,DAC_ALIGN_12B_R,0x07FF);	// 50% = around 2V35 @ TMC,reserved / not used now

	// ----- Encoder -----
	SendRes1_H;			// no index puls
	ENCODER_start();	// start Encoder Counter
	// replacements for old (C167CS) hardware registers:
	HrzEncoder = 0;		// Enc.1 (was T2) Hrz / Tisch
	VertEncoder = 0;	// Enc.2 (was T3) Vert / Ausheber
	T4Encoder = 0; 		// Enc.3 (Was T4) Disk / Drehtisch - initialisiert, aber ungenutzt

	// ----- nucleo
	LD1_ON;				// Green (EEPROM nSPI3_Hold = not holding)
	LD2_ON;				// Blue
	LD3_ON;				// Red
	HAL_Delay(100);
	LD1_OFF;			// Green (EEPROM nSPI3_Hold)
	LD2_OFF;			// Blue
	LD3_OFF;			// Red
	WDO();

	// ----- Steppers -----
	HAL_Delay(900);	// wait of end TMC5130 Controllers boot-sequence
	WDO();

	MOT_EN_ON;
	SPI2_A0_H;			// address 4 -> dummy unit
	SPI2_A1_H;
	nSPI2_SS_H;			// not selected

	tmc5130a_init();	// Set Motor Preferences
	HAL_Delay(100);
	WDO();

	float varrpm = 0.2;												// Test mit Fließkommazahl
	tmc5130a_VELOCITY(0, (uint32_t) (varrpm	*M1_SpU	*uSpS /60));	// Pumpe
	tmc5130a_VELOCITY(1, (uint32_t) (varrpm	*M2_SpU	*uSpS /60));	// Ausheber
	tmc5130a_VELOCITY(2, (uint32_t) (varrpm	*M3_SpU	*uSpS /60));	// Drehtisch


	// ----- Buzzer -----
	Buzzer_Beep(50);

	// ----- uart -----
	DTR3_ON;
	UART3_Start();
	setvbuf(stdout, NULL, _IONBF, 0); //Send strings without nullterminator
	setvbuf( stdin, NULL, _IONBF, 0); //get single chars

	/* Infinite loop */
	while (1)
	{
		WDO();
		main_cybi();					// start / run original maschine
	}

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	*/
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

	DAC_ChannelConfTypeDef sConfig = {0};

	/** DAC Initialization
	*/
	hdac.Instance = DAC;
	if (HAL_DAC_Init(&hdac) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT2 config
	*/
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 50000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Analogue filter
	*/
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Digital filter
	*/
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

	/* USER CODE BEGIN IWDG_Init 0 */

	/* USER CODE END IWDG_Init 0 */

	/* USER CODE BEGIN IWDG_Init 1 */

	/* USER CODE END IWDG_Init 1 */
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN IWDG_Init 2 */

	/* USER CODE END IWDG_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

	/* USER CODE BEGIN SPI3_Init 0 */

	/* USER CODE END SPI3_Init 0 */

	/* USER CODE BEGIN SPI3_Init 1 */

	/* USER CODE END SPI3_Init 1 */
	/* SPI3 parameter configuration*/
	hspi3.Instance = SPI3;
	hspi3.Init.Mode = SPI_MODE_MASTER;
	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi3.Init.NSS = SPI_NSS_SOFT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi3.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI3_Init 2 */

	/* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{
	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 83;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 20000;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_TIM_DISABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_1);
	sConfigOC.Pulse = 1500;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_TIM_DISABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_2);
	sConfigOC.Pulse = 2000;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	__HAL_TIM_DISABLE_OCxPRELOAD(&htim3, TIM_CHANNEL_3);
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

	/* USER CODE BEGIN TIM4_Init 0 */

	/* USER CODE END TIM4_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 65535;
	htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM4_Init 2 */

	/* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

	/* USER CODE BEGIN TIM8_Init 0 */

	/* USER CODE END TIM8_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM8_Init 1 */

	/* USER CODE END TIM8_Init 1 */
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 0;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 65535;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM8_Init 2 */

	/* USER CODE END TIM8_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_CTS;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 19200;
	huart3.Init.WordLength = UART_WORDLENGTH_9B;
	huart3.Init.StopBits = UART_STOPBITS_2;
	huart3.Init.Parity = UART_PARITY_ODD;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

	/* USER CODE BEGIN USART6_Init 0 */

	/* USER CODE END USART6_Init 0 */

	/* USER CODE BEGIN USART6_Init 1 */

	/* USER CODE END USART6_Init 1 */
	huart6.Instance = USART6;
	huart6.Init.BaudRate = 9600;
	huart6.Init.WordLength = UART_WORDLENGTH_8B;
	huart6.Init.StopBits = UART_STOPBITS_1;
	huart6.Init.Parity = UART_PARITY_NONE;
	huart6.Init.Mode = UART_MODE_TX_RX;
	huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart6.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart6) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART6_Init 2 */

	/* USER CODE END USART6_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

	/* USER CODE BEGIN USB_OTG_FS_Init 0 */

	/* USER CODE END USB_OTG_FS_Init 0 */

	/* USER CODE BEGIN USB_OTG_FS_Init 1 */

	/* USER CODE END USB_OTG_FS_Init 1 */
	hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
	hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
	hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
	if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USB_OTG_FS_Init 2 */

	/* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOF, nSPI3_WP_Pin|nGripper_dir_close_Pin|nGripper_start_Pin|nShield_start_Pin
						  |nShield_dir_close_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOF, SendRes1_Pin|key_out1_Pin|key_out2_Pin|key_out3_Pin
						  |key_out4_Pin|Refl_Step1_Pin|Refr_Dir1_Pin|Refl_Step2_Pin
						  |Refr_Dir2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SPI2_A1_GPIO_Port, SPI2_A1_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port, nSPI3_SS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, nSPI3_Hold_Pin|nSPI2_SS_Pin|SPI2_A0_Pin|nMOT_EN_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, Refl_Step3_Pin|Refr_Dir3_Pin|USB_GPIO_OUT_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(nDTR3_GPIO_Port, nDTR3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(nBuzzer_GPIO_Port, nBuzzer_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : Diag0o1_Pin Diag1o1_Pin Diag0o2_Pin Diag1o2_Pin
						   Diag0o3_Pin Diag1o3_Pin nMechZero1_Pin nGripper_Endlage_Pin
						   nShield_Endlage_Pin */
	GPIO_InitStruct.Pin = Diag0o1_Pin|Diag1o1_Pin|Diag0o2_Pin|Diag1o2_Pin
						  |Diag0o3_Pin|Diag1o3_Pin|nMechZero1_Pin|nGripper_Endlage_Pin
						  |nShield_Endlage_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : nSPI3_WP_Pin SendRes1_Pin key_out1_Pin key_out2_Pin
						   key_out3_Pin key_out4_Pin nGripper_dir_close_Pin nGripper_start_Pin
						   nShield_start_Pin nShield_dir_close_Pin Refl_Step1_Pin Refr_Dir1_Pin
						   Refl_Step2_Pin Refr_Dir2_Pin */
	GPIO_InitStruct.Pin = nSPI3_WP_Pin|SendRes1_Pin|key_out1_Pin|key_out2_Pin
						  |key_out3_Pin|key_out4_Pin|nGripper_dir_close_Pin|nGripper_start_Pin
						  |nShield_start_Pin|nShield_dir_close_Pin|Refl_Step1_Pin|Refr_Dir1_Pin
						  |Refl_Step2_Pin|Refr_Dir2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pin : SPI2_A1_Pin */
	GPIO_InitStruct.Pin = SPI2_A1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SPI2_A1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : RMII_MDC_Pin RMII_RXD0_Pin RMII_RXD1_Pin */
	GPIO_InitStruct.Pin = RMII_MDC_Pin|RMII_RXD0_Pin|RMII_RXD1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : RMII_REF_CLK_Pin RMII_MDIO_Pin RMII_CRS_DV_Pin */
	GPIO_InitStruct.Pin = RMII_REF_CLK_Pin|RMII_MDIO_Pin|RMII_CRS_DV_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : nSPI3_SS_Pin */
	GPIO_InitStruct.Pin = nSPI3_SS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(nSPI3_SS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : nSPI3_Hold_Pin nSPI2_SS_Pin LD3_Pin SPI2_A0_Pin
						   nMOT_EN_Pin LD2_Pin */
	GPIO_InitStruct.Pin = nSPI3_Hold_Pin|nSPI2_SS_Pin|LD3_Pin|SPI2_A0_Pin
						  |nMOT_EN_Pin|LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : Refl_Step3_Pin Refr_Dir3_Pin nBuzzer_Pin USB_GPIO_OUT_Pin */
	GPIO_InitStruct.Pin = Refl_Step3_Pin|Refr_Dir3_Pin|nBuzzer_Pin|USB_GPIO_OUT_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : nGripper_connected_Pin nShield_connected_Pin */
	GPIO_InitStruct.Pin = nGripper_connected_Pin|nShield_connected_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : RMII_TXD1_Pin */
	GPIO_InitStruct.Pin = RMII_TXD1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(RMII_TXD1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : nDTR3_Pin */
	GPIO_InitStruct.Pin = nDTR3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(nDTR3_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : nWagen_Null_Lage_Pin nReceiveRes1_Pin nReceiveRes2_Pin nReceiveRes3_Pin
						   nVorwarn_Pin Oben_Pin Unten_Pin */
	GPIO_InitStruct.Pin = nWagen_Null_Lage_Pin|nReceiveRes1_Pin|nReceiveRes2_Pin|nReceiveRes3_Pin
						  |nVorwarn_Pin|Oben_Pin|Unten_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : nKopf_eingesetzt_Pin */
	GPIO_InitStruct.Pin = nKopf_eingesetzt_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(nKopf_eingesetzt_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_OverCurrent_Pin */
	GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : nkey_in_Pin */
	GPIO_InitStruct.Pin = nkey_in_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(nkey_in_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : RMII_TX_EN_Pin RMII_TXD0_Pin */
	GPIO_InitStruct.Pin = RMII_TX_EN_Pin|RMII_TXD0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
// ----- Input (Interrupts) -----
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin){
#ifdef nReceiveRes1_EXTI_IRQn
	case nReceiveRes1_Pin:		// EXTI0_IRQn
		_nReceiveRes1 = nReceiveRes1_RD;
		break;
#endif
#ifdef nReceiveRes2_EXTI_IRQn
	case nReceiveRes2_Pin: 		// EXTI1_IRQn
		_nReceiveRes2 = nReceiveRes2_RD;
		break;
#endif
#ifdef nReceiveRes3_EXTI_IRQn
	case nReceiveRes3_Pin:		// EXTI2_IRQn
		_nReceiveRes3 = nReceiveRes3_RD;
		break;
#endif
#ifdef Diag0o1_EXTI_IRQn
	case Diag0o1_Pin:			// EXTI2_IRQn
		_Diag0o1 = Diag0o1_RD;
		break;
#endif
#ifdef Diag1o1_EXTI_IRQn
	case Diag1o1_Pin:			// EXTI3_IRQn
		_Diag1o1 = Diag1o1_RD;
		break;
#endif
#ifdef nVorwarn_EXTI_IRQn
	case nVorwarn_Pin:			// EXTI4_IRQn
		_nVorwarn = nVorwarn_RD;
		break;
#endif
#ifdef Diag0o2_EXTI_IRQn
	case Diag0o2_Pin:			// EXTI4_IRQn
		_Diag0o2 = Diag0o2_RD;
		break;
#endif
#ifdef Diag1o2_EXTI_IRQn
	case Diag1o2_Pin:			// EXTI9_5_IRQn
		_Diag1o2 = Diag1o2_RD;
		break;
#endif
#ifdef Oben_EXTI_IRQn
	case Oben_Pin:				// EXTI9_5_IRQn (6)
		_Oben = Oben_RD;
		break;
#endif
#ifdef Unten_EXTI_IRQn
	case Unten_Pin:				// EXTI9_5_IRQn (7)
		_Unten = Unten_RD;
		break;
#endif
#ifdef nMechZero1_EXTI_IRQn
	case nMechZero1_Pin:			// EXTI9_5_IRQn (8)
		_nMechZero1 = nMechZero1_RD;
		break;
#endif
#ifdef Diag0o3_EXTI_IRQn
	case Diag0o3_Pin:			// EXTI9_5_IRQn
		_Diag0o3 = Diag0o3_RD;
		break;
#endif
#ifdef Diag1o3_EXTI_IRQn
	case Diag1o3_Pin:			// EXTI9_5_IRQn
		_Diag1o3 = Diag1o3_RD;
		break;
#endif
#ifdef nGripper_Endlage_EXTI_IRQn
	case nGripper_Endlage_Pin:	// EXTI15_10_IRQn (12)
		_nGripper_Endlage = nGripper_Endlage_RD;
		break;
#endif
#ifdef nShield_Endlage_EXTI_IRQn
	case nShield_Endlage_Pin:	// EXTI15_10_IRQn (13)
		_nShield_Endlage = nShield_Endlage_RD;
		break;
#endif
#ifdef nGripper_connected_EXTI_IRQn
	case nGripper_connected_Pin:	// EXTI15_10_IRQn
		_nGripper_connected = nGripper_connected_RD;
		break;
#endif
#ifdef nShield_connected_EXTI_IRQn
	case nShield_connected_Pin:	// EXTI15_10_IRQn
		_nShield_connected = nShield_connected_RD;
		break;
#endif
#ifdef nWagen_Null_Lage_EXTI_IRQn
	case nWagen_Null_Lage_Pin:	// EXTI15_10_IRQn (14)
		_nWagen_Null_Lage = nWagen_Null_Lage_RD;
		break;
#endif
#ifdef nKopf_eingesetzt_EXTI_IRQn
	case nKopf_eingesetzt_Pin:	// EXTI15_10_IRQn (15)
		//_nKopf_eingesetzt = nKopf_eingesetzt_RD;
		// was P2.8 -> CC8INT
//		if(_nKopf_eingesetzt){							// only positive edge
			if (HeadInterruptEnabled)
			{
				Wait(10);
				_nKopf_eingesetzt= nKopf_eingesetzt_RD;	//Störungen filtern
				if(_nKopf_eingesetzt){UnexpectedHeadRemove();}
			}
//		}
		break;
#endif
	default:
		break;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n",file,line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
