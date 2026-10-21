//****************************************************************************
// @Module
// @Filename	  ch_ig_man.h
// @Project	   Ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
// @Codegenerator DAvE 2-0-0
//
// @Description   This is the include header file for all other modules.
//
//----------------------------------------------------------------------------
// @Date		  23.08.01 07:57:54
//
//****************************************************************************
#ifndef _MAN_H_
 #define _MAN_H_

//****************************************************************************
// @Defines
//****************************************************************************

 void prg_man_dos(void);
 void prg_man_pip(void);
 void prg_man_disp(void);
 void prg_man_dil(void);
 void prg_rinsing(void);
 float ed_volume(float ist,float min,float max,DispString_t *str,ubyte cls);
 unsigned char prg_execution(void);
 ubyte VolumeOutOfRange(float vol);
 ubyte cls_show_asp(DispString_t *s_ptr,float vol,unsigned char (*fp)(float));
 void AspDispAbortCycle(void);   /* Abbruch durch den Bediener */
 ubyte cls_show_disp(DispString_t *s_ptr,float vol,ubyte enter_flag,float time);
 ubyte dispense_end_cycle(void);
 ubyte show_wait_ex(float time);		/* Anzeige und Ausführung der Wartezeit */
 void tipping_break(ubyte exec);  /* Anzeige,Enter,Nullage fahren */
 ubyte vol_assemble(ubyte count,DispString_t *s_ptr,float *vol_ptr,float *sum_vol);
 unsigned char rinsing_exec(int count);
 void prg_OL3387_man_disp(void);

#endif

