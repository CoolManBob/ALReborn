// UITWPGridDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPGridDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPGridDialog property page

IMPLEMENT_DYNCREATE(UITWPGridDialog, UITPropertyPage)

UITWPGridDialog::UITWPGridDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPGridDialog)
	m_lStartX = 0;
	m_lStartY = 0;
	m_lGapX = 0;
	m_lGapY = 0;
	m_lItemWidth = 0;
	m_lItemHeight = 0;
	m_bMovableItem = FALSE;
	m_bGridItemBottomCount = FALSE;
	m_bMoveItemCopy = FALSE;
	m_bDrawImageForeground = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPGridDialog::IDD);

	m_pcsGrid		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPGridDialog::~UITWPGridDialog()
{
}

void UITWPGridDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPGridDialog)
	DDX_Control(pDX, IDC_UIT_GRID_LIST, m_csGridList);
	DDX_Text(pDX, IDC_UIT_GRID_START_X, m_lStartX);
	DDX_Text(pDX, IDC_UIT_GRID_START_Y, m_lStartY);
	DDX_Text(pDX, IDC_UIT_GRID_GAP_X, m_lGapX);
	DDX_Text(pDX, IDC_UIT_GRID_GAP_Y, m_lGapY);
	DDX_Text(pDX, IDC_UIT_GRID_ITEM_WIDTH, m_lItemWidth);
	DDX_Text(pDX, IDC_UIT_GRID_ITEM_HEIGHT, m_lItemHeight);
	DDX_Check(pDX, IDC_UIT_MOVABLE_GRID, m_bMovableItem);
	DDX_Check(pDX, IDC_UIT_ITEMWRITECOUNT_GRID, m_bGridItemBottomCount);
	DDX_Check(pDX, IDC_UIT_MOVEITEMCOPY_GRID, m_bMoveItemCopy);
	DDX_Check(pDX, IDC_UIT_DRAWFOREGROUND_GRID, m_bDrawImageForeground);
	DDX_Control(pDX, IDC_UIT_REUSABLE_DISPLAY, m_csReusableDisplayList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPGridDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPGridDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPGridDialog message handlers

void UITWPGridDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsGrid, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPGridDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

BOOL UITWPGridDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsGrid, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPGridDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsGrid		= (AcUIGrid *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsGrid || !m_pcsControl)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	AgcdUIUserData *	pstUserData;
	static CHAR *		szNULLString = "NULL";
	static CHAR *		szUSERDATA = "<User Data>";

	m_csGridList.ResetContent();

	lListIndex = m_csGridList.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;

	m_csGridList.SetItemDataPtr(lListIndex, NULL);

	lListIndex = m_csGridList.AddString(szUSERDATA);
	if (lListIndex == CB_ERR)
		return FALSE;

	m_csGridList.SetItemDataPtr(lListIndex, (VOID *) 1);

	for (lIndex = 0; lIndex < g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData && pstUserData->m_eType == AGCDUI_USERDATA_TYPE_GRID)
		{
			lListIndex = m_csGridList.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csGridList.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	m_csReusableDisplayList.ResetContent();

	lListIndex = m_csReusableDisplayList.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csReusableDisplayList.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < m_pcsGrid->m_csTextureList.GetCount(); ++lIndex)
	{
		INT32		lImageID;
		RwTexture	*pstTexture = m_pcsGrid->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		if (pstTexture)
		{
			lListIndex = m_csReusableDisplayList.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csReusableDisplayList.SetItemData(lListIndex, lImageID);
		}
	}

	RwTexture	*pstTexture = m_pcsGrid->m_csTextureList.GetImage_ID(m_pcsGrid->GetReusableDisplayImage());
	if (pstTexture)
		m_csReusableDisplayList.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csReusableDisplayList.SelectString(-1, szNULLString);

	if (m_pcsGrid)
	{
		if (m_pcsControl->m_uoData.m_stGrid.m_bControlUserData)
			m_csGridList.SelectString(-1, szUSERDATA);
		else if (!m_pcsControl->m_uoData.m_stGrid.m_pstGrid)
			m_csGridList.SelectString(-1, szNULLString);
		else
			m_csGridList.SelectString(-1, m_pcsControl->m_uoData.m_stGrid.m_pstGrid->m_szName);

		m_lStartX		= m_pcsGrid->GetStartX()		;
		m_lStartY		= m_pcsGrid->GetStartY()		;
		m_lGapX			= m_pcsGrid->GetGapX()			;
		m_lGapY			= m_pcsGrid->GetGapY()			;
		m_lItemWidth	= m_pcsGrid->GetItemWidth()		;
		m_lItemHeight	= m_pcsGrid->GetItemHeight()	;

		m_bMovableItem	= m_pcsGrid->GetGridItemMovable()					;
		m_bMoveItemCopy	= m_pcsGrid->GetGridItemMoveItemCopy()				;
		m_bGridItemBottomCount = m_pcsGrid->GetGridItemBottomCountWrite()	;
		m_bDrawImageForeground = m_pcsGrid->GetGridItemDrawImageForeground()	;
	}

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPGridDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsGrid || !m_pcsControl)
		return TRUE;

	UpdateData();

	AgcdUIUserData *	pstGrid;

	pstGrid = (AgcdUIUserData *) m_csGridList.GetItemDataPtr(m_csGridList.GetCurSel());
	if (pstGrid == (PVOID) -1)
		pstGrid = NULL;

	if (!pstGrid)
		g_pcsAgcmUIManager2->ResetControlGrid(m_pcsControl);
	else if (pstGrid == (PVOID) 1)
		g_pcsAgcmUIManager2->SetControlGrid(m_pcsControl, NULL, TRUE);
	else
		g_pcsAgcmUIManager2->SetControlGrid(m_pcsControl, pstGrid, FALSE);

	m_pcsGrid->SetGridItemDrawInfo(m_lStartX, m_lStartY, m_lGapX, m_lGapY, m_lItemWidth, m_lItemHeight);
	m_pcsGrid->SetGridItemMovable(m_bMovableItem);
	m_pcsGrid->SetGridItemBottomCountWrite(m_bGridItemBottomCount);
	m_pcsGrid->SetGridItemMoveItemCopy(m_bMoveItemCopy);
	m_pcsGrid->SetGridItemDrawImageForeground(m_bDrawImageForeground);

	INT32 lImageID = m_csReusableDisplayList.GetItemData(m_csReusableDisplayList.GetCurSel());
	if (lImageID >= 0)
		m_pcsGrid->SetReusableDisplayImage(lImageID);
	else
		m_pcsGrid->SetReusableDisplayImage(ACUIBUTTON_INIT_INDEX);

	return TRUE;
}

BOOL UITWPGridDialog::OnInitDialog() 
{
	UITPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
