/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- BASE
// ---------- Inc:
#include <_i2c_fram.h>
// ---------- Master/C167CS
// ---------- Master
#include "ch_ig_cmd.h"
#include "ch_ig_eeprom.h"
#include "ch_ig_err.h"
#include "ch_ig_head.h"
#include "ch_ig_man.h"
#include "ch_ig_peri.h"
#include "ch_ig_piston.h"
#include "ch_ig_ptme.h"
#include "ch_ig_spw.h"
#include "ch_ig_xy.h"
#include "ch_ig_z.h"
#include "lcd.h"
// ---------- Pipettor_Allgemein/VolumenKorrektur
// ---------- Pipettor_Allgemein
#include "Well_Defines.h"
#include "Well_Error.h"

/** local definitions **/
 #define TIMEOUT_TIPS_HANDLING 1200		/*  1200*10ms = 12s */
 #define ASPW_IDENT 13	//Kennung

/** default settings **/

/** external data **/
 extern DispString_t str_go_on[];
 extern DispString_t str_ls_interrupted[];
 extern DispString_t str_ls_release[];
 extern DispString_t str_PresMechPresentPos[];

 extern uword g_uwWaitTime;

/** internal functions **/

/** public data **/
 ubyte g_ubytASPW;
 ubyte g_ubytFlgTCMAXWay = FALSE;
 ubyte g_ubytMagNotInsertAfterHeadExchange;

/** private data **/
DispString_t str_big_ex_tips[]				 = 	{{1,1,"SPITZENWECHSEL"}
												,{1,1,"CHANGING TIPS"}};
DispString_t str_tips_detach[]				 = 	{{2,1,"Spitzen l\357sen"}
												,{2,1,"Loosen tips"}};
DispString_t str_head_tool_detach[]			 = 	{{2,1,"Magazinadapter l\357sen"}
												,{2,1,"Loosen tip magazine adapter"}};
DispString_t str_magazin_remove[]			 = 	{{2,1,"Magazin entfernen"}
												,{2,1,"Remove tip rack"}};
DispString_t str_magazin_intro[]			 = 	{{2,1,"Magazin einlegen (mit Spitzen)"}
												,{2,1,"Insert tip rack (with tips only)"}};
DispString_t str_tips_tighten[]				 = 	{{2,1,"Spitzen anziehen"}
												,{2,1,"Tighten tips"}};
DispString_t str_wrong_magazine[]			 = 	{{2,1,"Falsches Magazin eingesetzt"}
												,{2,1,"Wrong magazine inserted"}};
/*
DispString_t str_PresMechMoveInMagazine[]	 = 	{{2,1,"Einfahren mit Magazin"}
															,{2,1,"Move in with magazine"}};
*/

ubyte CmdTipsLoose(void)	// Aufruf vom Host
{ubyte state;
  VertDrvWayXYCtrl(0);// Fehler auswerten
  if((state = SuckWithoutTips()) != CB_READY) return state;
  state = TipsLoose();
  if(state == CB_READY) {
	if(IsPresMechConfig())	// Wenn Präsentiermechanik Konfiguriert...
		state = MovePresMechPresentPos();	// ... Tip Magazin ausschieben
  }
  return state;
}

/*Anmerkungen zum Handeln der Spitzenparameter beim Betrieb mit ASPW
  - Befehl "Spitzen anziehen" wird vom ASPW mit einer zusätzlichen Kennung gesendet
	daraufhin wird ein globales Flag (g_ubytASPW) gesetzt. Falls der Befehl nicht vom ASPW kommt
	wird das Flag gelöscht,auch die Kopie im EEPROM.
  - Mit jedem Anziehen der Spitzen werden auch die dazugehörigen Magazinparameter eingelesen,
	gleich ob sie gültig oder falsch sind.
  - Mit dem Befehl Magazinparameter setzen wird das gesetzte Flag g_ubytASPW und die Magazinparameter
	im EEPROM abgelegt,anderenfalls eine Fehlermeldung generiert.
  - Gerät wird mit angezogenem Magazin (vom ASPW) ausgeschalten. Beim Einschalten wird die Kopie
	des Flags im EEPROM getestet und gegebenenfalls die Magazinparameter ausgelesen,
	ansonsten werden die Magazinparameter durch den Kopf selbst festgelegt!
*/
ubyte CmdTipsTighten(void)   /* Aufruf vom Host,falls Parameter (13) vorhanden,dann ASPW */
{ubyte state;
  if(!*g_pchCmdBuf)	//kein Parameter vorhanden --> Befehl nicht vom ASPW
  {if((state = ASPWCtrlReset()) != CB_READY) return state; }
  else {if(atoi(g_pchCmdBuf) == ASPW_IDENT) g_ubytASPW = TRUE; }
	//noch keine Auswertung,evtl. im EEPROM Speichern

  state = PstReadEccentricState();
  if(state == PST_ECCENTRIC_TIGHTEN) return CB_READY;
  if(state == IERR_I2C_ECC) return state;
  if(IsPresMechConfig()) {// Wenn Präsentiermechanik Konfiguriert...
	state = MovePresMechCyBiWell(); 	// ... Tip Magazin einschieben...
	if(state != CB_READY) return state;
	Wait(20);	// ... ein wenig warten,das das Tip Magazin erkannt wird
  }
  if((state = TipsTighten()) != CB_READY) return state;
  if(IsChangeToolInsert()) return IERR_MAG_NOT_INSERT;	//Kopfwechseltool wurde eingeschoben
  if(g_pudtHeadType->type == OL3387_8)
  {
	 state = ReadAndSetCartridgeTyp();
	 if(state != CB_READY) return state;
	 SetPlateTypeMode(g_pudtHeadType);
  }
  else
  {
	 state = ReadMagazineTypeAndSetTipsPara(g_pudtHeadType); if(state != CB_READY) return state;
  }
  return PstDrvCalibrate();
}

/* FUNCTION: 	ProgManTipsChange
   PARAMETERS:	---
   DESCRIPTION:	Wechseln des Magazines (Spitzen) im manuellen Mode
   RETURNS: 	--- */
void ProgManTipsChange(void)
{
  ErrorDisplay(VertDrvWayXYCtrl(0)); ShowLocation(TRUE);
  SpwManTipsChange(str_big_ex_tips,str_magazin_remove,FALSE);
}

/* FUNCTION: 	SpwManTipsChange
   PARAMETERS:	ZK-Poiner HeadLine und Remove
   DESCRIPTION:	Wechseln des Magazines bzw. Wechseltool im manuellen Mode
   RETURNS: 	--- */
void SpwManTipsChange(DispString_t *head_line, DispString_t *remove, ubyte magazine_not_insert)
{
	unsigned char state;
	unsigned int ls_cnt;
	struct LCD_DISP dHelp;
	DispString_t *str;
	state = CB_READY;
	ls_cnt = LS_WATCH_CYCLES; // Lichtvorhangüberwachung
	SaveDisplay(&dHelp);	  // lcd_disp_save(disp_buffer);
	if (IsChangeToolInsert())
		str = str_head_tool_detach;
	else
		str = str_tips_detach;
	while (1)
	{
		ClearAllAndDisplay(head_line);
		Display(str);
		ErrorDisplay(SuckWithoutTips());
		while (1)
		{
			ClearLineAndDisplay(str);
			ErrorDisplay(TipsLoose());
			if (IsPresMechConfig())
			{														  // Wenn Präsentiermechanik Konfiguriert...
				UserMovePresMechToPresentPos(str_PresMechPresentPos); // ... Tip Magazin ausschieben
			}
			ClearLineAndDisplay(remove);
			while (1)
			{ // Warten auf das Entfernen des Magazins
				WDO();
				if (!IsSpwMagazineInsert())
					break;
			}

			if (magazine_not_insert)
				break; // kein neues Magazin einsetzen

			while (1)
			{
				if (IsPresMechConfig())
				{ // Wenn Präsentiermechanik Konfiguriert...
					// ClearLineAndDisplay(str_magazin_intro);DisplayAndEnter(str_go_on);
					while (1)
					{ // Warten auf das Einschieben des Magazins
						WDO();
						UserMovePresMechToCyBiWell(str_magazin_intro);
						// state = UserMovePresMechToCyBiWell(); 	// ... Tip Magazin einschieben...
						/*if(state != CB_READY) {
							ErrorDisplay(state);
							break;
						}	*/
						Wait(20); // ... ein wenig warten,das das Tip Magazin erkannt wird
						if ((ls_cnt != 0) && (IsLSInterrupted()))
							ls_cnt -= 1;
						if (IsSpwMagazineInsert())
						{
							if (IsChangeToolInsert())
							{
								UserMovePresMechToPresentPos(str_PresMechPresentPos);
								continue; // Kopfwechseltool nicht erlaubt
							}
							else
								break;
						}
						else
							UserMovePresMechToPresentPos(str_PresMechPresentPos); // ... Tip Magazin ausschieben
					}

					if (g_ubytSafetyHead == WORKING_WITH_LS)
					{
						if (ls_cnt != 0)
							ErrorDisplay(IERR_LS_FAULTY);
					}
					ClearLineAndDisplay(str_tips_tighten);
					if (IsChangeToolInsert())
						continue; // Kopfwechseltool nicht erlaubt
					else
						break;
				}
				else
				{
					ClearLineAndDisplay(str_magazin_intro);
					while (1)
					{ // Warten auf das Einschieben des Magazins
						WDO();
						if ((ls_cnt != 0) && (IsLSInterrupted()))
							ls_cnt -= 1;
						if (IsSpwMagazineInsert())
						{
							if (IsChangeToolInsert())
								continue; // Kopfwechseltool nicht erlaubt
							else
								break;
						}
					}
					if (g_ubytSafetyHead == WORKING_WITH_LS)
					{
						if (ls_cnt != 0)
							ErrorDisplay(IERR_LS_FAULTY);
					}
					ClearLineAndDisplay(str_tips_tighten);
					DisplayAndEnter(str_go_on);
					if (IsChangeToolInsert())
						continue; // Kopfwechseltool nicht erlaubt
					else
						break;
				}
			}
			while (1)
			{
				state = TipsTighten();
				if (state == IERR_LS_INTERRUPTED)
				{
					WarningDisplay(str_ls_interrupted, str_ls_release);
				}
				else
					break;
			}
			if (state == CB_READY)
			{
				if (IsChangeToolInsert())
					continue; /*Kopfwechseltool nicht erlaubt*/
				else
					break;
			}
			if ((state == IERR_HEAD_ECCENTRIC) || (state == IERR_I2C_ECC))
				ErrorDisplay(state);
		}

		if (magazine_not_insert)
			break; // kein neues Magazin einsetzen

		// Magazin eingesetzt und angezogen
		ASPWCtrlReset();
		if (g_pudtHeadType->type == OL3387_8)
		{
			ErrorDisplay(ReadAndSetCartridgeTyp());
			SetPlateTypeMode(g_pudtHeadType);
		}
		else
		{
			ErrorDisplay(ReadMagazineTypeAndSetTipsPara(g_pudtHeadType));
		}
		// if((state = PstDrvCalibrate()) != IERR_PROG_ABORT) break;
		state = PstDrvCalibrate();
		if (state == IERR_PROG_ABORT)
			continue;
		if (state != IERR_PST_ZERO_POS)
			break;
		else
		{
			ClearLineAndDisplay(str_wrong_magazine);
			DisplayAndEnter(str_go_on);
		}
	}
	ErrorDisplay(state); // evtl. sind noch Fehler von PstDrvCalibrate anhängig
	RestoreDisplay(&dHelp);

	if ((g_ubytSafetyHead == WORKING_WITH_BLIND) && (g_ubytShieldWatch == WORKING_WITH_SHIELD))
	{
		// Sicherheitsabfrage überwachung Magazinabdeckung offen
		if (SecurityCheckIsShieldOpen() != CB_READY)
			ErrorDisplay(IERR_SHIELD_MONITORING);
	}
	return;
}

/* FUNCTION: 	TipsTighten
   PARAMETERS:	---
   DESCRIPTION:	Anziehen der Spitzen
   RETURNS: 	CB_READY / Fehlercode */
ubyte TipsTighten(void)   /* Anziehen */
{ubyte state;
  state = PstReadEccentricState();
  if(state == PST_ECCENTRIC_TIGHTEN) {DCMotorStop(); return CB_READY;}
  if(state == IERR_I2C_ECC){DCMotorStop(); return state;}
  DCMotorOnClockwise();
  g_uwWaitTime = TIMEOUT_TIPS_HANDLING;	/* Generieren der Zeitüberwachung */
  while(g_uwWaitTime)
  {WDO();
	/*Spitzenmagazinüberwachung*/
	state = PstReadMagazineInsert();
	if(state == PST_MAG_NOT_AVAILABLE){DCMotorStop(); return IERR_MAG_NOT_INSERT;}
	if(state == IERR_I2C_HEAD_MAG) {DCMotorStop(); return state;}
	if((g_ubytSafetyHead == WORKING_WITH_LS)&&(IsLSInterrupted()))	{DCMotorStop(); return IERR_LS_INTERRUPTED;}

	if(ReadAbortConditions()){DCMotorStop(); return IERR_PROG_ABORT;}
	state = PstReadEccentricState();
	if(state == PST_ECCENTRIC_TIGHTEN) {Wait(10); DCMotorStop(); return CB_READY;}
	if(state == IERR_I2C_ECC){DCMotorStop(); return state;}
  }
  DCMotorStop();
  return IERR_HEAD_ECCENTRIC;
}

/* FUNCTION: 	TipsLoose
   PARAMETERS:	---
   DESCRIPTION:	Lösen der Spitzen
   RETURNS: 	CB_READY / Fehlercode */
ubyte TipsLoose(void)  /* Lösen */
{ubyte state;
  state = PstReadEccentricState();
  if(state == PST_ECCENTRIC_LOOSE){DCMotorStop(); return CB_READY;}
  if(state == IERR_I2C_ECC){DCMotorStop(); return state;}
  /* Auswählen der Drehrichtung */
  if(I2CRead(0x20,1,&state) == FALSE) return IERR_I2C_ECC;
  if((state&0xc0) == 0x40) DCMotorOnCounterwise(); else DCMotorOnClockwise();
  g_uwWaitTime = TIMEOUT_TIPS_HANDLING;	/* Generieren der Zeitüberwachung */
  /* Spitzen lose,beide Signale haben Low-Pegel*/
  while(g_uwWaitTime)
  {WDO();
	state = PstReadEccentricState();
	if(state == PST_ECCENTRIC_LOOSE)
	{
	 	state = CB_READY;
	 	DCMotorStop();
		if((g_pudtHeadType->type == OL5021_1) || (g_pudtHeadType->type == OL5021_12))
		{
		   state = PstDrvWay(2.50);	//Spitzen 2,5mm nach oben fahren
		}
	 	return state;
	}
	if(state == IERR_I2C_ECC){DCMotorStop(); return state;}
  }
  DCMotorStop();
  return IERR_HEAD_ECCENTRIC;
}

ubyte IsSpwMagazineInsert(void){
	ubyte state;
	state = PstReadMagazineInsert();
	if(state == IERR_I2C_HEAD_MAG) ErrorDisplay(state);
	if(state == PST_MAG_AVAILABLE) return TRUE;
	return FALSE;
}

/** private functions **/

void DCMotorOnClockwise(void)	//dc_mot_clockwise
{ubyte ctrl;
	// Achtung,nur bit 0 und 1 verwenden,der Rest sind Eingänge,die high bleiben müssen
  ctrl = 0xFE;
  I2CWrite(0x20,1,&ctrl);	// Motor einschalten
}

void DCMotorOnCounterwise(void)
{ubyte ctrl;
	// Achtung,nur bit 0 und 1 verwenden,der Rest sind Eingänge,die high bleiben müssen
  ctrl = 0xFD;;	// Motor einschalten
  I2CWrite(0x20,1,&ctrl);	// Motor einschalten
}

void DCMotorStop(void)
{ubyte ctrl;
	// Achtung,nur bit 0 und 1 verwenden,der Rest sind Eingänge,die high bleiben müssen
  ctrl = 0xFF;
  I2CWrite(0x20,1,&ctrl);	// Motor ausschalten
}


/* FUNCTION: 	IsMagazineTighten
   PARAMETERS:	---
   DESCRIPTION:	Abfrage Spitzen angezogen?
   RETURNS: 	TRUE / FALSE */
ubyte IsMagazineTighten(void)
{ubyte state;
  state = PstReadEccentricState();
  if(state == IERR_I2C_ECC) ErrorDisplay(state);
  if(state == PST_ECCENTRIC_TIGHTEN) return TRUE;
  return FALSE;
}

ubyte ASPWCtrlReset(void)
{
  g_ubytASPW = FALSE;
  return EEPROMASPWFlagCtrl(FALSE);
}
