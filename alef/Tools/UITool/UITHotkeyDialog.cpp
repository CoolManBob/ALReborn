// UITHotkeyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITHotkeyDialog.h"
#include "UITActionDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CHAR *	g_aszKeyStatus[AGCDUI_MAX_HOTKEY_TYPE];
extern CHAR *	g_aszKeyTable[256];

extern CHAR *	g_szArgumentNULL;
extern CHAR *	g_aszUIActionType[];
extern CHAR *	g_aszUIActionUIType[];

/////////////////////////////////////////////////////////////////////////////
// UITHotkeyDialog dialog


UITHotkeyDialog::UITHotkeyDialog(AgcdUI *pcsUI, CWnd* pParent /*=NULL*/)
	: CDialog(UITHotkeyDialog::IDD, pParent)
	, m_bCustomizable(FALSE)
	, m_strDescription(_T(""))
{
	//{{AFX_DATA_INIT(UITHotkeyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pcsUI = pcsUI;
	m_pstHotkey = NULL;
}


void UITHotkeyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITHotkeyDialog)
	DDX_Control(pDX, IDC_UIT_HOTKEY_CODE, m_csKeyCode);
	DDX_Control(pDX, IDC_UIT_HOTKEY_STATUS, m_csKeyType);
	DDX_Control(pDX, IDC_UIT_ACTION_LIST, m_csActionList);
	DDX_Control(pDX, IDC_UIT_HOTKEY_LIST, m_csHotkeyList);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_HOTKEY_CUSTOMIZABLE, m_bCustomizable);
	DDX_Text(pDX, IDC_HOTKEY_DESCRIPTION, m_strDescription);
}


BEGIN_MESSAGE_MAP(UITHotkeyDialog, CDialog)
	//{{AFX_MSG_MAP(UITHotkeyDialog)
	ON_BN_CLICKED(IDC_UIT_ADD_ACTION, OnUITAddAction)
	ON_BN_CLICKED(IDC_UIT_EDIT_ACTION, OnUITEditAction)
	ON_BN_CLICKED(IDC_UIT_DELETE_ACTION, OnUITDeleteAction)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UIT_HOTKEY_LIST, OnItemchangedUITHotkeyList)
	ON_BN_CLICKED(IDC_UIT_ADD_HOTKEY, OnUITAddHotkey)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HOTKEY_CUSTOMIZABLE, &UITHotkeyDialog::OnBnClickedHotkeyCustomizable)
	ON_EN_CHANGE(IDC_HOTKEY_DESCRIPTION, &UITHotkeyDialog::OnEnChangeHotkeyDescription)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITHotkeyDialog message handlers

void UITHotkeyDialog::OnUITDeleteAction() 
{
	if (!m_pstHotkey)
	{
		AfxMessageBox("Select Hotkey First !!!");
		return;
	}

	INT32					lIndex;
	INT32					lListIndex;
	POSITION				pos = m_csActionList.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		lListIndex = m_csActionList.GetNextSelectedItem(pos);
		lIndex = m_csActionList.GetItemData(lListIndex);
		if (lIndex == -1)
			return;

		AgcdUIAction &	stAction = m_pstHotkey->m_listAction[lIndex];

		m_pstHotkey->m_listAction.RemoveData(stAction);
		/*
		memcpy(m_pstHotkey->m_astAction + lIndex, 
			m_pstHotkey->m_astAction + lIndex + 1, 
			(AGCDUIMANAGER2_MAX_ACTIONS - lIndex - 1) * sizeof(AgcdUIAction));
		memset(m_pstHotkey->m_astAction + AGCDUIMANAGER2_MAX_ACTIONS - 1, 0, sizeof(AgcdUIAction));

		--(m_pstHotkey->m_lCount);
		*/

		RefreshActionList();
	}
}

void UITHotkeyDialog::OnUITAddAction() 
{
	if (!m_pstHotkey)
	{
		AfxMessageBox("Select Hotkey First !!!");
		return;
	}

	AgcdUIAction	stAction;

	memset(&stAction, 0, sizeof(stAction));

	UITActionDialog		dlgAction(m_pcsUI, &stAction);

	if (dlgAction.DoModal() == IDOK)
	{
		m_pstHotkey->m_listAction.AddTail(stAction);
	}

	RefreshActionList();
}

void UITHotkeyDialog::OnUITEditAction() 
{
	if (!m_pstHotkey)
	{
		AfxMessageBox("Select Hotkey First !!!");
		return;
	}

	INT32					lIndex;
	INT32					lListIndex;
	POSITION				pos = m_csActionList.GetFirstSelectedItemPosition();

	UpdateData();

	if (pos)
	{
		lListIndex = m_csActionList.GetNextSelectedItem(pos);
		lIndex = m_csActionList.GetItemData(lListIndex);
		if (lIndex == -1)
			return;

		AgcdUIAction &	stAction = m_pstHotkey->m_listAction[lIndex];

		UITActionDialog	dlgAction(m_pcsUI, &stAction);

		dlgAction.DoModal();

		RefreshActionList();
	}
}

BOOL UITHotkeyDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_csHotkeyList.InsertColumn(0, "Hotkey", LVCFMT_LEFT, 150);
	m_csHotkeyList.InsertColumn(0, "Status", LVCFMT_LEFT, 155);

	m_csActionList.InsertColumn(0, "Action", LVCFMT_LEFT, 60);
	m_csActionList.InsertColumn(1, "Type", LVCFMT_LEFT, 125);
	m_csActionList.InsertColumn(2, "Target", LVCFMT_LEFT, 120);

	return RefreshHotkeyList();
}

BOOL UITHotkeyDialog::RefreshHotkeyList()
{
	INT32				lIndex;
	INT32				lIndex1;
	INT32				lIndex2;
	INT32				lListIndex;
	AgcdUIHotkey *		pstHotkey;

	m_csHotkeyList.DeleteAllItems();
	m_csKeyType.ResetContent();
	m_csKeyCode.ResetContent();

	lIndex = 0;
	for (lIndex2 = 0; lIndex2 < 256; ++lIndex2)
	{
		for (lIndex1 = AGCDUI_HOTKEY_TYPE_NORMAL; lIndex1 < AGCDUI_MAX_HOTKEY_TYPE; ++lIndex1)
		{
			pstHotkey = g_pcsAgcmUIManager2->GetHotkey((AgcdUIHotkeyType) lIndex1, lIndex2);
			if (!pstHotkey)
				continue;

			lListIndex = m_csHotkeyList.InsertItem(lIndex, g_aszKeyTable[lIndex2]);
			if (lListIndex == -1)
				return FALSE;

			m_csHotkeyList.SetItemText(lListIndex, 1, g_aszKeyStatus[lIndex1]);
			m_csHotkeyList.SetItemData(lListIndex, (INT32) pstHotkey);

			++lIndex;
		}

		if (g_aszKeyTable[lIndex2])
		{
			lListIndex = m_csKeyCode.AddString(g_aszKeyTable[lIndex2]);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csKeyCode.SetItemData(lListIndex, lIndex2);
		}
	}

	for (lIndex = AGCDUI_HOTKEY_TYPE_NORMAL; lIndex < AGCDUI_MAX_HOTKEY_TYPE; ++lIndex)
	{
		if (!g_aszKeyStatus[lIndex])
			continue;

		lListIndex = m_csKeyType.AddString(g_aszKeyStatus[lIndex]);
		if (lListIndex == CB_ERR)
			return FALSE;

		m_csKeyType.SetItemData(lListIndex, lIndex);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID UITHotkeyDialog::RefreshActionList()
{
	INT32					lListIndex;
	INT32					lIndex;
	POSITION				pos = m_csHotkeyList.GetFirstSelectedItemPosition();
	AgcdUIAction *			pstAction;

	UpdateData();

	if (pos)
	{
		lListIndex = m_csHotkeyList.GetNextSelectedItem(pos);
		m_pstHotkey = (AgcdUIHotkey *) m_csHotkeyList.GetItemData(lListIndex);

		if (m_pstHotkey == (PVOID) -1)
			m_pstHotkey = NULL;

		if (!m_pstHotkey)
		{
			m_csKeyCode.SetCurSel(-1);
			m_csKeyType.SetCurSel(-1);
		}
		else
		{
			m_csKeyCode.SelectString(-1, g_aszKeyTable[m_pstHotkey->m_cKeyCode]);
			m_csKeyType.SelectString(-1, g_aszKeyStatus[m_pstHotkey->m_eType]);
		}

		m_csActionList.DeleteAllItems();

		for (lIndex = 0; lIndex < m_pstHotkey->m_listAction.GetCount(); ++lIndex)
		{
			pstAction = &(m_pstHotkey->m_listAction[lIndex]);

			lListIndex = m_csActionList.InsertItem(lIndex, g_aszUIActionType[pstAction->m_eType]);
			if (lListIndex == -1)
				return;

			m_csActionList.SetItemData(lIndex, lIndex);

			switch (pstAction->m_eType)
			{
			case AGCDUI_ACTION_UI:
				if (pstAction->m_uoAction.m_stUI.m_pcsUI)
				{
					m_csActionList.SetItemText(lListIndex, 1, g_aszUIActionUIType[pstAction->m_uoAction.m_stUI.m_eType]);
					m_csActionList.SetItemText(lListIndex, 2, pstAction->m_uoAction.m_stUI.m_pcsUI->m_szUIName);
				}

				break;

			case AGCDUI_ACTION_CONTROL:
				if (pstAction->m_uoAction.m_stControl.m_pcsControl)
				{
					/*
					stStatusInfo *	pstStatus = pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(pstAction->m_uoAction.m_stControl.m_lStatus);

					if (pstStatus)
						m_csActionList.SetItemText(lListIndex, 1, pstStatus->m_szStatusName);
					*/

					m_csActionList.SetItemText(lListIndex, 2, pstAction->m_uoAction.m_stControl.m_pcsControl->m_szName);
				}

				break;

			case AGCDUI_ACTION_FUNCTION:
				if (pstAction->m_uoAction.m_stFunction.m_pstFunction)
				{
					m_csActionList.SetItemText(lListIndex, 2, pstAction->m_uoAction.m_stFunction.m_pstFunction->m_szName);
				}

				break;
			}
		}

		m_bCustomizable = m_pstHotkey->m_bCustomizable;
		m_strDescription = m_pstHotkey->m_szDescription ? m_pstHotkey->m_szDescription : "";

		UpdateData(FALSE);
	}
}

void UITHotkeyDialog::OnItemchangedUITHotkeyList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	RefreshActionList();
}

void UITHotkeyDialog::OnUITAddHotkey() 
{
	AgcdUIHotkeyType	eType;
	UINT8				cCode;

	eType = (AgcdUIHotkeyType) m_csKeyType.GetItemData(m_csKeyType.GetCurSel());
	cCode = m_csKeyCode.GetItemData(m_csKeyCode.GetCurSel());

	if (eType == CB_ERR || cCode == CB_ERR)
		return;

	g_pcsAgcmUIManager2->AddHotkey(eType, cCode);

	RefreshHotkeyList();
}

void UITHotkeyDialog::OnBnClickedHotkeyCustomizable()
{
	if (!m_pstHotkey)
	{
		AfxMessageBox("Select Hotkey First !!!");
		return;
	}

	UpdateData();

	m_pstHotkey->m_bCustomizable = m_bCustomizable;
}

void UITHotkeyDialog::OnEnChangeHotkeyDescription()
{
	if (!m_pstHotkey)
	{
		AfxMessageBox("Select Hotkey First !!!");
		return;
	}

	UpdateData();

	g_pcsAgcmUIManager2->SetHotkeyDescription(m_pstHotkey, (LPSTR) (LPCTSTR) m_strDescription);
}
