// CustomizePreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "CustomizePreviewDlg.h"
#include ".\customizepreviewdlg.h"

#include "AgcmCustomizeRender.h"

// CCustomizePreviewDlg dialog

IMPLEMENT_DYNAMIC(CCustomizePreviewDlg, CDialog)
CCustomizePreviewDlg::CCustomizePreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomizePreviewDlg::IDD, pParent)
{
	m_pAgcmCustomizeRender = NULL;
	ZeroMemory( m_bKey, sizeof(m_bKey) );

	m_fCameraY = 15.f;
	m_fCameraZ = 80.f;

	m_fTargetY = 0.f;
}

CCustomizePreviewDlg::~CCustomizePreviewDlg()
{
}

void CCustomizePreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCustomizePreviewDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CUSTOMIZE_PREVIEW_PREVIEW, OnBnClickedCustomizePreviewPreview)
	ON_BN_CLICKED(IDC_CUSTOMIZE_PREVIEW_SETTING, OnBnClickedCustomizePreviewSetting)
	ON_BN_CLICKED(IDC_CUSTOMIZE_PREVIEW_SETNEAR, OnBnClickedCustomizePreviewSetnear)
	ON_BN_CLICKED(IDC_CUSTOMIZE_PREVIEW_SETFAR, OnBnClickedCustomizePreviewSetfar)
END_MESSAGE_MAP()

BOOL CCustomizePreviewDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CCustomizePreviewDlg::Start()
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

	m_pAgcmCustomizeRender = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCustomizeRenderModule();
	m_pAgcmCustomizeRender->SetState( AgcmCustomizeRender::STATE_SETTING );
	m_pAgcmCustomizeRender->Start( pcsAgcdCharacter );

	ZeroMemory( m_bKey, sizeof(m_bKey) );

	m_fCameraY = 0;
	m_fCameraZ = 0;
}

void CCustomizePreviewDlg::End()
{
	if( m_pAgcmCustomizeRender ) {
		m_pAgcmCustomizeRender->End();
	}
}

void CCustomizePreviewDlg::OnIdle()
{
	SHORT sUP	= GetAsyncKeyState( VK_UP );
	SHORT sDOWN = GetAsyncKeyState( VK_DOWN );
	SHORT sA	= GetAsyncKeyState( 'A' );
	SHORT sZ	= GetAsyncKeyState( 'Z' );
	SHORT sS	= GetAsyncKeyState( 'S' );
	SHORT sX	= GetAsyncKeyState( 'X' );

	SHORT sLeft = GetAsyncKeyState( VK_LEFT );
	SHORT sRight= GetAsyncKeyState( VK_RIGHT );

	switch( m_pAgcmCustomizeRender->GetState() )
	{
	case AgcmCustomizeRender::STATE_SETTING:
		{
			if( sUP ) {
				m_fCameraY -= 0.1f;
				m_pAgcmCustomizeRender->SetCameraPosY( m_fCameraY );
			}
			if( sDOWN ) {
				m_fCameraY += 0.1f;
				m_pAgcmCustomizeRender->SetCameraPosY( m_fCameraY );
			}
			if( sA ) {
				m_fCameraZ -= 0.1f;
				m_pAgcmCustomizeRender->SetCameraPosZ( m_fCameraZ );
			}
			if( sZ ) {
				m_fCameraZ += 0.1f;
				m_pAgcmCustomizeRender->SetCameraPosZ( m_fCameraZ );
			}
			if( sS ) {
				m_fTargetY -= 0.1f;
				m_pAgcmCustomizeRender->SetTargetPosY( m_fTargetY );
			}
			if( sX ) {
				m_fTargetY += 0.1f;
				m_pAgcmCustomizeRender->SetTargetPosY( m_fTargetY );
			}
		}
		break;
	case AgcmCustomizeRender::STATE_PREVIEW:
		{
			if( sA ) {
				m_pAgcmCustomizeRender->ZoomInOn();
			}
			else {
				m_pAgcmCustomizeRender->ZoomInOff();
			}
			if( sZ ) {
				m_pAgcmCustomizeRender->ZoomOutOn();
			}
			else {
				m_pAgcmCustomizeRender->ZoomOutOff();
			}

			if( sS ) {
				m_pAgcmCustomizeRender->AtUpOn();
			}
			else {
				m_pAgcmCustomizeRender->AtUpOff();
			}
			if( sX ) {
				m_pAgcmCustomizeRender->AtDownOn();
			}
			else {
				m_pAgcmCustomizeRender->AtDownOff();
			}

			if( sLeft ) {
				m_pAgcmCustomizeRender->LeftTurnOn();
			}
			else {
				m_pAgcmCustomizeRender->LeftTurnOff();
			}
			if( sRight ) {
				m_pAgcmCustomizeRender->RightTurnOn();
			}
			else {
				m_pAgcmCustomizeRender->RightTurnOff();
			}
		}
		break;
	}
}

// CCustomizePreviewDlg message handlers
void CCustomizePreviewDlg::OnClose()
{
	CDialog::OnClose();
	CModelToolDlg::GetInstance()->CloseCustomizePreviewDlg();
}

void CCustomizePreviewDlg::OnBnClickedOk()
{
}

void CCustomizePreviewDlg::OnBnClickedCancel()
{
	//OnCancel();
}

void CCustomizePreviewDlg::OnBnClickedCustomizePreviewPreview()
{
	m_pAgcmCustomizeRender->SetState( AgcmCustomizeRender::STATE_PREVIEW );
}

void CCustomizePreviewDlg::OnBnClickedCustomizePreviewSetting()
{
	m_pAgcmCustomizeRender->SetState( AgcmCustomizeRender::STATE_SETTING );
}

void CCustomizePreviewDlg::OnBnClickedCustomizePreviewSetnear()
{
	if( m_pAgcmCustomizeRender->GetState() == AgcmCustomizeRender::STATE_SETTING ) {
		m_pAgcmCustomizeRender->SetPreviewNear();
	}
}

void CCustomizePreviewDlg::OnBnClickedCustomizePreviewSetfar()
{
	if( m_pAgcmCustomizeRender->GetState() == AgcmCustomizeRender::STATE_SETTING ) {
		m_pAgcmCustomizeRender->SetPreviewFar();
	}
}
