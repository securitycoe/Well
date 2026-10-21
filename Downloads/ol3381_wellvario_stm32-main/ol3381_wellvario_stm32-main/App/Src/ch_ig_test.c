/*******************************************************************
 *
 *	DESCRIPTION:	Diverse Testroutinen
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:		---
 *
 *******************************************************************/

/** include files **/
#include <malloc.h>

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- BASE
#include "ch_head_fram.h"
// ---------- Inc
#include <_i2c_fram.h>
#include <_uart_terminal.h>
// ---------- Master
#include "ch_ig_cmd.h"
#include "ch_ig_config.h"
#include "ch_ig_err.h"
#include "ch_ig_grp.h"
#include "ch_ig_head.h"
#include "ch_ig_hrz_pos.h"
#include "ch_ig_hrz.h"
#include "ch_ig_key.h"
#include "ch_ig_peri.h"
#include "ch_ig_piston.h"
#include "ch_ig_ptme.h"
#include "ch_ig_spw.h"
#include "ch_ig_test.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "lcd.h"
// ---------- Pipettor_Allgemein
#include "Well_Defines.h"
#include "Well_Error.h"

/** local definitions **/
#define CYCLE_TIME 15
#define LifterCycleNo 50
#define LifterCycleRepeat 1
#define DeviceCycleNo 1440 	// Laufzeit bei 1440 --> ca. 8h
#define DeviceCycleRepeat 4 	// Anzahl der Kontrollzyklen

//#define HrzDrvSpeedLimit 20	//Umdr/min
/** default settings **/

/** external functions **/

/** external data **/
extern DispString_t str_go_on[];
extern DispString_t str_xy_go_esc[];

extern uword g_uwWaitTime;
extern uword g_uwKeyRepeatTime;
extern uword g_uwBeepTime;
extern uword g_uwTimeout;
extern uword g_uwHeadSafetyTime;

extern unsigned char 	DISPLAY;
extern unsigned char	io0;
extern unsigned char	io2;
extern unsigned char	io3;
extern unsigned char	io7;


/** internal functions **/

/** public data **/

/** private data **/

const char is_shielded[]	 = "geschlossen";
const char is_opened[]		 = "offen";
const char is_pressed[]	 = "betaetigt";
const char is_not_pressed[] = "nicht betaetigt";

DispString_t str_stac_adj[]			 = 	{{1,11,"STACKERJUSTIERUNG"}
						 	 	 		,{1,11,"ADJUSTMENT STACKER"}};
DispString_t str_pos[]				 = 	{{2,1,"Position:"}
										,{2,1,"Position:"}};
DispString_t str_dev_test[]			 = 	{{1,11,"*** Eigentest ***",}
										,{1,11,"*** self test ***"}};
DispString_t str_selftest[]			 = 	{{1,15,"EIGENTEST"}
										,{1,15,"SELF TEST"}};
DispString_t str_cycle_no[]			 = 	{{2,1,"Zyklusanzahl:"}
										,{2,1,"cycle no:"}};
DispString_t str_lifter_test[]		 = 	{{1,6,"*** Eigentest Kopfausheber ***"}
										,{1,6,"*** self test head lifter ***"}};
DispString_t str_of[]				 = 	{{2,6,"von"}
										,{2,6,"of"}};
DispString_t str_head_test[]		 = 	{{1,9,"*** Eigentest Kopf ***"}
										,{1,9,"*** self test head ***"}};
DispString_t str_prg_self_test[]	 = 	{{2,1,"Ger\204t"}
										,{2,1,"Device"}};
DispString_t str_prg_lifter_test[]	 = 	{{2,8,"Kopfausheber"}
										,{2,9,"Lifter"}};
DispString_t str_prg_head_test[]	 = 	{{2,22,"Kopf"}
										,{2,17,"Head"}};
DispString_t str_prg_exit[]			 = 	{{2,28,"Verlassen"}
										,{2,23,"Exit"}};

struct Modus selftests[]		 = 	{{str_prg_self_test,SelfTest}
									,{str_prg_lifter_test,LifterSelfTest}
									,{str_prg_head_test,HeadSelfTest}
									,{str_prg_exit,ProgExit}};

struct Modus selftests3714[]	 = 	{{str_prg_self_test,SelfTest}
 									,{str_prg_head_test,HeadSelfTest}
									,{str_prg_exit,ProgExit}};

struct ADJ_POS {ubyte no;
 				DispString_t *str;
			   };

	// Justierpositionen für die Stacker
DispString_t Pos1[]	 = 	{{2,11,"1"}
						,{2,11,"1"}};
DispString_t Pos2[]	 = 	{{2,11,"2"}
						,{2,11,"2"}};
DispString_t Pos3[]	 = 	{{2,11,"3"}
						,{2,11,"3"}};
DispString_t Pos4[]	 = 	{{2,11,"links 2"}
						,{2,11,"left 2"}};
DispString_t Pos5[]	 = 	{{2,11,"links 3"}
						,{2,11,"left 3"}};
DispString_t Pos6[]	 = 	{{2,11,"rechts 3"}
						,{2,11,"right 3"}};
DispString_t Pos7[]	 = 	{{2,11,"rechts 2"}
						,{2,11,"right 2"}};
DispString_t Pos8[]	 = 	{{2,11,"4"}
						,{2,11,"4"}};
DispString_t Pos10[] = 	{{2,11,"links 1"}
						,{2,11,"left 1"}};
DispString_t Pos11[] = 	{{2,11,"rechts 1"}
						,{2,11,"right 1"}};

const struct ADJ_POS AdjPosTab4[]	 = 	{{1,Pos1}
		 	 	 	 	 	 	 	 	,{2,Pos2}
										,{3,Pos3}
										,{8,Pos8}
										,{10,Pos10}
										,{4,Pos4}
										,{5,Pos5}
										,{11,Pos11}
										,{7,Pos7}
										,{6,Pos6}};

const struct ADJ_POS AdjPosTab35[]	 = 	{{1,Pos1}
		 								,{2,Pos2}
										,{3,Pos3}
										,{4,Pos10}
										,{5,Pos4}};

/*const struct ADJ_POS AdjPosTab5[]	 = 	{{1,Car5Pos1}
										,{2,Car5Pos2}
										,{3,Car5Pos3}
										,{4,Car5Pos4}
										,{5,Car5Pos5}
										,{60,Car5Pos60}
										,{61,Car5Pos61}
										,{62,Car5Pos62}
										,{63,Car5Pos63}
										,{4,Car5Pos64}
										,{65,Car5Pos65}
										,{66,Car5Pos66}
										,{67,Car5Pos67}
										,{80,Car5Pos80}
										,{81,Car5Pos81}
										,{82,Car5Pos82}
										,{83,Car5Pos83}
										,{84,Car5Pos84}
										,{85,Car5Pos85}
										,{86,Car5Pos86}
										,{87,Car5Pos87}};	*/

const ubyte AdjPosTab5[]	 = 	{1,2,3,4,5
		 	 	 		 	 	,60,61,62,63,64,65,66,67
								,80,81,82,83,84,85,86,87,};

DispString_t str_disp_connected[]		 = 	{{2,1,"Kopf verbunden"}
											,{2,1,"Head connected"}};
DispString_t str_disp_not_connected[]	 = 	{{2,1,"Kopf nicht verbunden"}
											,{2,1,"Head not connected"}};
DispString_t str_lift_to_upper[]		= 	{{2,1,"Lift f\204hrt nach oben ..."}
											,{2,1,"Lift is moving up ..."}};
DispString_t str_lift_to_bottom[]		 = 	{{2,1,"Lift f\204hrt nach unten ..."}
											,{2,1,"Lift is moving down ..."}};
DispString_t str_adjust_lift[]			 = 	{{1,1,"JUSTIERUNG LIFT"}
											,{1,1,"ADJUSTMENT LIFT"}};

static const char *m_ppstrHallSensorState[]	 = 	{"X----- rechts"
												,"-X---- mitte"
												,"--X--- unbenutzt"
												,"---X-- links"
												,"----X- Wechseltool r"
												,"-----X Wechseltool l"
												,"------ keine"
												,"Fehler"};

/** public functions **/

/** private functions **/

void AdjStackPos(void){
	switch(g_pudtHrzPosTabPtr->cartype){
	case CARR_4:
		AdjStackPos4(); break;
	case CARR_5:
		AdjStackPos5(); break;
	case CARR_35:
		AdjStackPos35(); break;
	}
}

/* FUNCTION: 	AdjStackPos35
   PARAMETERS:	---
   DESCRIPTION:	Hilfsprogramm für die Justierung der Stacker beim Einsatz des 3-Platzwagen für LHA OL3714
   RETURNS: 	--- */
void AdjStackPos35(void){  	// Justage der Stackerposition
	ubyte pos,key,state;
	ClearAllAndDisplay(str_stac_adj); SetCursorOff();
	VertDrvCalibrate();
	HrzDrvCalibrate(); pos = g_ubytHrzActNo;
	Display(str_pos);
	while(1){
		LcdClear(AdjPosTab35[pos-1].str->line);
		Display(str_pos); Display(AdjPosTab35[pos-1].str);
		while(1){
			state = CarDrvNo(AdjPosTab35[pos-1].no);
			if(state == CB_READY) break;
			if(state == IERR_PROG_ABORT) CarManAbortHandling(); else ErrorDisplay(state);
		}
		key = KeyWaitForAny();
		if(key == ENTER_KEY) return;
		if(key == TRI_RIGHT_KEY) pos-= 1;
		if(key == TRI_LEFT_KEY) pos+= 1;
		if(pos > sizeof(AdjPosTab35)/sizeof(struct ADJ_POS)) pos = 1;
		if(pos < 1) pos = sizeof(AdjPosTab35)/sizeof(struct ADJ_POS);
	}
}

/* FUNCTION: 	AdjStackPos5
   PARAMETERS:	---
   DESCRIPTION:	Hilfsprogramm für die Justierung der Stacker beim Einsatz des 5-Platzwagens
   RETURNS: 	--- */
void AdjStackPos5(void){  	// Justage der Stackerposition
	ubyte pos,key,state; char cHelp[5];
	ClearAllAndDisplay(str_stac_adj); SetCursorOff();
	VertDrvCalibrate();
	HrzDrvCalibrate();
	pos = g_ubytHrzActNo;
	// Display(str_pos);
	while(1){
		LcdClear(SECOND_LINE);
		sprintf(cHelp,"%u",(int)AdjPosTab5[pos-1]);
		Display(str_pos);
		LcdShow(SECOND_LINE,strlen(str_pos->message)+2,cHelp);	//Display(AdjPosTab4[pos-1].str);
		while(1){
			state = CarDrvNo(AdjPosTab5[pos-1]);
			if(state == CB_READY) break;
			if(state == IERR_PROG_ABORT)
				CarManAbortHandling();
			else
				ErrorDisplay(state);
		}
		key = KeyWaitForAny();
		if(key == ENTER_KEY) return;
		if(key == TRI_RIGHT_KEY) pos-= 1;
		if(key == TRI_LEFT_KEY) pos+= 1;
		if(pos > sizeof(AdjPosTab5)/sizeof(ubyte)) pos = 1;
		if(pos < 1) pos = sizeof(AdjPosTab5)/sizeof(ubyte);
	}
}

/* FUNCTION: 	AdjStackPos4
   PARAMETERS:	---
   DESCRIPTION:	Hilfsprogramm für die Justierung der Stacker beim Einsatz des 4- und des 4,5-Platzwagen
   RETURNS: 	--- */
void AdjStackPos4(void){  	// Justage der Stackerposition
	ubyte pos,key,state;
	ClearAllAndDisplay(str_stac_adj); SetCursorOff();
	VertDrvCalibrate();
	HrzDrvCalibrate(); pos = g_ubytHrzActNo;
	Display(str_pos);
	while(1){
		LcdClear(AdjPosTab4[pos-1].str->line);
		Display(str_pos); Display(AdjPosTab4[pos-1].str);
		while(1){
			state = CarDrvNo(AdjPosTab4[pos-1].no);
			if(state == CB_READY) break;
			if(state == IERR_PROG_ABORT)
				CarManAbortHandling();
			else
				ErrorDisplay(state);
		}
		key = KeyWaitForAny();
		if(key == ENTER_KEY) return;
		if(key == TRI_RIGHT_KEY) pos-= 1;
		if(key == TRI_LEFT_KEY) pos+= 1;
		if(pos > sizeof(AdjPosTab4)/sizeof(struct ADJ_POS)) pos = 1;
		if(pos < 1) pos = sizeof(AdjPosTab4)/sizeof(struct ADJ_POS);
	}
}

/* FUNCTION: 	Test_OL3387_ReadMagazinePara
   PARAMETERS:	---
   DESCRIPTION:	Test der Magnetsensoren	am Hummingwell-Kopf
   RETURNS: 	CB_READY,ERR_I2C */
ubyte Test_OL3387_ReadMagazinePara(void){
	ubyte state; bool key_hit; const char **ptr;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		if(I2CRead(0x22,1,&state) == FALSE) return IERR_I2C_HEAD_MAG;
		state = (state>>2);
		printf("Magnetkodierung:%x  ",(int)state);
		switch(state){
		case 0x3f:
			ptr = &m_ppstrHallSensorState[6];
			break;
		case 0x2f:
			ptr = &m_ppstrHallSensorState[5];
			break;
		case 0x1f:
			ptr = &m_ppstrHallSensorState[4];
			break;
	  /*case 0x3b:
			ptr = &m_ppstrHallSensorState[3];
			break;
		case 0x37:
			ptr = &m_ppstrHallSensorState[2];
			break;*/
		case 0x3E:
			ptr = &m_ppstrHallSensorState[0];
			break;	// LP wurde um 180° gedreht  eingebaut
		case 0x3D:
			ptr = &m_ppstrHallSensorState[1];
			break;
		default:
			ptr = &m_ppstrHallSensorState[7];
			break;
		}
		printf("%s\r",*ptr);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadMagazinePara
   PARAMETERS:	---
   DESCRIPTION:	Test der Magnetsensoren
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadMagazinePara(void){
	ubyte state;
	bool key_hit;
	const char **ptr;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		if(I2CRead(0x22,1,&state) == FALSE) return IERR_I2C_HEAD_MAG;
		state = (state>>2);
		printf("Magnetkodierung:%x  ",(int)state);
		switch(state){
		case 0x3f:
			ptr = &m_ppstrHallSensorState[6];
			break;
		case 0x2f:
			ptr = &m_ppstrHallSensorState[5];
			break;
		case 0x1f:
			ptr = &m_ppstrHallSensorState[4];
			break;
		case 0x3b:
			ptr = &m_ppstrHallSensorState[3];
			break;
		case 0x37:
			ptr = &m_ppstrHallSensorState[2];
			break;
		case 0x3E:
			ptr = &m_ppstrHallSensorState[1];
			break;
		case 0x3D:
			ptr = &m_ppstrHallSensorState[0];
			break;
		default:
			ptr = &m_ppstrHallSensorState[7];
			break;
	}
	printf("%s\r",*ptr);
	fflush(stdout);
	Wait(CYCLE_TIME);
	if(KeyScan() == NO_KEYS) key_hit = FALSE;
	if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadLowerPos
   PARAMETERS:	---
   DESCRIPTION:	Test der unteren Endlage Kolben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadLowerPos(){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadLowerPos();
		if(state == IERR_I2C_PST) return state;
		printf("Endlage Kolben unten:%s\r",(state == PST_POS)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Test der oberen Endlage Kolben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadUpperPos(void){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadUpperPos();
		if(state == IERR_I2C_PST) return state;
		printf("Endlage Kolben oben:%s\r",(state == PST_POS)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadFineZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Test der Fein-Nullposition Kolben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadFineZeroPos(void){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadFineZeroPos();
		if(state == IERR_I2C_PST) return state;
		printf("Null-Lage fein:%s\r",(state == PST_POS)?is_opened:is_shielded);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadCoarseZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Test der Grob-Nullposition Kolben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadCoarseZeroPos(void){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadCoarseZeroPos();
		if(state == IERR_I2C_PST) return state;
		printf("Null-Lage grob:%s\r",(state == PST_POS)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestReadEccentricSen
   PARAMETERS:	---
   DESCRIPTION:	Test des vorderen Sensors der Spanneinrichtung
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestReadEccentricSensorInFront(void){
	ubyte state;
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(I2CRead(0x20,1,&state) == FALSE) return ERR_I2C;
		printf("Spitzenanzug vorderer Sensor:%s\r",((state&0x40) == 0x40)?is_opened:is_shielded);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestReadEccentricBackSensor
   PARAMETERS:	---
   DESCRIPTION:	Test des hinteren Sensors der Spanneinrichtung
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestReadEccentricBackSensor(void){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(I2CRead(0x20,1,&state) == FALSE) return ERR_I2C;
		printf("Spitzenanzug hinterer Sensor:%s\r",((state&0x80) == 0x80)?is_opened:is_shielded);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestPstReadMagazineInsert
   PARAMETERS:	---
   DESCRIPTION:	Test des Schalters Magazin eingeschoben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte TestPstReadMagazineInsert(void){
	ubyte state; bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadMagazineInsert();
		if(state == IERR_I2C_HEAD_MAG) return state;
		printf("Magazinschalter:%s\r",(state == PST_MAG_AVAILABLE)?is_pressed:is_not_pressed);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestLiftUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Test der oberen Kopfposition
   RETURNS: 	CB_READY */
ubyte DevTestLiftUpperPos(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Endlage Kopf oben:%s\r",(HeadReadUpperPos)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestLiftLowerPos
   PARAMETERS:	---
   DESCRIPTION:	Test der unteren Kopfposition
   RETURNS: 	CB_READY */
ubyte DevTestLiftLowerPos(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Endlage Kopf unten:%s\r",(HeadReadLowerPos)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestLiftPreWarnPos
   PARAMETERS:	---
   DESCRIPTION:	Test der unteren nVorwarn-Kopfposition
   RETURNS: 	CB_READY */
ubyte DevTestLiftPreWarnPos(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("nVorwarn Kopf unten:%s\r",(HeadReadPreWarnPos)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestSwitchCover
   PARAMETERS:	---
   DESCRIPTION:	Test des Sicherheitsschalter S2 - Klappe
   RETURNS: 	CB_READY */
ubyte DevTestSwitchCover(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Zustand Kopfklappe:%s\r",(IsHeadCoverOpen())?is_opened:is_shielded);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestSwitchShield
   PARAMETERS:	---
   DESCRIPTION:	Test des Sicherheitsschalter S4 - Abdeckung
   RETURNS: 	CB_READY */
ubyte DevTestSwitchShield(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Zustand Jalousie:%s\r",(IsHeadShieldOpen())?is_opened:is_shielded);fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

ubyte DevTestSwitchHeadInsert(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		printf("Zustand Schalter Kopfposition:%s\r",(IsHeadInsertPos())?is_pressed:is_not_pressed);fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestHrzZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Test des Null-Lage des Horizontalantriebes
   RETURNS: 	CB_READY */
ubyte DevTestHrzZeroPos(void){
	bool key_hit;
	key_hit = TRUE;
	ErrorDisplay(EEPROMReadDeviceType());	// davon hängt der Null-Lageneingang ab
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Null-Lage Wagen:%s\r",(HrzZeroRequest())?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestVertZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Test des Null-Lage des Vertikalantriebes
   RETURNS: 	CB_READY */
ubyte DevTestVertZeroPos(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();	// Optokoppler TLP1001A,shield --> high
		printf("Ausheber nVorwarn: %s Null-Lage: %s\r",(VertReadAdvancePos)?is_shielded:is_opened,(VertReadEndPos)?is_shielded:is_opened);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestDrvEccentric
   PARAMETERS:	---
   DESCRIPTION:	überprüfung der Drehrichtung des Spitzenanzugmotors
   RETURNS: 	CB_READY,FEHLER */
ubyte TestDrvEccentric(void){
	bool key_hit;
	key_hit = TRUE;
	WDO();
	printf("Test Antrieb Spitzenanzug (Blick auf Abtriebsrad)\r"); fflush(stdout);
	printf("Drehrichtung: Uhrzeigersinn\r");
	fflush(stdout);
	DCMotorOnClockwise();
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	DCMotorStop(); Wait(CYCLE_TIME);
	key_hit = TRUE;
	printf("Drehrichtung: entgegen Uhrzeigersinn\r");
	fflush(stdout);
	DCMotorOnCounterwise();
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	DCMotorStop();
	return CB_READY;
}

/* FUNCTION: 	AdjTipsTightenMechanism
   PARAMETERS:	---
   DESCRIPTION:	Justierung des Spitzenanzugmechanismus
   RETURNS: 	CB_READY,FEHLER */
ubyte AdjTipsTightenMechanism(void){
	ubyte key_hit,state,old_state;
	bool flg_first;
	printf("Test mit Magazin\rAchtung,waehrend des Bewegungsablaufes keine\rmanuellen Eingriffe vornehmen...\r");
	fflush(stdout);
	flg_first = TRUE; key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		switch(tast_read()){
		case TRI_UP_KEY:
			DCMotorOnClockwise();
			break;
		case TRI_DOWN_KEY:
			DCMotorOnCounterwise();
			break;
		default:
			DCMotorStop();
			break;
		}
		//Sensorabfrage
		state = PstReadEccentricState();
		if(state == IERR_I2C_ECC) return state;
		if((old_state != state)||(flg_first == TRUE)){
			if(state == PST_ECCENTRIC_TIGHTEN){
				BeepOn();
				g_uwBeepTime = 20;
				printf("Spitzezustand angezogen\r");
				fflush(stdout);
			}
			if(state == PST_ECCENTRIC_LOOSE){
				BeepOn();
				g_uwBeepTime = 20;
				printf("Spitzezustand geloest\r");
				fflush(stdout);
			}
			if(state == PST_ECCENTRIC_NOT_TIGHTEN){
				BeepOn(); g_uwBeepTime = 20; printf("Spitzezustand nicht angezogen\r");
				fflush(stdout);}
		}
		if(g_uwBeepTime == 0) BeepOff();
		old_state = state; flg_first = FALSE;
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	TestTipsTightenMechanism
   PARAMETERS:	---
   DESCRIPTION:	Test des Spitzenanzugmechanismus
   RETURNS: 	CB_READY,FEHLER */
ubyte TestTipsTightenMechanism(void){	//Spitzenanzug mit Magazin testen
	ubyte key_hit,state;
	enum{TIGHTEN,LOOSEN,UNDEF} tips_state;
	tips_state = UNDEF;
	key_hit = TRUE;
	printf("Test mit Magazin\rAchtung,waehrend des Bewegungsablaufes keine\rmanuellen Eingriffe vornehmen...\r");
	fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		switch(tast_read()){
		case TRI_UP_KEY:
			WDO(); state = TipsTighten();
			if(state != CB_READY) return state;
			if(tips_state != TIGHTEN){
				printf("Magazin angezogen\r"),fflush(stdout);
				tips_state = TIGHTEN;
			}
			break;
		case TRI_DOWN_KEY:
			WDO();
			state = TipsLoose();
			if(state != CB_READY) return state;
			if(tips_state != LOOSEN){
				printf("Magazin geloest\r");
				fflush(stdout);
				tips_state = LOOSEN;
			}
			break;
		}
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	AdjPstFineZero
   PARAMETERS:	---
   DESCRIPTION:	Justierung der Fein-Nullposition Kolben
   RETURNS: 	CB_READY,FEHLER */
ubyte AdjPstFineZero(void){
	ubyte state,key;
	printf("\r_Einstellung der Fein-Null\r");
	fflush(stdout);
	printf("Start durch Tastedruck (ausser Abbruch-Taste)\r");
	fflush(stdout);
	SystemVarSetToDefault();
	if((state = PstInitZkMan()) != CB_READY) return state;
	if((state = ReadHeadNoAndSetDevPara()) != CB_READY) return state;
	if((state = ReadMagazineTypeAndSetTipsPara(g_pudtHeadType)) != CB_READY) return state;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		key = tast_read();
		if(key == S_KEY) break;
		if(key != NO_KEYS){
			if((state = PstReadCoarseZeroPos()) == IERR_I2C_PST) return state;
			if((state = SuckWithoutTips()) != CB_READY) return state;
			if((state = PstDrvPreZeroPos()) != CB_READY) return state;
			// Verlassen der Grobnull im Start /Stopp-Modus
			if((state = PstDrvLeavePreZeroPos()) != CB_READY) return state;
		}
	}
	while(tast_read() != NO_KEYS) WDO();
	//Kalibrierung testen
	printf("\rTest der Kalibrierung\r");
	fflush(stdout);
	printf("Start durch Tastedruck (ausser Abbruch-Taste)\r");
	fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		key = tast_read();
		if(key == S_KEY) break;
		if(key != NO_KEYS){
			if((state = SuckWithoutTips()) != CB_READY) return state;
			if((state = PstDrvCalibrate()) != CB_READY) return state;
		}
	}
	return CB_READY;
}

/* FUNCTION: 	AdjPstCoarseZeroPos
   PARAMETERS:	---
   DESCRIPTION:	Justierung der Grob-Nullposition Kolben
   RETURNS: 	CB_READY,ERR_I2C */
ubyte AdjPstCoarseZeroPos(void){
	ubyte state,old_state; bool key_hit;
	key_hit = TRUE;
	printf("Justierung der Grobnull\r"); fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		state = PstReadCoarseZeroPos();
		if(state == IERR_I2C_PST) return state;
		if(state != old_state){
			printf("Null-Lage grob:%s\r",(state == PST_POS)?is_shielded:is_opened);
			fflush(stdout);
			if(state == PST_POS) BeepOn(); else BeepOff();
			old_state = state;
		}
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	BeepOff();
	return CB_READY;
}

/* FUNCTION: 	TestHeadSwitchS1
   PARAMETERS:	---
   DESCRIPTION:	Hilfsprogramm zum Test des Schalters S1 auf Kopf-LP OL3383-71-001-02
   RETURNS: 	CB_READY */
ubyte TestHeadSwitchS1(void){
	//ubyte state
	ubyte S1;
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		//state = HeadReadSwitchS1(&S1);
		HeadReadSwitchS1(&S1);
		printf("Schalter S1:%x\r",(int)S1);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestBeep
   PARAMETERS:	---
   DESCRIPTION:	Test des Piepers
   RETURNS: 	CB_READY */
ubyte DevTestBeep(void){
	bool key_hit;
	key_hit = TRUE;
	BeepOn();
	printf("Piepton an\r");fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	BeepOff();
	printf("Piepton aus\r");fflush(stdout);
	return CB_READY;
}

/* FUNCTION: 	DevTestIn0
   PARAMETERS:	---
   DESCRIPTION:	Test des Xilinxs Input Adr. 0
   RETURNS: 	CB_READY */
ubyte DevTestIn0(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		printf("Input1: %x\r",(int)io0);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestIn2
   PARAMETERS:	---
   DESCRIPTION:	Test des Xilinxs Input Adr. 2
   RETURNS: 	CB_READY */
ubyte DevTestIn2(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		printf("Input2: %x\r",(int)io2);
		fflush(stdout);
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestIn3
   PARAMETERS:	---
   DESCRIPTION:	Test des Xilinxs Input Adr. 3
   RETURNS: 	CB_READY */
ubyte DevTestIn3(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		printf("Input3: %x\r",(int)(io3&0x03));
		fflush(stdout);			// obsolet XS10/2,4 - in (frei)
		Wait(CYCLE_TIME);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

/* FUNCTION: 	DevTestOut7
   PARAMETERS:	---
   DESCRIPTION:	Test des Xilinxs Output Adr. 7 (Achtung,nur für Inbetriebnahme ohne Peri !!! )
   RETURNS: 	CB_READY */
ubyte DevTestOut7(void){
	bool key_hit;
	key_hit = TRUE;
	printf("Entfernen des Steckers XS17 /Remove connector XS17... \r"); fflush(stdout);
	io7 = 0xaa;					// OUT7[0,1,2]	BLP U11,XS17		- Lifter DC-Motor = 0b010	(STOP)
													// OUT7[3,4,5]	BLP XS4/2,3,4		- out (frei)	  = 0b101
	printf("Output7 (Bit7-0): 1010 1010\r");
	fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	key_hit = TRUE;
	io7 = 0x55;					// OUT7[0,1,2]	BLP U11,XS17		- Lifter DC-Motor = 0b101	(Hoch)
								// OUT7[3,4,5]	BLP XS4/2,3,4		- out (frei)	  = 0b010
	printf("Output7(Bit7-0): 0101 0101\r"); fflush(stdout);
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(!RequestSKey()) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	io7 = 0x00;					// OUT7[0,1,2]	BLP U11,XS17		- Lifter DC-Motor = 0		(STOP)
								// OUT7[3,4,5]	BLP XS4/2,3,4		- out (frei)	  = 0
	return CB_READY;
}

/* FUNCTION: 	PrgSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Test des gesamten CyBi-Well (Progr. wurde auch zum EMV-Test verwendet)
   RETURNS: 	CB_READY */
void SelfTest(void){
	ubyte i,/* wagen_pos,*/ state; unsigned int j,counter; char help[15];
	const ubyte *pStartPos,*pPos;
	g_bShowSpots384 = TRUE;
	/*wagen_pos = 1;*/
	pStartPos = g_pudtHrzPosTabPtr->CarPara->ManLoc->Location;
	pPos = pStartPos;
	ClearAllAndDisplay(str_dev_test);
	counter = DeviceCycleNo;
	Display(str_cycle_no);
	state = VertDrvWay(0); if(state == ERR_PROG_ABORT) return; ErrorDisplay(state);
	state = PstDrvCalibrate();
	if(state == ERR_PROG_ABORT) return; else ErrorDisplay(state);
	while(1){
		for(j = 1; j<= counter; j++){
			snprintf(help, 15,"%u %s %u",j,str_of[g_ubytLang].message,counter);
			LcdShow(2,strlen(str_cycle_no[g_ubytLang].message)+2,help);
			// Test Wagen
			if(g_pudtHrzPosTabPtr->cartype != CARR_0){
				state = CarDrvNo(*pPos);
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				if(pPos < pStartPos+g_pudtHrzPosTabPtr->CarPara->ManLoc->Len-1)
					++pPos;
				else
					pPos = pStartPos;
			}
			// Test Volumenhandling
			ShowLocation(TRUE);
			state = VertDrvWay(50); if(state == ERR_PROG_ABORT) return;
			ErrorDisplay(state);
			state = SuckWithOverstroke(g_pudtTipsPara->MaxVol);
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
			for(i = 0; i<10; i++){
				state = Dispense(g_pudtTipsPara->MaxVol/10,TRUE);
				if(state == ERR_PROG_ABORT)
					return;
				else
					ErrorDisplay(state);
			}
			state = Dispense(g_pudtTipsPara->MoreDisp,TRUE);
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
			state = PstDrvCalibrate();	//falls Taste zu lang gedrückt wird,wird auch diese Routine übersprungen
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
			// Test XY-Komponenten
			if(j%2){
				for(i = 0; i<5; i++) XYNextManPos(1);
				state = XYServoPos(0);
				if(state == ERR_PROG_ABORT)
					return;
				ErrorDisplay(state);
			}
			// Test Ausheber und Gripper (falls angeschlossen)
			for(i = 0; i<2; i++){
				state = VertDrvWay(50);
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = VertDrvWay(0);
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = GripperSelfTest();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
			}
			state = VertDrvWay(0);
			if(state == ERR_PROG_ABORT) return;
			ErrorDisplay(state);
			// Spitzenwechseltest
			if((j%20) == 0){
				state = SuckWithoutTips();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = TipsLoose();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = TipsTighten();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = PstDrvCalibrate();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
			}
		}
		counter = DeviceCycleRepeat;
		Display(str_xy_go_esc);
		if(KeyWaitForAny() == ENTER_KEY){
			ClearStringOnDisplay(str_xy_go_esc);
			continue;
		}else
			break;
	}
}

/* FUNCTION: 	HeadSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Dauertest des Kopfes
   RETURNS: 	--- */
void HeadSelfTest(void){
	ubyte i,state; unsigned int j,counter; char help[10];
	ClearAllAndDisplay(str_head_test);
#if(HEAD_TEST_WITHOUT_MAGAZINE)
	counter = 50000;
#else
	counter = 10000;	// 10000 DeviceCycleNo;
#endif
	Display(str_cycle_no);
	state = PstDrvCalibrate();
	if(state == ERR_PROG_ABORT)
		return;
	else
		ErrorDisplay(state);
	while(1){
		for(j = 1; j<= counter; j++){
			sprintf(help,"%u %s %u",j,str_of[g_ubytLang].message,counter);
			LcdShow(2,strlen(str_cycle_no[g_ubytLang].message)+2,help);
			// Test Volumenhandling
			state = SuckWithOverstroke((g_pudtTipsPara->MaxVol)*0.8);		//0.8
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
			for(i = 0; i<5; i++){
				state = Dispense((g_pudtTipsPara->MaxVol) *0.8/5,TRUE);	// 0.7 für Kopftest 40 µl
				if(state == ERR_PROG_ABORT)
					return;
				else
					ErrorDisplay(state);
				Wait(100);
			}
			state = Dispense(g_pudtTipsPara->MoreDisp,TRUE);
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
			Wait(100);
			state = PstDrvCalibrate();
			if(state == ERR_PROG_ABORT)
				return;
			else
				ErrorDisplay(state);
#if(HEAD_TEST_WITHOUT_MAGAZINE)
			// Spitzenwechseltest
			if((j%20) == 0){
				state = SuckWithoutTips();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = TipsLoose();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = TipsTighten();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
				state = PstDrvCalibrate();
				if(state == ERR_PROG_ABORT) return;
				ErrorDisplay(state);
			}
#endif
		}
		counter = DeviceCycleRepeat;
		Display(str_xy_go_esc);
		if(KeyWaitForAny() == ENTER_KEY){
			ClearStringOnDisplay(str_xy_go_esc);
			continue;
		}else
			break;
	}
}

/* FUNCTION: 	DevAdjustHeadLift
   PARAMETERS:	---
   DESCRIPTION:	Test und Justierung des Kopfaushebersystems
   RETURNS: 	CB_READY / Fehlermeldung */
ubyte DevAdjustHeadLift(void){
	ubyte error,DispConnected,DispNotConnected,break_flg;
	struct LCD_DISP dHelp;
	DispConnected = FALSE;
	DispNotConnected = FALSE;
	break_flg = FALSE;
	SaveDisplay(&dHelp);
	ClearAllAndDisplay(str_adjust_lift);
	do{
		ClearLineAndDisplay(str_lift_to_upper);
		error = HeadMoveCtrl(HeadDrvToUpperPos);
		if(error == CB_READY){
			ClearLineAndDisplay(str_go_on);
			while(tast_read() != ENTER_KEY){
				WDO();
				if(tast_read() == S_KEY){
					error = IERR_PROG_ABORT;
					break;}
			}
			if(error == CB_READY){
				LcdClear(SECOND_LINE);
				ClearLineAndDisplay(str_lift_to_bottom);
				error = HeadMoveCtrl(HeadDrvToLowerPos);
				if(error == CB_READY){
					ClearLineAndDisplay(str_go_on);
					while(tast_read() != ENTER_KEY){
						WDO();
						if(tast_read() == S_KEY){
							break_flg = TRUE;
							break;
						}
						if(HeadConnectionPin == 0){
							if(!DispConnected){
								ClearLineAndDisplay(str_disp_connected);
								Display(str_go_on);
								DispConnected = TRUE;
								DispNotConnected = FALSE;}
						}else{
							if(!DispNotConnected){
								ClearLineAndDisplay(str_disp_not_connected);
								Display(str_go_on);
								DispConnected = FALSE;
								DispNotConnected = TRUE;
							}
						}
					}
					LcdClear(SECOND_LINE);
					DispConnected = FALSE;
					DispNotConnected = FALSE;
					if(break_flg ==  TRUE) break;
				}
			}
		}
	} while(error == CB_READY);
	RestoreDisplay(&dHelp);
	return error;
}

/* FUNCTION: 	DevTestHeadLift
   PARAMETERS:	---
   DESCRIPTION:	Dauertest des Kopfaushebers
   RETURNS: 	Fehlerkode	*/
ubyte DevTestHeadLift(void){
	ubyte error;
	do{
		error = TestHeadLiftOneCycle();
	} while(error == CB_READY);
	return error;
}

/* FUNCTION: 	TestHeadLiftOneCycle
   PARAMETERS:	---
   DESCRIPTION:	Einen Kopfausheberzyklus fahren
   RETURNS: 	Fehlerkode */
ubyte TestHeadLiftOneCycle(void){
	ubyte error;
	error = HeadDrvToUpperPos();
	if(error == CB_READY){
		Wait(200);
		error = HeadDrvToLowerPos();
		if(error == CB_READY){
			Wait(200);
		}
	}
	return error;
}

/* FUNCTION: 	PrgLifterSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Selbsttest des Kopfaushebers
   RETURNS: 	--- */
void LifterSelfTest(void){
	ubyte state;
	unsigned int i,counter;
	char help[10];
	g_bShowSpots384 = FALSE;
	counter = LifterCycleNo;
	ClearAllAndDisplay(str_lifter_test);
	if(!IsHeadLowerPos()){
		PrgLifterDrvToUpperPos();
		state = HeadRemove(FALSE);
		if(state != CB_READY) return;
	}else{
		if(!IsChangeToolInsert()){
			state = RemoveTipsMagAndInsertHeadChangeTool();
			if(state != CB_READY) return;
		}
	}
	ClearLineAndDisplay(str_cycle_no);
	while(1){
		for(i = 1; i<= counter; i++){
			if(i != 1) Wait(600);
			sprintf(help,"%u %s %u",i,str_of[g_ubytLang].message,counter);
			LcdShow(2,strlen(str_cycle_no[g_ubytLang].message)+2,help);
			state = HeadMoveCtrl(HeadDrvToUpperPos);
			if(state == IERR_PROG_ABORT) break;
			if(state != CB_READY) ErrorDisplay(state);
			Wait(200);
			state = HeadMoveCtrl(HeadDrvToLowerPos);
			if(state == IERR_PROG_ABORT) break;
			if(state != CB_READY) ErrorDisplay(state);
		}
		counter = LifterCycleRepeat;
		Display(str_xy_go_esc);
		if(KeyWaitForAny() == ENTER_KEY){
			ClearStringOnDisplay(str_xy_go_esc);
			continue;
		}else
			break;
	}
	SytemInitialization();
}

/* FUNCTION: 	PrgSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Auswahl der geräteinternen Dauertests
   RETURNS: 	--- */
void PrgSelfTest(void){
	ubyte id;
	while(1){
		ClearAllAndDisplay(str_selftest);
		if(g_uiDeviceType == 3714){
			id = ProgModification(selftests3714,sizeof(selftests3714)/sizeof(struct Modus),0,SECOND_LINE);
			if(selftests3714[id].fp == ProgExit) break;
			selftests3714[id].fp();
		}else{
			id = ProgModification(selftests,sizeof(selftests)/sizeof(struct Modus),0,SECOND_LINE);
			if(selftests[id].fp == ProgExit) break;
			selftests[id].fp();
		}
	}
}

/* FUNCTION: 	DevTestHeadContactX20
   PARAMETERS:	---
   DESCRIPTION:	Test auf Kontaktgabe des eingesetzten Kopfes
   RETURNS: 	CB_READY */
ubyte DevTestHeadContactX20(void){
	bool key_hit;
	key_hit = TRUE;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(HeadConnectionPin == 1)// kein Kopf eingesetzt
			printf("Kopf hat keinen Kontakt\r");
		else
			printf("Kopf hat Kontakt\r");
		fflush(stdout);
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	return CB_READY;
}

ubyte PtmeTimeTest(void){
	ubyte state = CB_READY;
	unsigned int iCounter	 = 0;
	unsigned short usMode = PTME_CyBiWellPos;
	char Ready = 0;

	if(!IsPresMechConfig()) return IERR_PMTE_NOT_CONFIG;
	state = CmdTipsLoose();
	if(state != CB_READY) return state;

	ClearAllAndDisplay(str_dev_test);
	while(!ExceptionCtrl()){
		Ready = 0;
		iCounter	 = 0;

		if(IsPresMechCyBiWellPos()){
			MagVentilPresentPos;	// Magnetventil schalten
			usMode = PTME_PresentPos;
		}else if(IsPresMechPresentPos()){
			MagVentilCyBiWellPos;	// Magnetventil schalten
			usMode = PTME_CyBiWellPos;
		}
		do{
			Wait(1);	// 10ms
			iCounter++;
			if((usMode == PTME_PresentPos) && (IsPresMechPresentPos())) Ready = 1;
			if((usMode == PTME_CyBiWellPos) && (IsPresMechCyBiWellPos())) Ready = 1;
		} while (Ready == 0);

		printf("I%u\r",(unsigned int)iCounter*10);
		fflush(stdout);
		Wait(100);
	}
	CmdTipsTighten();
	return CB_READY;
}

/* FUNCTION: 	TestOL3387_Valve
   PARAMETERS:	---
   DESCRIPTION:	Test des Magnetventiles am Hummingwellkopf,
				Achtung,Hardwarekonfiguration an OL3383-71-001 / XS4 wird geändert per Firmware!
   RETURNS: 	CB_READY / Error*/
ubyte TestOL3387_Valve(void){
	bool key_hit;
	ubyte state;
	state = ReadHeadNoAndSetDevPara();
	if(state != CB_READY) return state;	// Fehler beim Lesen der Kopfnummer
	if(g_pudtHeadType->type != OL3387_8) return IERR_COM_NOT_PRACTICABLE;	// kein Hummingwell-Kopf,Kommando nicht ausführbar
	key_hit = TRUE;
	WDO();
	printf("\rMagnetventil ein\r");
	fflush(stdout);
	state = OL3387_ValveOnOff(VALVE_ON);
	if(state != CB_READY) return state;
	while(!ExceptionCtrl()){	//Abbruch per Host
		WDO();
		if(KeyScan() == NO_KEYS) key_hit = FALSE;
		if((key_hit == FALSE)&&(RequestSKey())) break;	//erst nach losgelassener Taste wirksam
	}
	key_hit = TRUE;
	printf("Magnetventil aus");
	fflush(stdout);
	state = OL3387_ValveOnOff(!VALVE_ON);
	if(state != CB_READY) return state;
	return CB_READY;
}
