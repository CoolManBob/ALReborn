#pragma once

//Debugging
inline void PrintLastError()
{
	DWORD dwErrorNo = GetLastError();

	LPVOID lpMsgBuf;
	if( FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |  
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwErrorNo,
						MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),// Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL ))
	{
		// Handle the error.
		return;
	}

	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.

	// 이곳을 바꾸면 오류를 적당히 처리 가능하다
	::MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
	// 이런 식으로...
	//printf("%s ", (LPCTSTR)lpMsgBuf);


	// Free the buffer.
	LocalFree( lpMsgBuf ); 
}

inline BOOL IsFileExist( const char* path )
{
    DWORD dwAttr = GetFileAttributes( path );
    return dwAttr != INVALID_FILE_ATTRIBUTES && ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0;
}

inline BOOL MyCopyFile( char* szSrc, char* szDst )
{
	HANDLE hSrc = CreateFile( szSrc, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	HANDLE hDst = CreateFile( szDst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == hSrc || INVALID_HANDLE_VALUE == hDst )
	{
		if( INVALID_HANDLE_VALUE != hSrc )		CloseHandle( hSrc );
		if( INVALID_HANDLE_VALUE != hDst )		CloseHandle( hDst );
		return FALSE;
	}

	char szBuffer[1024*10];

	DWORD dwRead, dwWrite;
	while( ReadFile( hSrc, szBuffer, 1024*10, &dwRead, NULL ) )
		WriteFile( hDst, szBuffer, dwRead, &dwWrite, NULL );

	CloseHandle( hSrc );
	CloseHandle( hDst );

	return TRUE;
}

class CUtil : public CSingleton< CUtil >
{
public:
	CUtil()		{		}
	~CUtil()	{		}

	BOOL	Initialize();

	void	ReallocCopyString( char** szDst, const char* szSrc );
};

#define UTIL()	CUtil::GetInst()

//RenderWare Define
#define RW_TEXTURE_DESTORY(P)	{	if(P)	{	RwTextureDestroy(P);	(P) = NULL;	}	}

//그냥 랩핑만해서 쓰자.. @@ vec, mat 함수를 잘 모르니깐. 여기다 작성해서 정리해두자..
//맴버변수로는 사용하지 못하도록.. ^^;
class CRwV3d
{
public:
	CRwV3d()				{	v3d = s_v3dZero;			}
	CRwV3d( RwV3d* v )		{	RwV3dAssign( &v3d, v );		}
	CRwV3d( float x, float y, float z )	{
		v3d.x = x;	v3d.y = y;	v3d.z = z;
	}

	RwV3d&	GetV3d()					{	return v3d;					}
	void	SetV3d( RwV3d* v )			{	RwV3dAssign( &v3d, v);		}

	void	Assign( RwV3d* out )		{	RwV3dAssign( out, &v3d );	}
	void	TransPoint( RwMatrix* mat )	{	RwV3dTransformPoint( &v3d, &v3d, mat );	}
	void	TransVec( RwMatrix* mat )	{	RwV3dTransformVector( &v3d, &v3d, mat );}
	void	Scale( float f )			{	RwV3dScale( &v3d, &v3d, f );			}

	void	Add( RwV3d* v )				{	RwV3dAdd( &v3d, &v3d, v );	}

	//엄청 많은뎅. @@;
public:
	static const RwV3d s_v3dZero;
	static const RwV3d s_v3dAt;
	static const RwV3d s_v3dUp;
	static const RwV3d s_v3dRight;

protected:
	RwV3d	v3d;
};


//기본적인 정보를 셋팅해두는게 좋을듯 싶은데...
class CRwMatrix
{
public:
	CRwMatrix( RwFrame* frame )	: matrix( RwFrameGetMatrix( frame ) )	{		}
	CRwMatrix( RwMatrix* m )	: matrix( m )							{		}

	RwV3d*		GetRight()	{	return RwMatrixGetRight( matrix );	}
	RwV3d*		GetUp()		{	return RwMatrixGetUp( matrix );		}
	RwV3d*		GetAt()		{	return RwMatrixGetAt( matrix );		}
	RwV3d*		GetPos()	{	return RwMatrixGetPos( matrix );	}

	RwMatrix*	GetMatrix()					{	return matrix;		}
	RwMatrix*	GetInverse( RwMatrix* out )	{	return RwMatrixInvert( out, matrix );	}

protected:
	RwMatrix*	matrix;
};

class CRwFrame : public CRwMatrix
{
public:
	CRwFrame( RwCamera* camera ) : CRwMatrix( frame = RwCameraGetFrame( camera ) )	{	}
	CRwFrame( RpAtomic* atomic ) : CRwMatrix( frame = RpAtomicGetFrame( atomic ) )	{	}
	CRwFrame( RwFrame* f ) : CRwMatrix( frame = f )									{	}

	RwFrame*	GetParent()	{	return RwFrameGetParent( frame );	}
	RwMatrix*	GetLTM()	{	return RwFrameGetLTM( frame );		}

	void Pos( RwV3d* v, RwOpCombineType combine )				{	RwFrameTranslate( frame, v, combine );		}
	void Rot( RwV3d* v, float angle, RwOpCombineType combine )	{	RwFrameRotate( frame, v, angle, combine );	}
	void Scale( RwV3d* v, RwOpCombineType combine )				{	RwFrameScale( frame, v, combine );			}
	void Scale( float fScale, RwOpCombineType combine )			{
		RwV3d v = { 1.f +  fScale, 1.f +  fScale, 1.f +  fScale };
		RwFrameScale( frame, &v, combine );			
	}
	void Transform( RwMatrix* m, RwOpCombineType combine )		{	RwFrameTransform( frame, m, combine );		}

protected:
	RwFrame*	frame;
};

class CRwCamera : public CRwFrame
{
public:
	CRwCamera( RwCamera* pCamera ) : CRwFrame( pCamera ), camera( pCamera ) 	{		}

	RwFrame*	GetFreme()	{	return frame;	}
	RwMatrix*	GetMatrix()	{	return matrix;	}

private:
	RwCamera*	camera;
};

class CRpAtomic : public CRwFrame
{
public:
	CRpAtomic( RpAtomic* pAtomic ) : CRwFrame( pAtomic ), atomic( pAtomic )		{		}

	RwFrame*	GetFreme()	{	return frame;	}
	RwMatrix*	GetMatrix()	{	return matrix;	}

private:
	RpAtomic*	atomic;
};

//enum   RpLightType
//{ 
//  rpNALIGHTTYPE = 0,
//  rpLIGHTDIRECTIONAL,
//  rpLIGHTAMBIENT,
//  rpLIGHTPOINT = rpLIGHTPOSITIONINGSTART, 
//  rpLIGHTSPOT,
//  rpLIGHTSPOTSOFT,
//  rpLIGHTTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT 
//};

class CRpLight/* : public RpLight*/
{
public:
	CRpLight( RpLightType eType, RpWorld* pWolrd = NULL )	{
		m_pRpLight = RpLightCreate( eType );
		if( pWolrd )
			RpWorldAddLight( pWolrd, m_pRpLight );

	}
	CRpLight( RpLight* pRpLight, RpWorld* pWolrd = NULL ) : m_pRpLight(pRpLight)	{
		if( pWolrd )
			RpWorldAddLight( pWolrd, m_pRpLight );
	}
	~CRpLight()		{
		RpWorld* pWorld = GetWorld();
		if( pWorld )
			RpWorldRemoveLight( pWorld, m_pRpLight );

		RwFrame* pFrame = RpLightGetFrame( m_pRpLight );
		if( pFrame )
		{
			RpLightSetFrame( m_pRpLight, NULL );
			RwFrameDestroy( pFrame );
		}

		RpLightDestroy( m_pRpLight );
	}

	RpLight*	GetRpLight()		{	return m_pRpLight;						}
	RpLightType GetType()			{	return RpLightGetType( m_pRpLight );	}
	RpClump*	GetClump()			{	return RpLightGetClump( m_pRpLight );	}
	RpWorld*	GetWorld()			{	return RpLightGetWorld( m_pRpLight );	}

	const RwRGBAReal*	GetColor()		{	return RpLightGetColor( m_pRpLight );	}
	RwReal				GetConeAngle()	{	return RpLightGetConeAngle( m_pRpLight );	}	//Spot, SpotSoft
	RwUInt32			GetFlags()		{	return RpLightGetFlags( m_pRpLight );	}		//Atomics, World
	RwFrame*			GetFrame()		{	return RpLightGetFrame( m_pRpLight );	}
	RwReal				GetRadius()		{	return RpLightGetRadius( m_pRpLight );	}
	
	void		SetColor( RwRGBAReal& color )	{	RpLightSetColor( m_pRpLight, &color );		}
	void		SetConeAngle( RwReal angle )	{	RpLightSetConeAngle( m_pRpLight, angle );	}
	void		SetFlags( RwUInt32 flags )		{	RpLightSetFlags( m_pRpLight, flags );		}
	void		SetFrame( RwFrame* frame )		{	RpLightSetFrame( m_pRpLight, frame );		}
	void		SetRadius( RwReal radius )		{	RpLightSetRadius( m_pRpLight, radius );		}

private:
	RpLight*	m_pRpLight;
};
typedef CRpLight *LPRpLight;

typedef map< string, LPRpLight >	RpLightMap;
typedef RpLightMap::iterator		RpLightMapItr;

class CRwUtil : public CSingleton< CRwUtil >
{
public:
	BOOL	Initialize( RpWorld* pWorld, RwCamera* pCamera );

	BOOL	MouseTransAtomic( RpAtomic* pAtomic, const RwV2d& vPos );
	BOOL	MouseRotAtomic( RpAtomic* pAtomic, int nAxisType, float fAngle );
	BOOL	MouseScaleAtomic( RpAtomic* pAtomic, float fScale );

	BOOL	TransAtomic( RpAtomic* pAtomic, const RwV3d& vOffPos );
	BOOL	RotAtomic( RpAtomic* pAtomic, const RwV3d& vRot );
	BOOL	ScaleAtomic( RpAtomic* pAtomic, const RwV3d& vScale );

	RpClump*	LoadClump( char* szName );

private:
	RpWorld*	m_pWorld;
	RwCamera*	m_pCamera;
};

#define RWUTIL()	CRwUtil::GetInst()