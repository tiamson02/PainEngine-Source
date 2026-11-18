vs.1.1

dp4 oPos.x, c[0], v0
dp4 oPos.y, c[1], v0
dp4 oPos.z, c[2], v0
dp4 oPos.w, c[3], v0

// L vector
add oT0, -v0, c[8]

// normal map
dp4 oT1.x, v2, c[10]
dp4 oT2.y, v2, c[11]

// 1D z atten
mov oT2, c[9]
dp4 oT2.x, v0, c[20]

// spot projection
dp4 oT3.x, v0, c[6]
dp4 oT3.y, v0, c[7]
dp4 oT3.w, v0, c[8]
