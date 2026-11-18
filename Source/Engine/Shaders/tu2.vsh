vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		oFog, r0.z, c[9].y, c[9].x

dp4 oT0.x, v1, c[24]	; color map
dp4 oT0.y, v1, c[25]

mov	oT1.xy, v2

