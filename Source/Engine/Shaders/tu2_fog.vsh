vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		r0.w, r0.w, c[9].y, c[9].x
sub		oD0.w, c[8].z, r0.w


