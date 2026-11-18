vs.1.1
; INPUTS:
; v0 = vertex position stream
; v1 = vertex normal stream
; v2 = vertex texcoords stream
; v3 = vertex weights
; v4 = vertex bones
; c0-c3	: ObjectToWorld matrix
; c4-c7	: WorldToCamera matrix
; c8	: constants (24.5, 255*3, 1, 0.5)
; c9	: constant fog params (start, end, volstart, volend)
; c13	: light0 dir	/ light0 dir
; c14	: light0 half	/ half0 dir
; c13	: light0 dir	/ light1 dir
; c14	: light0 half	/ half1 dir 
; ** skinned mesh **
; c24-c95 : bones

; c4-c7 = model to clip space transform
; c[8] = light direction
; c[9] = halfvector direction
; c[23] = {24,255*3,1,0}
; c24-c26 = bone 0
; c27-c29 = bone 1
; .. next bones
; c93-c95 = bone 24 
;
; OUTPUTS:
; oT0 = base texture UVs
; oD0 = light color precomputed by per vertex attenuation

; compute the last weight 
dp3 r0.w, v3, c[8].z
sub r0.w, c[8].z, r0.w

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

mov r8, c[13]
mov r9, c[14]
mov r10, c[15]
mov r11, c[16]

; skin transform by matrix 0
mov a0.x, r1.x
m4x3 r2, v0, c[a0.x]
mul r3, r2, v3.x

mul r2.xyz, r8.x, c[a0.x]
mad r2.xyz, r8.y, c[a0.x+1], r2
mad r2.xyz, r8.z, c[a0.x+2], r2
mul r4.xyz, r2, v3.x

mul r2.xyz, r9.x, c[a0.x]
mad r2.xyz, r9.y, c[a0.x+1], r2
mad r2.xyz, r9.z, c[a0.x+2], r2
mul r5.xyz, r2, v3.x

mul r2.xyz, r10.x, c[a0.x]
mad r2.xyz, r10.y, c[a0.x+1], r2
mad r2.xyz, r10.z, c[a0.x+2], r2
mul r6.xyz, r2, v3.x

mul r2.xyz, r11.x, c[a0.x]
mad r2.xyz, r11.y, c[a0.x+1], r2
mad r2.xyz, r11.z, c[a0.x+2], r2
mul r7.xyz, r2, v3.x

; skin transform by matrix 1
mov a0.x, r1.y
m4x3 r2, v0, c[a0.x]
mad r3, r2, v3.y, r3

mul r2.xyz, r8.x, c[a0.x]
mad r2.xyz, r8.y, c[a0.x+1], r2
mad r2.xyz, r8.z, c[a0.x+2], r2
mad r4.xyz, r4, v3.y, r4

mul r2.xyz, r9.x, c[a0.x]
mad r2.xyz, r9.y, c[a0.x+1], r2
mad r2.xyz, r9.z, c[a0.x+2], r2
mad r5.xyz, r2, v3.y, r5

mul r2.xyz, r10.x, c[a0.x]
mad r2.xyz, r10.y, c[a0.x+1], r2
mad r2.xyz, r10.z, c[a0.x+2], r2
mad r6.xyz, r2, v3.y, r6

mul r2.xyz, r11.x, c[a0.x]
mad r2.xyz, r11.y, c[a0.x+1], r2
mad r2.xyz, r11.z, c[a0.x+2], r2
mad r7.xyz, r2, v3.y, r7

; skin transform by matrix 2
mov a0.x, r1.z
m4x3 r2, v0, c[a0.x]
mad r3, r2, v3.z, r3

; skin transform by matrix 3
mov a0.x, r1.w
m4x3 r2, v0, c[a0.x]
mad r3, r2, r0.w, r3

mov r3.w, c[8].z

; OUTPUT vertex transformed to clip space 
m4x4 oPos, r3, c[0]

; OUTPUT fog
mad		oFog.x, r3.w, c[9].y, c[9].x

; decal texture coords
mov oT0, v2
; normalmap texture coords
mov oT1, v2
; normalized L
mov oT2, r4.xyz
; normalized H
mov oT3, r5.xyz
; normalized L
mov oT4, r6.xyz
; normalized H
mov oT5, r7.xyz

