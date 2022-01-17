// AcuMathFunc.cpp
// -----------------------------------------------------------------------------
//                      __  __       _   _     ______                                       
//     /\              |  \/  |     | | | |   |  ____|                                      
//    /  \    ___ _   _| \  / | __ _| |_| |__ | |__   _   _ _ __   ___      ___ _ __  _ __  
//   / /\ \  / __| | | | |\/| |/ _` | __| '_ \|  __| | | | | '_ \ / __|    / __| '_ \| '_ \ 
//  / ____ \| (__| |_| | |  | | (_| | |_| | | | |    | |_| | | | | (__  _ | (__| |_) | |_) |
// /_/    \_\\___|\__,_|_|  |_|\__,_|\__|_| |_|_|     \__,_|_| |_|\___|(_) \___| .__/| .__/ 
//                                                                             | |   | |    
//                                                                             |_|   |_|    
//
// util math functions
//
// -----------------------------------------------------------------------------
// Originally created on 02/19/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AcuMathFunc.h"
NSACUMATH_BEGIN

// /////////////////////////////////////////////////////////////////////////////
// RwRGBA linear interpolation
// /////////////////////////////////////////////////////////////////////////////
template <> RwRGBA* LinearIntp( RwRGBA* pOut, const RwRGBA* pVal1, const RwRGBA* pVal2, const RwReal fCof )
{
	ASSERT( pOut && "@ AcuMath::LinearIntp(RwRGBA)" );
	ASSERT( pVal1 && "@ AcuMath::LinearIntp(RwRGBA)" );
	ASSERT( pVal2 && "@ AcuMath::LinearIntp(RwRGBA)" );
	ASSERT( fCof >= 0.f && "@ AcuMath::LinearIntp(RwRGBA)" );
	ASSERT( fCof <= 1.f && "@ AcuMath::LinearIntp(RwRGBA)" );	
	
	RwInt32 r	=  (RwInt32)pVal1->red	 + floatToInt( (RwReal)( ((RwInt32)pVal2->red	- (RwInt32)pVal1->red	) ) * fCof );
	RwInt32 g	=  (RwInt32)pVal1->green + floatToInt( (RwReal)( ((RwInt32)pVal2->green - (RwInt32)pVal1->green ) ) * fCof );
	RwInt32 b	=  (RwInt32)pVal1->blue  + floatToInt( (RwReal)( ((RwInt32)pVal2->blue	- (RwInt32)pVal1->blue	) ) * fCof );
	RwInt32 a	=  (RwInt32)pVal1->alpha + floatToInt( (RwReal)( ((RwInt32)pVal2->alpha - (RwInt32)pVal1->alpha ) ) * fCof );
	DEF_CLAMP(r, 0, 255);
	DEF_CLAMP(g, 0, 255);
	DEF_CLAMP(b, 0, 255);
	DEF_CLAMP(a, 0, 255);
	pOut->red	= (UINT8)r;
	pOut->green	= (UINT8)g;
	pOut->blue	= (UINT8)b;
	pOut->alpha	= (UINT8)a;
	return pOut;
};

// /////////////////////////////////////////////////////////////////////////////
// rotation axis and angle between 2 vectors
//
// warning : MUST BE VALIDATION CHECKING!
//
// input : uvFrom and uvTo must be normalized
// output : puvAxis normalized and prad in radian
// return value : TRUE - available angle and axis, FALSE - invalid axis
// /////////////////////////////////////////////////////////////////////////////
BOOL AngleFrom2Vec(RwV3d* puvAxis, FLOAT* prad, const RwV3d& uvFrom, const RwV3d& uvTo)
{
	ASSERT(puvAxis);

	FLOAT dot = RwV3dDotProduct( &uvFrom, &uvTo );

	const FLOAT EPSILON	= 0.0001f;
	if( 1.f - floatAbs(dot) < EPSILON )
	{
		if( T_ISMINUS4(dot) )
		{
			*prad = DEF_PI;
			return FALSE;		//서로 마주보는(등을 맞대는)백터.
		}
		else
		{
			*prad = 0.f;
			return FALSE;		//같은 백터
		}
	}

	RwV3dCrossProduct(puvAxis, &uvFrom, &uvTo);
	RwV3dNormalize(puvAxis, puvAxis);

	NSAcuMath::T_CLAMP(dot, -1.f, 1.f);	
	*prad = acosf(dot);
	return TRUE;
}


// /////////////////////////////////////////////////////////////////////////////
// Is tst multiplier of x?
// /////////////////////////////////////////////////////////////////////////////
BOOL IsMultiplierOfX( int tst, int x )
{
	if( !tst || !x )
		return FALSE;
	while( tst >= x )
		tst /= x;

	return (tst==0 ? TRUE : FALSE);
}

// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
void NormalOfTri( RwV3d& normal, const RwV3d& v0, const RwV3d& v1, const RwV3d& v2, bool bnormalize )
{
	RwV3d	v01 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	RwV3d	v02 = { v2.x - v0.x, v2.y - v0.y, v2.z - v0.z };
	RwV3dCrossProduct( &normal, &v01, &v02 );
	if( bnormalize )
		RwV3dNormalize( &normal, &normal );
}


// /////////////////////////////////////////////////////////////////////////////
// mat2x2
// /////////////////////////////////////////////////////////////////////////////
mat2x2::mat2x2( CONST RwReal* f)
: _11(*f), _12(*(++f))
, _21(*(++f)), _22(*(++f))
{
};
mat2x2::mat2x2( CONST mat2x2& cpy)
: _11(cpy._11), _12(cpy._12)
, _21(cpy._21), _22(cpy._22)
{
};
mat2x2::mat2x2( RwReal _11, RwReal _12,
				RwReal _21, RwReal _22)
: _11(_11), _12(_12)
, _21(_21), _22(_22)
{
};
// access grants
RwReal& mat2x2::operator () ( UINT Row, UINT Col )
{
	ASSERT( 0<=Row && 2>Row );
	ASSERT( 0<=Col && 2>Col );
	return m[Row][Col];
};
RwReal mat2x2::operator () ( UINT Row, UINT Col ) const
{
	ASSERT( 0<=Row && 2>Row );
	ASSERT( 0<=Col && 2>Col );
	return m[Row][Col];
};
// casting operators
mat2x2::operator RwReal* ()
{
	return m[0];
};
mat2x2::operator CONST RwReal* () const
{
	return m[0];
};
// assignment operators
mat2x2& mat2x2::operator *= ( CONST mat2x2& rhs)
{
	ASSERT( this != &rhs );
	mat2x2	tmp(*this);
	_11 = tmp._11*rhs._11 + tmp._12*rhs._12;
	_12 = tmp._11*rhs._21 + tmp._12*rhs._22;
	_21 = tmp._21*rhs._11 + tmp._22*rhs._21;
	_22 = tmp._21*rhs._12 + tmp._22*rhs._22;

	return (*this);
};
mat2x2& mat2x2::operator += ( CONST mat2x2& rhs)
{
	_11 += rhs._11; _12 += rhs._12;
	_21 += rhs._21; _22 += rhs._22;

	return (*this);
};
mat2x2& mat2x2::operator -= ( CONST mat2x2& rhs)
{
	_11 -= rhs._11; _12 -= rhs._12;
	_21 -= rhs._21; _22 -= rhs._22;

	return (*this);
};
mat2x2& mat2x2::operator *= ( RwReal scale)
{
	_11 *= scale; _12 *= scale;
	_21 *= scale; _22 *= scale;

	return (*this);
};
mat2x2& mat2x2::operator /= ( RwReal devider)
{
	ASSERT( devider != 0.f );
	_11 /= devider; _12 /= devider;
	_21 /= devider; _22 /= devider;

	return (*this);
};

// unary operators
mat2x2 mat2x2::operator + () const
{
	return (*this);
};
mat2x2 mat2x2::operator - () const
{
	return mat2x2(-_11, -_12,
				  -_21, -_22);
};

// binary operators
mat2x2 mat2x2::operator * ( CONST mat2x2& rhs ) const
{
	return mat2x2(
		_11*rhs._11 + _12*rhs._12,
		_11*rhs._21 + _12*rhs._22,
		_21*rhs._11 + _22*rhs._21,
		_21*rhs._12 + _22*rhs._22);
};
mat2x2 mat2x2::operator + ( CONST mat2x2& rhs ) const
{
	return mat2x2(_11+rhs._11, _12+rhs._12,
				  _21+rhs._21, _22+rhs._22);
};
mat2x2 mat2x2::operator - ( CONST mat2x2& rhs ) const
{
	return mat2x2(_11-rhs._11, _12-rhs._12,
				  _21-rhs._21, _22-rhs._22);
};
mat2x2 mat2x2::operator * ( RwReal scale ) const
{
	return mat2x2(_11 * scale, _12 * scale,
				  _21 * scale, _22 * scale);
};
mat2x2 mat2x2::operator / ( RwReal devider ) const
{
	ASSERT( devider != 0.f );
	return mat2x2(_11 / devider, _12 / devider,
				  _21 / devider, _22 / devider);

};

mat2x2 operator * ( RwReal scale, CONST mat2x2& rhs )
{
	return (rhs*scale);
};

BOOL mat2x2::operator == ( CONST mat2x2& rhs ) const
{
	return (_11 != rhs._11 ||
			_12 != rhs._12 ||
			_21 != rhs._21 ||
			_22 != rhs._22 ) ? FALSE : TRUE;
};
BOOL mat2x2::operator != ( CONST mat2x2& rhs ) const
{
	return (_11 != rhs._11 ||
			_12 != rhs._12 ||
			_21 != rhs._21 ||
			_22 != rhs._22 ) ? TRUE : FALSE;
};

// /////////////////////////////////////////////////////////////////////////////
// MAT2X2Inverse
// /////////////////////////////////////////////////////////////////////////////
LPMAT2X2 MAT2X2Inverse(LPMAT2X2 invs, RwReal* det, LPCMAT2X2 src)
{
	*det = src->Det();
	if( *det != 0.f )
	{
		invs->_11 =  src->_22/(*det); invs->_12 = -src->_12/(*det);
		invs->_21 = -src->_21/(*det); invs->_22 =  src->_11/(*det);

		return invs;
	}
	return NULL;
}
// /////////////////////////////////////////////////////////////////////////////
// V2dTransform
// /////////////////////////////////////////////////////////////////////////////
RwV2d* V2dTransform(RwV2d* out, const RwV2d* in, LPCMAT2X2 mat)
{
	ASSERT( out && in && mat );

	RwV2d tmp = *in;
	out->x = tmp.x*mat->_11 + tmp.y*mat->_21;
	out->y = tmp.x*mat->_12 + tmp.y*mat->_22;

	return out;
};
// /////////////////////////////////////////////////////////////////////////////
// Intersect2DLine
// return
//		intersect	: 0
//		subset		: 1
//		parallel	: 2
//		over range	| -1
// /////////////////////////////////////////////////////////////////////////////
RwInt32 Intersect2DLine( RwV2d& _intsect
						 , const RwV2d& from1, const RwV2d& to1
						 , const RwV2d& from2, const RwV2d& to2
						 , bool bChkRange)
{
	RwInt32 ir = 0;
	const RwV2d	fromto1	= { to1.x-from1.x, to1.y-from1.y };
	const RwV2d	fromto2	= { to2.x-from2.x, to2.y-from2.y };

	const MAT2X2 matCoef( fromto1.x,  fromto1.y,
						 -fromto2.x, -fromto2.y);
	MAT2X2	matInvs;
	RwReal	det;
	if( MAT2X2Inverse(&matInvs, &det, &matCoef) )
	{
		RwV2d	param = { from2.x - from1.x, from2.y - from1.y };
		V2dTransform( &param, &param, &matInvs );

		_intsect.x = from1.x + param.x*fromto1.x;
		_intsect.y = from1.y + param.x*fromto1.y;

		if( bChkRange )
		{
			if( T_MIN( from1.x, to1.x ) > _intsect.x
			|| T_MAX( from1.x, to1.x ) < _intsect.x
			|| T_MIN( from1.y, to1.y ) > _intsect.y
			|| T_MAX( from1.y, to1.y ) < _intsect.y

			|| T_MIN( from2.x, to2.x ) > _intsect.x
			|| T_MAX( from2.x, to2.x ) < _intsect.x
			|| T_MIN( from2.y, to2.y ) > _intsect.y
			|| T_MAX( from2.y, to2.y ) < _intsect.y
			)
			return -1;
		}

		//ASSERT( _intsect.x == from2.x + param.y*fromto2.x );
		//ASSERT( _intsect.y == from2.y + param.y*fromto2.y );

		ir = 0;
	}
	else//parallel or one is subset of other one
	{
		RwV2d	tmp = { to1.x+to2.x, to1.y+to2.y};
        if( V2dCrossProduct( &fromto1, &tmp ) == 0.f )
			ir = 1;
		else
			ir = 2;
	}

	return ir;
}


// /////////////////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////////////////
// intsect true or false
BOOL Intsct2D_TriPoint( const RwV2d& tri0
					  , const RwV2d& tri1
					  , const RwV2d& tri2
					  , const RwV2d& point	)
{
	RwV2d	triToPoint	= {point.x - tri0.x, point.y - tri0.y};
	RwV2d	triToNext	= {tri1.x - tri0.x, tri1.y - tri0.y};
	RwReal	cross = V2dCrossProduct( &triToPoint, &triToNext );

	RwV2dSub ( &triToPoint, &point, &tri1 ); 
	RwV2dSub ( &triToNext, &tri2, &tri1 ); 
	RwReal	crosstmp = V2dCrossProduct( &triToPoint, &triToNext );
	if( crosstmp * cross < 0.f )
		return FALSE;

	RwV2dSub ( &triToPoint, &point, &tri2 ); 
	RwV2dSub ( &triToNext, &tri0, &tri2 ); 
	crosstmp = V2dCrossProduct( &triToPoint, &triToNext );
	if( crosstmp * cross < 0.f )
		return FALSE;

	return TRUE;// The point is in the triangle
};

// /////////////////////////////////////////////////////////////////////////////
//  
// /////////////////////////////////////////////////////////////////////////////
// intsect true or false
BOOL intsct2D_TriCircle(const RwV2d& tri0
					  , const RwV2d& tri1
					  , const RwV2d& tri2
					  , const RwV2d& center
					  , const RwReal radius		)
{
	// is the center in the tri?
	if( Intsct2D_TriPoint(tri0, tri1, tri2, center) )
		return TRUE;

	// is least one tripoint in the circle?
	const RwReal radiusSq = radius*radius;
	RwV2d tmp = { tri0.x - center.x, tri0.y - center.y };
	if( V2dLengthSq(&tmp) <= radiusSq )
		return TRUE;
	RwV2dSub ( &tmp, &tri1, &center ); 
	if( V2dLengthSq(&tmp) <= radiusSq )
		return TRUE;
	RwV2dSub ( &tmp, &tri2, &center ); 
	if( V2dLengthSq(&tmp) <= radiusSq )
		return TRUE;

	// check the three line 
	// condition is perpendicular line's length is smaller or equal to the radius
	// and all the tri(center is the one point of the tri) angle is acute
	RwV2d p0ToCenter = { center.x - tri0.x, center.y - tri0.y };
	RwV2d p1ToCenter = { center.x - tri1.x, center.y - tri1.y };
	RwV2d p2ToCenter = { center.x - tri2.x, center.y - tri2.y };
	RwV2dLineNormal ( &tmp, &tri0, &tri1 );
	RwV2d tmp2 = { -tmp.x, -tmp.y };
	RwReal cross = V2dCrossProduct(&p0ToCenter, &tmp);
	if( fabsf(cross) <= radius 
	 && V2dDotProduct(&p0ToCenter, &tmp) > 0.f
	 && V2dDotProduct(&p1ToCenter, &tmp2) > 0.f
	 )
	 return TRUE;

	RwV2dLineNormal ( &tmp, &tri1, &tri2 );
	V2dNegate(&tmp2, &tmp);
	cross = V2dCrossProduct(&p1ToCenter, &tmp);
	if( fabsf(cross) <= radius 
	 && V2dDotProduct(&p1ToCenter, &tmp) > 0.f
	 && V2dDotProduct(&p2ToCenter, &tmp2) > 0.f
	 )
	 return TRUE;

	RwV2dLineNormal ( &tmp, &tri2, &tri0 );
	V2dNegate(&tmp2, &tmp);
	cross = V2dCrossProduct(&p2ToCenter, &tmp);
	if( fabsf(cross) <= radius 
	 && V2dDotProduct(&p2ToCenter, &tmp) > 0.f
	 && V2dDotProduct(&p0ToCenter, &tmp2) > 0.f
	 )
	 return TRUE;

	return FALSE;
};

NSACUMATH_END
// -----------------------------------------------------------------------------
// AcuMathFunc.cpp - End of file
// -----------------------------------------------------------------------------
