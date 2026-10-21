//****************************************************************************
// @Module		Asynchronous Serial Interface (ASC1)
// @Filename	  ASC1.H
// @Project
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
// @Codegenerator
// @Description   This file contains all function prototypes and macros for
//				the Asynchronous Serial Interface 1 (on board from TQM).
//
//----------------------------------------------------------------------------
// @Date		  21.08.01 09:31:00
//
//****************************************************************************
#ifndef _LCD_H_
#define _LCD_H_

#define LCD_DIGITS 80

/* Kennung der LCD-Zeile für das Löschen */
#define NO_LINE 0
#define FIRST_LINE 1
#define SECOND_LINE 2
#define ALL_LINE 3

extern bool g_bCursorState;
extern UART_HandleTypeDef huart6;

struct CurPos{
	ubyte Line;
	ubyte Row;
};

struct LCD_DISP{
	char Buffer[LCD_DIGITS+2];
	ubyte Line;
	ubyte Row;
	ubyte CurState;
};


void LcdInit(void);											// Grundinitialisierung des Displays
void SetCursorOff(void);									// Kursor ausschalten
void SetCursorOn(DispString_t *ptr);				// Kursor einschalten als Block,blinkend
void SetCursorPos(ubyte line,ubyte column);					// Kursor positionieren
void GetCursorPos(struct CurPos *ptr);
void LcdShow(ubyte line,ubyte row,char *zk);				// Text ausgeben
void LcdClear(ubyte line);									// Display löschen,selektiv
void LcdClearAndShow(ubyte zeile,ubyte spalte,char *zk);	// Display löschen und Text ausgeben
ubyte LcdCheckPara(ubyte line,ubyte column,int zk_len);

void Display(DispString_t *ptr);					// Sprachabhängigen Text ausgeben
void ClearAllAndDisplay(DispString_t *ptr);		// Löschen des gesamten Display und sprachabhängigen Text ausgeben
void ClearLineAndDisplay(DispString_t *ptr);		// Löschen der ausgewählten Zeile und sprachabhängigen Text ausgeben
void ClearStringOnDisplay(DispString_t *ptr);	// Löschen eines srachabhängigen Textes auf dem Display

void RestoreDisplay(struct LCD_DISP *cPtr);
void SaveDisplay(struct LCD_DISP *cPtr);

void LcdInstallPattern(ubyte memory_no, ubyte *cPtr);	//  Benutzerdefinierte Zeichen erstellen

void printLCD(char* ptr, uint16_t size); 									// UART Sendebefehl

#endif
