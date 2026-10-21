/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

#ifndef _HRZ_H_
 #define _HRZ_H_

 //#define HrzEncoder T2// T2 (Register) ist Encoder für Hrz-Antrieb

 #define CARR_0 0		// Anzahl der Wagenplätze 0 --> kein Wagen
 #define CARR_3 3
 #define CARR_4	4		// 4-Platzwagen   OL3391-940-14
 #define CARR_5 5		// 5-Platzwagen   OL3292-14-050
 #define CARR_45 45		// 4,5-Platzwagen OL3292-14-060
 #define CARR_35 35		// 3-Platzwagen   OL3714-14-940

 #define DISK_10 110

// #define DiskRawZeroRequest 	P5_8 == 0			// ReceiveRes3 - Grobnull (Encoder3)
// #define DiskFineZero			P5_0				// ReceiveRes1 - Feinnull Encoder1
 #define DiskRawZeroRequest		(nReceiveRes3_RD == 0)
 #define DiskFineZero			nReceiveRes1_RD

 #define DISK_ADJ_REV		20			//Umdr/min

 struct HRZ_POS_CTRL {ubyte no;
 					   float way;
 	 				 };
 struct HRZ_POS_TABLE {ubyte cartype;
 						const struct HRZ_POS_CTRL *tab;
 						uword size;
						const struct HRZ_CAR_PARA *CarPara;
					  };

 struct HRZ_CAR_PARA {	float  		MinRev;
						float  		MaxRev;
						unsigned int MinAcc;
						unsigned int MaxAcc;
						struct REV_PARA Rev;
					   /*	float  		DefStartRev;
						float  		DefEndRev;
						unsigned int DefAcc;
						float 		DefEndReduce; */
						float		WayMin;
						float		WayMax;
						float		HoldCurrent;
						float		GearRatio;
						float		Precision;
						float		UnitToEnc;
						const struct MAN_LOC *ManLoc;
					};

 struct MAN_LOC		 {	const ubyte *Location;
 						const ubyte Len;
 					 };

 extern ubyte g_ubytHrzActNo;
 extern ubyte g_ubytCarType;
 extern ubyte g_ubytHrzDrvState;
 extern bool g_bHrzDrvDir;
 extern long g_lHrzDrvSteps;
 extern short g_sHrzDrvTabWay;
 extern struct DRV_ZK *g_pudtHrzActZk,*pudtHrzActEndZk;
 extern struct DRV_ZK g_audtHrzZk[];
 extern const struct HRZ_POS_TABLE *g_pudtHrzPosTabPtr;

 extern struct REV_PARA m_udtHrzRevDef;

 ubyte HrzSetCarType(ubyte type);						// Parameter Schrittmotor Encoder für Tisch oder Drehtisch Model laden
 ubyte CarDrvCalibrate(void);							// Kalibrierung des Wagens bzw des Rundtisches
 ubyte HrzDrvCalibrate(void);							// Tisch Nullfahrt
 ubyte DiskInit(void);									// Drehtisch Nullfahrt
 ubyte HrzExtEncoderReset(void);						// externe Decoder (Drehtisch) zurücksetzen
 ubyte CarDrvNo(ubyte no);
 ubyte HrzDrvWay(float way);
 ubyte HrzInitZk(void);
 ubyte HrzCheckDrvPara(const struct REV_PARA *rev);
 ubyte HrzCalcDrvPara(const struct REV_PARA *rev);
 void CarDrvGetLimits(char *zk);
 void CarDrvGetPara(char *zk);
 void CarDrvSetCurrentToZero(void);
 ubyte CarWayCheck(float way);
 ubyte HrzDrvSpeedLimit(float speed);
 void CarNextPos(int8_t offset);
 ubyte HrzCheckAndSetRevToDefault(void);
 ubyte CarManAbortHandling(void);
 ubyte IsCarrType(ubyte type);
 float CarReadWayPos(void);
 float XReadPos(void);
 bool HrzZeroRequest(void);
 ubyte SetAndSaveHrzDrvParaToDefault(void);
 void CarDrvAbort(void);
#endif
