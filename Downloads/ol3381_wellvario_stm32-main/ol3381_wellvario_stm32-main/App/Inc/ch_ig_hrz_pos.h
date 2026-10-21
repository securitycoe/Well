/*******************************************************************
 *
 *	DESCRIPTION:	 Platzpositionen des Horizontalantriebes
 *
 *	AUTHOR:
 *
 *	HISTORY:
 *
 *******************************************************************/

#ifndef _HRZ_POS_H_
#define _HRZ_POS_H_

 #define DIFF 0

 /**** 3-PLatz-Wagen alle Angaben in mm von Platz 1 aus gemessen ****/
 #define POS3_1		0+DIFF	  /* Platz 1 */
 #define POS3_2		175+DIFF	/* Platz 2 */
 #define POS3_3		350+DIFF	/* Platz 3 */

/* Stackerpositionen linke Seite Platz 1 */
 #define POS3_4		240.5+DIFF	/* Schacht A nah,Platz 1,Position 4 */
 #define POS3_5		377.5+DIFF	/* Schacht B nah,Platz 1,Position 5 */

/* Stackerpositionen rechte Seite Platz 3 */
 #define POS3_6		-27.5+DIFF	/* Schacht A nah,Position 6 */
 #define POS3_7		109.5+DIFF	/* Schacht B nah,Position 7 */

 /* Plätze für die Kombiwanne,evtl. mit Servo-Offset verwenden */
 #define POS3_43	2.25+DIFF   /* Platz 1,Position 43 */
 #define POS3_44	177.25+DIFF	/* Platz 2,Position 44 */
 #define POS3_45	352.25+DIFF	/* Platz 3,Position 45 */
 
 #define POS3_47	-2.25+DIFF  /* Platz 1,Position 47 */
 #define POS3_48	172.75+DIFF	/* Platz 2,Position 48 */
 #define POS3_49	347.75+DIFF	/* Platz 3,Position 49 */

 /**** OL3714-14-940,LHA 3-PLatz-Wagen alle Angaben in mm von Platz 1 aus gemessen ****/
 #define POS35_1	0+DIFF	  /* Platz 1 */
 #define POS35_2	232.5+DIFF	/* Platz 2 */
 #define POS35_3	407.5+DIFF	/* Platz 3 */

/* Stackerpositionen linke Seite Platz 1 */
 #define POS35_4 	245.5+DIFF	/* Schacht A nah,Platz 1,Position 4 */
 #define POS35_5	382.5+DIFF	/* Schacht B nah,Platz 1,Position 5 */

 /* Plätze für die Kombiwanne,evtl. mit Servo-Offset verwenden */
 #define POS35_43	2.25+DIFF   /* Platz 1,Position 43 */
 #define POS35_44	234.75+DIFF	/* Platz 2,Position 44 */
 #define POS35_45	409.75+DIFF	/* Platz 3,Position 45 */
 
 #define POS35_47	-2.25+DIFF  /* Platz 1,Position 47 */
 #define POS35_48	230.25+DIFF	/* Platz 2,Position 48 */
 #define POS35_49	405.25+DIFF	/* Platz 3,Position 49 */

 
 /**** 4-Platz-Wagen alle Angaben in mm von Platz 1 aus gemessen ****/
 #define POS4_1		0+DIFF	  /* Platz 1 */
 #define POS4_2		175+DIFF	/* Platz 2 */
 #define POS4_3		350+DIFF	/* Platz 3 */
 #define POS4_8		525+DIFF	/* Platz 4,Position 8 */
 
 /* Für Spezialadapter "Inkubator" */	
 #define POS4_30	-25.0+DIFF	/* Position 30 */
 
 /* Plätze für "Deep well" Platten des Hersteller Greiner */	
 #define POS4_31	4.5+DIFF	/* Platz 1,Position 31 */	
 #define POS4_32	179.5+DIFF	/* Platz 2,Position 32 */
 #define POS4_33	354.5+DIFF	/* Platz 3,Position 33 */
 #define POS4_38	529.5+DIFF	/* Platz 4,Position 38 */

 /* Plätze für spezielle Gefäße TRANSKRIPT */
 #define POS4_39	-4.5+DIFF	/* Platz 1,Position 39 */	
 #define POS4_40	170.5+DIFF	/* Platz 2,Position 40 */
 #define POS4_41	345.5+DIFF	/* Platz 3,Position 41 */
 #define POS4_42	520.5+DIFF	/* Platz 4,Position 42 */

 /* Plätze für die Kombiwanne,evtl. mit Servo-Offset verwenden */
 #define POS4_43	2.25+DIFF   /* Platz 1,Position 43 */
 #define POS4_44	177.25+DIFF	/* Platz 2,Position 44 */
 #define POS4_45	352.25+DIFF	/* Platz 3,Position 45 */
 #define POS4_46	527.25+DIFF	/* Platz 4,Position 46 */

 #define POS4_47	-2.25+DIFF  /* Platz 1,Position 47 */
 #define POS4_48	172.75+DIFF	/* Platz 2,Position 48 */
 #define POS4_49	347.75+DIFF	/* Platz 3,Position 49 */
 #define POS4_50	522.75+DIFF	/* Platz 4,Position 50 */

 /* Stackerpositionen linke Seite Platz 1,Platz 2*/
 #define POS4_10	240.5+DIFF	/* Schacht A nah,Platz 1,Position 10 */
 #define POS4_4		377.5+DIFF  /* Schacht A weit,Schacht B nah,Platz 1,Position 4 */
 #define POS4_5		514.5+DIFF  /* Schacht B weit,Position 5 */
 #define POS4_20	415.5+DIFF  /* Schacht A nah,Platz 2,Position 20 */
 #define POS4_14	552.5+DIFF	/* Schacht A weit,Schacht B nah,Platz 2,Position 14 */	
 
 /* Stackerpositionen rechte Seite Platz 3,Platz 4*/	
 #define POS4_6		10.5+DIFF	/* Schacht A weit,Platz 4,Position 6 */						 
 #define POS4_7		147.5+DIFF  /* Schacht A nah,Schacht B weit,Platz 4,Positionen 7 */
 #define POS4_11	284.5+DIFF  /* Schacht B nah,Platz 3,Position 11 */
 #define POS4_17	-27.5+DIFF	/* Schacht A nah,Schacht B weit,Platz 3,Position 17 */	
 #define POS4_21	109.5+DIFF	/* Schacht B nah,Platz 3,Position 21 */

 /* Inkubatorpositionen linke Seite*/
 #define POS4_22	352.5+DIFF	/* Schacht A weit,Schacht B nah,Platz 1,Position 22 */
 #define POS4_23	489.5+DIFF	/* Schacht B weit,Platz 1,Position 23 */
 #define POS4_28	215.5+DIFF	/* Schacht A nah,Platz 1,Position 28 */
 /* Inkubatorpositionen rechte Seite*/
 #define POS4_25	35.5+DIFF	/* Schacht A weit,Platz 4,Position 25 */
 #define POS4_26	172.5+DIFF	/* Schacht A nah,Schacht B weit,Platz 4,Position 26 */
 #define POS4_27	309.5+DIFF	/* Schacht B nah,Platz 4,Position 27 */

 #define POS4_34	-25.0+DIFF	/* Platz 1 für Adapter "Inkubator" links stehend,Position 34 */	
 #define POS4_24	550.0+DIFF	/* Platz 4 für Adapter "Inkubator" rechts stehend,Position 24 */	

 #define POS4_120	552.5		/* Bahnende linke Seite */
 #define POS4_121	-27.5		/* Bahnende rechte Seite */


/**** Positionen des 4,5-Platzwagens OL3292-14-060 ****/
 /* alle Angaben in mm von Platz 1 aus gemessen */
 #define CAR45_1	0		/* Platz 1 */
 #define CAR45_2	175		/* Platz 2 */
 #define CAR45_3	350		/* Platz 3 */
 #define CAR45_8	525		/* Platz 4,Position 8 */
 
 /* Für Spezialadapter "Inkubator" */	
	//#define CAR45_30	-25.0	/* nicht realisierbar,Position 30 */
 
 /* Plätze für "Deep well" Platten des Hersteller Greiner */	
 #define CAR45_31	4.5		/* Platz 1,Position 31 */	
 #define CAR45_32	179.5	/* Platz 2,Position 32 */
 #define CAR45_33	354.5	/* Platz 3,Position 33 */
 #define CAR45_38	529.5	/* Platz 4,Position 38 */

 /* Plätze für spezielle Gefäße TRANSKRIPT */
 #define CAR45_39	-4.5	/* Platz 1,Position 39 */	
 #define CAR45_40	170.5	/* Platz 2,Position 40 */
 #define CAR45_41	345.5	/* Platz 3,Position 41 */
 #define CAR45_42	520.5	/* Platz 4,Position 42 */

 /* Plätze für die Kombiwanne,evtl. mit Servo-Offset verwenden */
 #define CAR45_43	2.25	/* Platz 1,Position 43 */
 #define CAR45_44	177.25	/* Platz 2,Position 44 */
 #define CAR45_45	352.25	/* Platz 3,Position 45 */
 #define CAR45_46	527.25	/* Platz 4,Position 46 */

 #define CAR45_47	-2.25	/* Platz 1,Position 47 */
 #define CAR45_48	172.75	/* Platz 2,Position 48 */
 #define CAR45_49	347.75	/* Platz 3,Position 49 */
 #define CAR45_50	522.75	/* Platz 4,Position 50 */

// folgende Positionen sind getestet und i.O
 /* Stackerpositionen linke Seite Platz 1,Platz 2*/
	//#define CAR45_10	328		/* nicht realisierbar,Schacht A nah,Platz 1,Position 10 */
 #define CAR45_4	359.5	/* Schacht A weit,Schacht B nah,Platz 1,Position 4 */
 #define CAR45_5	496.5	/* Schacht B weit,Position 5 */
	//#define CAR45_20	503		/* nicht realisierbar,Schacht A nah,Platz 2,Position 20 */
	//#define CAR45_14	640		/* nicht realisierbar,Schacht A weit,Schacht B nah,Platz 2,Position 14 */	
 
 /* Stackerpositionen rechte Seite Platz 3,Platz 4*/	
 #define CAR45_6	-307.5		/* Schacht A weit,Platz 4,Position 6 */					 
 #define CAR45_7	-170.5		/* Schacht A nah,Schacht B weit,Platz 4,Positionen 7 */
 #define CAR45_11	-33.5		/* Schacht B nah,Platz 3,Position 11 */
 #define CAR45_17	-345.5		/* Schacht A nah,Schacht B weit,Platz 3,Position 17 */	
 #define CAR45_21	-208.5		/* Schacht B nah,Platz 3,Position 21 */

 /* Positionen der Plätze 1-4 unterm DILUS,rechtsstehend vom CyBi-Well */
 #define CAR45_61 	-329.3		/* Platz 1 unterm DILUS*/
 #define CAR45_62	-154.3		/* Platz 2 unterm DILUS*/
 #define CAR45_63	  20.7		/* Platz 3 unterm DILUS*/
 #define CAR45_68	 195.7		/* Platz 4 unterm DILUS*/
 
 #define CAR45_120	525			/* Bahnende linke Seite */
 #define CAR45_121	-340		/* Bahnende rechte Seite */



/**** Positionen des 5-Platzwagens OL3292-14-050 ****/
// Standardplätze unterm Igel
 #define CAR5_1		0			// Platz 1
 #define CAR5_2		175			// Platz 2
 #define CAR5_3		350			// Platz 3
 #define CAR5_4		525			// Platz 4
 #define CAR5_5		700			// Platz 5
// Positionen mit einem Offset von 2,25mm unterm Igel
 #define CAR5_11	2.25		// Platz 1
 #define CAR5_12	177.25		// Platz 2
 #define CAR5_13	352.25		// Platz 3
 #define CAR5_14	527.25		// Platz 4
 #define CAR5_15	702.25		// Platz 5
// Positionen mit einem Offset von -2,25mm unterm Igel
 #define CAR5_21	-2.25		// Platz 1
 #define CAR5_22	172.75		// Platz 2
 #define CAR5_23	347.75		// Platz 3
 #define CAR5_24	522.75		// Platz 4
 #define CAR5_25	697.75		// Platz 5
// Positionen mit einem Offset von 4,5mm unterm Igel
 #define CAR5_31	4.5			// Platz 1
 #define CAR5_32	179.5		// Platz 2
 #define CAR5_33	354.5		// Platz 3
 #define CAR5_34	529.5		// Platz 4
 #define CAR5_35	704.5		// Platz 5
// Positionen mit einem Offset von -4,5mm unterm Igel
 #define CAR5_41	-4.5		// Platz 1
 #define CAR5_42	170.5		// Platz 2
 #define CAR5_43	345.5		// Platz 3
 #define CAR5_44	520.5		// Platz 4
 #define CAR5_45	695.5		// Platz 5
// Positionen unter Stackern von links beginnend
 #define CAR5_60	689.5		// Platz 1 Schacht B außen weit,Platz 2 Schacht B innen weit 
 #define CAR5_61	552.5		// Platz 1 Schacht A außen weit u. Schacht B außen nah,Platz 2 Schacht A innen weit u. Schacht B innen nah
 #define CAR5_62	727.5		// Platz 2 Schacht A außen weit u. Schacht B außen nah,Platz 3 Schacht A innen weit u. Schacht B innen nah
 #define CAR5_63	415.5		// Platz 1 Schacht A außen nah,Platz 2 Schacht A innen nah
 #define CAR5_64	590.5		// Platz 2 Schacht A außen nah,Platz 3 Schacht A innen nah
 #define CAR5_65	514.5		// Platz 1 Schacht B innen weit
 #define CAR5_66	377.5		// Platz 1 Schacht A innen weit u. Schacht B innen nah
 #define CAR5_67	240.5		// Platz 1 Schacht A innen nah
 #define CAR5_80	109.5		// Platz 3 Schacht B innen nah,Platz 4 Schacht B außen nah 
 #define CAR5_81	284.5		// Platz 4 Schacht B innen nah,Platz 5 Schacht B außen nah
 #define CAR5_82	459.5		// Platz 5 Schacht B innen nah
 #define CAR5_83	-27.5		// Platz 3 Schacht A innen nah u. Schacht B innen weit,Platz 4 Schacht A außen nah u. Schacht B außen weit
 #define CAR5_84	147.5		// Platz 4 Schacht A innen nah u. Schacht B innen weit,Platz 5 Schacht A außen nah u. Schacht B außen weit
 #define CAR5_85	322.5		// Platz 5 Schacht A innen nah u. Schacht B innen weit
 #define CAR5_86	10.5		// Platz 4 Schacht A innen weit,Platz 5 Schacht A außen weit
 #define CAR5_87	185.5		// Platz 5 Schacht A innen weit
// Positionen für den Inkubatoradapter auf Platz 1
 #define CAR5_100	664.5		// Platz 1 Schacht B außen weit 
 #define CAR5_101	527.5		// Platz 1 Schacht A außen weit u. Schacht B außen nah
 #define CAR5_102	390.5		// Platz 1 Schacht A außen nah
 #define CAR5_103	489.5		// Platz 1 Schacht B innen weit
 #define CAR5_104	352.5		// Platz 1 Schacht A innen weit u. Schacht B innen nah
 #define CAR5_105	215.5		// Platz 1 Schacht A innen nah
// Positionen für den Inkubatoradapter auf Platz 5
 #define CAR5_110	484.5		// Platz 5 Schacht B innen nah 
 #define CAR5_111	347.5		// Platz 5 Schacht A innen nah u. Schacht B innen weit
 #define CAR5_112	210.5		// Platz 5 Schacht A innen weit
 #define CAR5_113	309.5		// Platz 5 Schacht B außen nah
 #define CAR5_114	172.5		// Platz 5 Schacht A außen nah u. Schacht B außen weit
 #define CAR5_115	35.5		// Platz 5 Schacht A außen weit
	// Positionen außen
 #define CAR5_120	727.5		// Platz 1 links außen
 #define CAR5_121	-27.5		// Platz 5 rechts außen
	// Positionen für den Inkubatoradapter unterm CyBi-Well
 #define CAR5_122	-25.0		// Inkubatoradapter Platz1 unterm CyBi-Well
 #define CAR5_123	725.0 		// Inkubatoradapter Platz5 unterm CyBi-Well

/**** CyBi Disk Plätze ****/
 #define DISK10_1	0
 #define DISK10_2	800
 #define DISK10_3	1600
 #define DISK10_4	2400
 #define DISK10_5	3200
 #define DISK10_6	4000
 #define DISK10_7	4800
 #define DISK10_8	5600
 #define DISK10_9	6400
 #define DISK10_10	7200
#endif
