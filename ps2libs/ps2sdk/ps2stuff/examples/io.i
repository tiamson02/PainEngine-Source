/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */


	.macro		init_out_buf
	; fill in the nloop field in the giftag and store at
	; top of the output buffer
	lq			gif_tag\@, kGifTag(vi00)
	mtir			eop\@, gif_tag\@x
	ior			eop\@, eop\@, num_verts
	mfir.x		gif_tag\@, eop\@
	sq			gif_tag\@, -1(next_output)
	.endm

	.macro		init_io_loop
	; input/output ptrs and strides (increments to next vertex data)
	xtop			buffer_top
	iaddiu		next_input, buffer_top, kInputStart
	iaddiu		next_output, buffer_top, kOutputStart+1

	; get the number of vertices in the buffer
	ilw.x		num_verts, kNumVertices(buffer_top)

	; when to stop
	iadd			last_input, next_input, num_verts
	iadd			last_input, last_input, num_verts
	iadd			last_input, last_input, num_verts
	.endm

	.macro		init_o_loop
	xtop			buffer_top
	iaddiu		next_output, buffer_top, kOutputStart+1
	; when to stop
	iadd			last_output, next_output, num_verts
	iadd			last_output, last_output, num_verts
	iadd			last_output, last_output, num_verts
	.endm

	.macro		init_stride
	lq			strides, kInputStrides(buffer_top)
	.endm

	.macro		next_io
	iaddiu		next_input, next_input, 3
	iaddiu		next_output, next_output, 3
	.endm

	.macro		next_o
	iaddiu		next_output, next_output, 3
	.endm

	.macro		next_io_stride
	; increment the input ptr -- for quads strides = 
	; quadsPerVert * (1, 1, 2, -1) and for everything else =
	; quadsPerVert * (1, 1, 1, 1)
	mtir			stride, strides[x]
	iadd			next_input, next_input, stride
	mr32			strides, strides

	iaddiu		next_output, next_output, 3
	.endm

	.macro		loop_io		loop_target
	ibne			next_input, last_input, \loop_target
	.endm

	.macro		loop_o		loop_target
	ibne			next_output, last_output, \loop_target
	.endm

	.macro		kick_to_gs
	iaddiu		packet_start, buffer_top, kOutputStart
	xgkick		packet_start
	.endm
