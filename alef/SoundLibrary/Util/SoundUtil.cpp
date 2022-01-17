#include "SoundUtil.h"

namespace SoundUtil
{
	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::GetWorldPositionFromNode( SoundNode* pNode )
	{
		SoundVector vVector = vZeroVector;
		if( !pNode ) return vVector;

		RwMatrix* pmatTM = RwFrameGetMatrix( pNode );
		if( !pmatTM ) return vVector;

		SoundVector* pVector = RwMatrixGetPos( pmatTM );
		if( !pVector ) return vVector;

		vVector.x = pVector->x;
		vVector.y = pVector->y;
		vVector.z = pVector->z;

		return vVector;
	}

	//-----------------------------------------------------------------------
	//

	BOOL SoundUtil::IsSameSoundVector( SoundVector * vVec1, SoundVector * vVec2 )
	{
		if( vVec1->x != vVec2->x ) return FALSE;
		if( vVec1->y != vVec2->y ) return FALSE;
		if( vVec1->z != vVec2->z ) return FALSE;
		return TRUE;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::NormalizeVector( SoundVector * vVec )
	{
		SoundVector vNormal = *vVec;

		float flNormal;
		flNormal = ( float )sqrt( vNormal.x * vNormal.x + vNormal.y * vNormal.y + vNormal.z * vNormal.z );

		if( flNormal <= 0.0001f )
		{
			flNormal = 0.0001f;
		}

		vNormal.x /= flNormal;
		vNormal.y /= flNormal;
		vNormal.z /= flNormal;

		if( fabs( vNormal.x ) < 0.0001f )	vNormal.x = 0.0f;
		if( fabs( vNormal.y ) < 0.0001f )	vNormal.y = 0.0f;
		if( fabs( vNormal.z ) < 0.0001f )	vNormal.z = 0.0f;

		return vNormal;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::CalcVectorVelocity( SoundVector * vCurr, SoundVector * vPrev, float fElapsedTime )
	{
		if(vCurr->x < -1000000000.f || vCurr->x > 1000000000.f)
			int i = 0;
		if(vCurr->y < -1000000000.f || vCurr->y > 1000000000.f)
			int j = 0;
		SoundVector vDistance = SoundUtil::CalcVectorDistance( vCurr, vPrev );
		SoundVector vResult = SoundUtil::CalcVectorScale( &vDistance, ( 1000.0f / ( fElapsedTime * 1000.0f ) ) );
		return vResult;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::CalcVectorDistance( SoundVector * vVec1, SoundVector * vVec2 )
	{
		SoundVector vResult = vZeroVector;
		RwV3dSub( &vResult, vVec1, vVec2 );
		return vResult;
	}

	//-----------------------------------------------------------------------
	//

	float SoundUtil::CalcScalarDistance( SoundVector * vVec1, SoundVector * vVec2 )
	{
		SoundVector vDistance = SoundUtil::CalcVectorDistance( vVec1, vVec2 );
		float fDistance = RwV3dLength( &vDistance );
		return fDistance;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::CalcVectorScale( SoundVector * vVec, float fScaleFactor )
	{
		SoundVector vResult = vZeroVector;
		RwV3dScale( &vResult, vVec, fScaleFactor );
		return vResult;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::GetAtFromNode( SoundNode* pNode )
	{
		SoundVector vVector = vZeroVector;
		if( !pNode ) return vVector;

		RwMatrix* pmatTM = RwFrameGetMatrix( pNode );
		if( !pmatTM ) return vVector;

		SoundVector* pVector = RwMatrixGetAt( pmatTM );
		if( !pVector ) return vVector;

		vVector.x = -pVector->x;
		vVector.y = -pVector->y;
		vVector.z = -pVector->z;
		//vVector = NormalizeVector(vVector);

		RwV3dNormalize( &vVector, &vVector );

		return vVector;
	}

	//-----------------------------------------------------------------------
	//

	SoundVector SoundUtil::GetUpFromNode( SoundNode* pNode )
	{
		SoundVector vVector = vZeroVector;
		if( !pNode ) return vVector;

		RwMatrix* pmatTM = RwFrameGetMatrix( pNode );
		if( !pmatTM ) return vVector;

		SoundVector* pVector = RwMatrixGetUp( pmatTM );
		if( !pVector ) return vVector;

		//vVector.x = pVector->x;
		//vVector.y = pVector->y;
		//vVector.z = pVector->z;
		//vVector = NormalizeVector(vVector);

		RwV3dNormalize( &vVector, pVector );

		return vVector;
	}

	//-----------------------------------------------------------------------
	//
	// 윈도우 버전을 알아냄
	//////////////////////////////////////////////////////////
	//  Function Name  
	//      GetOSVersionType
	//
	//  Parameters 
	//
	//  Return Values
	//      설치된 Windows 버전을 return
	//      -1: Failed
	//      1 : Windows 95
	//      2 : Windows 98
	//      3 : Windows ME
	//      4 : Windows NT
	//      5 : Windows 2000
	//      6 : Windows XP
	//		7 : Vista
	//		8 : Server 2008
	//		9 : Windows 7
	//		10: Server 2008 R2
	int WINAPI GetOSVersionType() 
	{
		static UINT nOSVersion = 0;

		if( nOSVersion )
			return nOSVersion;

		OSVERSIONINFOEX osvi;
		BOOL bOsVersionInfoEx;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
				return -1;
		}

		switch (osvi.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			// Test for the product.
			if ( osvi.dwMajorVersion <= 4 )
				nOSVersion = 4;
			else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
				nOSVersion = 5;
			else if( bOsVersionInfoEx )  
			{
				if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
				{
					nOSVersion = 6;
				}
				else if( osvi.dwMajorVersion == 6 )
				{
					if( osvi.dwMinorVersion == 0 )
					{
						if( osvi.wProductType == VER_NT_WORKSTATION )
							nOSVersion = 7;
						else
							nOSVersion = 8;
					}
					else if( osvi.dwMinorVersion == 1 )
					{
						if( osvi.wProductType == VER_NT_WORKSTATION )
							nOSVersion = 9;
						else
							nOSVersion = 10;
					}
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				nOSVersion = 1;
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				nOSVersion = 2;
			else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				nOSVersion = 3;
			break;
		}

		return nOSVersion; 
	}

	//-----------------------------------------------------------------------
}