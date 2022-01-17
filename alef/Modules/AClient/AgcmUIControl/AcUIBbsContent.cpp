#include "AcUIBbsContent.h"

extern AgcEngine *	g_pEngine;

/*****************************************************************
*   Function : AcUIBbsContent
*   Comment  : AcUIBbsContent Constructor
*   Date&Time : 2003-03-12, 오전 11:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIBbsContent::AcUIBbsContent()
{
	m_pszContentText		=		NULL	;
	m_rectDrawContent.x		=		0		;
	m_rectDrawContent.y		=		0		;
	m_rectDrawContent.w		=		0		;
	m_rectDrawContent.h		=		0		;
}

/*****************************************************************
*   Function : ~AcUIBbsContent
*   Comment  : AcUIBbsContent Destructor
*   Date&Time : 2003-03-12, 오전 11:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcUIBbsContent::~AcUIBbsContent()
{

}

/*****************************************************************
*   Function : OnPostRender
*   Comment  : On PostRender virtual function
*   Date&Time : 2003-03-12, 오전 11:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIBbsContent::OnPostRender	( RwRaster* raster	)
{
/*	RwRect rect = { 0, 0, w, h };

	ClientToScreen( &(rect.x), &(rect.y) );

	if( m_listImage.GetCount() > 0 )
	{
		RwRaster * pImageRaster = m_listImage.GetHead();

		ASSERT( NULL != pImageRaster );

		RwRasterPushContext	( raster				);
		RwRasterRenderScaled( pImageRaster, &rect	);
		RwRasterPopContext	(						);
	}
	
	//Valid Checking 
	if ( NULL == m_pszContentText ) return;
	else if ( 0 >= m_rectDrawContent.w || 0 >= m_rectDrawContent.h ) return;

	//Draw
	DrawContentText( raster );	*/
}

/*****************************************************************
*   Function : OnWindowRender
*   Comment  : On Window Render 
*   Date&Time : 2003-06-24, 오전 11:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIBbsContent::OnWindowRender	(					)
{
	PROFILE("AcUIBbsContent::OnWindowRender");

	int nAbsolute_x = 0;
	int nAbsolute_y = 0;
	ClientToScreen( &nAbsolute_x, &nAbsolute_y );

	if( m_csTextureList.GetCount() > 0 )
	{
		stTextureListInfo * pTextureListInfo = m_csTextureList.GetHeadTextureListInfo();

		ASSERT( NULL != pTextureListInfo );

		if ( pTextureListInfo->pTexture != NULL )
			g_pEngine->DrawIm2D( pTextureListInfo->pTexture, (float)nAbsolute_x, (float)nAbsolute_y, (float)w, (float)h,
			0.0f, 0.0f, 1.0f, 1.0f, 0xffffffff, ( UINT8 ) ( 255.0f * (m_pfAlpha ? *m_pfAlpha : 1) ));
	}
	
	//Valid Checking 
	if ( NULL == m_pszContentText ) return;
	else if ( 0 >= m_rectDrawContent.w || 0 >= m_rectDrawContent.h ) return;

	//Draw
	DrawContentText();
}

/*****************************************************************
*   Function : OnClose
*   Comment  : On Close virtual fuction
*   Date&Time : 2003-03-12, 오전 11:26
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIBbsContent::OnClose			(					)
{
	AcUIBase::OnClose();

	ReleaseContentTextMemory();
	m_csTextureList.DestroyTextureList();
}

/*****************************************************************
*   Function : SetContentText
*   Comment  : Set Content Text 
*   Date&Time : 2003-03-12, 오전 11:28
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIBbsContent::SetContentText( char* pszContent, UINT16 nContentLength )
{
	if ( NULL != m_pszContentText )
	{
		free( m_pszContentText );
	}

	m_pszContentText = (char*)malloc( sizeof(char) * ( nContentLength + 1 ) );
	
	strncpy( m_pszContentText, pszContent, (nContentLength + 1) );

	return TRUE;
}

/*****************************************************************
*   Function : DrawContentText
*   Comment  : Draw Content text at Post Render 
*   Date&Time : 2003-03-12, 오전 11:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIBbsContent::DrawContentText( )
{
//	return HanFontDrawTextRect_MultiLine( raster, &m_rectDrawContent, m_pszContentText );

	return TRUE;
}

/*****************************************************************
*   Function : SetDrawRect
*   Comment  : SetDrawRect
*   Date&Time : 2003-03-12, 오후 8:09
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIBbsContent::SetDrawRect( RwRect rect )
{
	ClientToScreen( &rect.x , &rect.y );

	m_rectDrawContent = rect;
}

/*****************************************************************
*   Function : ReleaseContentTextMemory
*   Comment  : ReleaseContentTextMemory
*   Date&Time : 2003-03-12, 오후 9:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcUIBbsContent::ReleaseContentTextMemory()
{
	if ( NULL != m_pszContentText )
	{
		free( m_pszContentText );
	}
}

/*****************************************************************
*   Function : OnInit
*   Comment  : On Initialize virtual function
*   Date&Time : 2003-03-13, 오전 11:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AcUIBbsContent::OnInit(	)
{
	AcUIBase::OnInit();

	m_clUIButton.MoveWindow( 300, 300 , m_clUIButton.w, m_clUIButton.h );
	AddChild( &m_clUIButton, ACUIBBSCONTENT_CLOSE_BUTTON );

	return TRUE;
}

BOOL	AcUIBbsContent::OnCommand		( INT32	nID , PVOID pParam	)
{
	INT32* pUIID = (INT32*)pParam;

	switch( *pUIID )
	{
		case ACUIBBSCONTENT_CLOSE_BUTTON:
			if ( UICM_BUTTON_MESSAGE_CLICK == nID )
			{
				ShowWindow( FALSE );
				return TRUE;
			}
			break;
	}
	
	return TRUE;
}