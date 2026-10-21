/*  Definitionen für Präprozessor - Grippersteuerung*/

#ifndef _GRP_H_
#define _GRP_H_

  /* FUNCTION: 	CmdGripperCtrl
	 PARAMETERS:	---
	 DESCRIPTION:	Kommando zur Steuerung des Grippers
	 RETURNS: 	CB_READY / ERROR */
  ubyte CmdGripperCtrl(void);

/* FUNCTION: 	GripperSelfTest
   PARAMETERS:	---
   DESCRIPTION:	Steuert den Gripper zu und auf (1x)
				Verwendung im Geräteselbsttest
   RETURNS: 	CB_READY / ERROR */
ubyte GripperSelfTest(void);

#endif