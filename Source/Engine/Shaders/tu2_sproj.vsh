vs.1.1

dcl_position v0;
dcl_normal v3;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		oFog, r0.z, c[9].y, c[9].x

dp4 oT0.x, v0, c[24]
dp4 oT0.y, v0, c[25]

dp4 oT1.x, v0, c[14]
mov oT1.y, c[8].w

dp3 r0, v3, c[14]
sge oD0.xyz, c[12], r0

