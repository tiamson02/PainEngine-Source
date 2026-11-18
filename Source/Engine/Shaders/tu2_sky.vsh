vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

; transform vertex to clip space and emit
m4x4 r0, v0, c[0]
;mov r0.z, r0.w
mov oPos, r0
mad	oFog, r0.w, c[9].y, c[9].x

; output texcoord        

dp4 oT0.x, v1, c[24]	; color map
dp4 oT0.y, v1, c[25]

dp4 oT1.x, v1, c[27]	; blend map
dp4 oT1.y, v1, c[28]

//mov oT0.xy, v1.xy
//mov oT1.xy, v1.xy

mov oT2.xy, v2.xy
mov oT3.xy, v2.xy
