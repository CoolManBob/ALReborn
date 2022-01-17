// UITWPClockDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPClockDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPClockDialog property page

IMPLEMENT_DYNCREATE(UITWPClockDialog, UITPropertyPage)

UITWPClockDialog::UITWPClockDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPClockDialog)
	//}}AFX_DATA_INIT

	Construct(UITWPClockDialog::IDD);

	m_pcsClock		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPClockDialog::~UITWPClockDialog()
{
}

void UITWPClockDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPClockDialog)
	DDX_Control(pDX, IDC_UIT_CLOCK_CURRENT_DISPLAY, m_csCurrentDisplay);
	DDX_Control(pDX, IDC_UIT_CLOCK_MAX_DISPLAY, m_csMaxDisplay);
	DDX_Control(pDX, IDC_UIT_CLOCK_IMAGE_ALPHA, m_csImageAlpha);
	DDX_Control(pDX, IDC_UIT_CLOCK_CURRENT, m_csCurrentPoint);
	DDX_Control(pDX, IDC_UIT_CLOCK_MAX, m_csMaxPoint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPClockDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPClockDialog)
	ON_CBN_SELCHANGE(IDC_UIT_CLOCK_MAX, OnSelchangeUITClockMax)
	ON_CBN_SELCHANGE(IDC_UIT_CLOCK_CURRENT, OnSelchangeUITClockCurrent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPClockDialog message handlers

BOOL UITWPClockDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsClock, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPClockDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsClock		= (AcUIClock *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsClock || !m_pcsControl)
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

	m_csImageAlpha.ResetContent();

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

	lListIndex = m_csImageAlpha.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageAlpha.SetItemData(lListIndex, 0);

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

	for (lIndex = 0; lIndex < m_pcsClock->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsClock->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csImageAlpha.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImageAlpha.SetItemData(lListIndex, lImageID);
	}

	if (m_pcsClock)
	{
		if (!m_pcsControl->m_uoData.m_stClock.m_pstMax)
			m_csMaxPoint.SelectString(-1, szNULLString);
		else
			m_csMaxPoint.SelectString(-1, m_pcsControl->m_uoData.m_stClock.m_pstMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stClock.m_pstCurrent)
			m_csCurrentPoint.SelectString(-1, szNULLString);
		else
			m_csCurrentPoint.SelectString(-1, m_pcsControl->m_uoData.m_stClock.m_pstCurrent->m_szName);

		OnSelchangeUITClockMax();
		OnSelchangeUITClockCurrent();

		if (!m_pcsControl->m_uoData.m_stClock.m_pstDisplayMax)
			m_csMaxDisplay.SelectString(-1, szNULLString);
		else
			m_csMaxDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stClock.m_pstDisplayMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent)
			m_csCurrentDisplay.SelectString(-1, szNULLString);
		else
			m_csCurrentDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent->m_szName);
	}

	pstTexture = m_pcsClock->m_csTextureList.GetImage_ID(m_pcsClock->GetAlphaImageID());
	if (pstTexture)
		m_csImageAlpha.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageAlpha.SelectString(-1, szNULLString);

	UpdateData(FALSE);

	g_pcsAgcmUIManager2->CountUserData();

	return TRUE;
}

BOOL UITWPClockDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsClock || !m_pcsControl)
		return TRUE;

	UpdateData();

	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lImageID;

	pstUserData = (AgcdUIUserData *) m_csMaxPoint.GetItemDataPtr(m_csMaxPoint.GetCurSel());
	if (pstUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stClock.m_pstMax = NULL;
	else if (pstUserData == (PVOID) 1)
	{
		m_pcsControl->m_bUseParentUserData = TRUE;
		m_pcsControl->m_uoData.m_stClock.m_pstMax = g_pcsAgcmUIManager2->GetControlUserData(m_pcsControl->m_pcsParentControl);
	}
	else
		m_pcsControl->m_uoData.m_stClock.m_pstMax = pstUserData;

	pstUserData = (AgcdUIUserData *) m_csCurrentPoint.GetItemDataPtr(m_csCurrentPoint.GetCurSel());
	if (pstUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stClock.m_pstCurrent = NULL;
	else if (pstUserData == (PVOID) 1)
	{
		m_pcsControl->m_bUseParentUserData = TRUE;
		m_pcsControl->m_uoData.m_stClock.m_pstCurrent = g_pcsAgcmUIManager2->GetControlUserData(m_pcsControl->m_pcsParentControl);
	}
	else
		m_pcsControl->m_uoData.m_stClock.m_pstCurrent = pstUserData;

	pstDisplay = (AgcdUIDisplay *) m_csMaxDisplay.GetItemDataPtr(m_csMaxDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stClock.m_pstDisplayMax = pstDisplay;

	pstDisplay = (AgcdUIDisplay *) m_csCurrentDisplay.GetItemDataPtr(m_csCurrentDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stClock.m_pstDisplayCurrent = pstDisplay;

	lImageID = m_csImageAlpha.GetItemData(m_csImageAlpha.GetCurSel());
	if (lImageID == -1)
		lImageID = 0;

	m_pcsClock->SetAlphaImageID(lImageID);

	g_pcsAgcmUIManager2->CountUserData();
	g_pcsAgcmUIManager2->RefreshControl(m_pcsControl, TRUE);

	m_pcsClock->m_eClockType = 0;

	return TRUE;
}

void UITWPClockDialog::OnOK() 
{
	UpdateDataFromDialog();

//	UITPropertyPage::OnOK();
}

void UITWPClockDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsClock, m_pcsUI, m_pcsControl);

//	UITPropertyPage::OnCancel();
}

void UITWPClockDialog::OnSelchangeUITClockMax() 
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

void UITWPClockDialog::OnSelchangeUITClockCurrent() 
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
