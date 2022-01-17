// AttachHair.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "AttachHairDlg.h"
#include ".\attachhairdlg.h"
#include "ModelToolDlg.h"


// CAttachHair dialog

IMPLEMENT_DYNAMIC(CAttachHairDlg, CDialog)
CAttachHairDlg::CAttachHairDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttachHairDlg::IDD, pParent)
{
}

CAttachHairDlg::~CAttachHairDlg()
{
}

void CAttachHairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ATTACH_HAIR_LIST, m_ctlHairList);
}

BEGIN_MESSAGE_MAP(CAttachHairDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_LBN_DBLCLK(IDC_ATTACH_HAIR_LIST, OnLbnDblclkAttachHairList)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CAttachHairDlg message handlers
BOOL CAttachHairDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CAttachHairDlg::OnBnClickedOk()
{
	OnOK();
}

void CAttachHairDlg::OnBnClickedCancel()
{
	//OnCancel();
}

void CAttachHairDlg::ClearDefaultHair()
{
	while( m_ctlHairList.GetCount() )
	{
		m_ctlHairList.DeleteString( 0 );
	}
}

void CAttachHairDlg::OnLbnDblclkAttachHairList()
{
	int nID = CModelToolApp::GetInstance()->GetCurrentID();

	AgpmCharacter* pAgpmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return;

	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( pCharTemplate->m_pcsDefaultHeadData == NULL )
		return;

	int nIdx = m_ctlHairList.GetCurSel();

	pAgcmCharacter->SetHair( pcsAgcdCharacter, nIdx );

#ifdef USE_MFC
	pAgcmCharacter->m_bisSaveHairClump = TRUE;
#endif
}

void CAttachHairDlg::SetDefaultHair()
{
	int nID = CModelToolApp::GetInstance()->GetCurrentID();
	AgpdCharacter *pcsAgpdCharacter	= CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule()->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return;

	AgcdCharacter *pcsAgcdCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;

	ClearDefaultHair();

	if( pCharTemplate->m_pcsDefaultHeadData == NULL )
		return;

	vector< string >& vecString = pCharTemplate->m_pcsDefaultHeadData->vecHairInfo;
	for( vector< string >::iterator Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		m_ctlHairList.AddString( (*Itr).c_str() );
}

void CAttachHairDlg::OnClose()
{
	CDialog::OnClose();

	CModelToolDlg::GetInstance()->CloseAttachHairDlg();
}
