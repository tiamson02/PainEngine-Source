vs.1.1

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;

m4x4	r0, v0, c[0]
mov		oPos, r0
mad		oFog, r0.z, c[9].y, c[9].x

dp4 oT0.x, v2, c[24]	; color map
dp4 oT0.y, v2, c[25]

dp4 oT1.x, v2, c[27]	; detail map (same uvset, different tiling)
dp4 oT1.y, v2, c[28]

; OUTPUT ambient color
mov r4, c[11].xyz

mov r0.w, c[10].w			; specular pow

m3x3 r3, v1, c[4]			; transform normal by inverse transpose

; compute light 0 contribution
dp3 r0.x, r3, c[13]			; NdotL
dp3 r0.y, r3, c[14]			; NdotH
lit r1, r0
mad r4, r1.y, c[12], r4		; NdotL * color
mul r5, r1.z, c[12]			; NdotH * color

; compute light 1 contribution
dp3 r0.x, r3, c[16]			; NdotL
dp3 r0.y, r3, c[17]			; NdotH
lit r1, r0
mad r4, r1.y, c[15], r4		; NdotL * color
mad r5, r1.z, c[15], r5		; NdotH * color

; compute light 2 contribution
dp3 r0.x, r3, c[19]			; NdotL
dp3 r0.y, r3, c[20]			; NdotH
lit r1, r0
mad r4, r1.y, c[18], r4		; NdotL * color
mad r5, r1.z, c[18], r5		; NdotH * color

; compute light 3 contribution
dp3 r0.x, r3, c[22]			; NdotL
dp3 r0.y, r3, c[23]			; NdotH
lit r1, r0
mad oD0, r1.y, c[21], r4	; NdotL * color
mad r5,  r1.z, c[21], r5	; NdotH * color

mul oD1, r5, c[10]			; specularColor
