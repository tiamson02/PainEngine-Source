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
;
; OUTPUTS:
; oD0 = light color precomputed by per vertex attenuation

; OUTPUT texcoords (needed for alpha testing)
mov oT0, v2

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r1.x
m4x3 r4, v0, c[a0.x]

mov r4.w, c[8].z

m4x4 r0, r4, c[0]

; OUTPUT vertex transformed to clip space 
mov		oPos, r0

; OUTPUT fog
mad		oFog, r0.z, c[9].y, c[9].x

; OUTPUT ambient color
mov oD0, c[11].xyz
