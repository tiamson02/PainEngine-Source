
; OUTPUT ambient color
mov r4, c[11].xyz

mov r0.w, c[10].w			; specular pow

; compute light 0 contribution
dp3 r0.x, r3, c[13]			; NdotL
dp3 r0.y, r3, c[14]			; NdotH
lit r1, r0
mad oD0, r1.y, c[12], r4	; NdotL * color
mul r5,  r1.z, c[12]		; NdotH * color

mul oD1, r5, c[10]			; specularColor