#include "AgcmSocialAction.h"
#include <fstream>

//---------------------- SocialAction --------------------------
SocialAction::SocialAction( char* szIcon, char* szCommand, char* szTooltipTitle, char* szTooltipText ) : 
 pGridItem(NULL),
 pTexture(NULL)
{
	strcpy( this->szIcon, szIcon );
	strcpy( this->szCommand, szCommand );
	strcpy( this->szTooltipTitle, szTooltipTitle );
	strcpy( this->szTooltipText, szTooltipText );
}

AgcmSocialActon::AgcmSocialActon()
{
	m_vecInfo.reserve( SOCIALACTION_MAX );
	//m_vecInfo.push_back( new SocialAction("SocialGreeting.png", " ", "인사", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialCelebration.png", " ", "축하", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialGratitude.png", " ", "감사", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialEncouragement.png", " ", "격려", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDisregard.png", " ", "무시", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDancing.png", " ", "춤추기", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDoziness.png", " ", "졸기", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialStretch.png", " ", "기지개", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialLaugh.png", " ", "웃기", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialWeeping.png", " ", "울기", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialPout.png", " ", "분노", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialRage.png", " ", "토라짐", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialApology.png", " ", "사과", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialToast.png", " ", "건배", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialCheer.png", " ", "환호", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialRush.png", " ", "돌격", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialSit.png", " ", "앉기", " " ) );
}																  

AgcmSocialActon::~AgcmSocialActon()
{
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr )
	{
		if( (*Itr) )
		{
			delete (*Itr);
			(*Itr) = NULL;
		}
	}
	m_vecInfo.clear();
}

BOOL	AgcmSocialActon::Create()
{
	//Texture를 로드해야겠군 ㅎㅎ
	m_pcsAgcmResourceLoader->SetTexturePath( "Texture\\UI\\BASE" );

	m_pcsAgpmGrid->Init( &m_cGrid, 1, 5, 4 );
	m_cGrid.m_lGridType = AGPDGRID_TYPE_SOCIALACTION;
	m_pcsAgpmGrid->Reset( &m_cGrid );

	int nCount = 0;
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr, ++nCount )
	{
		(*Itr)->pGridItem	= m_pcsAgpmGrid->CreateGridItem();
		(*Itr)->pTexture	= RwTextureRead( (*Itr)->szIcon, NULL );

		if( (*Itr)->pTexture )
		{
			RwTextureSetFilterMode( (*Itr)->pTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( (*Itr)->pTexture, rwTEXTUREADDRESSCLAMP );
			(*m_pcsAgcmUIControl->GetAttachGridItemTextureData( (*Itr)->pGridItem )) = (*Itr)->pTexture;
		}

		(*Itr)->pGridItem->m_eType		= AGPDGRID_ITEM_TYPE_SOCIALACTION;
		(*Itr)->pGridItem->m_bMoveable	= TRUE;
		(*Itr)->pGridItem->m_lItemID	= nCount;
		int nRow = nCount / 4, nColum = nCount % 4;
		m_pcsAgpmGrid->Add( &m_cGrid, 0, nRow, nColum, (*Itr)->pGridItem, 1, 1 );
	}

	//Tooltip Setting
	m_cTooltip.m_Property.bTopmost = TRUE;
	m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)&m_cTooltip );
	m_cTooltip.ShowWindow( FALSE );

	return TRUE;
}

void	AgcmSocialActon::Destory()
{
	//Texture를 쭉 지워줘야 한다..
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr )
	{
		m_pcsAgpmGrid->DeleteGridItem( (*Itr)->pGridItem );
		delete (*Itr);
	}
	m_vecInfo.clear();

	m_pcsAgpmGrid->Clear( &m_cGrid, 1, 5, 4, 1, 1 );
	m_pcsAgpmGrid->Remove( &m_cGrid );
}

BOOL	AgcmSocialActon::Load( char* szFilename, BOOL bDecryption )
{
	if( !szFilename )				return FALSE;
	if( !strlen( szFilename ) )		return FALSE;
	
	AuExcelTxtLib	cExcel;
	if( !cExcel.OpenExcelFile( szFilename, TRUE, bDecryption ) )
		return FALSE;

	for( int nRow = 1; nRow < cExcel.GetRow(); ++nRow )
		m_vecInfo.push_back( new SocialAction( cExcel.GetData( 1, nRow ), cExcel.GetData( 2, nRow ), cExcel.GetData( 2, nRow )+1, cExcel.GetData( 3, nRow ) ) );

	return TRUE;
}

void	AgcmSocialActon::SetModule( AgpmGrid* pAgpmGrid, AgcmResourceLoader* pResourceLoader, AgcmUIControl* pUIControl, AgcmUIManager2* pUIManager2 )
{
	m_pcsAgpmGrid				= pAgpmGrid;
	m_pcsAgcmResourceLoader		= pResourceLoader;
	m_pcsAgcmUIControl			= pUIControl;
	m_pcsAgcmUIManager2			= pUIManager2;
}

BOOL	AgcmSocialActon::OpenTooltip( int nIndex )
{
	const SocialAction* pInfo = GetInfo( nIndex );
	if( !pInfo )	return FALSE;

	int nX = (int)m_pcsAgcmUIManager2->m_v2dCurMousePos.x + 30;
	int nY = (int)m_pcsAgcmUIManager2->m_v2dCurMousePos.y + 30;
	m_cTooltip.MoveWindow( nX, nY, m_cTooltip.w, m_cTooltip.h );

	//Setting String
	m_cTooltip.AddString( (CHAR*)pInfo->szTooltipTitle, 14, 0xffffffff );
	m_cTooltip.AddNewLine( 14 );
	m_cTooltip.AddString( (CHAR*)pInfo->szTooltipText, 12, 0xffffffff );
	m_cTooltip.AddNewLine( 14 );

	m_cTooltip.ShowWindow( TRUE );

	return TRUE;
}

void	AgcmSocialActon::CloseTooltip()
{
	m_cTooltip.DeleteAllStringInfo();
	m_cTooltip.ShowWindow( FALSE );
}