// UITWPAnimationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UITWPAnimationDialog.h"
#include ".\uitwpanimationdialog.h"


// UITWPAnimationDialog dialog

IMPLEMENT_DYNAMIC(UITWPAnimationDialog, CPropertyPage)
UITWPAnimationDialog::UITWPAnimationDialog()
	: CPropertyPage(UITWPAnimationDialog::IDD)
	, m_ulClockCount(0)
	, m_bRenderString(TRUE)
	, m_bRenderBaseTexture(TRUE)
	, m_bAnimationDefault(FALSE)
{
	m_pcsBase	= NULL;
}

UITWPAnimationDialog::~UITWPAnimationDialog()
{
}

void UITWPAnimationDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UIT_ANIM_LIST, m_csAnimDataList);
	DDX_Control(pDX, IDC_UIT_ANIM_TEXTURE, m_csTexture);
	DDX_Text(pDX, IDC_UIT_ANIM_CLOCK, m_ulClockCount);
	DDX_Check(pDX, IDC_UIT_ANIM_STRING, m_bRenderString);
	DDX_Check(pDX, IDC_UIT_ANIM_BASE_TEXTURE, m_bRenderBaseTexture);
	DDX_Check(pDX, IDC_UIT_ANIM_DEFAULT, m_bAnimationDefault);
}


BEGIN_MESSAGE_MAP(UITWPAnimationDialog, CPropertyPage)
	ON_BN_CLICKED(IDC_UIT_ANIM_ADD, OnBnClickedUitAnimAdd)
	ON_BN_CLICKED(IDC_UIT_ANIM_REMOVE, OnBnClickedUitAnimRemove)
	ON_BN_CLICKED(IDC_UIT_ANIM_UPDATE, OnBnClickedUitAnimUpdate)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UIT_ANIM_LIST, OnLvnItemchangedUitAnimList)
	ON_BN_CLICKED(IDC_UIT_ANIM_START, OnBnClickedUitAnimStart)
	ON_BN_CLICKED(IDC_UIT_ANIM_STOP, OnBnClickedUitAnimStop)
	ON_BN_CLICKED(IDC_UIT_ANIM_DEFAULT, OnBnClickedUitAnimDefault)
END_MESSAGE_MAP()


BOOL UITWPAnimationDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsBase, NULL, NULL);

	return TRUE;
}

// UITWPAnimationDialog message handlers

void UITWPAnimationDialog::OnBnClickedUitAnimAdd()
{
	UpdateData(TRUE);

	if (!m_pcsBase)
		return;

	INT32	lIndex;

	lIndex = m_csTexture.GetItemData(m_csTexture.GetCurSel());
	if (lIndex == -1)
		lIndex = 0;

	m_pcsBase->AddAnimation(lIndex, m_ulClockCount, m_bRenderString, m_bRenderBaseTexture);

	RefreshAnimation();
}

void UITWPAnimationDialog::OnBnClickedUitAnimRemove()
{
	AcUIBaseAnimation *	pcsAnim = GetSelectedAnimData();

	if (!pcsAnim || !m_pcsBase)
		return;

	m_pcsBase->RemoveAnimation(pcsAnim);

	RefreshAnimation();
}

void UITWPAnimationDialog::OnBnClickedUitAnimUpdate()
{
	UpdateData();

	AcUIBaseAnimation *	pcsAnim = GetSelectedAnimData();

	if (!pcsAnim || !m_pcsBase)
		return;

	INT32	lIndex;

	lIndex = m_csTexture.GetItemData(m_csTexture.GetCurSel());
	if (lIndex == -1)
		lIndex = 0;

	pcsAnim->m_lTextureID = lIndex;
	pcsAnim->m_ulClockCount = m_ulClockCount;
	pcsAnim->m_bRenderString = m_bRenderString;
	pcsAnim->m_bRenderBaseTexture = m_bRenderBaseTexture;

	RefreshAnimation();
}

BOOL UITWPAnimationDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsBase	= pcsBase;

	if (!GetSafeHwnd())
		return TRUE;

	INT32	lIndex;
	INT32	lImageID;
	INT32	lListIndex;

	if (!m_pcsBase)
		return TRUE;

	m_csTexture.ResetContent();

	for (lIndex = 0; lIndex < m_pcsBase->m_csTextureList.GetCount(); ++lIndex)
	{
		RwTexture * pstTexture = m_pcsBase->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csTexture.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csTexture.SetItemData(lListIndex, lImageID);
	}

	RefreshAnimation();

	m_bAnimationDefault = m_pcsBase->m_bAnimationDefault;

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPAnimationDialog::UpdateDataFromDialog()
{
	return TRUE;
}

BOOL UITWPAnimationDialog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_csAnimDataList.InsertColumn(0, "Texture", LVCFMT_LEFT, 150);
	m_csAnimDataList.InsertColumn(1, "Clock", LVCFMT_LEFT, 80);
	m_csAnimDataList.InsertColumn(2, "String", LVCFMT_LEFT, 50);
	m_csAnimDataList.InsertColumn(3, "Texture", LVCFMT_LEFT, 50);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void UITWPAnimationDialog::OnLvnItemchangedUitAnimList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	AcUIBaseAnimation *	pcsAnim = GetSelectedAnimData();

	if (!pcsAnim || !m_pcsBase)
		return;

	RwTexture *	pstTexture = m_pcsBase->m_csTextureList.GetImage_ID(pcsAnim->m_lTextureID);

	if (pstTexture)
		m_csTexture.SelectString(-1, RwTextureGetName(pstTexture));
	m_ulClockCount = pcsAnim->m_ulClockCount;
	m_bRenderString = pcsAnim->m_bRenderString;
	m_bRenderBaseTexture = pcsAnim->m_bRenderBaseTexture;

	UpdateData(FALSE);
}

AcUIBaseAnimation *	UITWPAnimationDialog::GetSelectedAnimData()
{
	if (!m_pcsBase)
		return NULL;

	POSITION			pos = m_csAnimDataList.GetFirstSelectedItemPosition();

	if (!pos)
		return NULL;

	AcUIBaseAnimation * pcsAnim = (AcUIBaseAnimation *) m_csAnimDataList.GetItemData(m_csAnimDataList.GetNextSelectedItem(pos));
	if ((INT32) pcsAnim == -1)
		return NULL;

	return pcsAnim;
}

VOID UITWPAnimationDialog::RefreshAnimation()
{
	if (!m_pcsBase)
		return;

	INT32	lIndex;
	INT32	lImageID;
	INT32	lListIndex;
	CHAR	szTemp[128];

	m_csAnimDataList.DeleteAllItems();
	m_ulClockCount = 0;
	m_bRenderString = FALSE;
	m_bRenderBaseTexture = FALSE;

	AcUIBaseAnimation *	pcsAnim = m_pcsBase->m_pAnimationData;

	lIndex = 0;
	while (pcsAnim)
	{
		RwTexture *	pstTexture = m_pcsBase->m_csTextureList.GetImage_ID(pcsAnim->m_lTextureID);
		lListIndex = m_csAnimDataList.InsertItem(lIndex, pstTexture ? RwTextureGetName(pstTexture) : "");
		if (lListIndex == -1)
			return;
		sprintf(szTemp, "%d", pcsAnim->m_ulClockCount);
		m_csAnimDataList.SetItemText(lListIndex, 1, szTemp);
		sprintf(szTemp, "%d", pcsAnim->m_bRenderString);
		m_csAnimDataList.SetItemText(lListIndex, 2, szTemp);
		sprintf(szTemp, "%d", pcsAnim->m_bRenderBaseTexture);
		m_csAnimDataList.SetItemText(lListIndex, 3, szTemp);

		m_csAnimDataList.SetItemData(lListIndex, (INT32) pcsAnim);

		pcsAnim = pcsAnim->m_pNext;
		++lIndex;
	}
}

void UITWPAnimationDialog::OnBnClickedUitAnimStart()
{
	if (m_pcsBase)
		m_pcsBase->StartAnimation();
}

void UITWPAnimationDialog::OnBnClickedUitAnimStop()
{
	if (m_pcsBase)
		m_pcsBase->StopAnimation();
}

void UITWPAnimationDialog::OnBnClickedUitAnimDefault()
{
	UpdateData();

	if (m_pcsBase)
		m_pcsBase->m_bAnimationDefault = m_bAnimationDefault;
}
