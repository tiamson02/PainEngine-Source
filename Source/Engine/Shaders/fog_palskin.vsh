vs.1.1

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;
dcl_blendweight v3;
dcl_blendindices v4;

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
; ** skinned mesh **
; c24-c95 : bones


; c4-c7 = model to clip space transform
; c[23] = {24,255*3,1,0}
; c24-c26 = bone 0
; c27-c29 = bone 1
; .. next bones
; c93-c95 = bone 24 
;
; OUTPUTS:
; oT1 = depth

; compute the last weight 
dp3 r0.w, v3, c[8].z
sub r0.w, c[8].z, r0.w

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r1.x
m4x3 r6, v0, c[a0.x]
mul r4, r6.xyz, v3.x

; skin transform by matrix 1
mov a0.x, r1.y
m4x3 r6, v0, c[a0.x]
mad r4, r6.xyz, v3.y, r4

; skin transform by matrix 2
mov a0.x, r1.z
m4x3 r6, v0, c[a0.x]
mad r4, r6.xyz, v3.z, r4

; skin transform by matrix 3
mov a0.x, r1.w
m4x3 r6, v0, c[a0.x]
mad r4, r6.xyz, r0.w, r4

mov r4.w, c[8].z

; OUTPUT vertex transformed to clip space 
m4x4 r0, r4, c[0]

mov oPos, r0

mad oT0.xy, r0.w, c[9].w, -c[9].z
