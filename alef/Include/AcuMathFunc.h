// AcuMathFunc.h
// -----------------------------------------------------------------------------
//                      __  __       _   _     ______                      _     
//     /\              |  \/  |     | | | |   |  ____|                    | |    
//    /  \    ___ _   _| \  / | __ _| |_| |__ | |__   _   _ _ __   ___    | |__  
//   / /\ \  / __| | | | |\/| |/ _` | __| '_ \|  __| | | | | '_ \ / __|   | '_ \ 
//  / ____ \| (__| |_| | |  | | (_| | |_| | | | |    | |_| | | | | (__  _ | | | |
// /_/    \_\\___|\__,_|_|  |_|\__,_|\__|_| |_|_|     \__,_|_| |_|\___|(_)|_| |_|
//                                                                               
//                                                                               
//
// util math functions
//
// -----------------------------------------------------------------------------
// Originally created on 02/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#ifndef	_H_ACUMATHFUNC_20050219
#define _H_ACUMATHFUNC_20050219

#include "ApDefine.h"
#include "MagDebug.h"
#include "rwcore.h"

#ifndef NSACUMATH_BEGIN
#define NSACUMATH_BEGIN	namespace NSAcuMath {
#define NSACUMATH_END	};
#define USING_ACUMATH	using namespace NSAcuMath;
#define NSACUMATH		NSAcuMath
#endif

NSACUMATH_BEGIN


// /////////////////////////////////////////////////////////////////////////////
// float inline func
// /////////////////////////////////////////////////////////////////////////////
union FNI
{
	float f;
	int	i;

	FNI(int nval=((23+127)<<23) + (1<<22)) : i(nval)
	{
		ASSERT( sizeof(float) == 4 && sizeof(int) == 4 );
	}
	FNI(float fval) : f(fval)
	{
		ASSERT( sizeof(float) == 4 && sizeof(int) == 4 );
	}

	BOOL IsMinus()const
	{
		return DEF_ISMINUS4(i);
	}
};
inline int floatToInt( const float f )
{
	FNI tmp(f);
	const FNI	bias( tmp.IsMinus() ? (((23+127)<<23)+(1<<22)) : ((23+127)<<23));

	tmp.f += bias.f;
	return (tmp.i - bias.i);
}
inline float floatAbs(const float f)
{
	FNI tmp(f);
	tmp.i &= 0x7fffffff;
	return tmp.f;
}

// /////////////////////////////////////////////////////////////////////////////
// float inline func
// /////////////////////////////////////////////////////////////////////////////
// clamp
template<typename T> inline void T_CLAMP(T& out, const T& min, const T& max)
{
	(out<min) ? (out=min) : ( (out>max) ? out=max : out );
}

// swap
template<typename T> inline void T_SWAP(T& a, T& b)
{
	T tmp(a);
	a = b;
	b = tmp;
}
template<> inline void T_SWAP(INT32& a, INT32& b)
{
	SWAP(a,b);
}
template<> inline void T_SWAP(UINT32& a, UINT32& b)
{
	SWAP(a,b);
}
template<> inline void T_SWAP(float& a, float& b)
{
	SWAP(*(int*)(&a),*(int*)(&b));
}

// min / max
template<typename T> inline const T& T_MIN(const T& a, const T& b)
{
	return MIN(a,b);
}
template<typename T> inline const T& T_MAX(const T& a, const T& b)
{
	return MAX(a,b);
}

// Is Odd
template <typename T> inline BOOL T_ISODD(const T& t)
{
	return DEF_ISODD(t);
}

// Is Minus
template <typename T> inline BOOL T_ISMINUS4(const T t)
{
	ASSERT( sizeof(T) == 4 );
	return DEF_ISMINUS4(t);
}
template <> inline BOOL T_ISMINUS4(const float f)
{
	ASSERT( sizeof(float) == 4 );
	FNI tmp(f);
	return tmp.IsMinus();
}
template <typename T> inline BOOL T_ISMINUS2(const T t)
{
	ASSERT( sizeof(T) == 2 );
	return DEF_ISMINUS2(t);
}
template <typename T> inline BOOL T_ISMINUS1(const T t)
{
	ASSERT( sizeof(T) == 1 );
	return DEF_ISMINUS1(t);
}


// /////////////////////////////////////////////////////////////////////////////
// linear interpolation
// /////////////////////////////////////////////////////////////////////////////
template < class T > inline T* LinearIntp( T* pOut
						, const T* pVal1
						, const T* pVal2
						, const RwReal fCof )// (time[key] - time[1]) / (time[2] - time[1])
											 // (time[1] <= time[key] && time[key] <= time[2])
{
	ASSERT( pOut && "@ AcuMath::LinearIntp(T)" );
	ASSERT( pVal1 && "@ AcuMath::LinearIntp(T)" );
	ASSERT( pVal2 && "@ AcuMath::LinearIntp(T)" );
	ASSERT( fCof >= 0.f && "@ AcuMath::LinearIntp(T)" );
	ASSERT( fCof <= 1.f && "@ AcuMath::LinearIntp(T)" );	

	*pOut	= *pVal1 + (T)( ( (RwReal)(*pVal2) - (RwReal)(*pVal1) ) * fCof );
	return pOut;
}

template <> inline RwReal* LinearIntp( RwReal* pOut, const RwReal* pVal1, const RwReal* pVal2, const RwReal fCof )
{
	ASSERT( pOut && "@ AcuMath::LinearIntp(RwReal)" );
	ASSERT( pVal1 && "@ AcuMath::LinearIntp(RwReal)" );
	ASSERT( pVal2 && "@ AcuMath::LinearIntp(RwReal)" );
	ASSERT( fCof >= 0.f && "@ AcuMath::LinearIntp(RwReal)" );
	ASSERT( fCof <= 1.f && "@ AcuMath::LinearIntp(RwReal)" );		

	*pOut	= *pVal1 + ( (*pVal2) - (*pVal1) ) * fCof;
	return pOut;
};
template <> inline RwV3d* LinearIntp( RwV3d* pOut, const RwV3d* pVal1, const RwV3d* pVal2, const RwReal fCof )
{
	ASSERT( pOut && "@ AcuMath::LinearIntp(RwV3d)" );
	ASSERT( pVal1 && "@ AcuMath::LinearIntp(RwV3d)" );
	ASSERT( pVal2 && "@ AcuMath::LinearIntp(RwV3d)" );
	ASSERT( fCof >= 0.f && "@ AcuMath::LinearIntp(RwV3d)" );
	ASSERT( fCof <= 1.f && "@ AcuMath::LinearIntp(RwV3d)" );	

	RwV3d	vdiff;
	RwV3dSub	( &vdiff, pVal2, pVal1 );
	RwV3dScale	( &vdiff, &vdiff, fCof );
	RwV3dAdd	( pOut, pVal1, &vdiff );	
	return pOut;
};
template <> RwRGBA* LinearIntp( RwRGBA* pOut, const RwRGBA* pVal1, const RwRGBA* pVal2, const RwReal fCof );


// /////////////////////////////////////////////////////////////////////////////
// rotation axis and angle between 2 vectors
//
// warning : MUST BE VALIDATION CHECKING!
//
// input : uvFrom and uvTo must be normalized
// output : puvAxis normalized and prad in radian
// return value : TRUE - available angle and axis, FALSE - invalid axis
// /////////////////////////////////////////////////////////////////////////////
BOOL AngleFrom2Vec(RwV3d* puvAxis, FLOAT* prad, const RwV3d& uvFrom, const RwV3d& uvTo);
// * ex : AngleFrom2Vec
//{
//	RwV3d	vFrom = { 0.f, 10.f, 0.f };
//	RwV3d	vTo   = { 10.f, 0.f, 0.f };
//
//	FLOAT	radian;
//	RwV3d	uvAxis;
//	RwV3d	uvFrom;
//	RwV3d	uvTo;
//	RwV3dNormalize( &uvFrom, &vFrom );
//	RwV3dNormalize( &uvTo, &vTo );
//
//	if(AngleFrom2Vec(&uvAxis, &radian, uvFrom, uvTo))
//	{
//		RwMatrix mat;
//		RwMatrixRotate( &mat, &uvAxis, DEF_R2D(radian), rwCOMBINEREPLACE );
//		RwV3dTransformPoint( &vFrom, &vTo, &mat );
//	}
//	else
//	{
//		if( radian == DEF_PI )
//			RwV3dNegate(&vFrom, &vTo);
//		else
//			vFrom = vTo;
//	}
//}

BOOL IsMultiplierOfX( int tst, int x=2 );
//ex
//TRUE == IsMultiplierOfX(2,2);
//FALSE == IsMultiplierOf2(3);

inline bool IsMultiplierOf2(int tst)
{
	return (tst && ((tst & tst-1) == 0));
}
//ex
//TRUE == IsMultiplierOf2(2);
//FALSE == IsMultiplierOf2(3);

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
void NormalOfTri( RwV3d& normal
				 , const RwV3d& v0, const RwV3d& v1, const RwV3d& v2
				 , bool bnormalize=false );

// /////////////////////////////////////////////////////////////////////////////
// struct mat2x2
// /////////////////////////////////////////////////////////////////////////////
typedef struct mat2x2
{
	union{
		struct {
			RwReal	_11, _12;
			RwReal	_21, _22;
		};
		RwReal	m[2][2];
	};

	mat2x2(){};
	mat2x2(CONST RwReal* f);
	mat2x2(CONST mat2x2& cpy);
	mat2x2(RwReal _11, RwReal _12,
		   RwReal _21, RwReal _22);
	
    // access grants
    RwReal& operator () ( UINT Row, UINT Col );
    RwReal  operator () ( UINT Row, UINT Col ) const;

    // casting operators
    operator RwReal* ();
    operator CONST RwReal* () const;

    // assignment operators
    mat2x2& operator *= ( CONST mat2x2& );
    mat2x2& operator += ( CONST mat2x2& );
    mat2x2& operator -= ( CONST mat2x2& );
    mat2x2& operator *= ( RwReal );
    mat2x2& operator /= ( RwReal );

    // unary operators
    mat2x2 operator + () const;
    mat2x2 operator - () const;

    // binary operators
    mat2x2 operator * ( CONST mat2x2& ) const;
    mat2x2 operator + ( CONST mat2x2& ) const;
    mat2x2 operator - ( CONST mat2x2& ) const;
    mat2x2 operator * ( RwReal ) const;
    mat2x2 operator / ( RwReal ) const;

    friend mat2x2 operator * ( RwReal, CONST mat2x2& );

    BOOL operator == ( CONST mat2x2& ) const;
    BOOL operator != ( CONST mat2x2& ) const;

	RwReal Det()const { return (_11*_22 - _12*_21); }
}MAT2X2, *LPMAT2X2;
typedef const MAT2X2*	LPCMAT2X2;

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
LPMAT2X2 MAT2X2Inverse(LPMAT2X2 invs, RwReal* det, LPCMAT2X2 src);
// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
RwV2d* V2dTransform(RwV2d* out, const RwV2d* in, LPCMAT2X2 mat);
// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
inline RwReal V2dCrossProduct(const RwV2d* v0, const RwV2d* v1)
{
	return (v0->x*v1->y - v0->y*v1->x);
};

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
inline RwReal V2dDotProduct(const RwV2d* v0, const RwV2d* v1)
{
	return (v0->x*v1->x + v0->y*v1->y);
}

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
inline VOID V2dNegate(RwV2d* out, const RwV2d* in)
{
	out->x = -in->x;
	out->y = -in->y;
};

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
inline RwReal V2dLengthSq(const RwV2d* v)
{
	return (v->x*v->x + v->y*v->y);
}

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
// 0 : intersect, 1 : subset, 2 : parallel
RwInt32 Intersect2DLine( RwV2d& _intsect
					 , const RwV2d& from1, const RwV2d& to1
					 , const RwV2d& from2, const RwV2d& to2
					 , bool bChkRange=false );
inline
RwInt32 Intersect2DLine( RwV2d& _intsect
					 , const RwV3d& from1, const RwV3d& to1
					 , const RwV3d& from2, const RwV3d& to2
					 , bool bChkRange=false )
{
	const RwV2d v2from1 = { from1.x, from1.z };	const RwV2d v2to1 = { to1.x, to1.z };
	const RwV2d v2from2 = { from2.x, from2.z };	const RwV2d v2to2 = { to2.x, to2.z };
	return Intersect2DLine(_intsect, v2from1, v2to1, v2from2, v2to2, bChkRange);
};

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
// intsect true or false
BOOL Intsct2D_TriPoint( const RwV2d& tri0
					  , const RwV2d& tri1
					  , const RwV2d& tri2
					  , const RwV2d& point	);
inline 
BOOL Intsct2D_TriPoint( const RwV3d& tri0
					  , const RwV3d& tri1
					  , const RwV3d& tri2
					  , const RwV3d& point	)
{
	const RwV2d v2Tri0 = { tri0.x, tri0.z };
	const RwV2d v2Tri1 = { tri1.x, tri1.z };
	const RwV2d v2Tri2 = { tri2.x, tri2.z };
	const RwV2d v2Point = { point.x, point.z };
	return Intsct2D_TriPoint(v2Tri0,v2Tri1,v2Tri2,v2Point);
};

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
// intsect true or false
BOOL intsct2D_TriCircle(const RwV2d& tri0
					  , const RwV2d& tri1
					  , const RwV2d& tri2
					  , const RwV2d& center
					  , RwReal radius		);
inline
BOOL intsct2D_TriCircle(const RwV3d& tri0
					  , const RwV3d& tri1
					  , const RwV3d& tri2
					  , const RwV3d& center
					  , RwReal radius		)
{
	const RwV2d v2Tri0 = { tri0.x, tri0.z };
	const RwV2d v2Tri1 = { tri1.x, tri1.z };
	const RwV2d v2Tri2 = { tri2.x, tri2.z };
	const RwV2d v2Center = { center.x, center.z };
	return intsct2D_TriCircle(v2Tri0,v2Tri1,v2Tri2,v2Center, radius);
};
inline
BOOL intsct2D_TriCircle(const AuPOS& tri0
					  , const AuPOS& tri1
					  , const AuPOS& tri2
					  , const AuPOS& center
					  , FLOAT		 radius )
{
	const RwV2d v2Tri0 = { tri0.x, tri0.z };
	const RwV2d v2Tri1 = { tri1.x, tri1.z };
	const RwV2d v2Tri2 = { tri2.x, tri2.z };
	const RwV2d v2Center = { center.x, center.z };
	return intsct2D_TriCircle(v2Tri0,v2Tri1,v2Tri2,v2Center, radius);
}



NSACUMATH_END
#endif // _H_ACUMATHFUNC_20050219
// -----------------------------------------------------------------------------
// AcuMathFunc.h - End of file
// -----------------------------------------------------------------------------

