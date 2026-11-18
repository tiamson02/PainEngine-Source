vs.1.1

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;
dcl_blendweight v3;
dcl_blendindices v4;

; INPUTS:
; v0 = vertex position stream
; v1 = vertex normal stream
; v2 = vertex texcoords stream
; v3 = vertex weights
; v4 = vertex bones
; OUTPUTS:
; oT0 = dirt map
; oT1 = refract cube texture UVs
; oT2 = reflect cube texture UVs
; oD0 = fresnel

mov oT0, v2

;============= COMPUTE POSITION & NORMAL =============
; use: r0,r1,r5,r6, c8,c[a0.x]
; out: r4,r3(POS,NOR)


; compute the last weight 
dp3 r2.w, v3, c[8].z
sub r2.w, c[8].z, r2.w

; unpack world matrix offset constants
mad r3, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r3.x
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mul r1, r5.xyz, v3.x
mul r0, r6.xyz, v3.x

; skin transform by matrix 1
mov a0.x, r3.y
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r1, r5.xyz, v3.y, r1
mad r0, r6.xyz, v3.y, r0

; skin transform by matrix 2
mov a0.x, r3.z
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r1, r5.xyz, v3.z, r1
mad r0, r6.xyz, v3.z, r0

; skin transform by matrix 3
mov a0.x, r3.w
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r1, r5.xyz, r2.w, r1
mad r0, r6.xyz, r2.w, r0

mov r0.w, c[8].z

;============= OUTPUT POSITION =============
;use: r4, c[0-3],c[9]
;in:  r4
;out: r0,oPos,oFog
dp4 oT0.x, v2, c[24]	; color map
dp4 oT0.y, v2, c[25]

m4x4 r2, r0, c[0]

; OUTPUT vertex transformed to clip space 
mov		oPos, r2

; OUTPUT fog
mad		oFog, r2.w, c[9].y, c[9].x

;============= COMPUTE REFRACTION, REFLECTION AND FRESNEL =============
;use: r0,r3,r4,r8, c[8],c[6],c[10],c[11]
;in:  r0,r1
;out: oT0,oT1,oD0

add r0, -r0, c[10] ; c[10] = eye position in world space

;normalize

dp3 r8.w, r0, r0
rsq	r8.w, r8.w
mul	r8, r0, r8.w;       ; r8 = eye/incident vector

; c[11] = {eta, eta*eta, fresnel, 2}

mul    r0, r1, c[11].w
dp3    r1.w, r1, r8
mad    r1, r1.w, r0, -r8

; cheesy Fresnel approximation = (1-(I.N))^p 

dp3    r0.x, r8, r1
add    r0.x, c[8].z, -r0.x
mul    r0.x, r0.x, r0.x
mul    oT1.xy, r0.x, c[11].z
