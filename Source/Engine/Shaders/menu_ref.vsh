vs.1.1

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;

mov oPos, v0

mov oT1, c[24]
mov oT2, c[25]
mov oT3, c[26]
add r0, -v0, c[27]
dp4	oT0.x, v2, c[28]
dp4	oT0.y, v2, c[29]

mov oT1.w, r0.x
mov oT2.w, r0.y
mov oT3.w, r0.z
