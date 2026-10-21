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
 #include "ch_ig_key.h"
 #include "ch_ig_cmd.h"
 #include "ch_ig_z.h"
 #include "ch_igel.h"
#include <_uart_terminal.h>
/** local definitions **/
 #define KEY_TIME_FIRST 50  /* *10 ms */
 #define KEY_TIME_REP 1	  /* ...	  */
 #define KEY_MAX_FIELD 16	/* Anzahl der max. Tasten */

/** default settings **/

/** external data **/
 extern uword g_uwWaitTime;
 extern uword g_uwKeyRepeatTime;
 extern uword g_uwBeepTime;
 extern uword g_uwTimeout;
 extern uword g_uwHeadSafetyTime;

/** internal functions **/
 unsigned char RequestEnterKeyOnly(void);

 extern void KeySetRequest(uint16_t KeyNumber);

/** public data **/
 ubyte g_ubytKeyRepeateFlag;			/* Kennung auf Tastenwiederholung */
 ubyte g_ubytKeyboardLayout = KEYBOARD_LAYOUT_DEFAULT;

/** private data **/
 static const char m_acKeyArray[]	 =
						{TRI_RIGHT_KEY,TRI_DOWN_KEY,TRI_LEFT_KEY,0,TRI_UP_KEY,
			 			  0,0,ARR_UP_KEY,0,0,S_KEY,ENTER_KEY,0,ARR_RIGHT_KEY,
			 			  ARR_DOWN_KEY,ARR_LEFT_KEY
			 		  	};

 static unsigned char m_ucPreviousKey = 0;	/* Tastaturspeicher */
 static uword m_uwKeyTime = KEY_TIME_FIRST;	 	/* Wiederholzeit in 10 ms */
 static unsigned char m_ucRepeatFlag = FALSE;		/* Kennung für Wiederholung */
 static ubyte m_ubytKeyEvent[KEY_MAX_FIELD]; /* key_event[KEY_MAX_FIELD]; */

/** private functions **/
#ifdef KEYBORD_EPR3_REV01
/* FUNCTION: 	KeyScan
   PARAMETERS:	---
   DESCRIPTION:	Scannen der Tastatur
   RETURNS: 	Scancode */
ubyte KeyScan(void)
{	//tast_scan(void)
	ubyte i,c,j;
	c = NO_KEYS;
	j = 2;
	for(i = 0; i<KEY_MAX_FIELD; i++)
	{
		io4 = i;
		while(j--);	// Diese Zeit wird für den Einschwingvorgang benötigt tmin = 1µs
		if(!(io1&0x01))
		{
			m_ubytKeyEvent[i] = TRUE;
			c = m_acKeyArray[i];
		} /*Taste gedrückt*/
		else
			m_ubytKeyEvent[i] = FALSE;
	}
	WDO();
	return c;
}
#else

/* FUNCTION: 	KeyScan
   PARAMETERS:	---
   DESCRIPTION:	Scannen der Tastatur
   RETURNS: 	Scancode */
ubyte KeyScan(void)
{	//tast_scan(void)
	ubyte i,c,j;
	c = NO_KEYS;
	for(i = 0; i<KEY_MAX_FIELD; i++)
	{
		KeySetRequest(i);		// new Taste 0...15
		j = 100;				// new ggf. auch Hal_Delay(1)
		while(j--);				// Diese Zeit wird für den Einschwingvorgang benötigt tmin = 1µs
		if (!(nkey_in_RD))
		{ 	/*Taste gedrückt*/
			m_ubytKeyEvent[i] = TRUE;
			c = m_acKeyArray[i];
		}
		else
			m_ubytKeyEvent[i] = FALSE;
	}
	WDO();
	return c;
}
#endif

	// Tastaturabfrage
	// Return: Tastencode
ubyte tast_read(void)
{
	unsigned char c;
	c = KeyScan();
	/* Auswertung */
	//if(!g_bHostMode)
	{
		if(IsCmdComplete())
		{
			CtrlExecute();
			c = CMD_COMPLETE;
			return c;
		}
	}
	if(((c == TRI_UP_KEY) && (m_ubytKeyEvent[1]))||((c == TRI_DOWN_KEY) && (m_ubytKeyEvent[4])))
		c = BOTH_KEYS;
	if(g_bEnableDrive)
		return KeyMoveCtrl(c); /*auf_ab_keys(c);*/
	else
		return c;
}

#ifdef KEYBORD_EPR3_REV01
/* FUNCTION: 	RequestSKey
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der Abbruchtaste
   RETURNS: 	TRUE/FALSE */
unsigned char RequestSKey(void)
{
	 ubyte i;
	 io4 = 0x0a;	//Auswahl SKey
	 for(i = 0; i<15; i++) WDO();	// Warten auf das Einschwingen des Eingangssignales
	 if(!(io1&0x01))
		 return TRUE; /*Taste gedrückt*/
	 return FALSE;
}
#else
/* FUNCTION: 	RequestSKey
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der Abbruchtaste
   RETURNS: 	TRUE/FALSE */
unsigned char RequestSKey(void)
{

	//ab Version 1.42
	ubyte static bytSKeyCnt = REPEAT_SKEY_CNT;

	KeySetRequest(0x0a);			// new Abbruch Taste

	ubyte j;
	j = 100;							// new
	while(j--)		// Diese Zeit wird für den Einschwingvorgang benötigt tmin = 1µs
	if (!(nkey_in_RD))
	{					// new
		bytSKeyCnt -= 1;
		if(bytSKeyCnt == 0)
		{
			bytSKeyCnt = REPEAT_SKEY_CNT;
			return TRUE;
		}
		else
			return FALSE;
	}
	bytSKeyCnt = REPEAT_SKEY_CNT;
	return FALSE;
}

/* FUNCTION: 	RequestEnterKeyOnly
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der Start/Stopp-Taste
   RETURNS: 	TRUE/FALSE */
unsigned char RequestEnterKeyOnly(void)
{
	ubyte static bytEKeyCnt = REPEAT_EKEY_CNT;
	ubyte j;
	KeySetRequest(0x0b);			// Enter Taste
	j = 100;							// new
	while(j--)		// Diese Zeit wird für den Einschwingvorgang benötigt tmin = 1µs
	if (!(nkey_in_RD))
	{		//new
		bytEKeyCnt -= 1;
		if(bytEKeyCnt == 0)
		{
			bytEKeyCnt = REPEAT_EKEY_CNT;
			return TRUE;
		}
		else
			return FALSE;
	}
	bytEKeyCnt = REPEAT_EKEY_CNT;
	return FALSE;
 }

/* FUNCTION: 	WaitForRequestEnterKeyOnly
   PARAMETERS:	---
   DESCRIPTION:	Abfrage der Start/Stopp-Taste
   RETURNS: 	--- */
void WaitForRequestEnterKeyOnly(void)
{
	while(1)
	{
		WDO();
		if(RequestEnterKeyOnly())
		{
			BeepCtrl(BEEP_TIME);
			break;
		}
	}
}
#endif

ubyte KeyRequestFromHost(void)
{				// prg_warte_tast(void)		/* Tastaturabfrage vom Host */
	unsigned char c;
	while(!(c = KeyScan()))
	{
		WDO();
		if(ExceptionCtrl()) return NO_KEYS;
	}
	Wait(15);  /* 150 ms warten */
	return c;
}

	// Warten auf Tastendruck
	// Return: Tastencode
ubyte KeyWaitForAny()
{						//warte_tast()
	unsigned char c,not_key_flag;
	not_key_flag = FALSE;
	while(1)
	{
		WDO();
						while (!(c = tast_read()))
						{
							WDO();
							not_key_flag = TRUE;
							m_ucPreviousKey = 0;
						}
		switch (c)
		{
		case CMD_COMPLETE:
						return c;
		case S_KEY:
						/* keine Wiederholfunktion für S_Key,ENTER_KEY,C_INIT */
		case ENTER_KEY:
						if(not_key_flag == TRUE){
							BeepCtrl(BEEP_TIME);
							return c;
						}
						break;
		default:
						if(c != m_ucPreviousKey)
						{
							m_uwKeyTime = KEY_TIME_FIRST;
							m_ucRepeatFlag = FALSE;
							g_ubytKeyRepeateFlag = FALSE;
							BeepCtrl(BEEP_TIME);
							m_ucPreviousKey = c;
							return c;
						}
						if(m_ucRepeatFlag == FALSE)
						{
							g_uwKeyRepeatTime = m_uwKeyTime; m_ucRepeatFlag = TRUE;
							g_ubytKeyRepeateFlag = TRUE;
							continue;
						}
						if(g_uwKeyRepeatTime == 0)
						{
							m_uwKeyTime = KEY_TIME_REP;
							m_ucRepeatFlag = FALSE;
							return c;
						}
		}
	}
}

	// Warten auf das Betätigen der Enter-Taste
void KeyWaitForEnter(void)
{	//warte_enter()
	while (KeyWaitForAny()!= ENTER_KEY) WDO();
}


ubyte KeysCheckForSetup(void)
{
	ubyte key,i,triangle_up_count,triangle_down_count,arrow_right_count,stop_count,enter_count;
	ubyte arr_tri_up_count,arrow_down_count,arrow_up_count;
	triangle_up_count = 0;
	arrow_right_count = 0;
	stop_count = 0;
	enter_count = 0;
	arr_tri_up_count = 0;
	arrow_down_count = 0;
	triangle_down_count = 0;
	arrow_up_count = 0;
	for(i = 1; i < 100; i++)
	{
		key = KeyScan();
		//Hier können auch exotische Tastenkombinationen generiert werden
		if(((key ==  TRI_UP_KEY)&&(m_ubytKeyEvent[7]))||((key == ARR_UP_KEY)&&(m_ubytKeyEvent[4])))
			arr_tri_up_count++;
		if(key == TRI_UP_KEY)
			++triangle_up_count;
		if(key == TRI_DOWN_KEY)
			++triangle_down_count;
		if(key == ARR_RIGHT_KEY)
			++arrow_right_count;
		if(key == S_KEY)
			++stop_count;
		if(key == ENTER_KEY)
			++enter_count;
		if(key == ARR_DOWN_KEY)
			++arrow_down_count;
		if(key == ARR_UP_KEY)
			++arrow_up_count;
	}
	key = NO_KEYS;
	if((arr_tri_up_count > 60))
		return TRI_ARR_UP_KEYS;
	if(enter_count > 60)
		key = ENTER_KEY;
	if(stop_count > 60)
		key = S_KEY;
	if(triangle_up_count > 60)
		key = TRI_UP_KEY;
	if(triangle_down_count > 60)
		key = TRI_DOWN_KEY;
	if(arrow_right_count > 60 )
		key = ARR_RIGHT_KEY;
	if((arrow_down_count > 60))
		return ARR_DOWN_KEY;
	if((arrow_up_count > 60))
		return ARR_UP_KEY;
	return key;
}

ubyte WaitForAbortOrEnter(void)
{
	ubyte key;
	while(1){
		key = KeyWaitForAny();
		if(key == ENTER_KEY)
			break;
		if(key == S_KEY)
			break;
	}
	return key;
}
