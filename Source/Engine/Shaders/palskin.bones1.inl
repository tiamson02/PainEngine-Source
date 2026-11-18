
dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;
dcl_blendweight v3;
dcl_blendindices v4;

; OUTPUT texcoords
dp4 oT0.x, v2, c[24]	; color map
dp4 oT0.y, v2, c[25]

dp4 oT1.x, v2, c[24]	; emmisive map
dp4 oT1.y, v2, c[25]

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r1.x
m3x3 r3, v1, c[a0.x]
m4x3 r4, v0, c[a0.x]

mov r4.w, c[8].z

m4x4 r0, r4, c[0]

; OUTPUT vertex transformed to clip space 
mov		oPos, r0

; OUTPUT fog
mad		oFog, r0.z, c[9].y, c[9].x

