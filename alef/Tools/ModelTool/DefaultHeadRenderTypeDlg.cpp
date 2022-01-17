// DefaultHeadRenderTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "DefaultHeadRenderTypeDlg.h"
#include ".\defaultheadrendertypedlg.h"

#include "AgcmUDADlg.h"

// CDefaultHeadRenderTypeDlg dialog

IMPLEMENT_DYNAMIC(CDefaultHeadRenderTypeDlg, CDialog)
CDefaultHeadRenderTypeDlg::CDefaultHeadRenderTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefaultHeadRenderTypeDlg::IDD, pParent)
{
	m_nState = STATE_NONE;
}

CDefaultHeadRenderTypeDlg::~CDefaultHeadRenderTypeDlg()
{
}

void CDefaultHeadRenderTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RENDER_TYPE, m_ctlRenderTypeCombo);
	DDX_Control(pDX, IDC_COMBO_BLEND_MODE, m_ctlBlendModeCombo);
}


BEGIN_MESSAGE_MAP(CDefaultHeadRenderTypeDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDefaultHeadRenderTypeDlg::SetState( int nState )
{
	m_nState = nState;
}


// CDefaultHeadRenderTypeDlg message handlers
BOOL CDefaultHeadRenderTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!AgcmUDADlg::GetInstance())
		return FALSE;

	if (!AgcmUDADlg::GetInstance()->GetAgcmRenderModule())
		return FALSE;

	CHAR		*pszTemp;
	INT32		nIndex;
	for( nIndex = 0; nIndex < R_RENDER_TYPE_NUM; ++nIndex)
	{
		pszTemp = AgcmUDADlg::GetInstance()->GetAgcmRenderModule()->GetRenderTypeName( nIndex );
		m_ctlRenderTypeCombo.InsertString( nIndex, pszTemp );
	}
	for( nIndex = 0; nIndex < R_BELND_MODE_NUM; ++nIndex )
	{
		pszTemp = AgcmUDADlg::GetInstance()->GetAgcmRenderModule()->GetRenderBlendModeName( nIndex );
		m_ctlBlendModeCombo.InsertString( nIndex, pszTemp );
	}

	int nID = CModelToolApp::GetInstance()->GetCurrentID();
	AgpmCharacter* pAgpmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = CModelToolApp::GetInstance()->GetEngine()->GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return FALSE;

	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return FALSE;

	if( pcsAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return FALSE;

	AgcdCharacterTemplate* pCharTemplate = pcsAgcdCharacter->m_pstAgcdCharacterTemplate;
	if( pCharTemplate->m_pcsDefaultHeadData == NULL )
		return FALSE;

	int nRenderType, nBlendMode, nTemp;
	nRenderType = nBlendMode = nTemp = 0;
	switch( m_nState )
	{
	case STATE_FACE:
		{
			int nFaceID = pcsAgcdCharacter->m_nAttachFaceID;
			nTemp = pCharTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[nFaceID];
		}
		break;
	case STATE_HAIR:
		{
			int nHairID = pcsAgcdCharacter->m_nAttachHairID;
			nTemp = pCharTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[nHairID];
		}
		break;
	};

	nRenderType = nTemp & 0x0000FFFF;
	nBlendMode  = (nTemp & 0xFFFF0000) >> 16;

	m_ctlRenderTypeCombo.SetCurSel(nRenderType);
	m_ctlBlendModeCombo.SetCurSel(nBlendMode);

	return TRUE;
}

void CDefaultHeadRenderTypeDlg::OnBnClickedOk()
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

	int nResultRenderType, nBlendMode;
	int	nRenderType = m_ctlRenderTypeCombo.GetCurSel();

	nBlendMode = 0;
	nResultRenderType = nRenderType;
	if( (nRenderType == R_BLEND_SORT) || (nRenderType == R_BLEND_NSORT) ) {
		nBlendMode = m_ctlBlendModeCombo.GetCurSel();

		nResultRenderType = (INT32)((nBlendMode << 16) | (nRenderType));
	}

	switch( m_nState )
	{
	case STATE_FACE:
		{
			if( pcsAgcdCharacter->m_pFace == NULL )
				break;

			int nFaceID = pcsAgcdCharacter->m_nAttachFaceID;
			if( nFaceID >= (int)pCharTemplate->m_vpFace.size() )
				break;

			RpAtomic* pTemplateFaceAtomic = pCharTemplate->m_vpFace[ nFaceID ];
			pAgcmCharacter->SetCustomizeRenderType( pTemplateFaceAtomic, nResultRenderType );
			pCharTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[nFaceID] = nResultRenderType;
			pCharTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData[nFaceID] = 0;

			pAgcmCharacter->SetFace( pcsAgcdCharacter, nFaceID );
		}
		break;
	case STATE_HAIR:
		{
			if( pcsAgcdCharacter->m_pHair == NULL )
				break;

			int nHairID = pcsAgcdCharacter->m_nAttachHairID;
			if( nHairID >= (int)pCharTemplate->m_vpHair.size() )
				break;

			RpAtomic* pTemplateHairAtomic = pCharTemplate->m_vpHair[ nHairID ];
			pAgcmCharacter->SetCustomizeRenderType( pTemplateHairAtomic, nResultRenderType );
			pCharTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[nHairID] = nResultRenderType;
			pCharTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData[nHairID] = 0;

			pAgcmCharacter->SetHair( pcsAgcdCharacter, nHairID );
		}
		break;
	}

	OnOK();
}
