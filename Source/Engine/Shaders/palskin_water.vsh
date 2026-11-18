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

dp4 oT0.x, v2, c[24]	; color map
dp4 oT0.y, v2, c[25]

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

;============= PERTRUB =============
; use: r4,r5,r6,r7,r8,r9, c[8],c[4],c[5],c[6],c[12],c[13],c[14]
; in: r0,r1
; out: r0(POS)

; Compute theta from distance and time
sub r3.xyz, r0, c[17]			; compute epicenter position
dp3 r4.x, r3, r3                ; compute distance^2
rsq r5.x, r4.x                  ; compute the square root

sub r3.xyz, r0, c[18]			; compute epicenter position
dp3 r4.y, r3, r3                ; compute distance^2
rsq r5.y, r4.y                  ; compute the square root

sub r3.xyz, r0, c[19]			; compute epicenter position
dp3 r4.z, r3, r3                ; compute distance^2
rsq r5.z, r4.z                  ; compute the square root

sub r3.xyz, r0, c[20]			; compute epicenter position
dp3 r4.w, r3, r3                ; compute distance^2
rsq r5.w, r4.w                  ; compute the square root

mul r2, r4, r5

mul r4, r2, c[13] 
add r4, r4, -c[12]

; Clamp theta to -pi..pi
mad  r4, r4, c[4].z, c[4].w            ; add PI and divide by 2*PI

expp r6.y, r4.x		                        ; get fractional part only 
mad  r4.x, r6.y, c[4].y, -c[4].x	        ; multiply with 2*PI and subtract PI

expp r6.y, r4.y		                        ; get fractional part only 
mad  r4.y, r6.y, c[4].y, -c[4].x	        ; multiply with 2*PI and subtract PI

expp r6.y, r4.z		                        ; get fractional part only 
mad  r4.z, r6.y, c[4].y, -c[4].x	        ; multiply with 2*PI and subtract PI

expp r6.y, r4.w		                        ; get fractional part only 
mad  r4.w, r6.y, c[4].y, -c[4].x	        ; multiply with 2*PI and subtract PI

; Compute first 4 values in sin and cos series
                  ; theta^1 is contained in r4
mul r5,  r4, r4   ; theta^2
mul r6,  r4, r5   ; theta^3
mul r7,  r5, r5   ; theta^4
mul r8,  r5, r6   ; theta^5
mul r9,  r6, r6   ; theta^6
mul r10, r6, r7   ; theta^7

mad r11, r5, c[6].y, c[6].x
mad r11, r7, c[6].z, r11
mad r11, r9, c[6].w, r11

mad r11, r4,  c[5].x, r11
mad r11, r6,  c[5].y, r11
mad r11, r8,  c[5].z, r11
mad r11, r10, c[5].w, r11

mul r2, r2, -c[14]
add r2, r2,  c[8].z
max r2, r2,  c[15]
mul r11, r11, r2
mul r11, r11, c[16]
dp4 r7.x, r11, c[8].z

mad r0.xyz, r7.x, r1, r0

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

;calculate normalized eye direction in world space
add r0, -r0, c[10] ; c[10] = eye position in world space
dp3 r8.w, r0, r0
rsq	r8.w, r8.w
mul	r8, r0, r8.w;       ; r8 = eye/incident vector

; refraction, renderman style
; float IdotN = I.N
; float k = 1 - eta*eta*(1 - IdotN*IdotN)
; return k < 0 ? (0,0,0) : eta*I - (eta*IdotN + sqrt(k))*N
dp3    r0.x, r1, -r8              ; r0 = N.I

; c[11] = {eta, eta*eta, fresnel, 2}
mad	   r3.x, -r0.x, r0.x, c[8].z   ; r3 = -IdotN*IdotN + 1
;mul	   r3.x, r0.x, r0.x
mul	   r3.x, r3.x, c[11].y         ; r3 = -(r3*eta*eta)+1
add    r3.x, c[8].z, -r3.x

rsq    r2.x, r3.x
rcp    r2.x, r2.x
mad    r2.x, c[11].x, r0.x, r2.x
mul    r2, r1, r2.x
mad    r2, c[11].x, -r8, r2

; transform refracted ray by cubemap transform
mov	   oT1.x, r2.x
mov	   oT1.y, r2.y
mov	   oT1.z, -r2.z

; calculate reflection
mul    r0, r1, c[11].w
dp3    r1.w, r1, r8
mad    r1, r1.w, r0, -r8

; transform reflected ray by cubemap transform
mov	   oT2.x, r1.x
mov	   oT2.y, r1.y
mov	   oT2.z, -r1.z

; cheesy Fresnel approximation = (1-(I.N))^p

dp3    r0.w, r8, r1
add    r0.w, c[8].z, -r0.w
mul    r0.w, r0.w, r0.w
mul    r0.w, r0.w, c[11].z
mov    r1, c[21]
mad    oD0, r1, r0.w, c[22]
