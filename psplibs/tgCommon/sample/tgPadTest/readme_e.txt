[SCE CONFIDENTIAL DOCUMENT]
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
                Copyright (C) 2004-2005 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

tgPadTest

<Description of the Sample>

	This program sets up the basic template that just sets up draw
	environment and draws a polygon on screen, then the current
	pad variables are drawn on screen


<Files>

	"source"
	  	main.cpp	: Main program
   		makefile	: Makefile
		makefileSN	: SN Makefile


<Building the Program>

	Change to the "sample/tgPadTest/source" directory.

	To compile (builds sample/tgPadTest/source/tgPadTest.prx):
		% make

	To clean:
		% make clean

	After compilation, the program is activated from the debugger.


<Building the Program using SN makefiles>

	Change to the "sample/tgPadTest/source" directory.

	To compile (builds sample/tgPadTest/source/tgPadTestSN.prx):
		% make -f makefileSN

	To clean:
		% make -f makefileSN clean

	This has been tested under the following:
		- ProDG for PSP v1.2.0


<Building the Program using SN project files>
	
	Load tgPadTest.sln in the "sample/tgPadTest/proj/sn" directory.

	To compile (builds sample/tgBasic/source/tgPadTestSN.prx):
		% select "Build" menu then "Build Solution"

	To clean:
		% select "Build" menu then "Clean Solution"

	This has been tested under the following:
		- ProDG for PSP v1.2.0
		- ProDG for PSP VSI v1.5.1
		- Microsoft Visual Studio .NET v7.1


<Building the Program using CodeWarrior project files>

	Load tgPadTest.mcp in the "sample/tgPadTest/proj/cw" directory.

	To compile (builds sample/tgBasic/source/tgPadTestCW.prx):
		% select "Project" menu then "Make"

	To clean:
		% select "Project" menu then "Remove Object Code..."
	
	This has been tested under the following:
		- CodeWarrior for PSP R1.3


<Using the Controller>

	All button and stick values are output to screen


<Notes>

	Nov 23rd, 2005 (V1.0.1): Added CodeWarrior (CW PSP R1.3) and ProDG (VSI.NET v1.5.1) project files.

