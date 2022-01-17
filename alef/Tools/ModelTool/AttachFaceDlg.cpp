// AttachFaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "AttachFaceDlg.h"
#include ".\attachfacedlg.h"
#include "ModelToolDlg.h"


// AttachFaceDlg dialog
IMPLEMENT_DYNAMIC(CAttachFaceDlg, CDialog)
CAttachFaceDlg::CAttachFaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAttachFaceDlg::IDD, pParent)
{
}

CAttachFaceDlg::~CAttachFaceDlg()
{
}

void CAttachFaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ATTACK_FACE_LIST, m_ctlFaceList);
}

BEGIN_MESSAGE_MAP(CAttachFaceDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_LBN_DBLCLK(IDC_ATTACK_FACE_LIST, OnLbnDblclkAttackFaceList)
	ON_WM_CLOSE()
//	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
//	ON_BN_CLICKED(IDC_LEFT_TURN, OnBnClickedLeftTurn)
//	ON_BN_CLICKED(IDC_RIGHT_TURN, OnBnClickedRightTurn)
ON_BN_CLICKED(IDC_VIEW_HELMET, OnBnClickedViewHelmet)
END_MESSAGE_MAP()


// AttachFaceDlg message handlers
BOOL CAttachFaceDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CAttachFaceDlg::OnOk()
{
	//CDialog::OnOK();
}

void CAttachFaceDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CAttachFaceDlg::ClearDefaultFace()
{
	while( m_ctlFaceList.GetCount() )
	{
		m_ctlFaceList.DeleteString( 0 );
	}
}

void CAttachFaceDlg::OnLbnDblclkAttackFaceList()
{
	int nID = CModelToolApp::GetInstance()->GetCurrentID();

	AgpmCharacter* pAgpmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( !pcsAgpdCharacter )		return;

	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return;
	if( !pcsAgcdCharacter->m_pstAgcdCharacterTemplate )		return;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( !pCharTemplate->m_pcsDefaultHeadData )				return;

	pAgcmCharacter->SetFace( pcsAgcdCharacter, m_ctlFaceList.GetCurSel() );
}

void CAttachFaceDlg::SetDefaultFace()
{
	int nID = CModelToolApp::GetInstance()->GetCurrentID();
	AgpdCharacter *pcsAgpdCharacter	= CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule()->GetCharacter( nID );
	if( !pcsAgpdCharacter )			return;
	AgcdCharacter *pcsAgcdCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule()->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )			return;
	if( !pcsAgcdCharacter->m_pstAgcdCharacterTemplate )		return; 

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( !pCharTemplate->m_pcsDefaultHeadData  )				return;

	ClearDefaultFace();

	vector< string >& vecString = pCharTemplate->m_pcsDefaultHeadData->vecFaceInfo;
	for( vector< string >::iterator Itr = vecString.begin(); Itr != vecString.end(); ++Itr )
		m_ctlFaceList.AddString( (*Itr).c_str() );
}

void CAttachFaceDlg::OnClose()
{
	CDialog::OnClose();

	CModelToolDlg::GetInstance()->CloseAttachFaceDlg();
}


BOOL g_bViewHelmet = TRUE;
void CAttachFaceDlg::OnBnClickedViewHelmet()
{
	AgcmItem* pAgcmItem = CModelToolApp::GetInstance()->GetEngine()->GetAgcmItemModule();

	int nID = CModelToolApp::GetInstance()->GetCurrentID();

	AgpmCharacter* pAgpmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( !pcsAgpdCharacter )		return;

	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return;
		

	pAgcmItem->ViewHelmet( pcsAgcdCharacter, g_bViewHelmet );

	g_bViewHelmet = !g_bViewHelmet;
}
