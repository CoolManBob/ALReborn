// AgcmEventAuctionDlgDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "	\ add additional includes here"
#include "AgcmEventAuctionDlgDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventAuctionDlgDialog dialog


CAgcmEventAuctionDlgDialog::CAgcmEventAuctionDlgDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CAgcmEventAuctionDlgDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAgcmEventAuctionDlgDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAgcmEventAuctionDlgDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAgcmEventAuctionDlgDialog)
	DDX_Control(pDX, IDC_EventAuctionStatic, m_cEventAuctionStatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAgcmEventAuctionDlgDialog, CDialog)
	//{{AFX_MSG_MAP(CAgcmEventAuctionDlgDialog)
	ON_BN_CLICKED(IDC_EventAuctionCancelButton, OnEventAuctionCancelButton)
	ON_BN_CLICKED(IDC_EventAuctionApplyButton, OnEventAuctionApplyButton)
	ON_BN_CLICKED(IDC_EventAuctionExitButton, OnEventAuctionExitButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventAuctionDlgDialog message handlers

void CAgcmEventAuctionDlgDialog::InitData( ApmObject *pcsApmObject, AgpmCharacter *pcsAgpmCharacter, AgpdEventAuction *pcsEventData )
{
	m_pcsApmObject = pcsApmObject;
	m_pcsAgpmCharacter = pcsAgpmCharacter;
	m_pcsEventData = pcsEventData;
}

void CAgcmEventAuctionDlgDialog::OnEventAuctionApplyButton() 
{
	// TODO: Add your control notification handler code here
	if( m_pcsEventData )
	{
		m_cEventAuctionStatic.SetWindowText( "옥션 이벤트가 세팅되어있음" );

		m_pcsEventData->m_bHaveAuctionEvent = TRUE;
	}
}

void CAgcmEventAuctionDlgDialog::OnEventAuctionCancelButton() 
{
	// TODO: Add your control notification handler code here
	if( m_pcsEventData )
	{
		m_cEventAuctionStatic.SetWindowText( "옥션 이벤트가 세팅되어있지 않음" );

		m_pcsEventData->m_bHaveAuctionEvent = FALSE;
	}
}

void CAgcmEventAuctionDlgDialog::OnEventAuctionExitButton() 
{
	// TODO: Add your control notification handler code here
	OnOK();	
}

BOOL CAgcmEventAuctionDlgDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if( m_pcsEventData )
	{
		if( m_pcsEventData->m_bHaveAuctionEvent )
		{
			m_cEventAuctionStatic.SetWindowText( "옥션 이벤트가 세팅되어있음" );
		}
		else
		{
			m_cEventAuctionStatic.SetWindowText( "옥션 이벤트가 세팅되어있지 않음" );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
