/*******************************************************************
 *
 *	DESCRIPTION:	Ausgabefunktionen für das LCD-Display
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

/** include files **/
#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

#include "ch_igel.h"
#include "lcd.h"
#include "ig_define.h"
#include "Well_Defines.h"

/** local definitions **/

/** default settings **/

/** external functions **/

/** external data **/


/** internal functions **/
void ClearDispBuffer(ubyte line);

/** public data **/

/** private data **/
const char strSetCursorOff[] = "\033C0";
const char strSetCursorOn[] = "\033C3";
const char strSetCursorInvisible[] = "\033C0";
const char strSetCursorPos[] = "\033O";
const char strClearDisp[] = "\014";
const char strPatternDef[] = "\033Z";

#define UserEnter 2
#define UserStart 4
#define UserStop 5
ubyte strUserEnter[] = {0x00,0x01,0x05,0x0d,0x1f,0x0c,0x04,0x00};   //Enter
ubyte strUserStart[] = {0x18,0x14,0x12,0x11,0x12,0x14,0x18,0x00, 0x00}; //linker Teil von Stopp (pfeil)
ubyte strUserStop[] = {0xff,0x11,0x11,0x11,0x11,0x11,0xff,0x00};		//rechter Teil von Stopp (kasten)

struct CurPos aktCurPos;

bool g_bCursorState = FALSE;
char DispBuffer[LCD_DIGITS+2];	//Feld[0] wird nicht genutzt,Index beginnt bei 1,Endezeichen \000

void printLCD(char* ptr, uint16_t size){
	HAL_UART_Transmit(&huart6,(uint8_t *)ptr, size ,HAL_MAX_DELAY);
}

	// Anfangsinitialisierung der LCD-Anzeige
	// inkl. Hilfsvariablen einstellen
void LcdInit(void){	//getestet
	LcdInstallPattern(UserEnter,strUserEnter);
	LcdInstallPattern(UserStart,strUserStart);
	LcdInstallPattern(UserStop,strUserStop);
	aktCurPos.Line = 1;
	aktCurPos.Row = 0;
	ClearDispBuffer(ALL_LINE); DispBuffer[LCD_DIGITS] = 0;// Endekennung für Zeichenkette
	LcdClear(ALL_LINE); SetCursorOff();
}

void SetCursorOff(void){	// getestet
	g_bCursorState = FALSE;
	char msg[LCD_DIGITS+2];
	uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s",strSetCursorOff);
	printLCD(msg, len);
}

void SetCursorOn(DispString_t *ptr){
	g_bCursorState = TRUE;
	aktCurPos.Line = ptr->line;
	aktCurPos.Row = ptr->column;
	char msg[LCD_DIGITS+2];
	uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s%c%c",strSetCursorPos,aktCurPos.Row,aktCurPos.Line);
	printLCD(msg, len);
	len = snprintf(msg, LCD_DIGITS+2, "%s",strSetCursorOn);
	printLCD(msg, len);
}

	// Cursor auf Position setzen und einschalten
void SetCursorPos(ubyte line,ubyte row){	// getestet
	g_bCursorState = TRUE;
	aktCurPos.Line = line;
	aktCurPos.Row = row;
	char msg[LCD_DIGITS+2];
	uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s%c%c",strSetCursorPos,row,line);
	printLCD(msg, len);
	len = snprintf(msg, LCD_DIGITS+2, "%s",strSetCursorOn);
	printLCD(msg, len);
}

void GetCursorPos(struct CurPos *ptr){
	ptr->Line = aktCurPos.Line;
	ptr->Row = aktCurPos.Row;
}

// Diese Funktion wird nicht mehr für Längenprüfung benötigt, kann aber für Zeilen-/Spaltenprüfung bleiben
ubyte LcdCheckPara(ubyte line, ubyte column, int zk_len)
{
	if ((line > SECOND_LINE) || (line < FIRST_LINE) || (column > (LCD_DIGITS / 2)) || (column < 1)||(zk_len>LCD_DIGITS))
		return FALSE;
	return TRUE;
}


void LcdShow(ubyte line,ubyte row,char *zk)
{
	char *ptrDisp,*ptr;
	bool blnCurStateSave;

	blnCurStateSave = g_bCursorState;
	ptr = zk;
	SetCursorOff();
	SetCursorPos(line,row);
	char msg[LCD_DIGITS+2];
	uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s",ptr);
	printLCD(msg, len);
	ptrDisp = &DispBuffer[((line-1)*LCD_DIGITS/2)+row];	//Speichern der LCD-Anzeige im RAM
	while(*ptr != 0)
	{
		*ptrDisp = *ptr;
		ptr++;
		ptrDisp++;
	}
	if(blnCurStateSave == FALSE)
		SetCursorOff();
}

void LcdClear(ubyte line){
	ubyte i;
	char strHelp[LCD_DIGITS],*strPtr;
	if(line == NO_LINE) return;
	ClearDispBuffer(line);
	if(line == ALL_LINE){  	// Löschen des gesamten Anzeigebereiches
		char msg[LCD_DIGITS+2];
		uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s",strClearDisp);
		printLCD(msg, len);
		aktCurPos.Line = 1; aktCurPos.Row = 0;
		return;
	}
	strPtr = strHelp; for(i = 0;i<LCD_DIGITS/2;i++) *strPtr++= ' '; *strPtr = 0; /* ZK generieren*/
	LcdShow(line,1,strHelp);
}

void LcdClearAndShow(ubyte zeile,ubyte spalte,char *zk){	//cls_lcd_show(char zeile,char spalte,char *zk)
	LcdClear(zeile);LcdShow(zeile,spalte,zk);
}

void Display(DispString_t *ptr){	//show(struct String *ptr)
	ptr += g_ubytLang; //set language
	LcdShow(ptr->line,ptr->column,ptr->message);
}

void ClearAllAndDisplay(DispString_t *ptr){	//cls_disp(struct String *ptr)
	LcdClear(ALL_LINE);
	Display(ptr);
}

void ClearLineAndDisplay(DispString_t *ptr){	//cls_show(struct String *ptr)
	DispString_t *h_ptr;
	h_ptr = ptr+g_ubytLang;
	LcdClear(h_ptr->line);
	Display(ptr);
}

void ClearStringOnDisplay(DispString_t *ptr){	//cls_show_len(struct String *ptr)
	ubyte i,len; char strHelp[LCD_DIGITS],*strPtr;
	ptr += g_ubytLang;
	strPtr = strHelp;
	len = strlen(ptr->message);
	if(len>LCD_DIGITS) return;
	for(i = ptr->column;i<ptr->column+len;i++)	*strPtr++= ' ';
	*strPtr = 0; /* Ende der ZK */
	LcdShow(ptr->line,ptr->column,strHelp);
}

void ClearDispBuffer(ubyte line){
	ubyte i,pos,anz;
	switch (line){
	case FIRST_LINE:	pos = 0; anz = LCD_DIGITS/2; break;
	case SECOND_LINE:	pos = LCD_DIGITS/2+1; anz = LCD_DIGITS/2; break;
	default: pos = 0;	anz = LCD_DIGITS; break; 		// gesamtes Display
	}
	for(i = pos; i<= pos+anz; i++) DispBuffer[i] = ' ';
	DispBuffer[LCD_DIGITS+1] = 0;
}

void SaveDisplay(struct LCD_DISP *cPtr){
	strcpy(cPtr->Buffer,DispBuffer);
	cPtr->Line = aktCurPos.Line;
	cPtr->Row = aktCurPos.Row;
	cPtr->CurState = g_bCursorState;
}

void RestoreDisplay(struct LCD_DISP *cPtr){
	LcdClear(ALL_LINE);
	LcdShow(1,1,cPtr->Buffer+1);
	if(cPtr->CurState == TRUE){
		SetCursorPos(cPtr->Line,cPtr->Row);
	}else{
		SetCursorOff();
	}
}

//Installation von Sonderzeichen im Display-RAM
void LcdInstallPattern(ubyte memory_no, ubyte *cPtr){
	ubyte i;
	char msg[LCD_DIGITS+2];
	uint16_t len = snprintf(msg, LCD_DIGITS+2, "%s%c",strPatternDef,memory_no);
	printLCD(msg, len);	// Kommando + Speichernummer
	for(i = 0; i<8; i++){ //alle 8 zeilen ausgeben
		HAL_UART_Transmit(&huart6,(uint8_t *)cPtr++, 1 ,HAL_MAX_DELAY);
	}
	// Pattern ausgeben
}
