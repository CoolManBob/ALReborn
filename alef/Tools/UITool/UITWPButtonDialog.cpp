// UITWPButtonDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPButtonDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPButtonDialog property page

IMPLEMENT_DYNCREATE(UITWPButtonDialog, UITPropertyPage)

UITWPButtonDialog::UITWPButtonDialog() : UITPropertyPage()
{
	//{{AFX_DATA_INIT(UITWPButtonDialog)
	m_bPushButton = FALSE;
	m_lTextOffsetX = 0;
	m_lTextOffsetY = 0;
	//}}AFX_DATA_INIT

	Construct(UITWPButtonDialog::IDD);

	m_pcsButton	= NULL	;
	m_pcsControl = NULL	;
}

UITWPButtonDialog::~UITWPButtonDialog()
{
}

void UITWPButtonDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPButtonDialog)
	DDX_Control(pDX, IDC_UIT_BUTTON_ENABLE_USERDATA, m_csEnableUserData);
	DDX_Control(pDX, IDC_UIT_BUTTON_ENABLE_BOOLEAN, m_csEnableBoolean);
	DDX_Control(pDX, IDC_UIT_BUTTON_CHECK_USERDATA, m_csCheckUserData);
	DDX_Control(pDX, IDC_UIT_BUTTON_CHECK_BOOLEAN, m_csCheckBoolean);
	DDX_Control(pDX, IDC_UIT_BUTTON_DISABLE, m_csDisable);
	DDX_Control(pDX, IDC_UIT_BUTTON_CLICK, m_csClick);
	DDX_Control(pDX, IDC_UIT_BUTTON_ONMOUSE, m_csOnMouse);
	DDX_Control(pDX, IDC_UIT_BUTTON_NORMAL, m_csNormal);
	DDX_Check(pDX, IDC_UIT_PUSH_BUTTON, m_bPushButton);
	DDX_Check(pDX, IDC_UIT_START_ACTIVE, m_bStartOnClickStatus);
	DDX_Text(pDX, IDC_UIT_BUTTON_TEXT_OFFSET_X, m_lTextOffsetX);
	DDX_Text(pDX, IDC_UIT_BUTTON_TEXT_OFFSET_Y, m_lTextOffsetY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(UITWPButtonDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPButtonDialog)
	ON_BN_CLICKED(IDC_UIT_BUTTON_APPLY, OnUITButtonApply)
	ON_BN_CLICKED(IDC_UIT_BUTTON_CANCEL, OnUITButtonCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPButtonDialog message handlers

BOOL UITWPButtonDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsButton, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPButtonDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsButton = (AcUIButton *) pcsBase;
	m_pcsControl = pcsControl;
	m_pcsUI = pcsUI;

	if (!GetSafeHwnd() || !m_pcsButton)
		return TRUE;

	m_csNormal.ResetContent();
	m_csOnMouse.ResetContent();
	m_csClick.ResetContent();
	m_csDisable.ResetContent();

	INT32		lIndex;
	INT32		lListIndex;
	INT32		lImageID;
	RwTexture *	pstTexture;
	static CHAR *		szNULLString = "NULL";
	AgcdUIUserData *	pstUserData;
	AgcdUIBoolean *		pstBoolean;

	// Set Images
	lListIndex = m_csNormal.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csNormal.SetItemData(lListIndex, 0);

	lListIndex = m_csOnMouse.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csOnMouse.SetItemData(lListIndex, 0);

	lListIndex = m_csClick.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csClick.SetItemData(lListIndex, 0);

	lListIndex = m_csDisable.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csDisable.SetItemData(lListIndex, 0);

	for (lIndex = 0; lIndex < m_pcsButton->m_csTextureList.GetCount(); ++lIndex)
	{
		pstTexture = m_pcsButton->m_csTextureList.GetImage_Index(lIndex, &lImageID);

		if (pstTexture)
		{
			lListIndex = m_csNormal.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csNormal.SetItemData(lListIndex, lImageID);

			lListIndex = m_csOnMouse.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csOnMouse.SetItemData(lListIndex, lImageID);

			lListIndex = m_csClick.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csClick.SetItemData(lListIndex, lImageID);

			lListIndex = m_csDisable.AddString(RwTextureGetName(pstTexture));
			if (lListIndex == CB_ERR)
				return FALSE;
			m_csDisable.SetItemData(lListIndex, lImageID);
		}
	}

	pstTexture = m_pcsButton->m_csTextureList.GetImage_ID(m_pcsButton->GetButtonImage(ACUIBUTTON_MODE_NORMAL));
	if (pstTexture)
		m_csNormal.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csNormal.SelectString(-1, szNULLString);

	pstTexture = m_pcsButton->m_csTextureList.GetImage_ID(m_pcsButton->GetButtonImage(ACUIBUTTON_MODE_ONMOUSE));
	if (pstTexture)
		m_csOnMouse.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csOnMouse.SelectString(-1, szNULLString);

	pstTexture = m_pcsButton->m_csTextureList.GetImage_ID(m_pcsButton->GetButtonImage(ACUIBUTTON_MODE_CLICK));
	if (pstTexture)
		m_csClick.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csClick.SelectString(-1, szNULLString);

	pstTexture = m_pcsButton->m_csTextureList.GetImage_ID(m_pcsButton->GetButtonImage(ACUIBUTTON_MODE_DISABLE));
	if (pstTexture)
		m_csDisable.SelectString(-1, RwTextureGetName(pstTexture));
	else
		m_csDisable.SelectString(-1, szNULLString);

	// Flags
	m_bPushButton = m_pcsButton->m_stProperty.m_bPushButton;
	m_bStartOnClickStatus = m_pcsButton->m_stProperty.m_bStartOnClickStatus;

	//////////////////////////////////////////////////////
	////////////// Check
	//////////////////////////////////////////////////////

	// Check Boolean
	m_csCheckBoolean.ResetContent();

	lListIndex = m_csCheckBoolean.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csCheckBoolean.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetBooleanCount(); ++lIndex)
	{
		pstBoolean = g_pcsAgcmUIManager2->GetBoolean(lIndex);
		if (pstBoolean)
		{
			lListIndex = m_csCheckBoolean.AddString(pstBoolean->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csCheckBoolean.SetItemDataPtr(lListIndex, pstBoolean);
		}
	}

	if (m_pcsControl->m_uoData.m_stButton.m_pstCheckBoolean)
		m_csCheckBoolean.SelectString(-1, m_pcsControl->m_uoData.m_stButton.m_pstCheckBoolean->m_szName);
	else
		m_csCheckBoolean.SelectString(-1, szNULLString);

	// Check UserData
	m_csCheckUserData.ResetContent();

	lListIndex = m_csCheckUserData.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csCheckUserData.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csCheckUserData.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csCheckUserData.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	if (m_pcsControl->m_uoData.m_stButton.m_pstCheckUserData)
		m_csCheckUserData.SelectString(-1, m_pcsControl->m_uoData.m_stButton.m_pstCheckUserData->m_szName);
	else
		m_csCheckUserData.SelectString(-1, szNULLString);

	//////////////////////////////////////////////////////
	////////////// Enable
	//////////////////////////////////////////////////////

	// Enable Boolean
	m_csEnableBoolean.ResetContent();

	lListIndex = m_csEnableBoolean.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csEnableBoolean.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetBooleanCount(); ++lIndex)
	{
		pstBoolean = g_pcsAgcmUIManager2->GetBoolean(lIndex);
		if (pstBoolean)
		{
			lListIndex = m_csEnableBoolean.AddString(pstBoolean->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csEnableBoolean.SetItemDataPtr(lListIndex, pstBoolean);
		}
	}

	if (m_pcsControl->m_uoData.m_stButton.m_pstEnableBoolean)
		m_csEnableBoolean.SelectString(-1, m_pcsControl->m_uoData.m_stButton.m_pstEnableBoolean->m_szName);
	else
		m_csEnableBoolean.SelectString(-1, szNULLString);

	// Enable UserData
	m_csEnableUserData.ResetContent();

	lListIndex = m_csEnableUserData.AddString(szNULLString);
	if (lListIndex == CB_ERR)
		return FALSE;
	m_csEnableUserData.SetItemDataPtr(lListIndex, NULL);

	for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
	{
		pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
		if (pstUserData)
		{
			lListIndex = m_csEnableUserData.AddString(pstUserData->m_szName);
			if (lListIndex == CB_ERR)
				return FALSE;

			m_csEnableUserData.SetItemDataPtr(lListIndex, pstUserData);
		}
	}

	if (m_pcsControl->m_uoData.m_stButton.m_pstEnableUserData)
		m_csEnableUserData.SelectString(-1, m_pcsControl->m_uoData.m_stButton.m_pstEnableUserData->m_szName);
	else
		m_csEnableUserData.SelectString(-1, szNULLString);

	m_lTextOffsetX = m_pcsButton->m_lButtonDownStringOffsetX;
	m_lTextOffsetY = m_pcsButton->m_lButtonDownStringOffsetY;

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPButtonDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	return TRUE;
}

BOOL UITWPButtonDialog::OnInitDialog() 
{
	UITPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITWPButtonDialog::OnUITButtonApply() 
{
	if (!m_pcsButton)
		return;

	UpdateData();

	INT32	lImageID;

	// Set Images
	lImageID = m_csNormal.GetItemData(m_csNormal.GetCurSel());
	if (lImageID >= 0)
		m_pcsButton->SetButtonImage(lImageID, ACUIBUTTON_MODE_NORMAL);
	else
		m_pcsButton->SetButtonImage(ACUIBUTTON_INIT_INDEX, ACUIBUTTON_MODE_NORMAL);

	lImageID = m_csOnMouse.GetItemData(m_csOnMouse.GetCurSel());
	if (lImageID >= 0)
		m_pcsButton->SetButtonImage(lImageID, ACUIBUTTON_MODE_ONMOUSE);
	else
		m_pcsButton->SetButtonImage(ACUIBUTTON_INIT_INDEX, ACUIBUTTON_MODE_ONMOUSE);

	lImageID = m_csClick.GetItemData(m_csClick.GetCurSel());
	if (lImageID >= 0)
		m_pcsButton->SetButtonImage(lImageID, ACUIBUTTON_MODE_CLICK);
	else
		m_pcsButton->SetButtonImage(ACUIBUTTON_INIT_INDEX, ACUIBUTTON_MODE_CLICK);

	lImageID = m_csDisable.GetItemData(m_csDisable.GetCurSel());
	if (lImageID >= 0)
		m_pcsButton->SetButtonImage(lImageID, ACUIBUTTON_MODE_DISABLE);
	else
		m_pcsButton->SetButtonImage(ACUIBUTTON_INIT_INDEX, ACUIBUTTON_MODE_DISABLE);

	// Set Flags
	m_pcsButton->m_stProperty.m_bPushButton = m_bPushButton;
	m_pcsButton->m_stProperty.m_bStartOnClickStatus	= m_bStartOnClickStatus;

	// Set Check Boolean
	m_pcsControl->m_uoData.m_stButton.m_pstCheckBoolean = (AgcdUIBoolean *) m_csCheckBoolean.GetItemDataPtr(m_csCheckBoolean.GetCurSel());
	if (m_pcsControl->m_uoData.m_stButton.m_pstCheckBoolean == (PVOID) -1)
		m_pcsControl->m_uoData.m_stButton.m_pstCheckBoolean = NULL;

	// Set Check UserData
	m_pcsControl->m_uoData.m_stButton.m_pstCheckUserData = (AgcdUIUserData *) m_csCheckUserData.GetItemDataPtr(m_csCheckUserData.GetCurSel());
	if (m_pcsControl->m_uoData.m_stButton.m_pstCheckUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stButton.m_pstCheckUserData = NULL;

	// Set Enable Boolean
	m_pcsControl->m_uoData.m_stButton.m_pstEnableBoolean = (AgcdUIBoolean *) m_csEnableBoolean.GetItemDataPtr(m_csEnableBoolean.GetCurSel());
	if (m_pcsControl->m_uoData.m_stButton.m_pstEnableBoolean == (PVOID) -1)
		m_pcsControl->m_uoData.m_stButton.m_pstEnableBoolean = NULL;

	// Set Enable UserData
	m_pcsControl->m_uoData.m_stButton.m_pstEnableUserData = (AgcdUIUserData *) m_csEnableUserData.GetItemDataPtr(m_csEnableUserData.GetCurSel());
	if (m_pcsControl->m_uoData.m_stButton.m_pstEnableUserData == (PVOID) -1)
		m_pcsControl->m_uoData.m_stButton.m_pstEnableUserData = NULL;


	m_pcsButton->m_lButtonDownStringOffsetX = m_lTextOffsetX;
	m_pcsButton->m_lButtonDownStringOffsetY = m_lTextOffsetY;
}

void UITWPButtonDialog::OnUITButtonCancel() 
{
	//UpdateDataToDialog(m_pcsButton, NULL, NULL);
}
