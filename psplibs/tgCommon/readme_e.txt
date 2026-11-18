[SCE CONFIDENTIAL DOCUMENT]
PSP(TM) Programmer Tool Runtime Library Release 2.6.0
                Copyright (C) 2004-2005 Sony Computer Entertainment Inc.
                                                     All Rights Reserved

tgCommon

<Description>
	
	The tgCommon library offers a modularized set of APIs providing common functionality 
	required to develop applications on PSP. 
	Its aim is to provide a common framework or source code repository for game and sample 
	code developers, which is TRC compliant for the PSP platform.
	
	The tgCommon package contains a source code library and some simple test samples.


<Building the Library>

	Change to the "source" directory.

	To compile (builds lib/tgCommon.a):
		% make

	To clean:
		% make clean
		

<Building the Library using SN makefiles>
	
	Change to the "source" directory.

	To compile (builds lib/tgCommonSN.a):
		% make -f makefileSN

	To clean:
		% make -f makefileSN clean

	This has been tested under the following:
		- ProDG for PSP v1.2.0


<Building the Library using SN project files>
	
	Load tgCommon.sln in the "proj/sn" directory.

	To compile (builds lib/tgCommonSN.a):
		% select "Build" menu then "Build Solution"

	To clean:
		% select "Build" menu then "Clean Solution"
	
	This has been tested under the following:
		- ProDG for PSP v1.2.0
		- ProDG for PSP VSI v1.5.1
		- Microsoft Visual Studio .NET v7.1


<Building the Library using CodeWarrior project files>

	Load tgCommon.mcp in the "proj/cw" directory.

	To compile (builds lib/tgCommonCW.a):
		% select "Project" menu then "Make"

	To clean:
		% select "Project" menu then "Remove Object Code..."
	
	This has been tested under the following:
		- CodeWarrior for PSP R1.3


<Notes>

	Nov 23rd, 2005 (V1.0.1): Added CodeWarrior (CW PSP R1.3) and ProDG (VSI.NET v1.5.1) project files.

