// UITWPComboDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPComboDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPComboDialog property page

IMPLEMENT_DYNCREATE(UITWPComboDialog, UITPropertyPage)

UITWPComboDialog::UITWPComboDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPComboDialog)
	m_lLineHeight = 0;
	//}}AFX_DATA_INIT

	Construct(UITWPComboDialog::IDD);

	m_pcsCombo		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPComboDialog::~UITWPComboDialog()
{
}

void UITWPComboDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPComboDialog)
	DDX_Control(pDX, IDC_UIT_COMBO_BOTTOM, m_csImageBottom);
	DDX_Control(pDX, IDC_UIT_COMBO_CLICK, m_csImageClick);
	DDX_Control(pDX, IDC_UIT_COMBO_ONMOUSE, m_csImageMouse);
	DDX_Control(pDX, IDC_UIT_COMBO_NORMAL, m_csImageNormal);
	DDX_Text(pDX, IDC_UIT_COMBO_HEIGHT, m_lLineHeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPComboDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPComboDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPComboDialog message handlers

void UITWPComboDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsCombo, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPComboDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

BOOL UITWPComboDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsCombo, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPComboDialog::OnInitDialog() 
{
	UITPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL UITWPComboDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsCombo		= (AcUICombo *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsCombo || !m_pcsControl)
		return TRUE;

	m_csImageNormal.ResetContent();
	m_csImageMouse.ResetContent();
	m_csImageClick.ResetContent();

	INT32		lIndex;
	INT32		lListIndex;
	INT32		lImageID;
	RwTexture *	pstTexture;
	static CHAR *		szNULLString = "NULL";

	// Set Images
	lListIndex = m_csImageNormal.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageNormal.SetItemData(lListIndex, 0);

	lListIndex = m_csImageMouse.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageMouse.SetItemData(lListIndex, 0);

	lListIndex = m_csImageClick.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageClick.SetItemData(lListIndex, 0);

	lListIndex = m_csImageBottom.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csImageBottom.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < m_pcsCombo->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsCombo->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		if (pstTexture)
		{
			lListIndex = m_csImageNormal.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csImageNormal.SetItemData(lListIndex, lImageID);

			lListIndex = m_csImageMouse.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csImageMouse.SetItemData(lListIndex, lImageID);

			lListIndex = m_csImageClick.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csImageClick.SetItemData(lListIndex, lImageID);

			lListIndex = m_csImageBottom.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csImageBottom.SetItemData(lListIndex, lImageID);
		}
	}

	pstTexture = m_pcsCombo->m_csTextureList.GetImage_ID(m_pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_NORMAL));
	if (pstTexture)
		m_csImageNormal.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageNormal.SelectString(-1, szNULLString);

	pstTexture = m_pcsCombo->m_csTextureList.GetImage_ID(m_pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_ONMOUSE));
	if (pstTexture)
		m_csImageMouse.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageMouse.SelectString(-1, szNULLString);

	pstTexture = m_pcsCombo->m_csTextureList.GetImage_ID(m_pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_CLICK));
	if (pstTexture)
		m_csImageClick.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageClick.SelectString(-1, szNULLString);

	pstTexture = m_pcsCombo->m_csTextureList.GetImage_ID(m_pcsCombo->GetDropDownTexture(ACUICOMBO_TEXTURE_BOTTOM));
	if (pstTexture)
		m_csImageBottom.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csImageBottom.SelectString(-1, szNULLString);

	m_lLineHeight = m_pcsCombo->GetOpenLineHeight();

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPComboDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsCombo || !m_pcsControl)
		return TRUE;

	UpdateData();

	INT32	lImageID;

	// Set Images
	lImageID = m_csImageNormal.GetItemData(m_csImageNormal.GetCurSel());
	if (lImageID >= 0)
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_NORMAL, lImageID);
	else
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_NORMAL, 0);

	lImageID = m_csImageMouse.GetItemData(m_csImageMouse.GetCurSel());
	if (lImageID >= 0)
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_ONMOUSE, lImageID);
	else
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_ONMOUSE, 0);

	lImageID = m_csImageClick.GetItemData(m_csImageClick.GetCurSel());
	if (lImageID >= 0)
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_CLICK, lImageID);
	else
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_CLICK, 0);

	lImageID = m_csImageBottom.GetItemData(m_csImageBottom.GetCurSel());
	if (lImageID >= 0)
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_BOTTOM, lImageID);
	else
		m_pcsCombo->SetDropDownTexture(ACUICOMBO_TEXTURE_BOTTOM, 0);

	m_pcsCombo->SetOpenLineHeight(m_lLineHeight);

	return TRUE;
}
