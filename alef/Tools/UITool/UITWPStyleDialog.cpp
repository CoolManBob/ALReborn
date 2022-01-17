// UITWPStyleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "uitool.h"
#include "UITWPStyleDialog.h"

#include "MyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// UITWPStyleDialog property page

IMPLEMENT_DYNCREATE(UITWPStyleDialog, CPropertyPage)

UITWPStyleDialog::UITWPStyleDialog() : CPropertyPage(UITWPStyleDialog::IDD)
, m_bImageClipping(FALSE)
{
	//{{AFX_DATA_INIT(UITWPStyleDialog)
	m_bUseInput = FALSE;
	m_bTopMost = FALSE;
	m_bMovable = FALSE;
	m_bVisible = FALSE;
	m_bModal = FALSE;
	m_bShrink = FALSE;
	m_bAutoAlign = FALSE;
	m_strTooltip = _T("");
	m_bUseCondensation = FALSE;
	//}}AFX_DATA_INIT

	m_pcsBase		= NULL;
	m_pcsUI			= NULL;
	m_pcsControl	= NULL;

	m_pstWinProperty	= NULL;
	m_pstBaseProperty	= NULL;
}

UITWPStyleDialog::~UITWPStyleDialog()
{
}

void UITWPStyleDialog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(UITWPStyleDialog)
	DDX_Control(pDX, IDC_UIT_TOOLTIP, m_csTooltip);
	DDX_Control(pDX, IDC_UIT_SHOW_UD, m_csShowUD);
	DDX_Control(pDX, IDC_UIT_SHOW_CB, m_csShowBoolean);
	DDX_Control(pDX, IDC_UIT_WP_ALIGN, m_csAutoAlign);
	DDX_Check(pDX, IDC_UIT_WP_USE_INPUT, m_bUseInput);
	DDX_Check(pDX, IDC_UIT_WP_TOPMOST, m_bTopMost);
	DDX_Check(pDX, IDC_UIT_WP_MOVABLE, m_bMovable);
	DDX_Check(pDX, IDC_UIT_WP_VISIBLE, m_bVisible);
	DDX_Check(pDX, IDC_UIT_WP_MODAL, m_bModal);
	DDX_Check(pDX, IDC_UIT_WP_SHRINK, m_bShrink);
	DDX_Check(pDX, IDC_UIT_WP_ALIGN, m_bAutoAlign);
	DDX_Text(pDX, IDC_UIT_TOOLTIP, m_strTooltip);
	DDX_Check(pDX, IDC_UIT_WP_CONDENSATION, m_bUseCondensation);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_UIT_WP_CLIPIMAGE, m_bImageClipping);
}


BEGIN_MESSAGE_MAP(UITWPStyleDialog, CPropertyPage)
	//{{AFX_MSG_MAP(UITWPStyleDialog)
	ON_BN_CLICKED(IDC_UIT_APPLY, OnUITApply)
	ON_BN_CLICKED(IDC_UIT_CANCEL, OnUITCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// UITWPStyleDialog message handlers

BOOL UITWPStyleDialog::OnSetActive()
{
	UpdateDataToDialog(m_pcsBase, m_pcsUI, m_pcsControl);

	return TRUE;
}

BOOL UITWPStyleDialog::UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl)
{
	m_pcsBase		= pcsBase;
	m_pcsUI			= pcsUI;
	m_pcsControl	= pcsControl;

	if (!GetSafeHwnd())
		return TRUE;

	if (m_pcsControl)
	{
		m_pstWinProperty	= &GET_CONTROL_STYLE(m_pcsControl)->m_stWinProperty	;
		m_pstBaseProperty	= &GET_CONTROL_STYLE(m_pcsControl)->m_stBaseProperty;
	}
	else if (m_pcsBase)
	{
		m_pstWinProperty	= &m_pcsBase->m_Property	;
		m_pstBaseProperty	= &m_pcsBase->m_clProperty	;
	}

	if (m_pcsBase)
	{
		m_bUseInput	= m_pstWinProperty->bUseInput	;
		m_bMovable	= m_pstWinProperty->bMovable	;
		m_bTopMost	= m_pstWinProperty->bTopmost	;
		m_bModal	= m_pstWinProperty->bModal		;
		m_bVisible	= m_pstWinProperty->bVisible	;

		m_bShrink	= m_pstBaseProperty->bShrink	;

		m_bUseCondensation	= m_pcsBase->m_bAutoFitString;
		m_bImageClipping	= m_pcsBase->m_bClipImage;
	}
	else
	{
		m_bUseInput	= FALSE	;
		m_bMovable	= FALSE	;
		m_bTopMost	= FALSE	;
		m_bModal	= FALSE	;
		m_bVisible	= FALSE	;
	}

	if (m_pcsControl)
	{
		INT32				lIndex;
		INT32				lListIndex;
		AgcdUIUserData *	pstUserData;
		AgcdUIBoolean *		pstBoolean;
		static CHAR *		szNullString = "<NULL>";

		m_csAutoAlign.EnableWindow(TRUE);
		m_csShowBoolean.EnableWindow(TRUE);
		m_csShowUD.EnableWindow(TRUE);
		m_csTooltip.EnableWindow(TRUE);

		m_csShowBoolean.ResetContent();

		lListIndex = m_csShowBoolean.AddString(szNullString);
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csShowBoolean.SetItemDataPtr(lListIndex, NULL);

		for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetBooleanCount(); ++lIndex)
		{
			pstBoolean = g_pcsAgcmUIManager2->GetBoolean(lIndex);
			if (pstBoolean)
			{
				lListIndex = m_csShowBoolean.AddString(pstBoolean->m_szName);
				if (lListIndex == CB_ERR)
					return FALSE;

				m_csShowBoolean.SetItemDataPtr(lListIndex, pstBoolean);
			}
		}

		if (m_pcsControl->m_pstShowCB)
			m_csShowBoolean.SelectString(-1, m_pcsControl->m_pstShowCB->m_szName);
		else
			m_csShowBoolean.SelectString(-1, szNullString);

		m_csShowUD.ResetContent();

		lListIndex = m_csShowUD.AddString(szNullString);
		if (lListIndex == CB_ERR)
			return FALSE;
		m_csShowUD.SetItemDataPtr(lListIndex, NULL);

		for (lIndex = 0; lIndex <= g_pcsAgcmUIManager2->GetUserDataCount(); ++lIndex)
		{
			pstUserData = g_pcsAgcmUIManager2->GetUserData(lIndex);
			if (pstUserData)
			{
				lListIndex = m_csShowUD.AddString(pstUserData->m_szName);
				if (lListIndex == CB_ERR)
					return FALSE;

				m_csShowUD.SetItemDataPtr(lListIndex, pstUserData);
			}
		}

		if (m_pcsControl->m_pstShowUD)
			m_csShowUD.SelectString(-1, m_pcsControl->m_pstShowUD->m_szName);
		else
			m_csShowUD.SelectString(-1, szNullString);

		m_bAutoAlign		= m_pcsControl->m_bAutoAlign;

		if (m_pcsControl->m_szTooltip)
			m_strTooltip = m_pcsControl->m_szTooltip;
		else
			m_strTooltip = "";
	}
	else
	{
		m_csAutoAlign.EnableWindow(FALSE);
		m_csShowBoolean.EnableWindow(FALSE);
		m_csShowUD.EnableWindow(FALSE);
		m_csTooltip.EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	return TRUE;
}

BOOL UITWPStyleDialog::UpdateDataFromDialog()
{
	UpdateData();

	if (!GetSafeHwnd())
		return TRUE;

	if (m_pcsBase)
	{
		m_pstWinProperty->bUseInput	= m_bUseInput	;
		m_pstWinProperty->bMovable	= m_bMovable	;
		m_pstWinProperty->bTopmost	= m_bTopMost	;
		m_pstWinProperty->bModal	= m_bModal		;
		m_pstWinProperty->bVisible	= m_bVisible	;

		m_pstBaseProperty->bShrink	= m_bShrink		;

		m_pcsBase->m_bAutoFitString	= m_bUseCondensation;
		m_pcsBase->m_bClipImage		= m_bImageClipping;
	}

	if (m_pcsControl)
	{
		m_pcsControl->m_bAutoAlign	= m_bAutoAlign;

		m_pcsControl->m_pstShowCB = (AgcdUIBoolean *) m_csShowBoolean.GetItemDataPtr(m_csShowBoolean.GetCurSel());
		if (m_pcsControl->m_pstShowCB == (PVOID) -1)
			m_pcsControl->m_pstShowCB = NULL;

		m_pcsControl->m_pstShowUD = (AgcdUIUserData *) m_csShowUD.GetItemDataPtr(m_csShowUD.GetCurSel());
		if (m_pcsControl->m_pstShowUD == (PVOID) -1)
			m_pcsControl->m_pstShowUD = NULL;

		if (m_strTooltip.GetLength())
			g_pcsAgcmUIManager2->SetControlTooltip(m_pcsControl, (LPSTR) (LPCTSTR) m_strTooltip);
	}

	return TRUE;
}

BOOL UITWPStyleDialog::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UITWPStyleDialog::OnUITApply() 
{
	UpdateDataFromDialog();
}

void UITWPStyleDialog::OnUITCancel() 
{
	UpdateDataToDialog(m_pcsBase, m_pcsUI, m_pcsControl);
}
