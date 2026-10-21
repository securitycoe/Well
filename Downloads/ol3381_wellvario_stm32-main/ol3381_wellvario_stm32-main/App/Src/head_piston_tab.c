//****************************************************************************
//Module		head_piston_tab
//Filename		head_piston_tab.c
//Project		common
//----------------------------------------------------------------------------
//Controller	Infineon C167CS-L
//
//Compiler	  Tasking
//
//Description   This file contains functions and variables
//				for .
//
//----------------------------------------------------------------------------
//Date		  01.02.2008 15:22:34
//
//****************************************************************************

//****************************************************************************
//Project Includes
//****************************************************************************

#include <stdio.h>

#include "head_piston_tab.h"


//****************************************************************************
//Macros
//****************************************************************************

//****************************************************************************
//local definitions
//****************************************************************************

//****************************************************************************
//Imported External Variables
//****************************************************************************

//****************************************************************************
//Private Variables
//****************************************************************************
// Kopf OL3383 8 XXXX (96/250µl)
const struct TIPS_PARA TipsParaBigHead96[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 4,Magnet links u. rechts
		, { 1.018, -0.005, 150, 0.1, 25, 2 }		//Kodierung 5,Magnet rechts
		, { 1.018, -0.005, 100, 0.1, 25, 2 }		//Kodierung 6,Magnet links
		, { 1.018, -0.005, 250, 0.1, 70, 2 } };		//Kodierung 7,ohne Magnete

//Kopf OL3383 7 XXXX (96/25µl)
const struct TIPS_PARA TipsParaSmallHead96[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 25, 0.01, 2.0, 0.5 }//Kodierung 4,Magnet links u. rechts für 60µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 25, 0.01, 2, 0.5 }	//Kode 5,Magnet rechts,fine point tips und eigene Spitzen für 384 25µl/2µl
		, { 1.018, -0.00065, 10, 0.01, 2, 0.5 }	//Kode 6,Magnet links,fine point tips und eigene Spitzen für 384 10µl/2µl

		/* Nachfolgnde Spitzenparametersatz war historisch gewachsen,da diese Spitzen keine Anwendung finden,
		 aber ein Problem mit dem Einsatz des Plastmagazin mit unseren Spitzen darstellen,
		 erfolgt nach Absprache mit Thomas diese Änderung */
//	,{1.020,-0.00050,25,0.01,6,0.5}		//Kode 7,ohne Magnete	25µl Matrix Spitzen und CH3 Tips

		, { 1.018, -0.00065, 25, 0.01, 2, 0.5 } }; //Kode 7,ohne Magnete,fine point tips und eigene Spitzen für 384 25µl/2µl

//Kopf für LID und LHA OL3711 6 XXXXX,OL3714 6 XXXX (384/25µl)
const struct TIPS_PARA TipsParaSmallHead384[8] = { { 0, 0, 0, 0, 0, 0 }	//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 25, 0.01, 2.0, 0.5 }//Kodierung 4,Magnet links u. rechts für 60µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 25, 0.01, 2, 0.5 }	//Kode 5,Magnet rechts,eigene Spitzen für 384 25µl/2µl
		, { 1.018, -0.00065, 10, 0.01, 2, 0.5 }	//Kode 6,Magnet links,eigene Spitzen für 384 10µl/2µl

		/* Nachfolgnde Spitzenparametersatz war historisch gewachsen,da diese Spitzen keine Anwendung
		 finden können (zu grosser Kopf),aber ein Problem mit dem Einsatz des Plastmagazin
		 mit unseren Spitzen darstellen,erfolgt nach Absprache mit Thomas diese Änderung */
//	,{1.020,-0.00050,25,0.01,6,0.5}		//Kode 7,ohne Magnete,25µl Matrix Spitzen und CH3 Tips

		, { 1.018, -0.00065, 25, 0.01, 2, 0.5 } };//Kode 7,ohne Magnete,eigene Spitzen für 384 25µl/2µl

// Mit dem Kopf OL3383 5 XXXXX (384/40µl) sind durch die geänderte Justierung bei den 25µl-Spitzen auch 40µl möglich,siehe Kode 5
const struct TIPS_PARA TipsParaSmallHead384_5[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 40, 0.01, 2.0, 0.5 }//Kodierung 4,Magnet links u. rechts für 60µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 40, 0.01, 2, 0.5 }	//Kode 5,Magnet rechts,eigene Spitzen für 384 40µl/2µl
		, { 1.018, -0.00065, 10, 0.01, 2, 0.5 }	//Kode 6,Magnet links,eigene Spitzen für 384 10µl/2µl

		/* Nachfolgender Spitzenparametersatz war ursprünglich auf 0 gesetzt,um nur magnetkodierte Magazine
		 zu Verwenden,mit der Anwendung von eigenen TipsTrays wurde dieses aufgegeben.
		 Die nachfolgende Änderung wurde durch Herrn Mµller angewiesen und ist ab Version 2.21 verfügbar*/
		, { 1.018, -0.00065, 40, 0.01, 2, 0.5 } };	//Kode 7,ohne Magnete

// Mit dem Kopf OL3383 2 XXXXX (96/40µl) sind durch die geänderte Justierung bei den 25µl-Spitzen auch 40µl möglich,siehe Kode 5
// Die Parametertabelle wurde von den Kopf 384/40µl kopiert und hat folglich die gleichen Eigenschaften
const struct TIPS_PARA TipsParaSmallHead384_2[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 40, 0.01, 2.0, 0.5 }//Kodierung 4,Magnet links u. rechts für 60µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 40, 0.01, 2, 0.5 }	//Kode 5,Magnet rechts,eigene Spitzen für 384 40µl/2µl
		, { 1.018, -0.00065, 10, 0.01, 2, 0.5 }	//Kode 6,Magnet links,eigene Spitzen für 384 10µl/2µl
		, { 1.018, -0.00065, 40, 0.01, 2, 0.5 } }; 	//Kode 7,ohne Magnete

/* Mit dem Kopf OL3384 6: 60µl/384 ( Sonderkopf GSK UK wird wieder eingezogen,
 da Kolbendurchmesser größer wird --> max. Volumen 60 µl)	*/
const struct TIPS_PARA TipsParaSmallHead384_6[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 60, 0.01, 4.0, 0.5 }//Kodierung 4,Magnet links u. rechts Magazin: OL3384-25-616 (384 Doppeldecker),-25-617 (384)
		, { 1.018, -0.00065, 40, 0.01, 4.0, 0.5 }//Kode 5,Magnet rechts,für 25µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 10, 0.01, 4.0, 0.5 }//Kode 6,Magnet links,eigene Spitzen für 384 10µl/2µl -->  ??? keine Festlegung
		, { 1.018, -0.00065, 60, 0.01, 4.0, 0.5 } };//Kode 7,ohne Magnete für 60µl-Spitzen OL2001-17-162-03

/* Mit dem Kopf OL3384 7: 60µl/96 	*/
const struct TIPS_PARA TipsParaSmallHead384_7[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }						//Kodierung 3,Zusatzmagnet
		, { 1.018, -0.00065, 60, 0.01, 4.0, 0.5 }//Kodierung 4,Magnet links u. rechts Magazin: OL3384-25-616 (384 Doppeldecker),-25-617 (384)
		, { 1.018, -0.00065, 40, 0.01, 4.0, 0.5 }//Kode 5,Magnet rechts,für 25µl-Spitzen OL2001-17-162-03
		, { 1.018, -0.00065, 10, 0.01, 4.0, 0.5 }//Kode 6,Magnet links,eigene Spitzen für 384 10µl/2µl -->  ??? keine Festlegung
		, { 1.018, -0.00065, 60, 0.01, 4.0, 0.5 } };//Kode 7,ohne Magnete für 60µl-Spitzen OL2001-17-162-03

// Mit dem Kopf OL3384 2 2,5µl	(2µl --> ca. 1,63mm)
const struct TIPS_PARA TipsParaSmallHead96_2[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 3,Zusatzmagnet
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 4,Magnet links u. rechts
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 5,Magnet rechts
		, { 0, 0, 0, 0, 0, 0 }					//Kode 6,Magnet links
		, { 1.0, 0.0, 2.5, 0.001, 1.0, 0.5 } };	//Kode 7,ohne Magnete,Spitzen OL3384-28-102 (1,0)

// Mit dem Kopf OL3384 1 2,5µl	(2µl --> ca. 1,63mm)
const struct TIPS_PARA TipsParaSmallHead384_1[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 3,Zusatzmagnet
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 4,Magnet links u. rechts
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 5,Magnet rechts
		, { 0, 0, 0, 0, 0, 0 }					//Kode 6,Magnet links
		, { 1.0, 0.0, 2.5, 0.001, 1.0, 0.5 } };	//Kode 7,ohne Magnete,Spitzen OL3384-28-102 (1,0)

// Mit dem Kopf OL3384 15 8µl	(1µl --> ca. 0.81mm)
const struct TIPS_PARA TipsParaSmallHead384_15[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }			//Kodierung 1  Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 3,Zusatzmagnet
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 4,Magnet links u. rechts
		, { 1.0, 0.0, 1.0, 0.01, 1.0, 0.0 }		//Kodierung 5,LP OL5021-70-001
		, { 0, 0, 0, 0, 0, 0 }					//Kode 6,Magnet links
		, { 1.0, 0.0, 8.0, 0.01, 1.0, 0.0 } };//Kode 7,ohne Magnete,Spitzen OL3384-28-102 (1,0)

// Mit dem Kopf OL5021 1 8µl	(1µl --> ca. 1,76mm)
const struct TIPS_PARA TipsParaSmallHead1536_1[8] = { { 0, 0, 0, 0, 0, 0 }//Kodierung 0,Zusatzmagnet + rechter u. linker
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 1,Zusatzmagnet + rechter
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 2,Zusatzmagnet + linker
		, { 0, 0, 0, 0, 0, 0 }					//Kodierung 3,Zusatzmagnet
		, { 0, 0, 0, 0, 0, 0 }				//Kodierung 4,Magnet links u. rechts
		, { 1.0, 0.0, 1.0, 0.01, 1.0, 0.0 }		//Kodierung 5,LP OL5021-70-001
		, { 0, 0, 0, 0, 0, 0 }					//Kode 6,Magnet links
		, { 1.0, 0.0, 8.0, 0.01, 1.0, 0.0 } };//Kode 7,ohne Magnete,Spitzen OL3384-28-102 (1,0)

// Hummingwell-Kopf OL3387 8,Spitzenparameter sind nur aus Kompatibilität vorhanden
const struct TIPS_PARA TipsParaHummingWell[1] = { { 0, 0, 0, 0, 0, 0 } };//Magazin-Kodierung wird hier nicht ausgewertet,da Magnetecodierung für die Auswahl zw. 96 und 384 steht

// Magnetkonfiguration Kodierung 4,links u. rechts ist für 60 µl-Spitzen vorgesehen

//****************************************************************************
//Public Variables
//****************************************************************************
// HoldCurrent; PstMaxWay; PstUpperPlay(4µl); PstGearRatio[Imp/µl]; PstDrvMicroGearRatio;
// 8 --> gr. Kopf 96,7 --> kl. Kopf96,6,5 --> SIM (384)
//const struct HEAD_DEV HeadDev_OL3383_8	 =
//	{{0.25,50,0.32,260.475645,3200,"%5.1f",0.1,1.0,96}
// 	,{250,0.1,70,1.2,1.0,0.1,-0.1}};

// Grosser Kopf wird ab Version 1.01 mit einer größeren oberen Lose betrieben
// Z-Nr.: OL3383-14-830; Gerätenr.: OL3383 8 ####; 96/250 µl
const struct HEAD_DEV HeadDev_OL3383_8 = { 0.25, 50, 0.5, 260.475645, 3200,
		"%5.1f", 0.1, 1.0, 96, 250, 0.1, 70, 1.2, 1.0, 0.1, -0.1, 17, 250 };

// Z-Nr.: OL3383-14-730; Gerätenr.: OL3383 7 ####; 96/25 µl
const struct HEAD_DEV HeadDev_OL3383_7 = { 0.25, 50, 0.32, 2607.5947, 3200,
		"%5.2f", 0.01, 0.1, 96, 25, 0.01, 2, 1.2, 1.0, 0.1, -0.1, 17, 25 };

// Z-Nr.: OL3383-14-630; Gerätenr.: OL3383 6 ####; 384/25 µl
const struct HEAD_DEV HeadDev_OL3383_6 = { 0.25, 50, 0.32, 2607.5947, 3200,
		"%5.2f", 0.01, 0.1, 384, 25, 0.01, 2, 1.2, 1.0, 0.1, -0.1, 17, 25 };

// Z-Nr.: OL3714-14-630; Gerätenr.: OL3714 6 ####; 384/25 µl  LHA
const struct HEAD_DEV HeadDev_OL3714_6 = { 0.25, 50, 0.32, 2607.5947, 3200,
		"%5.2f", 0.01, 0.1, 384, 25, 0.01, 2, 1.2, 1.0, 0.1, -0.1, 17, 25 };

// Z-Nr.: OL3383-14-650; Gerätenr.: OL3383 5 ####; 384/40 µl
const struct HEAD_DEV HeadDev_OL3383_5 = { 0.25		// float PstDrvHoldCurrent
		, 50			// float PstMaxWay
		, 0.32		// float PstUpperPlay 			in mm
		, 2607.5947	// float PstGearRatio			Impulse pro µl
		, 3200		// float PstDrvMicroGearRatio	Impulse pro mm
		, "%5.2f"// char *VolFrmt				Anzeigeformat für das Volumen
		, 0.01		// float VolStartInc			Werte für den Volumeneditor
		, 0.1		// float VolEndInc
		, 384		// unsigned int PstCnt			Anzahl der Kolben
		, 40// float VolMax					Variable werden zur Plausibilitätskontrolle des Spitzenparameterbefehls benötigt
		, 0.01		// float VolMin
		, 2	// float VolOver				Max. überhub beim Dispensieren (zum Test bei Fremdeingabe)
		, 1.2		// float AscMax
		, 1.0		// float AscMin
		, 0.1		// float ZeroMax
		, -0.1		// float ZeroMin
		, 17// float TCWay					Fahrweg der Kolben nach oben beim Spitzenwechsel
		, 40// float Mech_MaxVolume			max. Volumen von der Null-Lage bis zum oberen Anschlag = Weg bis Anschlag - mechanisches Spiel-0.1mm Reserve
		};

// Z-Nr.: OL3383-14-650; Gerätenr.: OL3383 2 ####; 96/40 µl
const struct HEAD_DEV HeadDev_OL3383_2 = { 0.25, 50, 0.32, 2607.5947, 3200,
		"%5.2f", 0.01, 0.1, 96, 40, 0.01, 2, 1.2, 1.0, 0.1, -0.1, 17, 40 };

// Z-Nr.: OL3384-14-630; Gerätenr.: OL3384 6 ####; 384/60 µl,Kolbendurchmesser: 1.80 mm
const struct HEAD_DEV HeadDev_OL3384_6 = { 0.25, 50, 0.4, 1257.5205, 3200,
		"%5.2f", 0.01, 0.1, 384, 60, 0.01, 4, 1.2, 1.0, 0.1, -0.1, 0, 60 };

// Z-Nr.: OL3384-14-730; Gerätenr.: OL3384 7 ####; 96/60 µl,Kolbendurchmesser: 1.80 mm
const struct HEAD_DEV HeadDev_OL3384_7 = { 0.25, 50, 0.4, 1257.5205, 3200,
		"%5.2f", 0.01, 0.1, 96, 60, 0.01, 4, 1.2, 1.0, 0.1, -0.1, 0, 60 };

// Z-Nr.: OL3384-14-230; Gerätenr.: OL3384 2 ####; 96/2.5 µl
const struct HEAD_DEV HeadDev_OL3384_2 = { 0.25, 5.0, 0.32, 2607.5947, 3200,
		"%5.3f", 0.001, 0.01, 96, 2.5, 0.001, 1, 1.2, 1.0, 0.1, -0.1, 0, 2.5 };

// Z-Nr.: OL3384-14-130; Gerätenr.: OL3384 1 ####; 384/2.5 µl
const struct HEAD_DEV HeadDev_OL3384_1 = { 0.25, 5.0, 0.32, 2607.5947, 3200,
		"%5.3f", 0.001, 0.01, 384, 2.5, 0.001, 1, 1.2, 1.0, 0.1, -0.1, 0, 2.5 };

// Z-Nr.: OL3384-14-150; Gerätenr.: OL3384 15 ####; 384/8 µl Kolbendurchmesser: 1.25mm
const struct HEAD_DEV HeadDev_OL3384_15 = { 0.25, 7.5, 0.32, 2607.5947, 3200,
		"%5.2f", 0.01, 0.1, 384, 10.0, 0.01, 1, 1.2, 1.0, 0.1, -0.1, 0, 10.0 };

// Z-Nr.: OL5021-14-130; Gerätenr.: OL5021 1 ####; 1536/8.0 µl Kolbendurchmesser: 1.50mm
const struct HEAD_DEV HeadDev_OL5021_1 = { 0.25, 10.0, 0.30, 1810.82958, 3200,
		"%5.2f", 0.01, 0.1, 1536, 10.0, 0.01, 1, 1.2, 1.0, 0.1, -0.1, 0, 10 };

// Z-Nr.: OL3387-14-130; Gerätenr.: OL3387 8 ####; Hummingwell-Kopf
// Diese Werte haben keinen physikalischen Hintergrund und dienen nur der Kompatibilität,bis auf die Spitzenanzahl!!!
struct HEAD_DEV HeadDev_OL3387_8 = // Aktualisierung der Spitzenanzahl zur Laufzeit über Magazinkodierung
		{ 0.25, 5.0, 0.32, 2607.5947, 3200, "%5.3f", 0.001, 0.01, 96, 2.5,
				0.001, 1, 1.2, 1.0, 0.1, -0.1, 0, 2.5 };

const char OL3384_2[] = "OL3384 2";		// 96/2,5 µl
const char OL3384_20[] = "303384 20";	// 96/2,5 µl,nur neue Bezeichnung
const char OL3384_22[] = "303384 22";// 96/2,5 µl,mit tabellengestützter VC (reserviert)

const char OL3384_1[] = "OL3384 1";		// 384/2,5 µl
const char OL3384_10[] = "303384 10";	// 384/2,5 µl,nur neue Bezeichnung
const char OL3384_12[] = "303384 12";// 384/2,5 µl,mit tabellengestützter VC (reserviert)

const char OL3384_15[] = "OL3384 15"; // 384/8 µl,mit tabellengestützter VC

const char OL3383_7[] = "OL3383 7";		// 96/25 µl
const char OL3383_70[] = "303383 70";	// 96/25 µl,nur neue Bezeichnung
const char OL3383_72[] = "303383 72";// 96/25 µl,mit tabellengestützter VC

const char OL3383_6[] = "OL3383 6";		// 384/25 µl
const char OL3383_60[] = "303383 60";	// 384/25 µl,nur neue Bezeichnung
const char OL3383_62[] = "303383 62";// 384/25 µl,mit tabellengestützter VC

const char OL3383_2[] = "OL3383 2";		// 96/40 µl
const char OL3383_20[] = "303383 20";	// 96/40 µl,nur neue Bezeichnung
const char OL3383_22[] = "303383 22";// 96/40 µl,mit tabellengestützter VC (reserviert)

const char OL3383_5[] = "OL3383 5";		// 384/40 µl
const char OL3383_50[] = "303383 50";	// 384/40 µl,nur neue Bezeichnung
const char OL3383_52[] = "303383 52";// 384/40 µl,mit tabellengestützter VC

const char OL3384_7[] = "OL3384 7";	// 96/60 µl,mit tabellengestützter VC (von Anfang an)
const char OL3384_72[] = "303384 72";	// 96/60 µl,nur neue Bezeichnung

const char OL3384_6[] = "OL3384 6";	// 384/60 µl,mit tabellengestützter VC (von Anfang an)
const char OL3384_62[] = "303384 62";	// 384/60 µl,nur neue Bezeichnung

const char OL3383_8[] = "OL3383 8";		// 96/250 µl
const char OL3383_80[] = "303383 80";	// 96/250 µl,nur neue Bezeichnung
const char OL3383_82[] = "303383 82";// 96/250 µl,mit tabellengestützter VC

const char OL5021_1[] = "OL5021 1";	// 1536/8µl,mit tabellengestützter VC (von Anfang an)
const char OL5021_12[] = "305021 12";	// 1536/8µl,nur neue Bezeichnung

const char OL3711_6[] = "OL3711 6";	// 384/25 µl LID (techn. Daten wie OL3383 6)
const char OL3714_6[] = "OL3714 6";	// 384/25 µl LHA (techn. Daten wie OL3383 6)
const char OL3387_8[] = "OL3387 8"; // Orginalnummer: OL3387 8 Hummingwell-Kopf	(384 bzw. 96 siehe Magazinkodierung)

struct HEAD_TYPE HeadTypeTab[] =
{
		 {OL3384_2	,TipsParaSmallHead96_2		,&HeadDev_OL3384_2	,NULL }
		,{OL3384_20	,TipsParaSmallHead96_2		,&HeadDev_OL3384_2	,NULL }
	//	,{OL3384_22	,TipsParaSmallHead96_2		,&HeadDev_OL3384_2	,VC } 		// nicht vorhanden,,VC lt. Adrian fragwürdig

		,{OL3384_1	,TipsParaSmallHead384_1		,&HeadDev_OL3384_1	,NULL }
		,{OL3384_10	,TipsParaSmallHead384_1		,&HeadDev_OL3384_1	,NULL }
	//	,{OL3384_12	,TipsParaSmallHead384_1		,&HeadDev_OL3384_1	,VC}		// nicht vorhanden,VC lt. Adrian fragwürdig

		,{OL3384_15	,TipsParaSmallHead384_15	,&HeadDev_OL3384_15	,NULL }// &tVolumeCorr_OL3384_15__384_8

		,{OL3383_7	,TipsParaSmallHead96		,&HeadDev_OL3383_7	,NULL }// &tVolumeCorr_OL3383_6__384_25 in version 4.72 for Lundbeck only
		,{OL3383_70	,TipsParaSmallHead96		,&HeadDev_OL3383_7	,NULL }
		,{OL3383_72	,TipsParaSmallHead96		,&HeadDev_OL3383_7	,&tVolumeCorr_OL3383_6__384_25 }

		,{OL3383_6	,TipsParaSmallHead384		,&HeadDev_OL3383_6	,NULL }// &tVolumeCorr_OL3383_6__384_25 in version 4.72 for Lundbeck only
		,{OL3383_60	,TipsParaSmallHead384		,&HeadDev_OL3383_6	,NULL }
		,{OL3383_62	,TipsParaSmallHead384		,&HeadDev_OL3383_6	,&tVolumeCorr_OL3383_6__384_25 }

		,{OL3383_2	,TipsParaSmallHead384_2		,&HeadDev_OL3383_2	,NULL }
		,{OL3383_20	,TipsParaSmallHead384_2		,&HeadDev_OL3383_2	,NULL }
		,{OL3383_22	,TipsParaSmallHead384_2		,&HeadDev_OL3383_2	,&tVolumeCorr_OL3383_2__96_40 }

		,{OL3383_5	,TipsParaSmallHead384_5		,&HeadDev_OL3383_5	,NULL }
		,{OL3383_50	,TipsParaSmallHead384_5		,&HeadDev_OL3383_5	,NULL }
		,{OL3383_52	,TipsParaSmallHead384_5		,&HeadDev_OL3383_5	,&tVolumeCorr_OL3383_5__384_40 }

		,{OL3384_7	,TipsParaSmallHead384_7		,&HeadDev_OL3384_7	,&tVolumeCorr_OL3384_7__96_60 }
		,{OL3384_72	,TipsParaSmallHead384_7		,&HeadDev_OL3384_7	,&tVolumeCorr_OL3384_7__96_60 }

		,{OL3384_6	,TipsParaSmallHead384_6		,&HeadDev_OL3384_6	,&tVolumeCorr_OL3384_6__384_60 }
		,{OL3384_62	,TipsParaSmallHead384_6		,&HeadDev_OL3384_6	,&tVolumeCorr_OL3384_6__384_60 }

		,{OL3383_8	,TipsParaBigHead96			,&HeadDev_OL3383_8	,NULL }
		,{OL3383_80	,TipsParaBigHead96			,&HeadDev_OL3383_8	,NULL }
		,{OL3383_82	,TipsParaBigHead96			,&HeadDev_OL3383_8	,&tVolumeCorr_OL3383_8__96_250 }

		,{OL5021_1	,TipsParaSmallHead1536_1	,&HeadDev_OL5021_1	,&tVolumeCorr_OL5021_14_130__1536_8 }
		,{OL5021_12	,TipsParaSmallHead1536_1	,&HeadDev_OL5021_1	,&tVolumeCorr_OL5021_14_130__1536_8 }

		,{OL3711_6	,TipsParaSmallHead384		,&HeadDev_OL3383_6	,NULL }// da Baugleich mit OL3383 6
		,{OL3714_6	,TipsParaSmallHead384		,&HeadDev_OL3714_6	,NULL }
		,{OL3387_8	,TipsParaHummingWell		,&HeadDev_OL3387_8	,NULL }
};

/* Vorbereitet:
 lt. Aussage von Herrn Siemers war die Verifikation der nachfolgende Tabelle mit den neuen Korrekturwerten ok,
 aber es wurde offiziell noch keine Freigabe seitens der Führung erteilt!
 //	ab Version 5.0 alle Kennlinien mit Korrekturtabelle
 const struct HEAD_TYPE HeadTypeTab[]	 =
 {{OL3383_5,TipsParaSmallHead384_5,&HeadDev_OL3383_5,&tVolumeCorr_OL3383_5__384_40}
 ,{OL3383_2,TipsParaSmallHead384_2,&HeadDev_OL3383_2,&tVolumeCorr_OL3383_2__96_40}	// ??? wo liegt der Unterschied zum 384er Kopf OL3383 5
 ,{OL3383_6,TipsParaSmallHead384,&HeadDev_OL3383_6,&tVolumeCorr_OL3383_6__384_25}
 ,{OL3383_7,TipsParaSmallHead96,&HeadDev_OL3383_7,&tVolumeCorr_OL3383_7__96_25}
 ,{OL3383_8,TipsParaBigHead96,&HeadDev_OL3383_8,&tVolumeCorr_OL3383_8__96_250}
 ,{OL3384_6,TipsParaSmallHead384_6,&HeadDev_OL3384_6,&tVolumeCorr_OL3384_6__384_60}
 ,{OL3384_7,TipsParaSmallHead384_7,&HeadDev_OL3384_7,&tVolumeCorr_OL3384_7__96_60}
 ,{OL3384_2,TipsParaSmallHead96_2,&HeadDev_OL3384_2,NULL}
 ,{OL3384_1,TipsParaSmallHead384_1,&HeadDev_OL3384_1,NULL}
 ,{OL3711_6,TipsParaSmallHead384,&HeadDev_OL3383_6,NULL}								// da Baugleich mit OL3383 6
 ,{OL3714_6,TipsParaSmallHead384,&HeadDev_OL3714_6,NULL}
 ,{OL3387_8,TipsParaHummingWell,&HeadDev_OL3387_8,NULL}};

 */
//****************************************************************************
//External functions
//****************************************************************************
//****************************************************************************
//Prototypes Of Private Functions
//****************************************************************************
//****************************************************************************
//Prototypes Of Public Functions
//****************************************************************************
//****************************************************************************
//Default Settings
//****************************************************************************
unsigned int GetSize_HeadTypeTab(void) {
	return sizeof(HeadTypeTab);
}
