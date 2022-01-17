#include "AgcmWater.h"
#include "rpmatfx.h"
#include "AgcEngine.h"
#include "AcuRpMTexture.h"

#include "ApMemoryTracker.h"

#include "d3d9.h"
#include "ApModuleStream.h"

// c22: reflection - projection - matrix
// c0    - { 0.0,  0.5, 1.0, 2.0}
// c1    - { 4.0, .5pi, pi, 2pi}  
// c2    - {1, -1/3!, 1/5!, -1/7!  }  //for sin
// c3    - {1/2!, -1/4!, 1/6!, -1/8!  }  //for cos
// c4-7 - Composite World-View-Projection Matrix
// c8     - ModelSpace Camera Position
// c9     - ModelSpace Light Position
// c10   - {fixup factor for taylor series imprecision, }(0.9855, 0.1, 0, 0)
// c11   - {waveHeight0, waveHeight1, waveHeight2, waveHeight3}  (0.4, 0.5, 0.025, 0.025)
// c13   - {waveSpeed0, waveSpeed1, waveSpeed2, waveSpeed3}   (0.2, 0.15, 0.4, 0.4)
// c14   - {waveDirX0, waveDirX1, waveDirX2, waveDirX3}   (0.25, 0.0, -0.7, -0.8)
// c15   - {waveDirY0, waveDirY1, waveDirY2, waveDirY3}   (0.0, 0.15, -0.7, 0.1)
// c18   - World Martix(Identity이므로 무시)

const char g_vs14_HWater[] =
{
	"vs.1.1\n"
	"dcl_position v0\n"
	"dcl_normal v3\n"
	"dcl_blendweight v5\n"
	"dcl_texcoord0 v7\n"
	"dcl_tangent v8\n"
	"mul r0, c14, v7.x       // use tex coords as inputs to sinusoidal warp\n"
	"mad r0, c15, v7.y, r0   // use tex coords as inputs to sinusoidal warp\n" 
	"mov r1, c16.x           //time...\n"
	"mad r0, r1, c13, r0     // add scaled time to move bumps according to frequency\n"
	"frc r0.xy, r0           // take frac of all 4 components\n"
	"frc r1.xy, r0.zwzw      //\n"
	"mov r0.zw, r1.xyxy      //\n"
	"mul r0, r0, c10.x       // multiply by fixup factor (due to inaccuracy)\n"
	"sub r0, r0, c0.y        // subtract .5\n"
	"mul r0, r0, c1.w        // mult tex coords by 2pi  coords range from(-pi to pi)\n"
	"mul r5, r0, r0          // (wave vec)^2\n"
	"mul r1, r5, r0          // (wave vec)^3\n"
	"mul r6, r1, r0          // (wave vec)^4\n"
	"mul r2, r6, r0          // (wave vec)^5\n"
	"mul r7, r2, r0          // (wave vec)^6\n"
	"mul r3, r7, r0          // (wave vec)^7\n"
	"mul r8, r3, r0          // (wave vec)^8\n"
	"mad r4, r1, c2.y, r0    //(wave vec) - ((wave vec)^3)/3!\n"
	"mad r4, r2, c2.z, r4    //  + ((wave vec)^5)/5!\n"
	"mad r4, r3, c2.w, r4    //  - ((wave vec)^7)/7!\n"
	"mov r0, c0.z            //1\n"
	"mad r5, r5, c3.x ,r0    //-(wave vec)^2/2!\n"
	"mad r5, r6, c3.y, r5    //+(wave vec)^4/4!\n"
	"mad r5, r7, c3.z, r5    //-(wave vec)^6/6!\n"
	"mad r5, r8, c3.w, r5    //+(wave vec)^8/8!\n"
	"sub r0, c0.z, v5.x      //... 1-wave scale\n"
	"mul r4, r4, r0          // scale sin\n"
	"mul r5, r5, r0          // scale cos\n"
	"dp4 r0, r4, c11         //multiply wave heights by waves\n"
	"mul r0.xyz, v3, r0      //multiply wave magnitude at this vertex by normal\n"
	"add r0.xyz, r0, v0      //add to position\n"
	"mov r0.w, c0.z          //homogenous component\n"
	"m4x4    oPos, r0, c4    // OutPos = ObjSpacePos * World-View-Projection Matrix\n"
	
	"mul     r1, r5, c11				//cos* waveheight\n"
	"dp4     r9.x, -r1, c14				//normal x offset\n"
	"dp4     r9.yzw, -r1, c15			//normal y offset and tangent offset\n"
	"mov     r5, v3						//starting normal\n"
	"mad     r5.xz, r9, c10.y, r5		//warped normal move nx, ny according to cos*wavedir*waveeheight\n"
	
	"mov     r4, v8						//tangent\n"
	"mad     r4.y, -r9.x, c10.y, r4.y	//warped tangent vector \n"

	"dp3     r10.x, r5, r5\n"
	"rsq     r10.y, r10.x\n"
	"mul     r5, r5, r10.y				//normalize normal\n"
	
	"dp3     r10.x, r4, r4\n"
	"rsq     r10.y, r10.x\n"
	"mul     r4, r4, r10.y				//normalize tangent\n"
	
	"mul     r3, r4.yzxw, r5.zxyw\n"
	"mad     r3, r4.zxyw, -r5.yzxw, r3	//xprod to find binormal\n"
	
	"dp4	 r9.x, r0, c22		\n"
	"dp4	 r9.y, r0, c23		\n"
	"dp4	 r9.zw, r0, c24		\n"
	"rcp	 r10.x, r9.z		\n"
	"mul	 r9.xy,r9.xy,r10.x	\n"
	"mov	 oT2,	r9					// reflection uv \n"
	
	//@{ Jaewon 20050830
	// NVIDIA bug!
	"mad     r2, c8.w, -r0, c8          //view vector\n"
	//@} Jaewon
	"dp3     r10.x, r2, r2\n"
	"rsq     r10.y, r10.x\n"
	"mul     r2, r2, r10.y				//normalized view vector\n"

	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.xyxy\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT0, r0					//distorted tex coord 0\n"
	
	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.zwzw\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT1, r0.yxzw				//distorted tex coord 1\n"
	
	"dp3	 r0.x,r2,r3	\n"
	"dp3	 r0.y,r2,r4	\n"
	"dp3	 r0.z,r2,r5	\n"
	"mov     oT3, r0					//pass in view vector (texture space)\n"

	"dp3	 r0.x,c9,r3	\n"
	"dp3	 r0.y,c9,r4	\n"
	"dp3	 r0.z,c9,r5	\n"
	"mov     oT4, r0					//pass in light vector (texture space)\n"

	"mov	 oD0, v5	\n				// y val is vertex alpha \n"
};

const char g_ps14_HWater_new[] =
{
	"ps.1.4\n"
	"def	c3, 0.02f, 0.023f, 0.0f, 0.0f	\n"
	
	"texld		r0, t0				;bump map 0									\n"
	"texld		r1, t1				;bump map 1									\n"
	"texcrd		r2.rgb, t2														\n"
	"texld		r3, t3				;View vector								\n"
	"texld		r4, t4				;Light vector								\n"
	"add_d2		r1.xy, r0, r1		;Scaled Average of 2 bumpmaps xy offsets	\n"
	"+mov		r2.a,c0.z			\n"
	"dp3		r0, r1, r3_bx2		;V.N										\n"
	"mad		r5, r1, c3, r2		\n"
	"mad		r2.rgb, r1, r0_x2, -r3_bx2 ;R = 2N(V.N)-V						\n"
	"mov_sat r0, r0\n"
	"dp3		r4.rgb, r2,r4_bx2	;R.L										\n"
	"+mul r3.a, 1-r0, 1-r0\n"
	"mov_sat	r1, r0_x2			;2 * V.N  (sample over range of 1d map!)	\n"
	"mul r3, r3.a, r3.a\n"
	"phase																		\n"
	"texld		r2, r5		        ;reflection map								\n"   
	"texld		r5, r1              ;Index fresnel map using 2*V.N				\n"
	"mul		r2.a,r4.r,r4.r													\n"			
	"mul		r2.rgb,r2,c0.a													\n"
	"+mul		r2.a,r2.a,r2.a													\n"
	"mad_sat r2.rgb, r2, r3.b, r5												\n"
	"+mul		r2.a,r2.a,r2.a													\n"
	"mad_sat	r0.rgb, r2.a, c1, r2   ;+= Specular highlight * highlight color	\n"
	"+mov		r0.a,v0.y														\n"
};

const char g_ps14_HWater_old[] =
{
	"ps.1.4\n"
	"def	c3, 0.02f, 0.023f, 0.0f, 0.0f	\n"

	"texld		r0, t0				;bump map 0									\n"
	"texld		r1, t1				;bump map 1									\n"
	"texcrd		r2.rgb, t2														\n"
	"texcrd		r3.rgb, t3			;View vector								\n"
	"texcrd		r4.rgb, t4			;Light vector								\n"

	"add_d2		r1.xy, r0, r1		;Scaled Average of 2 bumpmaps xy offsets	\n"

	"dp3		r0.rgb, r1, r3		;V.N										\n"
	"+mov		r2.a,c0.z			\n"
	"mad		r5, r1, c3, r2		\n"
	"mad		r2.rgb, r1, r0_x2, -r3 ;R = 2N(V.N)-V							\n"
	"dp3		r4, r2,r4			;R.L										\n"
	"mov_sat	r1, r0_x2		    ;2 * V.N  (sample over range of 1d map!)	\n"
	"phase																		\n"
	"texld		r2, r5		        ;reflection map								\n"   
	"texld		r5, r1              ;Index fresnel map using 2*V.N				\n"
	//"mul		r2.rgb,r2,r2													\n" 
	"mul_d4		r2.a,r4.r,r4.r													\n"			
	"mul_x2		r2.rgb,r2,c0.a													\n"
	"+mul		r2.a,r2.a,r2.a													\n"
	"add_d4_sat r2.rgb, r2, r5_x2												\n"
	"+mul		r2.a,r2.a,r2.a													\n"
	"mad_sat	r0.rgb, r2.a, c1, r2   ;+= Specular highlight * highlight color	\n"
	"+mov		r0.a,v0.y														\n"
};

//@{ Jaewon 20050704
// ;)
const char g_vs11_HWater_specular[] =
{
	"vs.1.1\n"
	"dcl_position v0\n"
	"dcl_normal v3\n"
	"dcl_blendweight v5\n"
	"dcl_texcoord0 v7\n"
	"dcl_tangent v8\n"
	"mul r0, c14, v7.x       // use tex coords as inputs to sinusoidal warp\n"
	"mad r0, c15, v7.y, r0   // use tex coords as inputs to sinusoidal warp\n" 
	"mov r1, c16.x           //time...\n"
	"mad r0, r1, c13, r0     // add scaled time to move bumps according to frequency\n"
	"frc r0.xy, r0           // take frac of all 4 components\n"
	"frc r1.xy, r0.zwzw      //\n"
	"mov r0.zw, r1.xyxy      //\n"
	"mul r0, r0, c10.x       // multiply by fixup factor (due to inaccuracy)\n"
	"sub r0, r0, c0.y        // subtract .5\n"
	"mul r0, r0, c1.w        // mult tex coords by 2pi  coords range from(-pi to pi)\n"
	"mul r5, r0, r0          // (wave vec)^2\n"
	"mul r1, r5, r0          // (wave vec)^3\n"
	"mul r6, r1, r0          // (wave vec)^4\n"
	"mul r2, r6, r0          // (wave vec)^5\n"
	"mul r7, r2, r0          // (wave vec)^6\n"
	"mul r3, r7, r0          // (wave vec)^7\n"
	"mul r8, r3, r0          // (wave vec)^8\n"
	"mad r4, r1, c2.y, r0    //(wave vec) - ((wave vec)^3)/3!\n"
	"mad r4, r2, c2.z, r4    //  + ((wave vec)^5)/5!\n"
	"mad r4, r3, c2.w, r4    //  - ((wave vec)^7)/7!\n"
	"mov r0, c0.z            //1\n"
	"mad r5, r5, c3.x ,r0    //-(wave vec)^2/2!\n"
	"mad r5, r6, c3.y, r5    //+(wave vec)^4/4!\n"
	"mad r5, r7, c3.z, r5    //-(wave vec)^6/6!\n"
	"mad r5, r8, c3.w, r5    //+(wave vec)^8/8!\n"
	"sub r0, c0.z, v5.x      //... 1-wave scale\n"
	"mul r4, r4, r0          // scale sin\n"
	"mul r5, r5, r0          // scale cos\n"
	"dp4 r0, r4, c11         //multiply wave heights by waves\n"
	"mul r0.xyz, v3, r0      //multiply wave magnitude at this vertex by normal\n"
	"add r0.xyz, r0, v0      //add to position\n"
	"mov r0.w, c0.z          //homogenous component\n"
	"m4x4    oPos, r0, c4    // OutPos = ObjSpacePos * World-View-Projection Matrix\n"
	
	"mul     r1, r5, c11				//cos* waveheight\n"
	"dp4     r9.x, -r1, c14				//normal x offset\n"
	"dp4     r9.yzw, -r1, c15			//normal y offset and tangent offset\n"
	"mov     r5, v3						//starting normal\n"
	"mad     r5.xz, r9, c10.y, r5		//warped normal move nx, ny according to cos*wavedir*waveeheight\n"
	
	"mov     r4, v8						//tangent\n"
	"mad     r4.y, -r9.x, c10.y, r4.y	//warped tangent vector \n"

	"dp3     r10.x, r5, r5\n"
	"rsq     r10.y, r10.x\n"
	"mul     r5, r5, r10.y				//normalize normal\n"
	
	"dp3     r10.x, r4, r4\n"
	"rsq     r10.y, r10.x\n"
	"mul     r4, r4, r10.y				//normalize tangent\n"
	
	"mul     r3, r4.yzxw, r5.zxyw\n"
	"mad     r3, r4.zxyw, -r5.yzxw, r3	//xprod to find binormal\n"
	
	"dp4	 r9.x, r0, c22		\n"
	"dp4	 r9.y, r0, c23		\n"
	"dp4	 r9.zw, r0, c24		\n"
	"rcp	 r10.x, r9.z		\n"
	"mul	 r9.xy,r9.xy,r10.x	\n"
	"mov	 oT2,	r9					// reflection uv \n"

	//@{ Jaewon 20050830
	// NVIDIA bug!
	"mad     r2, c8.w, -r0, c8          //view vector\n"
	//@} Jaewon
	"dp3     r10.x, r2, r2\n"
	"rsq     r10.y, r10.x\n"
	"mul     r2, r2, r10.y				//normalized view vector\n"

	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.xyxy\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT0, r0					//distorted tex coord 0\n"
	
	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.zwzw\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT1, r0.yxzw				//distorted tex coord 1\n"
	
	// Get a normalized half-angle vector.
	"add r2, r2, c9\n"
	"dp3 r10.x, r2, r2\n"
	"rsq r10.y, r10.x\n"
	"mul r2, r2, r10.y\n"

	// Transform it to the tangent space and output to the 3rd texture coordinate.
	"dp3 r0.x,r2,r3	\n"
	"dp3 r0.y,r2,r4	\n"
	"dp3 r0.z,r2,r5	\n"
	"mov oT3, r0\n"

	// y is vertex alpha.
	"mov oD0, v5.yyyy\n"
};

const char g_ps11_HWater_specular[] =
{
	"ps.1.1\n"
	"tex t0\n"						// Bump map 0
	"tex t1\n"						// Bump map 1
	"tex t2\n"						// Reflection map
	"tex t3\n"						// Normalized half-angle vector
	"add_d2 r1, t0, t1\n"			// Scaled average of 2 bump maps xy offsets
	"dp3_sat t3, t3_bx2, r1\n"		// H.N
	"mad t2.rgb, t2, c0, c2\n"		// Reflection map * Reflection weight + Base color
	"+mul t3.a, t3.a, t3.a\n"		// (H.N) ^ 2
	"mul t3.a, t3.a, t3.a\n"		// (H.N) ^ 4
	"mul t3.a, t3.a, t3.a\n"		// (H.N) ^ 8
	"mad_sat r0.rgb, t3.a, c1, t2\n"// += (H.N)^16 * Highlight color
	"+mov r0.a, v0\n"
};

const char g_vs11_HWater_Fresnel[] =
{
	"vs.1.1\n"
	"dcl_position v0\n"
	"dcl_normal v3\n"
	"dcl_blendweight v5\n"
	"dcl_texcoord0 v7\n"
	"dcl_tangent v8\n"
	"mul r0, c14, v7.x       // use tex coords as inputs to sinusoidal warp\n"
	"mad r0, c15, v7.y, r0   // use tex coords as inputs to sinusoidal warp\n" 
	"mov r1, c16.x           //time...\n"
	"mad r0, r1, c13, r0     // add scaled time to move bumps according to frequency\n"
	"frc r0.xy, r0           // take frac of all 4 components\n"
	"frc r1.xy, r0.zwzw      //\n"
	"mov r0.zw, r1.xyxy      //\n"
	"mul r0, r0, c10.x       // multiply by fixup factor (due to inaccuracy)\n"
	"sub r0, r0, c0.y        // subtract .5\n"
	"mul r0, r0, c1.w        // mult tex coords by 2pi  coords range from(-pi to pi)\n"
	"mul r5, r0, r0          // (wave vec)^2\n"
	"mul r1, r5, r0          // (wave vec)^3\n"
	"mul r6, r1, r0          // (wave vec)^4\n"
	"mul r2, r6, r0          // (wave vec)^5\n"
	"mul r7, r2, r0          // (wave vec)^6\n"
	"mul r3, r7, r0          // (wave vec)^7\n"
	"mul r8, r3, r0          // (wave vec)^8\n"
	"mad r4, r1, c2.y, r0    //(wave vec) - ((wave vec)^3)/3!\n"
	"mad r4, r2, c2.z, r4    //  + ((wave vec)^5)/5!\n"
	"mad r4, r3, c2.w, r4    //  - ((wave vec)^7)/7!\n"
	"mov r0, c0.z            //1\n"
	"mad r5, r5, c3.x ,r0    //-(wave vec)^2/2!\n"
	"mad r5, r6, c3.y, r5    //+(wave vec)^4/4!\n"
	"mad r5, r7, c3.z, r5    //-(wave vec)^6/6!\n"
	"mad r5, r8, c3.w, r5    //+(wave vec)^8/8!\n"
	"sub r0, c0.z, v5.x      //... 1-wave scale\n"
	"mul r4, r4, r0          // scale sin\n"
	"mul r5, r5, r0          // scale cos\n"
	"dp4 r0, r4, c11         //multiply wave heights by waves\n"
	"mul r0.xyz, v3, r0      //multiply wave magnitude at this vertex by normal\n"
	"add r0.xyz, r0, v0      //add to position\n"
	"mov r0.w, c0.z          //homogenous component\n"
	"m4x4    oPos, r0, c4    // OutPos = ObjSpacePos * World-View-Projection Matrix\n"

	"mul     r1, r5, c11				//cos* waveheight\n"
	"dp4     r9.x, -r1, c14				//normal x offset\n"
	"dp4     r9.yzw, -r1, c15			//normal y offset and tangent offset\n"
	"mov     r5, v3						//starting normal\n"
	"mad     r5.xz, r9, c10.y, r5		//warped normal move nx, ny according to cos*wavedir*waveeheight\n"

	"mov     r4, v8						//tangent\n"
	"mad     r4.y, -r9.x, c10.y, r4.y	//warped tangent vector \n"

	"dp3     r10.x, r5, r5\n"
	"rsq     r10.y, r10.x\n"
	"mul     r5, r5, r10.y				//normalize normal\n"

	"dp3     r10.x, r4, r4\n"
	"rsq     r10.y, r10.x\n"
	"mul     r4, r4, r10.y				//normalize tangent\n"

	"mul     r3, r4.yzxw, r5.zxyw\n"
	"mad     r3, r4.zxyw, -r5.yzxw, r3	//xprod to find binormal\n"

	"dp4	 r9.x, r0, c22		\n"
	"dp4	 r9.y, r0, c23		\n"
	"dp4	 r9.zw, r0, c24		\n"
	"rcp	 r10.x, r9.z		\n"
	"mul	 r9.xy,r9.xy,r10.x	\n"
	"mov	 oT2,	r9					// reflection uv \n"

	//@{ Jaewon 20050830
	// NVIDIA bug!
	"mad     r2, c8.w, -r0, c8          //view vector\n"
	//@} Jaewon
	"dp3     r10.x, r2, r2\n"
	"rsq     r10.y, r10.x\n"
	"mul     r2, r2, r10.y				//normalized view vector\n"

	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.xyxy\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT0, r0					//distorted tex coord 0\n"

	"mov     r0, c16.x\n"
	"mul     r0, r0, c17.zwzw\n"
	"frc     r0.xy, r0					//frc of incoming time\n"
	"add     r0, v7, r0					//add time to tex coords\n"
	"mov     oT1, r0.yxzw				//distorted tex coord 1\n"

	// Transform the view vector to the tangent space and output to the 3rd texture coordinate.
	"dp3 r0.x,r2,r3	\n"
	"dp3 r0.y,r2,r4	\n"
	"dp3 r0.z,r2,r5	\n"
	"mov oT3, r0\n"

	// y is vertex alpha.
	"mov oD0, v5.yyyy\n"
};

const char g_ps11_HWater_Fresnel[] =
{
	"ps.1.1\n"
	"tex t0\n"						// Bump map 0
	"tex t1\n"						// Bump map 1
	"tex t2\n"						// Reflection map
	"tex t3\n"						// Normalized view vector
	"add_d2 r1, t0, t1\n"			// Scaled average of 2 bump maps xy offsets
	"dp3_sat t3, t3_bx2, r1\n"		// V.N
	"mul t2.rgb, t2, c0\n"			// Reflection map * Reflection weight
	"+mul t3.a, 1-t3, 1-t3\n"		// (1 - V.N) ^ 2
	"mul t3.a, t3, t3\n"			// (1 - V.N) ^ 4
	"mad_sat r0.rgb, t3.a, t2, c2\n"// Fresnel term * Reflection + Base Color
	"+mov r0.a, v0\n"
};
//@} Jaewon

// 스태틱 맴버 초기화,,
AgcmWater	* AgcmWater::m_pThis	= NULL	;

extern AgcEngine *	g_pEngine;

static rwD3D9DeviceRestoreCallBack _oldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack _oldD3D9ReleaseDeviceCB = NULL;

static void newD3D9ReleaseDeviceCB()
{
	//@{ 2006/11/30 burumal
	if ( NULL == AgcmWater::m_pThis )
		return;
	//@}

	AgcmWater::m_pThis->onLostDevice();

	if(_oldD3D9ReleaseDeviceCB)
		_oldD3D9ReleaseDeviceCB();
}

static void newD3D9RestoreDeviceCB()
{
	//@{ 2006/11/30 burumal
	if ( NULL == AgcmWater::m_pThis )
		return;
	//@}

	if(_oldD3D9RestoreDeviceCB)
		_oldD3D9RestoreDeviceCB();

	AgcmWater::m_pThis->onResetDevice();
}

extern "C"
{
void	_rwD3D9RenderStateFlushCache();

extern void         *_rwD3D9LastVertexShaderUsed;
extern void         *_rwD3D9LastPixelShaderUsed; 

extern D3DXMATRIX	_RwD3D9D3D9ProjTransform;
extern D3DXMATRIX*	_RwD3D9ActiveViewProjTransform;
}

AgcmWater::AgcmWater()
{
	AgcmWater::m_pThis	= this	;

	SetModuleName("AgcmWater");
	EnableIdle(TRUE);

	m_pcsAgcmMap	= NULL;
	m_pcsAgcmRender	= NULL;
	m_pcsAgcmOcTree = NULL;
	m_pcsApmOcTree	= NULL;
	m_pcsAgcmEventNature = NULL;
	m_pcsAgpmReturnToLogin = NULL;

	m_listSector  = NULL;

	m_ulTickDiff = 0;
	m_uiLastTick = 0;

	INT32 i;
	for(i=0;i<WMAP_FILE_NUM;++i)	
	{
		m_prWaterT[i] = NULL;
		m_prWaveT[i] = NULL;
		m_prHQFresnel[i] = NULL;

		m_prHQBumpSrc[i] = NULL;
		m_prHQBump[i] = NULL;
	}
		
	//@{ Jaewon 20050705
	// ;)
	m_pNormCubeMap = NULL;
	//@} Jaewon

	for(i=0;i<WMAP_STATUS_NUM;++i)
	{
		m_TexuvL1[i].x = m_TexuvL1[i].y = 0.0f;
		m_TexuvL2[i].x = m_TexuvL2[i].y = 0.0f;

		RwMatrixSetIdentity(&m_TexMatL1[i]);
		RwMatrixSetIdentity(&m_TexMatL2[i]);
	
		memset(&m_stWaterStatus[i],0,sizeof(WaterStatus));
	}

	for(i=0;i<WMAP_FILE_NUM;++i)
	{
		memset(&m_stWaterFInfo[i],0,sizeof(WaterFileInfo));
		memset(&m_stWaveFInfo[i],0,sizeof(WaveFileInfo));

		memset(&m_stHQBumpFInfo[i],0,sizeof(WaterFileInfo));
		memset(&m_stHQFresnelFInfo[i],0,sizeof(WaterFileInfo));
	}

	for(i=0;i<WMAP_HWATER_STATUS_NUM;++i)
	{
		// Default 값으로 Set
		m_stHWaterStatus[i].Height0 = 0.4f;
		m_stHWaterStatus[i].Height1 = 0.5f;
		m_stHWaterStatus[i].Height2 = 0.025f;
		m_stHWaterStatus[i].Height3 = 0.025f;
		m_stHWaterStatus[i].Speed0 = 0.2f;
		m_stHWaterStatus[i].Speed1 = 0.15f;
		m_stHWaterStatus[i].Speed2 = 0.4f;
		m_stHWaterStatus[i].Speed3 = 0.4f;
		m_stHWaterStatus[i].DirX0 = 2.0f;
		m_stHWaterStatus[i].DirX1 = 0.0f;
		m_stHWaterStatus[i].DirX2 = -7.0f;
		m_stHWaterStatus[i].DirX3 = -8.0f;
		m_stHWaterStatus[i].DirY0 = 0.0f;
		m_stHWaterStatus[i].DirY1 = 1.0f;
		m_stHWaterStatus[i].DirY2 = -7.0f;
		m_stHWaterStatus[i].DirY3 = 1.0f;
		m_stHWaterStatus[i].TexX0 = 0.031f;
		m_stHWaterStatus[i].TexY0 = 0.04f;
		m_stHWaterStatus[i].TexX1 = -0.03f;
		m_stHWaterStatus[i].TexY1 = 0.02f;
		m_stHWaterStatus[i].Red = 0.8f;
		m_stHWaterStatus[i].Green = 0.76f;
		m_stHWaterStatus[i].Blue = 0.92f;
		
		m_stHWaterStatus[i].Alpha_Min = 0.1f;
		m_stHWaterStatus[i].Alpha_Max = 0.75f;
		m_stHWaterStatus[i].Alpha_DecreaseHeight = 400.0f;

		m_stHWaterStatus[i].Height_Min = 0.0f;
		m_stHWaterStatus[i].Height_Max = 1.0f;
		m_stHWaterStatus[i].Height_DecreaseHeight = 400.0f;

		m_stHWaterStatus[i].VertexPerTile = 2;

		m_stHWaterStatus[i].ReflectionWeight = 1.0f;

		m_stHWaterStatus[i].BumpTexID = 0;
		m_stHWaterStatus[i].FresnelTexID = 0;

		//@{ Jaewon 20050706
		// ;)
		m_stHWaterStatus[i].BaseR = 0.3f;
		m_stHWaterStatus[i].BaseG = 0.3f;
		m_stHWaterStatus[i].BaseB = 0.3f;
		//@} Jaewon

		//@{ Jaewon 20050705
		// ;)
		m_stHWaterStatus[i].Shader14ID = 0;
		m_stHWaterStatus[i].Shader11ID = 0;
		//@} Jaewon
	}

	m_iWaterFileNum = 0;
	m_iWaveFileNum = 0;

	m_iHQBumpFileNum = 0;
	m_iHQFresnelFileNum = 0;

	m_iMaxWaterIDUsed = 10;	// 아래 hw water id 맥스값과 일치 시킬려고 했으나 별로 소용없을것 같아 그대로둠(burumal)

	//@{ 2006/06/30 burumal
	/*	
	m_iMaxHWaterIDUsed = 10;
	*/
	m_iMaxHWaterIDUsed = WMAP_HWATER_STATUS_NUM;
	//@}

	m_iWaterDetail	= WATER_DETAIL_MEDIUM;
	
	//m_listFO = NULL;
	//m_iFOCount = 0;

	m_listDrawWave = NULL;
	m_listDrawHWater = NULL;

	m_pCurD3D9Device = NULL;
	m_bEnableHWater	= FALSE;

	//@{ Jaewon 20050705
	// There are 2 versions of the shader pair.
	m_pHWaterPS14[0] = NULL;
	m_pHWaterVS14[0] = NULL;
	m_pHWaterPS14[1] = NULL;
	m_pHWaterVS14[1] = NULL;
	m_pHWaterPS11[0] = NULL;
	m_pHWaterVS11[0] = NULL;
	m_pHWaterPS11[1] = NULL;
	m_pHWaterVS11[1] = NULL;
	//@} Jaewon

	m_fTime = 0.0f;

	m_pReflectionCamera = NULL;
	m_pReflectionRaster = NULL;
	m_pReflectionRasterZ = NULL;
	m_pMainCamera	= NULL;
	m_bReflectionUpdate = FALSE;
	m_iReflectionFreq = 10;
	memset(&m_matReflectionTrafoMatrix,0,sizeof(D3DMATRIX));

	m_pVertexDecl = NULL;

	m_pReflectionTexture	= NULL;
	m_bEnableUserClipPlane = FALSE;

	memset(&m_matSetProj,0,sizeof(D3DXMATRIX));
	memset(&m_matSetViewProj,0,sizeof(D3DXMATRIX));
	memset(&m_matBackupProj,0,sizeof(D3DXMATRIX));
	memset(&m_matBackupViewProj,0,sizeof(D3DXMATRIX));

	m_iJobQueueCount = 0;
	m_iJobQueueMaxCount = 0;
	m_pJobQueue = NULL;

	m_iReflectionUpdateCount = 0;
	m_iSectorCount = 0;

	m_iTemp = 0;
	m_fReflectionWaterHeight = 0.0f;

	//@{ Jaewon 20050705
	// ;)
	m_bForce11Shader = false;
	//@} Jaewon
}

AgcmWater::~AgcmWater()
{

}

BOOL AgcmWater::OnAddModule()
{
	// 모듈 등록될때 일어나는 이벤트.
	m_pcsAgcmMap = ( AgcmMap* ) GetModule( "AgcmMap" );
	ASSERT( NULL != m_pcsAgcmMap && "워터모듈은 맵 클라이언트 모듈 뒤에 등록돼어야 합니다." );

	m_pcsAgcmRender = (AgcmRender* ) GetModule( "AgcmRender" );
	m_pcsAgcmOcTree = (AgcmOcTree*) GetModule("AgcmOcTree");
	m_pcsApmOcTree = (ApmOcTree*)	GetModule("ApmOcTree");
	m_pcsAgcmEventNature = (AgcmEventNature*) GetModule("AgcmEventNature");
	m_pcsAgpmReturnToLogin = (AgpmReturnToLogin*) GetModule("AgpmReturnToLogin");
		
	// 맵 클라이언트 모듈에 워터 포인터 등록함..
	if (m_pcsAgcmMap)
		m_pcsAgcmMap->SetSectorWaterCallback( OnLoadSectorWaterLevelCallback , OnReleaseSectorWaterLevelCallback );

	//m_pcsAgcmRender->SetCallbackPostRender(CB_POST_RENDER,this);
	
	//m_pcsAgcmRender->SetCallbackPreRender(CB_PRE_RENDER,this);

	m_pcsAgcmRender->SetCallbackPostRender2(CB_POST_RENDER2,this);
	m_pcsAgcmRender->SetCallbackPostRenderOcTreeClear(CB_POST_RENDER_REFLECTION_DRAW,this);

	if (m_pcsAgpmReturnToLogin)
		m_pcsAgpmReturnToLogin->SetCallbackRequestKey(CBDeleteAllQueue, this);

	_oldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	_oldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	_rwD3D9DeviceSetReleaseCallback(newD3D9ReleaseDeviceCB);
    _rwD3D9DeviceSetRestoreCallback(newD3D9RestoreDeviceCB);

	return TRUE;
}

BOOL AgcmWater::OnInit()
{
	INT32				i;
	int xwidth = 0;
	double	rad = 0.0f;

	if(m_pcsAgcmRender->m_iLoadRangeX2 > m_pcsAgcmRender->m_iLoadRangeX1)
	{
		xwidth = m_pcsAgcmRender->m_iLoadRangeX2 - m_pcsAgcmRender->m_iLoadRangeX1;
	}
	else
	{
		xwidth = m_pcsAgcmRender->m_iLoadRangeX1 - m_pcsAgcmRender->m_iLoadRangeX2;
	}

	m_iJobQueueMaxCount = xwidth*xwidth*2000;						// maptool 위해..
	if(m_iJobQueueMaxCount == 0) m_iJobQueueMaxCount = 40000;		// client는 4000정도 쓰자..
	m_pJobQueue = new WaterJobQueue[m_iJobQueueMaxCount];
	m_uiLastTick = 0;
		
	for(i=0;i<360;++i)
	{
		rad = (double)i * AU_DEG2RAD;

		m_fSinTable[i] = ( FLOAT ) sin(rad);
	}

	m_pCurD3D9Device = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice ();

	const D3DCAPS9 *caps = (D3DCAPS9*)RwD3D9GetCaps();

	//@{ Jaewon 20050704
	// ps.1.4 -> m_bEnableWater = 1, ps.1.1 -> m_bEnableWater = 2
	if ( (caps->VertexShaderVersion & 0xffff) >= 0x0101 && (caps->PixelShaderVersion & 0xffff) >= 0x0101)
	{
		if((caps->PixelShaderVersion & 0xffff) >= 0x0104)
			m_bEnableHWater = 1;
		else
			m_bEnableHWater = 2;

		m_matReflectionTrafoMatrix._11 = 1.0f;
		m_matReflectionTrafoMatrix._12 = 0.0f;
		m_matReflectionTrafoMatrix._13 = 0.0f;
		m_matReflectionTrafoMatrix._14 = 0.0f;
		m_matReflectionTrafoMatrix._21 = 0.0f;
		m_matReflectionTrafoMatrix._22 = -1.0f;
		m_matReflectionTrafoMatrix._23 = 0.0f;
		m_matReflectionTrafoMatrix._24 = 0.0f;
		m_matReflectionTrafoMatrix._31 = 0.0f;
		m_matReflectionTrafoMatrix._32 = 0.0f;
		m_matReflectionTrafoMatrix._33 = 1.0f;
		m_matReflectionTrafoMatrix._34 = 0.0f;
		m_matReflectionTrafoMatrix._41 = 0.0f;
		m_matReflectionTrafoMatrix._42 = 0.0f;
		m_matReflectionTrafoMatrix._43 = 0.0f;
		m_matReflectionTrafoMatrix._44 = 1.0f;

		// vertex declaration for post-processing
		D3DVERTEXELEMENT9 HWaterVertexDecl[] =
		{
			{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
			{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			{ 0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
			D3DDECL_END()
		};

		// create a vertex declaration
		if(RwD3D9CreateVertexDeclaration(HWaterVertexDecl, (void**)&m_pVertexDecl) == FALSE)
		{
			ASSERT(0);
			m_pVertexDecl = NULL;

			return TRUE;
		}

		if( caps->MaxUserClipPlanes > 0 )	m_bEnableUserClipPlane = TRUE;
	}
	//@} Jaewon
	else 
	{
		m_bEnableHWater = FALSE;
	}

	//@{ Jaewon 20050705
	// Load the normalization cube map.
	m_pNormCubeMap = RwTextureRead("Normalize", NULL);
	if(m_pNormCubeMap)
		RwTextureSetFilterMode(m_pNormCubeMap, rwFILTERLINEAR);
	else
		ASSERT(!"m_pNormCubeMap in AgcmWater cannot be read!\n");
	//@} Jaewon

	m_csCSection.Init();

	return TRUE;
}

void	AgcmWater::RemoveAll()
{
	Sector*			cur_sec = m_listSector;
	Sector*			remove_sec = NULL;

	LockFrame();

	while(cur_sec)
	{
		DeleteAll(cur_sec);

		remove_sec = cur_sec;
		cur_sec = cur_sec->next;

		delete remove_sec;
	}
	
	m_listSector = NULL;

	UnlockFrame();
}

BOOL AgcmWater::OnDestroy()
{
	RemoveAll();
	
	INT32 i;
	for(i=0;i<m_iWaterFileNum;++i)
	{
		if( m_prWaterT[i] )
		{
			RwTextureDestroy(m_prWaterT[i]);
			m_prWaterT[i] = NULL;
		}
	}

	for(i=0;i<m_iWaveFileNum;++i)
	{
		if( m_prWaveT[i] )
		{
			RwTextureDestroy(m_prWaveT[i]);
			m_prWaveT[i] = NULL;
		}
	}

	for(i=0;i<m_iHQBumpFileNum;++i)
	{
		if( m_prHQBumpSrc[i] )
		{
			RwTextureDestroy(m_prHQBumpSrc[i]);
			m_prHQBumpSrc[i] = NULL;
		}

		if( m_prHQBump[i] )
		{
			m_prHQBump[i]->Release();
			m_prHQBump[i] = NULL;
		}
	}

	for(i=0;i<m_iHQFresnelFileNum;++i)
	{
		if( m_prHQFresnel[i] )
		{
			RwTextureDestroy(m_prHQFresnel[i]);
			m_prHQFresnel[i] = NULL;
		}
	}

	//@{ Jaewon 20050705
	// ;)
	if(m_pNormCubeMap)
	{
		RwTextureDestroy(m_pNormCubeMap);
		m_pNormCubeMap = NULL;
	}
	//@} Jaewon

	//@{ Jaewon 20050705
	// There are 2 versions of the shader pair.
	for(i=0; i<2; ++i)
	{
		if(m_pHWaterVS14[i])
		{
			RwD3D9DeleteVertexShader(m_pHWaterVS14[i]);
			m_pHWaterVS14[i] = NULL;
		}

		if(m_pHWaterPS14[i])
		{
			RwD3D9DeletePixelShader(m_pHWaterPS14[i]);
			m_pHWaterPS14[i] = NULL;
		}
	}
	for(i=0; i<2; ++i)
	{
		if(m_pHWaterVS11[i])
		{
			RwD3D9DeleteVertexShader(m_pHWaterVS11[i]);
			m_pHWaterVS11[i] = NULL;
		}

		if(m_pHWaterPS11[i])
		{
			RwD3D9DeletePixelShader(m_pHWaterPS11[i]);
			m_pHWaterPS11[i] = NULL;
		}
	}
	//@} Jaewon

	LockFrame();
	if (m_pReflectionCamera)
    {
		RpWorldRemoveCamera(g_pEngine->m_pWorld , m_pReflectionCamera);

		RwFrame            *frame;

		frame = RwCameraGetFrame(m_pReflectionCamera);
        if (frame)
        {
            RwCameraSetFrame(m_pReflectionCamera, NULL);
            RwFrameDestroy(frame);
        }
		        
        RwCameraDestroy(m_pReflectionCamera);
		m_pReflectionCamera = NULL;
    }
	UnlockFrame();

//	if(m_pReflectionRaster)
//	{
//		RwRasterDestroy(m_pReflectionRaster);
//		m_pReflectionRaster = NULL;
//	}

	if(m_pReflectionRasterZ)
	{
		RwRasterDestroy(m_pReflectionRasterZ);
		m_pReflectionRasterZ = NULL;
	}

	if(m_pReflectionTexture)
	{
		RwTextureDestroy(m_pReflectionTexture);
		m_pReflectionTexture = NULL;
	}

	// destroy the vertex declaration
	if(m_pVertexDecl)
	{
		RwD3D9DeleteVertexDeclaration(m_pVertexDecl);
		m_pVertexDecl = NULL;
	}

	m_csCSection.Lock();
	/*WaterFOList*	cur_FO = m_listFO;
	WaterFOList*	remove_FO;
	while(cur_FO)
	{
		remove_FO = cur_FO;
		cur_FO = cur_FO->next;

		CloseHandle(remove_FO->fd);
		delete remove_FO;
	}

	m_listFO = NULL;
	m_iFOCount = 0;*/

	JobQueueRemoveAll();
	delete [] m_pJobQueue;

	m_csCSection.Unlock();

	return TRUE;
}

void AgcmWater::onLostDevice()
{

}

void AgcmWater::onResetDevice()
{

}

BOOL AgcmWater::OnIdle( UINT32 ulClockCount )
{
	PROFILE("AgcmWater::OnIdle");

	m_ulTickDiff = ulClockCount - m_uiLastTick;
	m_uiLastTick = ulClockCount;

	INT32	i;
	
	for(i=0;i<m_iMaxWaterIDUsed;++i)
	{
		m_TexMatL1[i].pos.x += m_TexuvL1[i].x;
		m_TexMatL1[i].pos.y += m_TexuvL1[i].y;

		m_TexMatL2[i].pos.x += m_TexuvL2[i].x;
		m_TexMatL2[i].pos.y += m_TexuvL2[i].y;

		RwMatrixUpdate (&m_TexMatL1[i]);
		RwMatrixUpdate (&m_TexMatL2[i]);
	}

	m_listDrawHWater = NULL;
	m_bReflectionUpdate = FALSE;

	if(m_bEnableHWater && m_iWaterDetail == WATER_DETAIL_HIGH)
	{
		m_fTime += (FLOAT)m_ulTickDiff * 0.001f;
		//@{ Jaewon 20050830
		// 100000.0f -> 10000.0f
		// NVIDIA cards have a bug related to large(>20000.0) shader constants.
		if(m_fTime > 10000.0f) m_fTime = 0.0f;
		//@} Jaewon

		//@{ 2006/06/30 burumal : 그냥 공회전 하고 있기에 주석 처리함
		/*
		for(i =0; i<m_iMaxHWaterIDUsed ; ++i)
		{
			// hwater update처리
		}
		*/
		//@}
	}

	// Job Queue 처리 !!
	m_csCSection.Lock();
	Wave*		pWave = NULL;
	Sector*		pSec = NULL;
	Water*		pWater = NULL;

	for(i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type == WATER_JOB_REMOVED)
			continue;

		pWave = ( Wave * ) m_pJobQueue[i].data2;
		pWater = ( Water * ) m_pJobQueue[i].data2;

		pSec = (Sector*) m_pJobQueue[i].data1;

		//@{ 2006/09/05 burumal
		if ( pSec && IsBadReadPtr( pSec, sizeof(Sector)) )
			continue;
		//@}

		if(m_pJobQueue[i].type == WATER_JOB_ADDWAVE)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if ( IsBadReadPtr( pWave, sizeof(Wave)) || (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water)))) )
			//if( IsBadReadPtr( pWave, sizeof(Wave)) )
			//@}
				continue;

			AddWaveToRender( pWave );
			
			pSec->AddedRenderWave = true;								// 파도 추가 플래그
		}
		else if(m_pJobQueue[i].type == WATER_JOB_REMOVEWAVE)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if ( IsBadReadPtr( pWave, sizeof(Wave)) || (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water)))) )
			//if( IsBadReadPtr( pWave, sizeof(Wave)) )
			//@}
				continue;

			RemoveWaveFromRender( pWave );
			
			pSec->AddedRenderWave = false;								// 파도 추가 플래그
		}
		else if(m_pJobQueue[i].type == WATER_JOB_WATERADDTOSECTOR)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if ( IsBadReadPtr( pWater, sizeof(Water)) || (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water)))) )
			//if( IsBadReadPtr( pWater, sizeof(Water)) )
			//@}
				continue;
			
			pWater->next = pSec->WaterList;
			pSec->WaterList = pWater;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_MAKEANDADDHWATER)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water))))			
				continue;
			//@}

			MakeHWaterFromWater(pSec);
			ChangeHWaterMode(TRUE,pSec);
			pSec->AddedRenderHWater = true;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_ADDWATER)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if ( IsBadReadPtr( pWater, sizeof(Water)) || (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water)))) )
			//if( IsBadReadPtr( pWater, sizeof(Water)) )
			//@}
				continue;

			AddWaterToRender( pWater );
			pSec->AddedRenderWater = true;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_WAVEADDTOSECTOR)
		{
			//@{ 2006/05/22 burumal
			if ( IsBadReadPtr( pWave, sizeof(Wave)) || (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water)))) )
			//if( IsBadReadPtr( pWave, sizeof(Wave)) )
			//@}
				continue;

			pWave->next = pSec->WaveList;
			pSec->WaveList = pWave;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_SECTORADDTOSECTORLIST)
		{
			//@{ 2006/05/22 burumal
			//. 2006. 7. 20. Nonstopdj validation condition edit
			if (!pSec || (pSec->WaterList && IsBadReadPtr(pSec->WaterList, sizeof(tag_water))))
				continue;
			//@}

			pSec->next = m_listSector;
			m_listSector = pSec;
			++m_iSectorCount;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_REMOVESECTOR)
		{
			Sector*		cur_sec = m_listSector;
			Sector*		before_sec;
			while(cur_sec)
			{
				if(cur_sec== pSec)
				{
					DeleteAll(cur_sec);
					if(cur_sec == m_listSector)
					{
						m_listSector = cur_sec->next;
					}
					else
					{
						before_sec->next = cur_sec->next;
					}

					delete cur_sec;
					--m_iSectorCount;
					break;
				}

				before_sec = cur_sec;
				cur_sec = cur_sec->next;
			}
		}
	}

	m_iJobQueueCount = 0;
	m_csCSection.Unlock();

	return TRUE;
}

void	AgcmWater::OnCameraStateChange(CAMERASTATECHANGETYPE	ctype)
{
	if(m_bEnableHWater && m_pMainCamera && m_pReflectionCamera)
	{
		FLOAT	fNear = RwCameraGetNearClipPlane(m_pMainCamera);
		FLOAT	fFar = RwCameraGetFarClipPlane(m_pMainCamera);
		FLOAT	fFog = RwCameraGetFogDistance(m_pMainCamera);
		const RwV2d*	vView = RwCameraGetViewWindow(m_pMainCamera);
		
		LockFrame();
		RwCameraSetNearClipPlane(m_pReflectionCamera,fNear);
		RwCameraSetFarClipPlane(m_pReflectionCamera,fFar);
		RwCameraSetFogDistance(m_pReflectionCamera,fFog);
		RwCameraSetViewWindow(m_pReflectionCamera,vView);
		UnlockFrame();
	}
}

BOOL	AgcmWater::CB_POST_RENDER_REFLECTION_DRAW( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmWater*		pThis = (AgcmWater*) pClass;
	
	if(pThis->m_bEnableHWater && pThis->m_iWaterDetail == WATER_DETAIL_HIGH)
	{
		pThis->LockFrame();
		RwFrame*	camFrame1 = RwCameraGetFrame(pThis->m_pMainCamera);
		RwFrame*	camFrame2 = RwCameraGetFrame(pThis->m_pReflectionCamera);
		
		RwMatrix*	camMatrix1 = RwFrameGetMatrix(camFrame1);
		RwMatrix*	camMatrix2 = RwFrameGetMatrix(camFrame2);

		RwMatrixCopy(camMatrix2,camMatrix1);
		
		camMatrix2->up.y = -camMatrix2->up.y;
		camMatrix2->right.x = -camMatrix2->right.x;
		camMatrix2->right.z = -camMatrix2->right.z;
		camMatrix2->at.y = -camMatrix2->at.y;

		float diffy = camMatrix2->pos.y - pThis->m_fReflectionWaterHeight;
		camMatrix2->pos.y -= 2*diffy;

		camMatrix2->pos.x -= camMatrix2->at.x * 200.0f;
		camMatrix2->pos.y -= camMatrix2->at.y * 200.0f;
		camMatrix2->pos.z -= camMatrix2->at.z * 200.0f;
		
		RwFrameUpdateObjects(camFrame2);

		if(pThis->m_bReflectionUpdate && pThis->m_iReflectionFreq++ >= pThis->m_iReflectionUpdateCount)
		{
			pThis->m_iReflectionFreq = 0;
			pThis->UpdateReflectionMap();
		}
		pThis->UnlockFrame();
	}

	return TRUE;
}

BOOL	AgcmWater::CB_POST_RENDER2 ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*) pClass;

	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );

	if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH)
		pThis->RenderHQWater();

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDONE);

	DrawWave*	cur_Wave = pThis->m_listDrawWave;
	int			before_statusid = -1;
	while(cur_Wave)
	{
		if(before_statusid != cur_Wave->wave->StatusID)
		{
			before_statusid = cur_Wave->wave->StatusID;
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
				(void *)RwTextureGetRaster(pThis->m_prWaveT[pThis->m_stWaterStatus[before_statusid].WaveTexID]));
		}

		if (RwIm3DTransform(cur_Wave->wave->vert, 6, &cur_Wave->wave->mat_final,
							rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ |
							rwIM3D_VERTEXRGBA ))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
			RwIm3DEnd();
		}
		
		cur_Wave = cur_Wave->next;
	}

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);

	pThis->m_listDrawWave = NULL;

	return TRUE;
}

BOOL	AgcmWater::CB_W1Update ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*) pClass;
	Water*		pW1 = (Water*)pData;
	
	return TRUE;
}

#define			WAVE_SCALE_MIN		(1.0f)
BOOL	AgcmWater::CB_W2Update ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*) pClass;
	Wave*			pW2 = (Wave*)pData;

	pThis->WaveUpdate(pW2);
		
	return TRUE;
}

BOOL	AgcmWater::CB_W2Render ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*) pClass;
	Wave*			pWave = (Wave*)pData;

	if(pWave->UpdateCount > 0)
	{
		DrawWave*		nw_DrawWave = (DrawWave*)AcuFrameMemory::AllocFrameMemory(sizeof(DrawWave));
		if( nw_DrawWave )
		{
			nw_DrawWave->wave = pWave;
			nw_DrawWave->next = pThis->m_listDrawWave;
			pThis->m_listDrawWave = nw_DrawWave;
		}
	}

	return TRUE;
}

BOOL	AgcmWater::CB_W1Release ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	return TRUE;
}

BOOL	AgcmWater::CB_W2Release ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	return TRUE;
}

void	AgcmWater::WaveUpdate(Wave*	pW2)
{
	static	const RwV3d		yaxis = {0.0f,1.0f,0.0f};

	INT32			i;
	RwV3d			W2pos = {0.0f, 0.0f, 0.0f};
	RwV3d			W2ScaleF = {1.0f, 1.0f, 1.0f};
	int				StatusID = pW2->StatusID;
			
	if(pW2->UpdateCount >= 0)
	{
		pW2->UpdateCount += m_ulTickDiff;

		float	per = (float)pW2->UpdateCount / (float)m_stWaterStatus[StatusID].WaveLifeTime;

		if(per<0.7f)
		{
			if(pW2->WaveDir == 0)
			{
				pW2->WaveHeight += 0.01f * m_ulTickDiff ;
			}
			else if(pW2->WaveDir == 1)
			{
				pW2->WaveHeight -= 0.01f * m_ulTickDiff ;
			}

			if(pW2->WaveHeight > 6.0f)
			{
				pW2->WaveDir = 1;
			}
			else if(pW2->WaveHeight < -6.0f) 
			{
				pW2->WaveDir = 0;
			}
		}
		else
		{
			pW2->WaveHeight += 0.015f * m_ulTickDiff;
		}

		if(pW2->AlphaDir == 0)
		{
			pW2->Alpha += pW2->AlphaDiff * m_ulTickDiff;
			if(pW2->Alpha > 1.0f)
			{
				pW2->Alpha = 1.0f;
				pW2->AlphaDir = 1;
				pW2->AlphaDiff = 1.0f / (m_stWaterStatus[StatusID].WaveLifeTime * 0.4f);
			}
		}
		else if(pW2->AlphaDir == 1)
		{
			pW2->Alpha -= pW2->AlphaDiff * m_ulTickDiff;
			if(pW2->Alpha < 0.0f) pW2->Alpha = 0.0f;
		}
		
		for(i=0;i<6;++i)
		{
			RwIm3DVertexSetRGBA (&pW2->vert[i] ,
				m_stWaterStatus[StatusID].WaveRed,
				m_stWaterStatus[StatusID].WaveGreen,
				m_stWaterStatus[StatusID].WaveBlue,
				(char)(pW2->Alpha * 255.0f));
		}

		if(per >= 1.0f)
		{
			W2ScaleF.x = pW2->WaveScaleX;
			W2ScaleF.y = 1.0f;
			W2ScaleF.z = pW2->WaveScaleZ;

			// respawn 대기
			pW2->UpdateCount = pW2->SpawnCount;

			pW2->pos = pW2->pos_origin;
						
			pW2->WaveScaleX = 1.0f;
			pW2->WaveScaleZ = 1.0f;

			pW2->Alpha = 0.0f;
			pW2->AlphaDiff = 1.0f / (m_stWaterStatus[StatusID].WaveLifeTime * 0.6f);
			pW2->AlphaDir = 0;

			pW2->WaveHeight = 0.0f;
			pW2->WaveDir = rand()%2;
		}
		else
		{
			if(per < 0.5f)
			{
				pW2->pos.x += pW2->MoveAmount.x * m_ulTickDiff;
				pW2->pos.z += pW2->MoveAmount.z * m_ulTickDiff;
			}
			else if(per < 0.6f)
			{
				pW2->pos.x += pW2->MoveAmount.x * m_ulTickDiff * 0.9f;
				pW2->pos.z += pW2->MoveAmount.z * m_ulTickDiff * 0.9f;
			}
			else
			{
				float	fTemp = m_ulTickDiff * 0.7f;
				pW2->pos.x += pW2->MoveAmount.x * fTemp;
				pW2->pos.z += pW2->MoveAmount.z * fTemp;
			}

			int		deg = (int)(360.0f * per*0.5f);
			pW2->WaveScaleX += m_fSinTable[deg] * pW2->WaveScaleXOffset * m_ulTickDiff;
			pW2->WaveScaleZ += m_fSinTable[deg] * pW2->WaveScaleZOffset * m_ulTickDiff;

			W2pos = pW2->pos;
			W2pos.y +=  pW2->WaveHeight;

			W2ScaleF.x = pW2->WaveScaleX;
			W2ScaleF.y = 1.0f;
			W2ScaleF.z = pW2->WaveScaleZ;
		}

		RwMatrixScale(&pW2->mat_final,&W2ScaleF,rwCOMBINEREPLACE);
		RwMatrixRotate(&pW2->mat_final,&yaxis,pW2->YRotAngle,rwCOMBINEPOSTCONCAT);
		RwMatrixTranslate(&pW2->mat_final,&W2pos,rwCOMBINEPOSTCONCAT);
	}
	else
	{
		pW2->UpdateCount += m_ulTickDiff;

		if(pW2->UpdateCount > 0)
		{
			for(i=0;i<6;++i)
			{
				RwIm3DVertexSetRGBA (&pW2->vert[i] , 255,255,255,0);
			}

			RwMatrixRotate(&pW2->mat_final,&yaxis,pW2->YRotAngle,rwCOMBINEPOSTCONCAT);
			RwMatrixTranslate(&pW2->mat_final,&pW2->pos_origin,rwCOMBINEPOSTCONCAT);
		}
	}
}

void	AgcmWater::SetWaterDetail(enumWaterDetail	detail)
{
	Sector*			cur_sec = m_listSector;
	Wave*			cur_Wave = NULL;

	// 워터 랜더링 오류를 해결할 때 까지 무조건 중옵으로 고정..
	if( detail == WATER_DETAIL_HIGH )
		detail = WATER_DETAIL_MEDIUM;

	if(detail == WATER_DETAIL_HIGH && m_bEnableHWater)
	{
		MakeHWaterFromWater();
		ChangeHWaterMode(TRUE);
	}
	else
	{
		if(m_iWaterDetail == WATER_DETAIL_HIGH)
		{
			ChangeHWaterMode(FALSE);
		}

		if(detail == WATER_DETAIL_MEDIUM)
		{
			while(cur_sec)
			{
				if(!cur_sec->AddedRenderWave)
				{
					cur_Wave = cur_sec->WaveList;

					while(cur_Wave)
					{
						AddWaveToRender(cur_Wave);
						cur_Wave = cur_Wave->next;
					}
					cur_sec->AddedRenderWave = true;
				}
				cur_sec = cur_sec->next;
			}
		}
		else
		{
			while(cur_sec)
			{
				if(cur_sec->AddedRenderWave)
				{
					cur_Wave = cur_sec->WaveList;
					while(cur_Wave)
					{
						RemoveWaveFromRender(cur_Wave);
						cur_Wave = cur_Wave->next;
					}
									
					cur_sec->AddedRenderWave = false;
				}
				
				cur_sec = cur_sec->next;
			}
		}
	}

	m_iWaterDetail = detail;
}

void	AgcmWater::DeleteAll(Sector*	sec)
{
	// Water Delete
	try
	{
		Water*	cur_Water = sec->WaterList;
		Water*	remove_Water = NULL;
		while(cur_Water)
		{
			remove_Water = cur_Water;
			cur_Water = cur_Water->next;
			
			if(sec->AddedRenderWater)
				RemoveWaterFromRender(remove_Water);
			ReleaseWater(remove_Water);

			delete remove_Water;
		}

		//. 2006. 5. 30. nonstopdj
		sec->WaterList = NULL;
	}
	catch( ... )
	{
		MD_SetErrorMessage( "AgcmWater::DeleteAll (%d,%d섹터) , WaterList 오류\n" , sec->SectorXIndex , sec->SectorZIndex );
	}

	try
	{
		Wave*	cur_Wave = sec->WaveList;
		Wave*	remove_Wave = NULL;
		
		// Wave Delete
		while(cur_Wave)
		{
			remove_Wave = cur_Wave;
			cur_Wave = cur_Wave->next;

			if(sec->AddedRenderWave)
				RemoveWaveFromRender(remove_Wave);
			ReleaseWave(remove_Wave);
				
			delete remove_Wave;
		}
		
		//. 2006. 5. 30. nonstopdj
		sec->WaveList = NULL;
	}
	catch( ... )
	{
		MD_SetErrorMessage( "AgcmWater::DeleteAll (%d,%d섹터) , WaveList 오류\n" , sec->SectorXIndex , sec->SectorZIndex );
	}

	
	try
	{
		HWater*		cur_HW = sec->HWaterList;
		HWater*		remove_HW = NULL;
		
		//HWater Delete
		while(cur_HW)
		{
			remove_HW = cur_HW;
			cur_HW  = cur_HW->next;

			if(sec->AddedRenderHWater)
				RemoveHWaterFromRender(remove_HW);
			ReleaseHWater(remove_HW);

			delete remove_HW;
		}
		
		//. 2006. 5. 30. nonstopdj
		sec->HWaterList = NULL;
	}
	catch( ... )
	{
		MD_SetErrorMessage( "AgcmWater::DeleteAll (%d,%d섹터) , HWaterList 오류\n" , sec->SectorXIndex , sec->SectorZIndex );
	}
}

void	AgcmWater::ReleaseWater(Water*	remove_Water)
{
	RwFrame*	remove_frame = NULL;

	LockFrame();

	if(remove_Water->AtomicL1)
	{
		TRACE("Water::ReleaseWater - %x\n",remove_Water->AtomicL1);

		remove_frame = RpAtomicGetFrame(remove_Water->AtomicL1);

		RpAtomicSetFrame(remove_Water->AtomicL1,NULL);
		RwFrameDestroy(remove_frame);

		RpAtomicDestroy(remove_Water->AtomicL1);
	}
	
	UnlockFrame();
}

void	AgcmWater::ReleaseWave(Wave*	remove_Wave)
{
	// do nothing ^^
}

void	AgcmWater::	AddWaterToRender(Water*	water)
{
	/*if(m_stWaterStatus[water->StatusID].RenderBMode_L1 == -1)
	{
		m_pcsAgcmRender->AddAtomicToWorld(water->AtomicL1, ONLY_NONALPHA);
		
		m_pcsAgcmRender->AddUpdateInfotoAtomic(water->AtomicL1,this,AgcmWater::CB_W1Update,AgcmWater::CB_W1Release,
									(PVOID)water,NULL);
	}
	else
	{
		m_pcsAgcmRender->AddAlphaBModeAtomicToWorld(water->AtomicL1 , (enumRenderBMode)m_stWaterStatus[water->StatusID].RenderBMode_L1,FALSE
			,AGCMRENDER_ADD_NONE,true);
		
		m_pcsAgcmRender->AddUpdateInfotoAtomic(water->AtomicL1,this,AgcmWater::CB_W1Update,AgcmWater::CB_W1Release,
									(PVOID)water,NULL);
	}*/

	m_pcsAgcmRender->AddAtomicToWorld(water->AtomicL1, ONLY_LASTZWRITE);
	m_pcsAgcmRender->AddUpdateInfotoAtomic(water->AtomicL1,this,AgcmWater::CB_W1Update,AgcmWater::CB_W1Release,
									(PVOID)water,NULL);
}

void	AgcmWater::	RemoveWaterFromRender(Water*	remove_Water)
{
	m_pcsAgcmRender->RemoveAtomicFromWorld(remove_Water->AtomicL1);

	m_pcsAgcmRender->RemoveUpdateInfoFromAtomic2(remove_Water->AtomicL1,this,
								AgcmWater::CB_W1Update,AgcmWater::CB_W1Release);
}

void	AgcmWater::	AddWaveToRender(Wave*	wave)
{
	if(m_pcsApmOcTree->m_bOcTreeEnable)
	{
		OcCustomDataList	stSetParam;

		memset(&stSetParam, 0, sizeof(OcCustomDataList));

		stSetParam.pData2 = NULL;
		stSetParam.iAppearanceDistance = WAVE_VIEW_SECTOR_RANGE;
		stSetParam.pClass = this;
		stSetParam.pRenderCB = CB_W2Render;
		stSetParam.pUpdateCB = CB_W2Update;
		//@{ 2006/11/17 burumal
		/*
		stSetParam.pDistCorrectCB = CB_DistCorrect;
		*/
		stSetParam.pDistCorrectCB = NULL;
		//@}

		stSetParam.pData1 = wave;
		stSetParam.BS.center = wave->BS.center;
		stSetParam.BS.radius = wave->BS.radius;
		stSetParam.piCameraZIndex = &m_iTemp; 

		stSetParam.TopVerts[0].x = stSetParam.BS.center.x - stSetParam.BS.radius;
		stSetParam.TopVerts[0].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[0].z = stSetParam.BS.center.z - stSetParam.BS.radius;

		stSetParam.TopVerts[1].x = stSetParam.BS.center.x + stSetParam.BS.radius;
		stSetParam.TopVerts[1].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[1].z = stSetParam.BS.center.z - stSetParam.BS.radius;

		stSetParam.TopVerts[2].x = stSetParam.BS.center.x + stSetParam.BS.radius;
		stSetParam.TopVerts[2].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[2].z = stSetParam.BS.center.z + stSetParam.BS.radius;

		stSetParam.TopVerts[3].x = stSetParam.BS.center.x - stSetParam.BS.radius;
		stSetParam.TopVerts[3].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[3].z = stSetParam.BS.center.z + stSetParam.BS.radius;

		m_pcsAgcmOcTree->AddCustomRenderDataToOcTree(wave->pos_origin.x,wave->pos_origin.y,
												wave->pos_origin.z,&stSetParam);
	}
	else
	{
		m_pcsAgcmRender->AddCustomRenderToSector(this,&wave->BS,CB_W2Update,CB_W2Render,(PVOID)wave,NULL);
	}
}

void	AgcmWater::	RemoveWaveFromRender(Wave*	wave)
{
	if(m_pcsApmOcTree->m_bOcTreeEnable)
	{
		m_pcsAgcmOcTree->RemoveCustomRenderDataFromOcTree(wave->pos_origin.x,wave->pos_origin.y,
																	wave->pos_origin.z,(PVOID)this,(PVOID)wave,NULL);
	}			
	else
	{
		m_pcsAgcmRender->RemoveCustomRenderFromSector(this,&wave->BS,(PVOID)wave,NULL);
	}
}

void	AgcmWater::JobQueueRemoveAll()
{
	int i;
	Water*	pWater;
	Wave*	pWave;
	Sector*	pSec;
	for(i=0;i<m_iJobQueueCount;++i)
	{
		// new 한 객체 release
		if(m_pJobQueue[i].type == WATER_JOB_WAVEADDTOSECTOR)
		{
			pWave = (Wave*)m_pJobQueue[i].data2;
			ReleaseWave(pWave);
			delete pWave;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_WATERADDTOSECTOR)
		{
			pWater = (Water*)m_pJobQueue[i].data2;
			ReleaseWater(pWater);
			delete pWater;
		}
		else if(m_pJobQueue[i].type == WATER_JOB_SECTORADDTOSECTORLIST)
		{
			pSec = (Sector*)m_pJobQueue[i].data1;
			delete pSec;
		}
	}

	m_iJobQueueCount = 0;
}

void	AgcmWater::JobQueueAdd (enumWaterJobType	type,PVOID data1,PVOID data2)
{
	m_csCSection.Lock();

	INT32				i;
	BOOL	bFind = FALSE;

	for(i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type	== type && m_pJobQueue[i].data1 == data1 && m_pJobQueue[i].data2 == data2)
		{
			bFind = TRUE;
			break;
		}

		#ifdef _DEBUG
		if( type == WATER_JOB_REMOVESECTOR &&
			m_pJobQueue[i].data1 == data1 )	// 같은 섹터..
		{
			// RemoveSector가 호출되기전에 
			// 섹터 딜리트가 들어가는경우
			__asm nop;
		}
		#endif // _DEBUG
	}

	if(!bFind)
	{
		ASSERT(m_iJobQueueCount < m_iJobQueueMaxCount);

		if(m_iJobQueueCount >= m_iJobQueueMaxCount)		// 초과시 버퍼 늘림..
		{
			WaterJobQueue*	temp = new WaterJobQueue[m_iJobQueueMaxCount];
			int				count = m_iJobQueueMaxCount;
			memcpy(temp,m_pJobQueue,sizeof(WaterJobQueue) * count);
			delete	[]m_pJobQueue;

            m_iJobQueueMaxCount *= 2;		
			m_pJobQueue = new WaterJobQueue[m_iJobQueueMaxCount];
			memcpy(m_pJobQueue,temp,sizeof(WaterJobQueue) * count);

			delete	[]temp;
		}

		m_pJobQueue[m_iJobQueueCount].type = type;
		m_pJobQueue[m_iJobQueueCount].data1 = data1;
		m_pJobQueue[m_iJobQueueCount].data2 = data2;

		++m_iJobQueueCount;
	}

	m_csCSection.Unlock();
}

void	AgcmWater::JobQueueRemove(INT32	six,INT32 siz)
{
	m_csCSection.Lock();
	Water*	pWater = NULL;
	Wave*	pWave = NULL;
	Sector*	pSec = NULL;
	INT32	i;
	INT32	j;

	for(i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type == WATER_JOB_REMOVED) continue;

		pSec = (Sector*)m_pJobQueue[i].data1;

		if(pSec->SectorXIndex == six && pSec->SectorZIndex == siz)
		{
			if(m_pJobQueue[i].type == WATER_JOB_REMOVESECTOR || m_pJobQueue[i].type == WATER_JOB_REMOVEWAVE 
				|| m_pJobQueue[i].type == WATER_JOB_SECTORADDTOSECTORLIST)
			{

			}
			else
			{
				// new 한 객체 release
				if(m_pJobQueue[i].type == WATER_JOB_WAVEADDTOSECTOR)
				{
					pWave = (Wave*)m_pJobQueue[i].data2;
					ReleaseWave(pWave);
					delete pWave;
				}
				else if(m_pJobQueue[i].type == WATER_JOB_WATERADDTOSECTOR)
				{
					pWater = (Water*)m_pJobQueue[i].data2;
					ReleaseWater(pWater);
					delete pWater;
				}
				m_pJobQueue[i].type = WATER_JOB_REMOVED;
			}
		}
	}

	// sector release
	for(i=0;i<m_iJobQueueCount;++i)
	{
		if(m_pJobQueue[i].type == WATER_JOB_REMOVED) continue;

		pSec = (Sector*)m_pJobQueue[i].data1;
		if(pSec->SectorXIndex == six && pSec->SectorZIndex == siz)
		{
			if(m_pJobQueue[i].type == WATER_JOB_SECTORADDTOSECTORLIST)
			{
				for(j=i;j<m_iJobQueueCount;++j)
				{
					if((Sector*)m_pJobQueue[j].data1 == pSec)
					{
						m_pJobQueue[j].type = WATER_JOB_REMOVED;
					}
				}

				delete pSec;
				m_pJobQueue[i].type = WATER_JOB_REMOVED;
			}
		}
	}
		
	m_csCSection.Unlock();
}

// changedetail 여기서 수행하자! 재검색을 피하기 위해
void	AgcmWater::OnLoadSectorWaterLevelCallback		( ApWorldSector* pWSector , INT32 nDetail )
{
	ASSERT( NULL != AgcmWater::m_pThis );

	//@{ 2006/11/30 burumal
	if ( NULL == AgcmWater::m_pThis )
		return;
	//@}

	AgcmWater* pThis = AgcmWater::m_pThis;

	INT32	six = pWSector->GetIndexX();
	INT32	siz = pWSector->GetIndexZ();

	BOOL	bFind = FALSE;

	pThis->m_csCSection.Lock();

	Sector*			cur_sec = pThis->m_listSector;
	Wave*			cur_Wave = NULL;
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			bFind = TRUE;
			if(cur_sec->Detail != nDetail) // Change Detail!
			{
				if(nDetail == SECTOR_LOWDETAIL)
				{
					if(cur_sec->AddedRenderWave)
					{
						cur_Wave = cur_sec->WaveList;

						while(cur_Wave)
						{
							pThis->JobQueueAdd( WATER_JOB_REMOVEWAVE , cur_sec, cur_Wave);
							cur_Wave = cur_Wave->next;
						}
					}
				}
				else if(nDetail == SECTOR_HIGHDETAIL)
				{
					if(!cur_sec->AddedRenderWave)
					{
						cur_Wave = cur_sec->WaveList;

						while(cur_Wave)
						{
							pThis->JobQueueAdd( WATER_JOB_ADDWAVE , cur_sec, cur_Wave);
							cur_Wave = cur_Wave->next;
						}
					}

					if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH && pThis->m_bEnableHWater)
					{
						pThis->JobQueueAdd(WATER_JOB_MAKEANDADDHWATER, cur_sec, NULL);
					}
				}
			}
		}

		cur_sec = cur_sec->next;
	}
	pThis->m_csCSection.Unlock();

	if(bFind == FALSE)
	{
		// 없었으므로 file에서 읽자
		INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH; // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
		INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

		if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
		if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

		INT32 loffx = six - lsix;
		INT32 loffz = siz - lsiz;

		FLOAT start_x = pWSector->GetXStart();
		FLOAT start_z = pWSector->GetZStart();

		INT32	x_arrayindex = SectorIndexToArrayIndexX(lsix);
		INT32	z_arrayindex = SectorIndexToArrayIndexZ(lsiz);
		INT32	division_index = GetDivisionIndex( x_arrayindex , z_arrayindex );
		
		char		fstr[100];
		memset(fstr,'\0',100);

		#ifdef USE_MFC
		sprintf(fstr,"map\\data\\water\\WT%d.dat",division_index);
		#else
		sprintf(fstr,"world\\water\\WT%d.dat",division_index);
		#endif

		AuPackingManager*		pPackingManager = AuPackingManager::GetSingletonPtr();
		ApdFile		csApdFile;

		if(!pPackingManager->OpenFile(fstr,&csApdFile))
		{
			//ASSERT(!"파일을 읽는데 실패하였습니다.Water");
			return;
		}

		DWORD	version = 0;
		pPackingManager->ReadFile( &version, 4, &csApdFile );

#ifdef _DEBUG
		if(version != 1 && version != 2)			// version 체크
		{
			ASSERT(!"water 파일 버전이 틀립니다!");
			pPackingManager->CloseFile(&csApdFile);
			return;
		}
#endif

#ifdef USE_MFC
		if(version != 1 && version != 2)			// version 체크
		{
			MD_SetErrorMessage("water 파일 버전이 틀립니다!");
			pPackingManager->CloseFile(&csApdFile);
			return;
		}
#endif

		INT32	foffset = 0;
		INT32	iLoadSize = 0;
		foffset = loffz * MAP_DEFAULT_DEPTH * 8 + loffx * 8 + 4;
		pPackingManager->SetPos(foffset,&csApdFile);
		pPackingManager->ReadFile( &foffset, 4, &csApdFile );
		pPackingManager->ReadFile( &iLoadSize, 4, &csApdFile );
			
/*		INT32	foffset = 0;
		DWORD	foffset2 = 0;
		INT32	iLoadSize = 0;

		foffset = loffz * MAP_DEFAULT_DEPTH * 4  + loffx * 4;

		pPackingManager->SetPos(foffset,&csApdFile);
		pPackingManager->ReadFile( &foffset, 4, &csApdFile );

		if(loffx+1 >= MAP_DEFAULT_DEPTH)
		{
			if(loffz+1 >= MAP_DEFAULT_DEPTH)
			{
				foffset2 = pPackingManager->GetFileSize( &csApdFile );
			}
			else
			{
				foffset2 = (loffz + 1) * MAP_DEFAULT_DEPTH * 4;

				pPackingManager->SetPos(foffset2,&csApdFile);
				pPackingManager->ReadFile( &foffset2, 4, &csApdFile );
			}
		}
		else
		{
			foffset2 = loffz * MAP_DEFAULT_DEPTH * 4 + (loffx + 1) * 4;

			pPackingManager->SetPos(foffset2,&csApdFile);
			pPackingManager->ReadFile( &foffset2, 4, &csApdFile );
		}
		
		iLoadSize = foffset2 - foffset;*/

		DWORD*		loadBuffer = new DWORD[iLoadSize/4];
		INT32		iLoadIndex = 0;
		INT32		root_num = 0;
		INT16*		pTemp_i16;
		RwV3d*		pTemp_v3;

		pPackingManager->SetPos(foffset,&csApdFile);
		INT32	LoadFileSize = pPackingManager->ReadFile( loadBuffer, iLoadSize, &csApdFile );
		if (!LoadFileSize)
		{
			delete	[]loadBuffer;
			pPackingManager->CloseFile(&csApdFile);
			return;
		}

		pPackingManager->CloseFile(&csApdFile);

		pThis->JobQueueRemove(six,siz);

		Sector*	nw_sec = new Sector;
		nw_sec->SectorXIndex = six;
		nw_sec->SectorZIndex = siz;
		nw_sec->Detail = nDetail;
		nw_sec->WaterList = NULL;
		nw_sec->WaveList = NULL;
		nw_sec->HWaterList = NULL;

		nw_sec->AddedRenderWave = false;
		nw_sec->AddedRenderWater = false;
		nw_sec->AddedRenderHWater = false;

		INT32 WaterNum = 0;
		INT32 WaveNum = 0;
		FLOAT WaterHeight = 0;

		WaterNum = ((INT32*)loadBuffer)[iLoadIndex++];

		Water*	nw_Water = NULL;
		Wave*	nw_Wave = NULL;
		INT32	i;

		for(i=0;i<WaterNum;++i)		//	Water추가
		{
			nw_Water = new Water;

			nw_Water->StatusID = ((INT32*)loadBuffer)[iLoadIndex++];

			pTemp_i16 = (INT16*)&(loadBuffer[iLoadIndex]);
			nw_Water->XOffset = *pTemp_i16;
			nw_Water->ZOffset = *(pTemp_i16+1);
			nw_Water->TileXLength = *(pTemp_i16+2);
			nw_Water->TileZLength = *(pTemp_i16+3);
			iLoadIndex += 2;

			nw_Water->Height = ((FLOAT*)loadBuffer)[iLoadIndex++];
			WaterHeight = nw_Water->Height;

			nw_Water->pos_origin.x = start_x + nw_Water->XOffset *MAP_STEPSIZE + (float)nw_Water->TileXLength * (float)MAP_STEPSIZE*0.5f;
			nw_Water->pos_origin.y = nw_Water->Height;
			nw_Water->pos_origin.z = start_z + nw_Water->ZOffset *MAP_STEPSIZE + (float)nw_Water->TileZLength * (float)MAP_STEPSIZE*0.5f;
			
			pThis->JobQueueAdd( WATER_JOB_WATERADDTOSECTOR , nw_sec, nw_Water );
			
			if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH && pThis->m_bEnableHWater)
			{
				pThis->MakeWater(nw_Water,nw_Water->StatusID,FALSE);

				pThis->JobQueueAdd(WATER_JOB_MAKEANDADDHWATER, nw_sec, NULL);
			}
			else
			{
				pThis->MakeWater(nw_Water,nw_Water->StatusID,FALSE);
				pThis->JobQueueAdd ( WATER_JOB_ADDWATER , nw_sec , nw_Water );
			}
		}

		WaveNum = ((INT32*)loadBuffer)[iLoadIndex++];
		
		for(i=0;i<WaveNum;++i)		// L2추가
		{
			nw_Wave = new Wave;

			nw_Wave->StatusID = ((INT32*)loadBuffer)[iLoadIndex++];
			
			//@{ 2006/05/12 burumal
			if ( nw_Wave->StatusID < 0 || nw_Wave->StatusID >= WMAP_STATUS_NUM )
			{
				delete nw_Wave;
				continue;
			}
			//@}

			pTemp_i16 = (INT16*)&(loadBuffer[iLoadIndex]);
			nw_Wave->XOffset = *pTemp_i16;
			nw_Wave->ZOffset = *(pTemp_i16+1);
			++iLoadIndex;

			nw_Wave->Width = ((INT32*)loadBuffer)[iLoadIndex++];
			nw_Wave->Height = ((INT32*)loadBuffer)[iLoadIndex++];
			
			nw_Wave->SpawnCount = ((INT32*)loadBuffer)[iLoadIndex++];
			// SpawnCount 텀이 너무 길다.. 나중에 version 2인지 체크하자..
			if(version == 1) nw_Wave->SpawnCount += 2000;
			
			nw_Wave->YRotAngle = ((FLOAT*)loadBuffer)[iLoadIndex++];
			nw_Wave->TranslationSpeed = ((FLOAT*)loadBuffer)[iLoadIndex++];

			pTemp_v3 = (RwV3d*)&(loadBuffer[iLoadIndex]);
			nw_Wave->pos_origin = *pTemp_v3;
			iLoadIndex += 3;

			pThis->WaveDataSet(nw_Wave);
            			
			if(nDetail == SECTOR_HIGHDETAIL)
			{
				pThis->MakeWave(nw_Wave,nw_Wave->StatusID,FALSE);
				pThis->JobQueueAdd(WATER_JOB_ADDWAVE ,nw_sec , nw_Wave);
			}
			else
			{
				pThis->MakeWave(nw_Wave,nw_Wave->StatusID,FALSE);
			}
		
			pThis->JobQueueAdd(WATER_JOB_WAVEADDTOSECTOR,nw_sec , nw_Wave);
		}

		pThis->JobQueueAdd( WATER_JOB_SECTORADDTOSECTORLIST , nw_sec, NULL);
		
		delete []loadBuffer;
	}
}

void	AgcmWater::OnReleaseSectorWaterLevelCallback	( ApWorldSector* pWSector , INT32 nDetail )
{
	//@{ 2006/11/30 burumal
	if ( NULL == AgcmWater::m_pThis )
		return;
	//@}

	if (nDetail == SECTOR_HIGHDETAIL)
		return;

	ASSERT( NULL != AgcmWater::m_pThis );

	AgcmWater* pThis = AgcmWater::m_pThis;

	INT32	six = pWSector->GetIndexX();
	INT32	siz = pWSector->GetIndexZ();

	FLOAT	sx = pWSector->GetXStart();
	FLOAT	sz = pWSector->GetZStart();

	Sector*		cur_sec = pThis->m_listSector;

	pThis->JobQueueRemove(six,siz);
	
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			pThis->JobQueueAdd(WATER_JOB_REMOVESECTOR,cur_sec,NULL);

			/*pThis->DeleteAll(cur_sec);
			
			if(cur_sec == pThis->m_listSector)
			{
				pThis->m_listSector = cur_sec->next;
			}
			else
			{
				before_sec->next = cur_sec->next;
			}

			delete cur_sec;*/

			return;
		}

		cur_sec = cur_sec->next;
	}
}

BOOL	AgcmWater::AddWater(FLOAT x,FLOAT z,FLOAT xlength,FLOAT zlength,FLOAT height,INT32 type,INT32 status_id)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	FLOAT   x2 = x+xlength;
	FLOAT	z2 = z+zlength;
	INT32	six2 = PosToSectorIndexX(x2);
	INT32	siz2 = PosToSectorIndexZ(z2);
	INT32	xoff2 = (INT32)(( x2 + 1 - GetSectorStartX( six2 )) / MAP_STEPSIZE);
	INT32	zoff2 = (INT32)(( z2 + 1 - GetSectorStartZ( siz2 )) / MAP_STEPSIZE);

	// sector별로 쪼개기 작업
	if(six2 > six && siz2 > siz)
	{
		xlength = GetSectorStartX(six+1) - x - 1;
		zlength = GetSectorStartZ(siz+1) - z - 1;

		FLOAT	tx1 = GetSectorStartX(six+1);
		FLOAT	tz1 = GetSectorStartZ(siz+1);
		FLOAT	txl1 = x2 - tx1;
		FLOAT	tzl1 = z2 - tz1;
		
		AddWater(x,z,xlength,zlength,height,type,status_id);
		AddWater(tx1,z,txl1,zlength,height,type,status_id);
		AddWater(x,tz1,xlength,tzl1,height,type,status_id);
		AddWater(tx1,tz1,txl1,tzl1,height,type,status_id);		// 자신의 sector영역이 아니면 재귀호출
		return TRUE;
	}
	else if(six2 > six)
	{
		xlength = GetSectorStartX(six+1) - x - 1;
 
		FLOAT	tx1 = GetSectorStartX(six+1);
		FLOAT	txl1 = x2 - tx1;
		
		AddWater(x,z,xlength,zlength,height,type,status_id);
		AddWater(tx1,z,txl1,zlength,height,type,status_id);		// 자신의 sector영역이 아니면 재귀호출
		return TRUE;
	}
	else if(siz2 > siz)
	{
		zlength = GetSectorStartZ(siz+1) - z - 1;

		FLOAT	tz1 = GetSectorStartZ(siz+1);
		FLOAT	tzl1 = z2 - tz1;
		
		AddWater(x,z,xlength,zlength,height,type,status_id);
		AddWater(x,tz1,xlength,tzl1,height,type,status_id);		// 자신의 sector영역이 아니면 재귀호출
		return TRUE;
	}

	Sector*		cur_sec = m_listSector;
	Water*		cur_Water = NULL;
	Water*		before_Water = NULL;
	Water*		remove_Water = NULL;
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			// 기존의 L1과 중복부분이 있으면 최대영역으로 update
			BOOL					bOverlap = FALSE;

			FLOAT					minx = x;
			FLOAT					minz = z;

			FLOAT					maxXlength = xlength;
			FLOAT					maxZlength = zlength;

			FLOAT					maxx = x + xlength;
			FLOAT					maxz = z + zlength;

			FLOAT					lwx = 0.0f;
			FLOAT					lwz = 0.0f;
			FLOAT					lwxend = 0.0f;
			FLOAT					lwzend = 0.0f;

			cur_Water = cur_sec->WaterList;
			while(cur_Water)
			{
				lwx = GetSectorStartX( six ) + cur_Water->XOffset * MAP_STEPSIZE;
				lwz = GetSectorStartZ( siz ) + cur_Water->ZOffset * MAP_STEPSIZE;

				lwxend = lwx + cur_Water->TileXLength * MAP_STEPSIZE;
				lwzend = lwz + cur_Water->TileZLength * MAP_STEPSIZE;

				if(minx < lwxend && maxx > lwx && minz < lwzend && maxz > lwz)
				{	
					bOverlap = TRUE;
					// 중복 
					RemoveWaterFromRender(cur_Water);
					ReleaseWater(cur_Water);

					remove_Water = cur_Water;
					cur_Water = cur_Water->next;

					if(remove_Water == cur_sec->WaterList)
					{
						cur_sec->WaterList = remove_Water->next;
					}
					else
					{
						before_Water->next = remove_Water->next;
					}	

					delete	remove_Water;

					minx = MIN(minx,lwx);
					minz = MIN(minz,lwz);

					maxXlength = MAX(maxx,lwxend) - minx;
					maxZlength = MAX(maxz,lwzend) - minz;

					maxx = minx + maxXlength;
					maxz = minz + maxZlength;
				}
				else 
				{
					before_Water = cur_Water;
					cur_Water = cur_Water->next;
				}
			}

			if(bOverlap)
			{
				INT32	lxoff = (INT32)(( minx - GetSectorStartX( six )) / MAP_STEPSIZE);
				INT32	lzoff = (INT32)(( minz - GetSectorStartZ( siz )) / MAP_STEPSIZE);

				INT32	xremain = (INT32)( minx - GetSectorStartX( six )) % (INT32)MAP_STEPSIZE;
				INT32	zremain = (INT32)( minz - GetSectorStartX( siz )) % (INT32)MAP_STEPSIZE;
				INT32	lxtilelength = (INT32) ((maxXlength + xremain + 1) / MAP_STEPSIZE);
				INT32	lztilelength = (INT32) ((maxZlength + zremain + 1) / MAP_STEPSIZE);

				if(lxtilelength > 0 && lztilelength > 0)
				{
					
					Water*	nw_Water = new Water;
					nw_Water->StatusID = status_id;
					nw_Water->XOffset = lxoff;
					nw_Water->ZOffset = lzoff;
					nw_Water->TileXLength = lxtilelength;			// 이값은 tile 길이로 전환
					nw_Water->TileZLength = lztilelength;
					nw_Water->Height = height;

					nw_Water->pos_origin.x = GetSectorStartX(six) + nw_Water->XOffset * MAP_STEPSIZE 
											+ (float)nw_Water->TileXLength * MAP_STEPSIZE*0.5f;
					nw_Water->pos_origin.y = height;
					nw_Water->pos_origin.z = GetSectorStartX(siz) + nw_Water->ZOffset * MAP_STEPSIZE 
											+ (float)nw_Water->TileZLength * MAP_STEPSIZE*0.5f;

					MakeWater(nw_Water,status_id);

					nw_Water->next = cur_sec->WaterList;
					cur_sec->WaterList = nw_Water;

					cur_sec->AddedRenderWater = true;
				}

				// 지형체크하여 파도 만들자
				AutoMakeWave(cur_sec);
			}
			else
			{
				if(xoff2 - xoff > 0 && zoff2 - zoff> 0)			// tile길이 1이상만 추가
				{
					// 중복되는 부분이 없었으므로 만든다.
					Water*	nw_Water = new Water;
					nw_Water->StatusID = status_id;
					nw_Water->XOffset = xoff;
					nw_Water->ZOffset = zoff;
					nw_Water->TileXLength = xoff2 - xoff;			// 이값은 tile 길이로 전환
					nw_Water->TileZLength = zoff2 - zoff;
					nw_Water->Height = height;

					nw_Water->pos_origin.x = GetSectorStartX(six) + xoff * MAP_STEPSIZE 
											+ (float)nw_Water->TileXLength * MAP_STEPSIZE*0.5f;
					nw_Water->pos_origin.y = height;
					nw_Water->pos_origin.z = GetSectorStartX(siz) + zoff * MAP_STEPSIZE 
											+ (float)nw_Water->TileZLength * MAP_STEPSIZE*0.5f;

					MakeWater(nw_Water,status_id);

					nw_Water->next = cur_sec->WaterList;
					cur_sec->WaterList = nw_Water;

					cur_sec->AddedRenderWater = true;
				}
				
				// 지형체크하여 파도 만들자
				AutoMakeWave(cur_sec);
			}

			return TRUE;
		}

		cur_sec = cur_sec->next;
	}
	
	//Sector와 layer들을 없으므로 만든다.
	Sector*		nw_sec = new Sector;
	nw_sec->SectorXIndex = six;
	nw_sec->SectorZIndex = siz;
	nw_sec->Detail = SECTOR_HIGHDETAIL;
	nw_sec->WaterList = NULL;
	nw_sec->WaveList = NULL;
	nw_sec->HWaterList = NULL;
	
	if(xoff2 - xoff > 0 && zoff2 - zoff > 0)
	{
		Water*	nw_Water = new Water;
		nw_Water->StatusID = status_id;
		nw_Water->XOffset = xoff;
		nw_Water->ZOffset = zoff;
		nw_Water->TileXLength = xoff2 - xoff;			// 이값은 tile 길이로 전환
		nw_Water->TileZLength = zoff2 - zoff;
		nw_Water->Height = height;

		nw_Water->pos_origin.x = GetSectorStartX(six) + xoff * MAP_STEPSIZE 
								+ (float)nw_Water->TileXLength * MAP_STEPSIZE*0.5f;
		nw_Water->pos_origin.y = height;
		nw_Water->pos_origin.z = GetSectorStartX(siz) + zoff * MAP_STEPSIZE 
								+ (float)nw_Water->TileZLength * MAP_STEPSIZE*0.5f;

		MakeWater(nw_Water,status_id);

		nw_Water->next = nw_sec->WaterList;
		nw_sec->WaterList = nw_Water;

		nw_sec->AddedRenderWater = true;
	}

	nw_sec->AddedRenderHWater = false;

	AutoMakeWave(nw_sec);
	nw_sec->AddedRenderWave = true;

	nw_sec->next = m_listSector;
	m_listSector = nw_sec;
		
	return FALSE;
}

BOOL	AgcmWater::DeleteWater(FLOAT x,FLOAT z)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	Sector*		cur_sec = m_listSector;
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			Water*	cur_Water = cur_sec->WaterList;
			Water*	before_Water = NULL;
			while(cur_Water)
			{
				if(xoff >= cur_Water->XOffset && xoff <= cur_Water->XOffset + cur_Water->TileXLength 
					&& zoff >= cur_Water->ZOffset && zoff <= cur_Water->ZOffset + cur_Water->TileZLength)
				{
					RemoveWaterFromRender(cur_Water);
					ReleaseWater(cur_Water);

					Water*	remove_Water = cur_Water;
					before_Water	= cur_Water;
					cur_Water		= cur_Water->next;

					if(remove_Water == cur_sec->WaterList)
					{
						cur_sec->WaterList = remove_Water->next;
					}
					else
					{
						before_Water->next = remove_Water->next;	
					}

					delete remove_Water;
				}
				else
				{
					before_Water = cur_Water;
					cur_Water = cur_Water->next;
				}
			}
		}

		cur_sec = cur_sec->next;
	}
	return TRUE;
}

void	AgcmWater::MakeWater(Water* pw,INT32 status_id,BOOL bAdd)		
{
	INT32	type = m_stWaterStatus[status_id].WaterType;
	INT32	i;

	if(type == WATER_RIVER || type == WATER_SEA)
	{
		RwFrame			*frame = NULL;

		LockFrame();
		
		pw->AtomicL1 = RpAtomicCreate();
		frame = RwFrameCreate();

		RpAtomicSetFrame(pw->AtomicL1, frame);
		RpMatFXAtomicEnableEffects(pw->AtomicL1);

		pw->pMaterial = RpMaterialCreate();

		RpMaterialSetTexture(pw->pMaterial,m_prWaterT[m_stWaterStatus[pw->StatusID].WaterTexID_L2]);
		
		// matfx set
        RpMatFXMaterialSetEffects(pw->pMaterial, rpMATFXEFFECTDUALUVTRANSFORM);
		
		// 마고자 (2004-10-21 오후 12:09:06) : 
		// 워터 없는 경우에 텍스쳐 없이 진행 
		if( m_prWaterT[m_stWaterStatus[pw->StatusID].WaterTexID_L2] )
		{
			RpMatFXMaterialSetupDualTexture(pw->pMaterial, 
                m_prWaterT[m_stWaterStatus[pw->StatusID].WaterTexID_L2],
				(RwBlendFunction)m_stWaterStatus[status_id].BModeSrc_L2, (RwBlendFunction)m_stWaterStatus[status_id].BModeDest_L2);
		}

        RpMatFXMaterialSetUVTransformMatrices(pw->pMaterial, &m_TexMatL1[status_id] , &m_TexMatL2[status_id]);

		RpGeometry *geometry = NULL;
		geometry = RpGeometryCreate( 4 , 2 , rpGEOMETRYPRELIT  | rpGEOMETRYNORMALS | rpGEOMETRYTEXTURED);
		if( geometry == NULL )
		{
			UnlockFrame();
	        return;
	    }

    	RwSurfaceProperties surfProp;
		surfProp.ambient = 0.3f;
		surfProp.diffuse = 0.7f;
		surfProp.specular = 0.0f;
		RpMaterialSetSurfaceProperties(pw->pMaterial, &surfProp);
		
		RpMorphTarget	*morphTarget = NULL;
		RwV3d			*vlist = NULL, *nlist = NULL;
		RpTriangle		*tlist = NULL;
		RwTexCoords		*texCoord = NULL;
		RwRGBA			*color = NULL;
		
		morphTarget = RpGeometryGetMorphTarget(geometry, 0);

		vlist = RpMorphTargetGetVertices(morphTarget);
		tlist = RpGeometryGetTriangles(geometry);
		nlist = RpMorphTargetGetVertexNormals(morphTarget);
		texCoord = RpGeometryGetVertexTexCoords(geometry, rwTEXTURECOORDINATEINDEX0);
		color = RpGeometryGetPreLightColors (geometry);

		FLOAT	rw = pw->TileXLength * MAP_STEPSIZE;
		FLOAT	rh = pw->TileZLength * MAP_STEPSIZE;

		FLOAT	hw = rw * 0.5f;
		FLOAT	hh = rh * 0.5f;

		RwV3d	temp[4] =	{{   -hw,  0.0f,   -hh },
							{   hw,  0.0f,   -hh },
							{  -hw,  0.0f,   hh },
							{  hw,  0.0f,   hh }};

		for(i=0; i<4;++i)
		{
			vlist->x = temp[i].x; vlist->z = temp[i].z; vlist->y = 0.0f; 
			++vlist;
		}

		RwV3d normal1 = {0.0f, 1.0f, 0.0f};

		*nlist = normal1; ++nlist;
		*nlist = normal1; ++nlist;
		*nlist = normal1; ++nlist;
		*nlist = normal1;

		float	fu1 = (float)pw->XOffset/(float)m_stWaterStatus[status_id].TileSize_L1;
		float	fu2 = ((float)(pw->XOffset+pw->TileXLength))/(float)m_stWaterStatus[status_id].TileSize_L1;
		float	fv1 = (float)pw->ZOffset/(float)m_stWaterStatus[status_id].TileSize_L1;
		float	fv2 = ((float)(pw->ZOffset+pw->TileZLength))/(float)m_stWaterStatus[status_id].TileSize_L1;

		texCoord->u = fu1; texCoord->v = fv1; ++texCoord;
		texCoord->u = fu2 ; texCoord->v = fv1; ++texCoord;
		texCoord->u = fu1; texCoord->v = fv2; ++texCoord;
		texCoord->u = fu2; texCoord->v = fv2; 
		
		for(i=0;i<4;++i)
		{
			color->red = m_stWaterStatus[status_id].Red_L1;
			color->green = m_stWaterStatus[status_id].Green_L1;
			color->blue = m_stWaterStatus[status_id].Blue_L1;
			color->alpha = m_stWaterStatus[status_id].Alpha_L1;
			++color;
		}

		RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, 1, 2);
        RpGeometryTriangleSetMaterial(geometry, tlist++, pw->pMaterial);

		RpGeometryTriangleSetVertexIndices(geometry, tlist, 1, 3, 2);
        RpGeometryTriangleSetMaterial(geometry, tlist++, pw->pMaterial);

		RwSphere boundingSphere;

        RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
        RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);

        RpGeometryUnlock(geometry);
		RpAtomicSetGeometry(pw->AtomicL1, geometry, 0);
		RpGeometryDestroy(geometry);
		RpMaterialDestroy(pw->pMaterial);
		RwFrameTranslate(frame,&pw->pos_origin,rwCOMBINEREPLACE);
		
		// render 모듈 등록
		// nonalpha로 등록시에는 bbox가 필요하다.
		pw->stRenderData.topVerts_MAX[0].x = boundingSphere.center.x - boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[0].y = boundingSphere.center.y + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[0].z = boundingSphere.center.z - boundingSphere.radius;

		pw->stRenderData.topVerts_MAX[1].x = boundingSphere.center.x + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[1].y = boundingSphere.center.y + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[1].z = boundingSphere.center.z - boundingSphere.radius;

		pw->stRenderData.topVerts_MAX[2].x = boundingSphere.center.x + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[2].y = boundingSphere.center.y + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[2].z = boundingSphere.center.z + boundingSphere.radius;
		
		pw->stRenderData.topVerts_MAX[3].x = boundingSphere.center.x - boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[3].y = boundingSphere.center.y + boundingSphere.radius;
		pw->stRenderData.topVerts_MAX[3].z = boundingSphere.center.z + boundingSphere.radius;

		AcuObject::SetAtomicType(pw->AtomicL1,ACUOBJECT_TYPE_NONE | ACUOBJECT_TYPE_NO_INTERSECTION | ACUOBJECT_TYPE_WORLDADD
				,0,NULL,NULL,NULL,NULL,NULL,(PVOID)&pw->stRenderData,NULL);

		if(bAdd) AddWaterToRender(pw);

		TRACE("AgcmWater::MakeWater-%x\n",pw->AtomicL1);
		
		UnlockFrame();
	}
}

BOOL	AgcmWater::MakeWave(Wave* pw,INT32 status_id,BOOL bAdd)
{
	INT32	i;
	int		type = m_stWaterStatus[status_id].WaterType;
	if(type == WATER_RIVER || type == WATER_SEA)
	{
		RwIm3DVertexSetPos(&pw->vert[0], -(FLOAT)pw->Height, 0.0f, -(FLOAT)pw->Width);
		RwIm3DVertexSetPos(&pw->vert[1], (FLOAT)pw->Height, 0.0f, -(FLOAT)pw->Width);
		RwIm3DVertexSetPos(&pw->vert[2], -(FLOAT)pw->Height, 0.0f, (FLOAT)pw->Width);
		RwIm3DVertexSetPos(&pw->vert[3], (FLOAT)pw->Height, 0.0f, -(FLOAT)pw->Width);
		RwIm3DVertexSetPos(&pw->vert[4], (FLOAT)pw->Height, 0.0f, (FLOAT)pw->Width);
		RwIm3DVertexSetPos(&pw->vert[5], -(FLOAT)pw->Height, 0.0f, (FLOAT)pw->Width);
	
		RwIm3DVertexSetU(&pw->vert[0], 0.0f);
		RwIm3DVertexSetU(&pw->vert[1], 1.0f);
		RwIm3DVertexSetU(&pw->vert[2], 0.0f);
		RwIm3DVertexSetU(&pw->vert[3], 1.0f);
		RwIm3DVertexSetU(&pw->vert[4], 1.0f);
		RwIm3DVertexSetU(&pw->vert[5], 0.0f);
				
		RwIm3DVertexSetV(&pw->vert[0], 0.0f);
		RwIm3DVertexSetV(&pw->vert[1], 0.0f);
		RwIm3DVertexSetV(&pw->vert[2], 1.0f);
		RwIm3DVertexSetV(&pw->vert[3], 0.0f);
		RwIm3DVertexSetV(&pw->vert[4], 1.0f);
		RwIm3DVertexSetV(&pw->vert[5], 1.0f);

		for(i= 0;i<6;++i)
		{
			RwIm3DVertexSetNormal(&pw->vert[i], 0.0f, 1.0f, 0.0f);
			RwIm3DVertexSetRGBA(&pw->vert[i], 255, 255, 255, 255);
		}

		if(bAdd) AddWaveToRender(pw);
	}	

	return TRUE;
}

void	AgcmWater::ChangeWater(Water* pL1,INT32 type,INT32 nDetail)
{
	ASSERT(pL1 != NULL);
	
	if(m_stWaterStatus[pL1->StatusID].WaterType == type) return;

	if(type == WATER_RIVER)
	{
		
	}
	else if(type == WATER_SEA)
	{
		
	}
}

// 파일에서 로드한 data로 wave를 set
void	AgcmWater::WaveDataSet(Wave*	wave)
{
	FLOAT		rad = wave->YRotAngle * (FLOAT)AU_DEG2RAD;
	FLOAT		xamount = (FLOAT)cos(rad);
	FLOAT		zamount = -(FLOAT)sin(rad);
			
	wave->MoveAmount.x = xamount *  wave->TranslationSpeed;
	wave->MoveAmount.y = 0.0f;
	wave->MoveAmount.z = zamount *  wave->TranslationSpeed;

	wave->WaveHeight = 0.0f;
	wave->WaveDir = 0;					//	0 - 상향 , 1 - 하향

	wave->UpdateCount = wave->SpawnCount;

	wave->WaveScaleX = 1.0f;
	wave->WaveScaleZ = 1.0f;
	wave->WaveScaleXOffset = m_stWaterStatus[wave->StatusID].WaveScaleX / ((float)m_stWaterStatus[wave->StatusID].WaveLifeTime * 0.5f) ;
	wave->WaveScaleZOffset = m_stWaterStatus[wave->StatusID].WaveScaleZ / ((float)m_stWaterStatus[wave->StatusID].WaveLifeTime * 0.5f);
	
	wave->Alpha = 0.0f;
	wave->AlphaDiff = 1.0f / (m_stWaterStatus[wave->StatusID].WaveLifeTime * 0.6f);
	wave->AlphaDir = 0;

	wave->pos = wave->pos_origin;

	wave->BS.center = wave->pos_origin;
	wave->BS.radius = 100.0f;
}

BOOL	AgcmWater::AddWave(Sector* pSector,FLOAT x,FLOAT z,FLOAT height,FLOAT dir,INT32 Wcount,INT32 status_id)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	INT32					tempVal = -10 - rand() % 20;
	int						wave_tid;

	int						before_SpawnCount = -1;

	for(INT32 i = 0; i<Wcount ; ++i)
	{
		Wave*	nw_Wave = new  Wave;
		nw_Wave->StatusID = status_id;

		//@{ 2006/05/12 burumal
		if ( nw_Wave->StatusID < 0 || nw_Wave->StatusID >= WMAP_STATUS_NUM )
		{
			delete nw_Wave;
			continue;
		}
		//@}

		nw_Wave->XOffset = xoff;
		nw_Wave->ZOffset = zoff;

		wave_tid = m_stWaterStatus[status_id].WaveTexID;
		
		// random 하게 생성
		do
		{
			nw_Wave->SpawnCount = -(rand()%6000 + 2000);
		}while(nw_Wave->SpawnCount - before_SpawnCount > 3000);

		before_SpawnCount = nw_Wave->SpawnCount;

		nw_Wave->Width = m_stWaterStatus[status_id].WaveWidth + rand() % 20 - 8;	// 절반 넓이
		nw_Wave->Height = m_stWaterStatus[status_id].WaveHeight + rand() % 8 - 4;	// 절반 높이
		nw_Wave->TranslationSpeed = (FLOAT)0.1f;
		
		nw_Wave->YRotAngle = dir;

		FLOAT		rad = nw_Wave->YRotAngle * (FLOAT)AU_DEG2RAD;
		FLOAT		xamount = (FLOAT)cos(rad);
		FLOAT		zamount = -(FLOAT)sin(rad);

		nw_Wave->pos_origin.x = x - xamount *  nw_Wave->TranslationSpeed * m_stWaterStatus[status_id].WaveLifeTime * 0.9f;
		nw_Wave->pos_origin.y = height + 9.0f;
		nw_Wave->pos_origin.z = z - zamount *  nw_Wave->TranslationSpeed * m_stWaterStatus[status_id].WaveLifeTime * 0.9f;	

		WaveDataSet(nw_Wave);
		
		MakeWave(nw_Wave,status_id);

		nw_Wave->next = pSector->WaveList;
		pSector->WaveList = nw_Wave;
	}

	return TRUE;
}

// 자동 파도 생성 ~~~
void	AgcmWater::AutoMakeWave(Sector* pSector)//,INT32 six,INT32 siz)
{
	if (!m_pcsAgcmMap)
		return;

	// 파도 클리어
	Wave*		cur_Wave = pSector->WaveList;
	Wave*		remove_Wave = NULL;
	while(cur_Wave)
	{
		remove_Wave = cur_Wave;
		cur_Wave = cur_Wave->next;

		RemoveWaveFromRender(remove_Wave);
		ReleaseWave(remove_Wave);

		delete remove_Wave;	
	}
	pSector->WaveList = NULL;

	int						status_id = 0;
	Water*	cur_Water = pSector->WaterList;
	
	INT32	xstart = 0;
	INT32	xend = 0;
	INT32	zstart = 0;
	INT32	zend = 0;
	INT32	i = 0;
	INT32	j = 0;
	FLOAT	f1x = 0.0f;
	FLOAT	f2x = 0.0f;
	FLOAT	f1y = 0.0f;
	FLOAT	f2y = 0.0f;
	FLOAT	oX = 0.0f;
	FLOAT	oZ = 0.0f;
	FLOAT	hsz = 0.0f;
	FLOAT	bX = 0.0f;
	FLOAT	bZ = 0.0f;
	FLOAT	FindStep = 0.0f;
	INT32	FindMode = 0,FindNum = 0;

	while(cur_Water)
	{
		xstart = cur_Water->XOffset;
		xend = xstart + cur_Water->TileXLength;
		zstart = cur_Water->ZOffset;
		zend = zstart + cur_Water->TileZLength;

		for(i=zstart;i<zend;++i)
		{
			for(j=xstart;j<xend;++j)
			{
				oX = GetSectorStartX(pSector->SectorXIndex) + j * MAP_STEPSIZE;
				oZ = GetSectorStartZ(pSector->SectorZIndex) + i * MAP_STEPSIZE;

				hsz = MAP_STEPSIZE/2;
		
				bX = oX + hsz;
				bZ = oZ + hsz;

				FindMode = 0;			//  0 - 물부터 시작, 1 - 지면부터 시작 
				FindNum = 0;
				FindStep = 5.0f;

				if(m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+MAP_STEPSIZE) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+hsz,oZ) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+hsz) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+MAP_STEPSIZE) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+hsz,oZ+MAP_STEPSIZE) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+MAP_STEPSIZE) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+hsz) < cur_Water->Height ||
					m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+hsz,oZ+hsz) < cur_Water->Height)
				{
					status_id = cur_Water->StatusID;
				}
				else 
					continue;		//  파도 안만듬
				
				if(m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ) > cur_Water->Height)
				{
					 FindMode = 1;
				}

				FLOAT k;

				// 상단 체크
				for(k = 0.0f;k<MAP_STEPSIZE; k += FindStep)
				{
					if(FindMode == 0)
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ) > cur_Water->Height)
						{
							++FindNum;
							f1x = k;
							f1y = 0.0f;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ) < cur_Water->Height)
						{
							++FindNum;
							f2x = k;
							f2y = 0.0f;
							break;
						}
					}
					else
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ) < cur_Water->Height)
						{
							++FindNum;
							f1x = k;
							f1y = 0.0f;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ) > cur_Water->Height)
						{
							++FindNum;
							f2x = k;
							f2y = 0.0f;
							break;
						}
					}
				}

				//  우측 체크
				for(k = 0.0f;k<MAP_STEPSIZE; k+= FindStep)
				{
					if(FindMode == 0)
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+k) > cur_Water->Height)
						{
							++FindNum;
							f1x = MAP_STEPSIZE;
							f1y = k;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+k) < cur_Water->Height)
						{
							++FindNum;
							f2x = MAP_STEPSIZE;
							f2y = k;
							break;
						}
					}
					else
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+k) < cur_Water->Height)
						{
							++FindNum;
							f1x = MAP_STEPSIZE;
							f1y = k;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+MAP_STEPSIZE,oZ+k) > cur_Water->Height)
						{
							++FindNum;
							f2x = MAP_STEPSIZE;
							f2y = k;
							break;
						}
					}
				}

				//  하단 체크
				for(k = MAP_STEPSIZE;k>0.0f; k-= FindStep)
				{
					if(FindMode == 0)
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ+MAP_STEPSIZE) > cur_Water->Height)
						{
							++FindNum;
							f1x = k;
							f1y = MAP_STEPSIZE;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ+MAP_STEPSIZE) < cur_Water->Height)
						{
							++FindNum;
							f2x = k;
							f2y = MAP_STEPSIZE;
							break;
						}
					}
					else
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ+MAP_STEPSIZE) < cur_Water->Height)
						{
							++FindNum;
							f1x = k;
							f1y = MAP_STEPSIZE;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX+k,oZ+MAP_STEPSIZE) > cur_Water->Height)
						{
							++FindNum;
							f2x = k;
							f2y = MAP_STEPSIZE;
							break;
						}
					}
				}

				//  좌측 체크
				for(k = MAP_STEPSIZE;k>0.0f; k-= FindStep)
				{
					if(FindMode == 0)
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+k) > cur_Water->Height)
						{
							++FindNum;
							f1x = 0.0f;
							f1y = k;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+k) < cur_Water->Height)
						{
							++FindNum;
							f2x = 0.0f;
							f2y = k;
							break;
						}
					}
					else
					{
						if(FindNum == 0 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+k) < cur_Water->Height)
						{
							++FindNum;
							f1x = 0.0f;
							f1y = k;
						}
						else if(FindNum == 1 && m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(oX,oZ+k) > cur_Water->Height)
						{
							++FindNum;
							f2x = 0.0f;
							f2y = k;
							break;
						}
					}
				}

				if(FindNum == 2)
				{
					FLOAT	wx = f1x - f2x;
					FLOAT	wy = f1y - f2y;
					
					FLOAT	cx = (f1x + f2x) * 0.5f - MAP_STEPSIZE/2;
					FLOAT	cy = (f1y + f2y) * 0.5f - MAP_STEPSIZE/2;

					FLOAT	deg = (FLOAT)atan(wx/wy) * (FLOAT)AU_RAD2DEG;

					FLOAT	dist = (FLOAT)sqrt(wx*wx + wy*wy);

					FLOAT		rad = deg * (FLOAT)AU_DEG2RAD;
					FLOAT		xamount = (FLOAT)cos(rad);
					FLOAT		zamount = -(FLOAT)sin(rad);

					FLOAT mx = xamount *  MAP_STEPSIZE * 0.2f;
					FLOAT mz = zamount *  MAP_STEPSIZE * 0.2f;

					INT32	wavemakenum = m_stWaterStatus[status_id].WaveMinNum +
						rand() % (m_stWaterStatus[status_id].WaveMaxNum - m_stWaterStatus[status_id].WaveMinNum + 1);

					if(m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(bX+cx+mx,bZ+cy+mz) < cur_Water->Height)
					{
						deg -= 180.0f;
						AddWave(pSector,bX+cx+mx,bZ+cy+mz,cur_Water->Height,deg,wavemakenum,status_id);
					}
					else
					{
						AddWave(pSector,bX+cx-mx,bZ+cy-mz,cur_Water->Height,deg,wavemakenum,status_id);
					}
				}
			}
		}

		cur_Water = cur_Water->next;
	}
}

BOOL	AgcmWater::DeleteWave(FLOAT x,FLOAT z)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	Sector*		cur_sec = m_listSector;
	Wave*	cur_Wave = NULL;
	Wave*	before_Wave = NULL;
	Wave*	remove_Wave = NULL;

	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			cur_Wave = cur_sec->WaveList;
			while( cur_Wave )
			{
				remove_Wave = NULL;

				if(cur_Wave->XOffset == xoff && cur_Wave->ZOffset == zoff)
				{
					remove_Wave = cur_Wave;
					cur_Wave = cur_Wave->next;

					RemoveWaveFromRender(remove_Wave);
					ReleaseWave(remove_Wave);

					if(remove_Wave == cur_sec->WaveList)
					{
						cur_sec->WaveList = remove_Wave->next;
					}
					else
					{
						before_Wave->next = remove_Wave->next;
					}
					delete	remove_Wave;
				}
				else
				{
					before_Wave = cur_Wave;
					cur_Wave = cur_Wave->next;
				}
			}
		}
		cur_sec = cur_sec->next;
	}

	return FALSE;
}

Water*	AgcmWater::GetWater(FLOAT x,FLOAT z)
{
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	Sector*		cur_sec = m_listSector;
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			Water*	cur_Water = cur_sec->WaterList;
			while(cur_Water)
			{
				if(xoff >= cur_Water->XOffset && xoff <= cur_Water->XOffset + cur_Water->TileXLength 
					&& zoff >= cur_Water->ZOffset && zoff <= cur_Water->ZOffset + cur_Water->TileZLength)
				{
					return cur_Water;
				}
				
				cur_Water = cur_Water->next;
			}
		}
		
		cur_sec = cur_sec->next;
	}
	return NULL;
}

INT32	AgcmWater::GetWaterType(FLOAT x,FLOAT z)
{
	Water* pL1 = GetWater(x,z);

	if(!pL1)
	{
		return m_stWaterStatus[pL1->StatusID].WaterType ;
	}
	else 	return -1;
}

BOOL	AgcmWater::GetWaterHeight( FLOAT x , FLOAT z , FLOAT * pHeight )
{
	// 마고자 (2003-06-09 오후 9:56:42) : BOOL타입 리턴으로 변경.

	ASSERT( NULL != pHeight );
	if( NULL == pHeight ) return FALSE;

	Water* pL1 = GetWater(x,z);

	if(pL1)
	{
		*pHeight	= pL1->Height;
		return TRUE;	// found
	}
	else
	{
		return FALSE;	// not found
	}
}

int		AgcmWater::GetWaterStatus(FLOAT x,FLOAT z)
{
	Water* pL1 = GetWater(x,z);

	if(pL1)
	{
		return pL1->StatusID;
	}
	else
	{
		return -1;	// not found
	}
}

void	AgcmWater::SaveToFiles(char*	szDir)
{
	if (!m_pcsAgcmMap)
		return;

	INT32	startx = MAP_WORLD_INDEX_WIDTH/2;
	INT32	startz = MAP_WORLD_INDEX_HEIGHT/2;

	INT32	endx = -MAP_WORLD_INDEX_WIDTH/2;
	INT32	endz = -MAP_WORLD_INDEX_HEIGHT/2;

	Sector*		cur_sec = m_listSector;
	bool					bfind = false;
		
	int		range_x1  = m_pcsAgcmMap->GetLoadRangeX1() - startx;
	int		range_x2  = m_pcsAgcmMap->GetLoadRangeX2() - startx;
	int		range_z1  = m_pcsAgcmMap->GetLoadRangeZ1() - startz;
	int		range_z2  = m_pcsAgcmMap->GetLoadRangeZ2() - startz;

	while(cur_sec)
	{
		if(cur_sec->WaterList == NULL ||
			cur_sec->SectorXIndex < range_x1 ||
			cur_sec->SectorXIndex >= range_x2 ||
			cur_sec->SectorZIndex < range_z1 ||
			cur_sec->SectorZIndex >= range_z2 )
		{
			cur_sec = cur_sec->next;
			continue;
		}

		bfind = true;
		
		if(cur_sec->SectorXIndex < startx)
			startx = cur_sec->SectorXIndex;

		if(cur_sec->SectorXIndex > endx)
			endx = cur_sec->SectorXIndex;

		if(cur_sec->SectorZIndex < startz)
			startz = cur_sec->SectorZIndex;

		if(cur_sec->SectorZIndex > endz)
			endz = cur_sec->SectorZIndex;

		cur_sec = cur_sec->next;
	}

	if(bfind == false) return;

	INT32 lstartx = startx / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	INT32 lstartz = startz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;
	
	if(startx < 0 && startx % MAP_DEFAULT_DEPTH != 0) lstartx -= MAP_DEFAULT_DEPTH;
	if(startz < 0 && startz % MAP_DEFAULT_DEPTH != 0) lstartz -= MAP_DEFAULT_DEPTH;

	INT32 lendx = endx / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	INT32 lendz = endz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH ;
	
	if(endx < 0 && endx % MAP_DEFAULT_DEPTH != 0) lendx -= MAP_DEFAULT_DEPTH;
	if(endz < 0 && endz % MAP_DEFAULT_DEPTH != 0) lendz -= MAP_DEFAULT_DEPTH;

	INT32	x_arrayindex = 0;
	INT32	z_arrayindex = 0;
	INT32	division_index = 0;
	
	for(INT32	i = lstartz; i <= lendz; i += MAP_DEFAULT_DEPTH)
	{
	  for(INT32	j = lstartx; j <= lendx; j+= MAP_DEFAULT_DEPTH)
	  {
        char		fstr[100];
		memset(fstr,'\0',100);

		x_arrayindex = SectorIndexToArrayIndexX(j);
		z_arrayindex = SectorIndexToArrayIndexZ(i);
		division_index = GetDivisionIndex( x_arrayindex , z_arrayindex );
		if(!szDir)
		{
			#ifdef USE_MFC
			sprintf(fstr,"map\\data\\water\\wt%d.dat",division_index);
			#else
			sprintf(fstr,"world\\water\\wt%d.dat",division_index);
			#endif
		}
		else
		{
			sprintf(fstr,"%s\\water\\wt%d.dat",szDir,division_index);
		}

//		CloseHandleList(j,i);			// 미리 열어둔 handle close(client용 이다.^^)

		HANDLE fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(fd == INVALID_HANDLE_VALUE)
		{
			if(!szDir)
			{
				char	fdirc[100];
				memset(fdirc,'\0',100);
				 
				sprintf(fdirc,"world\\water");
				CreateDirectory(fdirc,NULL);
			}
			else
			{
				CreateDirectory(szDir,NULL);
			}

			fd=CreateFile(fstr,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		}
		
		DWORD FP = 0;

		// Version History
		// 1 - fileoffest미리 계산해서 저장
		// 2 - wave의 SpawnCount 가 4-10초 이던것이 2-8초로 줄어듬
		
		DWORD	version = 2;
		WriteFile(fd,&version,sizeof(version),&FP,NULL);

		INT32	foffset = MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH * 8 + 4;	// version정보 추가
		INT32	fsize = 0;

		for(INT32	k = 0; k<MAP_DEFAULT_DEPTH ; ++k)
		{
		  for(INT32	l = 0; l<MAP_DEFAULT_DEPTH; ++l)
		  {
			SetFilePointer(fd,foffset,NULL,FILE_BEGIN);

			BOOL					bfind = FALSE;
			Sector*					cur_sec = m_listSector;
		 	while( cur_sec )
			{
			  if(cur_sec->SectorXIndex == j+l && cur_sec->SectorZIndex == i+k)
			  {
				  bfind = TRUE;			// 발견^^
				  
				  INT32 Watercount = 0;
				  INT32 Wavecount = 0;

				  Water*		tempWater = cur_sec->WaterList;
				  while(tempWater)
				  {
					++Watercount;
					tempWater = tempWater->next;
				  }

				  Wave*		tempWave = cur_sec->WaveList;
				  while(tempWave)
				  {
					 ++Wavecount;
					 tempWave = tempWave->next;
				  }

				  // sector datas 저장
				  WriteFile(fd,&Watercount,sizeof(INT32),&FP,NULL);			// L1 개수

				  // L1 저장
				  Water*			cur_Water = cur_sec->WaterList;
	              while( cur_Water )
				  {
					WriteFile(fd,&cur_Water->StatusID,sizeof(INT32),&FP,NULL);
					WriteFile(fd,&cur_Water->XOffset,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Water->ZOffset,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Water->TileXLength,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Water->TileZLength,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Water->Height,sizeof(INT32),&FP,NULL);
					
					cur_Water = cur_Water->next;
				  }

				  WriteFile(fd,&Wavecount,sizeof(INT32),&FP,NULL);			// L2 개수

				  // L2 저장 
				  Wave*			cur_Wave = cur_sec->WaveList;
				  while( cur_Wave )
				  {
					WriteFile(fd,&cur_Wave->StatusID,sizeof(INT32),&FP,NULL);
					WriteFile(fd,&cur_Wave->XOffset,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Wave->ZOffset,sizeof(INT16),&FP,NULL);
					WriteFile(fd,&cur_Wave->Width,sizeof(INT32),&FP,NULL);
					WriteFile(fd,&cur_Wave->Height,sizeof(INT32),&FP,NULL);
					WriteFile(fd,&cur_Wave->SpawnCount,sizeof(INT32),&FP,NULL);
					WriteFile(fd,&cur_Wave->YRotAngle,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&cur_Wave->TranslationSpeed,sizeof(FLOAT),&FP,NULL);
					WriteFile(fd,&cur_Wave->pos_origin,sizeof(RwV3d),&FP,NULL);
					
					cur_Wave = cur_Wave->next;
				  }

				  // file 시작위치 저장
				  SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
				  WriteFile(fd,&foffset,sizeof(INT32),&FP,NULL);
				  fsize = Watercount * WMAP_WATER_BYTE_SIZE + Wavecount * WMAP_WAVE_BYTE_SIZE + 8;//8 - 개수저장
				  WriteFile(fd,&fsize,sizeof(INT32),&FP,NULL);

				  foffset += fsize; 
		  
				  break;
			  }

			  cur_sec = cur_sec->next;
			}

			if(bfind == FALSE)
			{
				INT32		temp = 0;
				WriteFile(fd,&temp,sizeof(INT32),&FP,NULL);
				WriteFile(fd,&temp,sizeof(INT32),&FP,NULL);

				// file 시작위치 저장
			    SetFilePointer(fd,k*8*MAP_DEFAULT_DEPTH  + l*8 + 4,NULL,FILE_BEGIN);
			    WriteFile(fd,&foffset,sizeof(INT32),&FP,NULL);
				fsize = 8;														// 개수저장만 했음
				WriteFile(fd,&fsize,sizeof(INT32),&FP,NULL);

				foffset += fsize;	
			}
		  }
		}
		
		CloseHandle(fd);
	  }
	}
}

BOOL	AgcmWater::RemoveWaterRange( FLOAT x, FLOAT z , FLOAT xlength , FLOAT zlength )
{
	// x , z , xlength , zlength 범위 내에 있는 워터 삭제.
	// 살짝 걸린 녀석도 삭제한다.
	INT32	six = PosToSectorIndexX(x);
	INT32	siz = PosToSectorIndexZ(z);
	INT32	xoff = (INT32)(( x - GetSectorStartX( six )) / MAP_STEPSIZE);
	INT32	zoff = (INT32)(( z - GetSectorStartZ( siz )) / MAP_STEPSIZE);

	FLOAT   x2 = x+xlength;
	FLOAT	z2 = z+zlength;
	INT32	six2 = PosToSectorIndexX(x2);
	INT32	siz2 = PosToSectorIndexX(z2);
	INT32	xoff2 = (INT32)(( x2 + 1 - GetSectorStartX( six2 )) / MAP_STEPSIZE);
	INT32	zoff2 = (INT32)(( z2 + 1 - GetSectorStartZ( siz2 )) / MAP_STEPSIZE);

	// sector별로 쪼개기 작업
	if(six2 > six && siz2 > siz)
	{
		xlength = GetSectorStartX(six+1) - x - 1;
		zlength = GetSectorStartX(siz+1) - z - 1;

		FLOAT	tx1 = GetSectorStartX(six+1);
		FLOAT	tz1 = GetSectorStartZ(siz+1);
		FLOAT	txl1 = x2 - tx1;
		FLOAT	tzl1 = z2 - tz1;
		
		RemoveWaterRange(x,z,xlength,zlength);
		RemoveWaterRange(tx1,z,txl1,zlength);
		RemoveWaterRange(x,tz1,xlength,tzl1);
		RemoveWaterRange(tx1,tz1,txl1,tzl1);
		return TRUE;
	}
	else if(six2 > six)
	{
		xlength = GetSectorStartX(six+1) - x - 1;
 
		FLOAT	tx1 = GetSectorStartX(six+1);
		FLOAT	txl1 = x2 - tx1;
		
		RemoveWaterRange(x,z,xlength,zlength);
		RemoveWaterRange(tx1,z,txl1,zlength);		// 자신의 sector영역이 아니면 재귀호출
		return TRUE;
	}
	else if(siz2 > siz)
	{
		zlength = GetSectorStartX(siz+1) - z - 1;

		FLOAT	tz1 = GetSectorStartZ(siz+1);
		FLOAT	tzl1 = z2 - tz1;
		
		RemoveWaterRange(x,z,xlength,zlength);
		RemoveWaterRange(x,tz1,xlength,tzl1);		// 자신의 sector영역이 아니면 재귀호출
		return TRUE;
	}

	Sector*		cur_sec = m_listSector;
	Water*		cur_Water = NULL;
	Water*		before_Water = NULL;

	HWater*		cur_HWater = NULL;
	HWater*		before_HWater = NULL;

	while( cur_sec )
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			// 기존의 L1과 중복부분이 있으면 최대영역으로 update
			BOOL					bOverlap = FALSE;

			FLOAT					minx = x;
			FLOAT					minz = z;

			FLOAT					maxXlength = xlength;
			FLOAT					maxZlength = zlength;

			FLOAT					maxx = x + xlength;
			FLOAT					maxz = z + zlength;

			cur_Water = cur_sec->WaterList;
			while( cur_Water )
			{
				FLOAT	lwx = GetSectorStartX( six ) + cur_Water->XOffset * MAP_STEPSIZE;
				FLOAT	lwz = GetSectorStartZ( siz ) + cur_Water->ZOffset * MAP_STEPSIZE;

				FLOAT	lwxend = lwx + cur_Water->TileXLength * MAP_STEPSIZE;
				FLOAT	lwzend = lwz + cur_Water->TileZLength * MAP_STEPSIZE;

				if(minx < lwxend && maxx > lwx && minz < lwzend && maxz > lwz)
				{	
					RemoveWaterFromRender(cur_Water);
					ReleaseWater(cur_Water);

					if(cur_Water == cur_sec->WaterList)
					{
						cur_sec->WaterList = cur_Water->next;
					}
					else
					{
						before_Water->next = cur_Water->next;
					}

					Water*	remove_Water = cur_Water;
					cur_Water = cur_Water->next;

					delete remove_Water;
				}
				else
				{
					before_Water = cur_Water;
					cur_Water = cur_Water->next;
				}
			}

			if(!cur_sec->WaterList) cur_sec->AddedRenderWater = false;

			cur_HWater = cur_sec->HWaterList;
			while(cur_HWater)
			{
				FLOAT	lwx = GetSectorStartX( six ) + cur_HWater->XOffset * MAP_STEPSIZE;
				FLOAT	lwz = GetSectorStartZ( siz ) + cur_HWater->ZOffset * MAP_STEPSIZE;

				FLOAT	lwxend = lwx + cur_HWater->TileXLength * MAP_STEPSIZE;
				FLOAT	lwzend = lwz + cur_HWater->TileZLength * MAP_STEPSIZE;

				if(minx < lwxend && maxx > lwx && minz < lwzend && maxz > lwz)
				{	
					RemoveHWaterFromRender(cur_HWater);
					ReleaseHWater(cur_HWater);

					if(cur_HWater == cur_sec->HWaterList)
					{
						cur_sec->HWaterList = cur_HWater->next;
					}
					else
					{
						before_HWater->next = cur_HWater->next;
					}

					HWater*	remove_HWater = cur_HWater;
					cur_HWater = cur_HWater->next;

					delete remove_HWater;
				}
				else
				{
					before_HWater = cur_HWater;
					cur_HWater = cur_HWater->next;
				}
			}

			if(!cur_sec->HWaterList) cur_sec->AddedRenderHWater = false;

			AutoMakeWave(cur_sec);
			
			return TRUE;
		}
		
		cur_sec = cur_sec->next;
	}
	return FALSE;
}

void	AgcmWater::ChangeStatus(int status_id)
{
	INT32	i;

	// file추가 검사
	for(i=0;i<m_iWaterFileNum;++i)
	{
		if(m_stWaterFInfo[i].bReload) // file load
		{
			m_stWaterFInfo[i].bReload = FALSE;

			if(m_prWaterT[i])
			{
				RwTextureDestroy(m_prWaterT[i]);
				m_prWaterT[i] = NULL;
			}

			m_prWaterT[i] = RwTextureRead(m_stWaterFInfo[i].strFileName, NULL);
			ASSERT( NULL != m_prWaterT[i] && "워터 텍스쳐가 없습니다!" );
			RwTextureSetFilterMode(m_prWaterT[i], rwFILTERLINEAR);
		}
	}

	for(i=0;i<m_iWaveFileNum;++i)
	{
		if(m_stWaveFInfo[i].bReload) // file 추가
		{
			m_stWaveFInfo[i].bReload = FALSE;

			if(m_prWaveT[i])
			{
				RwTextureDestroy(m_prWaveT[i]);
				m_prWaveT[i] = NULL;
			}

			m_prWaveT[i] = RwTextureRead(m_stWaveFInfo[i].strFileName, NULL);
			ASSERT( NULL != m_prWaveT[i] && "워터 텍스쳐가 없습니다!" );
			RwTextureSetFilterMode(m_prWaveT[i], rwFILTERLINEAR);
		}
	}

	Sector*		cur_sec = m_listSector;
	int			water_tid1 = 0;
	int			water_tid2 = 0;
	int			wave_tid = 0;

	water_tid1 = m_stWaterStatus[status_id].WaterTexID_L1;
	water_tid2 = m_stWaterStatus[status_id].WaterTexID_L2;
	wave_tid = m_stWaterStatus[status_id].WaveTexID;

	Water*	cur_Water = NULL;
	Wave*	cur_Wave = NULL;

	while( cur_sec )
	{
		cur_Water = cur_sec->WaterList;
		while(cur_Water)
		{
			if(cur_Water->StatusID == status_id)		// atomic재설정
			{
				RpMaterialSetTexture  (  cur_Water->pMaterial ,  m_prWaterT[water_tid1]);

				RpMatFXMaterialSetupDualTexture(cur_Water->pMaterial, m_prWaterT[water_tid2],
				(RwBlendFunction)m_stWaterStatus[status_id].BModeSrc_L2, (RwBlendFunction)m_stWaterStatus[status_id].BModeDest_L2);
				
				RpGeometry*		pGeom = RpAtomicGetGeometry( cur_Water->AtomicL1 );

				RpGeometryLock(pGeom,rpGEOMETRYLOCKPRELIGHT |rpGEOMETRYLOCKTEXCOORDS);
				RwRGBA*			color = RpGeometryGetPreLightColors(pGeom);

				for(i=0;i<4;++i)
				{
					color->red = m_stWaterStatus[status_id].Red_L1;
					color->green = m_stWaterStatus[status_id].Green_L1;
					color->blue = m_stWaterStatus[status_id].Blue_L1;
					color->alpha = m_stWaterStatus[status_id].Alpha_L1;

					++color;
				}

				RwTexCoords		*texCoord = RpGeometryGetVertexTexCoords(pGeom, rwTEXTURECOORDINATEINDEX0);

				texCoord->u = 0.0f; texCoord->v = 0.0f; ++texCoord;
				texCoord->u = (float)cur_Water->TileXLength / (float)m_stWaterStatus[status_id].TileSize_L1 ; texCoord->v = 0.0f; ++texCoord;
				texCoord->u = 0.0f; texCoord->v = cur_Water->TileZLength / (float)m_stWaterStatus[status_id].TileSize_L1; ++texCoord;
				texCoord->u = cur_Water->TileXLength / (float)m_stWaterStatus[status_id].TileSize_L1; texCoord->v = cur_Water->TileZLength / (float)m_stWaterStatus[status_id].TileSize_L1; 

				RpGeometryUnlock( pGeom );

				// UD Blend Mode 와 BOP 재설정
				if(m_stWaterStatus[status_id].RenderBMode_L1 == R_NONE)
				{
					cur_Water->AtomicL1->stRenderInfo.renderType = R_NONALPHA;
					cur_Water->AtomicL1->stRenderInfo.blendMode = R_BLENDSRCAINVA_ADD;
										
					RpAtomicSetRenderCallBack(cur_Water->AtomicL1, AgcmRender::RenderCallbackForNonAlphaAtomic);
				}
				else if(m_stWaterStatus[status_id].RenderBMode_L1 < R_BLEND11_ADD)
				{
					cur_Water->AtomicL1->stRenderInfo.renderType = R_BLEND_SORT;
					cur_Water->AtomicL1->stRenderInfo.blendMode = m_stWaterStatus[status_id].RenderBMode_L1;
					RpAtomicSetRenderCallBack(cur_Water->AtomicL1, AgcmRender::RenderCallbackForBlendAtomic);
				}
				else
				{
					cur_Water->AtomicL1->stRenderInfo.renderType = R_BLEND_NSORT;
					cur_Water->AtomicL1->stRenderInfo.blendMode = m_stWaterStatus[status_id].RenderBMode_L1;

					RpAtomicSetRenderCallBack(cur_Water->AtomicL1, AgcmRender::RenderCallbackForBlendAtomicNotSort);
				}

				//@{ Jaewon 20050602
				// Just use atomic->renderCallBack.
				//m_pcsAgcmRender->UpdateRenderCallBack(cur_Water->AtomicL1,NULL);
				//@} Jaewon

				m_TexuvL1[status_id].x = m_stWaterStatus[status_id].U_L1;
				m_TexuvL1[status_id].y = m_stWaterStatus[status_id].V_L1;
				m_TexuvL2[status_id].x = m_stWaterStatus[status_id].U_L2;
				m_TexuvL2[status_id].y = m_stWaterStatus[status_id].V_L2;
			}

			cur_Water = cur_Water->next;
		}

		cur_Wave = cur_sec->WaveList;
		while(cur_Wave)
		{
			if(cur_Wave->StatusID == status_id)
			{
				cur_Wave->Width = m_stWaterStatus[status_id].WaveWidth;
				cur_Wave->Height = m_stWaterStatus[status_id].WaveHeight;

				RwIm3DVertexSetPos(&cur_Wave->vert[0], -(FLOAT)cur_Wave->Height, 0.0f, -(FLOAT)cur_Wave->Width);
				RwIm3DVertexSetPos(&cur_Wave->vert[1], (FLOAT)cur_Wave->Height, 0.0f, -(FLOAT)cur_Wave->Width);
				RwIm3DVertexSetPos(&cur_Wave->vert[2], -(FLOAT)cur_Wave->Height, 0.0f, (FLOAT)cur_Wave->Width);
				RwIm3DVertexSetPos(&cur_Wave->vert[3], (FLOAT)cur_Wave->Height, 0.0f, -(FLOAT)cur_Wave->Width);
				RwIm3DVertexSetPos(&cur_Wave->vert[4], (FLOAT)cur_Wave->Height, 0.0f, (FLOAT)cur_Wave->Width);
				RwIm3DVertexSetPos(&cur_Wave->vert[5], -(FLOAT)cur_Wave->Height, 0.0f, (FLOAT)cur_Wave->Width);

				cur_Wave->WaveScaleX = 1.0f;
				cur_Wave->WaveScaleZ = 1.0f;

				cur_Wave->WaveScaleXOffset = m_stWaterStatus[cur_Wave->StatusID].WaveScaleX / ((float)m_stWaterStatus[status_id].WaveLifeTime * 0.5f) ;
				cur_Wave->WaveScaleZOffset = m_stWaterStatus[cur_Wave->StatusID].WaveScaleZ / ((float)m_stWaterStatus[status_id].WaveLifeTime * 0.5f);

				cur_Wave->Alpha = 0.0f;
				cur_Wave->AlphaDiff = 1.0f / (m_stWaterStatus[cur_Wave->StatusID].WaveLifeTime * 0.7f);
				cur_Wave->AlphaDir = 0;
			}

			cur_Wave = cur_Wave->next;
		}

		cur_sec = cur_sec->next;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	High Quality Water	Functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PVOID AgcmWater::GetShader(const char* pSrc, BOOL bPixel) const
{
	PVOID ret = NULL;
	HRESULT hresult;
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsgs = NULL;
	
	hresult = D3DXAssembleShader(pSrc, strlen(pSrc), 
								NULL, NULL, 0,
								&pShader, &pErrorMsgs);
	
	// create the shader.
	if(SUCCEEDED(hresult))
	{
		RwBool hr;
		if(bPixel)
			hr = RwD3D9CreatePixelShader((const RwUInt32 *)(pShader->GetBufferPointer()), 
										&ret);
		else
			hr = RwD3D9CreateVertexShader((const RwUInt32 *)(pShader->GetBufferPointer()), 
										&ret);
		if(!hr)
			ret = NULL;
	}
	else if (pErrorMsgs)
	{
		const char* pLog = (const char*)pErrorMsgs->GetBufferPointer();
		OutputDebugString(pLog);
	}
	
	if(pErrorMsgs)
	{
		pErrorMsgs->Release();
	}
	if(pShader)
	{
		pShader->Release();
	}

	return ret;
}

// bump src texture를 노말맵으로 변환
BOOL	AgcmWater::InitBumpMaps()
{
	INT32	i;

	for(i=0;i<m_iHQBumpFileNum;++i)
	{
		if(!m_prHQBumpSrc[i])
		{
			ASSERT(0);
			m_bEnableHWater = FALSE;
			return FALSE;
		}

		RwRaster*	pRas = RwTextureGetRaster(m_prHQBumpSrc[i]);

		if(!pRas)
		{
			ASSERT(0);
			m_bEnableHWater = FALSE;
			return FALSE;
		}

		int		SrcWidth = RwRasterGetWidth(pRas);
		int		SrcHeight = RwRasterGetHeight(pRas);

		if( FAILED( m_pCurD3D9Device->CreateTexture( SrcWidth, SrcHeight, 0, D3DUSAGE_AUTOGENMIPMAP, 
		D3DFMT_V8U8, D3DPOOL_MANAGED, &m_prHQBump[i] ,NULL) ) )
		{
			ASSERT(0);
			m_bEnableHWater = FALSE;
			return FALSE;
		}

		BYTE*	pSrcTopRow = (BYTE*) RwRasterLock(pRas,0,rwRASTERLOCKREAD);
		int		SrcPitch = RwRasterGetStride(pRas);
		BYTE*	pSrcCurRow = pSrcTopRow;
		BYTE*	pSrcBotRow = pSrcTopRow + (SrcPitch * (SrcHeight - 1) );

		D3DLOCKED_RECT     d3dlr;
		m_prHQBump[i]->LockRect( 0, &d3dlr, 0, 0 );
		DWORD DstPitch = (DWORD)d3dlr.Pitch;
		BYTE* pDstTopRow = (BYTE*)d3dlr.pBits;
		BYTE* pDstCurRow = pDstTopRow;
		BYTE* pDstBotRow = pDstTopRow + (DstPitch * (SrcHeight - 1) );

		for( DWORD y=0; y<( DWORD ) SrcHeight; ++y )
		{
			BYTE* pSrcB0 = NULL; // addr of current pixel
			BYTE* pSrcB1 = NULL; // addr of pixel below current pixel, wrapping to top if necessary
			BYTE* pSrcB2 = NULL; // addr of pixel above current pixel, wrapping to bottom if necessary
			BYTE* pDstT = NULL;  // addr of dest pixel;

			pSrcB0 = pSrcCurRow;

			if( y == SrcHeight - 1)
				pSrcB1 = pSrcTopRow;
			else
				pSrcB1 = pSrcCurRow + SrcPitch;

			if( y == 0 )
				pSrcB2 = pSrcBotRow;
			else
				pSrcB2 = pSrcCurRow - SrcPitch;

			pDstT = pDstCurRow;

			for( DWORD x=0; x<( DWORD ) SrcWidth; x++ )
			{
				LONG v00 = 0; // Current pixel
				LONG v01 = 0; // Pixel to the right of current pixel, wrapping to left edge if necessary
				LONG vM1 = 0; // Pixel to the left of current pixel, wrapping to right edge if necessary
				LONG v10 = 0; // Pixel one line below.
				LONG v1M = 0; // Pixel one line above.

				v00 = *(pSrcB0+0);
        
				if( x == SrcWidth - 1 )
					v01 = *(pSrcCurRow);
				else
					v01 = *(pSrcB0+4);
        
				if( x == 0 )
					vM1 = *(pSrcCurRow + (4 * (SrcWidth - 1)));
				else
					vM1 = *(pSrcB0-4);
				v10 = *(pSrcB1+0);
				v1M = *(pSrcB2+0);

				LONG iDu = (vM1-v01); // The delta-u bump value
				LONG iDv = (v1M-v10); // The delta-v bump value
    
				*pDstT++ = (BYTE)(iDu );
				*pDstT++ = (BYTE)(iDv );
               
				// Move one pixel to the right (src is 32-bpp)
				pSrcB0+=4;
				pSrcB1+=4;
				pSrcB2+=4;
			}

			// Move to the next line
			pSrcCurRow += SrcPitch;
			pDstCurRow += DstPitch;
		}

		m_prHQBump[i]->UnlockRect(0);
		RwRasterUnlock(pRas);

		m_prHQBump[i]->GenerateMipSubLevels();
	}

	return TRUE;
}

void	AgcmWater::SkinAtomicDraw(RpAtomic*		atomic)
{
	RwD3D9SetTransform(D3DTS_PROJECTION, &m_matSetProj);

	_RwD3D9D3D9ProjTransform = m_matSetProj;
	if(_RwD3D9ActiveViewProjTransform)
	{
		*_RwD3D9ActiveViewProjTransform = m_matSetViewProj;
	}

	//Draw!
	AgcmRender::OriginalDefaultAtomicRenderCallback(atomic);

	RwD3D9SetTransform(D3DTS_PROJECTION, &m_matBackupProj);
	if(_RwD3D9ActiveViewProjTransform)
	{
		*_RwD3D9ActiveViewProjTransform = m_matBackupViewProj;
	}

	_RwD3D9D3D9ProjTransform = m_matBackupProj;
}

//@{ Jaewon 20050608
// OcClumpAtomicList -> RpAtomic
void	AgcmWater::ImmediateHWaterAtomicListRender(RpAtomic*	pList)
{
	if (!m_pThis->m_pcsAgcmMap)
		return;

	PROFILE("ImmediateHWaterAtomicListRender");

	const RwSphere*			WBS = NULL;
	RpAtomic*	cur_atomic = pList;
	RpGeometry*		pGeom = NULL;

	if(m_pThis->m_bEnableUserClipPlane)
	{
		if(cur_atomic)
		{
			do
			{
				m_pThis->LockFrame();
				WBS = RpAtomicGetWorldBoundingSphere(cur_atomic);
				m_pThis->UnlockFrame();
				if(WBS->center.y + WBS->radius < m_pThis->m_fReflectionWaterHeight)	// 물밑에 존재..
					return;

				pGeom = RpAtomicGetGeometry(cur_atomic);
				if(!pGeom)	return;
				if (RpGeometryGetNumTriangles(pGeom) < 2) 	return;

				RpSkin*			pSkin = RpSkinGeometryGetSkin(pGeom);
				if(pSkin)
				{
					m_pThis->SkinAtomicDraw(cur_atomic);
				}
				else
				{
					m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
					AgcmRender::OriginalDefaultAtomicRenderCallback(cur_atomic);
					m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00);
				}

				cur_atomic = cur_atomic->next;
			}
			while(cur_atomic && (cur_atomic != pList));
		}
	}
	else
	{
		if(cur_atomic)
		{
			do
			{
				m_pThis->LockFrame();
				WBS = RpAtomicGetWorldBoundingSphere(cur_atomic);
				m_pThis->UnlockFrame();

				float ObjHeight = m_pThis->m_pcsAgcmMap->GetHeight(WBS->center.x,WBS->center.z , SECTOR_MAX_HEIGHT );
				if(ObjHeight < m_pThis->m_fReflectionWaterHeight)
				{
					return;
				}

				pGeom = RpAtomicGetGeometry(cur_atomic);
				if(!pGeom)	return;

				if (RpGeometryGetNumTriangles(pGeom) < 2) return;

				RpSkin*			pSkin = RpSkinGeometryGetSkin(pGeom);
				if(pSkin)
				{
					m_pThis->SkinAtomicDraw(cur_atomic);
				}
				else
				{
					AgcmRender::OriginalDefaultAtomicRenderCallback(cur_atomic);
				}

				cur_atomic = cur_atomic->next;
			}
			while(cur_atomic && (cur_atomic != pList));
		}
	}
}
//@} Jaewon

void	AgcmWater::ImmediateHWaterAtomicRender(RpAtomic*	atomic)
{
	if (!m_pThis->m_pcsAgcmMap)
		return;

	PROFILE("ImmediateHWaterAtomicRender");

	RpGeometry*	pGeom = RpAtomicGetGeometry(atomic);
	if(!pGeom) return;

	if (RpGeometryGetNumTriangles(pGeom) < 2) return;

	RpSkin*			pSkin = RpSkinGeometryGetSkin(pGeom);
	if(pSkin)
	{
		m_pThis->SkinAtomicDraw(atomic);
	}
	else
	{
		m_pThis->LockFrame();
		const RwSphere*	WBS = RpAtomicGetWorldBoundingSphere(atomic);
		m_pThis->UnlockFrame();

		if(WBS->radius < 50.0f) return;		// 작은거 그리지 말기!

		if(m_pThis->m_bEnableUserClipPlane)
		{
			if(WBS->center.y + WBS->radius < m_pThis->m_fReflectionWaterHeight)	// 물밑에 존재..
				return;
						
			m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
			AgcmRender::OriginalDefaultAtomicRenderCallback(atomic);
			m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00);
		}
		else
		{
			float ObjHeight = m_pThis->m_pcsAgcmMap->GetHeight(WBS->center.x,WBS->center.z , SECTOR_MAX_HEIGHT);

			if(ObjHeight < m_pThis->m_fReflectionWaterHeight)
			{
				return;
			}
			AgcmRender::OriginalDefaultAtomicRenderCallback(atomic);
		}
	}
}

void	AgcmWater::ImmediateHWaterTerrainRender(RpAtomic*	atomic,INT32 SectorDist)
{
	PROFILE("ImmediateHWaterTerrainRender");

	RpAtomicCallBackRender	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;
	atomic->stRenderInfo.shaderUseType = 0;
	atomic->stRenderInfo.beforeLODLevel = SectorDist;
	
	RpLightSetColor(g_pEngine->m_pLightDirect, &m_pThis->m_pcsAgcmRender->m_stAdjustedDirectionalLight);
	RpLightSetColor(g_pEngine->m_pLightAmbient, &m_pThis->m_pcsAgcmRender->m_stAdjustedAmbientLight);

	RpMTextureDrawStart();
	
	if(m_pThis->m_bEnableUserClipPlane)
	{
		m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
		
		pFunc(atomic);
		RpMTextureDrawEnd(FALSE);

		m_pThis->m_pCurD3D9Device->SetRenderState( D3DRS_CLIPPLANEENABLE, 0x00 );
	}
	else
	{
		RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLBACK	);
		if(m_pThis->m_pcsAgcmRender->m_bVertexShaderEnable)	m_pThis->m_pcsAgcmRender->TerrainDataSet();
		pFunc(atomic);
		RpMTextureDrawEnd(TRUE);
		RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLNONE	);
	}

	RpLightSetColor(g_pEngine->m_pLightDirect, &m_pThis->m_pcsAgcmRender->m_colorOriginalDirect);
	RpLightSetColor(g_pEngine->m_pLightAmbient, &m_pThis->m_pcsAgcmRender->m_colorOriginalAmbient);
}

/*void	AgcmWater::SetReflectShaderCommonConstant()
{
	RpLight		*ambient,*direct;
	ambient = g_pEngine->m_pLightAmbient;
	direct = g_pEngine->m_pLightDirect;

	D3DXVECTOR4		ambient_color(ambient->color.red,ambient->color.green,
										ambient->color.blue,ambient->color.alpha);
	D3DXVECTOR4		direct_color(direct->color.red,direct->color.green,
										direct->color.blue,direct->color.alpha);

	LockFrame();
	RwV3d		* pAt		= RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(direct)));
	UnlockFrame();
	D3DXVECTOR4				directLight_dir(pAt->x,pAt->y,pAt->z,0.0f);
	D3DXVECTOR4				water_height(m_fReflectionWaterHeight,0.0f,0.0f,0.0f);
	
	RwD3D9SetVertexShaderConstant(4, (void *)&water_height, 1);
	RwD3D9SetVertexShaderConstant(5, (void *)&directLight_dir, 1);
	RwD3D9SetVertexShaderConstant(6, (void *)&direct_color, 1);
	RwD3D9SetVertexShaderConstant(7, (void *)&ambient_color, 1);
	RwD3D9SetVertexShaderConstant(8, (void *)&m_pcsAgcmRender->m_fFogConstant, 1);
}

void	AgcmWater::SetReflectShaderConstant(RpAtomic*	atomic)
{
	D3DMATRIX				composedMatrix;
	D3DMATRIX				world_inverse_transpose_Matrix;
	D3DMATRIX				worldviewMatrix;

	RwMatrix*				WorldMat;
	RwMatrix				InvWorldMat;

	world_inverse_transpose_Matrix.m[0][3] = 0.0f;
	world_inverse_transpose_Matrix.m[1][3] = 0.0f;
	world_inverse_transpose_Matrix.m[2][3] = 0.0f;
	world_inverse_transpose_Matrix.m[3][3] = 1.0f;

	LockFrame();
	WorldMat = RwFrameGetLTM(RpAtomicGetFrame(atomic));
	UnlockFrame();

	RwMatrixInvert(&InvWorldMat,WorldMat);
	
	world_inverse_transpose_Matrix.m[0][0] = InvWorldMat.right.x;
	world_inverse_transpose_Matrix.m[0][1] = InvWorldMat.right.y;
	world_inverse_transpose_Matrix.m[0][2] = InvWorldMat.right.z;
	
	world_inverse_transpose_Matrix.m[1][0] = InvWorldMat.up.x;
	world_inverse_transpose_Matrix.m[1][1] = InvWorldMat.up.y;
	world_inverse_transpose_Matrix.m[1][2] = InvWorldMat.up.z;
	
	world_inverse_transpose_Matrix.m[2][0] = InvWorldMat.at.x;
	world_inverse_transpose_Matrix.m[2][1] = InvWorldMat.at.y;
	world_inverse_transpose_Matrix.m[2][2] = InvWorldMat.at.z;
	
	world_inverse_transpose_Matrix.m[3][0] = InvWorldMat.pos.x;
	world_inverse_transpose_Matrix.m[3][1] = InvWorldMat.pos.y;
	world_inverse_transpose_Matrix.m[3][2] = InvWorldMat.pos.z;

	RwD3D9SetVertexShaderConstant(10, (void *)&world_inverse_transpose_Matrix, 4);
			
	LockFrame();
	_rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(RpAtomicGetFrame(atomic)));
	_rwD3D9VSGetComposedTransformMatrix((void *)&composedMatrix);
	_rwD3D9VSGetWorldViewTransposedMatrix((void *)&worldviewMatrix);
	UnlockFrame();

	RwD3D9SetVertexShaderConstant(0, (void *)&composedMatrix, 4);
	RwD3D9SetVertexShaderConstant(14, (void *)&worldviewMatrix.m[2], 1);

	D3DXVECTOR4				world_matrix_y;
	world_matrix_y.x = WorldMat->right.y;
	world_matrix_y.y = WorldMat->up.y;
	world_matrix_y.z = WorldMat->at.y;
	world_matrix_y.w = WorldMat->pos.y;
	
	RwD3D9SetVertexShaderConstant(15, (void *)&world_matrix_y, 1);
}*/

float AgcmWater::sgn(float a)
{
	if (a > 0.0F) return (1.0F);
	if (a < 0.0F) return (-1.0F);
	return (0.0F);
}

void AgcmWater::ModifyD3DProjectionMatrix(D3DXMATRIX&	view_matrix,D3DXMATRIX&	proj_matrix)
{
	//D3DXMATRIX	matrix;
	//D3DXMATRIX	view_matrix;
	D3DXMATRIX	view_matrix_IT;
	D3DXPLANE	q;

	m_matSetProj = proj_matrix;

	// Grab the current projection matrix from D3D
	//RwD3D9GetTransform(D3DTS_VIEW, &view_matrix);
	//RwD3D9GetTransform(D3DTS_PROJECTION, &m_matSetProj);

	D3DXMatrixInverse(&view_matrix_IT,NULL,&view_matrix);
	D3DXMatrixTranspose(&view_matrix_IT,&view_matrix_IT);

	D3DXVECTOR3		n,n2,p,p2;
	n.x = 0.0f;		n.y = 1.0f;		n.z = 0.0f;
	p.x = 0.0f,		p.y = m_fReflectionWaterHeight;  p.z = 0.0f;

	D3DXVec3TransformCoord(&n2,&n,&view_matrix_IT);
	D3DXVec3TransformCoord(&p2,&p,&view_matrix);
	
	D3DXPLANE clipPlane;

	clipPlane.a = n2.x;
	clipPlane.b = n2.y;
	clipPlane.c = n2.z;
	clipPlane.d = -D3DXVec3Dot(&n2,&p2);
	
	// Calculate the clip-space corner point opposite the clipping plane
	// as (sgn(clipPlane.x), sgn(clipPlane.y), 1, 1) and
	// transform it into camera space by multiplying it
	// by the inverse of the projection matrix
	
	q.a = sgn(clipPlane.a) / m_matSetProj._11;
	q.b = sgn(clipPlane.b) / m_matSetProj._22;
	q.c = 1.0F;
	q.d = (1.0F - m_matSetProj._33) / m_matSetProj._43;
	
	// Calculate the scaled plane vector
	D3DXPLANE c;
	float cq = clipPlane.a * q.a + clipPlane.b * q.b + clipPlane.c * q.c + clipPlane.d * q.d;
	c.a = clipPlane.a * (1.0f / cq);
	c.b = clipPlane.b * (1.0f / cq);
	c.c = clipPlane.c * (1.0f / cq);
	c.d = clipPlane.d * (1.0f / cq);
	
	// Replace the third column of the projection matrix
	m_matSetProj._13 = c.a;
	m_matSetProj._23 = c.b;
	m_matSetProj._33 = c.c;
	m_matSetProj._43 = c.d;
	m_matSetProj._44 = 0.0f;

	// Load it back into D3D
	//RwD3D9SetTransform(D3DTS_PROJECTION, &m_matSetProj);

	//if(_RwD3D9ActiveViewProjTransform)
	//{
	D3DXMatrixMultiply(&m_matSetViewProj, &view_matrix, &m_matSetProj);
	//}
}

void	AgcmWater::UpdateReflectionMap()
{
	LockFrame();
	RwCameraEndUpdate(m_pMainCamera);

	RwCameraBeginUpdate(m_pReflectionCamera);
	RwCameraClear(m_pReflectionCamera, NULL, rwCAMERACLEARZ);
	RwCameraEndUpdate(m_pReflectionCamera);

	//-----------------------------------------------------------------------
	// reflectionMap을 생략합니다. - 2010.11.29 kdi

	RwCameraBeginUpdate(m_pMainCamera);

	UnlockFrame();
	return;

	//-----------------------------------------------------------------------
	/*
	if (RwCameraBeginUpdate(m_pReflectionCamera))
	{
		RwD3D9SetRenderState( D3DRS_FOGEND,  *((DWORD*)(&m_pcsAgcmRender->m_fFogEnd))); 

		m_matBackupProj = _RwD3D9D3D9ProjTransform;
			
		if(_RwD3D9ActiveViewProjTransform)
		{
			m_matBackupViewProj = *_RwD3D9ActiveViewProjTransform;
		}

		D3DXMATRIX	view,proj;
		RwD3D9GetTransform(D3DTS_VIEW, &view);
		RwD3D9GetTransform(D3DTS_PROJECTION, &proj);

		ModifyD3DProjectionMatrix(view,proj);
	
		_rwD3D9VSSetActiveWorldMatrix(NULL);
		_rwD3D9VSGetComposedTransformMatrix((void*)&m_matReflectionViewProjection);

		{
		PROFILE("UpdateReflectionMap - RenderSky");
		m_pcsAgcmEventNature->RenderSky();
		}

		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)true );
		RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );

		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 100  );
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION , (void*) rwALPHATESTFUNCTIONGREATER );

		RwD3D9SetRenderState( D3DRS_FOGEND,  *((DWORD*)(&m_pcsAgcmRender->m_fFogEnd))); 

		m_pcsAgcmRender->ResetTickCount();
		
//		SetReflectShaderCommonConstant();
		if(m_bEnableUserClipPlane)
		{
			D3DXVECTOR3 a( 1.5f,m_fReflectionWaterHeight, 3.0f );
			D3DXVECTOR3 b( -1.5f,m_fReflectionWaterHeight, 3.0f );
			D3DXVECTOR3 c( 1.5f,m_fReflectionWaterHeight, -3.0f );
			D3DXPLANE  plane;

			m_pCurD3D9Device->SetClipPlane( 0, *D3DXPlaneFromPoints( &plane, &b, &a, &c ) );
		}

		{
		PROFILE("UpdateReflectionMap - RenderMyCamera");
	    m_pcsAgcmRender->RenderMyCamera(m_pReflectionCamera,TRUE,ImmediateHWaterTerrainRender,
									ImmediateHWaterAtomicListRender,ImmediateHWaterAtomicRender);
		}
				
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 0  );

		RwCameraEndUpdate(m_pReflectionCamera);
	}

	{
	PROFILE("UpdateReflectionMap - MainCamera BeginUpdate");
	RwCameraBeginUpdate(m_pMainCamera);

	UnlockFrame();

	// 릴리즈에서 카메라 비긴 업데이트 이후 projection matrix _44 값이 이상한 값이 들어갈 때가 있다.
// 	D3DMATRIX	proj;
// 	RwD3D9GetTransform(D3DTS_PROJECTION, &proj);
// 	proj._44 = 0.0f;
// 	RwD3D9SetTransform(D3DTS_PROJECTION, &proj);

	RwD3D9SetRenderState( D3DRS_FOGEND,  *((DWORD*)(&m_pcsAgcmRender->m_fFogEnd))); 

	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)false );
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)false );
	}
	*/
}

void	AgcmWater::AddHWaterToRender(HWater*	hwater)
{
	if(m_pcsApmOcTree->m_bOcTreeEnable)
	{
		// 옥트리에 넣는 것은 node 하나에만 추가된다. 물크기에 비해 너무 작음 ㅡㅡ;
		/*OcCustomDataList	stSetParam;
		stSetParam.pData2 = NULL;
		stSetParam.iAppearanceDistance = WAVE_VIEW_SECTOR_RANGE;
		stSetParam.pClass = this;
		stSetParam.pRenderCB = CB_HWRender;
		stSetParam.pUpdateCB = CB_HWUpdate;

		stSetParam.pData1 = hwater;
		stSetParam.BS.center = hwater->BS.center;
		stSetParam.BS.radius = hwater->BS.radius;
		
		stSetParam.piCameraZIndex = &m_iTemp; 

		stSetParam.TopVerts[0].x = stSetParam.BS.center.x - stSetParam.BS.radius;
		stSetParam.TopVerts[0].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[0].z = stSetParam.BS.center.z - stSetParam.BS.radius;

		stSetParam.TopVerts[1].x = stSetParam.BS.center.x + stSetParam.BS.radius;
		stSetParam.TopVerts[1].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[1].z = stSetParam.BS.center.z - stSetParam.BS.radius;

		stSetParam.TopVerts[2].x = stSetParam.BS.center.x + stSetParam.BS.radius;
		stSetParam.TopVerts[2].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[2].z = stSetParam.BS.center.z + stSetParam.BS.radius;

		stSetParam.TopVerts[3].x = stSetParam.BS.center.x - stSetParam.BS.radius;
		stSetParam.TopVerts[3].y = stSetParam.BS.center.y;
		stSetParam.TopVerts[3].z = stSetParam.BS.center.z + stSetParam.BS.radius;

		m_pcsAgcmOcTree->AddCustomRenderDataToOcTree(hwater->BS.center.x,hwater->BS.center.y,
												hwater->BS.center.z,&stSetParam);*/


		m_pcsAgcmRender->AddCustomRenderObject(this,&hwater->BS,50.0f,CB_HWUpdate,CB_HWRender,(PVOID)hwater,NULL);

	}
	else
	{
		m_pcsAgcmRender->AddCustomRenderToSector(this,&hwater->BS,CB_HWUpdate,CB_HWRender,(PVOID)hwater,NULL);
	}
}

void	AgcmWater::RemoveHWaterFromRender(HWater*	hwater)
{
	if(m_pcsApmOcTree->m_bOcTreeEnable)
	{
		//m_pcsAgcmOcTree->RemoveCustomRenderDataFromOcTree(hwater->BS.center.x,hwater->BS.center.y,
		//															hwater->BS.center.z,(PVOID)this,(PVOID)hwater,NULL);

		m_pcsAgcmRender->RemoveCustomRenderObject(this,(PVOID)hwater,NULL);
	}			
	else
	{
		m_pcsAgcmRender->RemoveCustomRenderFromSector(this,&hwater->BS,(PVOID)hwater,NULL);
	}
}

void	AgcmWater::ReleaseHWater(HWater*	remove_HWater)
{
	if(remove_HWater->pvVB)
	{
		//RwD3D9DestroyVertexBuffer(sizeof(HWATER_VERTEX), remove_HWater->numTris*3*sizeof(HWATER_VERTEX),
		//								remove_HWater->pvVB , 0);	

		//@{ 2006/10/31 burumal
		if ( !IsBadReadPtr(remove_HWater->pvVB, sizeof(LPDIRECT3DVERTEXBUFFER9)) )
		//@}
			remove_HWater->pvVB->Release();

		remove_HWater->pvVB = NULL;
	}

	if(remove_HWater->pvIB)
	{
		//@{ 2006/10/31 burumal
		if ( !IsBadReadPtr(remove_HWater->pvIB, sizeof(LPDIRECT3DINDEXBUFFER9)) )
		//@}
			remove_HWater->pvIB->Release();

		remove_HWater->pvIB = NULL;
	}
}

void	AgcmWater::ChangeHWaterMode(BOOL bOn)
{
	Sector*			cur_sec = m_listSector;
	
	while(cur_sec)
	{
		ChangeHWaterMode(bOn,cur_sec);

		cur_sec = cur_sec->next;
	}

	if(!bOn)
	{
		//@{ Jaewon 20050705
		// There are 2 versions of the shader pair.
		for(RwUInt32 i=0; i<2; ++i)
		{
			if(m_pHWaterVS11[i])
			{
				RwD3D9DeleteVertexShader(m_pHWaterVS11[i]);
				m_pHWaterVS11[i] = NULL;
			}

			if(m_pHWaterPS11[i])
			{
				RwD3D9DeletePixelShader(m_pHWaterPS11[i]);
				m_pHWaterPS11[i] = NULL;
			}
		}
		for(RwUInt32 i=0; i<2; ++i)
		{
			if(m_pHWaterVS14[i])
			{
				RwD3D9DeleteVertexShader(m_pHWaterVS14[i]);
				m_pHWaterVS14[i] = NULL;
			}

			if(m_pHWaterPS14[i])
			{
				RwD3D9DeletePixelShader(m_pHWaterPS14[i]);
				m_pHWaterPS14[i] = NULL;
			}
		}
		//@} Jaewon

		LockFrame();
		if (m_pReflectionCamera)
		{
			RpWorldRemoveCamera(g_pEngine->m_pWorld , m_pReflectionCamera);

			RwFrame            *frame;

			frame = RwCameraGetFrame(m_pReflectionCamera);
			if (frame)
			{
				RwCameraSetFrame(m_pReflectionCamera, NULL);
				RwFrameDestroy(frame);
			}
					
			RwCameraDestroy(m_pReflectionCamera);
			m_pReflectionCamera = NULL;
		}
		UnlockFrame();

		if(m_pReflectionTexture)
		{
			RwTextureSetRaster(m_pReflectionTexture, NULL);
			RwTextureDestroy(m_pReflectionTexture);
			m_pReflectionTexture = NULL;
		}
	
		if(m_pReflectionRaster)
		{
			RwRasterDestroy(m_pReflectionRaster);
			m_pReflectionRaster = NULL;
		}

		if(m_pReflectionRasterZ)
		{
			RwRasterDestroy(m_pReflectionRasterZ);
			m_pReflectionRasterZ = NULL;
		}
	}
	else 
	{
		if(m_bEnableHWater && !m_pReflectionCamera)			// m_pReflectionCamera 이 NULL일 경우 생성
		{
			ASSERT(!m_pReflectionRaster);		// 기존에 있는데 또 생성?

		    m_pReflectionRaster = RwRasterCreate(WATER_REFLECT_TEX_SIZE, WATER_REFLECT_TEX_SIZE, 0, rwRASTERTYPECAMERATEXTURE);
			if(m_pReflectionRaster == NULL)
			{
				ASSERT(!"Can't create a reflection map raster in AgcmWater::OnInit()!");
				m_bEnableHWater = FALSE;
				return;
			}

			m_pReflectionRasterZ = RwRasterCreate(WATER_REFLECT_TEX_SIZE, WATER_REFLECT_TEX_SIZE, 0, rwRASTERTYPEZBUFFER);
			if(m_pReflectionRasterZ == NULL)
			{
				ASSERT(!"Can't create a reflection map z raster in AgcmWater::OnInit()!");
				RwRasterDestroy(m_pReflectionRaster);
				m_pReflectionRaster = NULL;
				m_bEnableHWater = FALSE;

				return;
			}
	
			LockFrame();
			m_pReflectionCamera = RwCameraCreate();
			if(!m_pReflectionCamera)
			{
				UnlockFrame();
				ASSERT(!"Can't create a reflection map camera in AgcmWater::OnInit()!");

				RwRasterDestroy(m_pReflectionRaster);
				m_pReflectionRaster = NULL;
				RwRasterDestroy(m_pReflectionRasterZ);
				m_pReflectionRasterZ = NULL;
				m_bEnableHWater = FALSE;
				return;
			}
			RwCameraSetFrame(m_pReflectionCamera, RwFrameCreate());
			if(!RwCameraGetFrame(m_pReflectionCamera))
			{
				UnlockFrame();
				RwRasterDestroy(m_pReflectionRaster);
				m_pReflectionRaster = NULL;
				RwRasterDestroy(m_pReflectionRasterZ);
				m_pReflectionRasterZ = NULL;
				RwCameraDestroy(m_pReflectionCamera);
				m_pReflectionCamera = NULL;
				m_bEnableHWater = FALSE;
				return;
			}
			RwCameraSetRaster(m_pReflectionCamera, m_pReflectionRaster);
			RwCameraSetZRaster(m_pReflectionCamera, m_pReflectionRasterZ);
		
			m_pReflectionTexture = RwTextureCreate(m_pReflectionRaster);
			RwTextureSetFilterMode(m_pReflectionTexture,rwFILTERLINEAR );

			RpWorldAddCamera(g_pEngine->m_pWorld , m_pReflectionCamera);

			OnCameraStateChange(CSC_INIT);

			// HighWater Shaders
			//@{ Jaewon 20050705
			// ps.1.1 support added/new ps.1.4 shader added
			if(m_bEnableHWater == 1)
			{
				m_pHWaterVS14[0] = GetShader(g_vs14_HWater);
				ASSERT(m_pHWaterVS14[0]);
				m_pHWaterPS14[0] = GetShader(g_ps14_HWater_old, TRUE);
				ASSERT(m_pHWaterPS14[0]);

				m_pHWaterVS14[1] = GetShader(g_vs14_HWater);
				ASSERT(m_pHWaterVS14[1]);
				m_pHWaterPS14[1] = GetShader(g_ps14_HWater_new, TRUE);
				ASSERT(m_pHWaterPS14[1]);
			}
			if(m_bEnableHWater)
			{
				m_pHWaterVS11[0] = GetShader(g_vs11_HWater_Fresnel);
				ASSERT(m_pHWaterVS11[0]);
				m_pHWaterPS11[0] = GetShader(g_ps11_HWater_Fresnel, TRUE);
				ASSERT(m_pHWaterPS11[0]);

				m_pHWaterVS11[1] = GetShader(g_vs11_HWater_specular);
				ASSERT(m_pHWaterVS11[1]);
				m_pHWaterPS11[1] = GetShader(g_ps11_HWater_specular, TRUE);
				ASSERT(m_pHWaterPS11[1]);
			}
			//@} Jaewon

			UnlockFrame();
		}
	}
}

// HWater list를 만들고 호출하자
void	AgcmWater::ChangeHWaterMode(BOOL bOn,Sector*	pSec)
{
	Water*			cur_Water = NULL;
	HWater*			cur_HWater = NULL;

	cur_Water = pSec->WaterList;
	while(cur_Water)
	{
		//@{ 2006/08/31 burumal
		if ( IsBadReadPtr(cur_Water, sizeof(Water)) )
			break;
		//@}

		if(bOn && pSec->AddedRenderWater == true)
		{
			RemoveWaterFromRender(cur_Water);
			pSec->AddedRenderWater = false;
		}
		else if(!bOn && pSec->AddedRenderWater == false)
		{
			AddWaterToRender(cur_Water);
			pSec->AddedRenderWater = true;
		}
		
		cur_Water = cur_Water->next;
	}

	cur_HWater = pSec->HWaterList;
	HWater*		remove_HWater;
	while(cur_HWater)
	{
		//@{ 2006/08/31 burumal
		if ( IsBadReadPtr(cur_HWater, sizeof(HWater)) )
			break;
		//@}

		if(bOn && pSec->AddedRenderHWater == false)
		{
			AddHWaterToRender(cur_HWater);
			pSec->AddedRenderHWater = true;

			cur_HWater = cur_HWater->next;
		}
		else if(!bOn && pSec->AddedRenderHWater == true)
		{
			RemoveHWaterFromRender(cur_HWater);
			pSec->AddedRenderHWater = false;
			
			remove_HWater = cur_HWater;
			ReleaseHWater(remove_HWater);
			cur_HWater = cur_HWater->next;
			delete remove_HWater;
		}
		else
		{
			cur_HWater = cur_HWater->next;
		}
	}

	if(!bOn) 
		pSec->HWaterList = NULL;
}

void	AgcmWater::SetShaderCommonConstant()
{
	RwFrame*	camframe = RwCameraGetFrame(g_pEngine->m_pCamera);
	
	LockFrame();
	RwMatrix*	ltm = RwFrameGetLTM(camframe);
	RwV3d		* pAt		= RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(g_pEngine->m_pLightDirect)));
	UnlockFrame();
		
	//RwV3d	lightpos;
	//if(m_pcsAgcmEventNature)
		//m_pcsAgcmEventNature->GetSunPosition	( &lightpos );

	// c0    - { 0.0,  0.5, 1.0, 2.0}
	// c1    - { 4.0, .5pi, pi, 2pi}  
	// c2    - {1, -1/3!, 1/5!, -1/7!  }  //for sin
	// c3    - {1/2!, -1/4!, 1/6!, -1/8!  }  //for cos
	// c4-7 - Composite World-View-Projection Matrix
	// c8     - ModelSpace Camera Position
	// c9     - ModelSpace Light Direction
	// c10   - {fixup factor for taylor series imprecision, }(0.9855, 0.1, 0, 0)
	// c16    - { time, sin(time)}
	// c18   - World Martix

	D3DXVECTOR4 c0( 0.0f,  0.5f, 1.0f, 2.0f );
	D3DXVECTOR4 c1( 4.0f, 0.5f*D3DX_PI, D3DX_PI, 2.0f*D3DX_PI);
	D3DXVECTOR4 c2( 1.0f,      -1.0f/6.0f,  1.0f/120.0f, -1.0f/5040.0f);
	D3DXVECTOR4 c3( 1.0f/2.0f, -1.0f/24.0f, 1.0f/720.0f, -1.0f/40320.0f );
	//@{ Jaewon 20050830
	// NVIDIA cards have a bug related to large(>20000.0) shader constants.
	// So scale down coordinate values properly.
	D3DXVECTOR4 c8( ltm->pos.x*0.01f, ltm->pos.y*0.01f, ltm->pos.z*0.01f, 0.01f);	
	//@} Jaewon
	D3DXVECTOR4 c9( -pAt->x,-pAt->y,-pAt->z,0.0f);
	D3DXVECTOR4 c10( 0.9855f, 0.0002f, 0.0f, 0.0f );
	//D3DXVECTOR4 c10( 0.9855f, m_stHWaterStatus[0].DirY3, 0.0f, 0.0f );
	D3DXVECTOR4 c16( (float)m_fTime*0.75f, sinf(m_fTime), 0.0f, 0.0f);

	RwD3D9SetVertexShaderConstant (  0, c0,  1 );
	RwD3D9SetVertexShaderConstant (  1, c1,  1 );
	RwD3D9SetVertexShaderConstant (  2, c2,  1 );
	RwD3D9SetVertexShaderConstant (  3, c3,  1 );
	RwD3D9SetVertexShaderConstant (  8, c8,  1 );
	RwD3D9SetVertexShaderConstant (  9, c9,  1 );
	RwD3D9SetVertexShaderConstant (  10, c10,  1 );
	RwD3D9SetVertexShaderConstant (  16, c16,  1 );
	
	D3DMATRIX				composedMatrix;
	
	RwMatrix				identity;
	RwMatrixSetIdentity(&identity);
	
	LockFrame();
	_rwD3D9VSSetActiveWorldMatrix(&identity);
	_rwD3D9VSGetComposedTransformMatrix((void *)&composedMatrix);
	UnlockFrame();

	RwD3D9SetVertexShaderConstant(4, (void *)&composedMatrix, 4);

	/*D3DMATRIX				WorldTransposeMatrix;
	
	WorldTransposeMatrix.m[0][0] = identity.right.x;
	WorldTransposeMatrix.m[0][1] = identity.up.x;
	WorldTransposeMatrix.m[0][2] = identity.at.x;
	WorldTransposeMatrix.m[0][3] = identity.pos.x;

	WorldTransposeMatrix.m[1][0] = identity.right.y;
	WorldTransposeMatrix.m[1][1] = identity.up.y;
	WorldTransposeMatrix.m[1][2] = identity.at.y;
	WorldTransposeMatrix.m[1][3] = identity.pos.y;

	WorldTransposeMatrix.m[2][0] = identity.right.z;
	WorldTransposeMatrix.m[2][1] = identity.up.z;
	WorldTransposeMatrix.m[2][2] = identity.at.z;
	WorldTransposeMatrix.m[2][3] = identity.pos.z;

	WorldTransposeMatrix.m[3][0] = 0.0f;
	WorldTransposeMatrix.m[3][1] = 0.0f;
	WorldTransposeMatrix.m[3][2] = 0.0f;
	WorldTransposeMatrix.m[3][3] = 1.0f;

	RwD3D9SetVertexShaderConstant(18, (void *)&WorldTransposeMatrix, 4);*/

	D3DXMATRIX	final;

	//FLOAT fTexelOffs = (0.5f / (FLOAT)WATER_REFLECT_TEX_SIZE);
	D3DXMATRIX matTexAdj(0.5f,      0.0f,        0.0f,        0.5f,
						  0.0f,     -0.5f,        0.0f,        0.5f,
						  0.0f,      0.0f,        0.0f,        1.0f,
						  0.0f,		 0.0f,		  0.0f,		   1.0f);

	D3DXMatrixMultiply(&final,&matTexAdj,(D3DXMATRIX*)&m_matReflectionViewProjection);

	RwD3D9SetVertexShaderConstant(22, (void *)&final, 3);
}

// Status마다 다르게 설정되기 때문에..
void	AgcmWater::SetShaderConstant(INT32 ID)
{
	// c11   - {waveHeight0, waveHeight1, waveHeight2, waveHeight3}  (0.4, 0.5, 0.025, 0.025)
	// c13   - {waveSpeed0, waveSpeed1, waveSpeed2, waveSpeed3}   (0.2, 0.15, 0.4, 0.4)
	// c14   - {waveDirX0, waveDirX1, waveDirX2, waveDirX3}   (0.25, 0.0, -0.7, -0.8)
	// c15   - {waveDirY0, waveDirY1, waveDirY2, waveDirY3}   (0.0, 0.15, -0.7, 0.1)
	// c17    - {basetexcoord distortion x0, y0, x1, y1} (0.031, 0.04, -0.03, 0.02)
	D3DXVECTOR4 c11( m_stHWaterStatus[ID].Height0, m_stHWaterStatus[ID].Height1, m_stHWaterStatus[ID].Height2, m_stHWaterStatus[ID].Height3);
	D3DXVECTOR4 c13( m_stHWaterStatus[ID].Speed0, m_stHWaterStatus[ID].Speed1, m_stHWaterStatus[ID].Speed2, m_stHWaterStatus[ID].Speed3);
	D3DXVECTOR4 c14( m_stHWaterStatus[ID].DirX0, m_stHWaterStatus[ID].DirX1, m_stHWaterStatus[ID].DirX2, m_stHWaterStatus[ID].DirX3);
	D3DXVECTOR4 c15( m_stHWaterStatus[ID].DirY0, m_stHWaterStatus[ID].DirY1, m_stHWaterStatus[ID].DirY2, m_stHWaterStatus[ID].DirY3);
	//D3DXVECTOR4 c15( m_stHWaterStatus[ID].DirY0, m_stHWaterStatus[ID].DirY1, m_stHWaterStatus[ID].DirY2, 0.4f);
	D3DXVECTOR4 c17( m_stHWaterStatus[ID].TexX0, m_stHWaterStatus[ID].TexY0, m_stHWaterStatus[ID].TexX1, m_stHWaterStatus[ID].TexY1);
	
	RwD3D9SetVertexShaderConstant (  11, c11,  1 );
	RwD3D9SetVertexShaderConstant (  13, c13,  1 );
	RwD3D9SetVertexShaderConstant (  14, c14,  1 );
	RwD3D9SetVertexShaderConstant (  15, c15,  1 );
	RwD3D9SetVertexShaderConstant (  17, c17,  1 );
    
	//c0 - Common Const, a - water alpha
	//c1 - highlightColor
	//@{ Jaewon 20050706
	//c2 - base color
	D3DXVECTOR4 c0p( 0.0f, 0.5f, 1.0f, m_stHWaterStatus[ID].ReflectionWeight);
	D3DXVECTOR4 c1p( m_stHWaterStatus[ID].Red, m_stHWaterStatus[ID].Green, m_stHWaterStatus[ID].Blue, 1.0f);
	D3DXVECTOR4 c2p( m_stHWaterStatus[ID].BaseR, m_stHWaterStatus[ID].BaseG, m_stHWaterStatus[ID].BaseB, 1.0f);
	//@} Jaewon
		
	//@{ Jaewon 20050705
	if(m_bEnableHWater == 1 && m_bForce11Shader == false)
	// ps.1.4
		RwD3D9SetPixelShaderConstant(  0, c0p,  1 );
	else
	// ps.1.1
		RwD3D9SetPixelShaderConstant(0, 
				D3DXVECTOR4(m_stHWaterStatus[ID].ReflectionWeight,m_stHWaterStatus[ID].ReflectionWeight,
							m_stHWaterStatus[ID].ReflectionWeight,m_stHWaterStatus[ID].ReflectionWeight),  
									1);
	//@} Jaewon
	RwD3D9SetPixelShaderConstant(  1, c1p,  1 );
	//@{ Jaewon 20050706
	// c2 - base color
	RwD3D9SetPixelShaderConstant(  2, c2p,  1 );
	//@} Jaewon
}

BOOL	AgcmWater::CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*) pClass;

	return TRUE;
}

BOOL	AgcmWater::CB_HWUpdate ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*)pClass;
	if(!pThis->m_bReflectionUpdate)
	{
		pThis->m_bReflectionUpdate = TRUE;
					
		HWater*		pHW = (HWater*)pData;
		pThis->m_matReflectionTrafoMatrix._42 = 2.0f * pHW->Height;
		pThis->m_fReflectionWaterHeight = pHW->Height;
	}

	return TRUE;
}

BOOL	AgcmWater::CB_HWRender ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater*		pThis = (AgcmWater*)pClass;

	if(pThis->m_pcsApmOcTree->m_bOcTreeEnable)
	{
		if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH)
			pThis->RenderHQWater((HWater*)pData);
	}
	else
	{
		DrawHWater*		nw_DH = (DrawHWater*)AcuFrameMemory::AllocFrameMemory(sizeof(DrawHWater));
		if( nw_DH )
		{
			nw_DH->hwater = (HWater*)pData;
			nw_DH->next = pThis->m_listDrawHWater;
			pThis->m_listDrawHWater = nw_DH;
		}
	}

	return TRUE;
}

BOOL	AgcmWater::CB_HWRelease ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	return TRUE;
}

void	AgcmWater::RenderHQWater(HWater*	pHW)
{
	// Restore state to draw ocean
	RwD3D9SetTexture(m_pReflectionTexture,2);

	RwMatrix	identity;
	RwMatrixSetIdentity(&identity);
	RwD3D9SetTransform( D3DTS_WORLD, &identity );
	//@{ Jaewon 20050705
	// There are 2 versions of the shader pair.
	INT32	wid = pHW->StatusID;
	if(m_bEnableHWater == 1 && m_bForce11Shader == false)
	{
		RwD3D9SetVertexShader(m_pHWaterVS14[m_stHWaterStatus[wid].Shader14ID]);
		RwD3D9SetPixelShader(m_pHWaterPS14[m_stHWaterStatus[wid].Shader14ID]);
	}
	else
	{
		RwD3D9SetVertexShader(m_pHWaterVS11[m_stHWaterStatus[wid].Shader11ID]);
		RwD3D9SetPixelShader(m_pHWaterPS11[m_stHWaterStatus[wid].Shader11ID]);
	}
	//@} Jaewon

	// set the vertex declaration.
	RwD3D9SetVertexDeclaration(m_pVertexDecl);
	
	SetShaderCommonConstant();

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	_rwD3D9RenderStateFlushCache();
	
	INT32	bump_id = m_stHWaterStatus[wid].BumpTexID;

	m_pCurD3D9Device->SetTexture(0, m_prHQBump[bump_id]);
	m_pCurD3D9Device->SetTexture(1, m_prHQBump[bump_id]);

	RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

	RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);
	
	RwD3D9SetSamplerState(0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(0, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

	RwD3D9SetSamplerState(1, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(1, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(1, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

	RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
	RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);

	RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
	RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);

	RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
	RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);
	
	//@{ Jaewon 20050705
	if(m_bEnableHWater == 1 && m_bForce11Shader == false)
	// ps.1.4
	{
		RwD3D9SetTexture( m_pNormCubeMap, 3 );
		RwD3D9SetTexture( m_pNormCubeMap, 4 );
		RwD3D9SetTexture( m_prHQFresnel[m_stHWaterStatus[wid].FresnelTexID] ,5);
	}
	else
	// ps.1.1
		RwD3D9SetTexture( m_pNormCubeMap, 3 );
	//@} Jaewon

	SetShaderConstant(wid);

	m_pCurD3D9Device->SetStreamSource( 0, pHW->pvVB, 0, sizeof(HWATER_VERTEX) );
    m_pCurD3D9Device->SetIndices( pHW->pvIB );		
	m_pCurD3D9Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,0, pHW->numVerts, 0, pHW->numTris );
		
	RwD3D9SetTexture(NULL,0);		// cash clear
	RwD3D9SetTexture(NULL,1);		// cash clear

	RwD3D9SetStreamSource ( 0, NULL ,0 ,0 );
	RwD3D9SetIndices ( NULL );
	
	RwD3D9SetVertexShader(NULL);
	RwD3D9SetPixelShader(NULL);
}

void	AgcmWater::RenderHQWater()
{
	if(!m_listDrawHWater)	return;

	// Restore state to draw ocean
	RwD3D9SetTexture(m_pReflectionTexture,2);

	RwMatrix	identity;
	RwMatrixSetIdentity(&identity);
	RwD3D9SetTransform( D3DTS_WORLD, &identity );

	// set the vertex declaration.
	RwD3D9SetVertexDeclaration(m_pVertexDecl);
	
	SetShaderCommonConstant();

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	
	_rwD3D9RenderStateFlushCache();
	
	DrawHWater*		cur_DH = m_listDrawHWater;
	int				before_id = -1,bump_id;
	HWater*			cur_HW = NULL;
	while(cur_DH)
	{
		cur_HW = cur_DH->hwater;
		if(before_id != cur_HW->StatusID)
		{
			before_id = cur_HW->StatusID;

			//@{ Jaewon 20050705
			// There are 2 versions of the shader pair.
			if(m_bEnableHWater == 1 && m_bForce11Shader == false)
			{
				RwD3D9SetVertexShader(m_pHWaterVS14[m_stHWaterStatus[before_id].Shader14ID]);
				RwD3D9SetPixelShader(m_pHWaterPS14[m_stHWaterStatus[before_id].Shader14ID]);
			}
			else
			{
				RwD3D9SetVertexShader(m_pHWaterVS11[m_stHWaterStatus[before_id].Shader11ID]);
				RwD3D9SetPixelShader(m_pHWaterPS11[m_stHWaterStatus[before_id].Shader11ID]);
			}
			//@} Jaewon

			bump_id = m_stHWaterStatus[before_id].BumpTexID;

			m_pCurD3D9Device->SetTexture(0, m_prHQBump[bump_id]);
			m_pCurD3D9Device->SetTexture(1, m_prHQBump[bump_id]);

			RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
			RwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

			RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
			RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);

			RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSU , D3DTADDRESS_CLAMP);
			RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSV , D3DTADDRESS_CLAMP);

			RwD3D9SetSamplerState(0, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(0, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(0, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

			RwD3D9SetSamplerState(1, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(1, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(1, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);

			RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			
			//m_pCurD3D9Device->SetTexture(1, m_prHQBump[before_id]);
			//@{ Jaewon 20050705
			if(m_bEnableHWater == 1 && m_bForce11Shader == false)
			// ps.1.4
			{
				RwD3D9SetTexture( m_pNormCubeMap, 3 );
				RwD3D9SetTexture( m_pNormCubeMap, 4 );
				RwD3D9SetTexture( m_prHQFresnel[m_stHWaterStatus[before_id].FresnelTexID] ,5);
			}
			else
			// ps.1.1
				RwD3D9SetTexture( m_pNormCubeMap, 3 );
			//@} Jaewon

			SetShaderConstant(before_id);
		}

		//RwD3D9SetStreamSource ( 0, cur_HW->pvVB, cur_HW->iVertexOffset, sizeof(HWATER_VERTEX) );
		//RwD3D9SetIndices ( cur_HW->pvIB );
		//RwD3D9DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,0,cur_HW->numVerts,cur_HW->iVertexOffset, cur_HW->numTris );

		m_pCurD3D9Device->SetStreamSource( 0, cur_HW->pvVB, 0, sizeof(HWATER_VERTEX) );
        m_pCurD3D9Device->SetIndices( cur_HW->pvIB );		
		m_pCurD3D9Device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,0, cur_HW->numVerts, 0, cur_HW->numTris );
		
		cur_DH = cur_DH->next;
	}

	RwD3D9SetStreamSource ( 0, NULL ,0 ,0 );
	RwD3D9SetIndices ( NULL );

	RwD3D9SetTexture(m_pReflectionTexture,0);		// cash clear
	RwD3D9SetTexture(m_pReflectionTexture,1);		// cash clear
	
	RwD3D9SetVertexShader(NULL);
	RwD3D9SetPixelShader(NULL);

	//RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

//	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

//	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
//	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

/*D3DXMATRIX AgcmWater::GetCubeMapViewMatrix( DWORD dwFace )
{
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookDir;
    D3DXVECTOR3 vUpDir;

	RwMatrix*	CharacMatrix;
	
	LockFrame();
	CharacMatrix = RwFrameGetLTM(m_pcsAgcmRender->m_pFrame);
	UnlockFrame();

	vEyePt.x = CharacMatrix->pos.x;
	vEyePt.y = CharacMatrix->pos.y;
	vEyePt.z = CharacMatrix->pos.z;

    switch( dwFace )
    {
        case HQCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case HQCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case HQCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case HQCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case HQCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case HQCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
    }

    // Set the view transform for this cubemap surface
    D3DXMATRIXA16 mView;
    D3DXMatrixLookAtLH( &mView, &vEyePt, &vLookDir, &vUpDir );
    return mView;
}

void	AgcmWater::UpdateEMCB()
{
	LPDIRECT3DSURFACE9 pRTOld = NULL;
    m_pCurD3D9Device->GetRenderTarget( 0, &pRTOld );

	D3DMATRIX	matView;
	m_pCurD3D9Device->GetTransform(D3DTS_VIEW , &matView);

	m_pCurD3D9Device->EndScene();
		
	for( int nFace = 0; nFace < 6; ++nFace )
    {
        LPDIRECT3DSURFACE9 pSurf;

        m_pCubeEnvironment->GetCubeMapSurface( (D3DCUBEMAP_FACES)nFace, 0, &pSurf );
        m_pCurD3D9Device->SetRenderTarget( 0, pSurf );
        if( pSurf ) pSurf->Release();

        D3DXMATRIXA16 mView = GetCubeMapViewMatrix( nFace );
        
        m_pCurD3D9Device->Clear( 0L, NULL, D3DCLEAR_ZBUFFER,0x000000ff, 1.0f, 0L );

        // Begin the scene
        if( SUCCEEDED( m_pCurD3D9Device->BeginScene() ) )
        {
            m_pCurD3D9Device->SetTransform( D3DTS_VIEW, &mView );

            // End the scene.
            m_pCurD3D9Device->EndScene();
        }
    }

	m_pCurD3D9Device->SetTransform(D3DTS_VIEW , &matView);
	m_pCurD3D9Device->SetRenderTarget( 0, pRTOld );

	m_pCurD3D9Device->BeginScene();
}*/

BOOL	AgcmWater::MakeHWaterFromWater()
{
	Sector*			cur_sec = m_listSector;
	while(cur_sec)
	{
		if(!MakeHWaterFromWater(cur_sec))
			return FALSE;

		cur_sec = cur_sec->next;
	}

	return TRUE;
}
	
BOOL	AgcmWater::MakeHWaterFromWater(Sector*	pSec)
{
	if (!m_pcsAgcmMap || !m_bEnableHWater)
		return FALSE;

	Water*			cur_Water = NULL;
	HWater*			nw_HWater = NULL;
	
	HWater*			cur_HWater = NULL;	// release용
	HWater*			remove_HWater = NULL;

	int				xres = 0,zres = 0;
	WORD*			pIndices = NULL;
	HWATER_VERTEX*	pVertices = NULL;
	int				i = 0,j = 0;

	float			xmin = 0.0f,xmax = 0.0f,zmin = 0.0f,zmax = 0.0f,xdist = 0.0f,zdist = 0.0f;
	float			cur_height = 0.0f,height_diff = 0.0f;
	int				Status_ID = 0;

	float			height_limit_diff = 0.0f,alpha_limit_diff = 0.0f;
	BOOL			bRemoved = FALSE;

	cur_HWater = pSec->HWaterList;
	while(cur_HWater)
	{
		//@{ 2006/08/31 burumal
		if ( IsBadReadPtr(cur_HWater, sizeof(HWater)) )
			break;;
		//@}

		remove_HWater = cur_HWater;
		cur_HWater = cur_HWater->next;

		if(pSec->AddedRenderHWater == true)
		{
			bRemoved = TRUE;
			RemoveHWaterFromRender(remove_HWater);
		}
		ReleaseHWater(remove_HWater);

		delete remove_HWater;
	}

	if(bRemoved) pSec->AddedRenderHWater = false;

	pSec->HWaterList = NULL;

	// GetHeight_Lowlevel_HeightOnly .. thread safe안하다.. 안쓰기로 하자..
	// 전체 sector lock으로 감싸기..
	
	INT32	x_arrayindex = SectorIndexToArrayIndexX(pSec->SectorXIndex);
	INT32	z_arrayindex = SectorIndexToArrayIndexZ(pSec->SectorZIndex);
	ApWorldSector*	pWorldSector = AGCMMAP_THIS->m_pcsApmMap->GetSectorByArrayIndex(x_arrayindex,z_arrayindex);

	//. 2006. 7. 26. Nonstopdj
	//. option변경시 죽는 위치, 일단 블록.
	//if(IsBadReadPtr(pWorldSector, sizeof(ApWorldSector)))
		//return FALSE;
	
	if(pWorldSector)	pWorldSector->m_RWLock.LockWriter();

	cur_Water = pSec->WaterList;
	while(cur_Water)
	{
		//@{ 2006/11/16 burumal
		if ( cur_Water->StatusID >= WMAP_HWATER_STATUS_NUM || cur_Water < 0 )
		{
			cur_Water = cur_Water->next;
			continue;
		}
		//@}

		nw_HWater = new HWater;

		Status_ID = nw_HWater->StatusID = cur_Water->StatusID;
		nw_HWater->Height = cur_Water->Height;

		nw_HWater->BS.center.x = cur_Water->pos_origin.x;
		nw_HWater->BS.center.y = cur_Water->pos_origin.y;
		nw_HWater->BS.center.z = cur_Water->pos_origin.z;

		if(cur_Water->TileXLength >= cur_Water->TileZLength)
		{
			nw_HWater->BS.radius = cur_Water->TileXLength * MAP_STEPSIZE;
		}
		else
		{
			nw_HWater->BS.radius = cur_Water->TileZLength * MAP_STEPSIZE;
		}
		
		nw_HWater->TileXLength = cur_Water->TileXLength;
		nw_HWater->TileZLength = cur_Water->TileZLength;
		nw_HWater->XOffset = cur_Water->XOffset;
		nw_HWater->ZOffset = cur_Water->ZOffset;

		MD_SetErrorMessage("MakeHWaterFromWater : Status_ID (%d)\n", Status_ID);

		xres = nw_HWater->TileXLength * m_stHWaterStatus[Status_ID].VertexPerTile;
		zres = nw_HWater->TileZLength * m_stHWaterStatus[Status_ID].VertexPerTile;

		if(xres == 0 || zres == 0) 
		{
			delete nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		height_limit_diff = m_stHWaterStatus[Status_ID].Height_Max - m_stHWaterStatus[Status_ID].Height_Min;
		alpha_limit_diff = m_stHWaterStatus[Status_ID].Alpha_Max - m_stHWaterStatus[Status_ID].Alpha_Min;

		nw_HWater->numTris = xres * zres * 2;
		nw_HWater->numVerts = (xres + 1) * (zres + 1);

		nw_HWater->pvIB = NULL;
		nw_HWater->pvVB = NULL;

		// VB 생성
		if( FAILED(m_pCurD3D9Device->CreateVertexBuffer( nw_HWater->numVerts * sizeof(HWATER_VERTEX),
                                                       0, 0, D3DPOOL_MANAGED, &nw_HWater->pvVB ,NULL) ) )
        {
			nw_HWater->pvVB = NULL;
			nw_HWater->numTris = 0;
			nw_HWater->numVerts = 0;
			TRACE( "HQWater Vertex Buffer 생성실패\n" );
			
			delete	nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		xmin = GetSectorStartX(pSec->SectorXIndex) + nw_HWater->XOffset * MAP_STEPSIZE;
		xmax = xmin + nw_HWater->TileXLength * MAP_STEPSIZE;
		xdist = xmax - xmin;
		zmin = GetSectorStartZ(pSec->SectorZIndex) + nw_HWater->ZOffset * MAP_STEPSIZE;
		zmax = zmin + nw_HWater->TileZLength * MAP_STEPSIZE;
		zdist = zmax - zmin;

		if( FAILED( nw_HWater->pvVB->Lock( 0, nw_HWater->numVerts * sizeof(HWATER_VERTEX), (void**)&pVertices, 0 ) ) )
		{
			if(nw_HWater->pvVB) nw_HWater->pvVB->Release();

			delete	nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		float	ustart = (nw_HWater->XOffset * MAP_STEPSIZE) / MAP_SECTOR_WIDTH;
		float	vstart = (nw_HWater->ZOffset * MAP_STEPSIZE) / MAP_SECTOR_WIDTH;

		float	ii,jj,tu,tv;

		for (i = 0; i <= xres; ++i)
		{
			for (j = 0; j <= zres; ++j)
			{
				ii = i/(float)xres;
				jj = j/(float)zres;

				tu = ustart + ii*xdist / MAP_SECTOR_WIDTH;
				tv = vstart + jj*zdist / MAP_SECTOR_HEIGHT;
				
				pVertices->n[0] = 0.0f;
				pVertices->n[1] = 1.0f;
				pVertices->n[2] = 0.0f;
				pVertices->t[0] = -1.0f;
				pVertices->t[1] = 0.0f;
				pVertices->t[2] = 0.0f;
				pVertices->p[0] = xmin + ii * xdist;
				pVertices->p[1] = cur_Water->Height;
				pVertices->p[2] = zmin + jj * zdist;
				pVertices->tu1 = tu;
				pVertices->tv1 = tv;
				
				cur_height = m_pcsAgcmMap->GetHeight_Lowlevel_HeightOnly(pVertices->p[0],pVertices->p[2]);
				//cur_height = m_pcsAgcmMap->GetHeight(pVertices->p[0],pVertices->p[2]);

				height_diff = cur_Water->Height - cur_height;

				// 물 높이차가 파도 감소치 영역보다 크면
				if(height_diff > m_stHWaterStatus[Status_ID].Height_DecreaseHeight)
				{
					pVertices->p2[0] = 1.0f - m_stHWaterStatus[Status_ID].Height_Max;	
				}
				else if(height_diff < 0.0f)	// 물이 땅아래에 있다.
				{
					pVertices->p2[0] = 1.0f - m_stHWaterStatus[Status_ID].Height_Min;
				}
				else
				{
					pVertices->p2[0] = 1.0f - (m_stHWaterStatus[Status_ID].Height_Min + (height_diff/m_stHWaterStatus[Status_ID].Height_DecreaseHeight) 
										* height_limit_diff);
				}

				if(height_diff > m_stHWaterStatus[Status_ID].Alpha_DecreaseHeight)
				{
					pVertices->p2[1] = m_stHWaterStatus[Status_ID].Alpha_Max;	
				}
				else if(height_diff < 0.0f)	// 물이 땅아래에 있다.
				{
					pVertices->p2[1] = m_stHWaterStatus[Status_ID].Alpha_Min;
				}
				else
				{
					pVertices->p2[1] = m_stHWaterStatus[Status_ID].Alpha_Min + (height_diff/m_stHWaterStatus[Status_ID].Alpha_DecreaseHeight) 
										* alpha_limit_diff;
				}
									
				//pVertices->p2[0] = 0.5f;//(float)(2.0f * max(fabs(ii - 0.5f), fabs(jj - 0.5f)));
							
				++pVertices;	
			}
		}
					
		nw_HWater->pvVB->Unlock();

		// IB 생성
		if(FAILED( m_pCurD3D9Device->CreateIndexBuffer( nw_HWater->numTris*sizeof(WORD)*3,
                                                      0, D3DFMT_INDEX16,
                                                      D3DPOOL_MANAGED, &nw_HWater->pvIB,NULL ) ) )
		{
			if(nw_HWater->pvVB)
			{
				nw_HWater->pvVB->Release();
				nw_HWater->pvVB = NULL;
			}

			nw_HWater->pvIB = NULL;
			nw_HWater->numTris = 0;
			nw_HWater->numVerts = 0;

			TRACE( "HQWater Index Buffer 생성실패\n" );
			delete	nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		if( FAILED( nw_HWater->pvIB->Lock( 0, nw_HWater->numTris * 3 *sizeof(WORD), (void**)&pIndices, 0 ) ) )
		{
			if(nw_HWater->pvVB) nw_HWater->pvVB->Release();
			if(nw_HWater->pvIB) nw_HWater->pvIB->Release();

			delete	nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		for(i = 0; i < xres; ++i)
		{
			for(j = 0; j < zres; ++j)
			{
				*pIndices++ = (WORD) (i      * (zres + 1) + j    );
				*pIndices++ = (WORD) ((i + 1)* (zres + 1) + j + 1);
				*pIndices++ = (WORD) (i      * (zres + 1) + j + 1);
		
				*pIndices++ = (WORD) (i      * (zres + 1) + j    );
				*pIndices++ = (WORD) ((i + 1)* (zres + 1) + j    );
				*pIndices++ = (WORD) ((i + 1)* (zres + 1) + j + 1);
			}
		}

	    if( FAILED( nw_HWater->pvIB->Unlock() ) )
		{
			if(nw_HWater->pvVB) nw_HWater->pvVB->Release();
			if(nw_HWater->pvIB) nw_HWater->pvIB->Release();

			delete	nw_HWater;
			cur_Water = cur_Water->next;
			continue;
		}

		// VB 생성
		/*if( !RwD3D9CreateVertexBuffer( sizeof(HWATER_VERTEX) , nw_HWater->numVerts * sizeof(HWATER_VERTEX) ,
									(void**)&nw_HWater->pvVB , &nw_HWater->iVertexOffset ) )
		{
			nw_HWater->pvVB = NULL;
			nw_HWater->numTris = 0;
			nw_HWater->numVerts = 0;
			TRACE( "HQWater Vertex Buffer 생성실패\n" );
			return FALSE;
		}

		xmin = GetSectorStartX(cur_sec->SectorXIndex) + nw_HWater->XOffset * MAP_STEPSIZE;
		xmax = xmin + nw_HWater->TileXLength * MAP_STEPSIZE;
		xdist = xmax - xmin;
		zmin = GetSectorStartZ(cur_sec->SectorZIndex) + nw_HWater->ZOffset * MAP_STEPSIZE;
		zmax = zmin + nw_HWater->TileZLength * MAP_STEPSIZE;
		zdist = zmax - zmin;

		if( FAILED( nw_HWater->pvVB->Lock( nw_HWater->iVertexOffset, nw_HWater->numVerts * sizeof(HWATER_VERTEX), (void**)&pVertices, 0 ) ) )
			return FALSE;
		
		for (i = 0; i <= xres; ++i)
		{
			for (j = 0; j <= zres; ++j)
			{
				float ii = i/(float)xres;
				float jj = j/(float)zres;

				pVertices->n[0] = 0.0f;
				pVertices->n[1] = -1.0f;
				pVertices->n[2] = 0.0f;
				pVertices->t[0] = 0.0f;
				pVertices->t[1] = 0.0f;
				pVertices->t[2] = 1.0f;
				pVertices->p[0] = xmin + ii * xdist;
				pVertices->p[1] = cur_Water->Height;
				pVertices->p[2] = zmin + jj * zdist;
				pVertices->tu1 = ii;
				pVertices->tv1 = jj;

				pVertices->p2[0] = (float)(2.0f * max(fabs(ii - 0.5f), fabs(jj - 0.5f)));
		
				pVertices++;	
			}
		}
					
		nw_HWater->pvVB->Unlock();

		// IB 생성
		if(!RwD3D9IndexBufferCreate( nw_HWater->numTris * 3 , (void**)&nw_HWater->pvIB))
		//if(FAILED( m_pCurD3D9Device->CreateIndexBuffer( nw_HWater->numTris*sizeof(WORD)*3,
          //                                            D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
            //                                          D3DPOOL_DEFAULT, &nw_HWater->pvIB,NULL ) ) )
		{
			if(nw_HWater->pvVB)
			{
				RwD3D9DestroyVertexBuffer(sizeof(HWATER_VERTEX), nw_HWater->numTris*3*sizeof(HWATER_VERTEX),
									nw_HWater->pvVB , 0);	
				nw_HWater->pvVB = NULL;
			}

			nw_HWater->pvIB = NULL;
			nw_HWater->numTris = 0;
			nw_HWater->numVerts = 0;

			TRACE( "HQWater Index Buffer 생성실패\n" );
			return	FALSE;
		}

		if( FAILED( nw_HWater->pvIB->Lock( 0, nw_HWater->numTris * 3 *sizeof(WORD), (void**)&pIndices, 0 ) ) )
			return FALSE;

		for(i = 0; i < xres; ++i)
		{
			for(j = 0; j < zres; ++j)
			{
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + (i      * (zres + 1) + j    );
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + ((i + 1)* (zres + 1) + j + 1);
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + (i      * (zres + 1) + j + 1);
		
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + (i      * (zres + 1) + j    );
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + ((i + 1)* (zres + 1) + j    );
				*pIndices++ = (WORD) nw_HWater->iVertexOffset + ((i + 1)* (zres + 1) + j + 1);
			}
		}

	    if( FAILED( nw_HWater->pvIB->Unlock() ) )
			return FALSE;*/
		
		nw_HWater->next = pSec->HWaterList;
		pSec->HWaterList = nw_HWater;

		cur_Water = cur_Water->next;
	}

	if(pWorldSector)	pWorldSector->m_RWLock.UnlockWriter();
		
	return TRUE;
}

void	AgcmWater::ChangeHWaterStatus(int status_id)
{
	INT32	i;

	// file추가 검사
	for(i=0;i<m_iHQBumpFileNum;++i)
	{
		if(m_stHQBumpFInfo[i].bReload) // file load
		{
			m_stHQBumpFInfo[i].bReload = FALSE;

			if(m_prHQBumpSrc[i])
			{
				RwTextureDestroy(m_prHQBumpSrc[i]);
				m_prHQBumpSrc[i] = NULL;
			}

			if(m_prHQBump[i])
			{
				m_prHQBump[i]->Release();
				m_prHQBump[i] = NULL;
			}

			m_prHQBumpSrc[i] = RwTextureRead(m_stHQBumpFInfo[i].strFileName, NULL);
			ASSERT( NULL != m_prHQBumpSrc[i] && "워터 텍스쳐가 없습니다!" );
			RwTextureSetFilterMode(m_prHQBumpSrc[i], rwFILTERLINEAR);
		}
	}

	InitBumpMaps();

	for(i=0;i<m_iHQFresnelFileNum;++i)
	{
		if(m_stHQFresnelFInfo[i].bReload) // file 추가
		{
			m_stHQFresnelFInfo[i].bReload = FALSE;

			if(m_prHQFresnel[i])
			{
				RwTextureDestroy(m_prHQFresnel[i]);
				m_prHQFresnel[i] = NULL;
			}

			m_prHQFresnel[i] = RwTextureRead(m_stHQFresnelFInfo[i].strFileName, NULL);
			ASSERT( NULL != m_prHQFresnel[i] && "워터 텍스쳐가 없습니다!" );
			RwTextureSetFilterMode(m_prHQFresnel[i], rwFILTERLINEAR);
		}
	}

	MakeHWaterFromWater();			// 다시 만들자..

	Sector*			cur_sec = m_listSector;
	HWater*			cur_HWater = NULL;

	while(cur_sec)
	{
		cur_HWater = cur_sec->HWaterList;
		while(cur_HWater)
		{
			AddHWaterToRender(cur_HWater);
			//@{ Jaewon 20050712
			// A bug fix! You must set this to 'true'.
			// Otherwise, hwaters added to the render module will not be removed properly
			// when it have to be.
			cur_sec->AddedRenderHWater = true;
			//@} Jaewon
			cur_HWater = cur_HWater->next;
		}

		cur_sec = cur_sec->next;
	}
}

BOOL	AgcmWater::LoadStatusInfoT1FromINI(char*	szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	CHAR			szFileName[256];
	INT32			lIndex = 0;
		
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	VERIFY(csStream.Open(szFile, 0, bDecryption) && "Water INI파일을 읽지 못했습니다");

	// Water File Info를 읽는다.
	{
		csStream.ReadSectionName(0);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, "NUM"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iWaterFileNum);
			}
			else if(!strcmp(szValueName, "FN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&lIndex,szFileName);
				
				ASSERT(strlen(szFileName) < 32);
				strcpy(m_stWaterFInfo[lIndex].strFileName,szFileName);
				
				if(strcmp(szFileName,"none"))
				{
					m_prWaterT[lIndex] = RwTextureRead(szFileName, NULL);
					ASSERT( NULL != m_prWaterT[lIndex] && "워터 텍스쳐가 없습니다!" );

					// 마고자 (2003-10-28 오전 10:50:13) : 없을때 로그남기고 죽지않게 변경.
					if( m_prWaterT[lIndex] )
						RwTextureSetFilterMode(m_prWaterT[lIndex], rwFILTERLINEAR);
				}
			}
		}
	}

	//Wave File Info를 읽는다.
	{
		csStream.ReadSectionName(1);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, "NUM"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iWaveFileNum);
			}
			else if(!strcmp(szValueName, "FN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&lIndex,szFileName);

				ASSERT(strlen(szFileName) < 32);
				strcpy(m_stWaveFInfo[lIndex].strFileName,szFileName);

				if(strcmp(szFileName,"none"))
				{
					m_prWaveT[lIndex] = RwTextureRead(szFileName, NULL);
					// 마고자 (2003-10-28 오전 10:50:13) : 없을때 로그남기고 죽지않게 변경.

					if( m_prWaveT[lIndex] )
						RwTextureSetFilterMode(m_prWaveT[lIndex], rwFILTERLINEAR);
				}
			}
		}
	}

	// water status 읽는다.
	{
		csStream.ReadSectionName(2);
		csStream.SetValueID(-1);
		INT32			lID = 0;

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();
			if(!strcmp(szValueName, "MAX_ID_USED"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iMaxWaterIDUsed);
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &lID);
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WTTYPE))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaterType ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BMODEL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].BMode_L1 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BOPL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].BOP_L1 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_RENDERBMODEL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].RenderBMode_L1 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WTFILEL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaterTexID_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_REDL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].Red_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_GREENL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].Green_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BLUEL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].Blue_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_ALPHAL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].Alpha_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_UL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].U_L1);
				
				m_TexuvL1[lID].x = m_stWaterStatus[lID].U_L1;
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_VL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].V_L1); 

				m_TexuvL1[lID].y = m_stWaterStatus[lID].V_L1;
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_TSIZEL1))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].TileSize_L1); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BMODEL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].BMode_L2 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BLENDSRCL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].BModeSrc_L2 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_BLENDDESTL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].BModeDest_L2 ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WTFILEL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaterTexID_L2); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_UL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].U_L2); 

				m_TexuvL2[lID].x = m_stWaterStatus[lID].U_L2;
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_VL2))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].V_L2); 

				m_TexuvL2[lID].y = m_stWaterStatus[lID].V_L2;
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVFILE))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveTexID); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVWIDTH))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveWidth); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVHEIGHT))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveHeight); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVLIFETIME))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveLifeTime); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVSCALEX))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].WaveScaleX); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVSCALEZ))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stWaterStatus[lID].WaveScaleZ); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVRED))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveRed); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVGREEN))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveGreen); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVBLUE))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveBlue); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVMINNUM))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveMinNum); 
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_WVMAXNUM))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stWaterStatus[lID].WaveMaxNum); 
			}
		}
	}

	return TRUE;
}

BOOL	AgcmWater::SaveStatusInfoT1ToINI(char*		szFileName, BOOL bEncryption)
{
	CHAR szValue[255];

	INT32				i;
	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))
		return FALSE;
	
	if(!csStream.SetSection("WaterTextures"))
		return FALSE;

	sprintf(szValue, "%d", m_iWaterFileNum);
	if(!csStream.WriteValue("NUM", szValue))
		return FALSE;

	for(i=0;i<m_iWaterFileNum;++i)
	{
		if(!strcmp(m_stWaterFInfo[i].strFileName,"none"))
		{
			sprintf(szValue, "%d:none", i);
		}
		else
		{
			sprintf(szValue, "%d:%s", i, RwTextureGetName(m_prWaterT[i]));
		}
	
		if(!csStream.WriteValue("FN", szValue))
			return FALSE;
	}

	if(!csStream.SetSection("WaveTextures"))
		return FALSE;

	sprintf(szValue, "%d", m_iWaveFileNum);
	if(!csStream.WriteValue("NUM", szValue))
		return FALSE;
	for(i=0;i<m_iWaveFileNum;++i)
	{
		if(!strcmp(m_stWaveFInfo[i].strFileName,"none"))
		{
			sprintf(szValue, "%d:none", i);
		}
		else
		{
			sprintf(szValue, "%d:%s", i, RwTextureGetName(m_prWaveT[i]));
		}

		if(!csStream.WriteValue("FN", szValue))
			return FALSE;
	}

	if(!csStream.SetSection("Setting"))
		return FALSE;

	sprintf(szValue, "%d", m_iMaxWaterIDUsed);
	if(!csStream.WriteValue("MAX_ID_USED", szValue))
		return FALSE;

	for(i=0;i<WMAP_STATUS_NUM;++i)
	{
		sprintf(szValue, "%d", i);
		if(!csStream.WriteValue(WATER_INI_STATUS_ID, szValue))
		return FALSE;
		
		sprintf(szValue, "%d", m_stWaterStatus[i].WaterType);
		if(!csStream.WriteValue(WATER_INI_STATUS_WTTYPE, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].BMode_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_BMODEL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].BOP_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_BOPL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].RenderBMode_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_RENDERBMODEL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaterTexID_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_WTFILEL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].Red_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_REDL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].Green_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_GREENL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].Blue_L1);
		if(!csStream.WriteValue(WATER_INI_STATUS_BLUEL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].Alpha_L1 );
		if(!csStream.WriteValue(WATER_INI_STATUS_ALPHAL1, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].U_L1 );
		if(!csStream.WriteValue(WATER_INI_STATUS_UL1, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].V_L1 );
		if(!csStream.WriteValue(WATER_INI_STATUS_VL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].TileSize_L1 );
		if(!csStream.WriteValue(WATER_INI_STATUS_TSIZEL1, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].BMode_L2);
		if(!csStream.WriteValue(WATER_INI_STATUS_BMODEL2, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].BModeSrc_L2);
		if(!csStream.WriteValue(WATER_INI_STATUS_BLENDSRCL2, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].BModeDest_L2);
		if(!csStream.WriteValue(WATER_INI_STATUS_BLENDDESTL2, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaterTexID_L2);
		if(!csStream.WriteValue(WATER_INI_STATUS_WTFILEL2, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].U_L2 );
		if(!csStream.WriteValue(WATER_INI_STATUS_UL2, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].V_L2 );
		if(!csStream.WriteValue(WATER_INI_STATUS_VL2, szValue))
		return FALSE;
		
		sprintf(szValue, "%d", m_stWaterStatus[i].WaveTexID);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVFILE, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveWidth);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVWIDTH, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveHeight);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVHEIGHT, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveLifeTime);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVLIFETIME, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].WaveScaleX);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVSCALEX, szValue))
		return FALSE;

		print_compact_format(szValue, "%f", m_stWaterStatus[i].WaveScaleZ);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVSCALEZ, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveRed);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVRED, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveGreen);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVGREEN, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveBlue);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVBLUE, szValue))
		return FALSE;

		sprintf(szValue, "%d", m_stWaterStatus[i].WaveMinNum);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVMINNUM, szValue))
		return FALSE;
		
		sprintf(szValue, "%d", m_stWaterStatus[i].WaveMaxNum);
		if(!csStream.WriteValue(WATER_INI_STATUS_WVMAXNUM, szValue))
		return FALSE;
	}
	
	return csStream.Write(szFileName, 0, bEncryption);
}

BOOL	AgcmWater::LoadHWaterStatusFromINI(char*	szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	if(!m_bEnableHWater)
		return TRUE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];

	CHAR			szFileName[256];
	INT32			lIndex = 0;
	INT32			lFileNum = 0;
	//INT32				i;
		
	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	VERIFY(csStream.Open(szFile, 0, bDecryption) && "High Quality Water INI파일을 읽지 못했습니다");

	// BumpMap
	{
		csStream.ReadSectionName(0);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, "NUM"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iHQBumpFileNum);
			}
			else if(!strcmp(szValueName, "FN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&lIndex,szFileName);

				strcpy(m_stHQBumpFInfo[lIndex].strFileName,szFileName);

				if(strcmp(szFileName,"none"))
				{
					#ifndef _DEBUG
					try
					#endif
					{
						m_prHQBumpSrc[lIndex] = RwTextureRead(szFileName, NULL);
				
						if( m_prHQBumpSrc[lIndex] )
							RwTextureSetFilterMode(m_prHQBumpSrc[lIndex], rwFILTERLINEAR);
						else
						{
							MD_SetErrorMessage( "(HWaterStatus.ini) Water FresnelMap Not Find! (%s) -> 후야사마에게 확인\n" , szFileName );
						}
					}
					#ifndef _DEBUG
					catch( ... )
					{
						// 마고자 (2005-06-03 오후 3:53:18) : 
						// 이건 어떻게 감당해야하지 ;;
						m_prHQBumpSrc[lIndex] = NULL;
					}
					#endif
				}
			}
		}
	}

	if(!InitBumpMaps())		//heightmap convert to bump map(u8v8)
	{
		m_bEnableHWater = FALSE;
		return FALSE;
	}

	//FresnelMap
	{
		csStream.ReadSectionName(1);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, "NUM"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iHQFresnelFileNum);
			}
			else if(!strcmp(szValueName, "FN"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue,"%d:%s",&lIndex,szFileName);

				strcpy(m_stHQFresnelFInfo[lIndex].strFileName,szFileName);
				
				#ifndef _DEBUG
				try
				#endif
				{
					m_prHQFresnel[lIndex] = RwTextureRead(szFileName, NULL);
				
					if( m_prHQFresnel[lIndex] )
						RwTextureSetFilterMode(m_prHQFresnel[lIndex], rwFILTERLINEAR);
				}
				#ifndef _DEBUG
				catch( ... )
				{
					// 마고자 (2005-06-03 오후 3:53:18) : 
					// 이건 어떻게 감당해야하지 ;;
					m_prHQFresnel[lIndex] = NULL;
				}
				#endif
			}
		}
	}

	//Hwater status 읽는다.
	{
		csStream.ReadSectionName(2);
		csStream.SetValueID(-1);
		INT32			lID = 0;

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();
			if(!strcmp(szValueName, "MAX_ID_USED"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_iMaxHWaterIDUsed);
			}
			else if(!strcmp(szValueName, WATER_INI_STATUS_ID))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &lID);
			}
			else if(!strcmp(szValueName, "Red"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Red);
			}
			else if(!strcmp(szValueName, "Green"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Green);
			}
			else if(!strcmp(szValueName, "Blue"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Blue);
			}
			else if(!strcmp(szValueName, "DirX"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f:%f:%f:%f", &m_stHWaterStatus[lID].DirX0,&m_stHWaterStatus[lID].DirX1,
											&m_stHWaterStatus[lID].DirX2,&m_stHWaterStatus[lID].DirX3);
			}
			else if(!strcmp(szValueName, "DirY"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f:%f:%f:%f", &m_stHWaterStatus[lID].DirY0,&m_stHWaterStatus[lID].DirY1,
											&m_stHWaterStatus[lID].DirY2,&m_stHWaterStatus[lID].DirY3);
			}
			else if(!strcmp(szValueName, "Height"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f:%f:%f:%f", &m_stHWaterStatus[lID].Height0,&m_stHWaterStatus[lID].Height1,
											&m_stHWaterStatus[lID].Height2,&m_stHWaterStatus[lID].Height3);
			}
			else if(!strcmp(szValueName, "Speed"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f:%f:%f:%f", &m_stHWaterStatus[lID].Speed0,&m_stHWaterStatus[lID].Speed1,
											&m_stHWaterStatus[lID].Speed2,&m_stHWaterStatus[lID].Speed3);
			}
			else if(!strcmp(szValueName, "TexX0"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].TexX0);
			}
			else if(!strcmp(szValueName, "TexX1"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].TexX1);
			}
			else if(!strcmp(szValueName, "TexY0"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].TexY0);
			}
			else if(!strcmp(szValueName, "TexY1"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].TexY1);
			}
			else if(!strcmp(szValueName, "ReflectionWeight"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].ReflectionWeight);
			}
			else if(!strcmp(szValueName, "AlphaMin"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Alpha_Min);
			}
			else if(!strcmp(szValueName, "AlphaMax"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Alpha_Max);
			}
			else if(!strcmp(szValueName, "AlphaDecreaseHeight"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Alpha_DecreaseHeight);
			}
			else if(!strcmp(szValueName, "WaveMin"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Height_Min);
			}
			else if(!strcmp(szValueName, "WaveMax"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Height_Max);
			}
			else if(!strcmp(szValueName, "WaveDecreaseHeight"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].Height_DecreaseHeight);
			}
			else if(!strcmp(szValueName, "VertexPerTile"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stHWaterStatus[lID].VertexPerTile);
			}
			else if(!strcmp(szValueName, "BumpTexID"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stHWaterStatus[lID].BumpTexID);
			}
			else if(!strcmp(szValueName, "FresnelTexID"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stHWaterStatus[lID].FresnelTexID);
			}
			//@{ Jaewon 20050706
			// ;)
			else if(!strcmp(szValueName, "BaseR"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].BaseR);
			}
			else if(!strcmp(szValueName, "BaseG"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].BaseG);
			}
			else if(!strcmp(szValueName, "BaseB"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f", &m_stHWaterStatus[lID].BaseB);
			}
			//@} Jaewon
			//@{ Jaewon 20050705
			// ;)
			else if(!strcmp(szValueName, "Shader14ID"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stHWaterStatus[lID].Shader14ID);
			}
			else if(!strcmp(szValueName, "Shader11ID"))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &m_stHWaterStatus[lID].Shader11ID);
			}
			//@} Jaewon
		}
	}

	return TRUE;
}

BOOL	AgcmWater::SaveHWaterStatusToINI(char*		szFileName, BOOL bEncryption)
{
	CHAR szValue[255];

	INT32				i;
	ApModuleStream csStream;
	if(!csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE))
		return FALSE;
	
	if(!csStream.SetSection("BumpMap Textures"))
		return FALSE;

	sprintf(szValue, "%d", m_iHQBumpFileNum);
	if(!csStream.WriteValue("NUM", szValue))
		return FALSE;

	for(i=0;i<m_iHQBumpFileNum;++i)
	{
		if(!strcmp(m_stHQBumpFInfo[i].strFileName,"none" ) ||
			NULL == m_prHQBumpSrc[i] ) // 없으면 날려버림..
		{
			sprintf(szValue, "%d:none", i);
		}
		else
		{
			sprintf(szValue, "%d:%s", i, RwTextureGetName(m_prHQBumpSrc[i]));
		}

		if(!csStream.WriteValue("FN", szValue))
			return FALSE;
	}

	if(!csStream.SetSection("FresnelMap Textures"))
		return FALSE;

	sprintf(szValue, "%d", m_iHQFresnelFileNum);
	if(!csStream.WriteValue("NUM", szValue))
		return FALSE;
	for(i=0;i<m_iHQFresnelFileNum;++i)
	{
		if(!strcmp(m_stHQFresnelFInfo[i].strFileName,"none") ||
			NULL == m_prHQFresnel[i] ) // 없으면 날려버림
		{
			sprintf(szValue, "%d:none", i);
		}
		else
		{
			sprintf(szValue, "%d:%s", i, RwTextureGetName(m_prHQFresnel[i]));
		}

		if(!csStream.WriteValue("FN", szValue))
			return FALSE;
	}

	if(!csStream.SetSection("Setting"))
		return FALSE;

	sprintf(szValue, "%d", m_iMaxHWaterIDUsed);
	if(!csStream.WriteValue("MAX_ID_USED", szValue))
		return FALSE;

	for(i=0;i<WMAP_STATUS_NUM;++i)
	{
		sprintf(szValue, "%d", i);
		if(!csStream.WriteValue(WATER_INI_STATUS_ID, szValue))	return FALSE;
		
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Red);
		if(!csStream.WriteValue("Red", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Green);
		if(!csStream.WriteValue("Green", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Blue);
		if(!csStream.WriteValue("Blue", szValue))		return FALSE;
		
		print_compact_format(szValue, "%f:%f:%f:%f", m_stHWaterStatus[i].DirX0,m_stHWaterStatus[i].DirX1,
										m_stHWaterStatus[i].DirX2,m_stHWaterStatus[i].DirX3);
		if(!csStream.WriteValue("DirX", szValue))		return FALSE;
		
		print_compact_format(szValue, "%f:%f:%f:%f", m_stHWaterStatus[i].DirY0,m_stHWaterStatus[i].DirY1,
										m_stHWaterStatus[i].DirY2,m_stHWaterStatus[i].DirY3);
		if(!csStream.WriteValue("DirY", szValue))		return FALSE;
		
		print_compact_format(szValue, "%f:%f:%f:%f", m_stHWaterStatus[i].Height0,m_stHWaterStatus[i].Height1,
										m_stHWaterStatus[i].Height2,m_stHWaterStatus[i].Height3);
		if(!csStream.WriteValue("Height", szValue))		return FALSE;
		
		print_compact_format(szValue, "%f:%f:%f:%f", m_stHWaterStatus[i].Speed0,m_stHWaterStatus[i].Speed1,
										m_stHWaterStatus[i].Speed2,m_stHWaterStatus[i].Speed3);
		if(!csStream.WriteValue("Speed", szValue))		return FALSE;
		
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].TexX0);
		if(!csStream.WriteValue("TexX0", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].TexX1);
		if(!csStream.WriteValue("TexX1", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].TexY0);
		if(!csStream.WriteValue("TexY0", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].TexY1);
		if(!csStream.WriteValue("TexY1", szValue))		return FALSE;

		print_compact_format(szValue, "%f", m_stHWaterStatus[i].ReflectionWeight);
		if(!csStream.WriteValue("ReflectionWeight", szValue))		return FALSE;

		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Alpha_Min);
		if(!csStream.WriteValue("AlphaMin", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Alpha_Max);
		if(!csStream.WriteValue("AlphaMax", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Alpha_DecreaseHeight);
		if(!csStream.WriteValue("AlphaDecreaseHeight", szValue))		return FALSE;

		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Height_Min);
		if(!csStream.WriteValue("WaveMin", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Height_Max);
		if(!csStream.WriteValue("WaveMax", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].Height_DecreaseHeight);
		if(!csStream.WriteValue("WaveDecreaseHeight", szValue))		return FALSE;

		print_compact_format(szValue, "%d", m_stHWaterStatus[i].VertexPerTile);
		if(!csStream.WriteValue("VertexPerTile", szValue))		return FALSE;

		print_compact_format(szValue, "%d", m_stHWaterStatus[i].BumpTexID);
		if(!csStream.WriteValue("BumpTexID", szValue))		return FALSE;
		print_compact_format(szValue, "%d", m_stHWaterStatus[i].FresnelTexID);
		if(!csStream.WriteValue("FresnelTexID", szValue))		return FALSE;

		//@{ Jaewon 20050706
		// ;)
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].BaseR);
		if(!csStream.WriteValue("BaseR", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].BaseG);
		if(!csStream.WriteValue("BaseG", szValue))		return FALSE;
		print_compact_format(szValue, "%f", m_stHWaterStatus[i].BaseB);
		if(!csStream.WriteValue("BaseB", szValue))		return FALSE;
		//@} Jaewon

		//@{ Jaewon 20050705
		// ;)
		print_compact_format(szValue, "%d", m_stHWaterStatus[i].Shader14ID);
		if(!csStream.WriteValue("Shader14ID", szValue))		return FALSE;
		print_compact_format(szValue, "%d", m_stHWaterStatus[i].Shader11ID);
		if(!csStream.WriteValue("Shader11ID", szValue))		return FALSE;
		//@} Jaewon
	}
	
	return csStream.Write(szFileName, 0, bEncryption);
}

/*HANDLE		AgcmWater::GetHandle(INT32 lx,INT32 lz)
{
	HANDLE		res = NULL;

//	m_csCSection.Lock();		// BackThread만 접근 가능하다..

	BOOL			bFind = FALSE;
	WaterFOList*	cur_FO = m_listFO;
	WaterFOList*	bef_FO = NULL;
	while(cur_FO)
	{
		if(cur_FO->lx == lx && cur_FO->lz == lz)
		{
			res = cur_FO->fd;
			bFind = TRUE;
			break;
		}

		bef_FO = cur_FO;
		cur_FO = cur_FO->next;
	}

	if(bFind)
	{
		if(cur_FO != m_listFO)
		{
			bef_FO->next = cur_FO->next;

			cur_FO->next = m_listFO;
			m_listFO = cur_FO;
		}

		//m_csCSection.Unlock();
		return res;
	}

	//없었으므로 만든다.
	if(m_iFOCount >= WATER_FO_NUM)
	{
		// 맨뒤 원소 삭제
		cur_FO = m_listFO;
		while(cur_FO && cur_FO->next)
		{
			bef_FO = cur_FO;
			cur_FO = cur_FO->next;
		}

		bef_FO->next = cur_FO->next;

		CloseHandle(cur_FO->fd);
		delete cur_FO;
	}
	else 
	{
		++m_iFOCount;
	}

	WaterFOList*		nw_FO = new WaterFOList;
	nw_FO->lx = lx;
	nw_FO->lz = lz;

	INT32	x_arrayindex = SectorIndexToArrayIndexX(lx);
	INT32	z_arrayindex = SectorIndexToArrayIndexZ(lz);
	INT32	division_index = GetDivisionIndex( x_arrayindex , z_arrayindex );
		
	char		fstr[100];
	memset(fstr,'\0',100);
	sprintf(fstr,"world\\water\\WT%d.dat",division_index);
	//sprintf(fstr,"world\\water\\WT%dx%d.dat",lx,lz);
		
	// INVALID_HANDLE이래도 저장 ..
	res = CreateFile(fstr,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
	nw_FO->fd = res;

	nw_FO->next = m_listFO;
	m_listFO = nw_FO;

//	m_csCSection.Unlock();

	return res;
}

void	AgcmWater::CloseHandleList(INT32	lx,INT32	lz)
{
	WaterFOList*	cur_FO = m_listFO;
	WaterFOList*	bef_FO = NULL;
	while(cur_FO)
	{
		if(cur_FO->lx == lx && cur_FO->lz == lz)
		{
			if(cur_FO == m_listFO)
			{
				m_listFO = cur_FO->next;
			}
			else
			{
				bef_FO->next = cur_FO->next;
			}

			--m_iFOCount;

			CloseHandle(cur_FO->fd);

			delete cur_FO;
			return;
		}

		bef_FO = cur_FO;
		cur_FO = cur_FO->next;
	}
}

void	AgcmWater::OnLoadSectorWaterLevelCallback		( ApWorldSector* pWSector , INT32 nDetail )
{
	ASSERT( NULL != AgcmWater::m_pThis );

	AgcmWater* pThis = AgcmWater::m_pThis;

	INT32	six = pWSector->GetIndexX();
	INT32	siz = pWSector->GetIndexZ();

	BOOL	bFind = FALSE;

	Sector*			cur_sec = pThis->m_listSector;
	Wave*			cur_Wave = NULL;

	pThis->m_csCSection.Lock();
	while(cur_sec)
	{
		if(cur_sec->SectorXIndex == six && cur_sec->SectorZIndex == siz)
		{
			bFind = TRUE;
			if(cur_sec->Detail != nDetail) // Change Detail!
			{
				if(nDetail == SECTOR_LOWDETAIL)
				{
					if(cur_sec->AddedRenderWave)
					{
						cur_Wave = cur_sec->WaveList;

						while(cur_Wave)
						{
							pThis->JobQueueAdd( WATER_JOB_REMOVEWAVE , cur_sec, cur_Wave);
							cur_Wave = cur_Wave->next;
						}
					}
				}
				else if(nDetail == SECTOR_HIGHDETAIL)
				{
					if(!cur_sec->AddedRenderWave)
					{
						cur_Wave = cur_sec->WaveList;

						while(cur_Wave)
						{
							pThis->JobQueueAdd( WATER_JOB_ADDWAVE , cur_sec, cur_Wave);
							cur_Wave = cur_Wave->next;
						}
					}

					if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH && pThis->m_bEnableHWater)
					{
						pThis->JobQueueAdd(WATER_JOB_MAKEANDADDHWATER, cur_sec, NULL);
					}
				}
			}
		}

		cur_sec = cur_sec->next;
	}
	pThis->m_csCSection.Unlock();

	if(bFind == FALSE)
	{
		// 없었으므로 file에서 읽자
		INT32 lsix = six / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH; // MAP_DEFAULT_DEPTH 단위로 증가하기 위해
		INT32 lsiz = siz / MAP_DEFAULT_DEPTH * MAP_DEFAULT_DEPTH;

		if(six < 0 && six % MAP_DEFAULT_DEPTH != 0) lsix -= MAP_DEFAULT_DEPTH;
		if(siz < 0 && siz % MAP_DEFAULT_DEPTH != 0) lsiz -= MAP_DEFAULT_DEPTH;

		INT32 loffx = six - lsix;
		INT32 loffz = siz - lsiz;

		FLOAT start_x = pWSector->GetXStart();
		FLOAT start_z = pWSector->GetZStart();

		HANDLE fd = pThis->GetHandle(lsix,lsiz);
		if(fd == INVALID_HANDLE_VALUE)
		{
		//	ASSERT(!"파일을 읽는데 실패하였습니다.water");
			return;
		}

		DWORD FP = 0;
		INT32 foffset = 0;
		DWORD	foffset2 = 0;
		INT32	iLoadSize = 0;
		INT16*	pTemp_i16;
		RwV3d*	pTemp_v3;

		foffset = loffz * MAP_DEFAULT_DEPTH * 4  + loffx * 4;

		SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
		ReadFile(fd,&foffset,sizeof(foffset),&FP,NULL);

		if(loffx+1 >= MAP_DEFAULT_DEPTH)
		{
			if(loffz+1 >= MAP_DEFAULT_DEPTH)
			{
				foffset2 = GetFileSize(fd,NULL);
			}
			else
			{
				foffset2 = (loffz + 1) * MAP_DEFAULT_DEPTH * 4;

				SetFilePointer(fd,foffset2,NULL,FILE_BEGIN);
				ReadFile(fd,&foffset2,sizeof(foffset2),&FP,NULL);
			}
		}
		else
		{
			foffset2 = loffz * MAP_DEFAULT_DEPTH * 4 + (loffx + 1) * 4;

			SetFilePointer(fd,foffset2,NULL,FILE_BEGIN);
			ReadFile(fd,&foffset2,sizeof(foffset2),&FP,NULL);
		}

		iLoadSize = foffset2 - foffset;

		DWORD*		loadBuffer = new DWORD[iLoadSize/4];
		INT32		iLoadIndex = 0;
		INT32	root_num = 0;
		DWORD	dwError;

		SetFilePointer(fd,foffset,NULL,FILE_BEGIN);
		BOOL	bRead = ReadFile(fd,loadBuffer,iLoadSize,&FP,NULL);
		if (!bRead)
		{
			dwError = GetLastError();
			delete		[]loadBuffer;
			return;
		}

		pThis->JobQueueRemove(six,siz);

		Sector*	nw_sec = new Sector;
		nw_sec->SectorXIndex = six;
		nw_sec->SectorZIndex = siz;
		nw_sec->Detail = nDetail;
		nw_sec->WaterList = NULL;
		nw_sec->WaveList = NULL;
		nw_sec->HWaterList = NULL;

		nw_sec->AddedRenderWave = false;
		nw_sec->AddedRenderWater = false;
		nw_sec->AddedRenderHWater = false;

		INT32 WaterNum = 0;
		INT32 WaveNum = 0;
		FLOAT WaterHeight = 0;

		WaterNum = ((INT32*)loadBuffer)[iLoadIndex++];

		Water*	nw_Water = NULL;
		Wave*	nw_Wave = NULL;

		for(INT32 i=0;i<WaterNum;++i)		//	Water추가
		{
			nw_Water = new Water;

			nw_Water->StatusID = ((INT32*)loadBuffer)[iLoadIndex++];

			pTemp_i16 = (INT16*)&(loadBuffer[iLoadIndex]);
			nw_Water->XOffset = *pTemp_i16;
			nw_Water->ZOffset = *(pTemp_i16+1);
			nw_Water->TileXLength = *(pTemp_i16+2);
			nw_Water->TileZLength = *(pTemp_i16+3);
			iLoadIndex += 2;

			nw_Water->Height = ((FLOAT*)loadBuffer)[iLoadIndex++];
			WaterHeight = nw_Water->Height;

			nw_Water->pos_origin.x = start_x + nw_Water->XOffset *MAP_STEPSIZE + (float)nw_Water->TileXLength * (float)MAP_STEPSIZE*0.5f;
			nw_Water->pos_origin.y = nw_Water->Height;
			nw_Water->pos_origin.z = start_z + nw_Water->ZOffset *MAP_STEPSIZE + (float)nw_Water->TileZLength * (float)MAP_STEPSIZE*0.5f;
			
			pThis->JobQueueAdd( WATER_JOB_WATERADDTOSECTOR , nw_sec, nw_Water );
			
			if(pThis->m_iWaterDetail == WATER_DETAIL_HIGH && pThis->m_bEnableHWater)
			{
				pThis->MakeWater(nw_Water,nw_Water->StatusID,FALSE);

				pThis->JobQueueAdd(WATER_JOB_MAKEANDADDHWATER, nw_sec, NULL);
			}
			else
			{
				pThis->MakeWater(nw_Water,nw_Water->StatusID,FALSE);
				pThis->JobQueueAdd ( WATER_JOB_ADDWATER , nw_sec , nw_Water );
			}
		}

		WaveNum = ((INT32*)loadBuffer)[iLoadIndex++];
		
		for(i=0;i<WaveNum;++i)		// L2추가
		{
			nw_Wave = new Wave;

			nw_Wave->StatusID = ((INT32*)loadBuffer)[iLoadIndex++];

			pTemp_i16 = (INT16*)&(loadBuffer[iLoadIndex]);
			nw_Wave->XOffset = *pTemp_i16;
			nw_Wave->ZOffset = *(pTemp_i16+1);
			++iLoadIndex;

			nw_Wave->Width = ((INT32*)loadBuffer)[iLoadIndex++];
			nw_Wave->Height = ((INT32*)loadBuffer)[iLoadIndex++];
			nw_Wave->SpawnCount = ((INT32*)loadBuffer)[iLoadIndex++];
			nw_Wave->YRotAngle = ((FLOAT*)loadBuffer)[iLoadIndex++];
			nw_Wave->TranslationSpeed = ((FLOAT*)loadBuffer)[iLoadIndex++];

			pTemp_v3 = (RwV3d*)&(loadBuffer[iLoadIndex]);
			nw_Wave->pos_origin = *pTemp_v3;
			iLoadIndex += 3;

			pThis->WaveDataSet(nw_Wave);
			
			if(nDetail == SECTOR_HIGHDETAIL)
			{
				pThis->MakeWave(nw_Wave,nw_Wave->StatusID,FALSE);
				pThis->JobQueueAdd(WATER_JOB_ADDWAVE ,nw_sec , nw_Wave);
			}
			else
			{
				pThis->MakeWave(nw_Wave,nw_Wave->StatusID,FALSE);
			}
		
			pThis->JobQueueAdd(WATER_JOB_WAVEADDTOSECTOR,nw_sec , nw_Wave);
		}

		pThis->JobQueueAdd( WATER_JOB_SECTORADDTOSECTORLIST , nw_sec, NULL);

		delete []loadBuffer;
	}
}
*/

BOOL	AgcmWater::CBDeleteAllQueue ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmWater *	pThis = (AgcmWater *) pClass;

	pThis->JobQueueRemoveAll();

	return TRUE;
}

//@{ 2006/11/17 burumal
///*
#define fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_WATER		(0.0f)

#ifdef _DEBUG
FLOAT g_fWaterDistCorrectionValue = fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_WATER;
#endif

BOOL	AgcmWater::CB_DistCorrect( PVOID pDistFloat, PVOID pNull1, PVOID pNull2 )
{	
	pNull1;
	pNull2;

	if ( pDistFloat == NULL )
		return FALSE;

#ifdef _DEBUG
	*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + g_fWaterDistCorrectionValue;
#else
	*((FLOAT*) pDistFloat) = *((FLOAT*) pDistFloat) + fAPPEARANCE_DISTANCE_CORRECTION_VALUE_FOR_WATER;
#endif
	// -값이 되는것은 상관없다

	return TRUE;
}
//*/
//@}