#include "rwcore.h"
#include "rpworld.h"

#include <skeleton.h>
#include <menu.h>
#include <MagDebug.h>
#include <win.h>

#include "AuRwImage.h"

BOOL	AuRwImage::Load( char * pFileName )
{
	if( GetImagePointer() )
	{
		// 로딩돼어있다..
		TRACE( "메모리 릴리즈를 한 후에 로드해주세요.\n" );
		Destroy();
	}

	RwImage* pImage = RwImageRead( pFileName );
	if( !pImage )	return FALSE;

	m_pImage = pImage;
	return TRUE;
}

BOOL	AuRwImage::Create( INT32 nWidth , INT32 nHeight , INT32 nDepth , BOOL bAllocPixel )
{
	Destroy();

	m_pImage = RwImageCreate( nWidth , nHeight , nDepth );

	if( NULL == m_pImage )
	{
		TRACE( "얼록 이미지 실패\n" );
		return FALSE;
	}

	if( bAllocPixel )
	{
		VERIFY( RwImageAllocatePixels( m_pImage ) );
	}

	return TRUE;
}

void	AuRwImage::Destroy()
{
	if( m_pImage )
	{
		RwImageDestroy( m_pImage );
		m_pImage = 0;
	}

	m_nCount	= 0;
}

RwRaster *	AuRwImage::CheckOutRaster( INT32 nFlag )
{
	ASSERT( NULL != m_pImage );
	if( NULL == m_pImage ) return NULL;
	if( NULL != m_pRaster )
	{
		++m_nCount;	// CheckOut Count
		return m_pRaster;
	}

	RwRaster	* pRaster;
	pRaster	= RwRasterCreate(
		RwImageGetWidth( m_pImage ) ,
		RwImageGetWidth( m_pImage ) ,
		RwImageGetDepth( m_pImage ) ,
		nFlag						);

	// ASSERT( NULL != pRaster );
	if( NULL == pRaster ) return NULL;

	if( RwRasterSetFromImage( pRaster , m_pImage ) )
	{
		// 레스터리턴.
		// 사용하고 디스트로이 꼭 할것!.
		++m_nCount;	// CheckOut Count
		m_pRaster = pRaster;
		return m_pRaster;
	}
	else
	{
		TRACE("예상치 못한 오류 바생\n" );
		VERIFY( RwRasterDestroy( pRaster ) );
		return NULL;
	}
}

void		AuRwImage::CheckInRaster	()
{
	if( NULL == m_pRaster ) return;

	ASSERT( m_nCount > 0 );

	--m_nCount;
	if( m_nCount )
	{
		// 카운트가 남아있으니..
		return;
	}
	else
	{
		VERIFY( RwRasterDestroy( m_pRaster ) );
		m_pRaster = NULL;
		return;
	}
}


RwRaster *	AuRwImage::GetSubRaster( int x , int y , int cx , int cy , INT32 nFlag )
{
	ASSERT( NULL != m_pImage );
	if( NULL == m_pImage ) return NULL;

	ASSERT( x		<= RwImageGetWidth( m_pImage ) );
	ASSERT( y		<= RwImageGetWidth( m_pImage ) );
	ASSERT( x + cx	<= RwImageGetWidth( m_pImage ) );
	ASSERT( y + cy	<= RwImageGetWidth( m_pImage ) );

	RwRaster * pImageRaster = CheckOutRaster();
	ASSERT( pImageRaster );
	if( !pImageRaster )		return NULL;

	// 서브레스터 생성..
	RwRaster* pSubRaster = RwRasterCreate( 0 , 0 , 8 , nFlag );
	VERIFY( pSubRaster );
	RwRect rc = { x, y, cx, cy };
	RwRasterSubRaster( pSubRaster, pImageRaster , &rc );
	VERIFY( pSubRaster );

	CheckInRaster();

	return pSubRaster;
	// 서브레스터를 Create 하여 리턴한다.
	// 본 레스터는 Ref Count 관련하여 , 서브레스터는 남고
	// 본 레스터는 박살날 가능성이 높다.
	// 이거 호출하기전에 CheckOutRaster를 꼭! 호출하고
	// 서브레스터 사용후에 서브레스터와 함께 CheckInRaster를 해주도록.
}

BOOL	AuRwImage::CreateSubImage	( AuRwImage * pDestImage , int x , int y , int cx , int cy )
{
	ASSERT( NULL != pDestImage	);
	ASSERT( NULL != m_pImage	);
	ASSERT( NULL != m_pImage	);
	if( pDestImage->GetImagePointer() )
	{
		TRACE( "얼른 지우고 하셈 -_-\n" );
		pDestImage->Destroy();
	}

	ASSERT( x		<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( y		<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( x + cx	<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( y + cy	<= RwImageGetWidth( GetImagePointer() ) );

	CheckOutRaster();

	RwRaster	*	pSubRaster = GetSubRaster( x , y , cx , cy );
	ASSERT( NULL != pSubRaster );
	if( NULL == pSubRaster ) return FALSE;

	RwRaster	*	pDummyRaster = RwRasterCreate( cx , cy , GetDepth() , rwRASTERTYPENORMAL );
	ASSERT( NULL != pDummyRaster );

	RwRasterPushContext( pDummyRaster );
	RwRasterRender( pSubRaster , 0 , 0 );
	RwRasterPopContext();

	VERIFY( pDestImage->Create( cx , cy , GetDepth() ) );
	VERIFY( pDestImage->SetFromRaster( pDummyRaster ) );

	RwRasterDestroy( pSubRaster		);
	RwRasterDestroy( pDummyRaster	);
	CheckInRaster();

	return TRUE;
}
