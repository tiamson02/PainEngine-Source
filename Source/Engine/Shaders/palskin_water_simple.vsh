vs.1.1
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

dcl_position v0;
dcl_normal v1;
dcl_texcoord0 v2;
dcl_blendweight v3;
dcl_blendindices v4;

dp4 oT1.x, v2, c[24]	; color map
dp4 oT1.y, v2, c[25]

;============= COMPUTE POSITION & NORMAL =============
; use: r0,r1,r5,r6, c8,c[a0.x]
; out: r4,r3(POS,NOR)

; unpack world matrix offset constants
mad r3, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r3.x
m3x3 r1, v1, c[a0.x]
m4x3 r0, v0, c[a0.x]

mov r0.w, c[8].z

;============= OUTPUT POSITION =============
;use: r4, c[0-3],c[9]
;in:  r4
;out: r0,oPos,oFog

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
mul	r8.xyz, r0, r8.w;       ; r8 = eye/incident vector

; refraction, renderman style
; float IdotN = I.N
; float k = 1 - eta*eta*(1 - IdotN*IdotN)
; return k < 0 ? (0,0,0) : eta*I - (eta*IdotN + sqrt(k))*N

dp3    r0.x, r1, -r8              ; r0 = N.I

; c[11] = {eta, eta*eta, fresnel, 2}

mad	   r3.x, -r0.x, r0.x, c[64].y  ; r3 = -IdotN*IdotN + 1
mul	   r3.x, r3.x, c[11].y         ; r3 = -(r3*eta*eta)+1
add    r3.x, c[8].z, -r3.x

rsq    r2.x, r3.x
rcp    r2.x, r2.x
mad    r2.x, c[11].x, r0.x, r2.x
mul    r2.xyz, r1, r2.x
mad    r2.xyz, c[11].x, -r8, r2

; transform refracted ray by cubemap transform
mov	   oT0.x, r2.x
mov	   oT0.y, r2.y
mov	   oT0.z, -r2.z

