// Exp¸ðµå backup ^^

// constants
//c0 - c3 : composed world-view-projection matrix
//c4: direct light dir
//c5: direct light color
//c6: ambient light color
//c7: fog constant
//c14: x(1.0f),y(0.0f)
//c15: light adjustment
//c16: Ambient light adjustment

// ========================================= Terrain.Vsh ============================================
vs_1_1

dcl_position0 	v0
dcl_normal0	v1
dcl_color0	v2
dcl_texcoord0	v3
dcl_texcoord1	v4
dcl_texcoord2	v5
dcl_texcoord3	v6
dcl_texcoord4	v7
dcl_texcoord5	v8

def	c14,1.0,0.0,0.0,0.0

m4x4	r0,v0,c0
mov	oPos,r0

mul r0.w,r0.w,c7.x
expp r0.w,r0.w
rcp oFog,r0.w

mov oT0.xy,v3
mov oT1.xy,v4
mov oT2.xy,v5
mov oT3.xy,v6
mov oT4.xy,v7
mov oT5.xy,v8

dp3 r1.x,c4,v1
max r1.x,c14.y,-r1.x
mul r3,c5,r1.x
mad r2,r3,v2,c6
min oD0,r2,c14.x

// ========================================= Terrain2.Vsh ============================================
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

def	c14,1.0,0.0,0.0,0.0

m4x4	r0,v0,c0
mov	oPos,r0

mul r0.w,r0.w,c7.x
expp r0.w,r0.w
rcp oFog,r0.w

mov oT0.xy,v4
mov oT1.xy,v5
mov oT2.xy,v3
mov oT3.xy,v3
mov oT4.xy,v3
mov oT5.xy,v3

dp3 r1.x,c4,v1
max r1.x,c14.y,-r1.x
mul r3,c5,r1.x
mad r2,r3,v2,c6
min oD0,r2,c14.x

// ========================================= Terrain3.Vsh ============================================
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

def	c14,1.0,0.0,0.0,0.0

m4x4	r0,v0,c0
mov	oPos,r0

mul r0.w,r0.w,c7.x
expp r0.w,r0.w
rcp oFog,r0.w

mov oT0.xy,v6
mov oT1.xy,v7
mov oT2.xy,v3
mov oT3.xy,v3
mov oT4.xy,v3
mov oT5.xy,v3

dp3 r1.x,c4,v1
max r1.x,c14.y,-r1.x
mul r3,c5,r1.x
mad r2,r3,v2,c6
min oD0,r2,c14.x

