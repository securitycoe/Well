/* CODE BEGIN Header */
/**
 ******************************************************************************
 * @file		   : tmc5130a.h
 * @brief		  : Header for tmc5130a.c file.
 *				   This file contains the common defines of the application.
 * @created		: 21.04.2022 11:00
 *
 * @Author			: Ilmotronics
 ******************************************************************************
 */
/* CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INC__TMC5130A_H_
#define INC__TMC5130A_H_

#define P_DEVICE	0
#define VERT_DEVICE	1
#define HRZ_DEVICE	2

#define MAX_TRIES 5

typedef struct {

	uint8_t reg_adr;
	uint32_t data;
} cfg_reg;

struct Motor{					//saved motor settings
	uint32_t run_current;
	uint32_t hold_current;
	uint32_t hold_delay;
};
extern struct Motor motor_1, motor_2, motor_3;

/* Exported functions prototypes ---------------------------------------------*/
// init (set registers)
void tmc5130a_init(void);
void tmc5130a_CURR(uint8_t devadr);
void tmc5130a_CURR_MAN(uint8_t devadr,uint32_t holddelay, uint32_t run, uint32_t hold);
void tmc5130a_HOLD_CURR_MAN(uint8_t devadr, uint32_t hold);
void tmc5130a_ACCELERATION(uint8_t devadr);
void tmc5130a_ACCELERATION_MAN(uint8_t devadr, uint32_t a, uint32_t d);
float tmc5130a_SPEED_ACCELERATION_CURVE(float m, float t, float speed);
void tmc5130a_CHOPCONF(uint8_t devadr);
void tmc5130a_CHOPCONF_speed(uint8_t devadr, uint32_t speed);
void tmc5130a_CHOPCONF_MAN(uint8_t devadr, uint8_t* spread, uint32_t speed);
void tmc5130a_CHOPCONF_PAR(uint8_t devadr);
void tmc5130a_STEALTHCHOP_CONF(uint32_t devadr);
void tmc5130a_STEALTHCHOP_CONF_MAN(uint32_t devadr, uint8_t freewheel, uint8_t pwm_symmetric, uint8_t pwm_autoscale,
									uint8_t pwm_freq, uint8_t pwm_grad, uint8_t pwm_ampl);
void tmc5130a_STEALTHCHOP_ACTIVATE(uint32_t devadr, uint8_t enable);
void tmca5130aSetParameters(uint8_t devadr, uint8_t param[]);

void tmc5130a_COOLCONF(uint8_t devadr);		// cool config in tmc einschreiben
void tmc5130a_COOLCONF_MAN(uint8_t devadr, uint32_t tcoolthrs, uint8_t sfilt, int8_t sgt, uint8_t seimin, uint8_t sedn, uint8_t semax, uint8_t seup, uint8_t semin);

void tmc5130a_CFG_SOFT_STOP(uint8_t devadr);				// nur konfigurieren
void tmc5130a_CFG_HARD_STOP(uint8_t devadr);
void tmc5130a_PIN_STOP(uint8_t devadr);
void tmc5130a_PIN_RUN(uint8_t devadr);
void tmc5130a_STOP(uint8_t devadr);	// Motor Stop, Warten auf Stop, Zielmode setzen, Ziel = aktuelle Position, Motor Freigabe
void tmc5130a_SOFT_STOP(uint8_t devadr);		// konfigurieren und ausführen
void tmc5130a_HART_STOP(uint8_t devadr);

int32_t tmc5130a_read_ramp_stat(uint8_t devadr);// Abfrage dieses Statusregisters (return urdata2)
int32_t tmc5130a_read_vzero(uint8_t devadr);
void tmc5130a_wait_stop(uint8_t devadr);	// warte auf Stillstand des Motors
void tmc5130a_wait_stop_all_motors(void);// warte auf Stillstand aller Motoren

// (maximale) Geschwindigkeit festlegen
void tmc5130a_VELOCITY(uint8_t devadr, int32_t svelocity);// init Geschwindigkeit
int32_t tmc5130a_VELOCITY_FULL(uint8_t devadr);	// init Geschwindigkeit maximal (Daten Motor)
void tmc5130a_VELOCITY_FAST(uint8_t devadr);// init Schnellgang (Daten Anwendung)
void tmc5130a_VELOCITY_SLOW(uint8_t devadr);// init Langsamgang (Daten Anwendung)
void tmc5130a_VELOCITY_UPS__TBD__(uint8_t devadr, float ups);// TBD! init Geschwindigkeit [Umdrehungen per Sekunde]

void tmc5130a_POSITIONING_MODE(uint8_t devadr);					// init Zielmode
void tmc5130a_VELOCITY_MODE_POS(uint8_t devadr);// init Geschwindigkeitsmode & Richtung
void tmc5130a_VELOCITY_MODE_NEG(uint8_t devadr);					//
void tmc5130a_HOLD_MODE(uint8_t devadr);// Stop (velocity  remains  unchanged,unless stop event occurs)
void tmc5130a_HOMING(uint8_t devadr);			// Setze Aktuelle Position = 0

int32_t tmc5130a_read_velocity(uint8_t devadr);
void tmc5130a_start_velocity(uint8_t devadr, int32_t svelocity);// init Geschwindigkeitsmode,Geschwindigkeit,Richtung,starte Rampe
void tmc5130a_stop_velocity(uint8_t devadr);			// Geschwindigkeit = 0

int32_t tmc5130a_read_position(uint8_t devadr);
void tmc5130a_start_dest(uint8_t devadr, int32_t starget);// init & start Zielfahrt,Ziel
int32_t tmc5130a_read_dest(uint8_t devadr);
void tmc5130a_wait_destination(uint8_t devadr, int32_t starget);// warte bis Ziel erreicht,Ziel
bool tmc5130a_is_stop(uint8_t devadr);
uint32_t tmc5130a_read_ot(uint8_t devadr);
uint32_t tmc5130a_read_otpw(uint8_t devadr);

void tmc5130a_start_target(uint8_t devadr);	// init & start Zielfahrt,Ziel = config
//void tmc5130a_wait_target(uint8_t devadr);						// warte bis Ziel erreicht

void tmc5130a_read_register(uint8_t devadr);	// for debug: read register set
//uint8_t tmc5130a_read_engine_switch(void);									// old,read adress from external switches

// basics
void tcm5130a_150us(void);   // delay
void tcm5130a_1us(void);
uint8_t tmc5130a_spi_send(uint8_t devadr, cfg_reg *arr, int arrlen); // read/write a bundle of registers
uint8_t tmc5130a_spi_write_ureg(uint8_t devadr, uint8_t regadr, uint32_t tdata); // write register with "unsigned long"
uint8_t tmc5130a_spi_write_sreg(uint8_t devadr, uint8_t regadr, int32_t stdata); // write register with "signed long"
uint8_t tmc5130a_spi_read_reg(uint8_t devadr, uint8_t regadr); // read register,2 result data: "unsigned long" & "signed long"

// for other HALs,adapt the following subroutines (in _tmc5130a.c):
uint8_t tcm5130a_spi_transmitreceive(uint8_t devadr, uint8_t *txbuf,
		uint8_t *rxbuf);
void tmc5130a_delay(uint32_t Delay);							// [ms]

// ############################################################################
// Definitionen für alle Motoren
#define SpU				200				// Step per U
#define uSpS			256				// uStep per Step
#define FAST_ACCELERATION 1500*200*256/60

// Definitionen für die TMC5130 Beschaltung
#define Ishunt_120mR	1641			// [mArms] = 2321 mApeak, maximaler Strom des TMC5130 bei Rsense = 120mR
#define Ishunt_150mR	1352			// [mArms] = 1912 mApeak, maximaler Strom des TMC5130 bei Rsense = 150mR
#define Ishunt_M1		Ishunt_120mR
#define Ishunt_M2		Ishunt_150mR
#define Ishunt_M3		Ishunt_150mR
#define iholddelay		0 				// 0...15 Delay per current reduction step in multiple of 2^18 clocks

#define KCLK			100				// [%] Korrekturfaktor für externen 16MHz Quarz-Taktgeneratur
//#define KCLK			134				// [%] (mittlerer) Korrekturfaktor für den internen (~12MHz) Taktgeneratur

// ############################################################################
// Nachfolgend spezifische Werte für die Motoren eintragen
// ----------------------------------------------------------------------------
// motor 1 XS1 - M1 - Kolben/Pumpe (1U = 2mm laut "Well-Vario" Blatt)
// PK264-03B (Seriell)
//  max: 2A1,1.8°,(2ms,500Hz,10 rps,2000Step/s) 600 rpm @71% (0.46Nm)

// A T T E N T I O N ! TDB! DO NOT ACTIVATE >> 1000 miliAmpere
//#define M1_CURR	1571			// [mArms] (1200 mVpp/270mR = 2222 mApeak) Umrechnung in Stufen und Begrenzung auf Maximalstrom (abhängig von Rsense) erfolgt im Programm
#define M1_CURR		1571
#define M1_HOLD		760				// [mArms]

#define M1_CS_min	1				// semin  0...15; 0: no coolstep; 1: max coolstep
#define M1_CS_Imin	1				// seimin 0...1;  0: 1/2  1: 1/4 of current setting (IRUN)

#define M1_SpU		200				// [Step/U] (@ 1,8°)
#define M1_DEST		M1_SpU*uSpS		// [µStep] für 1 U (test destination position)

// Definition(en) für genutze Geschwindigkeiten
// Vmax [Step/s] Wenn hier ein Wert definiert ist,wird aus diesen Wert Vmax berechnet (inklusive Umrechnung -> µStep/s) und die nachfolgende Definitionesart nicht genutzt
// laut Motor Datenblatt max:

// 600 rpm
// 500 rpm max laut "Well-Vario"Blatt
#define M1_RPM		500				// 300 rpm def laut "Well-Vario"Blatt (10,00 mm/s)	-> 512000 µSps
// 300 rpm def laut Oszi Bild						-> 512000 µSps
//  35 rpm def laut "Well-Vario"Blatt ( 1,17 mm/s)	->  29867 µSps

//#define M1_RPM	600				// 300 rpm  -> 10U/s
//#define M1_SPS	2000			// [Step/s]	-> 2000 -> 512000 µSps
//#define M1_FREQ	500				// [Hz]		-> 500
//#define M1_CYCLE	2000			// [us]		_> 200

#define M1_FAST		60				// [%] von Vmax (300/500 rpm)
#define M1_SLOW		7				// [%] von Vmax ( 35/500 rpm)

// Geschwindigkeit Stützpunkte und Anstiege
#define M1_VSTART	1000		// [uStep/s] (1000) Motor start velocity (unsigned)
#define M1_VSTOP	10			// [uStep/s]  (>=  10) Stop velocity (Near to zero)
#define M1_VCOOL	500000	    // µSteptime 0...1048575 [n*1/16MHz]
#define M1_V1		50000		// [uStep/s] (50000) Acceleration threshold velocity V1 /D1

#define M1_A1		10000		// [uStep/s²] (1000) First acceleration between VSTART and V1 (unsigned)
#define M1_AMAX		5000		// [uStep/s²] (500) Second Acceleration above V1
#define M1_DMAX		7000		// [uStep/s²] (700) Deceleration above V1
#define M1_D1		14000		// [uStep/s²] (1400) Deceleration below V1

// spread Cycle
#define M1_TOFF		5			// (5)
#define	M1_HSTRT	4			// (4)
#define M1_HEND		1			// (1)
#define M1_RNDTF	0			// (0)
#define M1_TBL		1			// (1)

// ----------------------------------------------------------------------------
// motor 2 XS2 - M2 - Z - Ausheber (1U = 37,5mm laut "Well-Vario" Blatt)
// C102Y-9012KE
//  max: 1A2,1,8°,rpm @71% (0.xxNm)
#define M2_CURR		700				// [mArms] (400 mVpp = 741 mApeak)
#define M2_HOLD		420			    // [mArms]

#define M2_CS_min	1				// 1 semin 0...15; 0: no   1: max coolstep (motor load angle)
#define M2_CS_Imin	0				// 0 seimin 0...1; 0: 1/2  1: 1/4 of current setting (IRUN)

#define M2_SpU		200
#define M2_DEST		M2_SpU*uSpS		// 1 U

// Vmax [Step/s] Wenn hier ein Wert definiert ist,wird aus diesen Wert Vmax berechnet (inklusive Umrechnung -> µStep/s) und die nachfolgende Definitionesart nicht genutzt
#define M2_RPM		140				// 140 rpm laut "Well-Vario"Blatt 	(87,50 mm/s)	-> 113778 µSps
//  30 rpm laut ch_ig_z.c			(18,75 mm/s)	->  25600 µSps
//#define M2_SPS	466				// [Step/s]		(4*Freq)
//#define M2_FREQ	117				// [Hz]			(Freq)
//#define M2_CYCLE	8571			// [us]			(1/Freq)
#define M2_FAST		100
#define M2_SLOW		5

// Geschwindigkeit Stützpunkte und Anstiege
#define M2_VSTART	1000		// [uStep/s] (1000) Motor start velocity (unsigned)
#define M2_VSTOP	10			// [uStep/s]  (>=  10) Stop velocity (Near to zero)
#define M2_VCOOL	25000	//
#define M2_V1		50000		// [uStep/s] (50000) Acceleration threshold velocity V1 /D1

#define M2_A1		2000		// [uStep/s²] (1000) First acceleration between VSTART and V1 (unsigned)
#define M2_AMAX		1000		// [uStep/s²] (500) Second Acceleration above V1
#define M2_DMAX		1000		// [uStep/s²] (700) Deceleration above V1
#define M2_D1		2000		// [uStep/s²] (1400) Deceleration above V1

//spread Cycle
#define M2_TOFF		5
#define	M2_HSTRT	4
#define M2_HEND		1
#define M2_RNDTF	0
#define M2_TBL		1

// ----------------------------------------------------------------------------
// motor 3 XS3 - M3 - Hor/Tisch  (1U = 43,982mm laut "Well-Vario" Blatt)
// C102Y-9012KE
// max: 1A2,1,8°,rpm @71% (0.xxNm)
#define M3_CURR		1022			// mArms (780mVpp = 1444 mApeak)
#define M3_HOLD		760				//[mArms] (ori)
#define M3_CS_min	1				// semin 0...15; 0: no coolstep; 1: max coolstep
#define M3_CS_Imin	1				// seimin 0...1;  0: 1/2  1: 1/4 of current setting (IRUN)

#define M3_SpU		200
#define M3_DEST		M3_SpU*uSpS		// 1 U

// Vmax [Step/s] Wenn hier ein Wert definiert ist,wird aus diesen Wert Vmax berechnet (inklusive Umrechnung -> µStep/s) und die nachfolgende Definitionesart nicht genutzt
#define M3_RPM		250				// 250 rpm max laut "Well-Vario"Blatt (183 mm/s)	-> 213333 µSps
// 200 rpm def laut "Well-Vario"Blatt (147 mm/s)	-> 119467 µSps
//#define M3_SPS	833				// [Step/s]		(4*Freq)
//#define M3_FREQ	3333			// [Hz]			(Freq)
//#define M3_CYCLE	300				// [us]			(1/Freq)
// µSps				213333
#define M3_FAST		80				// 200/250 rpm laut "Well-Vario" Blatt
#define M3_SLOW		5

// Geschwindigkeit Stützpunkte und Anstiege
#define M3_VSTART	1000		// [uStep/s] (1000) Motor start velocity (unsigned)
#define M3_VSTOP	10			// [uStep/s]  (>=  10) Stop velocity (Near to zero)
#define M3_VCOOL	20000	//
#define M3_V1		50000		// [uStep/s] (50000) Acceleration threshold velocity V1 /D1

#define M3_A1		10000		// [uStep/s²] (1000) First acceleration between VSTART and V1 (unsigned)
#define M3_AMAX		5000		// [uStep/s²] (500) Second Acceleration above V1
#define M3_DMAX		7000		// [uStep/s²] (700) Deceleration above V1
#define M3_D1		14000		// [uStep/s²] (1400) Deceleration above V1

// spread Cycle
#define M3_TOFF		5
#define	M3_HSTRT	4
#define M3_HEND		1
#define M3_RNDTF	0
#define M3_TBL		1



#endif /* INC__TMC5130A_H_ */

/************************ (C) COPYRIGHT Ilmotronics GmbH *****END OF FILE****/
