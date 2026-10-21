/*
 * _uart_terminal.h
 *
 *  Created on: 04.11.2022
 *      Author: Mario Kuban
 */

#ifndef INC__UART_TERMINAL_H_
#define INC__UART_TERMINAL_H_

/* Exported functions prototypes ---------------------------------------------*/
#define	STDIN_BUFLEN 128

#define ON 0
#define OFF 1

#define EXCEPT_NO		1	// Ereignisse an der Schnittstelle
#define EXCEPT_SYNC 	2
#define EXCEPT_BREAK 	3

#define Asc0DtrCtrl(x)		(HAL_GPIO_WritePin(nDTR3_GPIO_Port,nDTR3_Pin,x))	// DTR0 -> DTR3 (RTS3)

void UART3_Start(void);

int kbhit(void);				// Anzahl der über ASC0 eingegangenen Zeichen
unsigned char getch(void);			// liest ein Zeichen aus dem stdin-buffer
ubyte IsCmdComplete(void);// Auswertung Endezeichen eines Befehls "CR" bzw. "SYNC"
ubyte IsCmdInit(void);						//
ubyte isException(void);				// testet,ob BREAK (##) gesendet wurde
ubyte ExceptionCtrl(void);					//
void ResetInpBuffHost(void);				// Reset Schnittstellenbuffer

int _write(int fd, char *ptr, int len);		//redirection of printf
size_t _read(int fd, char *base, size_t size);

#endif /* INC__UART_TERMINAL_H_ */
