// CharAnimAttachedDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgcmAnimationDlg.h"
#include "../resource.h"
#include "CharAnimAttachedDataDlg.h"
#include "AgcmFileListDlg.h"
#include "CharAnimCustFlagsDlg.h"
#include "CharAnimAttachedSoundDataDlg.h"

// CCharAnimAttachedDataDlg dialog

IMPLEMENT_DYNAMIC(CCharAnimAttachedDataDlg, CDialog)
CCharAnimAttachedDataDlg::CCharAnimAttachedDataDlg(AgcdAnimData2 *pcsAnimData, CWnd* pParent /*=NULL*/)
	: CDialog(CCharAnimAttachedDataDlg::IDD, pParent)
	, m_unCustType(0)
	, m_unActiveRate(0)
	, m_strPoint(_T(""))
	, m_strSubAnimation(_T(""))
	, m_strBlendingAnimation(_T(""))
{
	if (AgcmAnimationDlg::GetInstance())
	{
		AgcaAnimation2	*pcsAgcaAnimation2	=
			AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->GetAgcaAnimation2();
		if (pcsAgcaAnimation2)
		{
			m_pcsACA	= (ACA_AttachedData *)(pcsAgcaAnimation2->GetAttachedData(
							AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
							pcsAnimData												));

			m_pcsAEE	= (AEE_CharAnimAttachedData *)(pcsAgcaAnimation2->GetAttachedData(
							AGCD_EVENT_EFFECT_CHAR_ATTACHED_DATA_KEY_NAME,
							pcsAnimData														));
		}
	}
}

CCharAnimAttachedDataDlg::~CCharAnimAttachedDataDlg()
{
}

void CCharAnimAttachedDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CUSTOM_TYPE, m_unCustType);
	DDX_Text(pDX, IDC_EDIT_ACTIVE_RATE, m_unActiveRate);
	DDX_Text(pDX, IDC_EDIT_POINT, m_strPoint);
	DDX_Text(pDX, IDC_EDIT_SUB_ANIMATION, m_strSubAnimation);
	DDX_Text(pDX, IDC_EDIT_BLENDING_ANIMATION, m_strBlendingAnimation);
}


BEGIN_MESSAGE_MAP(CCharAnimAttachedDataDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CUSTOM_FLAGS, OnBnClickedButtonCustomFlags)
	ON_BN_CLICKED(IDC_BUTTON_SET_BLENDING_ANIMATION, OnBnClickedButtonSetBlendingAnimation)
	ON_BN_CLICKED(IDC_BUTTON_SET_SOUND_DATA, OnBnClickedButtonSetSoundData)
	ON_BN_CLICKED(IDC_BUTTON_SET_SUB_ANIMATION, OnBnClickedButtonSetSubAnimation)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CCharAnimAttachedDataDlg message handlers
void CCharAnimAttachedDataDlg::OnBnClickedButtonCustomFlags()
{
	// TODO: Add your control notification handler code here
	//@{ 2006/09/08 burumal
	//CCharAnimCustFlagsDlg dlg(&m_pcsACA->m_unCustFlags);
	CCharAnimCustFlagsDlg dlg(&m_pcsACA->m_unCustFlags,
		&m_pcsACA->m_lClumpShowOffsetTime,
		&m_pcsACA->m_uClumpFadeInTime,
		&m_pcsACA->m_lClumpHideOffsetTime,
		&m_pcsACA->m_uClumpFadeOutTime);
	//@}
	dlg.DoModal();
}

void CCharAnimAttachedDataDlg::OnBnClickedButtonSetBlendingAnimation()
{
	// TODO: Add your control notification handler code here
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp						= dlg.OpenFileList(	AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath1()	,
													AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath2()	,
													NULL													);

	if (pszTemp)
	{
		m_strBlendingAnimation	= pszTemp;
		UpdateData(FALSE);
	}
}

void CCharAnimAttachedDataDlg::OnBnClickedButtonSetSoundData()
{
	// TODO: Add your control notification handler code here
	CCharAnimAttachedSoundDataDlg dlg(m_pcsAEE);
	dlg.DoModal();
}

void CCharAnimAttachedDataDlg::OnBnClickedButtonSetSubAnimation()
{
	// TODO: Add your control notification handler code here
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp						= dlg.OpenFileList(	AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath1()	,
													AgcmAnimationDlg::GetInstance()->GetFindCharAnimPath2()	,
													NULL													);

	if (pszTemp)
	{
		m_strSubAnimation		= pszTemp;
		UpdateData(FALSE);
	}
}

BOOL CCharAnimAttachedDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_unCustType				= m_pcsACA->m_nCustType;
	m_unActiveRate				= m_pcsACA->m_unActiveRate;
	
	if (m_pcsACA->m_pszPoint)
	{
		m_strPoint				= m_pcsACA->m_pszPoint;
	}

	if (	(m_pcsACA->m_pcsBlendingData) &&
			(m_pcsACA->m_pcsBlendingData->m_pszRtAnimName)	)
	{
		m_strBlendingAnimation	= m_pcsACA->m_pcsBlendingData->m_pszRtAnimName;
	}

	if (	(m_pcsACA->m_pcsSubData) &&
			(m_pcsACA->m_pcsSubData->m_pszRtAnimName)	)
	{
		m_strSubAnimation	= m_pcsACA->m_pcsSubData->m_pszRtAnimName;
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCharAnimAttachedDataDlg::UpdateAnimData(AgcdAnimData2 **ppcsAnimData, CString strAnimation)
{
	if (	(ppcsAnimData) &&
			(*ppcsAnimData)		)
	{
		/*if (!AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->GetAgcaAnimation2()->RemoveAnimData(ppcsAnimData))
			return FALSE;*/

		AAD_RemoveAnimDataParams	csRemoveAnimDataParams;
		csRemoveAnimDataParams.m_ppSrcAnimData	= ppcsAnimData;

		AgcmAnimationDlg::GetInstance()->RemoveAnimData(&csRemoveAnimDataParams);
		if (!csRemoveAnimDataParams.m_bRemoved)
			return FALSE;
	}

	if (strAnimation != "")
	{
		/**ppcsAnimData	=
			AgcmAnimationDlg::GetInstance()->GetAgcmCharacterModule()->GetAgcaAnimation2()->AddAnimData(
				(LPSTR)(LPCSTR)(strAnimation),
				FALSE																					);

		if (!(*ppcsAnimData))
			return FALSE;*/

		AAD_AddAnimDataParams	csAddAnimDataParams;
		csAddAnimDataParams.m_pszSrcAnimData	= (LPSTR)(LPCSTR)(strAnimation);

		AgcmAnimationDlg::GetInstance()->AddAnimData(&csAddAnimDataParams);
		if (!csAddAnimDataParams.m_pDestAnimData)
			return FALSE;

		*ppcsAnimData	= csAddAnimDataParams.m_pDestAnimData;

		AAD_ReadRtAnimParams	csReadRtAnimParams;
		csReadRtAnimParams.m_pcsSrcAnimData		= *ppcsAnimData;
		AgcmAnimationDlg::GetInstance()->ReadRtAnim(&csReadRtAnimParams);

		if (!csReadRtAnimParams.m_bRead)
			return FALSE;
	}

	return TRUE;
}

BOOL CCharAnimAttachedDataDlg::UpdateAnimData(AgcdAnimData2 **ppcsAnimData, CHAR *pszAnimation)
{
	CString strTemp = _T("");

	if (pszAnimation)
		strTemp	= pszAnimation;

	return UpdateAnimData(ppcsAnimData, strTemp);
}

void CCharAnimAttachedDataDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(TRUE);

	m_pcsACA->m_nCustType			= m_unCustType;
	m_pcsACA->m_unActiveRate		= m_unActiveRate;

	ReallocCopyString( &m_pcsACA->m_pszPoint, (char*)m_strPoint.GetBuffer() );

	UpdateAnimData( &m_pcsACA->m_pcsSubData, m_strSubAnimation );
	UpdateAnimData( &m_pcsACA->m_pcsBlendingData, m_strBlendingAnimation );

	CDialog::OnClose();
}
