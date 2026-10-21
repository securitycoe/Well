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
#include "ch_ig_man.h"
#include "ch_igel.h"
#include "ch_ig_piston.h"
#include "lcd.h"
#include "ch_ig_key.h"
#include "ch_ig_z.h"
#include "ch_ig_xy.h"
#include "ch_ig_cmd.h"
#include "ch_ig_err.h"
#include "Well_Defines.h"
#include "Well_Error.h"
#include "ch_ig_config.h"

#define MAX_PART_VOL 24	  	 	/* max. Anzahl von Volumenzyklen */
#define MIN_RINSING_NO 1
#define MAX_RINSING_NO 20

/** local definitions **/

/** default settings **/

/** external data **/
extern DispString_t str_empty_rinse[];
extern DispString_t str_go_on[];
extern DispString_t str_ins_rinse[];

/** internal functions **/

/** public data **/

/** private data **/
static float m_fDosVolume,m_fPipVolume,m_fDilDispVolume[MAX_PART_VOL],m_fRinseVolume;

DispString_t str_dos_big[]		 = 	{{1,1,"DOSIEREN"}
									,{1,1,"DOSAGE"}};
DispString_t str_pip_big[]		 = 	{{1,1,"PIPETTIEREN"}
									,{1,1,"PIPETTING"}};
DispString_t str_dil_big[]		 = 	{{1,1,"DILUTIEREN"}
									,{1,1,"DILUTION"}};
DispString_t str_disp_big[]		 = 	{{1,1,"DISPENSIEREN"}
									,{1,1,"DISPENSING"}};
DispString_t str_vol_no_disp[]	 = 	{{2,1,"Anzahl der Volumina:"}
									,{2,1,"Total no. of dispenses:"}};
DispString_t str_vol_no_asp[]	 = 	{{2,1,"Anzahl der Volumina:"}
									,{2,1,"Total no. of aspirations:"}};
DispString_t str_suck_colon[]	 = 	{{2,1,"Ansaugvolumen:"}
									,{2,1,"Aspiration vol.:"}};
DispString_t str_suck_colon2[]	 = 	{{2,3,". Ansaugvol.:"}
									,{2,3,"st asp. vol.:"}};	//Ordnungszahl
DispString_t str_disp_colon2[]	 = 	{{2,3,". Aussto\342vol.:"}
									,{2,3,"st disp. vol.:"}};
DispString_t str_disp_colon[]	 = 	{{2,1,"Aussto\342volumen:"}
									,{2,1,"Dispensing vol.:"}};
DispString_t str_vol[]			 = 	{{1,99,". Volumen"}
									,{1,99,"st volume"}};
DispString_t str_tipping[]		 = 	{{2,1,"Abtupfpause"}
									,{2,1,"Tip blotting"}};
DispString_t str_big_rinsing[]	 = 	{{1,1,"SP\365LEN"}
									,{1,1,"RINSE"}};
DispString_t str_no_of_rinsing[] = 	{{2,1,"Anzahl der Sp\365lvorg\204nge:"}
									,{2,1,"Rinse cycles:"}};
DispString_t str_rinse_vol[]	 = 	{{2,1,"Volumen:"}
 									,{2,1,"Volume:"}};
DispString_t str_start[]		 = 	{{2,34,"Start:\002"}
					  				,{2,34,"Start:\002"}};
DispString_t str_vol_unit[]		 = 	{{2,10,"\346l"}
					  				,{2,10,"\346l"}};
DispString_t str_disp_res[]		 = 	{{2,1,"Restaussto\342"}
									,{2,1,"Purge remaining volume"}};
DispString_t str_vol_err[]		 = 	{{1,1,"Volumenfehler"}
 									,{1,1,"Volume error"}};
DispString_t str_vol_to_big[]	 = 	{{2,1,"Volumen ist zu gro\342"}
									,{2,1,"Total volume too high"}};
DispString_t str_user_abort[]	 = 	{{1,1,"Bedienerabbruch"}
									,{1,1,"Abort by user"}};
DispString_t str_aspirate[]		 = 	{{2,1,"Ansaugen"}
									,{2,1,"Aspiration"}};
DispString_t str_dispense[]		 = 	{{2,1,"Aussto\342en"}
									,{2,1,"Ejection"}};
DispString_t str_space[]		 = 	{{2,1,""}
									,{2,1,""}};					/* keine Maßeinheit */
DispString_t str_wait_time[]	 = 	{{2,1,"Wartezeit"}
									,{2,1,"Delay time"}};

/** private functions **/
 void prg_man_dos(void)
 {DispString_t *titel;
   char h_disp[40],h_disp1[20];
   if(m_fDosVolume < g_pudtTipsPara->MinVol)  m_fDosVolume = g_pudtTipsPara->MaxVol;
   titel = str_dos_big+g_ubytLang; ClearAllAndDisplay(str_dos_big);
   ShowLocation(TRUE);
   m_fDosVolume = ed_volume(m_fDosVolume,g_pudtTipsPara->MinVol,g_pudtTipsPara->MaxVol,str_suck_colon,SECOND_LINE);
   if(g_ubytSKeyFlag) return;
   sprintf(h_disp,"%s ",titel->message);
	// sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)m_fDosVolume);
   FloatFormatter(m_fDosVolume,g_pudtHeadDevPara->VolFrmt,h_disp1);
   strcat(h_disp,h_disp1);
	//h_disp[strlen(h_disp)-1] = 0;	//letzte Stelle vom Volumen wird ausgeblendet
   strcat(h_disp,str_vol_unit->message);
   LcdShow(titel->line,titel->column,h_disp);
   while(1)
   {if(prg_execution() == FALSE) return;  /* Ausführung beenden */
	 /* Neu: überprüfung des Volumens,da beim Spitzenwechsel ein anderes Magazin
			 zum Einsatz kommen könnte! */
	 if(VolumeOutOfRange(m_fDosVolume)) return;

	 if(cls_show_asp(str_aspirate,m_fDosVolume,Suck) == IERR_PROG_ABORT){AspDispAbortCycle();return;}
	 if(cls_show_disp(str_dispense,m_fDosVolume,TRUE,g_fDispWaitTime) == IERR_PROG_ABORT){AspDispAbortCycle();return;}
	 dispense_end_cycle();
   }
 }

 void prg_man_pip(void)
 {float suck_vol;
   DispString_t *titel;
   char h_disp[40],h_disp1[20];
   if(m_fPipVolume < g_pudtTipsPara->MinVol)  m_fPipVolume = g_pudtTipsPara->MaxVol;
   titel = str_pip_big+g_ubytLang; ClearAllAndDisplay(str_pip_big);
   ShowLocation(TRUE);

	   m_fPipVolume = ed_volume(m_fPipVolume,g_pudtTipsPara->MinVol,g_pudtTipsPara->MaxVol,str_disp_colon,SECOND_LINE);
	   if(g_ubytSKeyFlag) return;
	   sprintf(h_disp,"%s ",titel->message);
		// sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)m_fPipVolume);
	   FloatFormatter(m_fPipVolume,g_pudtHeadDevPara->VolFrmt,h_disp1);
	   strcat(h_disp,h_disp1);
	   strcat(h_disp,str_vol_unit->message);
	   LcdShow(titel->line,titel->column,h_disp);
	   suck_vol = ed_volume(m_fPipVolume+g_pudtTipsPara->MoreSuck,m_fPipVolume,g_pudtTipsPara->MaxVol+g_pudtTipsPara->MoreSuck,str_suck_colon,SECOND_LINE);
	   if(g_ubytSKeyFlag) return;
   while(1)
   {if(prg_execution() == FALSE) return;  /* Ausführung beenden */
	 /* Neu: überprüfung des Volumens,da beim Spitzenwechsel ein anderes Magazin
			 zum Einsatz kommen könnte! */
	 if(VolumeOutOfRange(suck_vol)) return;
	 if(cls_show_asp(str_aspirate,suck_vol,SuckWithOverstroke) == IERR_PROG_ABORT) {AspDispAbortCycle();return;}
	 if(cls_show_disp(str_dispense,m_fPipVolume,TRUE,0) == IERR_PROG_ABORT) {AspDispAbortCycle();return;}
	 cls_show_disp(str_disp_res,suck_vol-m_fPipVolume,TRUE,g_fDispWaitTime);
	 dispense_end_cycle();
   }
 }

/* FUNCTION: 	prg_OL3387_man_disp
   PARAMETERS:	---
   DESCRIPTION:	Pipettieren im manuellen Mode beim Hummingwell-Kopf (Ol3387)
   RETURNS: 	--- */
void prg_OL3387_man_disp(void)
{
   ClearAllAndDisplay(str_disp_big);
   ShowLocation(TRUE);
   while(1)
   {
	  if(prg_execution() == FALSE) return;  /* Ausführung beenden */
	  ClearLineAndDisplay(str_dispense);
	  ErrorDisplay(OL3387_ValveCtrl(VALVE_ON_TIME));
   }
}


 void prg_man_disp(void)
 {int count,i; float sum_vol,suck_vol,*iptr;
   DispString_t *titel; char h_disp[40],h_disp1[20];
   if(m_fDilDispVolume[0] < g_pudtTipsPara->MinVol)  m_fDilDispVolume[0] = g_pudtTipsPara->MaxVol;
   titel = str_disp_big+g_ubytLang;sum_vol = 0; ClearAllAndDisplay(str_disp_big);
   ShowLocation(TRUE);
   count = ed_int(3,1,MAX_PART_VOL,str_vol_no_disp,str_space,1,"%2u",SECOND_LINE,0);
   if(g_ubytSKeyFlag) return;
   sprintf(h_disp,"%s %2ux",titel->message,count);
   LcdShow(titel->line,titel->column,h_disp);
   /* Sammeln der Einzelvolumina */
   if(vol_assemble(count,str_disp_colon2,m_fDilDispVolume,&sum_vol) == FALSE) return ;
   sprintf(h_disp,"%s %2ux \344 = ",titel->message,count);
	//sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)sum_vol);
   FloatFormatter(sum_vol,g_pudtHeadDevPara->VolFrmt,h_disp1);
   strcat(h_disp,h_disp1);
   strcat(h_disp,str_vol_unit->message);
   LcdShow(titel->line,titel->column,h_disp);
//   suck_vol = ed_volume(sum_vol+PstDefOverstrokeSuck,sum_vol,PstMaxVolume+PstDefOverstrokeSuck,str_suck_colon,SECOND_LINE);
   suck_vol = ed_volume(sum_vol+g_pudtTipsPara->MoreSuck,sum_vol,g_pudtTipsPara->MaxVol+g_pudtTipsPara->MoreSuck,str_suck_colon,SECOND_LINE);
   if(g_ubytSKeyFlag) return;
   while(1)
   {if(prg_execution() == FALSE) return;  /* Ausführung beenden */
	 /* Neu: überprüfung des Volumens,da beim Spitzenwechsel ein anderes Magazin
			 zum Einsatz kommen könnte! */
	 if(VolumeOutOfRange(suck_vol)) return;
	 if(cls_show_asp(str_aspirate,suck_vol,SuckWithOverstroke) == IERR_PROG_ABORT) {AspDispAbortCycle();return;}
	 LcdClear(SECOND_LINE);iptr = m_fDilDispVolume;
	 for(i = 0;i<count;i++)
	 {sprintf(h_disp,"%s %2u%s:",str_dispense[g_ubytLang].message,i+1,str_vol[g_ubytLang].message);
		//show_volume(*iptr);
		//sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)*iptr);	//frt_volume
	   FloatFormatter(*iptr,g_pudtHeadDevPara->VolFrmt,h_disp1);
	   strcat(h_disp,h_disp1);
	   strcat(h_disp,str_vol_unit->message);
	   LcdShow(2,0,h_disp);DisplayAndEnter(str_start);
	   if((Dispense(*iptr,TRUE)) == IERR_PROG_ABORT) {AspDispAbortCycle();return;}
	   iptr++;
	 }
	 cls_show_disp(str_disp_res,suck_vol-sum_vol,TRUE,g_fDispWaitTime);
	 dispense_end_cycle();
   }
 }

 void prg_man_dil(void)
 {int count,i; float sum_vol,*iptr;
   DispString_t *titel; char h_disp[40],h_disp1[20];
   if(m_fDilDispVolume[0] < g_pudtTipsPara->MinVol)  m_fDilDispVolume[0] = g_pudtTipsPara->MaxVol;
   titel = str_dil_big+g_ubytLang; sum_vol = 0; ClearAllAndDisplay(str_dil_big);
   ShowLocation(TRUE);
   count = ed_int(3,1,MAX_PART_VOL,str_vol_no_asp,str_space,1,"%2u",SECOND_LINE,0);
   if(g_ubytSKeyFlag) return;
   sprintf(h_disp,"%s %2ux",titel->message,count);
   LcdShow(titel->line,titel->column,h_disp);
   /* Sammeln der Einzelvolumina */
   if(vol_assemble(count,str_suck_colon2,m_fDilDispVolume,&sum_vol) == FALSE) return;
   sprintf(h_disp,"%s %2ux \344 = ",titel->message,count);// show_volume(sum_vol);
	//sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)sum_vol);	//frt_volume
   FloatFormatter(sum_vol,g_pudtHeadDevPara->VolFrmt,h_disp1);
   strcat(h_disp,h_disp1);
   strcat(h_disp,str_vol_unit->message);
   LcdShow(titel->line,titel->column,h_disp);
   while(1)
   {if(prg_execution() == FALSE) return;  /* Ausführung beenden */
	 /* Neu: überprüfung des Volumens,da beim Spitzenwechsel ein anderes Magazin
			 zum Einsatz kommen könnte! */
	 if(VolumeOutOfRange(sum_vol)) return;
	 iptr = m_fDilDispVolume;
	 LcdClear(SECOND_LINE);
	 for(i = 0;i<count;i++)
	 {sprintf(h_disp,"%s %2u%s:",str_aspirate[g_ubytLang].message,i+1,str_vol[g_ubytLang].message);
  	//show_volume(*iptr);
		//sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)*iptr);		//frt_volume
	   FloatFormatter(*iptr,g_pudtHeadDevPara->VolFrmt,h_disp1);
	   strcat(h_disp,h_disp1);
	   strcat(h_disp,str_vol_unit->message);
	   LcdShow(2,0,h_disp); DisplayAndEnter(str_start);
	   if(Suck(*iptr) == IERR_PROG_ABORT){AspDispAbortCycle();return;}
	   iptr++;
	 }
	 if(cls_show_disp(str_dispense,sum_vol,TRUE,g_fDispWaitTime) == IERR_PROG_ABORT){AspDispAbortCycle();return;};
	 dispense_end_cycle();
   }
 }

 void prg_rinsing(void)
 {int count; struct LCD_DISP dHelp;
   char h_disp[40],h_disp1[20];
   SaveDisplay(&dHelp);
   count = 3;
   if(m_fRinseVolume < g_pudtTipsPara->MinVol)  m_fRinseVolume = g_pudtTipsPara->MaxVol;
   ClearAllAndDisplay(str_big_rinsing);	ShowLocation(TRUE);
   count = ed_int(count,MIN_RINSING_NO,MAX_RINSING_NO,str_no_of_rinsing,
					  str_space,1,"%2u",SECOND_LINE,0);
   sprintf(h_disp,"%s %2ux",str_big_rinsing[g_ubytLang].message,count);
   LcdShow(1,0,h_disp);
   m_fRinseVolume = ed_volume(m_fRinseVolume,g_pudtTipsPara->MinVol,g_pudtTipsPara->MaxVol,str_rinse_vol,SECOND_LINE);
   if(g_ubytSKeyFlag) {RestoreDisplay(&dHelp); return;}
   sprintf(h_disp,"%s  %2ux ",str_big_rinsing[g_ubytLang].message,count);
   sprintf(h_disp1,g_pudtHeadDevPara->VolFrmt,(double)m_fRinseVolume);	//frt_volume
   FloatFormatter(m_fRinseVolume,g_pudtHeadDevPara->VolFrmt,h_disp1);
   strcat(h_disp,h_disp1);
   strcat(h_disp,str_vol_unit->message);
   LcdShow(1,0,h_disp);
	// Ende
   ClearLineAndDisplay(str_start); KeyWaitForEnter();
   if(rinsing_exec(count) == IERR_PROG_ABORT) {AspDispAbortCycle(); return;}
   show_wait_ex(g_fDispWaitTime); dispense_end_cycle();
   RestoreDisplay(&dHelp);
 }

 unsigned char rinsing_exec(int count)
 {int i;char help[40];
   for(i = 0;i<count;i++)
   {sprintf(help,"%s %2u",str_aspirate[g_ubytLang].message,i+1);
	 LcdClearAndShow(2,0,help);if(Suck(m_fRinseVolume) == IERR_PROG_ABORT) return IERR_PROG_ABORT;
	 sprintf(help,"%s %2u",str_dispense[g_ubytLang].message,i+1);
	 LcdClearAndShow(2,0,help);if(Dispense(m_fRinseVolume,TRUE) == IERR_PROG_ABORT) return IERR_PROG_ABORT;
   }
  return CB_READY;
 }

/* FUNCTION: 	prg_execution
   PARAMETERS:	---
   DESCRIPTION:	Auswahl einer der ausführbaren Funktion "Start Exit ..."
   RETURNS: 	TRUE / FALSE */
unsigned char prg_execution(void)
{ubyte pos;
   const struct Modus *pMod;
   if(g_pudtHeadType->type == OL3387_8)	// Hummingwell-Kopf
   {
	  while(1)
	  {pos = ProgModification(OL3387_ExecuteFunctions,COUNT_OL3387_EX_F,0,SECOND_LINE);
		 pMod = &OL3387_ExecuteFunctions[pos];
		 if(pMod->fp == ProgExit) return FALSE;
		 if(pMod->fp == prg_man_dos) return TRUE;
		 execute_function[pos].fp();	//mod_ptr->fp();
		 ShowLocation(TRUE);
	  }
   }
   else
   {
	  while(1)
	  {pos = ProgModification(execute_function,COUNT_EX_F,0,SECOND_LINE);
		 pMod = &execute_function[pos];
		 if(pMod->fp == ProgExit) return FALSE;
		 if(pMod->fp == prg_man_dos) return TRUE;
		 execute_function[pos].fp();	//mod_ptr->fp();
		 ShowLocation(TRUE);
	  }
   }
}

float ed_volume(float ist,float min,float max,DispString_t *str,ubyte cls)
{
   return ed_float(ist,min,max,str,str_vol_unit,g_pudtHeadDevPara->VolStartInc,g_pudtHeadDevPara->VolEndInc,g_pudtHeadDevPara->VolFrmt,cls);//frt_volume
}

ubyte VolumeOutOfRange(float vol)
{
//   if((vol < PstMinVolume) || (vol > (PstMaxVolume+PstDefOverstrokeSuck))) {WarningDisplay(str_vol_err,str_vol_to_big);return TRUE;}
  if((vol < g_pudtTipsPara->MinVol) || (vol > (g_pudtTipsPara->MaxVol+g_pudtTipsPara->MoreSuck))) {WarningDisplay(str_vol_err,str_vol_to_big);return TRUE;}
  return FALSE;
}


ubyte cls_show_asp(DispString_t *s_ptr,float vol,unsigned char (*fp)(float)){
	ClearLineAndDisplay(s_ptr);
	return fp(vol);
}

ubyte cls_show_disp(DispString_t *s_ptr,float vol,ubyte enter_flag,float time)
{ubyte status;
  ClearLineAndDisplay(s_ptr);
  if(enter_flag) DisplayAndEnter(str_start);
  status = Dispense(vol,TRUE);if(status!= CB_READY) return status;
  show_wait_ex(time);
  return CB_READY;
}

ubyte show_wait_ex(float time)		/* Anzeige und Ausführung der Wartezeit [s]*/
{
  if(time == 0) return CB_READY;
  ClearLineAndDisplay(str_wait_time); Wait(time*100);	// da Zeitraster in 10ms
  return CB_READY;
}

void tipping_break(ubyte exec)  /* Anzeige,Enter,Nullage fahren */
{ubyte state;
  if(exec == TRUE)
  {state = VertDrvWayXYCtrl(0); if((state != CB_READY)&&(state != IERR_PROG_ABORT)) ErrorDisplay(state);
	ShowLocation(FALSE);
	ClearLineAndDisplay(str_tipping); DisplayAndEnter(str_go_on);}   /* neu */
  LcdClear(SECOND_LINE);
}

ubyte vol_assemble(ubyte count,DispString_t *s_ptr,float *vol_ptr,float *sum_vol)
{int i; char h_disp[40]; float pre_vol;
  LcdClear(SECOND_LINE); pre_vol = *vol_ptr;
  for(i = 0;i<count;i++)
  {
   sprintf(h_disp,"%2u",i+1);LcdShow(2,0,h_disp);
   *vol_ptr = ed_volume(pre_vol,g_pudtTipsPara->MinVol,g_pudtTipsPara->MaxVol,s_ptr,NO_LINE);
   pre_vol = *vol_ptr;
   if(g_ubytSKeyFlag) return FALSE;
   if((*sum_vol+= *vol_ptr) > (g_pudtTipsPara->MaxVol+ROUNDING_ERROR)) {WarningDisplay(str_vol_err,str_vol_to_big) ;return FALSE;}
   vol_ptr++;
  }
  return TRUE;
}


 void AspDispAbortCycle(void)   /* Abbruch durch den Bediener */
 {bool help;
   LcdClear(ALL_LINE); help = g_bShowSpots384; /* erlaubt Anzeige der Platznummern */
   g_bShowSpots384 = FALSE;
   while(1)
   {Display(str_user_abort); DisplayAndEnter(str_go_on);
	 WarningDisplay(str_empty_rinse,str_ins_rinse);
	 if(ErrorCheck(PstDrvPreZeroPos()) != CB_READY) continue;
	 if(ErrorCheck(PstDrvToZero())!= CB_READY) continue;
	 if(dispense_end_cycle() == CB_READY) break;
   }
   g_bShowSpots384	 = help;
 }

 ubyte dispense_end_cycle(void)
 {
	//Dispense(PstDefOverstrokeDisp,FALSE);tipping_break(blnTippState);
   ErrorDisplay(Dispense(g_pudtTipsPara->MoreDisp,FALSE));
   tipping_break(TRUE/*blnTippState*/);
	// if((state = PstDrvPreZeroPos()) != CB_READY) return state;	// evtl. bei 2,5 µl
   return ErrorCheck(PstDrvToZero());
 }
