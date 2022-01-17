// UITWPUIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPUIDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPUIDialog property page

CHAR *	g_szCoordSystems[AGCDUI_MAX_COORDSYSTEM] =
{
	"Absolute",
	"Relative",
	"Cursor"
};

CHAR *	g_szUITypes[AGCDUI_MAX_TYPE] = 
{
	"Normal",
	"Vertical Popup",
	"Horizontal Popup"
};

IMPLEMENT_DYNCREATE(UITWPUIDialog, UITPropertyPage)

UITWPUIDialog::UITWPUIDialog() : UITPropertyPage()
, m_bTransparentUI(FALSE)
{
	//{{AFX_DATA_INIT(UITWPUIDialog)
	m_bUseGroupPosition = FALSE;
	m_bMainUI = FALSE;
	m_bInitStatus = TRUE;
	m_bMode_1024 = FALSE;
	m_bMode_1280 = FALSE;
	m_bMode_1600 = FALSE;
	m_bAutoClose = FALSE;
	m_bEventUI = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPUIDialog::IDD);

	m_pcsUI = NULL;
}

UITWPUIDialog::~UITWPUIDialog()
{
}

void UITWPUIDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPUIDialog)
	DDX_Control(pDX, IDC_UIT_UI_FOCUS_KILL, m_csKillFocus);
	DDX_Control(pDX, IDC_UIT_UI_FOCUS_SET, m_csSetFocus);
	DDX_Control(pDX, IDC_UIT_UI_TYPE, m_csUIType);
	DDX_Control(pDX, IDC_UIT_COORD_SYSTEM, m_csCoordSystems);
	DDX_Control(pDX, IDC_UIT_PARENT_UI, m_csParentUI);
	DDX_Check(pDX, IDC_UIT_GROUP_POSITION, m_bUseGroupPosition);
	DDX_Check(pDX, IDC_UIT_MAIN_UI, m_bMainUI);
	DDX_Check(pDX, IDC_UIT_INIT_STATUS, m_bInitStatus);
	DDX_Check(pDX, IDC_UIT_MODE_1024, m_bMode_1024);
	DDX_Check(pDX, IDC_UIT_MODE_1280, m_bMode_1280);
	DDX_Check(pDX, IDC_UIT_MODE_1600, m_bMode_1600);
	DDX_Check(pDX, IDC_UIT_AUTO_CLOSE, m_bAutoClose);
	DDX_Check(pDX, IDC_UIT_EVENT_UI, m_bEventUI);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_UIT_UI_TRANSPARENT, m_bTransparentUI);
}


BEGIN_MESSAGE_MAP(UITWPUIDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPUIDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPUIDialog message handlers

BOOL UITWPUIDialog::OnSetActive()
{
	UpdateDataToDialog(NULL, m_pcsUI, NULL);

	return TRUE;
}

BOOL UITWPUIDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsUI			= pcsUI;

	if (!GetSafeHwnd() || !m_pcsUI)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIFunction *	pstFunction;
	static CHAR *		szNULLString = "NULL";

	m_csParentUI.ResetContent();

	lListIndex = m_csParentUI.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csParentUI.SetItemDataPtr(lListIndex, NULL);

	lIndex = 0;
	for (pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex); pcsUI; pcsUI = g_pcsAgcmUIManager2->GetSequenceUI(&lIndex))
	{
		if (pcsUI == m_pcsUI)
			continue;

		lListIndex = m_csParentUI.AddString(pcsUI->m_szUIName);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csParentUI.SetItemDataPtr(lListIndex, pcsUI);
	}

	if (m_pcsUI->m_pcsParentUI)
		m_csParentUI.SelectString(-1, m_pcsUI->m_pcsParentUI->m_szUIName);
	else
		m_csParentUI.SelectString(-1, szNULLString);

	m_bUseGroupPosition	= m_pcsUI->m_bUseParentPosition;
	m_bInitStatus		= m_pcsUI->m_bInitControlStatus;	
	m_bMainUI			= m_pcsUI->m_bMainUI;
	m_bAutoClose		= m_pcsUI->m_bAutoClose;
	m_bEventUI			= m_pcsUI->m_bEventUI;
	m_bTransparentUI	= m_pcsUI->m_bTransparent;

	for (lIndex = 0; lIndex < AGCDUI_MAX_COORDSYSTEM; ++lIndex)
	{
		lListIndex = m_csCoordSystems.AddString(g_szCoordSystems[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csCoordSystems.SetItemData(lListIndex, lIndex);
	}

	if (m_pcsUI->m_eCoordSystem >= AGCDUI_MAX_COORDSYSTEM)
		m_pcsUI->m_eCoordSystem = AGCDUI_COORDSYSTEM_RELATIVE;

	m_csCoordSystems.SelectString(-1, g_szCoordSystems[m_pcsUI->m_eCoordSystem]);

	for (lIndex = 0; lIndex < AGCDUI_MAX_TYPE; ++lIndex)
	{
		lListIndex = m_csUIType.AddString(g_szUITypes[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csUIType.SetItemData(lListIndex, lIndex);
	}

	m_csSetFocus.ResetContent();
	m_csSetFocus.AddString(szNULLString);

	m_csKillFocus.ResetContent();
	m_csKillFocus.AddString(szNULLString);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetFunctionCount(); ++lIndex)
	{
		pstFunction = g_pcsAgcmUIManager2->GetFunction(lIndex);
		if (pstFunction)
		{
			lListIndex = m_csSetFocus.AddString(pstFunction->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csSetFocus.SetItemDataPtr(lListIndex, pstFunction);

			lListIndex = m_csKillFocus.AddString(pstFunction->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csKillFocus.SetItemDataPtr(lListIndex, pstFunction);
		}
	}

	if (m_pcsUI->m_pstSetFocusFunction)
		m_csSetFocus.SelectString(-1, m_pcsUI->m_pstSetFocusFunction->m_szName);
	else
		m_csSetFocus.SelectString(-1, szNULLString);

	if (m_pcsUI->m_pstKillFocusFunction)
		m_csKillFocus.SelectString(-1, m_pcsUI->m_pstKillFocusFunction->m_szName);
	else
		m_csKillFocus.SelectString(-1, szNULLString);

	if (m_pcsUI->m_eUIType >= AGCDUI_MAX_TYPE)
		m_pcsUI->m_eUIType = AGCDUI_TYPE_NORMAL;

	m_csUIType.SelectString(-1, g_szUITypes[m_pcsUI->m_eUIType]);

	m_bMode_1024 = m_pcsUI->m_ulModeFlag & (1 << AGCDUI_MODE_1024_768) ? TRUE : FALSE;
	m_bMode_1280 = m_pcsUI->m_ulModeFlag & (1 << AGCDUI_MODE_1280_1024) ? TRUE : FALSE;
	m_bMode_1600 = m_pcsUI->m_ulModeFlag & (1 << AGCDUI_MODE_1600_1200) ? TRUE : FALSE;

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPUIDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsUI)
		return TRUE;

	AgcdUI *	pcsUI;

	UpdateData();

	pcsUI = (AgcdUI *) m_csParentUI.GetItemDataPtr(m_csParentUI.GetCurSel());
	if (pcsUI == (PVOID) -1)
		pcsUI = NULL;

	m_pcsUI->m_pcsParentUI			= pcsUI;
	m_pcsUI->m_bUseParentPosition	= m_bUseGroupPosition;
	m_pcsUI->m_bMainUI				= m_bMainUI;
	m_pcsUI->m_bInitControlStatus	= m_bInitStatus;
	m_pcsUI->m_bAutoClose			= m_bAutoClose;
	m_pcsUI->m_bEventUI				= m_bEventUI;
	m_pcsUI->m_bTransparent			= m_bTransparentUI;

	m_pcsUI->m_eCoordSystem = (AgcdUICoordSystem) m_csCoordSystems.GetItemData(m_csCoordSystems.GetCurSel());
	if (-1 == (INT32) m_pcsUI->m_eCoordSystem)
		m_pcsUI->m_eCoordSystem = AGCDUI_COORDSYSTEM_RELATIVE;

	m_pcsUI->m_eUIType = (AgcdUIType) m_csUIType.GetItemData(m_csUIType.GetCurSel());
	if (-1 == (INT32) m_pcsUI->m_eUIType)
		m_pcsUI->m_eUIType = AGCDUI_TYPE_NORMAL;

	m_pcsUI->m_pstSetFocusFunction = (AgcdUIFunction *) m_csSetFocus.GetItemDataPtr(m_csSetFocus.GetCurSel());
	if (-1 == (INT32) m_pcsUI->m_pstSetFocusFunction)
		m_pcsUI->m_pstSetFocusFunction = NULL;

	m_pcsUI->m_pstKillFocusFunction = (AgcdUIFunction *) m_csKillFocus.GetItemDataPtr(m_csKillFocus.GetCurSel());
	if (-1 == (INT32) m_pcsUI->m_pstKillFocusFunction)
		m_pcsUI->m_pstKillFocusFunction = NULL;

	m_pcsUI->m_ulModeFlag = 0xffffffff;

	if (!m_bMode_1024)
		m_pcsUI->m_ulModeFlag &= ~(1 << AGCDUI_MODE_1024_768);

	if (!m_bMode_1280)
		m_pcsUI->m_ulModeFlag &= ~(1 << AGCDUI_MODE_1280_1024);

	if (!m_bMode_1600)
		m_pcsUI->m_ulModeFlag &= ~(1 << AGCDUI_MODE_1600_1200);

	return TRUE;
}


void UITWPUIDialog::OnOK() 
{
	UpdateDataFromDialog();

//	UITPropertyPage::OnOK();
}

void UITWPUIDialog::OnCancel() 
{
	UpdateDataToDialog(NULL, m_pcsUI, NULL);

//	UITPropertyPage::OnCancel();
}
