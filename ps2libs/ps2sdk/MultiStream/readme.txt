MultiStream V7.1
6/January/2004 (Happy New Year!)

Jason Page @ Sony Computer Entertainment Europe


General update. No major changes.

Files included in update:

EE
	Sound.C			Handles all of the SOUND_ functions which the user needs to call to make a sound.
					Handles EE->IOP control via SIF DMA
	Sound.H			List of all global functions within Sound.C

IOP/

	Stream.C		This is the main code for handling streaming/SPU. Decodes messages sent from
					Sound.C and calls relevent functions.
	Stream.H		Holds priority for .IRX file
	Entry.C			Initialises MultiStream thread
	bgmrs2.S		Assembly file, for converting PCM to PS2 PCM format
	ms_switches.h	Switch header file (allowes for removal of unwanted code)
	ms_dsp.c		DSP source code
	ms_dsp.h		DSP header


For sample code and .PDF documentation, please download the relevant MultiStream V7.0 files from www.ps2-pro.com

**** PLEASE REPLACE YOUR CURRENT V7.0 FILES WITH THE CORRESPONDING V7.1 FILES ****

Changes since V7.0:

1) Improved Append fuctionality.
2) DTS Vu0 code updated (#if added around structure decleration)
3) Fix for SOUND_MemAllocClose
4) SOUND_WaitForFileToLoad now handles EXTERNAL loading
5) NEXT_ENABLE and NEXT_DISABLE have been removed from kernal.h. IOP code updated to fix compile problems.
6) SOUND_InitCD problem fixed. Possible problem if passing SOUND_SET_CALLBACK.
7) Loading Stream channel only sent back to EE if FastLoad is OFF, or FastLoad is EXTERNAL loading. Otherwise -1 passed.
8) New Function: SOUND_SetStreamBuffer (written by Cory Bloyd @ High Voltage)