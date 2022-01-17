// UITWPTreeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPTreeDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPTreeDialog property page

IMPLEMENT_DYNCREATE(UITWPTreeDialog, UITPropertyPage)

UITWPTreeDialog::UITWPTreeDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPTreeDialog)
	m_lItemNum = 0;
	m_bStartAtBottom = FALSE;
	m_ulItemColumn = 0;
	m_lDepthMargin = 0;
	//}}AFX_DATA_INIT

	Construct(UITWPTreeDialog::IDD);

	m_pcsTree		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPTreeDialog::~UITWPTreeDialog()
{
}

void UITWPTreeDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPTreeDialog)
	DDX_Control(pDX, IDC_UIT_TREE_CLOSE_BUTTON, m_csButtonClose);
	DDX_Control(pDX, IDC_UIT_TREE_OPEN_BUTTON, m_csButtonOpen);
	DDX_Control(pDX, IDC_UIT_TREE_DEPTH_USERDATA, m_csUDDepth);
	DDX_Text(pDX, IDC_UIT_LIST_ITEM, m_lItemNum);
	DDX_Check(pDX, IDC_UIT_LIST_START_BOTTOM, m_bStartAtBottom);
	DDX_Text(pDX, IDC_UIT_LIST_ITEM_COLUMN, m_ulItemColumn);
	DDX_Text(pDX, IDC_UIT_TREE_DEPTH_MARGIN, m_lDepthMargin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPTreeDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPTreeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPTreeDialog message handlers

BOOL UITWPTreeDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsTree, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPTreeDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsTree		= (AcUITree *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsTree || !m_pcsControl)
		return TRUE;

	INT32			lIndex;
	INT32				lListIndex;
	static CHAR *		szNULLString = "NULL";
	AgcdUIUserData *	pstUserData;
	AgcdUIBoolean *		pstBoolean;

	m_csUDDepth.ResetContent();
	m_csButtonOpen.ResetContent();
	m_csButtonClose.ResetContent();

	lListIndex = m_csButtonOpen.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csButtonOpen.SetItemData(lListIndex, 0);

	lListIndex = m_csButtonClose.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csButtonClose.SetItemData(lListIndex, 0);

	lIndex = 0;
	for (pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex); pcsControl; pcsControl = g_pcsAgcmUIManager2->GetSequenceControl(m_pcsUI, &lIndex))
	{
		if (pcsControl->m_lType == AcUIBase::TYPE_BUTTON)
		{
			lListIndex = m_csButtonOpen.AddString(pcsControl->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csButtonOpen.SetItemDataPtr(lListIndex, pcsControl);

			if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_OPEN)
				m_csButtonOpen.SelectString(-1, pcsControl->m_szName);

			lListIndex = m_csButtonClose.AddString(pcsControl->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csButtonClose.SetItemDataPtr(lListIndex, pcsControl);

			if (pcsControl->m_uoData.m_stButton.m_eButtonType == AGCDUI_BUTTON_TYPE_TREE_CLOSE)
				m_csButtonClose.SelectString(-1, pcsControl->m_szName);
		}
	}

	lListIndex = m_csUDDepth.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csUDDepth.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csUDDepth.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csUDDepth.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	if (m_pcsControl->m_uoData.m_stTree.m_pstDepthUserData)
		m_csUDDepth.SelectString(-1, m_pcsControl->m_uoData.m_stTree.m_pstDepthUserData->m_szName);
	else
		m_csUDDepth.SelectString(-1, szNULLString);

	m_lItemNum = GET_CONTROL_STYLE(m_pcsControl)->m_lListItemVisibleRow;
	m_ulItemColumn = m_pcsTree->m_lItemColumn;
	m_bStartAtBottom = m_pcsTree->m_bStartAtBottom;
	m_lDepthMargin = m_pcsTree->m_lItemDepthMargin;

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPTreeDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsTree || !m_pcsControl)
		return TRUE;

	AgcdUIControl *		pcsControlFormat = m_pcsControl->m_uoData.m_stTree.m_pcsListItemFormat;
	AcUITreeItem *		pcsTreeItem = NULL;
	AgcdUIControl *		pcsControl;
	AgcdUIUserData *	pstUserData;

	UpdateData();

	if (pcsControlFormat)
	{
		pcsTreeItem = (AcUITreeItem *) pcsControlFormat->m_pcsBase;
	}

	pcsControl = (AgcdUIControl *) m_csButtonOpen.GetItemDataPtr(m_csButtonOpen.GetCurSel());
	if (pcsControl && pcsControl != (PVOID) -1)
	{
		pcsControl->m_uoData.m_stButton.m_eButtonType = AGCDUI_BUTTON_TYPE_TREE_OPEN;

		if (pcsTreeItem)
			pcsTreeItem->SetTreeButtonOpen((AcUIButton *) pcsControl->m_pcsBase);
	}

	pcsControl = (AgcdUIControl *) m_csButtonClose.GetItemDataPtr(m_csButtonClose.GetCurSel());
	if (pcsControl && pcsControl != (PVOID) -1)
	{
		pcsControl->m_uoData.m_stButton.m_eButtonType = AGCDUI_BUTTON_TYPE_TREE_CLOSE;

		if (pcsTreeItem)
			pcsTreeItem->SetTreeButtonClose((AcUIButton *) pcsControl->m_pcsBase);
	}

	pstUserData = (AgcdUIUserData *) m_csUDDepth.GetItemDataPtr(m_csUDDepth.GetCurSel());
	if (pstUserData == (PVOID) -1)
		pstUserData = NULL;

	m_pcsControl->m_uoData.m_stTree.m_pstDepthUserData = pstUserData;

	GET_CONTROL_STYLE(m_pcsControl)->m_lListItemVisibleRow = m_lItemNum;
	m_pcsTree->SetListItemColumn(m_ulItemColumn);
	m_pcsTree->m_bStartAtBottom = m_bStartAtBottom;
	m_pcsTree->m_lItemDepthMargin = m_lDepthMargin;

	return TRUE;
}

void UITWPTreeDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsTree, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPTreeDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

