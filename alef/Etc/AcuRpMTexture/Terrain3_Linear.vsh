//c0 - c3 : composed world-view-projection matrix
//c4: direct light dir
//c5: direct light color
//c6: ambient light color
//c7: fog constant : x(fog end),y(1/fog end - fog start)
//c8-11: world transform matrix
//c12-13: texture transform matrix
//c14: x(1.0f),y(0.0f)
//c15: view matrix 3 row(for transform z to camera z)

vs_1_1

dcl_position0 v0
dcl_normal0 v1
dcl_color0	v2
dcl_texcoord0	v3
dcl_texcoord1	v4
dcl_texcoord2	v5
dcl_texcoord3	v6
dcl_texcoord4	v7
dcl_texcoord5	v8

def		c14,1.0,0.0,0.0,0.0

m4x4	oPos,v0,c0

dp4		r0.x,v0,c15
mad		oFog,-r0.x,c7.y,c7.x

mov		oT0.xy,v6
mov		oT1.xy,v7
mov		oT2.xy,v3
mov		oT3.xy,v3
mov		oT4.xy,v3
mov		oT5.xy,v3

dp3		r1.x,c4,v1
max		r1.x,c14.y,-r1.x
mul		r3,c5,r1.x
mad		oD0,r3,v2,c6
