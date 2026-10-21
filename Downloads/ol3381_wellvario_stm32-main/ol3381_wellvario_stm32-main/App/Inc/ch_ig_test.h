/****************************************************************************/
// @Module		
// @Filename	  ch_ig_test.h
// @Project	   Ch_ig.dav
//----------------------------------------------------------------------------
// @Controller	Infineon C167CR-L
//
// @Compiler	  TASKING C166
//
//----------------------------------------------------------------------------
// @Date		  08.03.2002
//
//****************************************************************************

//****************************************************************************
// @Defines
//****************************************************************************
#ifndef _TEST_H_
 #define _TEST_H_

 #define HEAD_TEST_WITHOUT_MAGAZINE FALSE		// Vergrößerte Zykluszahl,ohne Magazin lösen (für Dichtheitstest von neuen Dichtnippeln)

 struct AdjPos	{char space;
				  char **pShow;
				};

 ubyte TestPstReadLowerPos(void);
 ubyte TestPstReadUpperPos(void);
 ubyte TestPstReadFineZeroPos(void);
 ubyte TestPstReadCoarseZeroPos(void);
 ubyte TestReadEccentricSensorInFront(void);
 ubyte TestReadEccentricBackSensor(void);
 ubyte TestPstReadMagazineInsert(void);
 ubyte TestPstReadMagazinePara(void);
 ubyte DevTestLiftUpperPos(void);
 ubyte DevTestLiftLowerPos(void);
 ubyte DevTestLiftPreWarnPos(void);
 ubyte DevTestSwitchCover(void);
 ubyte DevTestSwitchShield(void);
 ubyte DevTestSwitchHeadInsert(void);
 ubyte DevTestHrzZeroPos(void);
 ubyte DevTestVertZeroPos(void);
 ubyte TestDrvEccentric(void);
 ubyte AdjTipsTightenMechanism(void);
 ubyte TestTipsTightenMechanism(void);	//Spitzenanzug mit Magazin testen
 ubyte AdjPstCoarseZeroPos(void);
 ubyte AdjPstFineZero(void);
 ubyte TestHeadSwitchS1(void);
 void AdjStackPos(void);
 void AdjStackPos35(void);
 void AdjStackPos4(void);
 void AdjStackPos5(void);
 void AdjDiskZero(void);
 ubyte DevTestBeep(void);
 ubyte DevTestIn0(void);
 ubyte DevTestIn2(void);
 ubyte DevTestIn3(void);
 ubyte DevTestOut7(void);
 void SelfTest(void);
 void HeadSelfTest(void);
 ubyte DevAdjustHeadLift(void);
 ubyte DevTestHeadLift(void);
 ubyte TestHeadLiftOneCycle(void);
 void LifterSelfTest(void);
 void PrgSelfTest(void);
 ubyte DevTestHeadContactX20(void);
 ubyte PtmeTimeTest(void);
 ubyte TestOL3387_Valve(void);
 ubyte Test_OL3387_ReadMagazinePara(void);

#endif
