// UITWPSkillTreeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPSkillTreeDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPSkillTreeDialog property page

IMPLEMENT_DYNCREATE(UITWPSkillTreeDialog, UITPropertyPage)

UITWPSkillTreeDialog::UITWPSkillTreeDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPSkillTreeDialog)
	m_lStartX = 0;
	m_lStartY = 0;
	m_lGapX = 0;
	m_lGapY = 0;
	m_lItemWidth = 0;
	m_lItemHeight = 0;
	m_bMovableItem = FALSE;
	m_bGridItemBottomCount = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPSkillTreeDialog::IDD);

	m_pcsSkillTree	= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPSkillTreeDialog::~UITWPSkillTreeDialog()
{
}

void UITWPSkillTreeDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPSkillTreeDialog)
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_2_2, m_csImage_2_2);
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_1_2, m_csImage_1_2);
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_1_10, m_csImage_1_10);
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_3_3, m_csImage_3_3);
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_2_3, m_csImage_2_3);
	DDX_Control(pDX, IDC_UIT_SKILLTREE_IMAGE_1_3, m_csImage_1_3);
	DDX_Control(pDX, IDC_UIT_GRID_LIST, m_csGridList);
	DDX_Text(pDX, IDC_UIT_GRID_START_X, m_lStartX);
	DDX_Text(pDX, IDC_UIT_GRID_START_Y, m_lStartY);
	DDX_Text(pDX, IDC_UIT_GRID_GAP_X, m_lGapX);
	DDX_Text(pDX, IDC_UIT_GRID_GAP_Y, m_lGapY);
	DDX_Text(pDX, IDC_UIT_GRID_ITEM_WIDTH, m_lItemWidth);
	DDX_Text(pDX, IDC_UIT_GRID_ITEM_HEIGHT, m_lItemHeight);
	DDX_Check(pDX, IDC_UIT_MOVABLE_GRID, m_bMovableItem);
	DDX_Check(pDX, IDC_UIT_ITEMWRITECOUNT_GRID, m_bGridItemBottomCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPSkillTreeDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPSkillTreeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPSkillTreeDialog message handlers

void UITWPSkillTreeDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsSkillTree, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPSkillTreeDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

BOOL UITWPSkillTreeDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsSkillTree, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPSkillTreeDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsSkillTree	= (AcUISkillTree *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsSkillTree || !m_pcsControl)
		return TRUE;

	INT32				lIndex;
	INT32				lListIndex;
	INT32				lImageID;
	AgcdUIUserData *	pstUserData;
	RwTexture *			pstTexture;
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

	if (m_pcsSkillTree)
	{
		if (m_pcsControl->m_uoData.m_stGrid.m_bControlUserData)
			m_csGridList.SelectString(-1, szUSERDATA);
		else if (!m_pcsControl->m_uoData.m_stGrid.m_pstGrid)
			m_csGridList.SelectString(-1, szNULLString);
		else
			m_csGridList.SelectString(-1, m_pcsControl->m_uoData.m_stGrid.m_pstGrid->m_szName);

		m_lStartX		= m_pcsSkillTree->GetStartX()		;
		m_lStartY		= m_pcsSkillTree->GetStartY()		;
		m_lGapX			= m_pcsSkillTree->GetGapX()			;
		m_lGapY			= m_pcsSkillTree->GetGapY()			;
		m_lItemWidth	= m_pcsSkillTree->GetItemWidth()	;
		m_lItemHeight	= m_pcsSkillTree->GetItemHeight()	;

		m_bMovableItem	= m_pcsSkillTree->GetGridItemMovable();
		m_bGridItemBottomCount = m_pcsSkillTree->GetGridItemBottomCountWrite();
	}

	m_csImage_1_3.ResetContent();
	m_csImage_2_3.ResetContent();
	m_csImage_3_3.ResetContent();
	m_csImage_1_10.ResetContent();
	m_csImage_1_2.ResetContent();
	m_csImage_2_2.ResetContent();

	lListIndex = m_csImage_1_3.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_1_3.SetItemData(lListIndex, 0);

	lListIndex = m_csImage_2_3.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_2_3.SetItemData(lListIndex, 0);

	lListIndex = m_csImage_3_3.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_3_3.SetItemData(lListIndex, 0);

	lListIndex = m_csImage_1_10.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_1_10.SetItemData(lListIndex, 0);

	lListIndex = m_csImage_1_2.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_1_2.SetItemData(lListIndex, 0);

	lListIndex = m_csImage_2_2.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImage_2_2.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < m_pcsSkillTree->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		lListIndex = m_csImage_1_3.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_1_3.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImage_2_3.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_2_3.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImage_3_3.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_3_3.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImage_1_10.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_1_10.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImage_1_2.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_1_2.SetItemData(lListIndex, lImageID);

		lListIndex = m_csImage_2_2.AddString(RwTextureGetName(pstTexture));
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csImage_2_2.SetItemData(lListIndex, lImageID);
	}

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_1_3));
	if (pstTexture)
		m_csImage_1_3.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_1_3.SelectString(-1, szNULLString);

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_2_3));
	if (pstTexture)
		m_csImage_2_3.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_2_3.SelectString(-1, szNULLString);

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_3_3));
	if (pstTexture)
		m_csImage_3_3.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_3_3.SelectString(-1, szNULLString);

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_1_10));
	if (pstTexture)
		m_csImage_1_10.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_1_10.SelectString(-1, szNULLString);

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_1_2));
	if (pstTexture)
		m_csImage_1_2.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_1_2.SelectString(-1, szNULLString);

	pstTexture = m_pcsSkillTree->m_csTextureList.GetImage_ID(m_pcsSkillTree->GetSkillBackImage(ACUI_SKILLTREE_TYPE_2_2));
	if (pstTexture)
		m_csImage_2_2.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImage_2_2.SelectString(-1, szNULLString);

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPSkillTreeDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsSkillTree || !m_pcsControl)
		return TRUE;

	UpdateData();

	AgcdUIUserData *	pstGrid;
	INT32				lImageID;

	pstGrid = (AgcdUIUserData *) m_csGridList.GetItemDataPtr(m_csGridList.GetCurSel());
	if (pstGrid == (PVOID) -1)
		pstGrid = NULL;

	if (!pstGrid)
		g_pcsAgcmUIManager2->ResetControlGrid(m_pcsControl);
	else if (pstGrid == (PVOID) 1)
		g_pcsAgcmUIManager2->SetControlGrid(m_pcsControl, NULL, TRUE);
	else
		g_pcsAgcmUIManager2->SetControlGrid(m_pcsControl, pstGrid, FALSE);

	m_pcsSkillTree->SetGridItemDrawInfo(m_lStartX, m_lStartY, m_lGapX, m_lGapY, m_lItemWidth, m_lItemHeight);
	m_pcsSkillTree->SetGridItemMovable(m_bMovableItem);
	m_pcsSkillTree->SetGridItemBottomCountWrite(m_bGridItemBottomCount);

	lImageID = m_csImage_1_3.GetItemData(m_csImage_1_3.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_1_3, lImageID);

	lImageID = m_csImage_2_3.GetItemData(m_csImage_2_3.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_2_3, lImageID);

	lImageID = m_csImage_3_3.GetItemData(m_csImage_3_3.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_3_3, lImageID);

	lImageID = m_csImage_1_10.GetItemData(m_csImage_1_10.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_1_10, lImageID);

	lImageID = m_csImage_1_2.GetItemData(m_csImage_1_2.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_1_2, lImageID);

	lImageID = m_csImage_2_2.GetItemData(m_csImage_2_2.GetCurSel());
	if (lImageID == CB_ERR)
		lImageID = 0;
	m_pcsSkillTree->SetSkillBackImage(ACUI_SKILLTREE_TYPE_2_2, lImageID);

	return TRUE;
}
