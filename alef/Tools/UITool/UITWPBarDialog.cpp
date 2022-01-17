// UITWPBarDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPBarDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPBarDialog property page

IMPLEMENT_DYNCREATE(UITWPBarDialog, UITPropertyPage)

UITWPBarDialog::UITWPBarDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPBarDialog)
	m_bVertical = FALSE;
	m_bCutHead = FALSE;
	m_bSmooth = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPBarDialog::IDD);

	m_pcsBar		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPBarDialog::~UITWPBarDialog()
{
}

void UITWPBarDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPBarDialog)
	DDX_Control(pDX, IDC_UIT_BAR_CURRENT_DISPLAY, m_csCurrentDisplay);
	DDX_Control(pDX, IDC_UIT_BAR_MAX_DISPLAY, m_csMaxDisplay);
	DDX_Control(pDX, IDC_UIT_BAR_IMAGE_BODY, m_csImageBody);
	DDX_Control(pDX, IDC_UIT_BAR_IMAGE_HEAD, m_csImageEdge);
	DDX_Control(pDX, IDC_UIT_BAR_CURRENT, m_csCurrentPoint);
	DDX_Control(pDX, IDC_UIT_BAR_MAX, m_csMaxPoint);
	DDX_Check(pDX, IDC_UIT_BAR_VERTICAL, m_bVertical);
	DDX_Check(pDX, IDC_UIT_BAR_CUT_HEAD, m_bCutHead);
	DDX_Check(pDX, IDC_UIT_BAR_SMOOTH, m_bSmooth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPBarDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPBarDialog)
	ON_CBN_SELCHANGE(IDC_UIT_BAR_MAX, OnSelchangeUITBarMax)
	ON_CBN_SELCHANGE(IDC_UIT_BAR_CURRENT, OnSelchangeUITBarCurrent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPBarDialog message handlers

BOOL UITWPBarDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsBar, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPBarDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsBar		= (AcUIBar *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsBar || !m_pcsControl)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	INT32				lImageID;
	AgcdUIUserData *	pstUserData;
	RwTexture *			pstTexture;
	static CHAR *		szParentString = "<Parent UserData>";
	static CHAR *		szNULLString = "NULL";

	m_csMaxPoint.ResetContent();
	m_csCurrentPoint.ResetContent();

	m_csImageEdge.ResetContent();
	m_csImageBody.ResetContent();

	lListIndex = m_csMaxPoint.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csMaxPoint.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csMaxPoint.AddString(szParentString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csMaxPoint.SetItemDataPtr(lListIndex, (PVOID) 1);

	lListIndex = m_csCurrentPoint.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csCurrentPoint.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csCurrentPoint.AddString(szParentString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csCurrentPoint.SetItemDataPtr(lListIndex, (PVOID) 1);

	lListIndex = m_csImageEdge.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageEdge.SetItemData(lListIndex, 0);

	lListIndex = m_csImageBody.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageBody.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csMaxPoint.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csMaxPoint.SetItemDataPtr(lListIndex, pstUserData);

			lListIndex = m_csCurrentPoint.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csCurrentPoint.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	for (lIndex = 0; lIndex < m_pcsBar->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsBar->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csImageEdge.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImageEdge.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImageBody.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImageBody.SetItemData(lListIndex, lImageID);
	}

	if (m_pcsBar)
	{
		if (!m_pcsControl->m_uoData.m_stBar.m_pstMax)
			m_csMaxPoint.SelectString(-1, szNULLString);
		else
			m_csMaxPoint.SelectString(-1, m_pcsControl->m_uoData.m_stBar.m_pstMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stBar.m_pstCurrent)
			m_csCurrentPoint.SelectString(-1, szNULLString);
		else
			m_csCurrentPoint.SelectString(-1, m_pcsControl->m_uoData.m_stBar.m_pstCurrent->m_szName);

		OnSelchangeUITBarMax();
		OnSelchangeUITBarCurrent();

		if (!m_pcsControl->m_uoData.m_stBar.m_pstDisplayMax)
			m_csMaxDisplay.SelectString(-1, szNULLString);
		else
			m_csMaxDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stBar.m_pstDisplayMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent)
			m_csCurrentDisplay.SelectString(-1, szNULLString);
		else
			m_csCurrentDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent->m_szName);
	}

	pstTexture = m_pcsBar->m_csTextureList.GetImage_ID(m_pcsBar->GetEdgeImageID());
	if (pstTexture)
		m_csImageEdge.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageEdge.SelectString(-1, szNULLString);

	pstTexture = m_pcsBar->m_csTextureList.GetImage_ID(m_pcsBar->GetBodyImageID());
	if (pstTexture)
		m_csImageBody.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageBody.SelectString(-1, szNULLString);

	m_bVertical = m_pcsBar->m_bVertical;

	if (m_pcsBar->m_eBarType & ACUIBAR_TYPE_CUT_HEAD)
		m_bCutHead = TRUE;
	else
		m_bCutHead = FALSE;

	if (m_pcsBar->m_eBarType & ACUIBAR_TYPE_SMOOTH)
		m_bSmooth = TRUE;
	else
		m_bSmooth = FALSE;

	UpdateData(FALSE);

	g_pcsAgcmUIManager2->CountUserData();

	return TRUE;
}

BOOL UITWPBarDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsBar || !m_pcsControl)
		return TRUE;

	UpdateData();

	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lImageID;

	pstUserData = (AgcdUIUserData *) m_csMaxPoint.GetItemDataPtr(m_csMaxPoint.GetCurSel());
	if (pstUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stBar.m_pstMax = NULL;
	else if (pstUserData == (PVOID) 1)
	{
		m_pcsControl->m_bUseParentUserData = TRUE;
		m_pcsControl->m_uoData.m_stBar.m_pstMax = g_pcsAgcmUIManager2->GetControlUserData(m_pcsControl->m_pcsParentControl);
	}
	else
		m_pcsControl->m_uoData.m_stBar.m_pstMax = pstUserData;

	pstUserData = (AgcdUIUserData *) m_csCurrentPoint.GetItemDataPtr(m_csCurrentPoint.GetCurSel());
	if (pstUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stBar.m_pstCurrent = NULL;
	else if (pstUserData == (PVOID) 1)
	{
		m_pcsControl->m_bUseParentUserData = TRUE;
		m_pcsControl->m_uoData.m_stBar.m_pstCurrent = g_pcsAgcmUIManager2->GetControlUserData(m_pcsControl->m_pcsParentControl);
	}
	else
		m_pcsControl->m_uoData.m_stBar.m_pstCurrent = pstUserData;

	pstDisplay = (AgcdUIDisplay *) m_csMaxDisplay.GetItemDataPtr(m_csMaxDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stBar.m_pstDisplayMax = pstDisplay;

	pstDisplay = (AgcdUIDisplay *) m_csCurrentDisplay.GetItemDataPtr(m_csCurrentDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stBar.m_pstDisplayCurrent = pstDisplay;

	lImageID = m_csImageEdge.GetItemData(m_csImageEdge.GetCurSel());
	if (lImageID == -1)
		lImageID = 0;

	m_pcsBar->SetEdgeImageID(lImageID);

	lImageID = m_csImageBody.GetItemData(m_csImageBody.GetCurSel());
	if (lImageID == -1)
		lImageID = 0;

	m_pcsBar->SetBodyImageID(lImageID);

	g_pcsAgcmUIManager2->CountUserData();
	g_pcsAgcmUIManager2->RefreshControl(m_pcsControl, TRUE);

	m_pcsBar->m_bVertical = m_bVertical;

	m_pcsBar->m_eBarType = 0;

	if (m_bCutHead)
		m_pcsBar->m_eBarType |= ACUIBAR_TYPE_CUT_HEAD;
	
	if (m_bSmooth)
		m_pcsBar->m_eBarType |= ACUIBAR_TYPE_SMOOTH;

	return TRUE;
}

void UITWPBarDialog::OnOK() 
{
	UpdateDataFromDialog();

//	UITPropertyPage::OnOK();
}

void UITWPBarDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsBar, m_pcsUI, m_pcsControl);

//	UITPropertyPage::OnCancel();
}

void UITWPBarDialog::OnSelchangeUITBarMax() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNULLString = "NULL";

	pstUserData = (AgcdUIUserData *) m_csMaxPoint.GetItemDataPtr(m_csMaxPoint.GetCurSel());

	if (pstUserData == (VOID *) -1)
		pstUserData = NULL;

	if (pstUserData == (VOID *) 1)
		pstUserData = m_pcsControl->m_pstUserData;

	m_csMaxDisplay.ResetContent();

	lListIndex = m_csMaxDisplay.AddString(szNULLString);
	m_csMaxDisplay.SetItemDataPtr(lListIndex, NULL);
	m_csMaxDisplay.SetCurSel(lListIndex);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetDisplayCount(); ++lIndex)
	{
		pstDisplay = g_pcsAgcmUIManager2->GetDisplay(lIndex);
		if (!pstDisplay)
			continue;

		if (pstUserData && !(pstUserData->m_eType & pstDisplay->m_ulDataType))
			continue;

		lListIndex = m_csMaxDisplay.AddString(pstDisplay->m_szName);
		if (lListIndex == CB_ERR)
			return;

		m_csMaxDisplay.SetItemDataPtr(lListIndex, pstDisplay);
	}
}

void UITWPBarDialog::OnSelchangeUITBarCurrent() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNULLString = "NULL";

	pstUserData = (AgcdUIUserData *) m_csCurrentPoint.GetItemDataPtr(m_csCurrentPoint.GetCurSel());

	if (pstUserData == (VOID *) -1)
		pstUserData = NULL;

	if (pstUserData == (VOID *) 1)
		pstUserData = m_pcsControl->m_pstUserData;

	m_csCurrentDisplay.ResetContent();

	lListIndex = m_csCurrentDisplay.AddString(szNULLString);
	m_csCurrentDisplay.SetItemDataPtr(lListIndex, NULL);
	m_csCurrentDisplay.SetCurSel(lListIndex);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetDisplayCount(); ++lIndex)
	{
		pstDisplay = g_pcsAgcmUIManager2->GetDisplay(lIndex);
		if (!pstDisplay)
			continue;

		if (pstUserData && !(pstUserData->m_eType & pstDisplay->m_ulDataType))
			continue;

		lListIndex = m_csCurrentDisplay.AddString(pstDisplay->m_szName);
		if (lListIndex == CB_ERR)
			return;

		m_csCurrentDisplay.SetItemDataPtr(lListIndex, pstDisplay);
	}
}
