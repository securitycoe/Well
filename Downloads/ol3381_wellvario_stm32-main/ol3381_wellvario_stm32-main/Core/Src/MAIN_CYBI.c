//****************************************************************************
// @Module		Project Settings
// @Filename	  MAIN.C
// @Project	   ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CS-L
//
// @Compiler	  Tasking
//
// @Codegenerator 2.2
//
// @Description   This file contains the project initialization function.
//
//----------------------------------------------------------------------------
// @Date		  24.04.2008 10:08:43
//
//****************************************************************************
// USER CODE BEGIN (MAIN_General,1)

// USER CODE END

//****************************************************************************
// @Project Includes
//****************************************************************************
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include <_pwm_servo.h>
#include <_spi_eeprom.h>

// ---------- Master
#include "ch_ig_err.h"
#include "ch_ig_hrz.h"
#include "ch_ig_peri.h"
#include "ch_ig_piston.h"
#include "ch_ig_utility.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein
#include "Well_Error.h"

// USER CODE END

//****************************************************************************
// @Macros
//****************************************************************************
// USER CODE BEGIN (MAIN_General,3)
bool HeadInterruptEnabled = 1;

// USER CODE END

//****************************************************************************
// @Function	  void main(void)
//
//----------------------------------------------------------------------------
// @Description   This is the main function.
//
//----------------------------------------------------------------------------
// @Returnvalue   None
//
//----------------------------------------------------------------------------
// @Parameters	None
//
//----------------------------------------------------------------------------
// @Date		  24.04.2008
//
//****************************************************************************
	// USER CODE BEGIN (Main,1)

	// USER CODE END

void main_cybi(void)
{
	// USER CODE BEGIN (Main,2)
	// USER CODE END
	HeadInterruptEnabled = DISABLE;

	InitEEprom();
	//eepromInit(); //Standardwerte einspeichern

	CarDrvSetCurrentToZero();
	VertDrvSetCurrentToZero();
	PstDrvSetCurrentToZero();

	Wait(60);	//40 Grenzfall
	LcdInit();

	UserMain();
	// USER CODE END
}

// USER CODE BEGIN (MAIN_General,10)

void ErrorDisplayFP(void)
{
	ErrorDisplay(31);
}
// USER CODE END
