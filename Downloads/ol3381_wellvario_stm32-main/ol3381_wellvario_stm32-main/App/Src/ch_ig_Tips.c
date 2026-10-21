/*******************************************************************
 *
 *	DESCRIPTION: Spitzen
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions
#include "head_piston_tab.h"	//

#include "VolumeCorr_Well.h"
#include "ch_ig_Tips.h"

//#include "DeviceConfig.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/
struct TIP const* g_pTipsPara;

/* Spitzenparameter Simulation für Transport- und Justieradapter */
struct TIPS_PARA CyBioTip_0				 = 	{0.0};

/* Spitzenparameter für CyBio-Spitzen 10µl */
struct TIPS_PARA CyBioTip10				 = 	{10.0	/* 0.01,2.0,1.0 */	};

/* Spitzenparameter für CyBio-Spitzen 25µl */
struct TIPS_PARA CyBioTip25				 = 	{25.0	/*0.01,2.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 25µl mit Filter */
struct TIPS_PARA CyBioTip25FI			 = 	{13.0	/*0.01,2.0,1.0 */	};	// mit Peter abgestimmt

/* Spitzenparameter für CyBio-Spitzen 25µl,am LPK 40µl möglich */
struct TIPS_PARA CyBioTip40				 = 	{40.0	/*0.01,2.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 25µl,am LPK 40µl möglich */
struct TIPS_PARA CyBioTip40FI			 = 	{15.0	/*0.01,2.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 40µl,am Adaptor */
struct TIPS_PARA CyBioTip40Adaptor		 = 	{40.0	/*0.01,2.0,1.0 */	};	// mit Konus auch 40µl

/* Spitzenparameter für CyBio-Spitzen 40µl,am Adaptor */
struct TIPS_PARA CyBioTip40AdaptorFI	 = 	{13.0	/*0.01,2.0,1.0 */	};	// mit Peter abgestimmt

/* Spitzenparameter für CyBio-Spitzen 60µl */
struct TIPS_PARA CyBioTip60				 = 	{60.0	/*0.01,4.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 60µl mit Filter */
struct TIPS_PARA CyBioTip60FI			 = 	{18.0	/*0.01,4.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 60µl  an Adapter	 = > Volumenreduzierung*/
struct TIPS_PARA CyBioTip60Adaptor		 = 	{50.0	/*0.01,4.0,1.0 */	};
/* Spitzenparameter für CyBio-Spitzen 60µl mit Filter am Adapter	 = > Volumenreduzierung*/
struct TIPS_PARA CyBioTip60AdaptorFI	 = 	{18.0 	/*0.01,4.0,1.0 */	};

/* Spitzenparameter für CyBio-Spitzen 250µl */
struct TIPS_PARA CyBioTip250			 = 	{250.0	/*0.1,70.0,4.0 */	};
/* Spitzenparameter für CyBio-Spitzen 250µl */
struct TIPS_PARA CyBioTip250FI			 = 	{250.0	/*0.1,70.0,4.0 */	};	//mit Konus auch 250µl





struct TIP CyBioNoTips		 = 	{NO_TIPS,&CyBioTip_0,(VolumeCorr_t*)NULL};
struct TIP CyBio10			 = 	{1,&CyBioTip10,(VolumeCorr_t*)NULL};		// Testeinträge: Volumenkorrektur
struct TIP CyBio25			 = 	{2,&CyBioTip25,&tVolumeCorr_OL3315_5__384_25};
struct TIP CyBio25FI		 = 	{3,&CyBioTip25FI,&tVolumeCorr_OL3315_5__384_25};

struct TIP CyBio40			 = 	{4,&CyBioTip40,(VolumeCorr_t*)NULL};		// CyBio-Spitze 25µl am LPK		Testeinträge: Volumenkorrektur
struct TIP CyBio40FI		 = 	{5,&CyBioTip40FI,(VolumeCorr_t*)NULL};		// CyBio-Spitze 25µl am LPK		Testeinträge: Volumenkorrektur

struct TIP CyBio60			 = 	{6,&CyBioTip60,&tVolumeCorr_OL3315_7__384_60};
struct TIP CyBio60FI		 = 	{7,&CyBioTip60FI,&tVolumeCorr_OL3315_7__384_60};

struct TIP CyBio250SW		 = 	{8,&CyBioTip250,&tVolumeCorr_OL3316_85__96_250SW};
struct TIP CyBio250DW		 = 	{9,&CyBioTip250,&tVolumeCorr_OL3316_85__96_250DW};
struct TIP CyBio250DW_FI	 = 	{10,&CyBioTip250FI,&tVolumeCorr_OL3316_85__96_250DW};

ubyte gTipNoState = FALSE;

/** private data **/

/** public functions **/

/** private functions **/
unsigned int GetTipNo(void)
{
	return g_pTipsPara->Id;
}

ubyte CheckAndSetTipNo(unsigned int no,	const struct TIP *const *list)
{
	ubyte state;
	const struct TIP* const* ptr;
	state = FALSE;

	ptr = list;

	while(*ptr != (const struct TIP*)NULL)
	{
		if((*ptr)->Id == no)
		{
		  	state = TRUE;
		  	g_pTipsPara = *ptr;
		  	break;
		}
		ptr++;
	}
	return state;
}
