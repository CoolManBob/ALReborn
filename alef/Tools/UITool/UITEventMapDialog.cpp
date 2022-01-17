// UITEventMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITEventMapDialog.h"

#include "UITActionDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITEventMapDialog dialog

extern CHAR *g_szArgumentNULL;
extern CHAR *g_aszUIActionType[];
extern CHAR *g_aszUIActionUIType[];


UITEventMapDialog::UITEventMapDialog(AgcdUI *pcsUI, CWnd* pParent /*=NULL*/)
	: CDialog(UITEventMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITEventMapDialog)
	m_strEventName = _T("");
	m_strEventDescription = _T("");
	//}}AFX_DATA_INIT

	m_pcsUI = pcsUI;
	m_pstEventMap = NULL;
}


void UITEventMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITEventMapDialog)
	DDX_Control(pDX, IDC_UIT_ACTION_LIST, m_csActionList);
	DDX_Control(pDX, IDC_UIT_EVENT_LIST, m_csEventList);
	DDX_Text(pDX, IDC_UIT_EVENT_NAME, m_strEventName);
	DDX_Text(pDX, IDC_UIT_EVENT_DESCRIPTION, m_strEventDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITEventMapDialog, CDialog)
	//{{AFX_MSG_MAP(UITEventMapDialog)
	ON_BN_CLICKED(IDC_UIT_ADD_ACTION, OnUITAddAction)
	ON_BN_CLICKED(IDC_UIT_EDIT_ACTION, OnUITEditAction)
	ON_BN_CLICKED(IDC_UIT_DELETE_ACTION, OnUITDeleteAction)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UIT_EVENT_LIST, OnItemchangedUITEventList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITEventMapDialog Event handlers

void UITEventMapDialog::OnUITDeleteAction() 
{
	if (!m_pstEventMap)
	{
		AfxMessageBox("Select Event First !!!");
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

		AgcdUIAction &	stAction = m_pstEventMap->m_listAction[lIndex];

		m_pstEventMap->m_listAction.RemoveData(stAction);
		/*
		memcpy(m_pstEventMap->m_astAction + lIndex, 
			m_pstEventMap->m_astAction + lIndex + 1, 
			(AGCDUIMANAGER2_MAX_ACTIONS - lIndex - 1) * sizeof(AgcdUIAction));
		memset(m_pstEventMap->m_astAction + AGCDUIMANAGER2_MAX_ACTIONS - 1, 0, sizeof(AgcdUIAction));

		--(m_pstEventMap->m_lCount);
		*/

		RefreshActionList();
	}
}

void UITEventMapDialog::OnUITAddAction() 
{
	if (!m_pstEventMap)
	{
		AfxMessageBox("Select Event First !!!");
		return;
	}

	AgcdUIAction	stAction;

	memset(&stAction, 0, sizeof(stAction));

	UITActionDialog		dlgAction(m_pcsUI, &stAction);

	if (dlgAction.DoModal() == IDOK)
	{
		m_pstEventMap->m_listAction.AddTail(stAction);
	}

	RefreshActionList();
}

void UITEventMapDialog::OnUITEditAction() 
{
	if (!m_pstEventMap)
	{
		AfxMessageBox("Select Event First !!!");
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

		AgcdUIAction &	stAction = m_pstEventMap->m_listAction[lIndex];

		UITActionDialog	dlgAction(m_pcsUI, &stAction);

		dlgAction.DoModal();

		RefreshActionList();
	}
}

BOOL UITEventMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIEventMap *	pstEventMap;

	m_csEventList.InsertColumn(0, "Event", LVCFMT_LEFT, 305);

	m_csActionList.InsertColumn(0, "Action", LVCFMT_LEFT, 60);
	m_csActionList.InsertColumn(1, "Type", LVCFMT_LEFT, 125);
	m_csActionList.InsertColumn(2, "Target", LVCFMT_LEFT, 120);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetEventCount(); ++lIndex)
	{
		pstEventMap = g_pcsAgcmUIManager2->GetEventMap(lIndex);

		lListIndex = m_csEventList.InsertItem(lIndex, pstEventMap->m_szName);
		if (lListIndex == -1)
			return FALSE;

		m_csEventList.SetItemData(lListIndex, lIndex);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

VOID UITEventMapDialog::RefreshActionList()
{
	INT32					lListIndex;
	INT32					lIndex;
	POSITION				pos = m_csEventList.GetFirstSelectedItemPosition();
	AgcdUIAction *			pstAction;

	UpdateData();

	if (pos)
	{
		lListIndex = m_csEventList.GetNextSelectedItem(pos);
		lIndex = m_csEventList.GetItemData(lListIndex);
		m_pstEventMap = g_pcsAgcmUIManager2->GetEventMap(lIndex);

		if (!m_pstEventMap)
		{
			m_strEventName = "";

			return;
		}

		m_strEventName = m_pstEventMap->m_szName;

		m_csActionList.DeleteAllItems();

		for (lIndex = 0; lIndex < m_pstEventMap->m_listAction.GetCount(); ++lIndex)
		{
			pstAction = &(m_pstEventMap->m_listAction[lIndex]);

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
					stStatusInfo *	pstStatus = pstAction->m_uoAction.m_stControl.m_pcsControl->m_pcsBase->GetStatusInfo_ID(pstAction->m_uoAction.m_stControl.);

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

		UpdateData(FALSE);
	}
}

void UITEventMapDialog::OnItemchangedUITEventList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	RefreshActionList();
}
