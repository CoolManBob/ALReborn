// EventNatureSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "RegionToolDlg.h"
#include "EventNatureSettingDlg.h"
//#include "TemplateSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	TEMPLATE_LIST_FORMAT	"%03d , %s"


/////////////////////////////////////////////////////////////////////////////
// CEventNatureSettingDlg dialog


CEventNatureSettingDlg::CEventNatureSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventNatureSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventNatureSettingDlg)
	m_fRange = 0.0f;
	//}}AFX_DATA_INIT
}


void CEventNatureSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventNatureSettingDlg)
	DDX_Control(pDX, IDC_WEATHER_TEMPLATE_COMBO, m_ctlWeatherCombo);
	DDX_Text(pDX, IDC_RANGE, m_fRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventNatureSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CEventNatureSettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventNatureSettingDlg message handlers

int CEventNatureSettingDlg::DoModal() 
{
	ASSERT( !"이거 호출하면안돼염!" );
	
	return IDCANCEL;
}

UINT CEventNatureSettingDlg::DoModal( ApdEvent * pstEvent) 
{
	m_pstEvent				= pstEvent	;
	
	return CDialog::DoModal();
}

BOOL CEventNatureSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AuList< AgpdSkySet * >	* pList = g_pcsAgpmEventNature->GetSkySetList();
	AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
	AgpdSkySet				* pSkySet	;

	int						count = 0;
	CString					str;
	int						nIndex;
	while( pNode )
	{
		pSkySet	= pNode->GetData();

		str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
		nIndex = m_ctlWeatherCombo.AddString( str );

		if( ( INT32 ) m_pstEvent->m_pvData == pSkySet->m_nIndex )
		{
			// 이놈이렸다!..
			m_ctlWeatherCombo.SetCurSel( nIndex );
		}

		count ++;

		pList->GetNext( pNode );
	}

	TRACE( "CEventNatureSettingDlg::OnInitDialog %d개의 템플릿 추가.." , count );

	UpdateData( TRUE );
	// 데이타 셋팅..
	ASSERT( NULL != m_pstEvent );
	if( m_pstEvent && m_pstEvent->m_pstCondition && m_pstEvent->m_pstCondition->m_pstArea )
	{
		// 헤헤.. 값 설정..
		m_fRange = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius / 100.0f; // 미터단위 환산함..
	}
	else
	{
		// 컨디션이 설정돼어 있지 않심..
	}

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventNatureSettingDlg::OnOK() 
{
	UpdateData( TRUE );

	// 데이타 삽입 과정..
	VERIFY( g_pcsApmEventManager->SetCondition( m_pstEvent, APDEVENT_COND_AREA ) );

	m_pstEvent->m_pstCondition->m_pstArea->m_eType					= APDEVENT_AREA_SPHERE	;
	m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius	= m_fRange * 100.0f		;// 미터단위임..


	CString	str;
	m_ctlWeatherCombo .GetWindowText( str );

	int	nTemplateID;
	nTemplateID	= atoi( (LPCTSTR) str );

	m_pstEvent->m_pvData		= ( PVOID ) nTemplateID;

	TRACE( "%d 템플릿 선택 .. (%s)\n" , nTemplateID , str );

	CDialog::OnOK();
}
