vs.1.1
; INPUTS:
; v0 = vertex position stream
; v1 = vertex normal stream
; v2 = vertex texcoords stream
; v3 = vertex weights
; v4 = vertex bones
; OUTPUTS:
; oT0 = refract cube texture UVs
; oT1 = reflect cube texture UVs
; oT2 = dirt texture UVs
; oD0 = fresnel

dp4 oT0.x, v2, c[24]	; color map
dp4 oT0.y, v2, c[25]

; compute the last weight 
dp3 r0.w, v3, c[8].z
sub r0.w, c[8].z, r0.w

; unpack world matrix offset constants
mad r1, v4.zyxw, c[8].y, c[8].x

; skin transform by matrix 0
mov a0.x, r1.x
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mul r3, r5, v3.x
mul r4, r6, v3.x

; skin transform by matrix 1
mov a0.x, r1.y
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5, v3.y, r3
mad r4, r6, v3.y, r4

; skin transform by matrix 2
mov a0.x, r1.z
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5, v3.z, r3
mad r4, r6, v3.z, r4

; skin transform by matrix 3
mov a0.x, r1.w
m3x3 r5, v1, c[a0.x]
m4x3 r6, v0, c[a0.x]
mad r3, r5, r0.w, r3
mad r4, r6, r0.w, r4

mov r4.w, c[8].z

;============= PERTRUB =============

; Compute theta from distance and time
sub r7, r4, c[18]				; compute epicenter position
dp3 r7.x, r7, r7                ; compute distance^2
rsq r7.y, r7.x                  ; compute the square root
mul r9,   r7.x, r7.y          

mad r7.x, r9.x, c[16].z, -c[16].x            ; scale by time

; Clamp theta to -pi..pi
mad  r7.x, r7.x, c[11].z, c[11].w            ; add PI and divide by 2*PI
expp r6.y, r7.x		                        ; get fractional part only 
mad  r7.x, r6.y, c[11].y, -c[11].x	        ; multiply with 2*PI and subtract PI

; Compute first 4 values in sin and cos series
sge r8.x, r7.x, r7.x   ; theta^0
                       ; theta^1 is contained in r7.x 
mul r8.y, r7.x, r7.x   ; theta^2
mul r7.y, r7.x, r8.y   ; theta^3
mul r8.z, r8.y, r8.y   ; theta^4
mul r7.z, r7.x, r8.z   ; theta^5
mul r8.w, r8.y, r8.z   ; theta^6
mul r7.w, r7.x, r8.w   ; theta^7

mul r7, r7,   c[12]				; sin
dp4 r7.x, r7, c[8].z       		; store sin(theta) in r7.x 

mul r8, r8,   c[13]				; cos
dp4 r8.x, r8, c[8].z	        ; store cos(theta) in r8.x

; attenuation = max(0, 1-dist/range)
mad r9.x, r9.x, -c[17].x, c[17].y
max r9.x, r9.x, c[17].z
mul r7.x, r9.x, r7.x

mul r7.x, r7.x, c[16].y
mad r4.xyz, r7.x, r5, r4
; normal pertrub
;mad r5.xyz, r7.x, r9, r5

;============= OUTPUT POSITION =============

m4x4 r0, r4, c[0]

; OUTPUT vertex transformed to clip space 
mov		oPos, r0

; OUTPUT fog
mad		oFog.x, r0.w, c[9].y, c[9].x

;============= COMPUTE REFRACTION, REFLECTION AND FRESNEL =============

add r0, -r4, c[14] ; c[14] = eye position in world space

;normalize

dp3 r8.w, r0, r0
rsq	r8.w, r8.w
mul	r8, r0, r8.w;       ; r8 = eye/incident vector

; refraction, renderman style
; float IdotN = I.N
; float k = 1 - eta*eta*(1 - IdotN*IdotN)
; return k < 0 ? (0,0,0) : eta*I - (eta*IdotN + sqrt(k))*N

dp3    r0.x, r3, -r8              ; r0 = N.I

; c[15] = {eta, eta*eta, fresnel, 2}

mad	   r1.x, -r0.x, r0.x, c[64].y  ; r1 = -IdotN*IdotN + 1
mul	   r1.x, r1.x, c[15].y         ; r1 = -(r1*eta*eta)+1
add    r1.x, c[8].z, -r1.x

rsq    r2.x, r1.x
rcp    r2.x, r2.x
mad    r2.x, c[15].x, r0.x, r2.x
mul    r2, r3, r2.x
mad    r2, c[15].x, -r8, r2

; transform refracted ray by cubemap transform
;dp3    oT0.x, c[24], r2
;dp3    oT0.y, c[25], r2
;dp3    oT0.z, c[26], r2
mov	   oT1.x, r2.x
mov	   oT1.y, r2.y
mov	   oT1.z, -r2.z

; calculate reflection

mul    r0, r3, c[15].w
dp3    r3.w, r3, r8
mad    r3, r3.w, r0, -r8

; transform reflected ray by cubemap transform

;dp3    oT1.x, c[24], r3
;dp3    oT1.y, c[25], r3
;dp3    oT1.z, c[26], r3
mov	   oT2.x, r3.x
mov	   oT2.y, r3.y
mov	   oT2.z, -r3.z

; cheesy Fresnel approximation = (1-(I.N))^p

dp3    r0.x, r8, r3
add    r0.x, c[8].z, -r0.x
mul    r0.x, r0.x, r0.x
mul    oD0, r0.x, c[15].z
