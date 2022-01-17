//c0 - c3 : composed world-view-projection matrix
//c4: direct light dir
//c5: direct light color
//c6: ambient light color
//c7: fog constant
//c8-11: world transform matrix
//c12-13: texture transform matrix
//c14: x(1.0f),y(0.0f)

vs_1_1

dcl_position0 v0
dcl_normal0 v1
dcl_color0	v2
dcl_texcoord0	v3
dcl_texcoord1	v4
dcl_texcoord2	v5
dcl_texcoord3	v6

def		c14,1.0,0.0,1.0,0.0

m4x4	r0,v0,c0
mov		oPos,r0

;fog out
dp4		r0.x,v0,c15
mad		r0.z,-r0.x,c7.y,c7.x

dp3		r1.x,-c4,v1
sge		r1.x,r1.x,c14.y
mul		r7.w,r0.z,r1.x
mov		oFog,c14.x

m4x4	r0,v0,c8

;texture coordinate calc
dp4		r1.x,r0,c12
dp4		r1.y,r0,c13

mov		oT0.xy,r1.xy

mov		oD0.rgb,r7.w
mov		oD0.a,c14.x 












//c8: x(fog end),y(1/fog end - fog start),z(1.0f),w(0.0f)
//c9: world-view transposed matrix 3 row(for camera space z calculate)

;m4x4	oPos,v0,c0

//linear fog calculate
;dp4		r0.x,v0,c9
;sub		r0.y,c8.x,r0.x			// (fog end - camera space Z)
;mul		r0.z,r0.y,c8.y			// (fog end - camera space Z) / (fog end - fog start)

;max		r0.w,c8.w,r0.z			// 0 이상으로 제한
;min		r0.w,c8.z,r0.w			// 1 이하로 제한
;mov		oFog,r0.w				// fog 출력
