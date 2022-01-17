// TemplateSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "TemplateSelectDlg.h"
#include "TemplateNameEditDlg.h"
#include "SkySettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDlg dialog


CTemplateSelectDlg::CTemplateSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTemplateSelectDlg)
	m_bTypeBGM		= FALSE	;
	m_bTypeEffect	= FALSE	;
	m_bTypeSky		= TRUE	;
	//}}AFX_DATA_INIT

	m_csAgcmEventNatureDlg	= NULL;
}


void CTemplateSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateSelectDlg)
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_ctlTemplateList);
	DDX_Check(pDX, IDC_TYPE_BGM, m_bTypeBGM);
	DDX_Check(pDX, IDC_TYPE_EFFECT, m_bTypeEffect);
	DDX_Check(pDX, IDC_TYPE_SKY, m_bTypeSky);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTemplateSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateSelectDlg)
	ON_BN_CLICKED(IDC_ADD_TEMPLATE, OnAddTemplate)
	ON_BN_CLICKED(IDC_EDIT_TEMPLATE, OnEditTemplate)
	ON_BN_CLICKED(IDC_REMOVE_TEMPLATE, OnRemoveTemplate)
	ON_BN_CLICKED(IDC_RENAME_TEMPLATE, OnRenameTemplate)
	ON_LBN_DBLCLK(IDC_TEMPLATE_LIST, OnDblclkTemplateList)
	ON_BN_CLICKED(IDC_TYPE_SKY, OnTypeSky)
	ON_BN_CLICKED(IDC_TYPE_EFFECT, OnTypeEffect)
	ON_BN_CLICKED(IDC_TYPE_BGM, OnTypeBgm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateSelectDlg message handlers

UINT CTemplateSelectDlg::DoModal( AgcmEventNatureDlg * pParent ) 
{
	ASSERT( NULL != pParent			);

	m_csAgcmEventNatureDlg	= pParent;
	
	return CDialog::DoModal();
}

void CTemplateSelectDlg::OnAddTemplate() 
{
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	CTemplateNameEditDlg	dlg;

	dlg.m_strName	= "템플릿 이름을 적어주세염";

	if( IDOK == dlg.DoModal() )
	{
		// 이름 등록

		AgpdSkySet * pSkySet = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->CreateSkySet();
		ASSERT( NULL != pSkySet );

		strncpy( pSkySet->m_strName , (LPCTSTR) dlg.m_strName , AGPDSKYSET_MAX_NAME );
		pSkySet->m_nIndex	= m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetEmptySkySetIndex();

		// 이름은 중복이 가능하다.. 조심!..

		VERIFY( m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->AddSkySet( pSkySet ) );
		
		CString	str;
		str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
		INT32	nIndex = m_ctlTemplateList.AddString( str );

		ASSERT( LB_ERR		!= nIndex );
		ASSERT( LB_ERRSPACE	!= nIndex );
		if( LB_ERR		== nIndex	||	LB_ERRSPACE	== nIndex )
		{
			// ERROR!
			TRACE( "리스트박스에 추가 실패\n" );
			return;
		}

		m_ctlTemplateList.SetCurSel( nIndex );

		// 추가한 뒤에 작업은 여기서...
		// Sky 설정 Dialog가 떠야하지 않을까나..
		
		AgcdSkySet *		pAgcdSkySet		;
		pAgcdSkySet	= m_csAgcmEventNatureDlg->m_pcsAgcmEventNature->GetSkySetClientData	( pSkySet );

		for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
		{
			pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag	= ASkySetting::DA_NONE;

			if( dlg.m_bUseLight		) pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_LIGHT	;
			if( dlg.m_bUseFog		) pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_FOG		; 
			if( dlg.m_bUseCloud		) pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_CLOUD	;
			if( dlg.m_bUseEffect	) pAgcdSkySet->m_aSkySetting[ i ].nDataAvailableFlag |= ASkySetting::DA_EFFECT	;
		}
		
	}
}

void CTemplateSelectDlg::OnRenameTemplate() 
{
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	
	CString	str;
	UINT	nListIndex = m_ctlTemplateList.GetCurSel();

	if( nListIndex == LB_ERR )
	{
		MessageBox( "리스트에서 선택하고 눌러엽!" );
		return;
	}

	m_ctlTemplateList.GetText( nListIndex , str );

	int	nTemplateID;
	nTemplateID	= atoi( (LPCTSTR) str );

	AgpdSkySet	* pSkySet = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetSkySet( nTemplateID );

	ASSERT( NULL != pSkySet );

	if( NULL != pSkySet )
	{
		// 이름변경 작업..
		CTemplateNameEditDlg	dlg;
		dlg.m_strName	= pSkySet->m_strName;
		if( IDOK == dlg.DoModal() )
		{
			m_ctlTemplateList.DeleteString( nListIndex );
			strncpy( pSkySet->m_strName , (LPCTSTR) dlg.m_strName , AGPDSKYSET_MAX_NAME );

			CString	str;
			str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
			m_ctlTemplateList.AddString( str );
		}
	}
	else
	{
		// -_-;;;
	}	
}

void CTemplateSelectDlg::OnEditTemplate() 
{
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	
	CString	str;
	UINT	nListIndex = m_ctlTemplateList.GetCurSel();

	if( nListIndex == LB_ERR )
	{
		MessageBox( "리스트에서 선택하고 눌러엽!" );
		return;
	}

	m_ctlTemplateList.GetText( nListIndex , str );

	int	nTemplateID;
	nTemplateID	= atoi( (LPCTSTR) str );

	AgpdSkySet	* pSkySet = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetSkySet( nTemplateID );

	//int	nPrevTemplateID = m_csAgcmEventNatureDlg->m_pcsAgcmEventNature->GetSkyTemplateID();
	// m_csAgcmEventNatureDlg->m_pcsAgcmEventNature->SetSkyTemplateID( nTemplateID );

	ASSERT( NULL != pSkySet );

	if( NULL != pSkySet )
	{
		// 설정 변화창 띄움!...

		CSkySettingDlg	dlg;

		// 설정 저장해둠..

		if( IDOK == dlg.DoModal( nTemplateID , m_csAgcmEventNatureDlg ) )
		{
			// 설정 ok 처리..

			UpdateList();
		}
		else
		{
			// 취소처리..
		}
		
	}
	else
	{
		// -_-;;;
	}

	// m_csAgcmEventNatureDlg->m_pcsAgcmEventNature->SetSkyTemplateID( nPrevTemplateID );
}

void CTemplateSelectDlg::OnRemoveTemplate() 
{
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	// TODO: Add your control notification handler code here
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	
	CString	str;
	UINT	nListIndex = m_ctlTemplateList.GetCurSel();

	if( nListIndex == LB_ERR )
	{
		MessageBox( "리스트에서 선택하고 눌러엽!" );
		return;
	}

	m_ctlTemplateList.GetText( nListIndex , str );

	int	nTemplateID;
	nTemplateID	= atoi( (LPCTSTR) str );

	AgpdSkySet	* pSkySet = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetSkySet( nTemplateID );

	ASSERT( NULL != pSkySet );

	if( NULL != pSkySet )
	{
		//  삭제작업..
		m_ctlTemplateList.DeleteString( nListIndex );
		m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->RemoveSkySet( pSkySet );
	}
	else
	{
		// -_-;;;
	}	
}

void CTemplateSelectDlg::OnOK() 
{
	ASSERT( NULL != m_csAgcmEventNatureDlg );
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CTemplateSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateList();
	
	// 윈도우 위치 수정...
	CRect	rect;
	GetWindowRect(	rect );

	int	nWidth	= GetSystemMetrics( SM_CXSCREEN );
	int nHeight	= GetSystemMetrics( SM_CYSCREEN );

	CRect	newRect;
	newRect.left	=	nWidth	- rect.Width()			;
	newRect.top		=	nHeight	- rect.Height()	- 100	;
	newRect.right	=	nWidth							;
	newRect.bottom	=	newRect.top + rect.Height()		;

	MoveWindow( newRect , TRUE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CTemplateSelectDlg::OnDblclkTemplateList() 
{
	OnEditTemplate();	
}

void CTemplateSelectDlg::OnTypeSky() 
{
	m_bTypeSky		= TRUE;
	m_bTypeEffect	= FALSE;
	m_bTypeBGM		= FALSE;

	UpdateData( FALSE );
	UpdateList();
}

void CTemplateSelectDlg::OnTypeEffect() 
{
	m_bTypeSky		= FALSE;
	m_bTypeEffect	= TRUE;
	m_bTypeBGM		= FALSE;

	UpdateData( FALSE );
	UpdateList();
}

void CTemplateSelectDlg::OnTypeBgm() 
{
	m_bTypeSky		= FALSE;
	m_bTypeEffect	= FALSE;
	m_bTypeBGM		= TRUE;

	UpdateData( FALSE );
	UpdateList();	
}

void CTemplateSelectDlg::UpdateList()
{
	// List 에다가 템플릿 삽입.
	UpdateData( TRUE );
	m_ctlTemplateList.ResetContent();

	CString	str;
	
	AuList< AgpdSkySet * > * pList = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetSkySetList	();

	ASSERT( NULL != pList );

	AuNode< AgpdSkySet * > * pNode = pList->GetHeadNode();
	AgpdSkySet	* pSkySet;
	AgcdSkySet	* pSkySetClient;

	while( pNode )
	{
		pSkySet	= pNode->GetData();

		VERIFY( pSkySetClient	= m_csAgcmEventNatureDlg->m_pcsAgcmEventNature->GetSkySetClientData	( pSkySet ) );

		str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );

		if(
			( m_bTypeSky	&& pSkySetClient->m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_LIGHT		)	||
			( m_bTypeSky	&& pSkySetClient->m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_FOG		)	||
			( m_bTypeSky	&& pSkySetClient->m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_CLOUD		)	||
			( m_bTypeEffect	&& pSkySetClient->m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_EFFECT	)	||
			( m_bTypeBGM	&& pSkySetClient->m_aSkySetting[ 0 ].nDataAvailableFlag & ASkySetting::DA_MUSIC		)	)
		{
			m_ctlTemplateList.AddString( str );
		}

		pList->GetNext( pNode )	;
	}
}
