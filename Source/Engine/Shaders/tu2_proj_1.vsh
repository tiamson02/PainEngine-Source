vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_normal v3;

m4x4 r0, v0, c[0]
mov oPos, r0
mad	oFog, r0.w, c[9].y, c[9].x

dp4 oT0.x, v0, c[24]
dp4 oT0.y, v0, c[25]
dp4 oT0.zw, v0, c[26]	//The "combined" write mask must write .x, .xy, .xyz, or .xyzw

dp4 oT1.x, v0, c[14]
mov oT1.y, c[8].w

sub oT2, c[13], v0

mov oT3, v1

mov oD0, c[12]
mad oD1, v3, c[8].w, c[8].w

