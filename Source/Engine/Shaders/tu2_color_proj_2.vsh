vs.1.1

m4x4	oPos, v0, c[0]
mov		oT0, v1

// projector 1
dp4 oT2.x, v0, c[15]
dp4 oT2.y, v0, c[16]
dp4 oT2.w, v0, c[17]

// projector 2
dp4 oT3.x, v0, c[18]
dp4 oT3.y, v0, c[19]
dp4 oT3.w, v0, c[20]
