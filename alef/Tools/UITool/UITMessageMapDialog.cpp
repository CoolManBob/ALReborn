// UITMessageMapDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UITool.h"
#include "UITMessageMapDialog.h"
#include "UITActionDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITMessageMapDialog dialog

extern CHAR *g_szArgumentNULL;
extern CHAR *g_aszUIActionType[];
extern CHAR *g_aszUIActionUIType[];

UITMessageMapDialog::UITMessageMapDialog(AgcdUI *pcsUI, AgcdUIControl *pcsControl, CWnd* pParent /*=NULL*/)
	: CDialog(UITMessageMapDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(UITMessageMapDialog)
	m_strMessageName = _T("");
	//}}AFX_DATA_INIT

	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	m_pstMessageMap	= NULL;
}


void UITMessageMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITMessageMapDialog)
	DDX_Control(pDX, IDC_UIT_ACTION_LIST, m_csActionList);
	DDX_Control(pDX, IDC_UIT_MESSAGE_LIST, m_csMessageList);
	DDX_Text(pDX, IDC_UIT_MESSAGE_NAME, m_strMessageName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITMessageMapDialog, CDialog)
	//{{AFX_MSG_MAP(UITMessageMapDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_UIT_MESSAGE_LIST, OnItemchangedUITMessageList)
	ON_BN_CLICKED(IDC_UIT_DELETE_ACTION, OnUITDeleteAction)
	ON_BN_CLICKED(IDC_UIT_ADD_ACTION, OnUITAddAction)
	ON_BN_CLICKED(IDC_UIT_EDIT_ACTION, OnUITEditAction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITMessageMapDialog message handlers

BOOL UITMessageMapDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	INT32				lIndex;
	INT32				lListIndex;

	m_csMessageList.InsertColumn(0, "Message", LVCFMT_LEFT, 305);

	m_csActionList.InsertColumn(0, "Action", LVCFMT_LEFT, 60);
	m_csActionList.InsertColumn(1, "Type(Status)", LVCFMT_LEFT, 110);
	m_csActionList.InsertColumn(2, "Target", LVCFMT_LEFT, 135);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->m_alControlMessages[m_pcsControl->m_lType]; ++lIndex)
	{
		lListIndex = m_csMessageList.InsertItem(lIndex, g_aszMessages[m_pcsControl->m_lType][lIndex]);
		if (lListIndex == -1)
			return FALSE;

		m_csMessageList.SetItemData(lListIndex, lIndex);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITMessageMapDialog::OnItemchangedUITMessageList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	*pResult = 0;

	RefreshActionList();
}

VOID UITMessageMapDialog::RefreshActionList()
{
	INT32					lListIndex;
	INT32					lIndex;
	POSITION				pos = m_csMessageList.GetFirstSelectedItemPosition();
	AgcdUIAction *			pstAction;

	UpdateData();

	if (pos)
	{
		lListIndex = m_csMessageList.GetNextSelectedItem(pos);
		lIndex = m_csMessageList.GetItemData(lListIndex);
		
		m_pstMessageMap = m_pcsControl->m_pstMessageMaps + lIndex;		

		if (!m_pstMessageMap)
		{
			m_strMessageName = "";

			return;
		}

		m_strMessageName = g_aszMessages[m_pcsControl->m_lType][lIndex];

		m_csActionList.DeleteAllItems();

		for (lIndex = 0; lIndex < m_pstMessageMap->m_listAction.GetCount(); ++lIndex)
		{
			pstAction = &(m_pstMessageMap->m_listAction[lIndex]);

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

		UpdateData(FALSE);
	}
}

void UITMessageMapDialog::OnCancel() 
{
	//CDialog::OnCancel();
}

void UITMessageMapDialog::OnOK() 
{
	CDialog::OnOK();
}

void UITMessageMapDialog::OnUITDeleteAction() 
{
	if (!m_pstMessageMap)
	{
		AfxMessageBox("Select Message First !!!");
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

		AgcdUIAction &	stAction = m_pstMessageMap->m_listAction[lIndex];

		m_pstMessageMap->m_listAction.RemoveData(stAction);

		/*
		memcpy(m_pstMessageMap->m_astAction + lIndex, 
			m_pstMessageMap->m_astAction + lIndex + 1, 
			(AGCDUIMANAGER2_MAX_ACTIONS - lIndex - 1) * sizeof(AgcdUIAction));
		memset(m_pstMessageMap->m_astAction + AGCDUIMANAGER2_MAX_ACTIONS - 1, 0, sizeof(AgcdUIAction));

		--(m_pstMessageMap->m_lCount);
		*/

		RefreshActionList();
	}
}

void UITMessageMapDialog::OnUITAddAction() 
{
	if (!m_pstMessageMap)
	{
		AfxMessageBox("Select Message First !!!");
		return;
	}

	AgcdUIAction	stAction;

	memset(&stAction, 0, sizeof(stAction));

	UITActionDialog		dlgAction(m_pcsUI, &stAction);

	if (dlgAction.DoModal() == IDOK)
	{
		m_pstMessageMap->m_listAction.AddTail(stAction);
	}

	RefreshActionList();
}

void UITMessageMapDialog::OnUITEditAction() 
{
	if (!m_pstMessageMap)
	{
		AfxMessageBox("Select Message First !!!");
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

		AgcdUIAction &	stAction = m_pstMessageMap->m_listAction[lIndex];

		UITActionDialog	dlgAction(m_pcsUI, &stAction);

		dlgAction.DoModal();

		RefreshActionList();
	}
}
