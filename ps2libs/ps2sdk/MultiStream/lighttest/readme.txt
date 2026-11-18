[SCE CONFIDENTIAL DOCUMENT]
"PlayStation 2" Programmer Tool Runtime Library Release 2.8
         Copyright (C) 2000 - 2001 Sony Computer Entertainment Europe
                                                  All Rights Reserved

 MultiStream DSP
 - lighttest - "laser sword" sample

<Description of the Sample Program>

	"Emulation" of a laser sword. Note the difference in audio.
	With analog mode enabled, use the right stick to move the sword
	around. 	
	
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
	LIGHT2.VAG (/sampdata)
	LIGHTSTART.VAG (/sampdata)
	LIGHTEND.VAG (/sampdata)
	LIGHTHIT.VAG (/sampdata)
	jed.tm2 (/sampdata)
	sabre2.tm2 (/sampdata)
	tglogo.tm2 (/sampdata)
	mslogo.tm2 (/sampdata)

<Activating the Program>
	% make		: Compile
	% make run	: Execute

	Then, the data is played back. 
	Use the controller to play, stop, and resume the program.

<Using the Controller>
      	Eks button	: DSP On/Off
	Square button	: LightSword on/off
	Triangle button	: Pauses/resumes the play of sound effect
	

<Note>
	Refer to the attached Multistream.pdf for the original functions
	used in this program and sound effect file format description.

