/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "typedefs.h"
#include "aj_typedefs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define $Name		"CyBiVario"
#define $Version	"0.1 TBD"
#define $Datum		"2022-11-04"

#define CLOCKWISE	TRUE

extern volatile int32_t global_flag_counter_10ms;
extern volatile int32_t global_counter_10ms;

extern bool HeadInterrupt;
extern bool HeadInterruptEnabled;

extern unsigned short g_uiDeviceType;


extern const char HW_Revision[];

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

void WDO(void);
void ResetWaitTime(void);
unsigned short IsWaitTime(void);
void SetWaitTime(unsigned short usValue);

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void TimeCounters10ms(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Diag0o1_Pin GPIO_PIN_2
#define Diag0o1_GPIO_Port GPIOE
#define Diag1o1_Pin GPIO_PIN_3
#define Diag1o1_GPIO_Port GPIOE
#define Diag0o2_Pin GPIO_PIN_4
#define Diag0o2_GPIO_Port GPIOE
#define Diag1o2_Pin GPIO_PIN_5
#define Diag1o2_GPIO_Port GPIOE
#define Diag0o3_Pin GPIO_PIN_6
#define Diag0o3_GPIO_Port GPIOE
#define nSPI3_WP_Pin GPIO_PIN_0
#define nSPI3_WP_GPIO_Port GPIOF
#define SendRes1_Pin GPIO_PIN_1
#define SendRes1_GPIO_Port GPIOF
#define key_out1_Pin GPIO_PIN_2
#define key_out1_GPIO_Port GPIOF
#define key_out2_Pin GPIO_PIN_3
#define key_out2_GPIO_Port GPIOF
#define key_out3_Pin GPIO_PIN_4
#define key_out3_GPIO_Port GPIOF
#define key_out4_Pin GPIO_PIN_5
#define key_out4_GPIO_Port GPIOF
#define nGripper_dir_close_Pin GPIO_PIN_7
#define nGripper_dir_close_GPIO_Port GPIOF
#define nGripper_start_Pin GPIO_PIN_8
#define nGripper_start_GPIO_Port GPIOF
#define nShield_start_Pin GPIO_PIN_9
#define nShield_start_GPIO_Port GPIOF
#define nShield_dir_close_Pin GPIO_PIN_10
#define nShield_dir_close_GPIO_Port GPIOF
#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOH
#define SPI2_A1_Pin GPIO_PIN_0
#define SPI2_A1_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define nSPI3_SS_Pin GPIO_PIN_4
#define nSPI3_SS_GPIO_Port GPIOA
#define PWM1_Pin GPIO_PIN_6
#define PWM1_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define nSPI3_Hold_Pin GPIO_PIN_0
#define nSPI3_Hold_GPIO_Port GPIOB
#define Refl_Step1_Pin GPIO_PIN_12
#define Refl_Step1_GPIO_Port GPIOF
#define Refr_Dir1_Pin GPIO_PIN_13
#define Refr_Dir1_GPIO_Port GPIOF
#define Refl_Step2_Pin GPIO_PIN_14
#define Refl_Step2_GPIO_Port GPIOF
#define Refr_Dir2_Pin GPIO_PIN_15
#define Refr_Dir2_GPIO_Port GPIOF
#define Refl_Step3_Pin GPIO_PIN_0
#define Refl_Step3_GPIO_Port GPIOG
#define Refr_Dir3_Pin GPIO_PIN_1
#define Refr_Dir3_GPIO_Port GPIOG
#define Diag1o3_Pin GPIO_PIN_7
#define Diag1o3_GPIO_Port GPIOE
#define nMechZero1_Pin GPIO_PIN_8
#define nMechZero1_GPIO_Port GPIOE
#define ENC1A_Pin GPIO_PIN_9
#define ENC1A_GPIO_Port GPIOE
#define ENC1B_Pin GPIO_PIN_11
#define ENC1B_GPIO_Port GPIOE
#define nGripper_Endlage_Pin GPIO_PIN_12
#define nGripper_Endlage_GPIO_Port GPIOE
#define nShield_Endlage_Pin GPIO_PIN_13
#define nShield_Endlage_GPIO_Port GPIOE
#define nGripper_connected_Pin GPIO_PIN_14
#define nGripper_connected_GPIO_Port GPIOE
#define nShield_connected_Pin GPIO_PIN_15
#define nShield_connected_GPIO_Port GPIOE
#define STLK_RX_Pin GPIO_PIN_10
#define STLK_RX_GPIO_Port GPIOB
#define STLK_TX_Pin GPIO_PIN_11
#define STLK_TX_GPIO_Port GPIOB
#define nSPI2_SS_Pin GPIO_PIN_12
#define nSPI2_SS_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_14
#define LD3_GPIO_Port GPIOB
#define SPI2_A0_Pin GPIO_PIN_15
#define SPI2_A0_GPIO_Port GPIOB
#define nDTR3_Pin GPIO_PIN_12
#define nDTR3_GPIO_Port GPIOD
#define ENC2B_Pin GPIO_PIN_13
#define ENC2B_GPIO_Port GPIOD
#define nWagen_Null_Lage_Pin GPIO_PIN_14
#define nWagen_Null_Lage_GPIO_Port GPIOD
#define nKopf_eingesetzt_Pin GPIO_PIN_15
#define nKopf_eingesetzt_GPIO_Port GPIOD
#define nKopf_eingesetzt_EXTI_IRQn EXTI15_10_IRQn
#define nBuzzer_Pin GPIO_PIN_2
#define nBuzzer_GPIO_Port GPIOG
#define USB_GPIO_OUT_Pin GPIO_PIN_6
#define USB_GPIO_OUT_GPIO_Port GPIOG
#define USB_OverCurrent_Pin GPIO_PIN_7
#define USB_OverCurrent_GPIO_Port GPIOG
#define PWM3_Pin GPIO_PIN_8
#define PWM3_GPIO_Port GPIOC
#define nkey_in_Pin GPIO_PIN_9
#define nkey_in_GPIO_Port GPIOC
#define USB_SOF_Pin GPIO_PIN_8
#define USB_SOF_GPIO_Port GPIOA
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_ID_Pin GPIO_PIN_10
#define USB_ID_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define nReceiveRes1_Pin GPIO_PIN_0
#define nReceiveRes1_GPIO_Port GPIOD
#define nReceiveRes2_Pin GPIO_PIN_1
#define nReceiveRes2_GPIO_Port GPIOD
#define nReceiveRes3_Pin GPIO_PIN_2
#define nReceiveRes3_GPIO_Port GPIOD
#define nVorwarn_Pin GPIO_PIN_4
#define nVorwarn_GPIO_Port GPIOD
#define Oben_Pin GPIO_PIN_6
#define Oben_GPIO_Port GPIOD
#define Unten_Pin GPIO_PIN_7
#define Unten_GPIO_Port GPIOD
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define nMOT_EN_Pin GPIO_PIN_4
#define nMOT_EN_GPIO_Port GPIOB
#define PWM2_Pin GPIO_PIN_5
#define PWM2_GPIO_Port GPIOB
#define ENC2A_Pin GPIO_PIN_6
#define ENC2A_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_7
#define LD2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	(!FALSE)
#endif
#ifndef LOW
#define LOW		0
#endif
#ifndef HIGH
#define HIGH	(!LOW)
#endif

#define nkey_in_RD (HAL_GPIO_ReadPin(nkey_in_GPIO_Port,nkey_in_Pin))
#define nReceiveRes1_RD (HAL_GPIO_ReadPin(nReceiveRes1_GPIO_Port,nReceiveRes1_Pin))							// EXTI_0
#define nReceiveRes2_RD (HAL_GPIO_ReadPin(nReceiveRes2_GPIO_Port,nReceiveRes2_Pin))							// EXTI_1
#define nReceiveRes3_RD (HAL_GPIO_ReadPin(nReceiveRes3_GPIO_Port,nReceiveRes3_Pin))							// EXTI_2
#define nVorwarn_RD (HAL_GPIO_ReadPin(nVorwarn_GPIO_Port,nVorwarn_Pin))										// EXTI_4
#define Oben_RD (HAL_GPIO_ReadPin(Oben_GPIO_Port,Oben_Pin))													// EXTI_6
#define Unten_RD (HAL_GPIO_ReadPin(Unten_GPIO_Port,Unten_Pin))												// EXTI_7
#define nMechZero1_RD (HAL_GPIO_ReadPin(nMechZero1_GPIO_Port,nMechZero1_Pin))								// EXTI_8	!P5_2 HeadConnectionPin
#define nGripper_Endlage_RD (HAL_GPIO_ReadPin(nGripper_Endlage_GPIO_Port,nGripper_Endlage_Pin))				// EXTI_12	!P2_3 IsGripperEndPos
#define nShield_Endlage_RD (HAL_GPIO_ReadPin(nShield_Endlage_GPIO_Port,nShield_Endlage_Pin))				// EXTI_13	P2_6 IsPresMechPresentPos
#define nWagen_Null_Lage_RD (HAL_GPIO_ReadPin(nWagen_Null_Lage_GPIO_Port,nWagen_Null_Lage_Pin))				// EXTI_14
#define nKopf_eingesetzt_RD (HAL_GPIO_ReadPin(nKopf_eingesetzt_GPIO_Port,nKopf_eingesetzt_Pin))				// EXTI_15
#define nGripper_connected_RD (HAL_GPIO_ReadPin(nGripper_connected_GPIO_Port,nGripper_connected_Pin))		//			P2_2 IsGripperNotConnected
#define nShield_connected_RD (HAL_GPIO_ReadPin(nShield_connected_GPIO_Port,nShield_connected_Pin))			//			P2_7 IsPresMechCyBiWellPos()

#define Gripper_Close (HAL_GPIO_WritePin(nGripper_dir_close_GPIO_Port,nGripper_dir_close_Pin,GPIO_PIN_RESET))		//	P2_2 GripperClose
#define Gripper_Open (HAL_GPIO_WritePin(nGripper_dir_close_GPIO_Port,nGripper_dir_close_Pin,GPIO_PIN_SET))			//	P2_2 GripperOpen

#define Gripper_Start (HAL_GPIO_WritePin(nGripper_start_GPIO_Port,nGripper_start_Pin,GPIO_PIN_RESET))				//	P2_0 GripperStart
#define Gripper_Stop (HAL_GPIO_WritePin(nGripper_start_GPIO_Port,nGripper_start_Pin,GPIO_PIN_SET))					//	P2_0 GripperStop

#define Shield_Start (HAL_GPIO_WritePin(nShield_start_GPIO_Port,nShield_start_Pin,GPIO_PIN_RESET))					//	P2_4 MagVentilCyBiWellPos
#define Shield_Stop (HAL_GPIO_WritePin(nShield_start_GPIO_Port,nShield_start_Pin,GPIO_PIN_SET))						//	P2_4 MagVentilPresentPos

#define Shield_Close_VcStart (HAL_GPIO_WritePin(nShield_dir_close_GPIO_Port,nShield_dir_close_Pin,GPIO_PIN_RESET))	//	P2_5 VcStart - unused
#define Shield_Open_VcStop (HAL_GPIO_WritePin(nShield_dir_close_GPIO_Port,nShield_dir_close_Pin,GPIO_PIN_SET))		//	P2_5 VcStop  - unused

#define nMechZero2	1																								//	P5_6  Pulldown,obsolete (Abdeckung S4)
#define nMechZero3	0																								//	P5_10 Pulldown,obsolete (Kopfhaube S2)

#define Diag0o1_RD (HAL_GPIO_ReadPin(Diag0o1_GPIO_Port,Diag0o1_Pin))
#define Diag1o1_RD (HAL_GPIO_ReadPin(Diag1o1_GPIO_Port,Diag1o1_Pin))
#define Diag0o2_RD (HAL_GPIO_ReadPin(Diag0o2_GPIO_Port,Diag0o2_Pin))
#define Diag1o2_RD (HAL_GPIO_ReadPin(Diag1o2_GPIO_Port,Diag1o2_Pin))
#define Diag0o3_RD (HAL_GPIO_ReadPin(Diag0o3_GPIO_Port,Diag0o3_Pin))
#define Diag1o3_RD (HAL_GPIO_ReadPin(Diag1o3_GPIO_Port,Diag1o3_Pin))

#define LD1_OFF (HAL_GPIO_WritePin(nSPI3_Hold_GPIO_Port,nSPI3_Hold_Pin,GPIO_PIN_RESET))				// LD1 Green
#define LD1_ON (HAL_GPIO_WritePin(nSPI3_Hold_GPIO_Port,nSPI3_Hold_Pin,GPIO_PIN_SET))
#define nSPI3_Hold_L (HAL_GPIO_WritePin(nSPI3_Hold_GPIO_Port,nSPI3_Hold_Pin,GPIO_PIN_RESET))		// = LD1
#define nSPI3_Hold_H (HAL_GPIO_WritePin(nSPI3_Hold_GPIO_Port,nSPI3_Hold_Pin,GPIO_PIN_SET))

#define MOT_EN_ON (HAL_GPIO_WritePin(nMOT_EN_GPIO_Port,nMOT_EN_Pin,GPIO_PIN_RESET))
#define MOT_EN_OFF (HAL_GPIO_WritePin(nMOT_EN_GPIO_Port,nMOT_EN_Pin,GPIO_PIN_SET))

#define LD2_OFF (HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_RESET))							// LD2 Blue
#define LD2_ON (HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,GPIO_PIN_SET))

#define nSPI2_SS_L (HAL_GPIO_WritePin(nSPI2_SS_GPIO_Port,nSPI2_SS_Pin,GPIO_PIN_RESET))
#define nSPI2_SS_H (HAL_GPIO_WritePin(nSPI2_SS_GPIO_Port,nSPI2_SS_Pin,GPIO_PIN_SET))

#define nSPI3_SS_L (HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,nSPI3_SS_Pin,GPIO_PIN_RESET))
#define nSPI3_SS_H (HAL_GPIO_WritePin(nSPI3_SS_GPIO_Port,nSPI3_SS_Pin,GPIO_PIN_SET))

#define LD3_OFF (HAL_GPIO_WritePin(LD3_GPIO_Port,LD3_Pin,GPIO_PIN_RESET)) 							// LD3 Red
#define LD3_ON (HAL_GPIO_WritePin(LD3_GPIO_Port,LD3_Pin,GPIO_PIN_SET))

#define SPI2_A0_L (HAL_GPIO_WritePin(SPI2_A0_GPIO_Port,SPI2_A0_Pin,GPIO_PIN_RESET))
#define SPI2_A0_H (HAL_GPIO_WritePin(SPI2_A0_GPIO_Port,SPI2_A0_Pin,GPIO_PIN_SET))

#define SPI2_A1_L (HAL_GPIO_WritePin(SPI2_A1_GPIO_Port,SPI2_A1_Pin,GPIO_PIN_RESET))
#define SPI2_A1_H (HAL_GPIO_WritePin(SPI2_A1_GPIO_Port,SPI2_A1_Pin,GPIO_PIN_SET))

#define nSPI3_WP_L (HAL_GPIO_WritePin(nSPI3_WP_GPIO_Port,GPIO_PIN_0,GPIO_PIN_RESET))
#define nSPI3_WP_H (HAL_GPIO_WritePin(nSPI3_WP_GPIO_Port,GPIO_PIN_0,GPIO_PIN_SET))

#define SendRes1_L (HAL_GPIO_WritePin(SendRes1_GPIO_Port,SendRes1_Pin,GPIO_PIN_RESET))
#define SendRes1_H (HAL_GPIO_WritePin(SendRes1_GPIO_Port,SendRes1_Pin,GPIO_PIN_SET))

#define DTR3_OFF (HAL_GPIO_WritePin(nDTR3_GPIO_Port,nDTR3_Pin,GPIO_PIN_RESET))
#define DTR3_ON (HAL_GPIO_WritePin(nDTR3_GPIO_Port,nDTR3_Pin,GPIO_PIN_SET))

#define Refl_Step1_L (HAL_GPIO_WritePin(Refl_Step1_GPIO_Port,Refl_Step1_Pin,GPIO_PIN_RESET))
#define Refl_Step1_H (HAL_GPIO_WritePin(Refl_Step1_GPIO_Port,Refl_Step1_Pin,GPIO_PIN_SET))

#define Refr_Dir1_L (HAL_GPIO_WritePin(Refr_Dir1_GPIO_Port,Refr_Dir1_Pin,GPIO_PIN_RESET))
#define Refr_Dir1_H (HAL_GPIO_WritePin(Refr_Dir1_GPIO_Port,Refr_Dir1_Pin,GPIO_PIN_SET))

#define Refl_Step2_L (HAL_GPIO_WritePin(Refl_Step2_GPIO_Port,Refl_Step2_Pin,GPIO_PIN_RESET))
#define Refl_Step2_H (HAL_GPIO_WritePin(Refl_Step2_GPIO_Port,Refl_Step2_Pin,GPIO_PIN_SET))

#define Refr_Dir2_L (HAL_GPIO_WritePin(Refr_Dir2_GPIO_Port,Refr_Dir2_Pin,GPIO_PIN_RESET))
#define Refr_Dir2_H (HAL_GPIO_WritePin(Refr_Dir2_GPIO_Port,Refr_Dir2_Pin,GPIO_PIN_SET))

#define Refl_Step3_L (HAL_GPIO_WritePin(Refl_Step3_GPIO_Port,Refl_Step3_Pin,GPIO_PIN_RESET))
#define Refl_Step3_H (HAL_GPIO_WritePin(Refl_Step3_GPIO_Port,Refl_Step3_Pin,GPIO_PIN_SET))

#define Refr_Dir3_L (HAL_GPIO_WritePin(Refr_Dir3_GPIO_Port,Refr_Dir3_Pin,GPIO_PIN_RESET))
#define Refr_Dir3_H (HAL_GPIO_WritePin(Refr_Dir3_GPIO_Port,Refr_Dir3_Pin,GPIO_PIN_SET))

#define Buzzer_ON (HAL_GPIO_WritePin(nBuzzer_GPIO_Port,nBuzzer_Pin,GPIO_PIN_RESET))
#define Buzzer_OFF (HAL_GPIO_WritePin(nBuzzer_GPIO_Port,nBuzzer_Pin,GPIO_PIN_SET))

#define Buzzer_Beep(tim) Buzzer_ON;HAL_Delay(tim);Buzzer_OFF

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
