/****************************************************************************/
// @Module		
// @Filename	  ch_ig_config.h
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

//****************************************************************************
// @Defines
//****************************************************************************

#ifndef _CONFIG_H_
 #define _CONFIG_H_

  /*Sprachauswahl */
 /*#define LANG_NUM 2 Anzahl der Sprachen */
 #define GERMAN 0
 #define ENGLISH 1

 #define DZ_MIN  -0.5			 /* Tischkorrekturfaktor zum Ausgleich der Tischhöhentoleranz */
 #define DZ_MAX  +0.5			 /* Tischkorrekturfaktor zum Ausgleich der Tischhöhentoleranz */
 #define DZ_DEF 0

 extern float g_fDispWaitTime;

 void ProgSetDispWait(void);
 void ProgSetDz(void);
 void ChoicePlateTypeMode(void);
 void ProgSetSpeaker(void);
 void ProgYes(void); 
 void ProgNo(void); 
 ubyte DispWaitTimeCheck(void);
 ubyte DzCheck(void);
 ubyte LanguageCheck(void);
 void ProgGerman(void);
 void ProgEnglish(void);
 void SystemConfiguration(void);
 ubyte SystemVarCheck(void);
 void SystemVarSetToDefault(void);
 void SytemInitialization(void);
// void HeadPstSetup(void);
 void UserRequestAndPstCalibrate(void);
 void HrzVertDrvCalibrate(void);
 void UserRequestRinse(void);
 void ProgSetLanguage(void);
 void ConfigPumpManEndRev(void);
 void PstSetRevParaToDefault(const struct REV_PARA *para);

 ubyte EEPROMReadDeviceType(void);

#endif
