[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
 - pitchdetect - Automatic pitch detection

<Description of the Sample Program>

	Automatically detects the frequency of a signal playing though the PS2.
	All frequency detection code is handled on the IOP. Due to the nature
	of the speed required to return a result for realtime use, the results
	are not exact, although	they are close enough (!)
	The results are given in Hz.
	
<Files>
	main.c	
	graphics.c
	graphics.h
	tim2.c
	tim2.h
	tglogo.tm2 (/sampdata)
	440SINE.VAG (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.

<Using the Controller>
     	Eks button	: DSP On/Off
	Triangle button	: Pauses/resumes the play of sound effect

<Note>
	Refer to the attached MultiStream .pdf for the original functions
	used in this program and sound effect file format description.
