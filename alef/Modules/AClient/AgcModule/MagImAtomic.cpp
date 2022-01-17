// MagImAtomic.cpp: implementation of the MagImAtomic class.
//
//////////////////////////////////////////////////////////////////////

#include "ApBase.h"
#include "RwCore.h"
#include "MagImAtomic.h"
#include "MagDebug.h"

#include "AuProfileManager.h"

#include "AgcEngine.h"
#include "ApMemoryTracker.h"

extern	AgcEngine*	g_pEngine;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MagImAtomic::MagImAtomic():
	m_pVertex	( NULL	),
	m_nVertex	( 0		),
	m_pIndex	( NULL	),
	m_nIndex	( 0		),

	m_nTransformFlag ( rwIM3D_VERTEXRGBA | rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ ),
	m_nPrimitiveType ( rwPRIMTYPETRILIST ),

	m_pTexture	( NULL	),
	m_pFrame	( NULL	),
	m_nTexture	( -1	),

	m_bVisible	( TRUE	)
{
	// 초기화..
}

MagImAtomic::~MagImAtomic()
{
	VERIFY( DestroyAtomic() );
}

BOOL	MagImAtomic::CreateAtomic(	INT32			nVertex	,
									INT32			nIndex	,
									UINT32			nTrans	,
									RwPrimitiveType	nType	)
{
	VERIFY( DestroyAtomic() );

	ASSERT( nVertex > 0 );
	ASSERT( nIndex > 0 );
	if( nVertex <= 0 || nIndex <= 0 )
		return FALSE;

	m_nVertex	= nVertex	;	// 갯수..
	m_nIndex	= nIndex	;	// 갯수..
	VERIFY( m_pVertex	= new RwIm3DVertex		[ m_nVertex	] );
	VERIFY( m_pIndex	= new RwImVertexIndex	[ m_nIndex	] );

	if( NULL == m_pVertex || NULL == m_pIndex )
	{
		DestroyAtomic();
		return FALSE;
	}

	m_nTransformFlag = nTrans	;	// 트랜스폼 플래그..
	m_nPrimitiveType = nType	;	// 렌더 플래그..

	// 프레임 생성..
	VERIFY( m_pFrame =	RwFrameCreate() );

	// 따로 초기화는 하지 않음둥.
	
	return TRUE;
}

BOOL	MagImAtomic::DestroyAtomic()
{
	// 데이타 삭제 과정..

	if( NULL != m_pVertex && !IsBadReadPtr( m_pVertex	, ( sizeof RwIm3DVertex		) * m_nVertex	) )
	{
		delete [] m_pVertex;
	}

	if( NULL != m_pIndex && !IsBadReadPtr( m_pIndex		, ( sizeof RwImVertexIndex	) * m_nVertex	) )
	{
		delete [] m_pIndex;
	}

	if( NULL != m_pFrame && !IsBadReadPtr( m_pFrame		, ( sizeof RwFrame			)				) )
	{
		RwFrameDestroy( m_pFrame );
	}

	if( NULL != m_pTexture	)
	{
		RwTextureDestroy( m_pTexture );
		m_pTexture = NULL;
	}

	// 변수 초기화..
	m_pVertex			= NULL				;
	m_nVertex			= 0					;
	m_pIndex			= NULL				;
	m_nIndex			= 0					;
	m_nTransformFlag	= rwIM3D_VERTEXRGBA | rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ;
	m_nPrimitiveType	= rwPRIMTYPETRILIST	;
	m_pFrame			= NULL				;
	m_pTexture			= NULL				;
	return TRUE;
}

BOOL	MagImAtomic::Render()
{
	if( m_bVisible && m_pVertex && m_pIndex )
	{
		PROFILE("MagImAtomic::Render() (m_bVisible && m_pVertex && m_pIndex)");

		//RwRenderStateSet( rwRENDERSTATESHADEMODE, ( void * ) rwSHADEMODEGOURAUD	);

		/* This is default render state (will be set at other place) */

		//RwUInt32	prevAddressingMode;
		if( m_pTexture )
		{
			RwRenderStateSet( rwRENDERSTATETEXTURERASTER	, ( VOID * ) m_pTexture->raster						);
			//RwRenderStateGet( rwRENDERSTATETEXTUREADDRESS	, ( VOID * ) &prevAddressingMode					);
			RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, ( VOID * ) RwTextureGetAddressing( m_pTexture )	);
		}
		else
		{
			RwRenderStateSet( rwRENDERSTATETEXTURERASTER	, NULL												);
		}

		g_pEngine->LockFrame();
		RwMatrix*	pLTM =  RwFrameGetLTM( m_pFrame );
		g_pEngine->UnlockFrame();

		if( RwIm3DTransform( m_pVertex , m_nVertex , pLTM , m_nTransformFlag ) )
		{                         
			RwIm3DRenderIndexedPrimitive(
				m_nPrimitiveType	,
				m_pIndex			,
				m_nIndex			);
			RwIm3DEnd();
		}
		//if( m_pTexture )
		//{
			//RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, ( VOID * ) prevAddressingMode	);
		//}
	}
	else
	{
		// do nothing....
	}

	return TRUE;
}

BOOL	MagImAtomic::SetTexture	(	RwTexture	*	pTexture , INT32 nTexture	)	// Ref Count를 증가시킨다.
{
	// 텍스쳐 중복 입력처리..
	// nTexture != -1이면 텍스쳐 인덱스를 사용하지 않음둥.
	if( nTexture != -1 && m_pTexture == pTexture ) return TRUE;

	RwTexture * pPrevTexture = m_pTexture;

	// 텍스쳐 인덱스 저장해둠
	m_nTexture	= nTexture;

	if( pTexture )
	{
		m_pTexture = pTexture;
		RwTextureAddRef( m_pTexture );
	}
	else
		m_pTexture = NULL;

	if( pPrevTexture )
	{
		// Ref Count 감소..
		RwTextureDestroy( pPrevTexture );
		pPrevTexture = NULL;
	}

	return TRUE;
}