// XTPCustomizeOptionsPage.cpp : implementation of the CXTPCustomizeOptionsPage class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"

#include "XTPCustomizeOptionsPage.h"
#include "XTPCustomizeSheet.h"
#include "XTPToolBar.h"
#include "XTPCommandBars.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CXTPGroupLine

CXTPGroupLine::CXTPGroupLine()
{
	m_themeButton.OpenThemeData(0, L"BUTTON");
}

BEGIN_MESSAGE_MAP(CXTPGroupLine, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CXTPGroupLine::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	COLORREF clrText = GetSysColor(COLOR_BTNTEXT);

	if (m_themeButton.IsAppThemeReady() &&
		FAILED(m_themeButton.GetThemeColor(BP_GROUPBOX, GBS_NORMAL, TMT_TEXTCOLOR, &clrText)))
	{
		clrText = GetSysColor(COLOR_BTNTEXT);
	}

	BOOL bRightToLeft = GetExStyle() & WS_EX_RTLREADING;

	CString strText;
	GetWindowText(strText);

	CXTPClientRect rc(this);

	CXTPFontDC font(&dc, GetFont());
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(clrText);

	dc.DrawText(strText, rc, DT_SINGLELINE | DT_VCENTER |
		(bRightToLeft ? DT_RIGHT | DT_RTLREADING: DT_LEFT));

	CRect rcLine(0, rc.top, 0, rc.bottom);
	dc.DrawText(strText, rcLine, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT);

	if (bRightToLeft)
		rcLine.SetRect(rc.left, rc.top, rc.right -  + rcLine.Width() - 6, rc.bottom);
	else
		rcLine.SetRect(rc.left + rcLine.Width() + 6, rc.top, rc.right, rc.bottom);

	COLORREF clrLine = GetSysColor(COLOR_3DSHADOW);
	if (m_themeButton.IsAppThemeReady() &&
		SUCCEEDED(m_themeButton.GetThemeColor(BP_GROUPBOX, GBS_NORMAL, TMT_BORDERCOLORHINT, &clrLine)))
	{
		dc.FillSolidRect(rcLine.left, rcLine.CenterPoint().y, rcLine.Width(), 1, clrLine);
	}
	else
	{
		dc.FillSolidRect(rcLine.left, rcLine.CenterPoint().y, rcLine.Width(), 1, GetSysColor(COLOR_3DSHADOW));
		dc.FillSolidRect(rcLine.left, rcLine.CenterPoint().y + 1, rcLine.Width(), 1, GetSysColor(COLOR_3DHIGHLIGHT));
	}
}


/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeOptionsPage property page


CXTPCustomizeOptionsPage::CXTPCustomizeOptionsPage(CXTPCustomizeSheet* pSheet) : CPropertyPage(CXTPCustomizeOptionsPage::IDD)
{
	CXTPCommandBars* pCommandBars = pSheet->GetCommandBars();
	CXTPCommandBarsOptions* pOptions = pCommandBars->GetCommandBarsOptions();

	m_bAlwaysShowFullMenus = pOptions->bAlwaysShowFullMenus;
	m_bShowFullAfterDelay = pOptions->bShowFullAfterDelay;
	m_bLargeIcons = pOptions->bLargeIcons;
	m_bToolBarScreenTips = pOptions->bToolBarScreenTips;
	m_bToolBarAccelTips = pOptions->bToolBarAccelTips;

	m_nAnimationType = pOptions->animationType;

	//{{AFX_DATA_INIT(CXTPCustomizeOptionsPage)
	//}}AFX_DATA_INIT
	m_pSheet = pSheet;

	//RevertResourceContext();
}

CXTPCustomizeOptionsPage::~CXTPCustomizeOptionsPage()
{
}

CXTPCommandBars* CXTPCustomizeOptionsPage::GetCommandBars() const
{
	ASSERT(m_pSheet && m_pSheet->GetCommandBars());
	return m_pSheet->GetCommandBars();
}

void CXTPCustomizeOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, XTP_IDC_COMBO_ANIMATION, m_comboAnimationType);
	DDX_CBIndex(pDX, XTP_IDC_COMBO_ANIMATION, m_nAnimationType);

	//{{AFX_DATA_MAP(CXTPCustomizeOptionsPage)
	DDX_Check(pDX, XTP_IDC_CHECK_FULL_MENUS, m_bAlwaysShowFullMenus);
	DDX_Check(pDX, XTP_IDC_CHECK_AFTERDELAY, m_bShowFullAfterDelay);
	DDX_Check(pDX, XTP_IDC_CHECK_LARGEICONS, m_bLargeIcons);
	DDX_Check(pDX, XTP_IDC_CHECK_SCREENTTIPS, m_bToolBarScreenTips);
	DDX_Check(pDX, XTP_IDC_CHECK_SHORTCUTS, m_bToolBarAccelTips);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXTPCustomizeOptionsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPCustomizeOptionsPage)
	ON_BN_CLICKED(XTP_IDC_CHECK_FULL_MENUS, OnCheckFullMenus)
	ON_BN_CLICKED(XTP_IDC_CHECK_AFTERDELAY, OnCheckAfterdelay)
	ON_BN_CLICKED(XTP_IDC_CHECK_LARGEICONS, OnCheckLargeicons)
	ON_BN_CLICKED(XTP_IDC_CHECK_SCREENTTIPS, OnCheckScreenttips)
	ON_BN_CLICKED(XTP_IDC_CHECK_SHORTCUTS, OnCheckShortcuts)
	ON_BN_CLICKED(XTP_IDC_BUTTON_RESETDATA, OnResetData)
	ON_CBN_SELCHANGE(XTP_IDC_COMBO_ANIMATION, OnAnimationChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCustomizeOptionsPage message handlers

void CXTPCustomizeOptionsPage::AddComboString(UINT nIDResource)
{
	CString str;
	XTPResourceManager()->LoadString(&str, nIDResource);

	m_comboAnimationType.AddString(str);
}


BOOL CXTPCustomizeOptionsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	GetDlgItem(XTP_IDC_CHECK_AFTERDELAY)->EnableWindow(!m_bAlwaysShowFullMenus);
	GetDlgItem(XTP_IDC_CHECK_SHORTCUTS)->EnableWindow(m_bToolBarScreenTips);

	if (GetDlgItem(XTP_IDC_GROUP_PERSONALIZED))
		m_wndPersonalizedGroup.SubclassDlgItem(XTP_IDC_GROUP_PERSONALIZED, this);

	if (GetDlgItem(XTP_IDC_GROUP_OTHER))
		m_wndOtherGroup.SubclassDlgItem(XTP_IDC_GROUP_OTHER, this);

	// Add strings to animation combo box.
	AddComboString(XTP_IDS_ANIMATION_SYSDEF);
	AddComboString(XTP_IDS_ANIMATION_RANDOM);
	AddComboString(XTP_IDS_ANIMATION_UNFOLD);
	AddComboString(XTP_IDS_ANIMATION_SLIDE);
	AddComboString(XTP_IDS_ANIMATION_FADE);
	AddComboString(XTP_IDS_ANIMATION_NONE);

	// Ensure combo selection is made.
	UpdateData(FALSE);


	return TRUE;
}

void CXTPCustomizeOptionsPage::OnCheckFullMenus()
{
	UpdateData();
	GetDlgItem(XTP_IDC_CHECK_AFTERDELAY)->EnableWindow(!m_bAlwaysShowFullMenus);

	GetCommandBars()->GetCommandBarsOptions()->bAlwaysShowFullMenus = m_bAlwaysShowFullMenus;
}


void CXTPCustomizeOptionsPage::OnAnimationChanged()
{
	UpdateData();
	if (m_nAnimationType != CB_ERR)
	{
		GetCommandBars()->GetCommandBarsOptions()->animationType = (XTPAnimationType)m_nAnimationType;
	}
}

void CXTPCustomizeOptionsPage::OnCheckAfterdelay()
{
	UpdateData();
	GetCommandBars()->GetCommandBarsOptions()->bShowFullAfterDelay = m_bShowFullAfterDelay;
}

void CXTPCustomizeOptionsPage::OnCheckLargeicons()
{
	UpdateData();
	CXTPCommandBars* pCommandBars = m_pSheet->GetCommandBars();
	pCommandBars->GetCommandBarsOptions()->bLargeIcons = m_bLargeIcons;

	pCommandBars->RedrawCommandBars();
}

void CXTPCustomizeOptionsPage::OnCheckScreenttips()
{
	UpdateData();
	GetDlgItem(XTP_IDC_CHECK_SHORTCUTS)->EnableWindow(m_bToolBarScreenTips);
	GetCommandBars()->GetCommandBarsOptions()->bToolBarScreenTips = m_bToolBarScreenTips;

}

void CXTPCustomizeOptionsPage::OnCheckShortcuts()
{
	UpdateData();
	GetCommandBars()->GetCommandBarsOptions()->bToolBarAccelTips = m_bToolBarAccelTips;
}

void CXTPCustomizeOptionsPage::OnResetData()
{
	CString strPrompt;
	VERIFY(XTPResourceManager()->LoadString(&strPrompt, XTP_IDS_CONFIRM_RESETDATA));

	if (m_pSheet->MessageBox(strPrompt, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES)
	{
		GetCommandBars()->ResetUsageData();
	}
}
