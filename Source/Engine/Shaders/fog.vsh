vs.1.1

dcl_position v0;

m4x4 r0, v0, c[0]
mov oPos, r0
mad oT0.xy, r0.z, c[9].w, -c[9].z
