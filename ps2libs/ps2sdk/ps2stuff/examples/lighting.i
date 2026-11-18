/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */



	.macro		init_dlt_loop
	; get number of dir lights
	ilw.x		num_dir_lights, kNumLights(vi00)
	iaddiu		light_ptr_ptr, vi00, kLightPointers0
	ilw.x		light_ptr, 0(light_ptr_ptr)
	.endm	

	.macro		init_plt_loop
	; get number of point lights
	ilw.y		num_pt_lights, kNumLights(vi00)
	iaddiu		light_ptr_ptr, vi00, kLightPointers0
	ilw.y		light_ptr, 0(light_ptr_ptr)
	.endm	

	.macro		load_glob_amb	amb
	lq.xyz		\amb, kGlobalAmbient(vi00)
	.endm

	; materials

	.macro		load_mat_amb	amb
	lq.xyz		\amb, kMaterialAmbient(vi00)
	.endm
	.macro		load_mat_diff	diff, mask=xyz
	lq.\mask		\diff, kMaterialDiffuse(vi00)
	.endm
	.macro		load_mat_spec	spec
	lq.xyz		\spec, kMaterialSpecular(vi00)
	.endm
	.macro		load_mat_emm	emm
	lq.xyz		\emm, kMaterialEmission(vi00)
	.endm

	.macro		load_vert_eye	vec
	lq.xyz		\vec, kFixedVertToEye(vi00)
	.endm

	; lights

	.macro		load_lt_amb	amb
	lq.xyz		\amb, kLightAmbientOffset(light_ptr)
	.endm
	.macro		load_lt_diff	diff
	lq.xyz		\diff, kLightDiffuseOffset(light_ptr)
	.endm
	.macro		load_lt_spec	spec
	lq.xyz		\spec, kLightSpecularOffset(light_ptr)
	.endm
	.macro		load_lt_pos	pos
	lq.xyz		\pos, kLightPosOffset(light_ptr)
	.endm
	.macro		load_lt_atten	coeff
	lq.xyz		\coeff, kLightAttenCoeffOffset(light_ptr)
	.endm
	.macro		next_dir_light
	iaddiu		light_ptr_ptr, light_ptr_ptr, 1
	ilw.x		light_ptr, 0(light_ptr_ptr)
	.endm


	; load transpose of object to world transform
	.macro		load_o2wt		xform
	lq.xyz		\xform[0], kObjToWorldXfrmTrans+0(vi00)
	lq.xyz		\xform[1], kObjToWorldXfrmTrans+1(vi00)
	lq.xyz		\xform[2], kObjToWorldXfrmTrans+2(vi00)
	.endm

	.macro		load_w2o		xform
	lq.xyz	    \xform[0], kWorldToObjXfrm+0(vi00)
	lq.xyz	    \xform[1], kWorldToObjXfrm+1(vi00)
	lq.xyz	    \xform[2], kWorldToObjXfrm+2(vi00)
	lq.xyz	    \xform[3], kWorldToObjXfrm+3(vi00)
	.endm

	.macro		get_cnst_color	color
	lq.xyz		global_amb, kGlobalAmbient(vi00)
	lq.xyz		material_amb, kMaterialAmbient(vi00)
	mul.xyz		global_amb, global_amb, material_amb
	lq.xyz		material_emm, kMaterialEmission(vi00)
	add.xyz		\color, material_emm, global_amb
	.endm

	.macro		get_half_angle	output, vec1, vec2
	add.xyz		\output, \vec1, \vec2
	esadd		p, \output
	mfp.w		\output, p
	ersqrt		p, \output[w]
	mfp.w		\output, p
	mulw.xyz		\output, \output, \output
	.endm

	.macro		get_diff_term	output, light_dir, normal, light_diff, mat_diffuse,
+							dot_op, dot, ones
	; dot normal with light direction
	.aif "\ones" EQ ""
	\dot_op		intensity\@, \light_dir, \normal
	.aelse
	\dot_op		intensity\@, \light_dir, \normal, \ones
	.aendi
	; clamp intens >= 0.0 (don't let light be sucked away...)
	maxx.\dot		intensity\@, intensity\@, vf00
	; modulate the light diffuse color by the intensity
	mul\dot.xyz	local_diff\@, \light_diff, intensity\@
	; modulate local diffuse light by material diffuse
	mula.xyz		acc, local_diff\@, \mat_diffuse
	.endm

	.macro		add_spec_term	output, half_angle, normal, local_spec, dot_op, dot, ones
	.aif "\ones" EQ ""
	\dot_op		intensity\@, \half_angle, \normal
	.aelse
	\dot_op		intensity\@, \half_angle, \normal, ones
	.aendi
	maxx.\dot		intensity\@, intensity\@, vf00
	mul.\dot		intensity\@, intensity\@, intensity\@
	mul.\dot		intensity\@, intensity\@, intensity\@
	mul.\dot		intensity\@, intensity\@, intensity\@
	mul.\dot		intensity\@, intensity\@, intensity\@
	mul.\dot		intensity\@, intensity\@, intensity\@
	madda\dot.xyz	\output, \local_spec, intensity\@
	.endm

	.macro		get_v2l_atten	vert_to_light, light_pos, vert, atten, atten_coeff, z_one
	; get unit vector from vert to light
	sub.xyz		\vert_to_light, \light_pos, \vert

	; normalize and cache distances for attenuation factor
	dot3_to_z		\atten, \vert_to_light, \vert_to_light, \z_one
	sqrt			q, \atten[z]
	addw.x		\atten, vf00, vf00
	addq.y		\atten, vf00, q
	div			q, vf00w, \atten[y]
	mulq.xyz		\vert_to_light, \vert_to_light, q
	dot3_to_w		\atten, \atten, \atten_coeff
	.endm

	.macro		atten_color	output, vert_color, atten
	div			q, vf00w, \atten[w]
	mulq.xyz		\output, \vert_color, q
	.endm

	.macro		add_amb_term	output, light_amb, mat_ambient
	madd.xyz		\output, \light_amb, \mat_ambient
	.endm

	.macro		accum_rgb		output, rgb
	lq.xyz		total_rgb\@, 1(next_output)
	add.xyz		\output, total_rgb\@, \rgb
	.endm

	.macro		loop_dir_lts	loop_target
	isubiu		num_dir_lights, num_dir_lights, 1
	ibne			num_dir_lights, vi00, \loop_target
	.endm

	.macro		loop_pt_lts	loop_target
	isubiu		num_pt_lights, num_pt_lights, 1
	ibne			num_pt_lights, vi00, \loop_target
	.endm

	.macro		finish_colors
	; clamp and convert to fixed-point colors that have been accumulated to mem
	init_o_loop

	; the alpha value of a vert is the mat diffuse alpha
	loi			128.0
	load_mat_diff	vert_color\@, w
	muli.w		vert_color\@, vert_color\@, i
	loi			255.0
	minii.w		vert_color\@, vert_color\@, i
	ftoi0.w		vert_color\@, vert_color\@

	final_loop_lid:	
	--LoopCS	1,3
	--LoopExtra 1

	load_rgb		vert_color\@
	minii.xyz		vert_color\@, vert_color\@, i
	ftoi0.xyz		vert_color\@, vert_color\@
	store_rgba	vert_color\@

	next_o
	loop_o		final_loop_lid
	.endm
