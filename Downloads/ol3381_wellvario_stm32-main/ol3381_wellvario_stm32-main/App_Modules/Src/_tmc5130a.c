/* CODE BEGIN Header */
/**
 ******************************************************************************
 * @file			: tmc5130a.c
 *
 * @created		: 21.04.2022 11:00
 *
 * @Author			: Ilmotronics
 ******************************************************************************
 */
/* CODE END Header */
#include <math.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Inc
#include <_tmc5130a.h>
#include <Well_Error.h>
#include <ch_ig_err.h>
extern SPI_HandleTypeDef hspi2;					// external for SPI routines

uint8_t devadr = 0xff;
uint8_t regadr = 0xff;
uint32_t tdata = 0xffffffff;
int32_t s32data = -1;

int32_t stdata = -1;
unsigned char txbuf[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };// (register adress byte (incl. r/w bit) & 4 data byte
unsigned char rxbuf[5] = { 0xff, 0xff, 0xff, 0xff, 0xff };// (status byte) & 4 data byte
uint8_t devstat = 0xff;			// device states
uint32_t rdata = 0xffffffff;	// first answer = obsolete old data

uint8_t devstat2 = 0xff;		// device states
uint32_t urdata2 = 0x00000000;// second answer = new data,converted to -> unsigned Double
int32_t srdata2 = -1;// second answer = new data,converted to -> signed Double

uint8_t spread_cfg[5] = { 5, 4, 1, 0, 1 };


struct Motor motor_1, motor_2, motor_3;

/* ******************************************************************************
 *  FUNCTION: tmc5130a_init
 *  PARAMETERS: none
 *  DESCRIPTION: Initialisierung und Standardkonfiguration
 *  RETURNS: none
 ******************************************************************************** */

void tmc5130a_init(void) {
	devadr = 3;			// dummy unit,only for init spi-HW (spi-clk startlevel)
	regadr = 0x21;
	tdata = 0x02030405;
	tcm5130a_spi_transmitreceive(devadr, rxbuf, txbuf);
	HAL_Delay(1000);// before write real TMC,wait of End TMC voltage reset

// 	tmc device answer,description state byte
// 	Bit	Name				Comment
// 	7  status_stop_r  		RAMP_STAT[1] – 1: Signals stop right switch status (motion controller only)
//	6  status_stop_l  		RAMP_STAT[0] – 1: Signals stop left switch status (motion controller only)
//	5  position_reached  	RAMP_STAT[9] – 1: Signals target reached (motion controller only)
//	4  velocity_reached  	RAMP_STAT[8] – 1: Signals target velocity reached (motion controller only)
//	3  standstill  			DRV_STATUS[31] – 1: Signals motor stand still
//	2  sg2  				DRV_STATUS[24] – 1:  Signals stallguard flag active
//	1  driver_error  		GSTAT[1] – 1:  Signals driver 1 driver error (clear by reading GSTAT)
//	0  reset_flag  			GSTAT[0] – 1: Signals,that a reset has occurred (clear by reading GSTAT)

// -----------------------------------
// standard config
	cfg_reg cfg_powerup[] = {
// GENERAL CONFIGURATION REGISTERS
//		 {0x80,0x00000004}	// RW	00 GCONF,en_pwm_mode	// tmc5130a en_pwm_mode
//		{0x80,0x00000000}	// RW	00 GCONF
			{ 0x80, 0x00000040}	// RW	00 GCONF
									// b5	1:  Enable DIAG0 active on driver errors:
									// Over  temperature  (ot),  short  to  GND  (s2g), undervoltage chargepump (uv_cp)
									// DIAG0 always shows the reset-status, i.e. is active low during reset condition.
									//b2    1: StealthChop voltage PWM mode enabled (depending on velocity threshholds).
									//		   Switch from off to on state while in stand still, only

			, { 0x01, 0x00000000 }	// R+C	01 GSTAT read & clear
//		,{0x02,0x00000000}	// R	02 IFCNT
//		,{0x83,0x00000000}	// W	03 SLAVECONF

//		,{0x04,0x00000000}	// R	04 IOIN
//		,{0x84,0x00000001}	// W	04 OUTPUT

//		,{0x85,0x00000000}	// W	05 X_COMPARE

// VELOCITY DEPENDENT DRIVER FEATURE CONTROL REGISTER SET
//		,{0x90,0x00061F0A}	// W	10 IHOLD_IRUN (ori) Imax,Imax/2 (# # # hier nicht schreiben,um überhöhte Werte /Wärmeentwicklung zu vermeiden # # #)
//		,{0x91,0x0000000A}	// W	11 TPOWERDOWN (~10µs)	// tmc5130a TPOWERDOWN
//		,{0x12,0x00000000}	// R	12 TSTEP Actual measured time between two 1/256 microsteps
//		,{0x93,0x000001F4}	// W	13 TPWMTHRS upper velocity 500
//		,{0x94,0x00000000}	// W	14 TCOOLTHRS
//		,{0x95,0x00000000}	// W	15 THIGH

// RAMP GENERATOR MOTION CONTROL REGISTER SET
//		,{0xA0,0x00000000}	// RW	20 RAMPMODE = 0 (ori) Positioning  mode  (using  all  A,D  and  V  parameters)
//		,{0xA0,0x00000001}	// RW	20 RAMPMODE = 1 Velocity  mode  to  positive  VMAX  (using AMAX acceleration)
//		,{0xA0,0x00000002}	// RW	20 RAMPMODE = 2 Velocity  mode  to  negative  VMAX  (using AMAX acceleration)
			, { 0xA0, 0x00000003 }// RW	20 RAMPMODE = 3 Hold  mode  (velocity  remains  unchanged,unless stop event occurs)

			, { 0xA1, 0x00000000 }// RW 	21 XACTUAL = 0 Actual motor position (signed),write 0 = auf Home Position setzen
//		,{0x22,0x00000000}	// R 	22 VACTUAL - Actual  motor  velocity  from  ramp  generator (signed)
			, { 0xA3, 0x000003E8 }// W 	23 VSTART = 1000 [µS/t] Motor start velocity (unsigned)
			, { 0xA4, 0x000003E8 }// W	24 A1 = 1 000 [µS/ta²] First acceleration between VSTART and V1 (unsigned)  (ori)
			, { 0xA5, 0x0000C350 }// W	25 V1 = 50 000 [µS/t] Acceleration threshold velocity V1 /D1 (ori)

			, { 0xA6, 0x000001F4 }// W	26 AMAX = 500 [µS/ta²] Second Acceleration above V1 (ori)
//		,{0xA6,0x000007D0}	// W	26 AMAX = 2000 [µS/ta²] Second Acceleration above V1

			, { 0xA7, 0x00000000 }	// W	27 VMAX = STOP
//		,{0xA7,0x0000047B}	// W	27 VMAX = 1138							 3,3 /s = 0,016  rps	 = 	  1  rpm @ 200 steppu  ### ACHTUNG Werte beinhalten KCLK für internen Taktgenerator!
//		,{0xA7,0x00030D40}	// W	27 VMAX = 200000 [µS/t] (ori)		   145,3 /s = 2,9  rps	 = 	174  rpm @ 200 steppu  ### Hier nicht (über)schreiben,Wert wurde bereits nach config gesetzt!
//		,{0xA7,0x000A7FCA}	// W	27 VMAX = 688074						 500 /s	 = 	10  rps	 = 	600  rpm @ 200 steppu
//		,{0xA7,0x0029ff28}	// W	27 VMAX = 2752296						2000 /s	 = 	40  rps = 2400  rpm @ 200 steppu
//		,{0xA7,0xFFFFFFFF}	// W	27 VMAX = 4294967295				 3121009 /s = 15605  rps = 936302  rpm @ 200 steppu

			, { 0xA8, 0x000002BC }// W	28 DMAX = 700 Deceleration above V1  (ori)
//		,{0xA8,0x00000AF0}	// W	28 DMAX = 2800 Deceleration above V1

			, { 0xAA, 0x00000578 }// W	2A D1 = 1400 Deceleration below V1  (ori)
//		,{0xAA,0x000015E0}	// W	2A D1 = 5600 Deceleration below V1

			, { 0xAB, 0x0000000A }// W	2B VSTOP = 10 Stop velocity (Near to zero)  (ori)
//		,{0xAC,0x00000000}	// W	2C TZEROWAIT
			, { 0xAD, 0x00000000 }	// RW	2D XTARGET = 0

// RAMP GENERATOR DRIVER FEATURE CONTROL REGISTER SET
//		,{0xB3,0x00000000}	// W	33 VDCMIN

//		,{0xB4,0x00000000}	// RW	34 SW_MODE (ori)
//		,{0xB4,0x00000800}	// RW	34 SW_MODE,en_softstop: Soft stop (Hard stop)
			, { 0xB4, 0x00000803 }	// RW	34 SW_MODE,
									// b11 en_softstop: Soft stop (Hard stop)
									// b3 pol_stop_r:  Sets the active polarity of the right reference switch input
									// b2 pol_stop_l:  Sets the active polarity of the left reference switch input
									// 				   0 =non-inverted, high active: a high level on REFL/R stops the motor
									// b1 stop_r_enable: Enables automatic motor stop during active left reference switch input
									// b0 stop_l_enable: Enables automatic motor stop during active left reference switch input

			, { 0x35, 0x00000000 }	// R+C	35 RAMP_STAT read & clear
//		,{0x36,0x00000000}	// R	36 XLATCH

// ENCODER REGISTER SET
//		,{0xB8,0x00000000}	// RW   38 ENCMODE
//		,{0xB9,0x00000000}	// RW   39 X_ENC
//		,{0xBA,0x00000000}	// W	3A ENC_CONST
			, { 0x3B, 0x00000000 }	// R+C	3B ENC_STATUS read & clear
//		,{0x3C,0x00000000}	// R	3C ENC_LATCH

// MICROSTEPPING CONTROL REGISTER SET
//		,{0xE0,0xAAAAB554}	// W 	60 MSLUT[0] Beispiel aus Beschreibung
//		,{0xE1,0x4A9554AA}	// W
//		,{0xE2,0x24492929}	// W
//		,{0xE3,0x10104222}	// W
//		,{0xE4,0xFBFFFFFF}	// W
//		,{0xE5,0xB5BB777D}	// W
//		,{0xE6,0x49295556}	// W
//		,{0xE7,0x00404222}	// W 	67 MSLUT[7]

//		,{0xE0,0x00000000}	// W 	60 reset MSLUT[0] with default sine wave table
//		,{0xE1,0x00000000}	// W
//		,{0xE2,0x00000000}	// W
//		,{0xE3,0x00000000}	// W
//		,{0xE4,0x00000000}	// W
//		,{0xE5,0x00000000}	// W
//		,{0xE6,0x00000000}	// W
//		,{0xE7,0x00000000}	// W 	67 reset MSLUT[7] with default sine wave table

//		,{0xE8,0x00000000}	// W	68 MSLUTSEL
//		,{0xE9,0x00000000}	// W	69 MSLUTSTART
//		,{0x6A,0x0000031f}	// R	6A MSCNT
//		,{0x6B,0x00f70000}	// R	6B MSCURACT (0x002f010d)

// DRIVER REGISTER SET
			, { 0xEC, 0x000100C5 }	// RW	6C CHOPCONF (ori)
//		,{0xED,0x00000000}	// W	6D COOLCONF
//		,{0xEE,0x00000000}	// W	6E DCCTRL
//		,{0x6F,0x810a00d3}	// R	6F DRV_STATUS (0xe00f0000)
//		,{0xF0,0x000401C8}	// W	70 PWMCONF pwm_autos cale,(04 grad,1b amplitude)	// tmc5130a PWMCONF
//		,{0x71,0x00000000}	// R	71 PWM_SCALE
//		,{0xF2,0x00000000}	// W	72 ENCM_CTRL
//		,{0x73,0x00000000}	// R	73 LOST_STEPS
	};

	for (devadr = 0; devadr < 3; devadr++) {
		// standard konfig einspielen
		tmc5130a_spi_send(devadr, cfg_powerup,
				sizeof(cfg_powerup) / sizeof(cfg_powerup[0]));// Grundwerte für alle Treiber /Motoren (HOLD_MODE included)

		// spezifische konfigurationen einspielen
		tmc5130a_CURR(devadr);
		tmc5130a_ACCELERATION(devadr);		// Motor spezifische Einstellungen
		tmc5130a_CHOPCONF(devadr);								// spread Cycle
		tmc5130a_COOLCONF(devadr);
		tmc5130a_VELOCITY_FAST(devadr);
//		tmc5130a_POSITIONING_MODE(devadr);
		// falls noch aktiv: alte fahrten beenden
		tmc5130a_CFG_HARD_STOP(devadr);		// Positionszähler bleibt erhalten!
		tmc5130a_PIN_STOP(devadr);
		tmc5130a_STEALTHCHOP_CONF(devadr);
//		tmc5130a_Homing(devadr);
//		tmc5130a_start_dest(devadr,0);
//		tmc5130a_wait_stop(devadr);
	}
}

/* ******************************************************************************
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			holddelay: Dauer hac Fahrt, nach der auf Haltestrom gewechselt wird
 *  			run: Fahrtstrom [mArms]
 *  			hold: Haltestrom [mArms]
 *  DESCRIPTION: setze maximalen Motorstrom nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CURR_MAN(uint8_t devadr,uint32_t holddelay, uint32_t run, uint32_t hold) {
	uint32_t Ishunt =	1352;	// [mArms] (Shunt == 150mR)
	uint32_t Srun =		11;		// ~25% (Stufen 0...31)
	uint32_t Shold =	5;
	switch(devadr){
	case 0: motor_1.run_current = run; motor_1.hold_current = hold; motor_1.hold_delay = holddelay; break;
	case 1: motor_2.run_current = run; motor_2.hold_current = hold; motor_2.hold_delay = holddelay; break;
	case 2: motor_3.run_current = run; motor_3.hold_current = hold; motor_3.hold_delay = holddelay; break;
	}
	// Motorströme, Shunts aus der jeweiligen Motor Konfig einlesen
	switch (devadr){
	case 0: Ishunt = Ishunt_M1; hold = hold*1000/1414; break;
	case 1: Ishunt = Ishunt_M2; hold = hold*1000/1414; break;
	case 2: Ishunt = Ishunt_M3; hold = hold*1000/1414; break;
	}

	// Stromstufe Lauf berechnen
	if (run >=  Ishunt){
		Srun = 31;										// wert für maximal strom (32/32 IRMS)
	}else if (run *32 < Ishunt){
		Srun = 0;										// wert für minimal strom (1/32 IRMS)
	}else{
		Srun = run *32 /Ishunt;
	}
	// Stromstufe Stand berechnen
	if (hold >=  Ishunt){
		Shold = 31;										// wert für maximal strom (32/32 IRMS)
	}else if (hold *32 < Ishunt){
		Shold = 0;										// wert für minimal strom (1/32 IRMS)
	}else{
		Shold = round(hold *32.0 /Ishunt);
	}
	//printf("M: %u, Run: %i, Hold: %i\r", devadr, run, hold);
	// Verzögerung, Laufstrom & Haltestrom in Motor Steuerschaltkreis einschreiben
	tdata = (holddelay<<16) | (Srun<<8) | Shold;
	tmc5130a_STEALTHCHOP_ACTIVATE(devadr, Shold==0?1:0);
	tmc5130a_spi_write_ureg(devadr,0x10,tdata);			// W 10 (IHOLDDELAY &) IHOLD_IRUN
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_HOLD_CURR_MAN
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			hold: Haltestrom [mArms]
 *  DESCRIPTION: setze maximalen Haltestrom
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_HOLD_CURR_MAN(uint8_t devadr, uint32_t hold) {
	uint32_t run, holddelay;
//	// Verzögerung, Laufstrom & Haltestrom in Motor Steuerschaltkreis einschreiben
	switch(devadr){
	case 0: run = motor_1.run_current; holddelay = motor_1.hold_delay; break;
	case 1: run = motor_2.run_current; holddelay = motor_2.hold_delay; break;
	case 2: run = motor_3.run_current; holddelay = motor_3.hold_delay; break;
	}
	tmc5130a_CURR_MAN(devadr, holddelay, run, hold);
//	tmc5130a_STEALTHCHOP_ACTIVATE(devadr, hold==0?1:0);
}
/* ******************************************************************************
 *  FUNCTION: tmc5130a_CURR
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setze maximalen Motorstrom nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CURR(uint8_t devadr) {
	// locale temporäre Variablen anlegen und mit default werten vorbelegen
	uint32_t Irun =		500;	// [mArms]
	uint32_t Ihold = 	200;	// [mArms]
	uint32_t Ishunt =	1352;	// [mArms] (Shunt == 150mR)
	uint32_t Srun =		11;		// ~25% (Stufen 0...31)
	uint32_t Shold =	5;

	// Motorströme, Shunts aus der jeweiligen Motor Konfig einlesen
		switch (devadr){
		case 0: Ishunt = Ishunt_M1; Irun = M1_CURR; Ihold = M1_HOLD*1000/1414; break;
		case 1: Ishunt = Ishunt_M2; Irun = M2_CURR; Ihold = M2_HOLD*1000/1414; break;
		case 2: Ishunt = Ishunt_M3; Irun = M3_CURR; Ihold = M3_HOLD*1000/1414; break;
		}
		switch(devadr){
		case 0: motor_1.run_current = Irun; motor_1.hold_current = M1_HOLD; motor_1.hold_delay = iholddelay; break;
		case 1: motor_2.run_current = Irun; motor_2.hold_current = M2_HOLD; motor_2.hold_delay = iholddelay; break;
		case 2: motor_3.run_current = Irun; motor_3.hold_current = M3_HOLD; motor_3.hold_delay = iholddelay; break;
		}
		// Stromstufe Lauf berechnen
		if (Irun >=  Ishunt){
			Srun = 31;										// wert für maximal strom (32/32 IRMS)
		}else if (Irun *32 < Ishunt){
			Srun = 0;										// wert für minimal strom (1/32 IRMS)
		}else{
			Srun = Irun *32 /Ishunt;
		}
		// Stromstufe Stand berechnen
		if (Ihold >=  Ishunt){
			Shold = 31;										// wert für maximal strom (32/32 IRMS)
		}else if (Ihold *32 < Ishunt){
			Shold = 0;										// wert für minimal strom (1/32 IRMS)
		}else{
			Shold = round(Ihold *32.0 /Ishunt);
		}
		// Verzögerung, Laufstrom & Haltestrom in Motor Steuerschaltkreis einschreiben
		tdata = (iholddelay<<16) | (Srun<<8) | Shold;
		tmc5130a_spi_write_ureg(devadr,0x10,tdata);			// W 10 (IHOLDDELAY &) IHOLD_IRUN
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_ACCELERATION
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			a Beschleunigungszwert in µSteps/s²
 *  			d Verzögerungswert in µSteps/s²
 *  DESCRIPTION: setzen der Beschleunigungen/Verzögerungen nach Parametern
 *  			Beschleunigung und Bremsen sind gleich
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_ACCELERATION_MAN(uint8_t devadr, uint32_t a, uint32_t d) {
	//Der Umrechnungsfaktor stammt aus Datenblatt S. 38
	// t_a² = 2^41/(fclk)² ;
	tmc5130a_spi_write_ureg(devadr, 0x24, a/116);
	tmc5130a_spi_write_ureg(devadr, 0x26, a/116);
	tmc5130a_spi_write_ureg(devadr, 0x28, d/116);
	tmc5130a_spi_write_ureg(devadr, 0x2A, d/116);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_SPEED_ACCELERATION_CURVE
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setzen der Beschleunigungen/Verzögerungen nach Parametern
 *  RETURNS: ACCELERATION OF A LINEAR SPEED-ACCELERATION-DIAGRAMM
 ******************************************************************************** */

float tmc5130a_SPEED_ACCELERATION_CURVE(float m, float t, float speed)
{
	return m*speed + t;
}
/* ******************************************************************************
 *  FUNCTION: tmc5130a_ACCELERATION
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setzen der Beschleunigungen/Verzögerungen nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_ACCELERATION(uint8_t devadr) {
	switch (devadr) {
	case 0:
		tmc5130a_spi_write_ureg(devadr, 0x23, M1_VSTART);
		tmc5130a_spi_write_ureg(devadr, 0x25, M1_V1);
		tmc5130a_spi_write_ureg(devadr, 0x2B, M1_VSTOP);
		tmc5130a_spi_write_ureg(devadr, 0x24, M1_A1);
		tmc5130a_spi_write_ureg(devadr, 0x26, M1_AMAX);
		tmc5130a_spi_write_ureg(devadr, 0x28, M1_DMAX);
		tmc5130a_spi_write_ureg(devadr, 0x2A, M1_D1);
		break;
	case 1:
		tmc5130a_spi_write_ureg(devadr, 0x23, M2_VSTART);
		tmc5130a_spi_write_ureg(devadr, 0x25, M2_V1);
		tmc5130a_spi_write_ureg(devadr, 0x2B, M2_VSTOP);
		tmc5130a_spi_write_ureg(devadr, 0x24, M2_A1);
		tmc5130a_spi_write_ureg(devadr, 0x26, M2_AMAX);
		tmc5130a_spi_write_ureg(devadr, 0x28, M2_DMAX);
		tmc5130a_spi_write_ureg(devadr, 0x2A, M2_D1);
		break;
	case 2:
		tmc5130a_spi_write_ureg(devadr, 0x23, M3_VSTART);
		tmc5130a_spi_write_ureg(devadr, 0x25, M3_V1);
		tmc5130a_spi_write_ureg(devadr, 0x2B, M3_VSTOP);
		tmc5130a_spi_write_ureg(devadr, 0x24, M3_A1);
		tmc5130a_spi_write_ureg(devadr, 0x26, M3_AMAX);
		tmc5130a_spi_write_ureg(devadr, 0x28, M3_DMAX);
		tmc5130a_spi_write_ureg(devadr, 0x2A, M3_D1);
		break;
	case 3:
	default:
		tmc5130a_spi_write_ureg(devadr, 0x23, 1000);
		tmc5130a_spi_write_ureg(devadr, 0x25, 50000);
		tmc5130a_spi_write_ureg(devadr, 0x2B, 10);
		tmc5130a_spi_write_ureg(devadr, 0x24, 1000);
		tmc5130a_spi_write_ureg(devadr, 0x26, 500);
		tmc5130a_spi_write_ureg(devadr, 0x28, 700);
		tmc5130a_spi_write_ureg(devadr, 0x2A, 1400);
		break;
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CHOPCONF
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setzen der Standard-Chopper-Konfiguration nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CHOPCONF(uint8_t devadr) {
	uint8_t spread_m1[5] = {M1_TOFF,M1_HSTRT,M1_HEND,M1_RNDTF,M1_TBL};
	uint8_t spread_m2[5] = {M2_TOFF,M2_HSTRT,M2_HEND,M2_RNDTF,M2_TBL};
	uint8_t spread_m3[5] = {M3_TOFF,M3_HSTRT,M3_HEND,M3_RNDTF,M3_TBL};

	switch(devadr){
	case 0:
		for (int i=0;i<5;i++) spread_cfg[i] = spread_m1[i];
		break;
	case 1:
		for (int i=0;i<5;i++) spread_cfg[i] = spread_m2[i];
		break;
	case 2:
		for (int i=0;i<5;i++) spread_cfg[i] = spread_m3[i];
		break;
	}
	tmc5130a_CHOPCONF_PAR(devadr);
}

//CHOP-Konfiguartion für Unterschiedliche Geschwindigkeiten 0...100% in 20% schritten
//m1: Motor1, m2: Motor2, m3: Motor3
uint8_t spread_m1_02[5] = { 2, 7, 1, 0, 1 };// # # # hier config werte eintragen # # # (TOFF, HSTRT, HEND, RNDTF, TBL)
uint8_t spread_m1_04[5] = { 2, 4, 1, 0, 1 };
uint8_t spread_m1_06[5] = { 2, 7, 1, 0, 1 };
uint8_t spread_m1_08[5] = { 2, 3, 1, 0, 1 };
uint8_t spread_m1_10[5] = { 2, 3, 1, 0, 1 };

uint8_t spread_m2_02[5] = { 2, 7, 4, 0, 1 };
uint8_t spread_m2_04[5] = { 2, 7, 4, 0, 1 };
uint8_t spread_m2_06[5] = { 2, 7, 4, 0, 1 };
uint8_t spread_m2_08[5] = { 2, 1, 7, 0, 1 };
uint8_t spread_m2_10[5] = { 2, 2, 1, 0, 1 };

uint8_t spread_m3_02[5] = { 2, 7, 4, 0, 1};
uint8_t spread_m3_04[5] = { 2, 7, 4, 0, 1};
uint8_t spread_m3_06[5] = { 2, 7, 4, 0, 1};
uint8_t spread_m3_08[5] = { 2, 2, 1, 0, 1};
uint8_t spread_m3_10[5] = { 2, 2, 1, 0, 1};

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CHOPCONF_speed
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			speed: Geschwindikeit des neuen Motors
 *  DESCRIPTION: setze Chopper-Konfiguartion nach oben stehender Tabelle
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CHOPCONF_speed(uint8_t devadr, uint32_t speed) {
	speed = speed * 100 / tmc5130a_VELOCITY_FULL(devadr);// speed [%] = aufgerufene Geschwindigkeit / maximale Geschwindigkeit
	//Standardeinstellungen
	uint32_t tcoolthrs =	0;

	uint8_t sfilt =			0;	// 0 0...1		stallGuard2 filter enable
	int8_t sgt =			3;	// 0 -64...63	stallGuard2 threshold value
	uint8_t seimin = 		1;	// 0 0...1		minimum current for smart current control
								// 0: 1/2 of current setting (IRUN)
								// 1: 1/4 of current setting (IRUN)
	uint8_t sedn =			1;	// 0 0...3		current down step speed
	uint8_t semax =			1;	// 0 0...15		stallGuard2 hysteresis value for smart current control
	uint8_t seup =			1;	// 0 0...3		current up step width
	uint8_t semin =			1;  // 0 0...15		minimum stallGuard2 value for smart current control and smart current enable
								// 0: no coolstep
								// 1: max coolstep (motor load angle)
	uint32_t hold, run;
	switch (devadr) {// setze spread config je nach % der maximalen Geschwindigkeit
	case 0:				// M1 Pumpe
		tcoolthrs = M1_VCOOL; //Standardeinstellungen
		hold = motor_1.hold_current;
		run = M1_CURR;
		if (speed < 20) {										// %
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m1_02[i];
			sgt = 0;
			run = M1_CURR*0.5; //bei langsamen Geschwindigkeiten Spitzenstrom runternehmen
		} else if (speed < 40) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m1_04[i];
			sgt = -2;
			run = M1_CURR*0.5; //bei langsamen Geschwindigkeiten Spitzenstrom runternehmen
		} else if (speed < 60) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m1_06[i];
		} else if (speed < 80) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m1_08[i];
		} else {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m1_10[i];
		}
		tmc5130a_CHOPCONF_PAR(devadr);
		break;
	case 1:														// M2 Ausheber
		tcoolthrs = M2_VCOOL; //Standardeinstellungen
		hold = motor_2.hold_current;
		run = M2_CURR;
		sgt = 1;
		if (speed < 20) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m2_02[i];
			run = M2_CURR*0.5; //bei langsamen Geschwindigkeiten Spitzenstrom runternehmen
			sgt = 0;
		} else if (speed < 40) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m2_04[i];
			run = M2_CURR*0.5; //bei langsamen Geschwindigkeiten Spitzenstrom runternehmen
			sgt = -2;
		} else if (speed < 60) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m2_06[i];
		} else if (speed < 80) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m2_08[i];
		} else {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m2_10[i];
		}
		tmc5130a_CHOPCONF_PAR(devadr);
		break;
	case 2:												// M3 Tisch /Drehtisch
		tcoolthrs = M3_VCOOL; //Standardeinstellungen
		hold = motor_3.hold_current;
		run = M3_CURR;
		if (speed < 20) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m3_02[i];
			sgt = 0;
			run = M3_CURR*0.75; //bei langsamen Geschwindigkeiten Spitzenstrom runternehmen
		} else if (speed < 40) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m3_04[i];
		} else if (speed < 60) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m3_06[i];
		} else if (speed < 80) {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m3_08[i];
		} else {
			for (int i = 0; i < 5; i++)
				spread_cfg[i] = spread_m3_10[i];
		}
		tmc5130a_CHOPCONF_PAR(devadr);
		break;
	}
	//printf("M: %u, ON: %i, SGT: %i, SPEED: %i", devadr, seimin, sgt, speed);
	tmc5130a_COOLCONF_MAN(devadr, tcoolthrs, sfilt, sgt, seimin, sedn, semax, seup, semin);
	tmc5130a_CURR_MAN(devadr,iholddelay, run, hold);
}
// -------------------------------

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CHOPCONF_MAN
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Schreiben der Chopper-Konfiguartion nach eingestellten Werten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CHOPCONF_MAN(uint8_t devadr, uint8_t* spread, uint32_t speed)
{
	uint8_t* spread_temp;
	switch(devadr)
	{
		case 0:
			if (speed < 20) {
				spread_temp = spread_m1_02;
			} else if (speed < 40) {
				spread_temp = spread_m1_04;
			} else if (speed < 60) {
				spread_temp = spread_m1_06;
			} else if (speed < 80) {
				spread_temp = spread_m1_08;
			} else {
				spread_temp = spread_m1_10;
			}
			break;
		case 1:
			if (speed < 20) {
				spread_temp = spread_m2_02;
			} else if (speed < 40) {
				spread_temp = spread_m2_04;
			} else if (speed < 60) {
				spread_temp = spread_m2_06;
			} else if (speed < 80) {
				spread_temp = spread_m2_08;
			} else {
				spread_temp = spread_m2_10;
			}
			break;
		case 2:
			if (speed < 20) {
				spread_temp = spread_m3_02;
			} else if (speed < 40) {
				spread_temp = spread_m3_04;
			} else if (speed < 60) {
				spread_temp = spread_m3_06;
			} else if (speed < 80) {
				spread_temp = spread_m3_08;
			} else {
				spread_temp = spread_m3_10;
			}
			break;
		for (int i = 0; i < 5; i++)
			*(spread_temp+i) = *(spread+i);
	}
}
/* ******************************************************************************
 *  FUNCTION: tmc5130a_CHOPCONF_PAR
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Schreiben der Chopper-Konfiguartion nach eingestellten Werten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CHOPCONF_PAR(uint8_t devadr) {
	uint8_t TOFF = spread_cfg[0];
	uint8_t HSTRT = spread_cfg[1];
	uint8_t HEND = spread_cfg[2];
	uint8_t RNDTF = spread_cfg[3];
	uint8_t TBL = spread_cfg[4];

	//printf("M %u CHOP: %i, %i, %i, %i, %i\r", devadr, TOFF, HSTRT, HEND, RNDTF, TBL);

	if (TOFF > 15)
		TOFF = 15;	// [%0010 … %1111] off time and driver enable
	// Off time setting controls duration of slow decay phase
	// N CLK = 12 + 32*TOFF
	// %0000: Driver disable,all bridges off
	// %0001: 1 – use only with TBL ≥ 2
	// %0010 … %1111: 2 … 15

	if (HSTRT > 7)
		HSTRT = 7;	// [%000 … %111] hysteresis start value added to HEND
	// Add 1,2,…,8 to hysteresis low value HEND
	// (1/512 of this setting adds to current setting)
	// Attention: Effective HEND+HSTRT ≤ 16.
	// Hint:  Hysteresis  decrement  is  done  each  16 clocks

	if (HEND > 15)
		HEND = 15;	// [%0000 … %1111] hysteresis low value OFFSET
	// Hysteresis is -3,-2,-1,0,1,…,12
	// (1/512 of this setting adds to current setting)
	// This is the hysteresis value which becomes used for the hysteresis chopper.

	uint8_t FD3 = 0;			// [0/1] not used @ cbm = 0 (spread Cycle)

	uint8_t DISFDCC = 0; 		// [0/1] not used @ cbm = 0 (spread Cycle)

	if (RNDTF > 1)
		RNDTF = 1;	// [0/1] random TOFF time
	// 0 Chopper off time is fixed as set by TOFF
	// 1  Random  mode,TOFF is  random  modulated  by  dN_CLK = -12 … +3 clocks.

	uint8_t CHM = 0;			// [0/1] chopper mode
	// 0 Standard mode (spreadCycle)
	// 1 Constant off time with fast decay time.
	//   Fast decay time is also terminated when the negative nominal current is reached.
	//   Fast decay is after on time.

	if (TBL < 3)
		TBL = 3;		// [%00 … %11] blank time select
	// Set comparator blank time to 16,24,36 or 54 clocks
	// Hint: %01 or %10 is recommended for most applications

	uint8_t VSENSE = 0;	// [0/1] sense resistor voltage based current scaling
	// 0: Low sensitivity,high sense resistor voltage
	// 1: High sensitivity,low sense resistor voltage

	uint8_t VHIGHFS = 0;		// [0/1] high velocity fullstep selection
	// This  bit  enables  switching  to  fullstep,when  VHIGH  is exceeded.  Switching  takes  place  only  at  45°  position.
	// The  fullstep  target  current  uses  the  current  value  from the microstep table at the 45° position.

	uint8_t VHIGHCHM = 0;// [0/1] high velocity chopper mode [not used @ cbm = 0 (spread Cycle)]
	// This bit enables switching to chm = 1 and fd = 0,when VHIGH is  exceeded.
	// This way,a higher velocity can be achieved. Can be combined with vhighfs = 1.
	// If set,the TOFF setting automatically  becomes doubled during high velocity operation in order to avoid doubling of the chopper	frequency.

	uint8_t SYNC = 0;			// [%0000 … %1111] PWM synchronization clock
	// This register allows synchronization of the chopper for both phases of a two phase motor in order to avoid the occurrence of a beat,especially at low motor velocities.
	// It is automatically switched off above VHIGH.
	// %0000: Chopper sync function chopSync off
	// %0001 … %1111: Synchronization with f SYNC = f CLK /(sync*64)
	// Hint: Set TOFF to a low value,so that the chopper cycle is ended,before the next sync clock pulse occurs.
	// Set for the double desired chopper frequency for chm = 0,for  the desired base chopper frequency for chm = 1.

	uint8_t MRES = 0;			// [%0000 … %1111] micro step resolution
	// %0000: Native 256 microstep setting. Normally use this setting with the internal motion controller.
	// %0001 … %1000:  128,64,32,16,8,4,2,FULLSTEP
	// Reduced microstep resolution esp. for Step/Dir operation.
	// The  resolution  gives  the  number  of  microstep  entries  per sine quarter wave.
	// The  driver  automatically  uses  microstep  positions  which result  in  a  symmetrical  wave,when  choosing  a  lower microstep resolution.
	// step width = 2^MRES [microsteps]

	uint8_t INTPOL = 0;			// [0/1] interpolation to 256 microsteps
	// 1: The actual microstep resolution (MRES) becomes extrapolated to 256 microsteps for smoothest motor operation (useful for Step/Dir operation,only)

	uint8_t DEDGE = 0;			// [0/1] enable double edge step pulses
	// 1: Enable step impulse at each step edge to reduce step frequency requirement.

	uint8_t DISS2G = 0;			// [0/1] short to GND protection disable
	// 0: Short to GND protection is on
	// 1: Short to GND protection is disabled

	tdata = TOFF | (HSTRT << 4) | (HEND << 7) | (FD3 << 11) | (DISFDCC << 12)
			| (RNDTF << 13) | (CHM << 14) | (TBL << 15) | (VSENSE << 17)
			| (VHIGHFS << 18) | (VHIGHCHM << 19) | (SYNC << 20) | (MRES << 24)
			| (INTPOL << 28) | (DEDGE << 29) | (DISS2G);
	tmc5130a_spi_write_ureg(devadr, 0xEC, tdata);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_COOLCONF_MAN
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			tcoolthrs: Zeitintervall zwischen Steps ab dem Coolstep aktiviert werden kann
 *  			sfilt: Filter an/aus (Abfrage von Schlupf nur jeden Vollschritt)
 *  			sgt: Sensitivität
 *  			seimin: Mindeststrom für Coolstep-Aktivierung
 *  			sedn: Nachregelungsgeschwindigkeit
 *  			semax: maximalstrom bei dem nachgeregelt wird
 *  			semin: minimalstrom, bei dem nachgeregelt wird
 *  DESCRIPTION: setze von CoolStep nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_COOLCONF_MAN(uint8_t devadr, uint32_t tcoolthrs, uint8_t sfilt, int8_t sgt, uint8_t seimin, uint8_t sedn, uint8_t semax,
		uint8_t seup, uint8_t semin){
	if (sfilt > 1)	sfilt = 1;
	if (sgt < -64)	sgt = -64;
	if (sgt > 63)	sgt = 63;
	if (seimin > 1)	seimin = 1;
	if (sedn > 3)	sedn = 3;
	if (semax > 15)	semax = 15;
	if (seup > 3)	seup = 3;
	if (semin > 15)	semin = 15;
	tdata = semin | (seup<<5) | (semax<<8) | (sedn<<13) | (seimin<<15) | (sgt<<16) | (sfilt<<24);
	tmc5130a_spi_write_sreg(devadr,0xED,tdata);
	//printf("M: %u COOL: %i, %i, %i, %i, %i, %i\r", devadr, sfilt, sgt, seimin, sedn, semax, seup);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_COOLCONF
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setze von CoolStep nach config (tmc5130.h)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_COOLCONF(uint8_t devadr){								// cool config, TCOOLTHRS (reg 0x14 20bit) in tmc einschreiben
	uint32_t TCOOLTHRS =	0;

	uint8_t sfilt =			0;	// 0 0...1		stallGuard2 filter enable
	int8_t sgt =			1;	// 0 -64...63	stallGuard2 threshold value
	uint8_t seimin = 		1;	// 0 0...1		minimum current for smart current control
								// 0: 1/2 of current setting (IRUN)
								// 1: 1/4 of current setting (IRUN)
	uint8_t sedn =			1;	// 0 0...3		current down step speed
	uint8_t semax =			1;	// 0 0...15		stallGuard2 hysteresis value for smart current control
	uint8_t seup =			1;	// 0 0...3		current up step width
	uint8_t semin =			1;  // 0 0...15		minimum stallGuard2 value for smart current control and smart current enable
								// 0: no coolstep
								// 1: max coolstep (motor load angle)

	switch (devadr){									// Schaltpunkt
	case 0:
		TCOOLTHRS = M1_VCOOL; break;
	case 1:
		TCOOLTHRS = M2_VCOOL; break;
	case 2:
		TCOOLTHRS = M3_VCOOL; break;
	}
	switch (devadr){
	case 0:
	case 1:
	case 2:
		if (TCOOLTHRS > 0xfffff) TCOOLTHRS = 0xfffff;	// 20bit = 1048575
		tmc5130a_spi_write_ureg(devadr,0x94,TCOOLTHRS);
		break;
	}
	switch (devadr){
	case 0:
		semin = M1_CS_min;
		seimin = M1_CS_Imin;
		break;
	case 1:
		semin = M2_CS_min;
		seimin = M2_CS_Imin;
		break;

	case 2:
		semin = M3_CS_min;
		seimin = M3_CS_Imin;
		break;
	}
	switch (devadr){
	case 0:
	case 1:
	case 2:
		if (sfilt > 1)	sfilt = 1;
		if (sgt < -64)	sgt = -64;
		if (sgt > 63)	sgt = 63;
		if (seimin > 1)	seimin = 1;
		if (sedn > 3)	sedn = 3;
		if (semax > 15)	semax = 15;
		if (seup > 3)	seup = 3;
		if (semin > 15)	semin = 15;
		tdata = semin | (seup<<5) | (semax<<8) | (sedn<<13) | (seimin<<15) | (sgt<<16) | (sfilt<<24);
		tmc5130a_spi_write_sreg(devadr,0xED,tdata);
		break;
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_STEALTHCHOP_CONF
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: STEAHLTHCHOP Modus konfigurieren; für Stillstand und langsame Geschwindigkeiten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_STEALTHCHOP_CONF(uint32_t devadr)
{
	uint32_t data = 0x00;
	uint8_t freewheel = 0x01;		//freewheeling if Hold current = 0;
	uint8_t pwm_symmetric = 0x00; //pwm value may change within each PWM cycle
	uint8_t pwm_autoscale = 0x01; 	//automatic current control
	uint8_t pwm_freq = 0x00; 		//f=2/1024 * f_clk
	uint8_t pwm_grad = 1; 			//gradient
	uint8_t pwm_ampl = 255; 			//offset
	data |= pwm_ampl | (pwm_grad <<8) | (pwm_freq << 16) | (pwm_autoscale << 18) | (pwm_symmetric << 19) | (freewheel << 20);
	tmc5130a_spi_write_ureg(devadr, 0x70, data);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_STEALTHCHOP_CONF
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: STEAHLTHCHOP Modus konfigurieren; für Stillstand und langsame Geschwindigkeiten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_STEALTHCHOP_CONF_MAN(uint32_t devadr, uint8_t freewheel, uint8_t pwm_symmetric, uint8_t pwm_autoscale,
									uint8_t pwm_freq, uint8_t pwm_grad, uint8_t pwm_ampl)
{
	uint32_t data = 0x00;
	data |= pwm_ampl | (pwm_grad <<8) | (pwm_freq << 16) | (pwm_autoscale << 18) | (pwm_symmetric << 19) | (freewheel << 20);
	tmc5130a_spi_write_ureg(devadr, 0x70, data);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_STEALTHCHOP_ACTIVATE
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: STEAHLTHCHOP Modus konfigurieren; für Stillstand und langsame Geschwindigkeiten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_STEALTHCHOP_ACTIVATE(uint32_t devadr, uint8_t enable)
{
	uint32_t data;
	tmc5130a_spi_read_reg(devadr, 0x00);
	data = urdata2;
	if(enable)
	{
		data |= 0x00000004;
		//printf("Stealth: %u\r", devadr);
	}
	else
	{
		data &= ~(0x00000004);
		//printf("Chop: %u\r", devadr);
	}
	tmc5130a_spi_write_ureg(devadr, 0x00, data);

}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CFG_SOFT_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Beim unerwartetem Bremsen Motor sanft auslaufen lassen
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CFG_SOFT_STOP(uint8_t devadr) {
	if (devadr < 3) {
		tmc5130a_spi_read_reg(devadr, 0x34); 		// read register;
		srdata2 |= 0x00000803; // enable "refl/refr" [b1/b0] as "Stop" and set "Soft stop" [b11]
		srdata2 &= ~0x0000000C;	// reset "refl/refr" [b3,b4] as "high active"
		tmc5130a_spi_write_ureg(devadr, 0x34, srdata2);
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CFG_HARD_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Beim unerwartetem Bremsen Motor sofort anhalten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_CFG_HARD_STOP(uint8_t devadr) {
	if (devadr < 3) {
		tmc5130a_spi_read_reg(devadr, 0x34);
		srdata2 |= 0x00000003;			// enable "refl/refr" [b1/b0] as "Stop"
		srdata2 &= ~0x0000080C;	// reset "refl/refr" [b3,b4] as "high active" and reset "Soft stop" [b11] (-> "hard stop")
		tmc5130a_spi_write_ureg(devadr, 0x34, srdata2);
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_PIN_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Fahrt sofort unterbinden
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_PIN_STOP(uint8_t devadr) {
	switch (devadr) {								// Motor stop
	case 0:
		Refl_Step1_H;
		Refr_Dir1_H;
		break;
	case 1:
		Refl_Step2_H;
		Refr_Dir2_H;
		break;
	case 2:
		Refl_Step3_H;
		Refr_Dir3_H;
		break;
	default:
		break;
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_CFG_SOFT_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Motorfahrt wieder freigeben
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_PIN_RUN(uint8_t devadr) {
	switch (devadr) {
	case 0:
		Refl_Step1_L;
		Refr_Dir1_L;
		break;
	case 1:
		Refl_Step2_L;
		Refr_Dir2_L;
		break;
	case 2:
		Refl_Step3_L;
		Refr_Dir3_L;
		break;
	default:
		break;
	}
}


/* ******************************************************************************
 *  FUNCTION: tmc5130a_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Motorfahrt anhalten und Fahrt abbrechen (fährt nach Freigabe
 *  			nicht wieder weiter)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_STOP(uint8_t devadr) {// ausführen, Positionszähler bleibt erhalten!
	tmc5130a_PIN_STOP(devadr);

	if (devadr < 3) {
		WDO();									// wdg schmeisst nach 4s raus
		// Damit nach Freigabe keine Fortsetzung des voran gegangenen (Ziel- oder Geschwindigkeits-) Fahrens erfolgt:
		tmc5130a_POSITIONING_MODE(devadr);			// Zielmode
//		tmc5130a_VELOCITY_FAST(devadr);
		tmc5130a_wait_stop(devadr);
		int32_t pos = tmc5130a_read_position(devadr);
		tmc5130a_start_dest(devadr, pos);			// Ziel = aktuelle Position
	}

//	tmc5130a_PIN_RUN(devadr);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_SOFT_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Beim unerwartetem Bremsen Motor sanft auslaufen lassen
 *  			Konfiguarieren und ausführen
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_SOFT_STOP(uint8_t devadr) {		// konfigurieren und ausführen
	if (devadr < 3) {
		tmc5130a_CFG_SOFT_STOP(devadr);
		tmc5130a_STOP(devadr);
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_HARD_STOP
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Beim unerwartetem Bremsen Motor sofort anhalten
 *  			Konfiguarieren und ausführen
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_HART_STOP(uint8_t devadr) {		// konfigurieren und ausführen
	if (devadr < 3) {
		tmc5130a_CFG_HARD_STOP(devadr);
		tmc5130a_STOP(devadr);
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_ramp_stat
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Lesen des Statusregisters RAMP_STAT (s. Datenblatt)
 *  RETURNS: none
 ******************************************************************************** */
int32_t tmc5130a_read_ramp_stat(uint8_t devadr){	// Abfrage dieses Statusregisters (return urdata2)
	tmc5130a_spi_read_reg(devadr,0x35);
	return urdata2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_vzero
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Lesen des Register für Nullgeschwindigkeit
 *  RETURNS: none
 ******************************************************************************** */
int32_t tmc5130a_read_vzero(uint8_t devadr){		// Abfrage vzero bit; Signals, that the actual velocity is 0.
	tmc5130a_read_ramp_stat(devadr);
	return (urdata2 & 0x00000400);					// bit10  vzero  1: Signals, that the actual velocity is 0.
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_ot
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Überprüfung, ob Overtemperature (~150°C) erreicht wurde
 *  RETURNS: OT-Flag
 ******************************************************************************** */
uint32_t tmc5130a_read_ot(uint8_t devadr){			// Abfrage reg "drv_status" bit 26 "ot" (typ 150°C)
													// H: Overtemperature  limit  has  been  reached.  Drivers  become disabled until "otpw" is also cleared...
	tmc5130a_spi_read_reg(devadr,0x6f);				// reg "drv_status"
	return (urdata2 & 0x02000000);					// bit 25 "ot" overtemperature flag
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_otpw
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Überprüfung ob Temperatur-Vorwarn-Flag erreicht wurde
 *  RETURNS: OTPW-Flag
 ******************************************************************************** */
uint32_t tmc5130a_read_otpw(uint8_t devadr){		// Abfrage otpw bit; 1: Overtemperature pre-warning threshold (typ 120°C) is exceeded.
	tmc5130a_spi_read_reg(devadr,0x6f);				// reg "drv_status"
	return (urdata2 & 0x04000000);					// bit 26 "otpw" overtemperature pre-warning flag
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_wait_stop
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Solange warten bis Motor steht (Mit Sicherheitsfaktor)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_wait_stop(uint8_t devadr){			// warte auf Stillstand des Motors
	uint8_t n = 0;
	while (1){
		if (tmc5130a_read_vzero(devadr)){
			n++;
			if (n >= 3) break;
		}else{
			n = 0;
		}
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_wait_stop_all_motors
 *  PARAMETERS: none
 *  DESCRIPTION: Solange warten bis alle Motoren stehen (M1-M3)
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_wait_stop_all_motors(void) {
	for (devadr = 0; devadr < 3; devadr++) {
		tmc5130a_wait_stop(devadr);
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			svelocity: Geschwindigkeit in [µSteps/s]
 *  DESCRIPTION: Schreiben der Geschwindigkeit
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY(uint8_t devadr, int32_t svelocity) {
	//Korrekturfaktor -> Datenblatt S.38
	//t = 2^24/f_clk ;
	svelocity = svelocity * 1048/1000;
	tmc5130a_CHOPCONF_speed(devadr, svelocity);		// spread configs
	tmc5130a_spi_write_ureg(devadr, 0xA7, svelocity);	// W	27 VMAX
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_FULL
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: setze maximale Motorgeschwindigkeit nach config (tmc5130.h)
 *  RETURNS: int32_t: Maximalgeschwindigeit
 ******************************************************************************** */
int32_t tmc5130a_VELOCITY_FULL(uint8_t devadr) {
	tdata = SpU * uSpS / 10;		// default: 6  rpm
	switch (devadr) {
	case 0:
#ifdef	M1_CYCLE											// [us] Cycledauer	(1/Freq)		-> µStep/s
		tdata = 1000000	/M1_CYCLE	*4	*uSpS;
#else
#ifdef  M1_FREQ												// [Hz] Frequenz 					-> µStep/s
		tdata = 			M1_FREQ		*4	*uSpS;
#else
#ifdef	M1_VMAX												// [Step/s] 		(4*Freq)		-> µStep/s
		tdata = 			M1_VMAX 		*uSpS;
#else
#ifdef	M1_RPM												// [ rpm] 			(   /60	4*Freq/SpU)	-> µStep/s
		tdata = M1_SpU * M1_RPM * uSpS / 60;
#endif
#endif
#endif
#endif
		break;
	case 1:
#ifdef	M2_CYCLE
		tdata = 1000000	/M2_CYCLE	*4	*uSpS;
#else
#ifdef  M2_FREQ
		tdata = 			M2_FREQ		*4	*uSpS;
#else
#ifdef	M2_VMAX
		tdata = 			M2_VMAX 		*uSpS;
#else
#ifdef	M2_RPM
		tdata = M2_SpU * M2_RPM * uSpS / 60;
#endif
#endif
#endif
#endif
		break;
	case 2:
#ifdef	M3_CYCLE
		tdata = 1000000	/M3_CYCLE	*4	*uSpS;
#else
#ifdef  M3_FREQ
		tdata = 			M3_FREQ		*4	*uSpS;
#else
#ifdef	M3_VMAX
		tdata = 			M3_VMAX 		*uSpS;
#else
#ifdef	M3_RPM
		tdata = M3_SpU * M3_RPM * uSpS / 60;
#endif
#endif
#endif
#endif
		break;
	case 3:
	default:
		tdata = SpU * uSpS / 10;	// default: 6  rpm
		break;
	}
	tdata = tdata * KCLK / 100;	// Korrekturfaktor für den internen Taktgenerator
	return tdata;
}


/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_FAST
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Schreiben der schnellen Geschwindigkeit nach tmc5130.h
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY_FAST(uint8_t devadr){
	tdata = tmc5130a_VELOCITY_FULL(devadr);
	switch (devadr){
	case 0: tdata = tdata *M1_FAST/100; tmc5130a_VELOCITY(devadr,tdata); break;
	case 1: tdata = tdata *M2_FAST/100; tmc5130a_VELOCITY(devadr,tdata); break;
	case 2: tdata = tdata *M3_FAST/100; tmc5130a_VELOCITY(devadr,tdata); break;
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_SLOW
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Schreiben der langsamen Geschwindigkeit nach tmc5130.h
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY_SLOW(uint8_t devadr){
	tdata = tmc5130a_VELOCITY_FULL(devadr);
	switch (devadr){
	case 0: tdata = tdata *M1_SLOW/100; tmc5130a_VELOCITY(devadr,tdata); break;
	case 1: tdata = tdata *M2_SLOW/100; tmc5130a_VELOCITY(devadr,tdata); break;
	case 2: tdata = tdata *M3_SLOW/100; tmc5130a_VELOCITY(devadr,tdata); break;
	}
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_UPS__TBD__
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			rpm: Geschwindigeit in RPM
 *  DESCRIPTION: Schreiben der Geschwindigkeit
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY_UPS__TBD__(uint8_t devadr, float rpm) {
	switch (devadr) {
	case 0:										// [ rpm] 			( -> µStep/s
		tdata = (uint32_t) (rpm * M1_SpU * uSpS / 60);
		break;
	case 1:
		tdata = (uint32_t) (rpm * M2_SpU * uSpS / 60);
		break;
	case 2:
		tdata = (uint32_t) (rpm * M3_SpU * uSpS / 60);
		break;
	case 3:
	default:
		tdata = SpU * uSpS / 60;			// default: 1  rpm
		break;
	}
	tdata = tdata * KCLK / 100;	// Korrekturfaktor für den internen Taktgenerator
	tmc5130a_VELOCITY(devadr, tdata);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_POSITIONING_MODE
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: MODUS: Motor fährt nach Zielvorgabe
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_POSITIONING_MODE(uint8_t devadr) {
	tmc5130a_spi_write_ureg(devadr, 0x20, 0x00000000);// RW	20 RAMPMODE 0 = Positioning mode (using all A,D and V parameters)
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_MODE_POS
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: MODUS: Fahrt mit einer Geschwindigkeit in positive Richtung
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY_MODE_POS(uint8_t devadr) {
	tmc5130a_spi_write_ureg(devadr, 0x20, 0x00000001);// RW	20 RAMPMODE = 1 Velocity  mode  to  positive  VMAX  (using AMAX acceleration)
}


/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_MODE_NEG
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: MODUS: Fahrt mit einer Geschwindigkeit in negative Richtung
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_VELOCITY_MODE_NEG(uint8_t devadr) {
	tmc5130a_spi_write_ureg(devadr, 0x20, 0x00000002);// RW	20 RAMPMODE = 2 Velocity  mode  to  negative  VMAX  (using AMAX acceleration)
}


/* ******************************************************************************
 *  FUNCTION: tmc5130a_HOLD_MODE
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: MODUS: Fahrt mit einer Geschwindigkeit bis Stoppevent
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_HOLD_MODE(uint8_t devadr) {
	tmc5130a_spi_write_ureg(devadr, 0x20, 0x00000003);// RW	20 RAMPMODE = 3 Hold  mode  (velocity  remains  unchanged,unless stop event occurs)
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_VELOCITY_MODE_POS
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Aktuelle Position wird zur neuen Nullposition gesetzt
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_HOMING(uint8_t devadr) {
	tmc5130a_spi_write_sreg(devadr, 0x21, 0);// RW 	21 XACTUAL = 0 Actual motor position (signed),write 0 = auf Home Position setzen
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_velocity
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Aktuelle Geschwindigkeit auslesen
 *  RETURNS: int32_t Geschwindigkeit in µSteps/s
 ******************************************************************************** */
int32_t tmc5130a_read_velocity(uint8_t devadr) {
	tmc5130a_spi_read_reg(devadr, 0x22);
	if (srdata2 > 0x7fffff)
		srdata2 = 0xffffff - srdata2; // 24bit negativ -> Absolutwert
	return srdata2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_start_velocity
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  			svelocity: Geschwindkeit für Fahrt
 *  DESCRIPTION: Geschwindigekti schreiben und mit dieser losfahren
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_start_velocity(uint8_t devadr, int32_t svelocity) {
	if (svelocity < 0) {
		tmc5130a_VELOCITY_MODE_NEG(devadr);
		svelocity = 0 - svelocity;
	} else {
		tmc5130a_VELOCITY_MODE_POS(devadr);
	}
	tmc5130a_VELOCITY(devadr, svelocity);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_stop_velocity
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Geschwindigkeit auf Null setzen
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_stop_velocity(uint8_t devadr) {
	tmc5130a_VELOCITY(devadr, 0);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_ist stop
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Abfragen, ob Motor steht
 *  RETURNS: bool false -> Motor läuft
 *  			  true -> Motor steht
 ******************************************************************************** */
bool tmc5130a_is_stop(uint8_t devadr) {
	int8_t counter = 0;
	for (int i = 0; i < 3; i++) {
		tmc5130a_read_ramp_stat(devadr);
		if (urdata2 & 0x00000400) {	// 10  vzero  1: Signals, that the actual velocity is 0.
			counter++;
		} else {
			counter = 0;
		}
	}
	//check if over tempature is reached
	if(tmc5130a_read_ot(devadr) != 0 || tmc5130a_read_otpw(devadr) != 0)
	{
		//printf("Overheat: %u\r", devadr);
		ErrorDisplay(IERR_OVERHEAT);
	}

	if (counter >= 3)
		return TRUE;
	else
		return FALSE;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_position
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Aktuelle Position in Bezug auf Nullposition auslesen
 *  RETURNS: int32_t Position in µSteps
 ******************************************************************************** */
int32_t tmc5130a_read_position(uint8_t devadr) {
	tmc5130a_spi_read_reg(devadr, 0x21);
	return srdata2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_start_dest
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft geändert werden soll
 *  DESCRIPTION: Ziel setzen und Losfahren
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_start_dest(uint8_t devadr, int32_t s32data) {
	tmc5130a_POSITIONING_MODE(devadr);
	tmc5130a_spi_write_sreg(devadr, 0x2D, s32data);	// RW	XTARGET
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_dest
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Aktuelle Zielposition auslesen
 *  RETURNS: int32_t Zielposition in µSteps
 ******************************************************************************** */
int32_t tmc5130a_read_dest(uint8_t devadr) {
	tmc5130a_spi_read_reg(devadr, 0x2D);
	return srdata2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_wait_destination
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Abwarten, bis Motor sein Ziel erreicht hat
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_wait_destination(uint8_t devadr, int32_t s32data) {
	tmc5130a_spi_read_reg(devadr, 0x21);		// RW 	XACTUAL (Zielposition)
	while (srdata2 != s32data) {
		tmc5130a_spi_read_reg(devadr, 0x21);
		// ... timeout
	}
}


/* ******************************************************************************
 *  FUNCTION: tmc5130a_read_register
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  DESCRIPTION: Alle Register auslesen
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_read_register(uint8_t devadr) {
	cfg_reg cfg_readreg[] = { { 0x00, 0x00000000 }	// RW
			, { 0x01, 0x00000000 }	// R+C
			, { 0x02, 0x00000000 }	// R
			, { 0x03, 0x00000000 }	// W
			, { 0x04, 0x1100000c }	// R
			, { 0x05, 0x00000000 }	// W
			, { 0x10, 0x00000000 }	// W
			, { 0x11, 0x00000000 }	// W
			, { 0x12, 0x000fffff }	// R
			, { 0x13, 0x00000000 }	// W
			, { 0x14, 0x00000000 }	// W
			, { 0x15, 0x00000000 }	// W
			, { 0x20, 0x00000000 }	// RW
			, { 0x21, 0x00000000 }	// RW Position
			, { 0x22, 0x00000000 }	// R
			, { 0x23, 0x00000000 }	// W
			, { 0x24, 0x00000000 }	// W
			, { 0x25, 0x00000000 }	// W
			, { 0x26, 0x00000000 }	// W
			, { 0x27, 0x00000000 }	// W
			, { 0x28, 0x00000000 }	// W
			, { 0x2A, 0x00000000 }	// W
			, { 0x2B, 0x00000000 }	// W
			, { 0x2C, 0x00000000 }	// W
			, { 0x2D, 0xffff3800 }	// RW Zielposition
			, { 0x33, 0x00000000 }	// W
			, { 0x34, 0x00000000 }	// RW
			, { 0x35, 0x00000680 }	// R+C
			, { 0x36, 0x00000000 }	// R
			, { 0x38, 0x00000000 }	// RW
			, { 0x39, 0x00000000 }	// RW
			, { 0x3A, 0x00000000 }	// W
			, { 0x3B, 0x00000000 }	// R+C
			, { 0x3C, 0x00000000 }	// R
			, { 0x60, 0x00000000 }	// W
			, { 0x61, 0x00000000 }	// W
			, { 0x62, 0x00000000 }	// W
			, { 0x63, 0x00000000 }	// W
			, { 0x64, 0x00000000 }	// W
			, { 0x65, 0x00000000 }	// W
			, { 0x66, 0x00000000 }	// W
			, { 0x67, 0x00000000 }	// W
			, { 0x68, 0x00000000 }	// W
			, { 0x69, 0x00000000 }	// W
			, { 0x6A, 0x0000031f }	// R
			, { 0x6B, 0x00f70000 }	// R	(0x002f010d)
			, { 0x6C, 0x000100C5 }	// RW
			, { 0x6D, 0x00000000 }	// W
			, { 0x6E, 0x00000000 }	// W
			, { 0x6F, 0x810a00d3 }	// R	(0xe00f0000)
			, { 0x70, 0x00000000 }	// W
			, { 0x71, 0x00000000 }	// R
			, { 0x72, 0x00000000 }	// W
			, { 0x73, 0x00000000 }	// R
	};

	tmc5130a_spi_send(devadr, cfg_readreg,
			sizeof(cfg_readreg) / sizeof(cfg_readreg[0]));
	tmc5130a_delay(1000);
	HAL_Delay(1);
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_150µs
 *  PARAMETERS: none
 *  DESCRIPTION: 150µs warten
 *  RETURNS: none
 ******************************************************************************** */
void tcm5130a_150us(void) {
	for (int m = 0; m < 2700; m++) m++;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_1µs
 *  PARAMETERS: none
 *  DESCRIPTION: 1µs warten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_1us(void){
	for (int m = 0; m<18; m++) m++;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_50ns
 *  PARAMETERS: none
 *  DESCRIPTION: 50ns warten
 *  RETURNS: none
 ******************************************************************************** */
void tmc5130a_50ns(void){
	for (int m = 0; m<1; m++) m++;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_spi_send
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  			arr: Array des Schreibinhalts
 *  			len: Anzahl der Register
 *  DESCRIPTION: Paket von Registern schreiben
 *  RETURNS: int8_t Status des SPI
 ******************************************************************************** */
uint8_t tmc5130a_spi_send(uint8_t devadr, cfg_reg *arr, int arrlen) {// read/write bundle of registers
	int i = 0;
	while (i < arrlen) {
		regadr = arr[i].reg_adr;
		tdata = arr[i].data;
		txbuf[0] = regadr;					// read or write (| 0x80)
		txbuf[1] = (tdata >> 24) & 0xff;	// msb
		txbuf[2] = (tdata >> 16) & 0xff;
		txbuf[3] = (tdata >> 8) & 0xff;
		txbuf[4] = tdata & 0xff;	// lsb

		tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);

		devstat = rxbuf[0];					// read
		rdata = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8)
				| rxbuf[4];
		arr[i].data = rdata;

		txbuf[0] = regadr & 0x7f;			// read
		tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);

		devstat2 = rxbuf[0];
		urdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8)
				| rxbuf[4];
		arr[i].data = urdata2;

		i++;
	}
	return devstat2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_spi_write_ureg
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  			regadr: Registeradresse
 *  			tdata: uint32_t Datensatz
 *  DESCRIPTION: vorzeichenunbehaftete Daten in Register schreiben
 *  RETURNS: int8_t Status
 ******************************************************************************** */
uint8_t tmc5130a_spi_write_ureg(uint8_t devadr, uint8_t regadr, uint32_t tdata) {
	txbuf[0] = regadr | 0x80;			// write forced
	txbuf[1] = (tdata >> 24) & 0xff;	// msb
	txbuf[2] = (tdata >> 16) & 0xff;
	txbuf[3] = (tdata >> 8) & 0xff;
	txbuf[4] = tdata  & 0xff;	// lsb

	tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);
	devstat = rxbuf[0];					// read
	rdata = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];

	tcm5130a_spi_transmitreceive(devadr,txbuf,rxbuf);
	devstat2 = rxbuf[0];
	urdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
	uint8_t i = 1;
	while(urdata2 != tdata && i < MAX_TRIES)
	{
		tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);  //now data comes back
		devstat2 = rxbuf[0];
		urdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
		i++;
	}
	if(i >= MAX_TRIES)
	{
		tmc5130a_PIN_STOP(devadr);
		ErrorDisplay(IERR_SPI_WRITE_ERROR);
	}
	return devstat; // old status
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_spi_write_sreg
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  			regadr: Registeradresse
 *  			tdata: int32_t Datensatz
 *  DESCRIPTION: vorzeichenbehaftete Daten in Register schreiben
 *  RETURNS: int8_t Status
 ******************************************************************************** */
uint8_t tmc5130a_spi_write_sreg(uint8_t devadr, uint8_t regadr, int32_t stdata) {
	txbuf[0] = regadr | 0x80;			// write forced
	txbuf[1] = (stdata >> 24) & 0xff;	// msb
	txbuf[2] = (stdata >> 16) & 0xff;
	txbuf[3] = (stdata >> 8) & 0xff;
	txbuf[4] = stdata & 0xff;	// lsb

	tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);
	devstat = rxbuf[0];					// read
	rdata = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
	tcm5130a_spi_transmitreceive(devadr,txbuf,rxbuf);
	devstat2 = rxbuf[0];
	srdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
	uint8_t i = 1;
	while(srdata2 != stdata && i < MAX_TRIES)
	{
		tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);  //now data comes back
		devstat2 = rxbuf[0];
		srdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
		i++;
	}
	if(i >= MAX_TRIES)
	{
		tmc5130a_PIN_STOP(devadr);
		ErrorDisplay(IERR_SPI_WRITE_ERROR);
	}
	return devstat; //old status

}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_spi_read_reg
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  			regadr: Registeradresse
 *  DESCRIPTION: Daten aus Register lesen
 *  			Antwort steht in srdata2 (vorzeichenbehaftetes Ergebnis)
 *  			und 			 urdata2 (vorzeichenunbehaftetes Ergebnis)
 *  RETURNS: int8_t Status
 ******************************************************************************** */
uint8_t tmc5130a_spi_read_reg(uint8_t devadr, uint8_t regadr) {
	txbuf[0] = regadr & 0x7f;			// read forced
	txbuf[1] = 0x00;
	txbuf[2] = 0x00;
	txbuf[3] = 0x00;
	txbuf[4] = 0x00;

	tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);
	devstat = rxbuf[0];					// read
	rdata = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];

	tcm5130a_spi_transmitreceive(devadr, txbuf, rxbuf);  //now data comes back
	devstat2 = rxbuf[0];
	urdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
	srdata2 = (rxbuf[1] << 24) | (rxbuf[2] << 16) | (rxbuf[3] << 8) | rxbuf[4];
	devstat2 = rxbuf[0];
	return devstat2;
}

/* ******************************************************************************
 *  FUNCTION: tmc5130a_spi_write_ureg
 *  PARAMETERS: devadr: Motor, desssen Eigenschaft gelesen werden soll
 *  			txbuf: Schreibsatz (erste 8 bit sind Adresse)
 *  			rxbuf: Lesesatz (erste 8 bit sind Adresse)
 *  DESCRIPTION: Daten in Register schreiben und vorherige Antwort empfangen
 *  			(SPI ist Schieberegister)
 *  RETURNS: int8_t Status
 ******************************************************************************** */
uint8_t tcm5130a_spi_transmitreceive(uint8_t devadr, uint8_t *txbuf, uint8_t *rxbuf) {
	unsigned char SPI2_STATUS;

	if (devadr & 0x01) { // SPI Chip Select + Device Address - Multiplexer (NSS + A0/A1 -> CS1...CS4)
		HAL_GPIO_WritePin(SPI2_A0_GPIO_Port, SPI2_A0_Pin, GPIO_PIN_SET); // A0 = high
	} else {
		HAL_GPIO_WritePin(SPI2_A0_GPIO_Port, SPI2_A0_Pin, GPIO_PIN_RESET); // A0 = low
	}
	if (devadr & 0x02) {
		HAL_GPIO_WritePin(SPI2_A1_GPIO_Port, SPI2_A1_Pin, GPIO_PIN_SET); // A1 = high
	} else {
		HAL_GPIO_WritePin(SPI2_A1_GPIO_Port, SPI2_A1_Pin, GPIO_PIN_RESET); // A1 = low
	}
	tmc5130a_50ns();
	HAL_GPIO_WritePin(nSPI2_SS_GPIO_Port, nSPI2_SS_Pin, GPIO_PIN_RESET); // SSn = low (select)
	SPI2_STATUS = HAL_SPI_TransmitReceive(&hspi2, txbuf, rxbuf, 5, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(nSPI2_SS_GPIO_Port, nSPI2_SS_Pin, GPIO_PIN_SET); // SSn = high (unselect)
	if(SPI2_STATUS != HAL_OK)
		ErrorDisplay(IERR_SPI_ERROR);
	return SPI2_STATUS;
}

/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
