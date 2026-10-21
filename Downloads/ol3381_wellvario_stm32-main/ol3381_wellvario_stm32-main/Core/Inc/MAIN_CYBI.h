//****************************************************************************
// @Module		Project Settings
// @Filename	  _MAIN.H
// @Project	   ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CS-L
//
// @Compiler	  Tasking
//
// @Codegenerator 2.2
//
// @Description   This file contains all function prototypes and macros for
//				the MAIN module.
//
//----------------------------------------------------------------------------
// @Date		  24.04.2008 10:08:42
//
//****************************************************************************
// USER CODE BEGIN (MAIN_Header,1)

// USER CODE END
#ifndef _MAIN_CIBY_H_
#define _MAIN_CIBY_H_

#ifndef FALSE
	#define FALSE			0
#endif
#ifndef TRUE
	#define TRUE			(!FALSE)
#endif

// Steuerung Timer für Schrittmotoren
#define DRV_ENABLE 			1
#define DRV_DISABLE 		0

#define HrzDrvRunBit(v)		tmc5130a_STOP(HRZ_DEVICE);//T0R = v
#define PstDrvRunBit(v)		tmc5130a_STOP(P_DEVICE);//T1R = v
#define VertDrvRunBit(v)	tmc5130a_STOP(VERT_DEVICE);//T7R = v

#define STDOUT_BUF			80

#define ROUNDING_ERROR		1E-04

typedef ubyte ErrorState_t;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <setjmp.h>

#include "ig_define.h"
// ---------- Inc:
#include <_pwm_servo.h>
// ---------- Master/C167CS:

// ---------- Master:
#include "ch_ig_utility.h"
#include "General_Fkt.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur:
// ---------- Pipettor_Allgemein:
#define FW_DEBUG(x, f) printf("%s: param=%x\n\r", x, f);
#define PIN_DEBUG HAL_GPIO_TogglePin(nShield_start_GPIO_Port, nShield_start_Pin);
void MAIN_vInit(void);
void main_cybi(void);
void ErrorDisplayFP(void);

#endif	// __MAIN_H_
