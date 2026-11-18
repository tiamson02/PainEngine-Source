/*****************************************************************************

	File created by SCEA Sound Tool (Windows)

	The names array is a bunch of u_long128s. They match the #defines in the
	".h" file.

	I DO NOT RECOMMEND USING THIS ARRAY OF U_LONG128!!!!!!!

	The sound designer might remove sounds at some point. This would leave your
	indexes into this array all out of whack! No... you should use the #defined
	names (u_long128) found in the other header file!

	I use this data when building auditioning tools, that allow a designer to
	scroll through all the sounds in a bank and play them.

*****************************************************************************/

#ifndef SND_BNK_JUNGLE_NAMES_H
#define SND_BNK_JUNGLE_NAMES_H

#ifndef SND_BNK_JUNGLE_NAME
	#define SND_BNK_JUNGLE_NAME	0x0000454c474e554a
#endif

u_long128 SND_BNK_JUNGLE_names[] = {
		0x0000000079616c65645f746e756f632eLL,    // .count_delay
		0x00000000656c6379635f746e756f632eLL,    // .count_cycle
		0x0000006d6f646e61725f746e756f632eLL,    // .count_random
		0x0000000000746961775f746e756f632eLL,    // .count_wait
		0x0000000000656e6f74735f706574732eLL,    // .step_stone
		0x00000000006c6174656d5f6f6369722eLL,    // .rico_metal
		0x0000000000656772616c5f657461677eLL,    // ~gate_large
		0x0000000000000000000000000034632eLL,    // .c4
		0x00000000000000006e6163696c65702eLL,    // .pelican
		0x00676e6977735f6d72615f6f626f722eLL,    // .robo_arm_swing
		0x00000074726174735f656e69676e652eLL,    // .engine_start
		0x0000000000746867696c5f646e69777eLL,    // ~wind_light
		0x00006472617a7a696c625f646e69777eLL,    // ~wind_blizzard
		0x00626d615f6f677261635f706968737eLL,    // ~ship_cargo_amb
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000626d615f797469637eLL,    // ~city_amb
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x0000000000626d615f656c676e756a7eLL,    // ~jungle_amb
		0x0000000000000000000000006e75672eLL,    // .gun
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL,    // 
		0x00000000000000000000000000000000LL     // 
	};

#endif
