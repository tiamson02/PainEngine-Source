vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		oFog, r0.z, c[9].y, c[9].x

mov oT0, v2
mov oT1, v1
mov oT3, v2

sub oT4.x, c[4].x, v0.w
sub oT4.y, c[5].y, v0.w
sub oT4.z, c[6].z, v0.w

sub r0.xyz, c[10], v0
mul oT2.xyz, r0, c[10].w
;dp3 r8.w, r0, r0
;rsq	r8.w, r8.w
;mul	oT2.xyz, r0, r8.w;  
