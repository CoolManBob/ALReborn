// UITWPEditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPEditDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPEditDialog property page

IMPLEMENT_DYNCREATE(UITWPEditDialog, UITPropertyPage)

UITWPEditDialog::UITWPEditDialog() : UITPropertyPage()
, m_bFilterCase(FALSE)
, m_bReleaseEditInputEnd(TRUE)
{
	//{{AFX_DATA_INIT(UITWPEditDialog)
	m_bPassword = FALSE;
	m_ulLength = 0;
	m_bMultiLine = FALSE;
	m_bReadOnly = FALSE;
	m_bAutoLF = FALSE;
	m_bUseTag = FALSE;
	m_bForHotkey = FALSE;
	//}}AFX_DATA_INIT

	Construct(UITWPEditDialog::IDD);

	m_pcsEdit		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;
}

UITWPEditDialog::~UITWPEditDialog()
{
}

void UITWPEditDialog::DoDataExchange(CDataExchange* pDX)
{
	UITPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPEditDialog)
	DDX_Check(pDX, IDC_UIT_EDIT_PASSWORD, m_bPassword);
	DDX_Text(pDX, IDC_UIT_EDIT_LENGTH, m_ulLength);
	DDX_Check(pDX, IDC_UIT_EDIT_MULTILINE, m_bMultiLine);
	DDX_Check(pDX, IDC_UIT_EDIT_READONLY, m_bReadOnly);
	DDX_Check(pDX, IDC_UIT_EDIT_AUTOLF, m_bAutoLF);
	DDX_Check(pDX, IDC_UIT_EDIT_ENABLE_TAG, m_bUseTag);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_UIT_EDIT_FILTER_CASE, m_bFilterCase);
	DDX_Check(pDX, IDC_UIT_EDIT_RELEASE_INPUTEND, m_bReleaseEditInputEnd);
	DDX_Check(pDX, IDC_UIT_EDIT_HOTKEY, m_bForHotkey);
}


BEGIN_MESSAGE_MAP(UITWPEditDialog, UITPropertyPage)
	//{{AFX_MSG_MAP(UITWPEditDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPEditDialog message handlers

void UITWPEditDialog::OnCancel() 
{
	UpdateDataToDialog(m_pcsEdit, m_pcsUI, m_pcsControl);

	//UITPropertyPage::OnCancel();
}

void UITWPEditDialog::OnOK() 
{
	UpdateDataFromDialog();

	//UITPropertyPage::OnOK();
}

BOOL UITWPEditDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsEdit, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPEditDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsEdit		= (AcUIEdit *) pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd() || !m_pcsEdit || !m_pcsControl)
		return TRUE;

	m_bPassword		= m_pcsEdit->m_bPasswordEdit;
	m_bMultiLine	= m_pcsEdit->m_bMultiLine;
	m_bReadOnly		= m_pcsEdit->m_bReadOnly;
	m_bAutoLF		= m_pcsEdit->m_bAutoLF;
	m_bUseTag		= m_pcsEdit->m_bEnableTag;
	m_bFilterCase	= m_pcsEdit->m_bFilterCase;
	m_bReleaseEditInputEnd	= m_pcsEdit->m_bReleaseEditInputEnd;
	m_ulLength		= m_pcsEdit->GetTextMaxLength();
	m_bForHotkey	= m_pcsEdit->m_bForHotkey;

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPEditDialog::UpdateDataFromDialog()
{
	if (!GetSafeHwnd())
		return TRUE;

	if (!m_pcsEdit || !m_pcsControl)
		return TRUE;

	UpdateData();

	m_pcsEdit->m_bPasswordEdit	= m_bPassword;
	m_pcsEdit->m_bMultiLine		= m_bMultiLine;
	m_pcsEdit->m_bReadOnly		= m_bReadOnly;
	m_pcsEdit->m_bAutoLF		= m_bAutoLF;
	m_pcsEdit->m_bEnableTag		= m_bUseTag;
	m_pcsEdit->m_bFilterCase	= m_bFilterCase;
	m_pcsEdit->m_bReleaseEditInputEnd	= m_bReleaseEditInputEnd;
	m_pcsEdit->m_bForHotkey		= m_bForHotkey;

	m_pcsEdit->SetTextMaxLength(m_ulLength);

	return TRUE;
}

BOOL UITWPEditDialog::OnInitDialog() 
{
	UITPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
