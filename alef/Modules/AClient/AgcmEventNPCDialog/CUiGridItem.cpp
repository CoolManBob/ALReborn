#include "CUiGridItem.h"
#include "AgpmItem.h"
#include "AgcmItem.h"



#define UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE			54



CUiGridSlot::CUiGridSlot( void )
{
	AcUIBase::AcUIBase();

	m_nSlotIndex = 0;
	m_eSlotItemType = UiGridItem_UnKnown;

	m_nTID = -1;
	m_nlID = -1;

	m_pSlotItemTexture = NULL;
}

CUiGridSlot::~CUiGridSlot( void )
{
}

void CUiGridSlot::OnWindowRender( void )
{
	AcUIBase::OnWindowRender();

	if( m_pSlotItemTexture )
	{
		RwTexture* pTexture = ( RwTexture* )m_pSlotItemTexture;

		float fPosX = ( float )m_lAbsolute_x + m_lImageOffsetX;
		float fPosY = ( float )m_lAbsolute_y + m_lImageOffsetY;
		unsigned char nAlpha = ( unsigned char )( m_stRenderInfo.cRenderAlpha * ( m_pfAlpha ? *m_pfAlpha : 1 ) );
		float fWidth = ( float )( RwRasterGetWidth( pTexture->raster ) ) * m_stRenderInfo.fScale;
		float fHeight = ( float )( RwRasterGetHeight( pTexture->raster ) ) * m_stRenderInfo.fScale;

		g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE, UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE,
			0.0, 0.0, 1.0, 1.0, m_stRenderInfo.lRenderColor, nAlpha );
	}
}

BOOL CUiGridSlot::OnDragDrop( void* pParam1, void* pParam2 )
{
	if( !m_pAgcmUIControl ) return FALSE;

	AcUIGridItem* pGridItem	= ( AcUIGridItem* )pParam2;
	if( !pGridItem ) return FALSE;

	AgpdGridItem* ppdGridItem = pGridItem->m_ppdGridItem;
	if( !ppdGridItem ) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )m_pAgcmUIControl->GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )m_pAgcmUIControl->GetModule( "AgcmItem" );
	if( !ppmItem || !pcmItem ) return FALSE;

	m_eSlotItemType = UiGridItem_Item;
	
	m_nTID = ppdGridItem->m_lItemTID;
	m_nlID = ppdGridItem->m_lItemID;

	AgpdItem* ppdItem = ppmItem->GetItem( m_nlID );
	if( !ppdItem ) return FALSE;

	AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItem->m_pcsItemTemplate );
	if( !pcdItemTemplate ) return FALSE;

	m_pSlotItemTexture = pcdItemTemplate->m_pTexture;
	if( !m_pSlotItemTexture ) return FALSE;

	return TRUE;
}

void CUiGridSlot::OnClear( void )
{
	m_eSlotItemType = UiGridItem_UnKnown;

	m_nTID = -1;
	m_nlID = -1;

	m_pSlotItemTexture = NULL;
}

BOOL CUiGridSlot::Initialize( int nMsgBoxWidth, int nMsgBoxHeight )
{
	m_nSlotIndex = 0;
	m_eSlotItemType = UiGridItem_UnKnown;

	m_nTID = -1;
	m_nlID = -1;

	m_pSlotItemTexture = NULL;
	m_stRenderInfo.lRenderID = AddImage( "ExNPCMsgBoxGridBG.bmp" ) + 1;
	m_Property.bUseInput = TRUE;

	int nPosX = ( nMsgBoxWidth / 2 ) - ( UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE / 2 );
	int nPosY = ( nMsgBoxHeight / 2 ) - ( UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE / 2 );

	MoveWindow( nPosX, nPosY, UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE, UI_EXNPC_MSGBOX_GRIDITEM_ICONSIZE );
	ShowWindow( TRUE );
	return TRUE;
}
