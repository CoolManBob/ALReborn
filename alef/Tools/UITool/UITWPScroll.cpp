// UITWPScroll.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPScroll.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPScroll property page

IMPLEMENT_DYNCREATE(UITWPScroll, UITPropertyPage)

UITWPScroll::UITWPScroll() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPScroll)
	m_bVScroll = FALSE;
	m_lMinPosition = 0;
	m_lMaxPosition = 0;
	m_fChangeUnit = 0.0f;
	m_bMoveByUnit = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPScroll::IDD);

	m_pcsScroll		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPScroll::~UITWPScroll()
{
}

void UITWPScroll::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPScroll)
	DDX_Control(pDX, IDC_UIT_SCROLL_DOWN_BUTTON, m_csButtonDown);
	DDX_Control(pDX, IDC_UIT_SCROLL_UP_BUTTON, m_csButtonUp);
	DDX_Control(pDX, IDC_UIT_SCROLL_CURRENT_DISPLAY, m_csCurrentDisplay);
	DDX_Control(pDX, IDC_UIT_SCROLL_CURRENT_UD, m_csCurrentUserData);
	DDX_Control(pDX, IDC_UIT_SCROLL_MAX_DISPLAY, m_csMaxDisplay);
	DDX_Control(pDX, IDC_UIT_SCROLL_MAX_UD, m_csMaxUserData);
	DDX_Check(pDX, IDC_UIT_SCROLL_VERTICAL, m_bVScroll);
	DDX_Text(pDX, IDC_UIT_SCROLL_MIN, m_lMinPosition);
	DDX_Text(pDX, IDC_UIT_SCROLL_MAX, m_lMaxPosition);
	DDX_Text(pDX, IDC_UIT_SCROLL_UNIT, m_fChangeUnit);
	DDX_Check(pDX, IDC_UIT_SCROLL_BY_UNIT, m_bMoveByUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPScroll, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPScroll)
	ON_CBN_SELCHANGE(IDC_UIT_SCROLL_MAX_UD, OnSelchangeUITScrollMaxUD)
	ON_CBN_SELCHANGE(IDC_UIT_SCROLL_CURRENT_UD, OnSelchangeUITScrollCurrentUD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPScroll message handlers

BOOL UITWPScroll::OnSetActive()
{
	UpdateDataToDialog(m_pcsScroll, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPScroll::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsScroll		= (AcUIScroll *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsScroll || !m_pcsControl)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIUserData *	pstUserData;
	static CHAR *		szNULLString = "NULL";

	m_bVScroll		= m_pcsScroll->m_bVScroll;
	m_lMinPosition	= m_pcsScroll->m_lMinPosition;
	m_lMaxPosition	= m_pcsScroll->m_lMaxPosition;
	m_fChangeUnit	= m_pcsScroll->m_fScrollUnit;
	m_bMoveByUnit	= m_pcsScroll->m_bMoveByUnit;

	m_csMaxUserData.ResetContent();
	m_csCurrentUserData.ResetContent();

	lListIndex = m_csMaxUserData.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csMaxUserData.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csCurrentUserData.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csCurrentUserData.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csMaxUserData.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csMaxUserData.SetItemDataPtr(lListIndex, pstUserData);

			lListIndex = m_csCurrentUserData.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csCurrentUserData.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	m_csButtonUp.ResetContent();
	m_csButtonDown.ResetContent();

	lListIndex = m_csButtonUp.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csButtonUp.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csButtonDown.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csButtonDown.SetItemDataPtr(lListIndex, NULL);

	lIndex = 0;
	for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsControl, &lIndex); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsControl, &lIndex))
	{
		if (pcsControl->m_lType != AcUIBase::TYPE_BUTTON)
			continue;

		lListIndex = m_csButtonUp.AddString(pcsControl->m_szName);
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csButtonUp.SetItemDataPtr(lListIndex, pcsControl);

		lListIndex = m_csButtonDown.AddString(pcsControl->m_szName);
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csButtonDown.SetItemDataPtr(lListIndex, pcsControl);
	}

	if (m_pcsScroll)
	{
		if (!m_pcsControl->m_uoData.m_stScroll.m_pstMax)
			m_csMaxUserData.SelectString(-1, szNULLString);
		else
			m_csMaxUserData.SelectString(-1, m_pcsControl->m_uoData.m_stScroll.m_pstMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stScroll.m_pstCurrent)
			m_csCurrentUserData.SelectString(-1, szNULLString);
		else
			m_csCurrentUserData.SelectString(-1, m_pcsControl->m_uoData.m_stScroll.m_pstCurrent->m_szName);

		OnSelchangeUITScrollMaxUD();
		OnSelchangeUITScrollCurrentUD();

		if (!m_pcsControl->m_uoData.m_stScroll.m_pstDisplayMax)
			m_csMaxDisplay.SelectString(-1, szNULLString);
		else
			m_csMaxDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stScroll.m_pstDisplayMax->m_szName);

		if (!m_pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent)
			m_csCurrentDisplay.SelectString(-1, szNULLString);
		else
			m_csCurrentDisplay.SelectString(-1, m_pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent->m_szName);

		if (m_pcsScroll->GetScrollUpButton())
		{
			pcsControl = g_pcsAgcmUIManager2->GetControl(m_pcsUI, m_pcsScroll->GetScrollUpButton());
			if (pcsControl)
				m_csButtonUp.SelectString(-1, pcsControl->m_szName);
		}

		if (m_pcsScroll->GetScrollDownButton())
		{
			pcsControl = g_pcsAgcmUIManager2->GetControl(m_pcsUI, m_pcsScroll->GetScrollDownButton());
			if (pcsControl)
				m_csButtonDown.SelectString(-1, pcsControl->m_szName);
		}
	}

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPScroll::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsScroll || !m_pcsControl)
		return TRUE;

	UpdateData();

	m_pcsScroll->SetScrollButtonInfo(m_bVScroll, m_lMinPosition, m_lMaxPosition, m_fChangeUnit, m_bMoveByUnit);

	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	AgcdUIControl *		pcsControl;

	pstUserData = (AgcdUIUserData *) m_csMaxUserData.GetItemDataPtr(m_csMaxUserData.GetCurSel());
	if (pstUserData == (PVOID) -1)
		pstUserData = NULL;

	m_pcsControl->m_uoData.m_stScroll.m_pstMax = pstUserData;

	pstUserData = (AgcdUIUserData *) m_csCurrentUserData.GetItemDataPtr(m_csCurrentUserData.GetCurSel());
	if (pstUserData == (PVOID) -1)
		pstUserData = NULL;

	m_pcsControl->m_uoData.m_stScroll.m_pstCurrent = pstUserData;

	pstDisplay = (AgcdUIDisplay *) m_csMaxDisplay.GetItemDataPtr(m_csMaxDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stScroll.m_pstDisplayMax = pstDisplay;

	pstDisplay = (AgcdUIDisplay *) m_csCurrentDisplay.GetItemDataPtr(m_csCurrentDisplay.GetCurSel());
	if (pstDisplay == (PVOID) -1)
		pstDisplay = NULL;

	m_pcsControl->m_uoData.m_stScroll.m_pstDisplayCurrent = pstDisplay;

	pcsControl = (AgcdUIControl *) m_csButtonUp.GetItemDataPtr(m_csButtonUp.GetCurSel());
	if (pcsControl && pcsControl != (PVOID) -1)
	{
		m_pcsScroll->SetScrollUpButton((AcUIButton *) pcsControl->m_pcsBase);
		pcsControl->m_uoData.m_stButton.m_eButtonType = AGCDUI_BUTTON_TYPE_SCROLL_UP;
	}

	pcsControl = (AgcdUIControl *) m_csButtonDown.GetItemDataPtr(m_csButtonDown.GetCurSel());
	if (pcsControl && pcsControl != (PVOID) -1)
	{
		m_pcsScroll->SetScrollDownButton((AcUIButton *) pcsControl->m_pcsBase);
		pcsControl->m_uoData.m_stButton.m_eButtonType = AGCDUI_BUTTON_TYPE_SCROLL_DOWN;
	}

	return TRUE;
}

void UITWPScroll::OnCancel() 
{
	UpdateDataToDialog(m_pcsScroll, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPScroll::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

void UITWPScroll::OnSelchangeUITScrollMaxUD() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNULLString = "NULL";

	pstUserData = (AgcdUIUserData *) m_csMaxUserData.GetItemDataPtr(m_csMaxUserData.GetCurSel());

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

void UITWPScroll::OnSelchangeUITScrollCurrentUD() 
{
	AgcdUIUserData *	pstUserData;
	AgcdUIDisplay *		pstDisplay;
	INT32				lIndex;
	INT32				lListIndex;
	static CHAR *		szNULLString = "NULL";

	pstUserData = (AgcdUIUserData *) m_csCurrentUserData.GetItemDataPtr(m_csCurrentUserData.GetCurSel());

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
