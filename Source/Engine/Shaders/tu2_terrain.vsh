vs.1.1

; transform vertex to clip space and emit
m4x4 oPos, v0, c4

; output texcoord        
dp4 oT0.x, v1, c20
dp4 oT0.y, v1, c21

dp4 oT1.x, v1, c22
dp4 oT1.y, v1, c23

mov oT2.xy, v1.xy
mov oT3.xy, v2.xy
