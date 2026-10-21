/*******************************************************************
 *
 *	DESCRIPTION:	Kopfwechsel
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include "ch_ig_head.h"
#include "ch_ig_err.h"
#include "ch_ig_cmd.h"
#include "ig_define.h"
#include "ch_ig_spw.h"
#include "lcd.h"
#include "ch_ig_config.h"
#include "ch_ig_key.h"
#include "ch_ig_piston.h"
#include "ch_ig_z.h"
#include "ch_igel.h"
#include "ch_ig_xy.h"
#include "ch_ig_peri.h"
#include "ch_head_fram.h"
#include "ch_ig_eeprom.h"
#include "Well_Error.h"
#include "ch_ig_ptme.h"
#include "ch_ig_hrz.h"

/** local definitions **/

 #define HEAD_MOVE_TIMEOUT 10000		/*  10000*10ms = 100s */
 #define SEC_COVER_OPEN_CYCLES 25
/** default settings **/

 void ProgHeadFlagInsert(void);
 void ProgHeadFlagRemove(void);

 extern bool HeadInterruptEnabled;

// ubyte HeadInsertKeyRequest(void);
 ubyte HeadInsertAndWaitForEnter(void);
 ubyte HeadChangeUserAbort(ubyte id);

/** external data **/
 extern DispString_t str_err_head_unknown[];
 extern DispString_t str_carriage_init[];
 extern DispString_t str_magazin_remove[];
 extern DispString_t str_tips_detach[];
 extern DispString_t str_go_on[];

 extern uword g_uwWaitTime;

/** public data **/
 ubyte g_ubytSafetyHead;	// Flag für die Art der Sicherheitsüberwachung beim Kopfwechsel
 ubyte g_ubytShieldWatch;


/** private data **/
DispString_t str_big_ex_head[] 			 = 	{{1,1,"KOPFWECHSEL"}
											,{1,1,"CHANGING HEAD"}};
DispString_t str_cover_open[]  			 = 	{{1,1,"KOPFWECHSEL - Klappe offen"}
										  	,{1,1,"CHANGING HEAD - cover is open"}};
DispString_t str_cover_open1[]  		 = 	{{1,1,"Kopfhaube offen"}
										  	,{1,1,"Cover is open"}};
DispString_t str_cover_shut[]			 = 	{{2,1,"Bitte Kopfhaube schlie\342en"}
  											,{2,1,"Shut the cover"}};
DispString_t str_blind_open[]			 = 	{{1,1,"KOPFWECHSEL - Jalousie offen"}
											,{1,1,"CHANGING HEAD - blind is open"}};
DispString_t str_door_open[]			 = 	{{1,1,"KOPFWECHSEL - T\365r offen"}
											,{1,1,"CHANGING HEAD - door is open"}};
DispString_t str_blind_open1[]			 = 	{{2,1,"Jalousie nach hinten schieben"}
											,{2,1,"Push back the blind"}};

DispString_t str_blind_shut[]			 = 	{{2,1,"Bitte Jalousie schlie\342en"}
											,{2,1,"Shut the blind"}};
DispString_t str_door_shut[]			 = 	{{2,1,"Bitte alle T\365ren schlie\342en"}
											,{2,1,"Shut all doors"}};
DispString_t str_ls_interrupted[]		 = 	{{1,1,"Lichtvorhang ist unterbrochen"}
											,{1,1,"Light curtain is interrupted"}};
DispString_t str_ls_release[]			 = 	{{2,1,"Bitte Lichtvorhang freigeben"}
											,{2,1,"Release light curtain"}};

DispString_t str_head_tool_intro[]		 = 	{{2,1,"Magazinadapter einsetzen"}
											,{2,1,"Insert the tip magazine adapter"}};
DispString_t str_head_tool_tighten[]	 = 	{{2,1,"Magazinadapter anziehen"}
											,{2,1,"Tighten the tip magazine adapter"}};
DispString_t str_head_tool_loose[]		 = 	{{2,1,"Magazinadapter l\357sen"}
											,{2,1,"Loose the tip magazine adapter"}};
DispString_t str_head_remove[]			 = 	{{2,1,"Bitte Kopf entnehmen"}
 											,{2,1,"Remove head"}};
DispString_t str_head_insert[]			 = 	{{2,1,"Bitte Kopf einsetzen"}
 											,{2,1,"Insert head"}};

DispString_t str_first_head_insert[]	 = 	{{1,1,"Kopf einschieben,Kopfwechselwerkzeug"}
 											,{1,1,"Insert head,remove head changing tool"}};
DispString_t str_head_remove_handle[]	 = 	{{2,1,"entfernen,Klappe schlie\342en"}
 											,{2,1,"and close the cover"}};
DispString_t str_head_remove_tool[]		 = 	{{2,1,"Bitte Magazinadapter entfernen"}
 											,{2,1,"Remove the tip magazine adapter"}};
DispString_t str_wait_head_remove[]		 = 	{{2,1,"Bitte warten - Kopf wird bereitgestellt"}
											,{2,1,"Wait - head is getting ready"}};
DispString_t str_wait_head_insert[]		 = 	{{2,1,"Bitte warten - Kopf wird eingefahren"}
											,{2,1,"Wait - head is moving in"}};

DispString_t str_head_insert_again[]	 = 	{{1,1,"KOPFWECHSEL - Kopf nicht in Endposition"}
											,{1,1,"CHANGING HEAD - head not in end position"}};
DispString_t str_head_insert_hint[]		 = 	{{2,1,"Kopf nach hinten schieben"}
											,{2,1,"Push back the head"}};
DispString_t str_head_not_end_pos[]		 = 	{{1,1,"Kopf nicht in Endposition"}
											,{1,1,"Head not in end position"}};
DispString_t str_head_remove_insert[]	 = 	{{2,1,"Bitte Kopf entnehmen/einsetzen"}
											,{2,1,"Remove/Insert head"}};
DispString_t str_head_user_abort[]		 = 	{{1,1,"KOPFWECHSEL - Bedienerabbruch"}
											,{1,1,"CHANGING HEAD - abort by user"}};
DispString_t str_head_go_on[]  			 = 	{{2,1,"weiter mit:"}
											,{2,1,"go on with:"}};
DispString_t str_head_go_on_insert[]	 = 	{{2,13,"Einsetzen"}
											,{2,13,"Insert"}};
DispString_t str_head_go_on_remove[]	 = 	{{2,23,"Entnahme"}
											,{2,20,"Remove"}};
DispString_t str_head_break[]			 = 	{{1,1,"Pause"}
											,{1,1,"Break"}};
DispString_t str_head_dummy[]			 = 	{{2,1," "}
											,{2,1," "}};
DispString_t str_PresMechCyBiWell[]		 = 	{{2,1,"Einfahren Magazin-Wechser"}		// unused?
											,{2,1,"Move in magazine changer"}};
DispString_t str_PresMechPresentPos[]	 = 	{{2,1,"Ausfahren Magazinwechsler"}
											,{2,1,"Move out magazine changer"}};
DispString_t str_PresMechRemoveRack[]	 = 	{{2,1,"Magazin entfernen und einfahren"}
											,{2,1,"Remove tip rack and move in"}};

 #define HEAD_INSERT 0	//Position in m_udtHeadInsertRemove
 #define HEAD_REMOVE 1
 struct Modus m_udtHeadInsertRemove[]		 = 	{{str_head_go_on_insert,ProgHeadFlagInsert}
 												,{str_head_go_on_remove,ProgHeadFlagRemove}};
/* FUNCTION: 	HeadInsertAndDrvDown
   PARAMETERS:	---
   DESCRIPTION:	Kopf einsetzen und Einfahren (nur beim start up des Gerätes)
   RETURNS: 	--- */
void HeadInsertAndDrvDown(void)	// wird beim Gerät OL3381 nicht aufgerufen!
{
	ubyte state,flg;
	//int intSecurityCover;
	bool blnFirstCallHeadInsert;
  blnFirstCallHeadInsert = TRUE;
  //intSecurityCover = 0;
  state = CB_READY;
  ClearAllAndDisplay(str_big_ex_head);
  while(1)
  {
	ClearLineAndDisplay(str_wait_head_remove);
	// g_uwHeadSafetyTime = TC_SAFETY_COUNT; TC Refresh über Interrupt Schnittstelle durch "]]"
	while(1)
	{
	  if((HeadConnectionPin == 0)&&(state == CB_READY))
	  {state = HeadRemove(TRUE);}
	  else
	  {state = HeadDrvToUpperPos();
		if(state == CB_READY) break;
		if(state == IERR_COVER_OPEN) {WarningDisplay(str_cover_open,str_cover_shut); continue;}
		if(g_ubytShieldWatch == WORKING_WITH_SHIELD)
		{if(state == IERR_JALOUSIE_OPEN) {WarningDisplay(str_blind_open,str_blind_shut); continue;}}
		if(state == IERR_LS_INTERRUPTED) {WarningDisplay(str_ls_interrupted,str_ls_release); continue;}
		if(state == IERR_TC_INTERRUPTED) {WarningDisplay(str_door_open,str_door_shut); continue;}
		if(state == IERR_HEAD_NOT_INSERT)
		{WarningDisplayHeadInsert(); continue;}
	  }
	  if(state == IERR_PROG_ABORT)
	  {flg = HeadChangeUserAbort(HEAD_INSERT); if(flg == HEAD_INSERT) goto HeadInsert1; else continue;}
	  else ErrorDisplay(state);
	}
	ClearLineAndDisplay(str_head_remove);
	while(IsHeadInsertPos()) WDO();//Error: Falls Kopf nicht ganz eingeschoben,dann Fehler
	if(blnFirstCallHeadInsert)
	{ClearLineAndDisplay(str_first_head_insert);
	  ClearLineAndDisplay(str_head_remove_handle);
	}
	else ClearLineAndDisplay(str_head_insert);
	Display(str_go_on);
	HeadInsertAndWaitForEnter();
	ClearAllAndDisplay(str_big_ex_head);
	HeadInsert1:
	if((state = HeadInsert()) == CB_READY) return;
  	if(state == IERR_HEAD_NOT_DEF)	WarningDisplay(str_big_ex_head,str_err_head_unknown);
	blnFirstCallHeadInsert = FALSE;
  }
}

/* FUNCTION: 	ProgHeadChange
   PARAMETERS:	---
   DESCRIPTION:	Kopfwechsel
   RETURNS: 	--- */
void ProgHeadChange(void){
	bool blnFirstCallHeadInsert; ubyte ubytFirstCallHeadRemove,state;
	//int intSecurityCover;
	struct LCD_DISP dHelp;
	blnFirstCallHeadInsert = TRUE;
	ubytFirstCallHeadRemove = TRUE;

	SaveDisplay(&dHelp);									// ab Version 2.10
	PermitHostMode(FALSE);									// keine Hostmode möglich
	ClearAllAndDisplay(str_big_ex_head); ErrorDisplay(VertDrvWayXYCtrl(0));
	if(HeadConnectionPin == 0){								// Kopf eingesetzt
		ErrorDisplay(FRAMWritePstVolume());					// Speicher der Lebensdaten des Kopfes
		ErrorDisplay(FRAMWritePstCycles());
	}else
		ubytFirstCallHeadRemove = FALSE;
	while(1){
		if(HeadRemove(ubytFirstCallHeadRemove) == CB_READY){
			if(g_uiDeviceType != 3381){						// OL3381
				//intSecurityCover = 0;
				ClearLineAndDisplay(str_head_remove);
				while(IsHeadInsertPos()) WDO();				//Warten auf das Kopfentfernen
				//Bitte KopfwechselTool entfernen und Klappe schliessen!,nur beim ersten Durchlauf
				if(blnFirstCallHeadInsert){
					ClearLineAndDisplay(str_first_head_insert);
					ClearLineAndDisplay(str_head_remove_handle);
				}else
					ClearLineAndDisplay(str_head_insert);
				Display(str_go_on);
				HeadInsertAndWaitForEnter();
				ClearAllAndDisplay(str_big_ex_head);
			}else{
				// verbiete Interrupt beim Kopfwechsel
				HeadInterruptEnabled = DISABLE;
				ClearLineAndDisplay(str_head_remove_insert);
				Display(str_go_on);
				//KeyWaitForEnter();
				WaitForRequestEnterKeyOnly();
				Wait(10); 								// Zeitraster in 10ms,Entprellzeit abwarten
				// Reset eines evtl. anhängigen Interrupts	(Prellen bei Kontaktgabe möglich)
				// Erlaube Interrupt beim unerlaubten Kopfwechsel
				HeadInterruptEnabled = ENABLE;
			}
		}
		ubytFirstCallHeadRemove = FALSE;
		if((state = HeadInsert()) == CB_READY){
			PermitHostMode(TRUE);
			break;
		}
		if(state ==  IERR_HEAD_NOT_DEF)	WarningDisplay(str_big_ex_head,str_err_head_unknown);
		blnFirstCallHeadInsert = FALSE;
	}
	RestoreDisplay(&dHelp);
}

/* FUNCTION: 	HeadInsert
   PARAMETERS:	---
   DESCRIPTION:	Einsetzen des Kopfes in die Arbeitsposition
   RETURNS: 	CB_READY / IERR_PROG_ABORT / IERR_HEAD_NOT_DEF */
ubyte HeadInsert(void){
	ubyte state,flg;
	ulong ulngHeadExAtDevice,ulngDevExAtHead;
	PstDrvSetCurrentToZero();
	//Disable Schrittmotorbestromung,da so der geringste Offset am Ausgang der SM-Endstufen entsteht
	// Nachteil: alle SM-Antriebe müssen neu initialisiert werden
	DrvDisable();
	if(g_uiDeviceType != 3381){
		if(IsHeadInsert() == FALSE){
			ClearLineAndDisplay(str_head_insert);
			Display(str_go_on);
			HeadInsertAndWaitForEnter();
		}
		//Kopf nach unten bewegen
		ClearLineAndDisplay(str_wait_head_insert);
		//g_uwHeadSafetyTime = TC_SAFETY_COUNT; TC Refresh über Interrupt Schnittstelle durch "]]"
		while(1){
			state = HeadDrvToLowerPos();
			if(state == CB_READY) break;
			if(state == IERR_COVER_OPEN){
				WarningDisplay(str_cover_open,str_cover_shut);
				continue;
			}
			if(g_ubytShieldWatch == WORKING_WITH_SHIELD){
				if(state == IERR_JALOUSIE_OPEN){
					WarningDisplay(str_blind_open,str_blind_shut);
					continue;
				}
			}
			if(state == IERR_LS_INTERRUPTED){
				WarningDisplay(str_ls_interrupted,str_ls_release);
				continue;
			}
			if(state == IERR_TC_INTERRUPTED){
				WarningDisplay(str_door_open,str_door_shut);
				continue;
			}
			if(state == IERR_HEAD_NOT_INSERT){
				WarningDisplayHeadInsert();
				continue;
			}
			if(state == IERR_PROG_ABORT){
				flg = HeadChangeUserAbort(HEAD_INSERT);
				if(flg == HEAD_INSERT)
					continue;
				else
					return IERR_PROG_ABORT;
			}	// Pause,weiter mit Enter
			else
				ErrorDisplay(state);
		}
		if(HeadConnectionPin == 1)  ErrorDisplay(IERR_HEAD_NOT_CONNECTED);	//Ersetzen durch evtl. erneuten Versuch
	}else{																	// manueller Kopfwechsel OL3381
																			// nur hier sollte der Interrupt verboten werden
		// verbiete Interrupt beim Kopfwechsel
		HeadInterruptEnabled = DISABLE;
		ClearLineAndDisplay(str_head_insert);
		Display(str_go_on);
		while(1){
			if(HeadConnectionPin == 0) break;		//Kopf eingesetzt
			WaitForRequestEnterKeyOnly();			//KeyWaitForEnter();
		}
		Wait(10);									// Zeitraster in 10ms,Entprellzeit abwarten
		// Reset eines evtl. anhängigen Interrupts	(Prellen bei Kontaktgabe möglich)
		// Erlaube Interrupt beim unerlaubten Kopfwechsel
		HeadInterruptEnabled = ENABLE;
	}

	ErrorDisplay(FRAMCheckAndSetStruktureVersionP1());
	state = ReadHeadNoAndSetDevPara();
	if(state ==  IERR_HEAD_NOT_DEF) return state;
	ErrorDisplay(state);
	ErrorDisplay(EEPROMSetHeadTypeNumber(g_pudtHeadType->type));
	ErrorDisplay(FRAMReadPstVolume()); 								// Einlesen der Lebensdaten des Kopfes
	ErrorDisplay(FRAMReadPstCycles());
	ErrorDisplay(FRAMReadAddVolume(&g_AddVolume));
	ErrorDisplay(FRAMReadDevExchangeAtHead(&ulngDevExAtHead));
	ulngDevExAtHead++;
	ErrorDisplay(FRAMWriteDevExchangeAtHead(&ulngDevExAtHead));		// Gerätewechsel am Kopf
	ErrorDisplay(EEPROMReadHeadExchangeAtDev(&ulngHeadExAtDevice));
	ulngHeadExAtDevice++;
	ErrorDisplay(EEPROMSetHeadExchangeAtDev(&ulngHeadExAtDevice));
	//ErrorDisplay(PstInitZkMan());
	ErrorDisplay(PstSetDrvParaToDefault());

	DrvEnable();													// Enable SM-Steuerung (Referenzspannung)

	if(IsPresMechConfig()){											// Wenn Präsentiermechanik konfiguriert...
																	// Falls Gerät ohne Kopf eingeschalten ist die erweiterte Prozedur notwendig
		ErrorDisplay(SuckWithoutTips());							// Kolben in die obere Endlage fahren
		ErrorDisplay(TipsLoose());
		UserMovePresMechToPresentPos(str_PresMechPresentPos);
	}

	if(g_uiDeviceType != 3381){										// OL3381
		// keine Eingabeaufforderung beim Betrieb mit TC
		if(g_ubytSafetyHead != WORKING_WITH_TC){
			if((g_ubytSafetyHead == WORKING_WITH_BLIND)&&(g_ubytShieldWatch == WORKING_WITH_SHIELD)){
				ClearLineAndDisplay(str_blind_open1);
				Display(str_go_on);
				while(1){
					WDO();
					if(IsHeadShieldOpen() && (KeyWaitForAny() == ENTER_KEY)) break;
				}
				ClearAllAndDisplay(str_big_ex_head); ClearLineAndDisplay(str_wait_head_insert);
			}
		}
		// Falls Betrieb mit TC dann wird nur noch das Spitzenwechseltool entfernt,ab Version 1.47
		if(g_ubytSafetyHead == WORKING_WITH_TC){
			RemoveHeadChangeTool();
			LcdClear(str_go_on[g_ubytLang].line);
			DisplayAndEnter(str_go_on);
		}else{
			// Entfernen des Kopfwechseltools bzw. des Magazins,Magazin einsetzen und Spitzen anziehen
			if(IsChangeToolInsert())
				SpwManTipsChange(str_big_ex_head,str_head_remove_tool,g_ubytMagNotInsertAfterHeadExchange);
			else
				SpwManTipsChange(str_big_ex_head,str_magazin_remove,g_ubytMagNotInsertAfterHeadExchange);
		}
	}else{
		if(IsChangeToolInsert())
			SpwManTipsChange(str_big_ex_head,str_head_remove_tool,g_ubytMagNotInsertAfterHeadExchange);
		else
			SpwManTipsChange(str_big_ex_head,str_magazin_remove,g_ubytMagNotInsertAfterHeadExchange);
	}
	LcdClear(SECOND_LINE);
	if(g_ubytMagNotInsertAfterHeadExchange){
		ClearAllAndDisplay(str_carriage_init);
		DisplayAndEnter(str_go_on);
	}
	HrzVertDrvCalibrate();
	return CB_READY;
}

/* FUNCTION: 	HeadRemove
   PARAMETERS:	---
   DESCRIPTION:	Vorbereitung des Kopfes für die Entnahme
   RETURNS: 	CB_READY / IERR_PROG_ABORT */
ubyte HeadRemove(ubyte FirstCall){
	ubyte state,flg;
	/* Speicher der Lebensdaten des Kopfes,nicht hier,da Programm in jedem Zustand
	 	 aufgerufen werden kann,auch wenn der Kopf zwischen den Endlagen steht!!! */

	// Magazin lösen ... beim ersten Aufruf
	if(FirstCall && (!IsChangeToolInsert() || !IsPstEccentricTighten())){	// Hier abfrage auf Wechseltool noch integrieren
		RemoveTipsMagAndInsertHeadChangeTool();
		// Kopf in die obere Position (Entnahme) bewegen
		// keine Eingabeaufforderung beim Betrieb mit TC
		if(g_uiDeviceType != 3381){		// OL3381
			if(g_ubytSafetyHead != WORKING_WITH_TC){
				if((g_ubytSafetyHead == WORKING_WITH_BLIND)&&(g_ubytShieldWatch == WORKING_WITH_SHIELD)){
					ClearLineAndDisplay(str_blind_shut);
					DisplayAndEnter(str_go_on);
				}
			}
		}
	}
	PstDrvSetCurrentToZero();
	DrvDisable();
	if(g_uiDeviceType != 3381){
		ClearLineAndDisplay(str_wait_head_remove);
		//g_uwHeadSafetyTime = TC_SAFETY_COUNT;  Refresh über Interrupt Schnittstelle durch "]]"
		while(1){
			state = HeadDrvToUpperPos();
			if(state == CB_READY) break;
			if(state == IERR_COVER_OPEN){
				WarningDisplay(str_cover_open,str_cover_shut);
				continue;
			}
			if(g_ubytShieldWatch == WORKING_WITH_SHIELD){
				if(state == IERR_JALOUSIE_OPEN){
					WarningDisplay(str_blind_open,str_blind_shut);
					continue;
				}
			}
			if(state == IERR_LS_INTERRUPTED){
				WarningDisplay(str_ls_interrupted,str_ls_release);
				continue;
			}
			if(state == IERR_TC_INTERRUPTED){
				WarningDisplay(str_door_open,str_door_shut);
				continue;
			}
			if(state == IERR_HEAD_NOT_INSERT){
				//WarningDisplay(str_head_insert_again,str_head_insert_hint);
				WarningDisplayHeadInsert();
				continue;
			}
			if(state == IERR_PROG_ABORT){
				flg = HeadChangeUserAbort(HEAD_REMOVE);
				if(flg == HEAD_REMOVE)
					continue;
				else
					return
				IERR_PROG_ABORT;
			}	// Pause,weiter mit Enter
			else
				ErrorDisplay(state);
		}
	}
	return CB_READY;
}


/* FUNCTION: 	HeadDrvToUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Fahren des Kopfes in die Entnahmeposition
   RETURNS: 	CB_READY,IERR_PROG_ABORT,IERR_COVER_OPEN,IERR_LS_INTERRUOPTED,IERR_JALOUSIE,
				IERR_HEAD_NOT_INSERT u. IERR_LIFT_UPPER_POS */
ubyte HeadDrvToUpperPos(void){
	ubyte state,hstate;
	state = CB_READY;
	DrvDisable();
	if(g_uiDeviceType != 3381){	// motorisierter Kopfwechsel
		g_uwWaitTime = HEAD_MOVE_TIMEOUT;
		if(IsHeadUpperPos()) return state;
		//Disable Schrittmotorbestromung,da so der geringste Offset am Ausgang der SM-Endstufen entsteht
		// Nachteil: alle SM-Antriebe müssen neu initialisiert werden
		//DrvDisable();  ab Version 3.0
		HeadDCMotorUpwards;
		while(g_uwWaitTime){
			WDO();
			if(IsHeadUpperPos()) break;
			if(IsHeadCoverOpen()){
				state = IERR_COVER_OPEN;
				break;
			}
			//mku obsolete:	if(IsHeadShieldOpen()){state = IERR_JALOUSIE_OPEN; break;}
			hstate = BlindOpenOrLSInterrupted();
			if(hstate != IERR_NO_ERR){
				state = hstate;
				break;
			}
			if(ReadAbortConditions()){
				state = IERR_PROG_ABORT;
				break;
			}
			//keine Abfrage der Position Kopf eingeschoben im nVorwarnbereich
			//if(!IsHeadPreWarnPos()) if(!IsHeadInsertPos()){state = IERR_HEAD_NOT_INSERT; break;}
			if(!IsHeadInsert()){
				state = IERR_HEAD_NOT_INSERT;
				break;
			}
		}
		HeadDCMotorStopp;
		if(g_uwWaitTime == 0) state = IERR_LIFT_UPPER_POS;
	}
	return state;
}

/* FUNCTION: 	HeadDrvToLowerPos
   PARAMETERS:	---
   DESCRIPTION:	Fahren des Kopfes in die Arbeitsposition
   RETURNS: 	CB_READY,IERR_PROG_ABORT,IERR_COVER_OPEN,IERR_HEAD_NOT_INSERT,IERR_JALOUSIE,
				IERR_LS_INTERRUPTED	u. IERR_LIFT_LOWER_POS */
ubyte HeadDrvToLowerPos(void){
	ubyte state,hstate;
	state = CB_READY;
	if(IsHeadLowerPos()) return state;
	//Disable Schrittmotorbestromung,da so der geringste Offset am Ausgang der SM-Endstufen entsteht
	// Nachteil: alle SM-Antriebe müssen neu initialisiert werden
	DrvDisable();
	g_uwWaitTime = HEAD_MOVE_TIMEOUT;
	HeadDCMotorDown;
	while(g_uwWaitTime){
		WDO();
		if(IsHeadLowerPos()) break;
		if(IsHeadCoverOpen()){
			state = IERR_COVER_OPEN;
			break;
		}
		// if(IsHeadShieldOpen()){state = IERR_JALOUSIE_OPEN; break; }
		hstate = BlindOpenOrLSInterrupted();
		if(hstate != IERR_NO_ERR){
			state = hstate;
			break;
		}
		if(ReadAbortConditions()){
			state = IERR_PROG_ABORT;
			break;
		}
		//keine Abfrage der Position Kopf eingeschoben im nVorwarnbereich
		//if(!IsHeadPreWarnPos()) if(!IsHeadInsertPos()) {state = IERR_HEAD_NOT_INSERT; break;}
		if(!IsHeadInsert()){
			state = IERR_HEAD_NOT_INSERT;
			break;
		}
	}
	HeadDCMotorStopp;
	if(g_uwWaitTime == 0) state = IERR_LIFT_LOWER_POS;
	return state;
}

/* FUNCTION: 	HeadChangeUserAbort
   PARAMETERS:	Cursorposition im Auswahlmenü
   DESCRIPTION:	Auswahl von Kopf einsetzen bzw. Kopf entfernen
   RETURNS: 	Index für die Position der Auswahlzeichenkette 0 basierend */
ubyte HeadChangeUserAbort(ubyte id){
	struct LCD_DISP dHelp;
	SaveDisplay(&dHelp);
 	ClearAllAndDisplay(str_head_user_abort);
 	Display(str_head_go_on);
 	SetCursorOff();
 	WarnBeep();
 	id = ProgModification(m_udtHeadInsertRemove,sizeof(m_udtHeadInsertRemove)/sizeof(struct Modus),id,NO_LINE);
 	RestoreDisplay(&dHelp);
 	return id;
}

/* FUNCTION: 	ProgHeadFlagInsert,ProgHeadFlagRemove
   PARAMETERS:	---
   DESCRIPTION:	Adresshalterfunktionen
   RETURNS: 	--- */
void ProgHeadFlagInsert(void) {}
void ProgHeadFlagRemove(void) {}

/* FUNCTION: 	HeadInsertAndWaitForEnter
   PARAMETERS:	---
   DESCRIPTION: Prüft die Kopfposition und das Betätigen der Entertaste
   RETURNS: 	TRUE -> Entertaste,FALSE -> Kopfendlage nicht betätigt  */
ubyte HeadInsertAndWaitForEnter(void){
	ubyte state,SecurityCheck;
	int intSecurityCover;
	intSecurityCover = 0;
	SecurityCheck = FALSE;
	while(1){
		WDO();
		if((IsHeadPreWarnPos()||(IsHeadInsertPos())) == FALSE){
	//Sicherheitsüberprüfung Kopfabdeckung,beim Kopfwechsel muss die Klappe geöffner werden!
			state = FALSE;
			SecurityCheck = TRUE;
			if(IsHeadCoverOpen()){
				if(intSecurityCover < SEC_COVER_OPEN_CYCLES) intSecurityCover++;
			}
		}else
			state = TRUE;
		if((state == FALSE) && (tast_read() == ENTER_KEY))
			WarningDisplay(str_head_insert_again,str_head_insert_hint);
		if((state == TRUE) && (tast_read() == ENTER_KEY)){
			if(SecurityCheck)
				if(intSecurityCover != SEC_COVER_OPEN_CYCLES) ErrorDisplay(IERR_COVER_MONITORING);
			return TRUE;
		}
	}
}

/* FUNCTION: 	HeadCheckInsertAndWarning
   PARAMETERS:	---
   DESCRIPTION: überprüft die Kopfposition hinten und gibt gegebenenfalls eine Warnung aus
   RETURNS: 	---  */
void HeadCheckInsertAndWarning(void){
	while(!IsHeadInsertPos()) WarningDisplay(str_head_not_end_pos,str_head_insert_hint);
}

/* FUNCTION: 	IsHeadInsert
   PARAMETERS:	---
   DESCRIPTION: Abfrage der Position Kopf eingeschoben (nicht in der nVorwarnlage)
   RETURNS: 	TRUE / FALSE  */
ubyte IsHeadInsert(void){
	return (IsHeadPreWarnPos()||(IsHeadInsertPos()));
}

/* FUNCTION: 	WarningDisplayHeadInsert
   PARAMETERS:	---
   DESCRIPTION: Warnhinweis Kopf nach hinten schieben mit überwachung
				des Sicherheitsschalters in HeadInsertAndWaitForEnter
   RETURNS: 	---  */
void WarningDisplayHeadInsert(void){
	struct LCD_DISP dHelp;
	SaveDisplay(&dHelp);
	ClearAllAndDisplay(str_head_insert_again); Display(str_head_insert_hint); SetCursorOff();
	Display(str_go_on); WarnBeep();
	HeadInsertAndWaitForEnter();Wait(50);
	RestoreDisplay(&dHelp);
}

/* FUNCTION: 	HeadMoveCtrl
   PARAMETERS:	Funktionspointer
   DESCRIPTION: Steuerung der Liftbewegung mit Fehlerhandling (Test und Inbetriebnahme)
   RETURNS: 	CB_READY / Fehlermeldung  */
ubyte HeadMoveCtrl(ubytFct fp){
	ubyte state;
	while(1){
		state = fp();
		switch(state){
		case CB_READY:
			return state;
		case IERR_COVER_OPEN:
			WarningDisplay(str_cover_open,str_cover_shut);
			continue;
		case IERR_JALOUSIE_OPEN:
			if(g_ubytShieldWatch == WORKING_WITH_SHIELD) WarningDisplay(str_blind_open,str_blind_shut);
			continue;
		case IERR_LS_INTERRUPTED:
			WarningDisplay(str_ls_interrupted,str_ls_release);
			continue;
		case IERR_TC_INTERRUPTED:
			WarningDisplay(str_door_open,str_door_shut);
			continue;
		case IERR_HEAD_NOT_INSERT:
			WarningDisplayHeadInsert();
			continue;
		default:
			return state;
		}
	}
}

/* FUNCTION: 	RemoveTipsMagAndInsertHeadChangeTool
   PARAMETERS:	---
   DESCRIPTION:	Entnahme des Magazins und Einsetzen des Kopfwechseltools
   RETURNS: 	CB_READY  */
ubyte RemoveTipsMagAndInsertHeadChangeTool(void){
	ubyte state,_state; unsigned int ls_cnt;
	ls_cnt = LS_WATCH_CYCLES;									// Lichtvorhangüberwachung
	ErrorDisplay(SuckWithoutTips());
	ClearLineAndDisplay(str_tips_detach);
	while(1){														//Magazin lösen
		while(1){
			state = TipsLoose();
			if(state == CB_READY){
				if(IsPresMechConfig()){	// Wenn Präsentiermechanik Konfiguriert...
					UserMovePresMechToPresentPos(str_PresMechPresentPos);
				}
				break;
			}
			if(state == IERR_COVER_OPEN){
				WarningDisplay(str_cover_open,str_cover_shut);
				continue;
			}
			else
				ErrorDisplay(state);
		}
		// Magazin entfernen
		ClearLineAndDisplay(str_magazin_remove);
		while(1){													// Warten auf das Entfernen des Magazins
			WDO();
			if(IsSpwMagazineInsert() == FALSE) break;
		}

		// Präsentiermechanik wieder einfahren,um Kopfwechsel zu erleichtern
		if(IsPresMechConfig()){				 						// Wenn Präsentiermechanik konfiguriert...
			UserMovePresMechToCyBiWell(str_PresMechRemoveRack);		// Präsentiermechanik wieder einfahren,um Kopfwechsel zu erleichtern
		}
		// Kopfwechseltool einsetzen	 --neu--
		while(1){
			ClearLineAndDisplay(str_head_tool_intro);
			while(1){	// Warten auf das Einschieben des Kopfwechseltools,Abfrage erst nach Anziehen sicher
				WDO();
				if((ls_cnt != 0)&&(IsLSInterrupted())) ls_cnt-= 1;
				if(IsSpwMagazineInsert()) break;
			}
			if(g_ubytSafetyHead == WORKING_WITH_LS) {if(ls_cnt != 0) ErrorDisplay(IERR_LS_FAULTY);}
			ClearLineAndDisplay(str_head_tool_tighten); DisplayAndEnter(str_go_on);
			while(1){
				state = TipsTighten();
				if(state == IERR_LS_INTERRUPTED)
					WarningDisplay(str_ls_interrupted,str_ls_release);
				else
				break;
			}
			if(state == CB_READY){
				if(IsChangeToolInsert())
					return CB_READY;
				else{
					ClearLineAndDisplay(str_tips_detach);
					_state = TipsLoose(); //state = TipsLoose();
					if(_state == CB_READY){
						if(IsPresMechConfig()){				// Wenn Präsentiermechanik Konfiguriert...
							//_state = MovePresMechPresentPos();	// ... Tip Magazin ausschieben
							UserMovePresMechToPresentPos(str_PresMechPresentPos);
						}
					}
					ErrorDisplay(_state); break;	//continue;
				}
			}
			if((state == IERR_PROG_ABORT)||(state == IERR_MAG_NOT_INSERT)||(state == IERR_LS_INTERRUPTED)){
				ClearLineAndDisplay(str_head_tool_loose);
				_state = TipsLoose();
				ErrorDisplay(_state);
			}
		}
	}
}

/* FUNCTION: 	LifterDrvToUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Fahren des Lifters in die Entnahmeposition,ohne Kopfüberwachung!!!
   RETURNS: 	CB_READY,IERR_PROG_ABORT,IERR_COVER_OPEN,IERR_JALOUSIE,IERR_LS_INTERRUPTED
				u. IERR_LIFT_UPPER_POS */
ubyte LifterDrvToUpperPos(void){
	ubyte state,hstate;
	state = CB_READY; g_uwWaitTime = HEAD_MOVE_TIMEOUT;
	if(IsHeadUpperPos())
		return state;
	//Disable Schrittmotorbestromung,da so der geringste Offset am Ausgang der SM-Endstufen entsteht
	// Nachteil: alle SM-Antriebe müssen neu initialisiert werden
	DrvDisable();
	HeadDCMotorUpwards;
	while(g_uwWaitTime){
		WDO();
		if(IsHeadUpperPos()) break;
		if(IsHeadCoverOpen()){
			state = IERR_COVER_OPEN;
			break;
		}
		hstate = BlindOpenOrLSInterrupted();
		if(hstate != IERR_NO_ERR){
			state = hstate;
			break;
		}
		if(ReadAbortConditions()){
			state = IERR_PROG_ABORT;
			break;
		}
	}
	HeadDCMotorStopp;
	if(g_uwWaitTime == 0)
		state = IERR_LIFT_UPPER_POS;
	return state;
}

/* FUNCTION: 	PrgLifterDrvToUpperPos
   PARAMETERS:	---
   DESCRIPTION:	Fahren des Lifters in die Entnahmeposition,ohne Kopfüberwachung!
				nur für Inbetriebnahme
   RETURNS: 	--- */
void PrgLifterDrvToUpperPos(void){
	ubyte state;
	while(1){
		state = LifterDrvToUpperPos();
		if(state == IERR_COVER_OPEN){
			WarningDisplay(str_cover_open,str_cover_shut);
			continue;
		}
		if(g_ubytShieldWatch == WORKING_WITH_SHIELD){
			if(state == IERR_JALOUSIE_OPEN){
				WarningDisplay(str_blind_open,str_blind_shut);
				continue;
			}
		}
		if(state == IERR_LS_INTERRUPTED){
			WarningDisplay(str_ls_interrupted,str_ls_release);
			continue;
		}
		if(state == IERR_TC_INTERRUPTED){
			WarningDisplay(str_door_open,str_door_shut);
			continue;
		}
		if(state == IERR_PROG_ABORT){
			WarningDisplay(str_head_break,str_head_dummy);
			continue;
		}
		if(state == CB_READY)
			break;
		else
			ErrorDisplay(state);
	}
}

void RemoveHeadChangeTool(void){
	ClearLineAndDisplay(str_head_tool_loose);
	ErrorDisplay(TipsLoose());
	ClearLineAndDisplay(str_head_remove_tool);
	while(1){										// Warten auf das Entfernen des Kopfwechseltools
		WDO();
		if(!IsSpwMagazineInsert())
			break;
	}
}

void UnexpectedHeadRemove(void){
	DrvDisable();					// nur so ist die Ausgangsspannung an der Endstufe am geringsten
	PstDrvSetCurrentToZero();
	Wait(30);	// Zeitraster in 10ms
	HAL_NVIC_SystemReset();			// Software Reset,da Kopf im Prozess entfernt wurde

}

/* FUNCTION: 	UserMovePresMechToPresentPos
   PARAMETERS:	---
   DESCRIPTION:	Ausfahren der Präsentiermechanik,
				wegen der Verletzungsgefahr nur in Verbindung mit einer User-Bestätigung
   RETURNS: 	--- */
void UserMovePresMechToPresentPos(DispString_t* str){
	if(!IsPresMechPresentPos()){
		ClearLineAndDisplay(str);
		DisplayAndEnter(str_go_on);
		ErrorDisplay(MovePresMechPresentPos());			// ... Kopfwechseltool ausschieben
	}
}

/* FUNCTION: 	UserMovePresMechToCyBiWell
   PARAMETERS:	---
   DESCRIPTION:	Einfahren der Präsentiermechanik,
				wegen der Verletzungsgefahr nur in Verbindung mit einer User-Bestätigung
   RETURNS: 	--- */
void UserMovePresMechToCyBiWell(DispString_t* str){
	if(!IsPresMechCyBiWellPos()){	// Position war schon vorhanden
		ClearLineAndDisplay(str);
		DisplayAndEnter(str_go_on);
		ErrorDisplay(MovePresMechCyBiWell());
	}
}

/* FUNCTION: 	IsHeadType1536
   PARAMETERS:	---
   DESCRIPTION:	Check auf 1536-Kopf
   RETURNS: 	TRUE / FALSE */
unsigned char IsHeadType1536(void){
	unsigned char state;
	if((g_pudtHeadType->type == OL5021_1)||(g_pudtHeadType->type == OL5021_12))
		state = TRUE;
  	else
		state = FALSE;
	return state;
}

