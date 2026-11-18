
MultiStream V7.0 . PS2 Multiple Streaming and SFX API.
8/August/2003 Jason Page @ SCEE (a division of SCEI R&D)

IOP source code

Overview.
---------
MultiStream is essentially sample code and an API that allows easy implementation of both streamed
audio and SFX (both in VAG format) into a product. Up to 48 mono
ADPCM streams can be played at one time, depending on IOP and SPU RAM
buffer sizes. Code for playback from either or both host0: and cdrom0:
is provided, so playback using audio files on CD-ROM can be easily tested without
any	rewriting of the sound code. Streams can be played as either looping or once off
and can be of any size. Stereo streaming is possible with the use of two streams.
MultiStreams own interleaved file format allows stereo (or more) files to be made into one,
single file. 
Streams	and SFX can be of any sample rate. Samples can be "chained" together to allow
a single stream to playback multiple samples in succession. This can then be used for
things like commentary. Routines for loading from your own CD routines have also been included.
The loading of resident samples (such as SFX) uses the same system as the playback of streams.
This allows you to load very large samples in to SPU RAM without needing to allocate a large
buffer in IOP RAM. Also, because the same system is used for SFX loading, it is possible to
load SFX while playing streamed music. Full support of the SPU hardware effects is
included. DMA access between EE and IOP is set to a minimum, due to the EE buffering all
commands, and only sending them to the IOP when requested to.
MultiStream also allows the user to load data into EE or IOP RAM using the same system. It is
therefore possible to play audio tracks while loading game data.
PCM playback is also handled, as is full 5.1 surround sound (in real time). 5.1 surround
requires the use of the DTS middleware solution.
Reverb units can also be controlled.
Four master sub volume groups can be created, so you can easily group audio together.
This would be required as most titles have an option screen for modifying the master SFX/Music
volumes.
MultiStream IOP->EE interface is non-blocking (if required), so processing of audio takes
very little EE time. Most other processing is handled by the IOP.
PCM playback is also possible.


Files.
------
EE/

Sound.C	Handles all of the SOUND_ functions which the user needs to call to make a sound.
	Handles EE->IOP control via SIF DMA
Sound.H	List of all global functions within Sound.C

IOP/

Stream.C This is the main code for handling streaming/SPU. Decodes messages sent from
	Sound.C and calls relevent functions.
Stream.H Holds priority for .IRX file
Entry.C	Initialises MultiStream thread
bgmrs2.S assembly file, for converting PCM to PS2 PCM format
ms_switches.h Switch header file (allowes for removal of unwanted code)
ms_dsp.c	DSP source code
ms_dsp.h	DSP header
