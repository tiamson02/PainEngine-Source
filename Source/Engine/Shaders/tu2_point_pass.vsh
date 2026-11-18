vs.1.1
; INPUTS:
; v0 = vertex position stream
; v1 = vertex texcoords stream 0 (N/A)
; v2 = vertex texcoords stream 1 (N/A)
; v3 = vertex normal stream
; c0-c3	: ObjectToWorld matrix
; c4-c7	: WorldToCamera matrix
; c8	: constants (24.5, 255*3, 1, 0.5)
; c9	: constant fog params (start, end, volstart, volend)
; ** general for mesh  **
; c12	: light0 color	/ color|radius
; c13	: light0 dir	/ light0 pos
; c14	: light0 half	/ eye pos 

dcl_position v0;
dcl_texcoord0 v1;
dcl_normal v3;

m4x4 r0, v0, c[0]
mov oPos, r0
mad	oFog, r0.w, c[9].y, c[9].x

add r0, v0, -c[13]
mul r1, r0, c[12].w
mad r1, r1, c[8].w, c[8].w

mov oT0.xy, r1.xy
mov oT1.x, r1.z
mov oT1.y, c[8].w
mov oT2, -r0
//mov oT3, v1

dp4 oT3.x, v1, c[33]	; blend xform
dp4 oT3.y, v1, c[34]


mov oD0, c[12]

mad oD1, v3,c[8].w,c[8].w

