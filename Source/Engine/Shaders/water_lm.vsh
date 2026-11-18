vs.1.1

dcl_position v0;
dcl_texcoord0 v1;
dcl_texcoord1 v2;

mov	oT0, v2

;============= WAVES =============

dp3 r4.x, v0, c[13]
dp3 r4.y, v0, c[14]

mul r4.xy, r4, c[16] 
add r4.xy, r4, -c[15]

; Clamp theta to -pi..pi
mad  r4.xy, r4, c[18].z, c[18].w            ; add PI and divide by 2*PI

expp r6.y, r4.x		                        ; get fractional part only 
mad  r4.x, r6.y, c[18].y, -c[18].x	        ; multiply with 2*PI and subtract PI

expp r6.y, r4.y		                        ; get fractional part only 
mad  r4.y, r6.y, c[18].y, -c[18].x	        ; multiply with 2*PI and subtract PI

; Compute first 4 values in sin and cos series
                  ; theta^1 is contained in r4
mul r5.xy,  r4, r4   ; theta^2
mul r6.xy,  r4, r5   ; theta^3
mul r7.xy,  r5, r5   ; theta^4
mul r8.xy,  r5, r6   ; theta^5
mul r9.xy,  r6, r6   ; theta^6
mul r10.xy, r6, r7   ; theta^7

mad r11.xy, r5, c[20].y, c[20].x
mad r11.xy, r7, c[20].z, r11
mad r11.xy, r9, c[20].w, r11

mad r11.xy, r4,  c[19].x, r11
mad r11.xy, r6,  c[19].y, r11
mad r11.xy, r8,  c[19].z, r11
mad r11.xy, r10, c[19].w, r11

mul r11.xy, r11, c[17]
dp4 r7.x, r11.xy, c[8].z

mov r1, v0
add r1.y, r1.y, r7.x

;============= POSITION =============

m4x4 r0, r1, c[0]
mov oPos, r0
mad oFog, r0.w, c[9].y, c[9].x


