vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		oFog, r0.z, c[9].y, c[9].x

dp4	oT0.x, v2, c[24]
dp4	oT0.y, v2, c[25]

sub oT1, c[4], v0.wwwx
sub oT2, c[5], v0.wwwy
sub oT3, c[6], v0.wwwz

