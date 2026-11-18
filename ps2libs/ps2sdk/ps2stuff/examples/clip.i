/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */

	; creates:	clip_scales
	; modifies:	i
	.macro		init_clip_cnst
	; clip scales and value to clip against (2048 - arbitrary)
	loi			4.266666667 ; 2048.0/(320.0 * 1.5)
	addi.x		clip_scales, vf00, i
	loi			12.19047619 ; 2048.0/(112.0 * 1.5)
	addi.y		clip_scales, vf00, i
	loi			2.45E-4 ; 2048.0/8388607.5 rounded up a little to comp for fp error
	addi.z		clip_scales, vf00, i
	loi			2048.0
	addi.w		clip_scales, vf00, i
	.endm

	; modifies:	vi01 contains clip result (1 or 0)
	.macro		clip_vert		vert
	; scale
	mul.xyz		clip_vert\@, \vert, clip_scales
	clipw.xyz		clip_vert\@, clip_scales[w]
	; test last three vertices
	fcand		vi01, 0x003ffff
	iaddiu		vi01, vi01, 0x7fff
	.endm
