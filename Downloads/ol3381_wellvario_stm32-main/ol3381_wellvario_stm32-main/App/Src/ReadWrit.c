/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
//#include "reent.h"

#include "stdio.h"

#include <main.h>
//#include "MAIN_CYBI.h"							// ... define (old) type definitions
#include <_uart_terminal.h>
/* ******************************************************************************
 *  FUNCTION: _ioread
 *  PARAMETERS:
 *  DESCRIPTION:
 *  RETURNS:
 ******************************************************************************** */
int _ioread( int fd ){
	/* At this point,it is known that 'fin' is a file opened for   */
	/* reading,and is not in error yet.							*/
	/* This routine should read the input for the given filehandle  */
	/* (in the standard version only 'stdin') from the required	 */
	/* hardware.													*/
	/* Whenever an error occurs,'EOF' should be returned		   */
	/* and fin->_flag should be set to the proper error value	   */
	/* i.e.		 _IOEOF on end of FILE						   */
	/*			  _IOERR on any other error					   */
	/* Note that when a read is done on 'stdin',this routine	   */
	/* should do the scanning,and whenever neccessary,do the	  */
	/* echoing to stdout											*/

	int c = EOF;
	if ( fd == 0 ){
	/* Assume terminal sending either CR or CRLF. Allow
		only LF as valid delimiter,so translate CR into LF
		and ignore received LF's */
		do{	/* blocking read from serial channel 0 */
			c = getch();			/* without echo */
			if ( c == 0x0a)		/* LF */
				c = EOF;		/* ignore it */
			else if(c == 0x0d)	/* CR */
				c = 0x0a;		/* turn into LF */
		} while (c<0);
		/* when reading 'stdin',echo to 'stdout' performing LF  == > CR-LF translation */
		//_iowrite( c,1 );
	}
	return( c );	/* return read character on succes */
}

/*************************************************************************
*																		*
*  DESCRIPTION :  Reads a block of characters from the given stream	  *
*				 returns nr of read characters						  *
*																		*
*************************************************************************/
size_t _read( int fd,char * base,size_t size ){
	int	c;
	size_t	cnt = 0;

	// At this point,it is known that 'fin' is a file opened for	//
	// reading,and is not in error yet.							//
	// This routine should read the input for the given filehandle	//
	// from the required hardware.									//
	// when nothing is read,'0' should be returned,meaning EOF 	//
	// is found												  	//
	// On any other error,the routine should return non zero		//
	// Note that when a read is done on 'stdin',this routine		//
	// should do the scanning,and whenever neccessary,do the	//
	// echoing to stdout										 	//

	for( ; size; size--,cnt++ )
	{
		// _ioread will echo to stdout when neccessary	//
		if( (c = _ioread( fd )) == EOF )
			break;	// Nothing more read	//
		*base++= c;
	}

	return( cnt );
}
