/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

#ifndef _PERI_H_
#define _PERI_H_

#include "typedefs.h"

//unused
extern unsigned char	io0;
extern unsigned char	io2;
extern unsigned char	io3;
extern unsigned char	io6;
extern unsigned char	io7;

#define HrzZeroRequest3714		nMechZero3			// Null-lage beim OL3714 mit 4-Platzwagen OL				// auch für Überwachung Kopfhaube S2 - Obsolete

#define HeadConnectionPin 		nMechZero1_RD		// Kopf kontaktiert

#define VertReadEndPos			Unten_RD			// IN0[0],	used,				Stage Unten,				High bei Abdeckung
#define VertReadAdvancePos		nVorwarn_RD			// IN0[1],	used,				Stage nVorwarn
#define HrzZeroRequestStd		nWagen_Null_Lage_RD	// IN0[2],	used,				Wagen Null-Lage,  			TLP1001 high at shielding,bei allen Wagentypen,außer OL3714

#define HeadRequestInsertPos	(io2&0x01)	// obsolet,	IN2_0,				PU S5 Kopf eingesetzt,		Wahr,Schalter betätigt
#define HeadReadPreWarnPos		(io2&0x10)	// obsolet,	IN2_4,				Kopflift nVorwarn
#define HeadReadLowerPos		(io2&0x20)	// obsolet,	IN2_5,				Kopflift unten
#define HeadReadUpperPos		(io2&0x40)	// obsolet,	IN2_6,				Kopflift oben
#define HeadRequestInsertPos	(io2&0x01)	// Wahr,Schalter betätigt

#define HeadDCMotorUpwards	io7 = 0x03	// obsolet 	0b00000011	Lifter dc-motor [      OUT2,EN]
#define HeadDCMotorDown		io7 = 0x05	// obsolet 	0b00000101	Lifter dc-motor [OUT1,      EN]
#define HeadDCMotorStopp	io7 = 0x07	// obsolet 	0b00000111	Lifter dc-motor [OUT1,OUT2,EN]

ubyte IsHeadShieldOpen(void);
ubyte IsLSInterrupted(void);
ubyte BlindOpenOrLSInterrupted(void);
ubyte SecurityCheckIsShieldOpen(void);
ubyte HeadReadChangeTool(void);
ubyte IsHeadCoverOpen(void);
ubyte IsChangeToolInsert(void);
ubyte IsHeadLowerPos(void);
ubyte IsHeadPreWarnPos(void);
ubyte IsHeadUpperPos(void);
ubyte IsHeadInsertPos(void);


ubyte HeadReadSwitchS1(ubyte *para);
void DrvEnable(void);
void DrvDisable(void);
#endif
