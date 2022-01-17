// AnimFlags.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "AnimFlags.h"
#include ".\animflags.h"


// CAnimFlags dialog

IMPLEMENT_DYNAMIC(CAnimFlags, CDialog)
CAnimFlags::CAnimFlags(AgcdAnimationFlag *pstFlags, CWnd* pParent /*=NULL*/)
	: CDialog(CAnimFlags::IDD, pParent)
	, m_bLoop(FALSE)
	, m_bBlend(FALSE)
	, m_bLink(FALSE)
{
	m_pstFlags	= pstFlags;
}

CAnimFlags::~CAnimFlags()
{
}

void CAnimFlags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ANIM_LOOP, m_bLoop);
	DDX_Check(pDX, IDC_CHECK_ANIM_BLEND, m_bBlend);
	DDX_Check(pDX, IDC_CHECK_ANIM_LINK, m_bLink);
}


BEGIN_MESSAGE_MAP(CAnimFlags, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAnimFlags message handlers

void CAnimFlags::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_pstFlags->m_unAnimFlag	= 0;

	if (m_bLoop)
		m_pstFlags->m_unAnimFlag	|= AGCD_ANIMATION_FLAG_TYPE_LOOP;

	if (m_bBlend)
		m_pstFlags->m_unAnimFlag	|= AGCD_ANIMATION_FLAG_TYPE_BLEND;

	if (m_bLink)
		m_pstFlags->m_unAnimFlag	|= AGCD_ANIMATION_FLAG_TYPE_LINK;

	OnOK();
}
