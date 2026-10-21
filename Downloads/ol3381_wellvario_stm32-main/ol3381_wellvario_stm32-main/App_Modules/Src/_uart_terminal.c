/*
 * _uart_terminal.c
 *
 *  Created on: 04.11.2022
 *      Author: Mario Kuban
 */

#include <main.h>
#include <MAIN_CYBI.h>							// ... define (old) type definitions

// ---------- Inc
#include <_uart_terminal.h>
// ---------- Master
#include <ch_ig_head.h>
#include <ch_igel.h>
// ---------- Pipettor_Allgemein
#include <Well_Cmd.h>

extern UART_HandleTypeDef huart3;
static uint8_t UART3rxBuf[1];

extern uword g_uwHeadSafetyTime;

char InpBufferHost[STDIN_BUFLEN];
char *pchInpHost;			// Daten Inputzeiger
char *pchStartCmd;		// Aktueller Kommando Inputzeiger
bit g_bBreakFlag = FALSE;
char chLastInpChar;
bit g_bCR;
static bit m_bSync;

unsigned char ASC_Ready = 0;

void UART3_Start(void) {
	ResetInpBuffHost();
	UART_Start_Receive_IT(&huart3, UART3rxBuf, 1);
}

/* ******************************************************************************
 *  FUNCTION: ResetInpBufferHost
 *  PARAMETERS:	UART_HandleTypeDef is Pointer to Device
 *  DESCRIPTION: Interrupt routine für UART3, empfangenes Zeichen wird in Ringpuffer geschrieben
 *  RETURNS: none
 ******************************************************************************** */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART3) {
		char c = UART3rxBuf[0];
		UART_Start_Receive_IT(huart, UART3rxBuf, 1);
		*pchInpHost = c;
		if (c == SYNC) {
			m_bSync = TRUE;
		}

		// Test auf Abbruch
		if ((chLastInpChar == BREAK) && (*pchInpHost == BREAK)) {
			pchInpHost = InpBufferHost;
			*pchInpHost = 0;
			pchStartCmd = pchInpHost;
			chLastInpChar = 0;
			if (IsPermitHostMode())
				g_bBreakFlag = TRUE;
			return;
		}
		// Test auf überwachungszeichen beim Betrieb mit TC "]]"
		if ((chLastInpChar == ']') && (*pchInpHost == ']')) {
			g_uwHeadSafetyTime = TC_SAFETY_COUNT;// Refresh über Interrupt Schnittstelle durch "]]"
			pchInpHost = InpBufferHost;
			*pchInpHost = 0;
			pchStartCmd = pchInpHost;
			chLastInpChar = 0;
			return;
		} else
			chLastInpChar = *pchInpHost;
		pchInpHost++;
		if (pchInpHost == &InpBufferHost[STDIN_BUFLEN])
			pchInpHost = InpBufferHost;
		if (chLastInpChar == C_CR)
			g_bCR = TRUE;
	}
}

/* ******************************************************************************
 *  FUNCTION:		isbreak
 *  PARAMETERS:		none
 *  DESCRIPTION:	testet,ob BREAK (##) gesendet wurde
 *  RETURNS:		0 -> FALSE
 *					1 -> TRUE
 ******************************************************************************** */
bit isbreak(void) {
	if (g_bBreakFlag) {
		g_bBreakFlag = FALSE;
		return TRUE;
	}
	return FALSE;
}
/* ******************************************************************************
 *  FUNCTION: 		kbhit
 *  PARAMETERS:		none
 *  DESCRIPTION:
 *  RETURNS:		Anzahl der über ASC0 eingegangenen Zeichen
 ******************************************************************************** */
int kbhit(void) {
	register int diff;
	diff = pchInpHost - pchStartCmd;
	if (diff < 0) {
		diff += STDIN_BUFLEN;	//sizeof(InpBufferHost);
	}
	return diff;
}

/* ******************************************************************************
 *  FUNCTION:		getch
 *  PARAMETERS:		none
 *  DESCRIPTION:	liest ein Zeichen aus dem stdin-buffer
 *  RETURNS:		character from stdin-buffer
 ******************************************************************************** */
unsigned char getch(void) {
	ubyte ch = 0;
	//while(!(diff = kbhit()));
	ch = *pchStartCmd++;
	if (pchStartCmd >= (InpBufferHost + STDIN_BUFLEN)) {
		pchStartCmd = InpBufferHost;
	}
	return ch;
}

/* ******************************************************************************
 FUNCTION: 	IsCmdComplete
 PARAMETERS:	keine
 DESCRIPTION:	Auswertung Endezeichen eines Befehls "CR" bzw. "SYNC"
 RETURNS: 	TRUE / FALSE
 ******************************************************************************** */
ubyte IsCmdComplete(void) {
	if (m_bSync) {
		ResetInpBuffHost();
		WDO();
		printf("%c", ACK);
		fflush(stdout);
		g_bCR = FALSE;
		m_bSync = FALSE;
	}
	return g_bCR;
}

/* ******************************************************************************
 FUNCTION: 	IsCmdInit
 PARAMETERS:	keine
 DESCRIPTION:	Auswertung  ob HostMode Initialisert wurde und bereit ist
 RETURNS: 	TRUE / FALSE
 ******************************************************************************** */
ubyte IsCmdInit(void) {
	if (IsCmdComplete() && (*pchStartCmd == C_INIT))
		return TRUE;
	return FALSE;
}

/* ******************************************************************************
 *  FUNCTION: isException
 *  PARAMETERS:	none
 *  DESCRIPTION:testet,ob BREAK (##) gesendet wurde
 *  RETURNS:	0 -> FALSE
 *				1 -> TRUE
 ******************************************************************************** */
ubyte isException(void) {
	if (g_bBreakFlag)
		return TRUE;
	return FALSE;
}

/* ******************************************************************************
 *  FUNCTION: ExceptionCtrl
 *  PARAMETERS:	none
 *  DESCRIPTION: Abrufen und zurücksetzen der Breakflag
 *  RETURNS:	0 -> FALSE
 *				1 -> TRUE
 ******************************************************************************** */
ubyte ExceptionCtrl(void) {
	if (isException()) {
		g_bBreakFlag = FALSE;
		return TRUE;
	}
	return FALSE;
}

/* ******************************************************************************
 *  FUNCTION: ResetInpBufferHost
 *  PARAMETERS:	none
 *  DESCRIPTION: Reset Schnittstellenbuffer (UART3)
 *  RETURNS: none
 ******************************************************************************** */
void ResetInpBuffHost(void) {
	pchInpHost = InpBufferHost;
	*pchInpHost = 0;
	pchStartCmd = pchInpHost;
	chLastInpChar = 0;
	g_bBreakFlag = FALSE;
	g_bCR = FALSE;
	m_bSync = FALSE;
}

/* ******************************************************************************
 *  FUNCTION: _write
 *  PARAMETERS:	fd: File type (STDBUF_OUT), char* ptr (string to be printed), len (amount of chars)
 *  DESCRIPTION: Umleitung von printf auf UART3 (COM-PORT)
 *  RETURNS: none
 ******************************************************************************** */
int _write(int fd, char *ptr, int len) {
	HAL_StatusTypeDef hstatus;
	hstatus = HAL_UART_Transmit(&huart3, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	if (hstatus == HAL_OK)
		return len;
	return 0;
}
