// UITWPListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPListDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPListDialog property page

IMPLEMENT_DYNCREATE(UITWPListDialog, UITPropertyPage)

UITWPListDialog::UITWPListDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPListDialog)
	m_lItemNum = 0;
	m_bStartAtBottom = FALSE;
	m_ulItemColumn = 0;
	//}}AFX_DATA_INIT

	Construct(UITWPListDialog::IDD);

	m_pcsList		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPListDialog::~UITWPListDialog()
{
}

void UITWPListDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPListDialog)
	DDX_Text(pDX, IDC_UIT_LIST_ITEM, m_lItemNum);
	DDX_Check(pDX, IDC_UIT_LIST_START_BOTTOM, m_bStartAtBottom);
	DDX_Text(pDX, IDC_UIT_LIST_ITEM_COLUMN, m_ulItemColumn);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_UIT_LIST_SELECTION_IMAGE, m_csImageSelection);
}


BEGIN_MESSAGE_MAP(UITWPListDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPListDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPListDialog message handlers

BOOL UITWPListDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsList, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPListDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsList		= (AcUIList *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsList || !m_pcsControl)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	INT32				lImageID;
	static CHAR *		szNULLString = "NULL";
	RwTexture *			pstTexture;

	m_lItemNum = GET_CONTROL_STYLE(m_pcsControl)->m_lListItemVisibleRow;
	m_ulItemColumn = m_pcsList->m_lItemColumn;
	m_bStartAtBottom = m_pcsList->m_bStartAtBottom;

	m_csImageSelection.ResetContent();

	lListIndex = m_csImageSelection.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageSelection.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex < m_pcsList->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsList->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csImageSelection.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImageSelection.SetItemData(lListIndex, lImageID);
	}

	pstTexture = m_pcsList->m_csTextureList.GetImage_ID(m_pcsList->m_lSelectedItemTextureID);
	if (pstTexture)
		m_csImageSelection.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageSelection.SelectString(-1, szNULLString);

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPListDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsList || !m_pcsControl)
		return TRUE;

	UpdateData();

	GET_CONTROL_STYLE(m_pcsControl)->m_lListItemVisibleRow = m_lItemNum;
	m_pcsList->SetListItemColumn(m_ulItemColumn);
	m_pcsList->m_bStartAtBottom = m_bStartAtBottom;

	INT32	lImageID = m_csImageSelection.GetItemData(m_csImageSelection.GetCurSel());
	if (lImageID == -1)
		lImageID = 0;

	m_pcsList->SetSelectedTexture(lImageID);

	return TRUE;
}

void UITWPListDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsList, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPListDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

