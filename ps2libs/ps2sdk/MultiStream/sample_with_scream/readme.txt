[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
	 - SCREAM sample - 

<Description of the Sample Program>
	Shows how to make MultiStream and SCREAM work together
	
<Files>
	main.c	
	graphics.c
	graphics.h
	tim2.c
	tim2.h
	tglogo.tm2 (/sampdata)
	test23.MIB (/sampdata)
	test23.MIH (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.
	Please enable analog mode.

<Using the Controller>
	Follow on-screen commands


<Note>
	Refer to the attached MultiStream .pdf for the original functions
	used in this program and sound effect file format description.

	Ensure that ms_switches.h has: #define SCREAM_COMPATIBLE 1
	And that the IOP stream.irx has been recompiled completely