/*******************************************************************
 *
 *	DESCRIPTION:	Dieser File enthµlt nur die Kopfparameter
 *
 *	AUTHOR:		Baumann
 *
 *	HISTORY:		Diese Parameter waren ursprünglich in File ch_ig_piston.c angesiedelt.
 *					Durch das Projekt OL3711_PIPETTOR wurde eine gemeinsamme Quelle notwendig,
 *					um die Köpfe kompatibel zu halten. Deshalb wurden diese Tabellen in diesen
 *					File verlagert.
 *
 *******************************************************************/
#ifndef _HEAD_PISTON_TAB_
#define _HEAD_PISTON_TAB_

/** includes **/
#include "VolumeCorr_Well.h"

/** public data **/

// Strukturen für die Kolben,Anwendung im CyBi-Well und LID
struct HEAD_DEV {
	float PstDrvHoldCurrent;
	float PstMaxWay;
	float PstUpperPlay;			// in mm
	float PstGearRatio;			// Impulse pro µl
	float PstDrvMicroGearRatio;	// Impulse pro mm
	char *VolFrmt;	 			// Anzeigeformat für das Volumen
	float VolStartInc; 			// Werte für den Volumeneditor
	float VolEndInc;
	unsigned int PstCnt;		// Anzahl der Kolben
	float VolMax;			// Variable werden zur Plausibilitätskontrolle des
	float VolMin;				// Spitzenparameterbefehls benötigt
	float VolOver;// Max. überhub beim Dispensieren (zum Test bei Fremdeingabe)
	float AscMax;
	float AscMin;
	float ZeroMax;
	float ZeroMin;
	float TCWay;			// Fahrweg der Kolben nach oben beim Spitzenwechsel
	float Mech_MaxVolume;// max. Volumen von der Null-Lage bis zum oberen Anschlag = Weg bis Anschlag - mechanisches Spiel-0.1mm Reserve
};

// Anwendung im CyBi-Well und LID
struct TIPS_PARA {
	float asc;
	float zero;
	float MaxVol;
	float MinVol;
	float MoreDisp;
	float MoreSuck;
};

// Anwendung im CyBi-Well und LID
struct HEAD_TYPE {
	const char *type;
	const struct TIPS_PARA *tips;
	const struct HEAD_DEV *dev;
	VolumeCorr_t *pudtVolCorr;
};

extern const struct HEAD_DEV HeadDev_OL3383_8;

// Z-Nr.: OL3383-14-730; Gerätenr.: OL3383 7 ####; 96/25 µl
extern const struct HEAD_DEV HeadDev_OL3383_7;

// Z-Nr.: OL3383-14-630; Gerätenr.: OL3383 6 ####; 384/25 µl
extern const struct HEAD_DEV HeadDev_OL3383_6;

// Z-Nr.: OL3714-14-630; Gerätenr.: OL3714 6 ####; 384/25 µl  LHA
extern const struct HEAD_DEV HeadDev_OL3714_6;

// Z-Nr.: OL3383-14-650; Gerätenr.: OL3383 5 ####; 384/40 µl
extern const struct HEAD_DEV HeadDev_OL3383_5;

// Z-Nr.: OL3383-14-650; Gerätenr.: OL3383 2 ####; 96/40 µl
extern const struct HEAD_DEV HeadDev_OL3383_2;

// Z-Nr.: OL3384-14-630; Gerätenr.: OL3384 6 ####; 384/60 µl,Kolbendurchmesser: 1.80 mm
extern const struct HEAD_DEV HeadDev_OL3384_6;

// Z-Nr.: OL3384-14-730; Gerätenr.: OL3384 7 ####; 96/60 µl,Kolbendurchmesser: 1.80 mm
extern const struct HEAD_DEV HeadDev_OL3384_7;

// Z-Nr.: OL3384-14-230; Gerätenr.: OL3384 2 ####; 96/2.5 µl
extern const struct HEAD_DEV HeadDev_OL3384_2;

// Z-Nr.: OL3384-14-130; Gerätenr.: OL3384 1 ####; 384/2.5 µl
extern const struct HEAD_DEV HeadDev_OL3384_1;

// Z-Nr.: OL3387-14-130; Gerätenr.: OL3387 8 	 ####; Hummingwell-Kopf
extern struct HEAD_DEV HeadDev_OL3387_8;

// Z-Nr.: OL5021-14-130; Gerätenr.: OL5021 1 ####; 1536/8.0 µl
extern const struct HEAD_DEV HeadDev_OL5021_1;

extern const char OL3383_5[];	// 384/40 µl
extern const char OL3383_2[];	//  96/40 µl
extern const char OL3383_6[];	// 384/25 µl
extern const char OL3383_7[];	//  96/25 µl
extern const char OL3383_8[];	//  96/250 µl
extern const char OL3384_6[];	//  384/60 µl
extern const char OL3384_7[];	//  96/60 µl
extern const char OL3384_2[];	//  96/2,5 µl
extern const char OL3384_1[];	// 384/2,5 µl
extern const char OL3384_15[];	// 384/8 µl
extern const char OL3711_6[];	// 384/25 µl LID (techn. Daten wie OL3383 6)
extern const char OL3714_6[];	// 384/25 µl LHA (techn. Daten wie OL3383 6)
extern const char OL3387_8[];// Hummingwell-Kopf	(384 bzw. 96 siehe Magazinkodierung)
extern const char OL5021_1[];	//1536/10µl
extern const char OL5021_12[]; 	//1536/10µl
extern struct HEAD_TYPE HeadTypeTab[];

extern unsigned int GetSize_HeadTypeTab(void);

#endif

