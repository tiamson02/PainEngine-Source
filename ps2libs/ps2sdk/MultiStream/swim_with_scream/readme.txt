[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
 - swim - emulate person being pushed into a swimming pool. Also SCREAM compatible

<Description of the Sample Program>
	Shows the effects possible with a simple low pass filter.
	
<Files>
	main.c	
	graphics.c
	graphics.h
	tim2.c
	tim2.h
	tglogo.tm2 (/sampdata)
	SPLASH1.VAG (/sampdata)
	SWIMAMB.VAG (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.
	Please enable analog mode.

<Using the Controller>
     	Eks button	: DSP On/Off
	Triangle button	: Pauses/resumes the play of sound effect
 Left Analog Stick	: Swim direction
			 Square : Swim (use with left analog stick)
			 Circle : Jump into pool again
  	Left		Play SCREAM sound
	Down		Stop SCREAM sound
	Right		Load / Unload SCREAM bank
	L1/L2		Select SCREAM sound to play


<Note>
	Refer to the attached MultiStream .pdf for the original functions
	used in this program and sound effect file format description.

	Ensure that ms_switches.h has: #define SCREAM_COMPATIBLE 1
	And that the IOP stream.irx has been recompiled completely
