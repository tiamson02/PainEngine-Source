vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

m4x4 r0, v0, c[0]
mov oPos, r0
mad	oFog, r0.w, c[9].y, c[9].x

mov		oT0, v2