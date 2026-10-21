/*******************************************************************
 *
 *	DESCRIPTION: Headerdatei
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _KEY_H_
#define _KEY_H_

 /** include files **/

 /* Definitionen für Fkt. Tast_Schrift */
	// #define C_INIT 'i' wird auch beim Abscannen verwendet!!!
 #define DEF_KEY 0
 #define NO_KEYS  DEF_KEY
 #define ENTER_KEY 1
 #define S_KEY 2				/*vorher C-TAST */
 #define ARR_DOWN_KEY 3
 #define ARR_UP_KEY 4
 #define ARR_RIGHT_KEY 5
 #define ARR_LEFT_KEY 6
 #define TRI_UP_KEY 7
 #define TRI_DOWN_KEY 8
 #define TRI_LEFT_KEY 9
 #define TRI_RIGHT_KEY 10

 #define BOTH_KEYS 17
 #define TRI_LEFT_RIGHT_KEYS 18
 #define TRI_ARR_UP_KEYS 19

 #define KEYBOARD_LAYOUT_DEFAULT 0
 #define KEYBOARD_LAYOUT_BEHIND_DEV 1

 #define CMD_COMPLETE 255	// Kennung Kommando vollständig

 #define REPEAT_SKEY_CNT 2	//Anzahl der Wiederholung bevor ein Abbruch ausgelöst wird
 #define REPEAT_EKEY_CNT 2	//Anzahl der Wiederholung bevor ein Enter ausgelöst wird
 extern  ubyte g_ubytKeyRepeateFlag;			/* Kennung auf Tastenwiederholung */
 extern ubyte g_ubytKeyboardLayout;
  
 /** global functions **/
 void KeyWaitForEnter(void);	//warte_enter()
 ubyte KeyWaitForAny(void); 	//warte_enter()
 ubyte KeyRequestFromHost(void);// prg_warte_tast(void) Tastaturabfrage vom Host
 ubyte KeyScan(void);	//tast_scan(void)
 ubyte tast_read(void);
 ubyte RequestSKey(void);	//Abfrage der Start / Stoptaste
 ubyte KeysCheckForSetup(void);
 ubyte WaitForAbortOrEnter(void);
 void WaitForRequestEnterKeyOnly(void);
#endif