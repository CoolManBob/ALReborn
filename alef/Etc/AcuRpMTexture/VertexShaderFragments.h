// c0 - c3 : world-view-projection matrix
// c4 : main light direction
// c5 : main light color
// c6 : ambient light color
// c7 : fog constants(x = fog end/(fog end - fog start), y = 1/(fog end - fog start))
// c15 : 3rd row of the view matrix(for transforming z to the camera coordinates)
// c21 : view position

const char _VSHeaderFrag[] = 
{
	"vs_1_1\n"
	"dcl_position v0\n"
	"dcl_normal v1\n"
	"dcl_color0 v2\n"
	"dcl_texcoord0 v3\n"
	"dcl_texcoord1 v4\n"
	"dcl_texcoord2 v5\n"
	"dcl_texcoord3 v6\n"
	"dcl_texcoord4 v7\n"
	"dcl_texcoord5 v8\n"

	"def c14, 1.0, 0.0, 0.5, 0.0\n"
};

const char _VSHeaderFrag_For_Rough[] = 
{
	"vs_1_1\n"
	"dcl_position v0\n"
	"dcl_normal v1\n"
	"dcl_color0 v2\n"
	"dcl_texcoord0 v3\n"

	"def c14, 1.0, 0.0, 0.5, 0.0\n"
};

const char _VSPosTransformFrag[] = 
{
	"m4x4 oPos, v0, c0\n"
};

const char _VSFogFrag[] = 
{
	"dp4 r0.x, v0, c15\n"
	"mad oFog, -r0.x, c7.y, c7.x\n"
};

const char _VSDiffuseFrag[] = 
{
	"dp3 r1.x, c4, v1\n"
	"max r1.x, c14.y, -r1.x\n"
	"mul r3, c5, r1.x\n"
	"mad oD0, r3, v2, c6\n"
	"mov oD0.a, c14.x\n"
};

const char _VSSpecularFrag[] =
{
//	"mad r0, c21.w, -v0, c21\n"
	"sub r0, c21, v0\n"
	
	"dp3 r1.x, r0, r0\n"
	"rsq r1.x, r1.x\n"
	"mul r0, r0, r1.x\n"

	"add r0, r0, -c4\n"

	"dp3 r1.x, r0, r0\n"
	"rsq r1.x, r1.x\n"
	"mul r0, r0, r1.x\n"

	"dp3 r1.x, r0, v1\n"
	"mul r1.x, r1.x, r1.x\n"
	"mul r1.x, r1.x, r1.x\n"
	"mul r1.x, r1.x, r1.x\n"
	"mul r1.x, r1.x, r1.x\n"
	"mul r1.x, r1.x, r1.x\n"
	"max r1.x, c14.y, r1.x\n"
	"mul oD1, c5, r1.x\n"
};

const char _VSTexCoordFrag0[] = 
{
	"mov oT0.xy, v3\n"
	"mov oT1.xy, v4\n"
	"mov oT2.xy, v5\n"
	"mov oT3.xy, v6\n"
	"mov oT4.xy, v7\n"
	"mov oT5.xy, v8\n"
};

const char _VSTexCoordFrag_For_Rough[] = 
{
	"mov oT0.xy, v3\n"
};

const char _VSTexCoordFrag1[] = 
{
	"mov oT0.xy, v4\n"
	"mov oT1.xy, v5\n"
};

const char _VSTexCoordFrag2[] = 
{
	"mov oT0.xy, v6\n"
	"mov oT1.xy, v7\n"
};

const char _VSGlossMapTexCoordFrag0[] = 
{
	"mov oT0.xy, v5\n"
	"mov oT1.xy, v4\n"
};

const char _VSGlossMapTexCoordFrag1[] = 
{
	"mov oT0.xy, v3\n"
	"mov oT2.xy, v5\n"
	"mov oT3.xy, v4\n"
};

const char _VSGlossMapTexCoordFrag2[] = 
{
	"mov oT0.xy, v7\n"
	"mov oT1.xy, v6\n"
};

const char _VSGlossMapTexCoordFrag3[] = 
{
	"mov oT0.xy, v3\n"
	"mov oT2.xy, v5\n"
	"mov oT3.xy, v4\n"
	"mov oT5.xy, v7\n"
	"mov oT6.xy, v6\n"
};

const char _VSShadow2Frag[] = 
{
	"m4x4 r0, v0, c0\n"
	"mov oPos, r0\n"

	// ;fog out
	"dp4 r0.x, v0, c15\n"
	"mad r0.z, -r0.x, c7.y, c7.x\n"

	"dp3 r1.x, -c4, v1\n"
	"sge r1.x, r1.x, c14.y\n"
	"mul r7.w, r0.z, r1.x\n"
	"mov oFog, c14.x\n"

	"m4x4 r0, v0, c8\n"

	// ;texture coordinate calc
	"dp4 r1.x, r0, c12\n"
	"dp4 r1.y, r0, c13\n"

	"mov oT0.xy, r1.xy\n"

	"mov oD0.rgb, r7.w\n"
	"mov oD0.a, c14.x\n"
};