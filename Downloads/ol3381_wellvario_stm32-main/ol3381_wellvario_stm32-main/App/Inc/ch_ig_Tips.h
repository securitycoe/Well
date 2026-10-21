/*******************************************************************
 *
 *	DESCRIPTION: Header-File für Spitzen
 *
 *	AUTHOR:  Baumann
 *
 *	HISTORY:
 *
 *******************************************************************/


#ifndef _LPK_TIPS_H_
#define _LPK_TIPS_H_

#include "head_piston_tab.h"	//
#include "VolumeCorr_Well.h"

#define NO_TIPS 0	// Kennung keine Spitztyp gesetzt

struct TIP
{
	unsigned int Id;
	struct TIPS_PARA *VolPara;
	VolumeCorr_t *pVolCorr;
};

extern ubyte gTipNoState;

extern struct TIP CyBio60_50;
extern struct TIP CyBio25;
extern struct TIP CyBio60;
extern struct TIP CyBio250SW;
extern struct TIP CyBio250DW;


extern struct TIPS_PARA CyBioTip25;
extern struct TIPS_PARA CyBioTip60;
extern struct TIPS_PARA CyBioTip60_50;
extern struct TIPS_PARA CyBioTip250;

extern const struct TIPS_PARA TipsPara_OL3730_00;

extern struct TIP const* g_pTipsPara;
extern struct TIP *CyBioTips0[];
extern struct TIP *CyBioTips60[];
extern struct TIP *CyBioTips25[];
//extern const struct TIP *CyBioTips40[];
extern struct TIP *CyBioTips250[];

unsigned int GetTipNo(void);
//ubyte CheckAndSetTipNo(unsigned int);
ubyte CheckAndSetTipNo(unsigned int no,const struct TIP* const* list);
ubyte SetTipNo(unsigned int tip_no);
void SetDefaultTipId(void);


#endif
