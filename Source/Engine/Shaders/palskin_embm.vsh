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
; c4-c7 = model to clip space transform
; c[8] = {27.5,255*3,1,0}
; c[10] = material color
; c[11] = ambient
; c[12] = attenuated light0 color
; c[13] = light1 direction
; c[14] = light1 halfvector
; c[15] = attenuated light1 color
; c[16] = light2 direction
; c[17] = light2 halfvector
; c[18] = attenuated light2 color
; c[19] = light3 direction
; c[20] = light3 halfvector
; c[21] = attenuated light2 color
; c[22] = light3 direction
; c[23] = light3 halfvector
; c[24-26] = TU transform
; c27-c29 = bone 0
; .. next bones
; c93-c95 = bone 23
;
; OUTPUTS:
; oT0 = base texture UVs
; oD0 = light color precomputed by per vertex attenuation
; 
; OUTPUT texcoords
;dp4 oT0.x, v2, c[24]	; emmisive map
;dp4 oT0.y, v2, c[25]

mov oT0, v2

; compute the last weight 
dp3 r0.w, v3, c[8].z
sub r0.w, c[8].z, r0.w

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r1.x
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mul r3, r5.xyz, v3.x
mul r4, r6.xyz, v3.x

; skin transform by matrix 1
mov a0.x, r1.y
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5.xyz, v3.y, r3
mad r4, r6.xyz, v3.y, r4

; skin transform by matrix 2
mov a0.x, r1.z
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5.xyz, v3.z, r3
mad r4, r6.xyz, v3.z, r4

; skin transform by matrix 3
mov a0.x, r1.w
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5.xyz, r0.w, r3
mad r4, r6.xyz, r0.w, r4

mov r4.w, c[8].z

m4x4 r0, r4, c[0]

dp4 oT1.x, r4, c[24]
dp4 oT1.y, r4, c[25]
dp4 oT1.zw, r4, c[26]	//The "combined" write mask must write .x, .xy, .xyz, or .xyzw

; OUTPUT vertex transformed to clip space 
mov		oPos, r0


; OUTPUT fog
mad		oFog, r0.z, c[9].y, c[9].x

;============= COMPUTE REFRACTION, REFLECTION AND FRESNEL =============
;use: r0,r3,r4,r8, c[8],c[6],c[10],c[11]
;in:  r0,r1

add r0, -r4, c[10] ; c[10] = eye position in world space

;normalize

dp3 r8.w, r0, r0
rsq	r8.w, r8.w
mul	r8, r0, r8.w;       ; r8 = eye/incident vector

; c[11] = {eta, eta*eta, fresnel, 2}

mul    r0, r3, c[11].w
dp3    r1.w, r3, r8
mad    r1, r1.w, r0, -r8

; cheesy Fresnel approximation = (1-(I.N))^p 

dp3    r0.x, r8, r1
add    r0.x, c[8].z, -r0.x
mul    r0.x, r0.x, r0.x
mul    oT2.xy, r0.x, c[11].z