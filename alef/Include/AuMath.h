// AuMath.h: interface for the AuMath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUMATH_H__59E94F31_4192_4A30_9C85_387BA1804B3E__INCLUDED_)
#define AFX_AUMATH_H__59E94F31_4192_4A30_9C85_387BA1804B3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( push )
#pragma warning ( disable : 4819 )

#include "ApBase.h"
#include "dx9/d3dx9math.h"

#pragma warning ( pop )


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuMathD" )
#else
#pragma comment ( lib , "AuMath" )
#endif
#endif

#pragma comment ( lib , "d3dx9.lib" )

#define	AUMATH_PI	3.141592
#define	AUMATH_2PI	6.283134

#define	AUMATH_PIF	3.141592f
#define	AUMATH_2PIF	6.283134f

class AuMath  
{
public:
	AuMath();
	virtual ~AuMath();

	static AuV3D *		V3DAdd			( AuV3D *	pstOut	, AuV3D *	pstV1	, AuV3D *pstV2			);
	static AuV3D *		V3DCross		( AuV3D *	pstOut	, AuV3D *	pstV1	, AuV3D *pstV2			);
	static FLOAT		V3DDot			( AuV3D *	pstV1	, AuV3D *	pstV2	);
	static FLOAT		V3DLength		( AuV3D *	pstV	);
	static FLOAT		V3DLengthSquare	( AuV3D *	pstV	);
	static AuV3D *		V3DNormalize	( AuV3D *	pstOut	, AuV3D *	pstV	);
	static AuV3D *		V3DScale		( AuV3D *	pstOut	, AuV3D *	pstV	, FLOAT fScale			);
	static AuV3D *		V3DSubtract		( AuV3D *	pstOut	, AuV3D *	pstV1	, AuV3D *pstV2			);
	static AuV3D *		V3DTransform	( AuV3D *	pstOut	, AuV3D *	pstV	, AuMATRIX *pstM		);

	static AuMATRIX *	MatrixIdentity	( AuMATRIX *pstOut	);
	static AuMATRIX *	MatrixInverse	( AuMATRIX *pstOut	, AuMATRIX *pstM	);
	static AuMATRIX *	MatrixMultiply	( AuMATRIX *pstOut	, AuMATRIX *pstM1	, AuMATRIX *	pstM2	);
	static AuMATRIX *	MatrixRotate	( AuMATRIX *pstOut	, AuV3D *	pstV	, FLOAT			fAngle	);
	static AuMATRIX *	MatrixRotateX	( AuMATRIX *pstOut	, FLOAT		fAngle	);
	static AuMATRIX *	MatrixRotateY	( AuMATRIX *pstOut	, FLOAT		fAngle	);
	static AuMATRIX *	MatrixRotateZ	( AuMATRIX *pstOut	, FLOAT		fAngle	);
	static AuMATRIX *	MatrixScale		( AuMATRIX *pstOut	, FLOAT		fSX		, FLOAT fSY, FLOAT fSZ	);
	static AuMATRIX *	MatrixTranslate	( AuMATRIX *pstOut	, FLOAT		fSX		, FLOAT fSY, FLOAT fSZ	);
	static AuMATRIX *	MatrixTransform	( AuMATRIX *pstOut	, AuMATRIX *pstM	);

	static FLOAT	GetWorldAngle( FLOAT fX , FLOAT fZ )
	{
		// Y 좌표 무시.

		FLOAT	fArc;
		fArc = (FLOAT) atan( fX / fZ ) ;

		while( fArc < 0 )  fArc += AUMATH_2PIF;

		if( fX < 0 && fZ < 0 )
		{
			fArc += AUMATH_PIF;
		}
		if( fX > 0 && fZ < 0 )
		{
			fArc -= AUMATH_PIF;
		}

		return 360.0f / AUMATH_PIF / 2.0f * fArc; // 각도
		// return fArc; // 라디안.
	}

	static BOOL ConsiderLeftPoint(AuPOS start, AuPOS pos1, AuPOS pos2);
	// Pos1 이 Left 인지 점검함. 즉 return TRUE이면 pos1 이 LEFT ,FALSE면 Right

};

#endif // !defined(AFX_AUMATH_H__59E94F31_4192_4A30_9C85_387BA1804B3E__INCLUDED_)
