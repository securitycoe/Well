/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _IG_DEF_H_
#define _IG_DEF_H_

#include "Well_Defines.h"

#define LID 0
#define VARIO 1
#define DEV LID	// LID or VARIO is possible

#define BEEP_TIME 5
#define CYCLE_CNT 3	// 20 Test
#define VALVE_ON_TIME 100		/*  für Humming well 100*10ms = 1s */

// Kodierungen für die 7-Segmentanzeige  z.Z keine Anwendung
typedef enum _DISP {
	START_WITHOUT_NEW_INITIALIZATION = 0, START_WITH_NEW_INITIALIZATION
} DISP;

// Definition im File Well_Defines.h (allgemeine Well Definitionen
// Anwendung im CyBi-Well und LID

enum VOLUME_CORRECTION {
	VC_INTERN = 0, VC_EXTERN
};

// zZ. keine Anwendung
struct MOT_PARA {
	float start;
	float end;
	unsigned int cnt;
};

// Anwendung im CyBi-Well und LID
struct REV_PARA {
	float Start;
	float End;
	unsigned int cnt;
	float EndReduce;
};

// Anwendung im CyBi-Well und LID
struct DRV_ZK {
	unsigned char step_width;
	unsigned int zk;
};

// Anwendung im CyBi-Well
struct POS_CTRL {
	unsigned char no;
	float pos_x;
	float pos_y;
};

// Anwendung im CyBi-Well
struct ADJ_XY {
	DispString_t *msg;
	void (*fp)(signed char);
	int *way;
	const struct ZW_POS *adj_pos;
/* 	char pos;		   xy-Position des 384er Tisches */
};

// Anwendung im CyBi-Well
struct ZW_POS {
	int8_t no;
	char *show; /* Anzeige */
};

// Anwendung im CyBi-Well
struct XYAdj {
	int intXOffset;
	int intYOffset;
	int intXSlope;
	int intYSlope;
};

// Anwendung im CyBi-Well
struct XYPara {
	float XOffset;
	float YOffset;
	float XSlope;
	float YSlope;
};

// Anwendung im CyBi-Well und LID
struct UNIAdj {
	int intUniOffset;
	int intUniGain;
};

// Anwendung im CyBi-Well und LID
struct DEV_TYPE {
	const struct ZW_POS *mpos;
	int CntZwPos;
	const struct ADJ_XY *adj;
	int CntAdjXY;
	const struct POS_CTRL *pos;
	int CntPos;
};

/*** Anwendung im CyBi-Well ***/
typedef void (*pfct)(void);
typedef void (*pFct)(char *str);
typedef unsigned char (*ubytFct)(void);

struct Modus {
	DispString_t *msg;
//	void (*fp)(void);
	pfct fp;
};

// Defines Sicherheitskreis:
#define  WORKING_WITH_BLIND 0	// ersetzt: WORKING_WITHOUT_LS 0
#define  WORKING_WITH_LS 1		// Sicherheitskreis wird durch Lichtschranke gesteuert
#define  WORKING_WITH_TC 2		// Zusammenarbeit mit TipChanger,Jalousie entfüllt
// Ende Sicherheitskreis

#define  WORKING_WITH_SHIELD 0	// 0 ist kompatibel zu Geräten,die schon auf den Markt sind!
#define  WORKING_WITHOUT_SHIELD 1

#endif
