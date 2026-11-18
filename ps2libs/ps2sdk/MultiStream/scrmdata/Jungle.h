/*****************************************************************************

	File created by SCEA Sound Tool (Windows)

	This bank has included names. This means you can trigger the sounds
	using the snd_PlaySoundByName API. The beauty of this system is that
	you (and the sound designer) no longer need to worry about rebuilding
	the executable whenever the sound designer removes sounds from the bank.

	If you use straight sound indexes to trigger sounds, one removed sound
	could throw all the indexes off.

	If you use the name #defines below as u_long128s you will not have to
	worry about this. If the specified sound isn't in the bank you will get
	a plain text message telling you the name of the sound that couldn't be
	found.

	The other advantage is that you can treat these u_long128s as null
	terminated strings. This is very useful for tools (e.g. letting the
	artist tag animation frames with sound IDs that will be resolved at
	run-time.) Using this method, the sound designer can supply the artist
	with a list of names, the artist can tag frames with strings that you
	stuff into your animation data as u_long128s, and everyone can hear
	the result WITHOUT the need to recompile the game!

	These u_long128s are 15 character strings... guaranteed to be null
	terminated.

	The index defines are still provided should you still decide to go that
	route.

*****************************************************************************/

#ifndef SND_BNK_JUNGLE
#define SND_BNK_JUNGLE	0x44454d4f

#ifndef SND_BNK_JUNGLE_NAME
	#define SND_BNK_JUNGLE_NAME	0x0000454c474e554a
#endif

#ifndef SND_INDX__COUNT_DELAY
	#define SND_INDX__COUNT_DELAY       0
	#define SND_NAME__COUNT_DELAY       0x0000000079616c65645f746e756f632eLL	// .count_delay
#endif

#ifndef SND_INDX__COUNT_CYCLE
	#define SND_INDX__COUNT_CYCLE       1
	#define SND_NAME__COUNT_CYCLE       0x00000000656c6379635f746e756f632eLL	// .count_cycle
#endif

#ifndef SND_INDX__COUNT_RANDOM
	#define SND_INDX__COUNT_RANDOM      2
	#define SND_NAME__COUNT_RANDOM      0x0000006d6f646e61725f746e756f632eLL	// .count_random
#endif

#ifndef SND_INDX__COUNT_WAIT
	#define SND_INDX__COUNT_WAIT        3
	#define SND_NAME__COUNT_WAIT        0x0000000000746961775f746e756f632eLL	// .count_wait
#endif

#ifndef SND_INDX__STEP_STONE
	#define SND_INDX__STEP_STONE        4
	#define SND_NAME__STEP_STONE        0x0000000000656e6f74735f706574732eLL	// .step_stone
#endif

#ifndef SND_INDX__RICO_METAL
	#define SND_INDX__RICO_METAL        5
	#define SND_NAME__RICO_METAL        0x00000000006c6174656d5f6f6369722eLL	// .rico_metal
#endif

#ifndef SND_INDX__GATE_LARGE
	#define SND_INDX__GATE_LARGE        6
	#define SND_NAME__GATE_LARGE        0x0000000000656772616c5f657461677eLL	// ~gate_large
#endif

#ifndef SND_INDX__C4
	#define SND_INDX__C4                7
	#define SND_NAME__C4                0x0000000000000000000000000034632eLL	// .c4
#endif

#ifndef SND_INDX__PELICAN
	#define SND_INDX__PELICAN           8
	#define SND_NAME__PELICAN           0x00000000000000006e6163696c65702eLL	// .pelican
#endif

#ifndef SND_INDX__ROBO_ARM_SWING
	#define SND_INDX__ROBO_ARM_SWING    9
	#define SND_NAME__ROBO_ARM_SWING    0x00676e6977735f6d72615f6f626f722eLL	// .robo_arm_swing
#endif

#ifndef SND_INDX__ENGINE_START
	#define SND_INDX__ENGINE_START      10
	#define SND_NAME__ENGINE_START      0x00000074726174735f656e69676e652eLL	// .engine_start
#endif

#ifndef SND_INDX__WIND_LIGHT
	#define SND_INDX__WIND_LIGHT        11
	#define SND_NAME__WIND_LIGHT        0x0000000000746867696c5f646e69777eLL	// ~wind_light
#endif

#ifndef SND_INDX__WIND_BLIZZARD
	#define SND_INDX__WIND_BLIZZARD     12
	#define SND_NAME__WIND_BLIZZARD     0x00006472617a7a696c625f646e69777eLL	// ~wind_blizzard
#endif

#ifndef SND_INDX__SHIP_CARGO_AMB
	#define SND_INDX__SHIP_CARGO_AMB    13
	#define SND_NAME__SHIP_CARGO_AMB    0x00626d615f6f677261635f706968737eLL	// ~ship_cargo_amb
#endif

#ifndef SND_INDX__CITY_AMB
	#define SND_INDX__CITY_AMB          15
	#define SND_NAME__CITY_AMB          0x00000000000000626d615f797469637eLL	// ~city_amb
#endif

#ifndef SND_INDX__JUNGLE_AMB
	#define SND_INDX__JUNGLE_AMB        20
	#define SND_NAME__JUNGLE_AMB        0x0000000000626d615f656c676e756a7eLL	// ~jungle_amb
#endif

#ifndef SND_INDX__GUN
	#define SND_INDX__GUN               21
	#define SND_NAME__GUN               0x0000000000000000000000006e75672eLL	// .gun
#endif


#define SND_BNK_JUNGLE_MAX_SOUND_ID	34
#define SND_BNK_JUNGLE_TOTAL_SOUNDS	35

#endif
