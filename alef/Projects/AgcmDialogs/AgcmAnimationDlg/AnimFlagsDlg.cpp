#include "stdafx.h"
#include "../resource.h"
#include "AnimFlagsDlg.h"
#include ".\animflagsdlg.h"

IMPLEMENT_DYNAMIC(CAnimFlagsDlg, CDialog)
CAnimFlagsDlg::CAnimFlagsDlg(AgcdAnimationFlag *pstFlags, ACA_AttachedData *pcsACA, CWnd* pParent /*=NULL*/)
	: CDialog(CAnimFlagsDlg::IDD, pParent)
	, m_bLoop(FALSE)
	, m_bBlend(FALSE)
	, m_bLink(FALSE)
	, m_lPreference(0)
	, m_cstrPoint(_T(""))
{
	m_pstFlags	= pstFlags;
	m_pcsACA	= pcsACA;
}

CAnimFlagsDlg::~CAnimFlagsDlg()
{
}

void CAnimFlagsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ANIM_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_ANIM_BLEND, m_bBlend);
	DDX_Check(pDX, IDC_CHECK_ANIM_LINK, m_bLink);
	DDX_Text(pDX, IDC_EDIT_ANIM_PREFERENCE, m_lPreference);
	DDX_Text(pDX, IDC_EDIT_ANIM_POINT, m_cstrPoint);
}


BEGIN_MESSAGE_MAP(CAnimFlagsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()


// CAnimFlagsDlg message handlers

void CAnimFlagsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_pstFlags->m_unAnimFlag	= 0;
	if( m_bLoop )	m_pstFlags->m_unAnimFlag |= AGCD_ANIMATION_FLAG_LOOP;
	if( m_bBlend )	m_pstFlags->m_unAnimFlag |= AGCD_ANIMATION_FLAG_BLEND;
	if( m_bLink )	m_pstFlags->m_unAnimFlag |= AGCD_ANIMATION_FLAG_LINK;

	m_pstFlags->m_unPreference	= m_lPreference;

	if( m_pcsACA )
	{
		ReallocCopyString( &m_pcsACA->m_pszPoint, (char*)m_cstrPoint.GetBuffer() );
	}

	CDialog::OnOK();
}

void CAnimFlagsDlg::OnCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

BOOL CAnimFlagsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lPreference	= m_pstFlags->m_unPreference;

	if (m_pstFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LOOP)	m_bLoop		= TRUE;
	if (m_pstFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_BLEND)	m_bBlend	= TRUE;
	if (m_pstFlags->m_unAnimFlag & AGCD_ANIMATION_FLAG_LINK)	m_bLink		= TRUE;

	if( m_pcsACA && m_pcsACA->m_pszPoint )
		m_cstrPoint = m_pcsACA->m_pszPoint;

	UpdateData(FALSE);

	return TRUE;
}
