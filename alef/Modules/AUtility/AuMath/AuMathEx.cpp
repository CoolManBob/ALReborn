#include "AuMathEx.h"
#include <math.h>

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AuMathEx::AuMathEx()
{
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AuMathEx::~AuMathEx()
{
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
FLOAT AuMathEx::InvSqrt(FLOAT fIn)
{
	return 1.0f / (FLOAT)(sqrt(fIn));
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_V3D *AuMathEx::V3dScale(AME_V3D *pstOut, AME_V3D *pstIn, FLOAT fAlpha)
{
	pstOut->x = pstIn->x * fAlpha;
    pstOut->y = pstIn->y * fAlpha;
    pstOut->z = pstIn->z * fAlpha;

	return pstOut;
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_V3D *AuMathEx::V3dNormalize(AME_V3D *pstOut, AME_V3D *pstIn)
{
	FLOAT	fResult, fLength;

	fLength	= (pstIn->x * pstIn->x) + (pstIn->y * pstIn->y) + (pstIn->z * pstIn->z);
	fResult	= AuMathEx::InvSqrt(fLength);
	return AuMathEx::V3dScale(pstOut, pstIn, fResult);
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_MATRIX *AuMathEx::MatrixIdentity(AME_MATRIX	*pstOut)
{
	pstOut->m[0][0] = pstOut->m[1][1] = pstOut->m[2][2] = 1.0f;

	pstOut->m[0][1] = pstOut->m[0][2] = pstOut->m[1][0] =
	pstOut->m[1][2] = pstOut->m[2][0] = pstOut->m[2][1] =
	pstOut->m[3][0] = pstOut->m[3][1] = pstOut->m[3][2] = 0.0f;

	return pstOut;
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_MATRIX *AuMathEx::MatrixMultiply(AME_MATRIX *pstOut, AME_MATRIX *pstSrc1, AME_MATRIX *pstSrc2)
{
	pstOut->right.x		=	(pstSrc1->right.x	*	pstSrc2->right.x)	+	(pstSrc1->up.x	*	pstSrc2->right.y)	+	(pstSrc1->at.x	*	pstSrc2->right.z)	;
	pstOut->right.y		=	(pstSrc1->right.y	*	pstSrc2->right.x)	+	(pstSrc1->up.y	*	pstSrc2->right.y)	+	(pstSrc1->at.y	*	pstSrc2->right.z)	;
	pstOut->right.z		=	(pstSrc1->right.z	*	pstSrc2->right.x)	+	(pstSrc1->up.z	*	pstSrc2->right.y)	+	(pstSrc1->at.z	*	pstSrc2->right.z)	;

	pstOut->up.x		=	(pstSrc1->right.x	*	pstSrc2->up.x)		+	(pstSrc1->up.x	*	pstSrc2->up.y)		+	(pstSrc1->at.x	*	pstSrc2->up.z)		;
	pstOut->up.y		=	(pstSrc1->right.y	*	pstSrc2->up.x)		+	(pstSrc1->up.y	*	pstSrc2->up.y)		+	(pstSrc1->at.y	*	pstSrc2->up.z)		;
	pstOut->up.z		=	(pstSrc1->right.z	*	pstSrc2->up.x)		+	(pstSrc1->up.z	*	pstSrc2->up.y)		+	(pstSrc1->at.z	*	pstSrc2->up.z)		;

	pstOut->at.x		=	(pstSrc1->right.x	*	pstSrc2->at.x)		+	(pstSrc1->up.x	*	pstSrc2->at.y)		+	(pstSrc1->at.x	*	pstSrc2->at.z)		;
	pstOut->at.y		=	(pstSrc1->right.y	*	pstSrc2->at.x)		+	(pstSrc1->up.y	*	pstSrc2->at.y)		+	(pstSrc1->at.y	*	pstSrc2->at.z)		;
	pstOut->at.z		=	(pstSrc1->right.z	*	pstSrc2->at.x)		+	(pstSrc1->up.z	*	pstSrc2->at.y)		+	(pstSrc1->at.z	*	pstSrc2->at.z)		;

	return pstOut;
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_MATRIX *AuMathEx::MatrixRotateOneMinusCosineSine(AME_MATRIX *pstMatrix, AME_V3D *pstUnitAxis, FLOAT fOneMinusCosine, FLOAT fSine)
{
	AME_MATRIX	matRotate, matLocal;
	AME_V3D		v3dLeading, v3dScaled, v3dCrossed;

	v3dLeading.x	= 1.0f - (pstUnitAxis->x * pstUnitAxis->x);
    v3dLeading.y	= 1.0f - (pstUnitAxis->y * pstUnitAxis->y);
    v3dLeading.z	= 1.0f - (pstUnitAxis->z * pstUnitAxis->z);

	AuMathEx::V3dScale(&v3dLeading, &v3dLeading, fOneMinusCosine);

    v3dCrossed.x	= pstUnitAxis->y * pstUnitAxis->z;
    v3dCrossed.y	= pstUnitAxis->z * pstUnitAxis->x;
    v3dCrossed.z	= pstUnitAxis->x * pstUnitAxis->y;

	AuMathEx::V3dScale(&v3dCrossed, &v3dCrossed, fOneMinusCosine);

	AuMathEx::V3dScale(&v3dScaled, pstUnitAxis, fSine);
    /*
     * Rotate matrix proper
     */
    matRotate.right.x	= 1.0f - v3dLeading.x;
    matRotate.right.y	= v3dCrossed.z + v3dScaled.z;
    matRotate.right.z	= v3dCrossed.y - v3dScaled.y;
    matRotate.up.x		= v3dCrossed.z - v3dScaled.z;
    matRotate.up.y		= 1.0f - v3dLeading.y;
    matRotate.up.z		= v3dCrossed.x + v3dScaled.x;
    matRotate.at.x		= v3dCrossed.y + v3dScaled.y;
    matRotate.at.y		= v3dCrossed.x - v3dScaled.x;
    matRotate.at.z		= 1.0f - v3dLeading.z;

	AuMathEx::MatrixMultiply(&matLocal, pstMatrix, &matRotate);
	memcpy(pstMatrix, &matLocal, sizeof(AME_MATRIX));

	return pstMatrix;
}

/******************************************************************************
* Purpose :
*
* 250804 BOB
******************************************************************************/
AME_MATRIX *AuMathEx::MatrixRotate(AME_MATRIX *pstMatrix, AME_V3D *pstAxis, FLOAT fAngle)
{
	AME_V3D		v3dUnitAxis;
	FLOAT		fSinVal, fOneMinusCosVal, fRadians;

	fRadians		= fAngle * ((FLOAT)(AUMATHEX_PI) / 180.0f);
	fSinVal			= (FLOAT)(sin(fRadians));
	fOneMinusCosVal	= 1.0f - (FLOAT)(cos(fRadians));

	AuMathEx::V3dNormalize(&v3dUnitAxis, pstAxis);

	return AuMathEx::MatrixRotateOneMinusCosineSine(pstMatrix, &v3dUnitAxis, fOneMinusCosVal, fSinVal);
}

/******************************************************************************
******************************************************************************/