#include "CUICashMallTexture.h"
#include "AgpdItem.h"
#include "AgcmUIItem.h"
#include "AgcmCashMall.h"




CUICashMallTexture::CUICashMallTexture( void )
{
	m_nItemID = 0;
	m_nItemTID = 0;
	m_nStackCount = 0;

	m_ppdItem = NULL;
	m_bIsEnableToolTip = TRUE;
}

CUICashMallTexture::~CUICashMallTexture( void )
{
}

void CUICashMallTexture::OnSetFocus( void )
{
	// AcUIBase 선행처리
	AcUIBase::OnSetFocus();

	if( m_bIsEnableToolTip )
	{
		// 툴팁 열기
		AgcmUIItem* pcmUIItem = ( AgcmUIItem* )g_pEngine->GetModule( "AgcmUIItem" );
		if( !pcmUIItem ) return;

		// 툴팁이 떠야 할 포지션 잡기..
		// 이 컨트롤의 좌표에 이 컨트롤이 들어간 UI 의 좌표를 더하면 되긔~

		int nPosX = m_lAbsolute_x;
		int nPosY = m_lAbsolute_y;

		pcmUIItem->OpenToolTip( m_ppdItem, nPosX, nPosY, FALSE, FALSE );
	}
}

void CUICashMallTexture::OnKillFocus( void )
{
	// AcUIBase 선행처리
	AcUIBase::OnKillFocus();

	// 툴팁 닫기
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )g_pEngine->GetModule( "AgcmUIItem" );
	if( !pcmUIItem ) return;

	pcmUIItem->CloseToolTip();
}

void CUICashMallTexture::OnWindowRender( void )
{
	AcUIBase::OnWindowRender();
}

void CUICashMallTexture::SetItemTID( int nTID, int nStackCount )
{
	if( m_nItemTID != nTID )
	{
		AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
		AgcmCashMall* pcmCashMall = ( AgcmCashMall* )g_pEngine->GetModule( "AgcmCashMall" );
		if( !ppmItem || !pcmCashMall ) return;

		// 기존에 만들어진 더미아이템이 있으면 제거
		if( m_ppdItem )
		{
			pcmCashMall->DestroyDummyItem( m_ppdItem );
			m_nItemID = 0;
			m_ppdItem = NULL;
		}

		m_nItemTID = nTID;
		m_nStackCount = nStackCount;
		
		m_ppdItem = pcmCashMall->CreateDummyItem( m_nItemTID, m_nStackCount );
		if( m_ppdItem )
		{
			m_nItemID = m_ppdItem->m_lID;
		}
	}
}

void CUICashMallTexture::SetTexture( RwTexture* pTexture )
{
	if( !pTexture )
	{
		SetRednerTexture( 0 );
		return;
	}
	
	int nTextureID = m_csTextureList.AddTexture( pTexture );
	SetRednerTexture( nTextureID );
}

void CUICashMallTexture::SetTexture( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 )
	{
		SetRednerTexture( 0 );
		return;
	}

	int nTextureID = m_csTextureList.AddImage( pFileName );
	SetRednerTexture( nTextureID );
}