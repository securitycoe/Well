/*  Definitionen für Präprozessor - Kommandokodierung*/

#ifndef _CMD_H_
#define _CMD_H_

#define ANSWER_BUF_LEN 127

struct ERR_MEM {
	unsigned char HostErr;
	unsigned char IntErr;
};

extern char *g_pchCmdBuf;
extern char g_acAnswerBuf[];
extern struct ERR_MEM g_udtErrorState;//Speicherung der internen,externen Fehlernummer
extern const char *VersionDate;
extern const char *VersionTime;
extern const uint16_t	gVersion_Major;
extern const uint16_t	gVersion_Minor;
extern const uint16_t	gVersion_BugFix;
extern const uint16_t	gVersion_Testbuild;

void CmdExec(void);
void CtrlExecute(void);
unsigned char CmdCopy(void);
void NextPara(void);
void Done(unsigned char state);
unsigned char CmdInit(void);
unsigned char CmdQuit(void);
unsigned char CmdDispCls(void);
unsigned char CmdDispPrint(void);
unsigned char CmdDispCurCtrl(void);
void CmdWaitForKey(void);
unsigned char CmdTestFlag(void);
unsigned char ComInitDrv(void);
unsigned char CmdHrzDrvNo(void);

unsigned char CmdSuck(void);
unsigned char CmdSuckOverstroke(void);
unsigned char CmdDispense(void);
unsigned char CmdSetLifePara(void);
unsigned char CmdPstSetTotalVolume(void);
unsigned char CmdPstCalcRev(void);

unsigned char CmdHrzDrvWay(void);
unsigned char CmdHrzDrvFunc(void);

unsigned char CmdVertDrvFunc(void);
unsigned char CmdVertDrvWay(void);
unsigned char CmdVertDrvSpeedLimit(void);

unsigned char CmdXYServoPos(void);
unsigned char CmdDrvCurrentToZero(void);
unsigned char CmdReadState(void);
unsigned char CmdDevReadErr(void);
unsigned char CmdXYServoExtension(void);
unsigned char CmdXYServoWay(void);
unsigned short CmdSlider(void);
unsigned char CmdSwitchCtrl(void);

// Funktionen vom Modul ch_ig_cmde
unsigned char CmdExtension(void);
unsigned char HeadSetPstCnt(long cycle);
unsigned char HeadSetTotalVolume(float vol);
unsigned char gVersion(char *buf);

#endif
