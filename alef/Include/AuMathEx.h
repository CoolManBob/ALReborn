#ifndef __AU_MATH_EX_H__
#define __AU_MATH_EX_H__

#include <windows.h>

#define AUMATHEX_PI			3.1415926535f

typedef struct _AME_V3D
{
	FLOAT	x, y, z;

	_AME_V3D()
	{
		x = y = z = 0.0f;
	}

	_AME_V3D(FLOAT _x, FLOAT _y, FLOAT _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

} AME_V3D;

typedef struct _AME_MATRIX
{
	union
	{
		struct
		{
			AME_V3D	right, up, at, pos;
		};

		FLOAT	m[4][3];
	};

	_AME_MATRIX()
	{
		m[0][0]	= m[1][1] = m[2][2] = 1.0f;
		m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
	}

} AME_MATRIX;

static AME_V3D	GV_AME_V3D_X_AXIS(1.0f, 0.0f, 0.0f);
static AME_V3D	GV_AME_V3D_Y_AXIS(0.0f, 1.0f, 0.0f);
static AME_V3D	GV_AME_V3D_Z_AXIS(0.0f, 0.0f, 1.0f);

class AuMathEx
{
public:
	AuMathEx();
	virtual ~AuMathEx();

	static FLOAT		InvSqrt(FLOAT fIn);

	static AME_V3D		*V3dNormalize(AME_V3D *pstOut, AME_V3D *pstIn);
	static AME_V3D		*V3dScale(AME_V3D *pstOut, AME_V3D *pstIn, FLOAT fAlpha);

	static AME_MATRIX	*MatrixIdentity(AME_MATRIX	*pstOut);
	static AME_MATRIX	*MatrixRotate(AME_MATRIX *pstMatrix, AME_V3D *pstAxis, FLOAT fAngle);
	static AME_MATRIX	*MatrixMultiply(AME_MATRIX *pstOut, AME_MATRIX *pstSrc1, AME_MATRIX *pstSrc2);

protected:
	static AME_MATRIX	*MatrixRotateOneMinusCosineSine(AME_MATRIX *pstMatrix, AME_V3D *pstUnitAxis, FLOAT fOneMinusCosine, FLOAT fSine);
};

#endif