[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
 - carrevs3 - Car engine emulation

<Description of the Sample Program>

	"Emulation" of a car engine.
	With analog mode enabled, use the right stick to rev the car engine.
	
	This program uses libraries and modules on both of EE and IOP, 
	which are loaded automatically once the make is executed.
	Be sure to check that the necessary libraries and modules exist 
	under /ee and /iop.
	
<Files>
	main.c	
	graphics.c
	graphics.h
	tim2.c
	tim2.h
	tglogo.tm2 (/sampdata)
	CARREV.VAG (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.

<Using the Controller>
     	Eks button	: DSP On/Off
	Triangle button	: Pauses/resumes the play of sound effect
		L2 / R2		: Change gear (up / down)
	Right Analog UP : Rev engine
	

<Note>
	Refer to the attached MultiStream .pdf for the original functions
	used in this program and sound effect file format description.


