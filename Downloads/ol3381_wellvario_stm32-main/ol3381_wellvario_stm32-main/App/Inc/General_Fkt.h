/*******************************************************************
 *
 *	DESCRIPTION:
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/
#ifndef _GENERAL_FKT_
#define _GENERAL_FKT_


void BeepOn(void);
void BeepOff(void);
void Wait(unsigned int time);
unsigned char CheckInt(int min,int max,int ist);

unsigned char ReadAbortConditions(void);
void CopyRevPara(struct REV_PARA *dst,const struct REV_PARA *scr);
int ftoi(float f);

void SetResetRequest(void);
bool IsResetRequest(void);
void PerformSoftwareReset(void);
#endif
