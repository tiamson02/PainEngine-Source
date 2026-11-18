/*	  Copyright (C) 2000,2001,2002  Sony Computer Entertainment America
       	  
       	  This file is subject to the terms and conditions of the GNU Lesser
	  General Public License Version 2.1. See the file "COPYING" in the
	  main directory of this archive for more details.                             */


	; these are for declaring arrays in the --enter and --exit sections for vcl

	.macro	mExpArrayDecl directive, name, length, regBase, startReg
count	.assigna	0
curReg	.assigna	\startReg
	.awhile	\&count lt \length
	\directive	\name[\&count](\regBase\&curReg)
count	.assigna	\&count + 1
curReg	.assigna	\&curReg + 1
	.aendw
	.endm

in_vfa	.macro	name, length, startReg
	mExpArrayDecl in_vf, \name, \length, vf, \startReg
	.endm

out_vfa	.macro	name, length, startReg
	mExpArrayDecl out_vf, \name, \length, vf, \startReg
	.endm

