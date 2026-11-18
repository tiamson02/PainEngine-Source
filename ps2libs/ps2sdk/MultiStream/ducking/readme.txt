[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
 - Ducking - Automatic volume ramping (also known as Side chain compression)

<Description of the Sample Program>

	Automatically fades in / out one SPU core, depending on the current volume
	of what SPU core 1 is outputting. Effectively, this allows a characters voice
	to automatically make background ambience fade down when they speak. This is
	what is used in radio broadcasting, so that a DJ is always heard over the top
	of music.
	
<Files>
	main.c	
	graphics.c
	graphics.h
	tim2.c
	tim2.h
	tglogo.tm2 (/sampdata)
	VOCAL1.VAG (/sampdata)
	trafnorm.vag (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.

<Using the Controller>
     	Eks button	: DSP On/Off
	Triangle button	: Pauses/resumes the play of sound effect
	Square button	: Play vocal
		Up / Down	: Select parameter to change
	Left / Right	: Modify parameter
	

<Note>
	Refer to the attached MultiStream .pdf for the original functions
	used in this program and sound effect file format description.



