// AuMath.cpp: implementation of the AuMath class.
//
//////////////////////////////////////////////////////////////////////

#include "AuMath.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AuMath::AuMath()
{

}

AuMath::~AuMath()
{

}

AuV3D *	AuMath::V3DAdd(AuV3D *pstOut, AuV3D *pstV1, AuV3D *pstV2)
{
	return (AuV3D *) D3DXVec3Add((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV1, (D3DXVECTOR3 *) pstV2);
}

AuV3D *	AuMath::V3DCross(AuV3D *pstOut, AuV3D *pstV1, AuV3D *pstV2)
{
	return (AuV3D *) D3DXVec3Cross((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV1, (D3DXVECTOR3 *) pstV2);
}

FLOAT	AuMath::V3DDot(AuV3D *pstV1, AuV3D *pstV2)
{
	return D3DXVec3Dot((D3DXVECTOR3 *) pstV1, (D3DXVECTOR3 *) pstV2);
}

FLOAT	AuMath::V3DLength(AuV3D *pstV)
{
	return D3DXVec3Length((D3DXVECTOR3 *) pstV);
}

FLOAT	AuMath::V3DLengthSquare(AuV3D *pstV)
{
	return D3DXVec3LengthSq((D3DXVECTOR3 *) pstV);
}

AuV3D *	AuMath::V3DNormalize(AuV3D *pstOut, AuV3D *pstV)
{
	return (AuV3D *) D3DXVec3Normalize((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV);
}

AuV3D *	AuMath::V3DScale(AuV3D *pstOut, AuV3D *pstV, FLOAT fScale)
{
	return (AuV3D *) D3DXVec3Scale((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV, fScale);
}

AuV3D *	AuMath::V3DSubtract(AuV3D *pstOut, AuV3D *pstV1, AuV3D *pstV2)
{
	return (AuV3D *) D3DXVec3Subtract((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV1, (D3DXVECTOR3 *) pstV2);
}

AuV3D *	AuMath::V3DTransform(AuV3D *pstOut, AuV3D *pstV, AuMATRIX *pstM)
{
	return (AuV3D *) D3DXVec3TransformCoord((D3DXVECTOR3 *) pstOut, (D3DXVECTOR3 *) pstV, (D3DXMATRIX *) pstM);
}


AuMATRIX *	AuMath::MatrixIdentity(AuMATRIX *pstOut)
{
	return (AuMATRIX *) D3DXMatrixIdentity((D3DXMATRIX *) pstOut);
}

AuMATRIX *	AuMath::MatrixInverse(AuMATRIX *pstOut, AuMATRIX *pstM)
{
	FLOAT	fDeterminant;

	return (AuMATRIX *) D3DXMatrixInverse((D3DXMATRIX *) pstOut, &fDeterminant, (D3DXMATRIX *) pstM);
}

AuMATRIX *	AuMath::MatrixMultiply(AuMATRIX *pstOut, AuMATRIX *pstM1, AuMATRIX *pstM2)
{
	return (AuMATRIX *) D3DXMatrixMultiply((D3DXMATRIX *) pstOut, (D3DXMATRIX *) pstM1, (D3DXMATRIX *) pstM2);
}

AuMATRIX *	AuMath::MatrixRotate(AuMATRIX *pstOut, AuV3D *pstV, FLOAT fAngle)
{
	return (AuMATRIX *) D3DXMatrixRotationAxis((D3DXMATRIX *) pstOut, (D3DXVECTOR3 *) pstV, fAngle * 3.141592f / 180.0f);
}

AuMATRIX *	AuMath::MatrixRotateX(AuMATRIX *pstOut, FLOAT fAngle)
{
	return (AuMATRIX *) D3DXMatrixRotationX((D3DXMATRIX *) pstOut, fAngle * 3.141592f / 180.0f);
}

AuMATRIX *	AuMath::MatrixRotateY(AuMATRIX *pstOut, FLOAT fAngle)
{
	return (AuMATRIX *) D3DXMatrixRotationY((D3DXMATRIX *) pstOut, fAngle * 3.141592f / 180.0f);
}

AuMATRIX *	AuMath::MatrixRotateZ(AuMATRIX *pstOut, FLOAT fAngle)
{
	return (AuMATRIX *) D3DXMatrixRotationZ((D3DXMATRIX *) pstOut, fAngle * 3.141592f / 180.0f);
}

AuMATRIX *	AuMath::MatrixScale(AuMATRIX *pstOut, FLOAT fSX, FLOAT fSY, FLOAT fSZ)
{
	return (AuMATRIX *) D3DXMatrixScaling((D3DXMATRIX *) pstOut, fSX, fSY, fSZ);
}

AuMATRIX *	AuMath::MatrixTranslate(AuMATRIX *pstOut, FLOAT fSX, FLOAT fSY, FLOAT fSZ)
{
	return (AuMATRIX *) D3DXMatrixTranslation((D3DXMATRIX *) pstOut, fSX, fSY, fSZ);
}

AuMATRIX *	AuMath::MatrixTransform(AuMATRIX *pstOut, AuMATRIX *pstM)
{
	return (AuMATRIX *) D3DXMatrixTranspose((D3DXMATRIX *) pstOut, (D3DXMATRIX *) pstM);
}

BOOL		AuMath::ConsiderLeftPoint(AuPOS start, AuPOS pos1, AuPOS pos2)
{
   AuPOS MiddlePos;
   AuPOS PlaneVec1, PlaneVec2, PlaneNormal;
   float fDistance;

   // 중점 구하기.
   MiddlePos.x = (pos1.x + pos2.x ) / 2.0f;
   MiddlePos.y = (pos1.y + pos2.y ) / 2.0f;
   MiddlePos.z = (pos1.z + pos2.z ) / 2.0f;

   PlaneVec1 = MiddlePos - start;

   PlaneVec2.x = PlaneVec1.x;
   PlaneVec2.y = PlaneVec1.y + 2.0f; 			//. 같은 평면위에 있는 방향벡터.
   PlaneVec2.z = PlaneVec1.z;

   V3DCross(&PlaneNormal, &PlaneVec1, &PlaneVec2);	//. 평면의 방정식을 위한 평면의 노멀벡터.
   V3DNormalize(&PlaneNormal, &PlaneNormal);

   //. pos1만 검사하여 판단. 판단되면 다른 쪽은 무조건 반대쪽이 되므로.
   //. 평면의 방정식, Ax + By + Cz + D = 0
   fDistance = -V3DDot(&PlaneNormal, &MiddlePos);

   //. 양수일경우 pos1이 왼쪽, 음수일 경우 오른쪽. 
   if( (int)(V3DDot(&PlaneNormal, &pos1) + fDistance) >= 0 )
	   return TRUE	;	// LEFT
   else
	   return FALSE;	// RIGHT
}