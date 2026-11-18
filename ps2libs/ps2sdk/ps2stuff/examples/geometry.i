/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */


	.macro		init_constants
	; gs offsets to center xformed vertex in gs coord space, also color clamping constant
	loi			2047.5
	addi.xy		gs_offsets, vf00, i
	loi			8388607.5
	addi.z		gs_offsets, vf00, i
	loi			12583167.0 ; clamp colors to 255 (255 + 12582912)
	maxi.w		gs_offsets, vf00, i

	fcset		0x000
	.endm

	.macro		load_vert_xfrm	xfrm
	lq			\xfrm[0], kVertexXfrm+0(vi00)
	lq			\xfrm[1], kVertexXfrm+1(vi00)
	lq			\xfrm[2], kVertexXfrm+2(vi00)
	lq			\xfrm[3], kVertexXfrm+3(vi00)
	.endm

	.macro		load_vert		vert
	lq.xyz		\vert, 0(next_input)
	.endm

	.macro		load_quad_adcs	adcs
	; adc bits for rendering quadrilaterals as tristrips
	lq			\adcs, kQuadADCs(buffer_top)
	.endm

	.macro		get_quad_adc	adc, adcs
	mtir			\adc, \adcs[x]
	mr32			\adcs, \adcs
	.endm

	.macro		set_vert_adc	vert, flag1, flag2=vi00
	ior			adc_bit\@, \flag1, \flag2
	mfir.w		\vert, adc_bit\@
	.endm

	; modifies:	q
	.macro		xform_vert	xformed_vert, vert_xform, vert
	mul_pt_mat_44	\xformed_vert, \vert_xform, \vert
	div			q, vf00w, \xformed_vert[w]
	mulq.xyz		\xformed_vert, \xformed_vert, q
	; visible vertices are now in range (+-320, +-112, +-2^24-1)
	.endm	

	.macro		vert_to_gs	gs_vert, vert
	; add screen offsets to xformed vertex
	add.xyz		\gs_vert, \vert, gs_offsets
	; convert to 4-bit fixed-point
	ftoi4.xyz		\gs_vert, \gs_vert
	.endm

	; texture

	.macro		load_stq		coords
	lq.xyz		\coords, 2(next_input)
	.endm

	.macro		xform_tex_stq	output, input, q
	; normalize  stq
	mulq.xyz		\output, \input, \q
	.endm

	.macro		load_normal	normal
	lq.xyz		\normal, 1(next_input)
	.endm

	.macro		load_rgba		rgba
	lq			\rgba, 1(next_output)
	.endm

	.macro		load_rgb		rgb
	lq.xyz		\rgb, 1(next_output)
	.endm

	.macro		store_stq		tex
	sq.xyz		\tex, 0(next_output)
	.endm
	.macro		store_rgb		rgb
	sq.xyz		\rgb, 1(next_output)
	.endm
	.macro		store_rgba	rgba
	sq			\rgba, 1(next_output)
	.endm
	.macro		store_xyz		xyz
	sq			\xyz, 2(next_output)
	.endm

	; set the adc bits at the beginnings of strips in this buffer

	.macro		set_adcs field
	; load adc bits and offset
	ilw.\field	adcs, 0(adcsPtr)
	iand			offset, adcs, offsetMask
	iand			adcs, adcs, adcMask
	iadd			offset, offset, firstVert

	ilw.w		vert1_w, 0(offset)
	ior			vert1_w, vert1_w, adcs
	isw.w		vert1_w, 0(offset)

	ilw.w		vert2_w, 3(offset)
	iadd			adcs, adcs, adcs ; shift up one bit
	ior			vert2_w, vert2_w, adcs
	isw.w		vert2_w, 3(offset)

	iand			adcs, stopMask, adcs ; this could be better..
	ibeq			adcs, stopMask, adcLoop_done_lid
	.endm

	.macro		set_strip_adcs
	iaddiu		firstVert, buffer_top, kOutputStart+3
	; see CVsmManager::XferBufferHeader for format
	iaddiu		offsetMask, vi00, 0x1fff
	iaddiu		adcMask, vi00, 0x7000 ; mask is 0xe000, / 2 = 0x7000
	iaddiu		adcMask, adcMask, 0x7000
	iaddiu		adcsPtr, buffer_top, kStripADCs
	iaddiu		lastAdcsPtr, adcsPtr, 4
	iaddiu		stopMask, vi00, 0x4000 ; 0x2000 shifted up one as above

	adcLoop_lid:

	set_adcs	x
	set_adcs	y
	set_adcs	z
	set_adcs	w

	; loop
	iaddiu	adcsPtr, adcsPtr, 1
	ibne	adcsPtr, lastAdcsPtr, adcLoop_lid

	adcLoop_done_lid:
	.endm
