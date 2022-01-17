// XTPSyntaxEditPropertiesPage.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
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

// common includes
#include "Common/XTPColorManager.h"
#include "Common/XTPImageManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPVC50Helpers.h"
#include "Common/XTPResourceManager.h"
#include "Common/XTPDrawHelpers.h"

// syntax editor includes
#include "XTPSyntaxEditDefines.h"
#include "XTPSyntaxEditStruct.h"
#include "XTPSyntaxEditUndoManager.h"
#include "XTPSyntaxEditLineMarksManager.h"
#include "XTPSyntaxEditLexPtrs.h"
#include "XTPSyntaxEditLexClassSubObjT.h"
#include "XTPSyntaxEditTextIterator.h"
#include "XTPSyntaxEditSectionManager.h"
#include "XTPSyntaxEditLexCfgFileReader.h"
#include "XTPSyntaxEditLexClassSubObjDef.h"
#include "XTPSyntaxEditLexClass.h"
#include "XTPSyntaxEditLexParser.h"
#include "XTPSyntaxEditLexColorFileReader.h"
#include "XTPSyntaxEditBufferManager.h"
#include "XTPSyntaxEditToolTipCtrl.h"
#include "XTPSyntaxEditAutoCompleteWnd.h"
#include "XTPSyntaxEditFindReplaceDlg.h"
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditDoc.h"
#include "XTPSyntaxEditView.h"
#include "XTPSyntaxEditPaintManager.h"
#include "XTPSyntaxEditColorSampleText.h"
#include "XTPSyntaxEditColorComboBox.h"
#include "XTPSyntaxEditPropertiesPage.h"
#include "XTPSyntaxEditPropertiesDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// EnumFontFamExProc
//===========================================================================

AFX_STATIC int CALLBACK EnumFontFamExProc(
	ENUMLOGFONTEX *lpelfe,    // logical-font data
	NEWTEXTMETRICEX *lpntme,  // physical-font data
	DWORD FontType,           // type of font
	LPARAM lParam)            // application-defined data
{
	CComboBox* pWndCombo = DYNAMIC_DOWNCAST(CComboBox, (CComboBox*)lParam);
	if (::IsWindow(pWndCombo->GetSafeHwnd()))
	{
		switch (pWndCombo->GetDlgCtrlID())
		{
		case XTP_IDC_EDIT_COMB_NAMES:
			{
				if (lpelfe->elfLogFont.lfPitchAndFamily & FIXED_PITCH)
				{
					// Make sure the fonts are only added once.
					if (pWndCombo->FindStringExact(0, (LPCTSTR)lpelfe->elfFullName) == CB_ERR)
					{
						// Add to list
						pWndCombo->AddString((LPCTSTR)lpelfe->elfLogFont.lfFaceName);
					}
				}
			}
			break;
		case XTP_IDC_EDIT_COMB_SIZES:
			{
				if (FontType != TRUETYPE_FONTTYPE)
				{
					CWindowDC dc(NULL);
					CString csSize;
					csSize.Format(_T("%i"),::MulDiv(lpntme->ntmTm.tmHeight - lpntme->ntmTm.tmInternalLeading,
						72, ::GetDeviceCaps(dc.m_hDC, LOGPIXELSY)));

					// Make sure the fonts are only added once.
					if (pWndCombo->FindStringExact(0, (LPCTSTR)csSize) == CB_ERR)
					{
						// Add to list
						pWndCombo->AddString((LPCTSTR)csSize);
					}
				}
			}
			break;
		case XTP_IDC_EDIT_COMB_SCRIPT:
			{
				if (lpelfe->elfScript[0] != _T('\0'))
				{
					// Make sure the fonts are only added once.
					if (pWndCombo->FindStringExact(0, (LPCTSTR)lpelfe->elfScript) == CB_ERR)
					{
						// Add to list
						int iIndex = pWndCombo->AddString((LPCTSTR)lpelfe->elfScript);
						if (iIndex != CB_ERR)
						{
							pWndCombo->SetItemData(iIndex,
								(DWORD)lpelfe->elfLogFont.lfCharSet);
						}
					}
				}
			}
			break;
		}
	}
	return TRUE;
}

namespace XTPSyntaxEditLexAnalyser
{
	extern CString MakeStr(const CStringArray& rArProps, LPCTSTR strSplitter);
	extern BOOL PropPathSplit(LPCTSTR pcszPropPath, CStringArray& rArProps);
}

using namespace XTPSyntaxEditLexAnalyser;

//===========================================================================
// CXTPSyntaxEditPropertiesPageEdit property page
//===========================================================================

CXTPSyntaxEditPropertiesPageEdit::CXTPSyntaxEditPropertiesPageEdit(CXTPSyntaxEditView* pEditView/*=NULL*/)
: //CPropertyPage(CXTPSyntaxEditPropertiesPageEdit::IDD),
m_bModified(FALSE)
, m_pEditView(pEditView)
{
	ASSERT_VALID(m_pEditView); // must be valid.

	m_psp.dwFlags |= PSP_DLGINDIRECT;
	m_psp.pResource = XTPResourceManager()->LoadDialogTemplate(IDD);

	//{{AFX_DATA_INIT(CXTPSyntaxEditPropertiesPageEdit)
	m_bAutoReload = TRUE;
	m_bHorzScrollBar = TRUE;
	m_bVertScrollBar = TRUE;
	m_bSyntaxColor = TRUE;
	m_bAutoIndent = TRUE;
	m_bSelMargin = TRUE;
	m_bLineNumbers = TRUE;
	m_nCaretStyle = 0;
	m_nTabSize = 4;
	m_nTabType = 1;
	//}}AFX_DATA_INIT

	ReadRegistryValues();
}

CXTPSyntaxEditPropertiesPageEdit::~CXTPSyntaxEditPropertiesPageEdit()
{
}

void CXTPSyntaxEditPropertiesPageEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPSyntaxEditPropertiesPageEdit)
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_AUTO_RELOAD, m_bAutoReload);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_HORZ_SCROLLBAR, m_bHorzScrollBar);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_VERT_SCROLLBAR, m_bVertScrollBar);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_SYNTAX_COLOR, m_bSyntaxColor);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_AUTO_INDENT, m_bAutoIndent);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_SELMARGIN, m_bSelMargin);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_LINE_NUMBERS, m_bLineNumbers);
	DDX_Control(pDX, XTP_IDC_EDIT_RAD_TABS_SPACES, m_btnRadioSpaces);
	DDX_Control(pDX, XTP_IDC_EDIT_RAD_TABS_TAB, m_btnRadioTab);
	DDX_Control(pDX, XTP_IDC_EDIT_RAD_CARET_THIN, m_btnRadioCaretThin);
	DDX_Control(pDX, XTP_IDC_EDIT_RAD_CARET_THICK, m_btnRadioCaretThick);
	DDX_Text(pDX, XTP_IDC_EDIT_TABS_SIZE, m_nTabSize);
	DDV_MinMaxInt(pDX, m_nTabSize, 1, 64);
	//}}AFX_DATA_MAP
}

IMPLEMENT_DYNCREATE(CXTPSyntaxEditPropertiesPageEdit, CPropertyPage)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditPropertiesPageEdit, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPSyntaxEditPropertiesPageEdit)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_AUTO_RELOAD, OnChkAutoReload)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_HORZ_SCROLLBAR, OnChkHorzScrollBar)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_VERT_SCROLLBAR, OnChkVertScrollBar)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_SYNTAX_COLOR, OnChkSyntaxColor)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_AUTO_INDENT, OnChkAutoIndent)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_SELMARGIN, OnChkSelMargin)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_LINE_NUMBERS, OnChkLineNumbers)
	ON_BN_CLICKED(XTP_IDC_EDIT_RAD_TABS_SPACES, OnTabsSpaces)
	ON_BN_CLICKED(XTP_IDC_EDIT_RAD_TABS_TAB, OnTabsTab)
	ON_BN_CLICKED(XTP_IDC_EDIT_RAD_CARET_THIN, OnCaretThin)
	ON_BN_CLICKED(XTP_IDC_EDIT_RAD_CARET_THICK, OnCaretThick)
	ON_EN_CHANGE(XTP_IDC_EDIT_TABS_SIZE, OnChangeTabsSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTPSyntaxEditPropertiesPageEdit::ReadRegistryValues()
{
	if (!::IsWindow(m_pEditView->GetSafeHwnd()))
		return FALSE;

	m_bVertScrollBar    = m_pEditView->GetVertScrollBar();
	m_bHorzScrollBar    = m_pEditView->GetHorzScrollBar();

	CXTPSyntaxEditDoc* pEditDoc = m_pEditView->GetDocument();
	if (pEditDoc)
	{
		m_bAutoReload = pEditDoc->GetAutoReload();
	}

	CXTPSyntaxEditCtrl& wndEditCtrl = m_pEditView->GetEditCtrl();
	if (::IsWindow(wndEditCtrl.m_hWnd))
	{
		m_bSyntaxColor  = wndEditCtrl.GetSyntaxColor();
		m_bAutoIndent   = wndEditCtrl.GetAutoIndent();
		m_bSelMargin    = wndEditCtrl.GetSelMargin();
		m_bLineNumbers  = wndEditCtrl.GetLineNumbers();
		m_nCaretStyle   = wndEditCtrl.GetWideCaret();
		m_nTabSize      = wndEditCtrl.GetTabSize();
		m_nTabType      = wndEditCtrl.GetTabWithSpace()? 0: 1;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditPropertiesPageEdit::WriteRegistryValues()
{
	if (!::IsWindow(m_pEditView->GetSafeHwnd()))
		return FALSE;

	m_pEditView->SetScrollBars(m_bHorzScrollBar, m_bVertScrollBar, TRUE);

	CXTPSyntaxEditDoc* pEditDoc = m_pEditView->GetDocument();
	if (pEditDoc)
	{
		pEditDoc->SetAutoReload(m_bAutoReload, TRUE);
	}

	CXTPSyntaxEditCtrl& wndEditCtrl = m_pEditView->GetEditCtrl();
	if (::IsWindow(wndEditCtrl.m_hWnd))
	{
		wndEditCtrl.SetSyntaxColor(m_bSyntaxColor, TRUE);
		wndEditCtrl.SetAutoIndent(m_bAutoIndent, TRUE);
		wndEditCtrl.SetSelMargin(m_bSelMargin, TRUE);
		wndEditCtrl.SetLineNumbers(m_bLineNumbers, TRUE);
		wndEditCtrl.SetWideCaret(m_nCaretStyle, TRUE);
		wndEditCtrl.SetTabSize(m_nTabSize, TRUE);
		wndEditCtrl.SetTabWithSpace(m_nTabType? FALSE: TRUE, TRUE);
	}

	wndEditCtrl.RestoreCursor();

	return TRUE;
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkAutoReload()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkHorzScrollBar()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkVertScrollBar()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkSyntaxColor()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkAutoIndent()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkSelMargin()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChkLineNumbers()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnChangeTabsSize()
{
	UpdateData();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnTabsSpaces()
{
	m_nTabType = 0;
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnTabsTab()
{
	m_nTabType = 1;
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnCaretThin()
{
	m_nCaretStyle = 0;
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageEdit::OnCaretThick()
{
	m_nCaretStyle = 1;
	SetModified(TRUE);
}

BOOL CXTPSyntaxEditPropertiesPageEdit::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_btnRadioSpaces.SetCheck(m_nTabType == 0);
	m_btnRadioTab.SetCheck(m_nTabType == 1);
	m_btnRadioCaretThin.SetCheck(m_nCaretStyle == 0);
	m_btnRadioCaretThick.SetCheck(m_nCaretStyle == 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CXTPSyntaxEditPropertiesPageEdit::OnApply()
{
	if (!CPropertyPage::OnApply())
		return FALSE;

	// no changes made or they have already been applied.
	if (!m_bModified)
		return TRUE;

	// update registry settings.
	if (!WriteRegistryValues())
		return FALSE;

	m_bModified = FALSE;
	return TRUE;
}

void CXTPSyntaxEditPropertiesPageEdit::SetModified(BOOL bChanged/*=TRUE*/)
{
	CPropertyPage::SetModified();
	m_bModified = bChanged;
}

//===========================================================================
// CXTPSyntaxEditPropertiesPageFont property page
//===========================================================================

CXTPSyntaxEditPropertiesPageFont::CXTPSyntaxEditPropertiesPageFont(CXTPSyntaxEditView* pEditView/*=NULL*/)
: //CPropertyPage(CXTPSyntaxEditPropertiesPageFont::IDD),
m_bModified(FALSE)
, m_pEditView(pEditView)
, m_uFaceSize(LF_FACESIZE)
{
	m_psp.dwFlags |= PSP_DLGINDIRECT;
	m_psp.pResource = XTPResourceManager()->LoadDialogTemplate(IDD);

	//{{AFX_DATA_INIT(CXTPSyntaxEditPropertiesPageFont)
	m_bStrikeOut = FALSE;
	m_bUnderline = FALSE;
	m_csName = _T("Courier");
	m_csStyle = _T("Regular");
	m_csSize = _T("10");
	//}}AFX_DATA_INIT

	m_iCharSet = DEFAULT_CHARSET;
	m_crHiliteText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crHiliteBack = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crBack = ::GetSysColor(COLOR_WINDOW);

	ReadRegistryValues();
}

CXTPSyntaxEditPropertiesPageFont::~CXTPSyntaxEditPropertiesPageFont()
{
}

void CXTPSyntaxEditPropertiesPageFont::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPSyntaxEditPropertiesPageFont)
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_SCRIPT, m_wndComboScript);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_STYLES, m_wndComboStyle);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_SIZES, m_wndComboSize);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_NAMES, m_wndComboName);
/*  DDX_Check(pDX, XTP_IDC_EDIT_CHK_STRIKEOUT, m_bStrikeOut);*/
/*  DDX_Check(pDX, XTP_IDC_EDIT_CHK_UNDERLINE, m_bUnderline);*/
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_TEXT, m_wndComboText);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_BACK, m_wndComboBack);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_TEXTSEL, m_wndComboHiliteText);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_BACKSEL, m_wndComboHiliteBack);
	DDX_Control(pDX, XTP_IDC_EDIT_TXT_SAMPLE, m_txtSample);
	DDX_Control(pDX, XTP_IDC_EDIT_TXT_SAMPLESEL, m_txtSampleSel);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_TEXT, m_crText);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_BACK, m_crBack);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_TEXTSEL, m_crHiliteText);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_BACKSEL, m_crHiliteBack);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_TEXT, m_btnCustomText);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_BACK, m_btnCustomBack);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_TEXTSEL, m_btnCustomHiliteText);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_BACKSEL, m_btnCustomHiliteBack);
	DDX_CBString(pDX, XTP_IDC_EDIT_COMB_STYLES, m_csStyle);
	DDX_CBString(pDX, XTP_IDC_EDIT_COMB_NAMES, m_csName);
	DDX_CBString(pDX, XTP_IDC_EDIT_COMB_SIZES, m_csSize);
	DDV_MaxChars(pDX, m_csSize, m_uFaceSize);
	//}}AFX_DATA_MAP
}

IMPLEMENT_DYNCREATE(CXTPSyntaxEditPropertiesPageFont, CPropertyPage)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditPropertiesPageFont, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPSyntaxEditPropertiesPageFont)
	ON_CBN_SELCHANGE(XTP_IDC_EDIT_COMB_NAMES, OnSelChangeComboNames)
	ON_CBN_SELCHANGE(XTP_IDC_EDIT_COMB_STYLES, OnSelChangeComboStyles)
	ON_CBN_SELCHANGE(XTP_IDC_EDIT_COMB_SIZES, OnSelChangeComboSizes)
/*  ON_BN_CLICKED(XTP_IDC_EDIT_CHK_STRIKEOUT, OnChkStrikeOut)*/
/*  ON_BN_CLICKED(XTP_IDC_EDIT_CHK_UNDERLINE, OnChkUnderline)*/
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_TEXT, OnBtnCustomText)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_BACK, OnBtnCustomBack)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_TEXTSEL, OnBtnCustomHiliteText)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_BACKSEL, OnBtnCustomtHiliteBack)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_TEXTSEL, OnSelEndOkHiliteText)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_BACKSEL, OnSelEndOkHiliteBack)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_TEXT, OnSelEndOkText)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_BACK, OnSelEndOkBack)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_SCRIPT, OnSelEndOkScript)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTPSyntaxEditPropertiesPageFont::CreateSafeFontIndirect(CFont& editFont, const LOGFONT& lf)
{
	if (editFont.GetSafeHandle())
		editFont.DeleteObject();

	if (!editFont.CreateFontIndirect(&lf))
		return FALSE;

	return TRUE;
}

BOOL CXTPSyntaxEditPropertiesPageFont::GetSafeLogFont(LOGFONT& lf)
{
	if (m_editFont.GetSafeHandle())
	{
		m_editFont.GetLogFont(&lf);
		return TRUE;
	}

	CFont* pFont = XTPSyntaxEditPaintManager()->GetFont();
	if (pFont->GetSafeHandle())
	{
		pFont->GetLogFont(&lf);
		return TRUE;
	}

	return FALSE;
}

BOOL CXTPSyntaxEditPropertiesPageFont::ReadRegistryValues()
{
	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return FALSE;

	if (!CreateSafeFontIndirect(m_editFont, lf))
		return FALSE;

	m_crText        = XTPSyntaxEditPaintManager()->GetTextColor();
	m_crBack        = XTPSyntaxEditPaintManager()->GetBackColor();
	m_crHiliteText  = XTPSyntaxEditPaintManager()->GetHiliteTextColor();
	m_crHiliteBack  = XTPSyntaxEditPaintManager()->GetHiliteBackColor();
	m_bStrikeOut    = (BOOL)lf.lfStrikeOut;
	m_bUnderline    = (BOOL)lf.lfUnderline;

	return TRUE;
}

BOOL CXTPSyntaxEditPropertiesPageFont::WriteRegistryValues()
{
	// update paint manager font(paint manager font is set with CXTPSyntaxEditCtrl::CreateFontIndirect).
	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return FALSE;

	m_pEditView->SetFontIndirect(&lf, TRUE);

	// commit values to registry.
	XTPSyntaxEditPaintManager()->SetTextColor(m_crText, TRUE);
	XTPSyntaxEditPaintManager()->SetBackColor(m_crBack, TRUE);
	XTPSyntaxEditPaintManager()->SetHiliteTextColor(m_crHiliteText, TRUE);
	XTPSyntaxEditPaintManager()->SetHiliteBackColor(m_crHiliteBack, TRUE);

	return TRUE;
}

BOOL CXTPSyntaxEditPropertiesPageFont::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Initialize combo boxes.
	InitFontCombo();
	InitStyleCombo();
	InitSizeCombo();
	InitScriptCombo();
	InitColorComboxes();

	UpdateSampleFont();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXTPSyntaxEditPropertiesPageFont::InitFontCombo()
{
	m_wndComboName.ResetContent();

	LOGFONT lfEnum = {0};
	lfEnum.lfCharSet = DEFAULT_CHARSET;
	lfEnum.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
	lfEnum.lfFaceName[0] = 0;

	CWindowDC dc(NULL);
	::EnumFontFamiliesEx(dc.m_hDC, &lfEnum,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_wndComboName, 0 );

	if (!m_editFont.GetSafeHandle())
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	m_csName = lf.lfFaceName;

	if (m_wndComboName.SelectString(-1, m_csName) == CB_ERR)
		m_wndComboName.SetWindowText(m_csName);
}

void CXTPSyntaxEditPropertiesPageFont::InitStyleCombo()
{
	m_wndComboStyle.ResetContent();

	CString csBuffer;
	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_REGULAR);
	m_wndComboStyle.AddString(csBuffer);

	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_BOLD);
	m_wndComboStyle.AddString(csBuffer);

	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_ITALIC);
	m_wndComboStyle.AddString(csBuffer);

	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_BOLDITALIC);
	m_wndComboStyle.AddString(csBuffer);

	if (!m_editFont.GetSafeHandle())
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	if (lf.lfWeight >= FW_BOLD && lf.lfItalic)
	{
		XTPResourceManager()->LoadString(&m_csStyle, XTP_IDS_EDIT_BOLDITALIC);
	}
	else if (lf.lfItalic)
	{
		XTPResourceManager()->LoadString(&m_csStyle, XTP_IDS_EDIT_ITALIC);
	}
	else if (lf.lfWeight >= FW_BOLD)
	{
		XTPResourceManager()->LoadString(&m_csStyle, XTP_IDS_EDIT_BOLD);
	}
	else
	{
		XTPResourceManager()->LoadString(&m_csStyle, XTP_IDS_EDIT_REGULAR);
	}

	if (m_wndComboStyle.SelectString(-1, m_csStyle) == CB_ERR)
		m_wndComboStyle.SetWindowText(m_csStyle);
}

void CXTPSyntaxEditPropertiesPageFont::InitSizeCombo()
{
	m_wndComboSize.ResetContent();

	if (!m_editFont.GetSafeHandle())
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	CWindowDC dc(NULL);
	::EnumFontFamiliesEx(dc.m_hDC, &lf,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_wndComboSize, 0);

	if (m_wndComboSize.GetCount() == 0)
	{
		TCHAR* szSize[] = {
			_T("8"),  _T("9"),  _T("10"), _T("11"),
				_T("12"), _T("14"), _T("16"), _T("18"),
				_T("20"), _T("22"), _T("24"), _T("26"),
				_T("28"), _T("36"), _T("48"), _T("72") };

			for (int i = 0; i < (sizeof(szSize)/sizeof(szSize[0])); i++)
			{
				m_wndComboSize.AddString(szSize[i]);
			}
	}

	m_csSize.Format(_T("%i"),-::MulDiv(lf.lfHeight,
		72, ::GetDeviceCaps(dc.m_hDC, LOGPIXELSY)));

	if (m_wndComboSize.SelectString(-1, m_csSize) == CB_ERR)
		m_wndComboSize.SetWindowText(m_csSize);
}

void CXTPSyntaxEditPropertiesPageFont::InitScriptCombo()
{
	m_wndComboScript.ResetContent();

	if (!m_editFont.GetSafeHandle())
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	m_iCharSet = lf.lfCharSet;

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

	CWindowDC dc(NULL);
	::EnumFontFamiliesEx(dc.m_hDC, &lf,
		(FONTENUMPROC)EnumFontFamExProc, (LPARAM)&m_wndComboScript, 0);

	int iCount = m_wndComboScript.GetCount();

	int i;
	for (i = 0; i < iCount; ++i)
	{
		BYTE lfCharSet = (BYTE)m_wndComboScript.GetItemData(i);
		if (lfCharSet == (BYTE)m_iCharSet)
		{
			m_wndComboScript.SetCurSel(i);
			break;
		}
	}

	if (i == iCount)
		m_wndComboScript.SetCurSel(0);
}

void CXTPSyntaxEditPropertiesPageFont::InitColorComboxes()
{
	m_wndComboText.SetAutoColor(::GetSysColor(COLOR_WINDOWTEXT));
	m_crText = XTPSyntaxEditPaintManager()->GetTextColor();
	if (m_wndComboText.SetSelColor(m_crText) == CB_ERR)
	{
		m_wndComboText.SetUserColor(m_crText);
		m_wndComboText.SetSelColor(m_crText);
	}

	m_wndComboBack.SetAutoColor(::GetSysColor(COLOR_WINDOW));
	m_crBack = XTPSyntaxEditPaintManager()->GetBackColor();
	if (m_wndComboBack.SetSelColor(m_crBack) == CB_ERR)
	{
		m_wndComboBack.SetUserColor(m_crBack);
		m_wndComboBack.SetSelColor(m_crBack);
	}

	m_wndComboHiliteText.SetAutoColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_crHiliteText = XTPSyntaxEditPaintManager()->GetHiliteTextColor();
	if (m_wndComboHiliteText.SetSelColor(m_crHiliteText) == CB_ERR)
	{
		m_wndComboHiliteText.SetUserColor(m_crHiliteText);
		m_wndComboHiliteText.SetSelColor(m_crHiliteText);
	}

	m_wndComboHiliteBack.SetAutoColor(::GetSysColor(COLOR_HIGHLIGHT));
	m_crHiliteBack = XTPSyntaxEditPaintManager()->GetHiliteBackColor();
	if (m_wndComboHiliteBack.SetSelColor(m_crHiliteBack) == CB_ERR)
	{
		m_wndComboHiliteBack.SetUserColor(m_crHiliteBack);
		m_wndComboHiliteBack.SetSelColor(m_crHiliteBack);
	}

	UpdateSampleColors();
}

void CXTPSyntaxEditPropertiesPageFont::UpdateSampleColors()
{
	m_txtSample.SetBackColor(m_crBack);
	m_txtSample.SetTextColor(m_crText);
	m_txtSampleSel.SetBackColor(m_crHiliteBack);
	m_txtSampleSel.SetTextColor(m_crHiliteText);
}

void CXTPSyntaxEditPropertiesPageFont::UpdateSampleFont()
{
	if (m_editFont.GetSafeHandle())
	{
		m_txtSample.SetFont(&m_editFont);
		m_txtSampleSel.SetFont(&m_editFont);
	}
}

int CXTPSyntaxEditPropertiesPageFont::GetLBText(CComboBox& comboBox, CString& csItemText)
{
	UpdateData();

	if (!::IsWindow(comboBox.m_hWnd))
		return CB_ERR;

	int iCurSel = comboBox.GetCurSel();
	if (iCurSel == CB_ERR)
		return CB_ERR;

	comboBox.GetLBText(iCurSel, csItemText);

	if (!csItemText.IsEmpty())
	{
		if (comboBox.SelectString(-1, csItemText) == CB_ERR)
			comboBox.SetWindowText(csItemText);

		return iCurSel;
	}

	return CB_ERR;
}

void CXTPSyntaxEditPropertiesPageFont::OnSelChangeComboNames()
{
	if (!m_wndComboName.GetListBox().SelChanged())
		return;

	if (GetLBText(m_wndComboName, m_csName) == CB_ERR)
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	STRCPY_S(lf.lfFaceName, m_csName.GetLength() + 1, m_csName);

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		InitSizeCombo();
		InitScriptCombo();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnSelChangeComboStyles()
{
	if (!m_wndComboStyle.GetListBox().SelChanged())
		return;

	if (GetLBText(m_wndComboStyle, m_csStyle) == CB_ERR)
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	CString csBuffer;
	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_ITALIC);
	lf.lfItalic = (m_csStyle.Find(csBuffer) == -1)? (BYTE)FALSE: (BYTE)TRUE;

	XTPResourceManager()->LoadString(&csBuffer, XTP_IDS_EDIT_BOLD);
	lf.lfWeight = (m_csStyle.Find(csBuffer) == -1)? FW_NORMAL: FW_BOLD;

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnSelChangeComboSizes()
{
	if (!m_wndComboSize.GetListBox().SelChanged())
		return;

	if (GetLBText(m_wndComboSize, m_csSize) == CB_ERR)
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	CWindowDC dc(NULL);
	lf.lfHeight = -MulDiv(_ttoi(m_csSize),
		::GetDeviceCaps(dc.m_hDC, LOGPIXELSY), 72);

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnChkStrikeOut()
{
	UpdateData();

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	lf.lfStrikeOut = (BYTE)m_bStrikeOut;

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnChkUnderline()
{
	UpdateData();

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	lf.lfUnderline = (BYTE)m_bUnderline;

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnSelEndOkScript()
{
	if (!m_wndComboScript.GetListBox().SelChanged())
		return;

	UpdateData();

	int iCurSel = m_wndComboScript.GetCurSel();
	if (iCurSel == CB_ERR)
		return;

	LOGFONT lf = {0};
	if (!GetSafeLogFont(lf))
		return;

	lf.lfCharSet = (BYTE)m_wndComboScript.GetItemData(iCurSel);
	m_iCharSet = lf.lfCharSet;

	if (CreateSafeFontIndirect(m_editFont, lf))
	{
		UpdateSampleFont();
		SetModified(TRUE);
		UpdateData(FALSE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnSelEndOkHiliteText()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageFont::OnSelEndOkHiliteBack()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageFont::OnSelEndOkText()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageFont::OnSelEndOkBack()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
}

void CXTPSyntaxEditPropertiesPageFont::OnBtnCustomText()
{
	if (m_wndComboText.SelectUserColor() != CB_ERR)
	{
		m_crText = m_wndComboText.GetSelColor();
		OnSelEndOkText();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnBtnCustomBack()
{
	if (m_wndComboBack.SelectUserColor() != CB_ERR)
	{
		m_crBack = m_wndComboBack.GetSelColor();
		OnSelEndOkBack();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnBtnCustomHiliteText()
{
	if (m_wndComboHiliteText.SelectUserColor() != CB_ERR)
	{
		m_crHiliteText = m_wndComboHiliteText.GetSelColor();
		OnSelEndOkHiliteText();
		SetModified(TRUE);
	}
}

void CXTPSyntaxEditPropertiesPageFont::OnBtnCustomtHiliteBack()
{
	if (m_wndComboHiliteBack.SelectUserColor() != CB_ERR)
	{
		m_crHiliteBack = m_wndComboHiliteBack.GetSelColor();
		OnSelEndOkHiliteBack();
		SetModified(TRUE);
	}
}

BOOL CXTPSyntaxEditPropertiesPageFont::OnApply()
{
	if (!CPropertyPage::OnApply())
		return FALSE;

	// no changes made or they have already been applied.
	if (!m_bModified)
		return TRUE;

	// update registry settings.
	if (!WriteRegistryValues())
		return FALSE;

	m_bModified = FALSE;
	return TRUE;
}

void CXTPSyntaxEditPropertiesPageFont::SetModified(BOOL bChanged/*=TRUE*/)
{
	CPropertyPage::SetModified();
	m_bModified = bChanged;
}

//===========================================================================
// CXTPSyntaxEditPropertiesPageColor property page
//===========================================================================

CXTPSyntaxEditPropertiesPageColor::CXTPSyntaxEditPropertiesPageColor(CXTPSyntaxEditView* pEditView/*=NULL*/)
: //CPropertyPage(CXTPSyntaxEditPropertiesPageColor::IDD),
  m_bModified(FALSE)
, m_pEditView(pEditView)
, m_parLexClassInfo(NULL)
{

	m_psp.dwFlags |= PSP_DLGINDIRECT;
	m_psp.pResource = XTPResourceManager()->LoadDialogTemplate(IDD);

	//{{AFX_DATA_INIT(CXTPSyntaxEditPropertiesPageColor)
	m_bBold = FALSE;
	m_bItalic = FALSE;
	m_bUnderline = FALSE;
	//}}AFX_DATA_INIT

	SetDefaults();

	if (m_pEditView)
	{
		CXTPSyntaxEditConfigurationManager* ptrConfigMgr =
			m_pEditView->GetEditCtrl().GetLexConfigurationManager();

		if (ptrConfigMgr)
		{
			m_ptrConfigMgr.SetPtr(ptrConfigMgr, TRUE);
			m_pTextSchemesMgr = &m_ptrConfigMgr->GetTextSchemesManager();
		}
	}
}

CXTPSyntaxEditPropertiesPageColor::~CXTPSyntaxEditPropertiesPageColor()
{
	XTPSyntaxEditLexConfig()->CleanInfoMap(m_mapLexClassInfo);
}

void CXTPSyntaxEditPropertiesPageColor::SetDefaults()
{
	// reset color values.
	m_crText        = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crBack        = ::GetSysColor(COLOR_WINDOW);
	m_crHiliteText  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crHiliteBack  = ::GetSysColor(COLOR_HIGHLIGHT);

	// reset font attribute flags.
	m_bBold         = FALSE;
	m_bItalic       = FALSE;
	m_bUnderline    = FALSE;

	if (::IsWindow(m_hWnd))
	{
		UpdateData(FALSE);
	}
}

void CXTPSyntaxEditPropertiesPageColor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPSyntaxEditPropertiesPageColor)
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_BOLD, m_bBold);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_ITALIC, m_bItalic);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_UNDERLINE, m_bUnderline);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_TEXT, m_wndComboText);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_BACK, m_wndComboBack);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_TEXTSEL, m_wndComboHiliteText);
	DDX_Control(pDX, XTP_IDC_EDIT_COMB_BACKSEL, m_wndComboHiliteBack);
	DDX_Control(pDX, XTP_IDC_EDIT_TXT_SAMPLE, m_txtSample);
	DDX_Control(pDX, XTP_IDC_EDIT_TXT_SAMPLESEL, m_txtSampleSel);
	DDX_Control(pDX, XTP_IDC_EDIT_LBOX_SCHEMANAMES, m_lboxName);
	DDX_Control(pDX, XTP_IDC_EDIT_LBOX_SCHEMAPROP, m_lboxProp);
	DDX_Control(pDX, XTP_IDC_EDIT_CHK_BOLD, m_btnBold);
	DDX_Control(pDX, XTP_IDC_EDIT_CHK_ITALIC, m_btnItalic);
	DDX_Control(pDX, XTP_IDC_EDIT_CHK_UNDERLINE, m_btnUnderline);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_TEXT, m_btnCustomText);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_BACK, m_btnCustomBack);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_TEXTSEL, m_btnCustomHiliteText);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_CUSTOM_BACKSEL, m_btnCustomHiliteBack);
	DDX_Control(pDX, XTP_IDC_EDIT_GRP_SAMPLE, m_gboxSampleText);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_TEXT, m_crText);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_BACK, m_crBack);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_TEXTSEL, m_crHiliteText);
	DDX_CBSyntaxColor(pDX, XTP_IDC_EDIT_COMB_BACKSEL, m_crHiliteBack);
	//}}AFX_DATA_MAP
}

IMPLEMENT_DYNCREATE(CXTPSyntaxEditPropertiesPageColor, CPropertyPage)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditPropertiesPageColor, CPropertyPage)
	//{{AFX_MSG_MAP(CXTPSyntaxEditPropertiesPageColor)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_BOLD, OnChkBold)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_ITALIC, OnChkItalic)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_UNDERLINE, OnChkUnderline)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_TEXT, OnBtnCustomText)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_BACK, OnBtnCustomBack)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_TEXTSEL, OnBtnCustomHiliteText)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_CUSTOM_BACKSEL, OnBtnCustomtHiliteBack)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_TEXTSEL, OnSelEndOkHiliteText)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_BACKSEL, OnSelEndOkHiliteBack)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_TEXT, OnSelEndOkText)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMB_BACK, OnSelEndOkBack)
	ON_LBN_SELCHANGE(XTP_IDC_EDIT_LBOX_SCHEMANAMES, OnSelChangeSchemaNames)
	ON_LBN_SELCHANGE(XTP_IDC_EDIT_LBOX_SCHEMAPROP, OnSelChangeSchemaProp)
	ON_LBN_DBLCLK(XTP_IDC_EDIT_LBOX_SCHEMANAMES, OnDblClickSchema)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CXTPSyntaxEditPropertiesPageColor::UpdateSampleColors()
{
	m_txtSampleSel.SetBackColor(m_crHiliteBack);
	m_txtSampleSel.SetTextColor(m_crHiliteText);
	m_txtSample.SetBackColor(m_crBack);
	m_txtSample.SetTextColor(m_crText);
}

BOOL CXTPSyntaxEditPropertiesPageColor::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	m_wndComboHiliteBack.SetAutoColor(::GetSysColor(COLOR_HIGHLIGHT));
	m_wndComboHiliteText.SetAutoColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	m_wndComboBack.SetAutoColor(::GetSysColor(COLOR_WINDOW));
	m_wndComboText.SetAutoColor(::GetSysColor(COLOR_WINDOWTEXT));

	if (m_wndComboBack.SetSelColor(m_crBack) == CB_ERR)
	{
		m_wndComboBack.SetUserColor(m_crBack);
		m_wndComboBack.SetSelColor(m_crBack);
	}

	if (m_wndComboText.SetSelColor(m_crText) == CB_ERR)
	{
		m_wndComboText.SetUserColor(m_crText);
		m_wndComboText.SetSelColor(m_crText);
	}

	if (m_wndComboHiliteBack.SetSelColor(m_crHiliteBack) == CB_ERR)
	{
		m_wndComboHiliteBack.SetUserColor(m_crHiliteBack);
		m_wndComboHiliteBack.SetSelColor(m_crHiliteBack);
	}

	if (m_wndComboHiliteText.SetSelColor(m_crHiliteText) == CB_ERR)
	{
		m_wndComboHiliteText.SetUserColor(m_crHiliteText);
		m_wndComboHiliteText.SetSelColor(m_crHiliteText);
	}

	XTPSyntaxEditLexConfig()->CopyInfoMap(m_mapLexClassInfo);

	// Get all schemes names
	CXTPSyntaxEditSchemaFileInfoList& infoList = m_pTextSchemesMgr->GetSchemaList();
	for (POSITION pos = infoList.GetHeadPosition(); pos;)
	{
		XTP_EDIT_SCHEMAFILEINFO& info = infoList.GetNext(pos);
		int iIndex = m_lboxName.AddString((info.csDesc.GetLength() > 0)? info.csDesc: info.csName);
		if (iIndex != LB_ERR)
		{
			m_lboxName.SetItemData(iIndex, (DWORD_PTR)&info);
		}
	}

	if (m_lboxName.GetCount() > 0)
	{
		m_lboxName.SetCurSel(0);
		OnSelChangeSchemaNames();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CXTPSyntaxEditPropertiesPageColor::WriteRegistryValues()
{
	if (!::IsWindow(m_pEditView->GetSafeHwnd()))
		return FALSE;

	for (POSITION pos = m_mapLexClassInfo.GetStartPosition(); pos;)
	{
		CString csKey; CXTPSyntaxEditLexClassInfoArray* pLexClassArray = NULL;
		m_mapLexClassInfo.GetNextAssoc(pos, csKey, (void*&)pLexClassArray);
		XTPSyntaxEditLexConfig()->WriteCfgFile(csKey, *pLexClassArray);
	}

	return TRUE;
}

void CXTPSyntaxEditPropertiesPageColor::OnSelEndOkHiliteText()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
	UpdateColorValue(m_wndComboHiliteText, m_crHiliteText, XTPLEX_ATTR_TXT_COLORSELFG);
}

void CXTPSyntaxEditPropertiesPageColor::OnSelEndOkHiliteBack()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
	UpdateColorValue(m_wndComboHiliteBack, m_crHiliteBack, XTPLEX_ATTR_TXT_COLORSELBK);
}

void CXTPSyntaxEditPropertiesPageColor::OnSelEndOkText()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
	UpdateColorValue(m_wndComboText, m_crText, XTPLEX_ATTR_TXT_COLORFG);
}

void CXTPSyntaxEditPropertiesPageColor::OnSelEndOkBack()
{
	UpdateData();
	UpdateSampleColors();
	SetModified(TRUE);
	UpdateColorValue(m_wndComboBack, m_crBack, XTPLEX_ATTR_TXT_COLORBK);
}

void CXTPSyntaxEditPropertiesPageColor::OnChkBold()
{
	UpdateData();
	UpdateFont();
	SetModified(TRUE);
	UpdateFontValue(m_bBold, XTPLEX_ATTR_TXT_BOLD);
}

void CXTPSyntaxEditPropertiesPageColor::OnChkItalic()
{
	UpdateData();
	UpdateFont();
	SetModified(TRUE);
	UpdateFontValue(m_bItalic, XTPLEX_ATTR_TXT_ITALIC);
}

void CXTPSyntaxEditPropertiesPageColor::OnChkUnderline()
{
	UpdateData();
	UpdateFont();
	SetModified(TRUE);
	UpdateFontValue(m_bUnderline, XTPLEX_ATTR_TXT_UNDERLINE);
}

void CXTPSyntaxEditPropertiesPageColor::OnBtnCustomText()
{
	if (m_wndComboText.SelectUserColor() != CB_ERR)
	{
		m_crText = m_wndComboText.GetSelColor();
		OnSelEndOkText();
	}
}

void CXTPSyntaxEditPropertiesPageColor::OnBtnCustomBack()
{
	if (m_wndComboBack.SelectUserColor() != CB_ERR)
	{
		m_crBack = m_wndComboBack.GetSelColor();
		OnSelEndOkBack();
	}
}

void CXTPSyntaxEditPropertiesPageColor::OnBtnCustomHiliteText()
{
	if (m_wndComboHiliteText.SelectUserColor() != CB_ERR)
	{
		m_crHiliteText = m_wndComboHiliteText.GetSelColor();
		OnSelEndOkHiliteText();
	}
}

void CXTPSyntaxEditPropertiesPageColor::OnBtnCustomtHiliteBack()
{
	if (m_wndComboHiliteBack.SelectUserColor() != CB_ERR)
	{
		m_crHiliteBack = m_wndComboHiliteBack.GetSelColor();
		OnSelEndOkHiliteBack();
	}
}

BOOL CXTPSyntaxEditPropertiesPageColor::OnSetActive()
{
	UpdateFont();
	return CPropertyPage::OnSetActive();
}

void CXTPSyntaxEditPropertiesPageColor::UpdateFont()
{
	CXTPSyntaxEditPropertiesDlg* pParentDlg = DYNAMIC_DOWNCAST(
		CXTPSyntaxEditPropertiesDlg, GetParent());

	if (!pParentDlg)
		return;

	CXTPSyntaxEditPropertiesPageFont* pPage2 = DYNAMIC_DOWNCAST(
		CXTPSyntaxEditPropertiesPageFont, pParentDlg->GetPage(1));

	if (!pPage2)
		return;

	LOGFONT lf = {0};
	if (!pPage2->GetSafeLogFont(lf))
		return;

	lf.lfStrikeOut = 0;
	lf.lfItalic    = (BYTE)m_bItalic;
	lf.lfUnderline = (BYTE)m_bUnderline;
	lf.lfWeight    = m_bBold? FW_BOLD: FW_NORMAL;

	if (pPage2->CreateSafeFontIndirect(m_editFont, lf))
	{
		m_txtSample.SetFont(&m_editFont);
		m_txtSampleSel.SetFont(&m_editFont);
	}
}

BOOL CXTPSyntaxEditPropertiesPageColor::OnApply()
{
	if (!CPropertyPage::OnApply())
		return FALSE;

	// no changes made or they have already been applied.
	if (!m_bModified)
		return TRUE;

	// update registry settings.
	if (!WriteRegistryValues())
		return FALSE;

	m_bModified = FALSE;

	return TRUE;
}

void CXTPSyntaxEditPropertiesPageColor::EnableControls()
{
	BOOL bEnable = (m_lboxName.GetCurSel() != LB_ERR);
	m_lboxProp.EnableWindow(bEnable);

	bEnable = (bEnable && m_lboxProp.GetCurSel() != LB_ERR);
	m_txtSampleSel.EnableWindow(bEnable);
	m_txtSample.EnableWindow(bEnable);
	m_gboxSampleText.EnableWindow(bEnable);
	m_wndComboHiliteText.EnableWindow(bEnable);
	m_wndComboHiliteBack.EnableWindow(bEnable);
	m_wndComboText.EnableWindow(bEnable);
	m_wndComboBack.EnableWindow(bEnable);
	m_btnBold.EnableWindow(bEnable);
	m_btnItalic.EnableWindow(bEnable);
	m_btnUnderline.EnableWindow(bEnable);
	m_btnCustomText.EnableWindow(bEnable);
	m_btnCustomBack.EnableWindow(bEnable);
	m_btnCustomHiliteText.EnableWindow(bEnable);
	m_btnCustomHiliteBack.EnableWindow(bEnable);
}

void CXTPSyntaxEditPropertiesPageColor::OnSelChangeSchemaNames()
{
	if (!m_lboxName.SelChanged())
		return;

	SetDefaults();

	int iIndex = m_lboxName.GetCurSel();
	if (iIndex != LB_ERR)
	{
		XTP_EDIT_SCHEMAFILEINFO* pSchemaInfo = (XTP_EDIT_SCHEMAFILEINFO*)m_lboxName.GetItemData(iIndex);
		if (pSchemaInfo)
		{
			InitSchemaClasses(pSchemaInfo);
		}
	}
}

void CXTPSyntaxEditPropertiesPageColor::OnSelChangeSchemaProp()
{
	if (!m_lboxProp.SelChanged())
		return;

	SetDefaults();

	int iIndex = m_lboxProp.GetCurSel();
	if (iIndex == LB_ERR)
		return;

	XTP_EDIT_LEXCLASSINFO* pInfoClass = (XTP_EDIT_LEXCLASSINFO*)m_lboxProp.GetItemData(iIndex);
	if (!pInfoClass)
		return;

	InitClassData(*pInfoClass);
	UpdateData(FALSE);
}

BOOL CXTPSyntaxEditPropertiesPageColor::IsTopLevelClass(const XTP_EDIT_LEXCLASSINFO& infoClass)
{
	int nPropertiesCount = (int)infoClass.arPropertyDesc.GetSize();
	for (int iIndex = 0; iIndex < nPropertiesCount; iIndex++)
	{
		// Process property name
		const XTP_EDIT_LEXPROPINFO& infoProp =
			infoClass.arPropertyDesc.GetAt(iIndex);

		if (infoProp.arPropName.GetSize() != 2)
			continue;

		CString csParent = infoProp.arPropName.GetAt(0);
		if (csParent.CompareNoCase(_T("parent")) != 0)
			continue;

		CString csFile = infoProp.arPropName.GetAt(1);
		if (csFile.CompareNoCase(_T("file")) != 0)
			continue;

		return TRUE;
	}

	return FALSE;
}

CString CXTPSyntaxEditPropertiesPageColor::GetDisplayName(const XTP_EDIT_LEXCLASSINFO& info) const
{
	for (int i = 0; i < info.arPropertyDesc.GetSize(); ++i)
	{
		if (MakeStr(info.arPropertyDesc[i].arPropName, _T(":")) == XTPLEX_ATTR_DISPLAYNAME)
		{
			CString csDisplayName = GetPropValue(info.arPropertyDesc[i]);
			REMOVE_S(csDisplayName, 39);
			return csDisplayName;
		}
	}
	return info.csClassName;
}

BOOL CXTPSyntaxEditPropertiesPageColor::InitSchemaClasses(XTP_EDIT_SCHEMAFILEINFO* pSchemaInfo)
{
	CString csKey = pSchemaInfo->csValue;
	csKey.MakeLower();

	m_parLexClassInfo = NULL;
	if (!m_mapLexClassInfo.Lookup(csKey, (void*&)m_parLexClassInfo) || !m_parLexClassInfo)
		return FALSE;

	m_lboxProp.SetRedraw(FALSE);
	m_lboxProp.ResetContent();

	// For all classes in the current scheme
	int iSize = (int)m_parLexClassInfo->GetSize();
	for (int i = 0; i < iSize; i++)
	{
		int iIndex = m_lboxProp.AddString(GetDisplayName((*m_parLexClassInfo)[i]));
		if (iIndex != LB_ERR)
			m_lboxProp.SetItemData(iIndex, (DWORD_PTR)&(*m_parLexClassInfo)[i]);
	}

	m_lboxProp.SetCurSel(0);
	OnSelChangeSchemaProp();

	m_lboxProp.SetRedraw(TRUE);
	m_lboxProp.RedrawWindow();

	return TRUE;
}

CString CXTPSyntaxEditPropertiesPageColor::GetPropValue(const XTP_EDIT_LEXPROPINFO& infoProp) const
{
	CString csValue;
	for (int iItem = 0; iItem < infoProp.arPropValue.GetSize(); iItem++)
	{
		if (iItem > 0)
		{
			csValue += _T(", ");
		}
		CString strProp1 = infoProp.arPropValue.GetAt(iItem);
		csValue += XTPSyntaxEditLexConfig()->ESToStr(strProp1, TRUE);
	}
	return csValue;
}

AFX_STATIC int AFX_CDECL AfxStrToInt(LPCTSTR pcszValue)
{
	TCHAR* pCh = NULL;
	return _tcstol(pcszValue, &pCh, 0);
}

AFX_STATIC DWORD AFX_CDECL AfxStrToRGB(LPCTSTR pcszValue)
{
	return XTP_EDIT_RGB_INT2CLR(AfxStrToInt(pcszValue));
}

void CXTPSyntaxEditPropertiesPageColor::InitClassData(const XTP_EDIT_LEXCLASSINFO& infoClass)
{
	int iCount = (int)infoClass.arPropertyDesc.GetSize();
	for (int iIndex = 0; iIndex < iCount; ++iIndex)
	{
		const XTP_EDIT_LEXPROPINFO& infoProp = infoClass.arPropertyDesc.GetAt(iIndex);
		CString csPropName = MakeStr(infoProp.arPropName, _T(":"));

		if (csPropName == XTPLEX_ATTR_TXT_COLORFG)
		{
			m_crText = AfxStrToRGB(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_COLORBK)
		{
			m_crBack = AfxStrToRGB(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_COLORSELFG)
		{
			m_crHiliteText = AfxStrToRGB(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_COLORSELBK)
		{
			m_crHiliteBack = AfxStrToRGB(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_BOLD)
		{
			m_bBold = (BOOL)_ttoi(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_ITALIC)
		{
			m_bItalic = (BOOL)_ttoi(GetPropValue(infoProp));
		}
		else if (csPropName == XTPLEX_ATTR_TXT_UNDERLINE)
		{
			m_bUnderline = (BOOL)_ttoi(GetPropValue(infoProp));
		}
	}

	UpdateFont();
	UpdateSampleColors();
	EnableControls();
}

void CXTPSyntaxEditPropertiesPageColor::SetModified(BOOL bChanged/*=TRUE*/)
{
	CPropertyPage::SetModified(bChanged);
	m_bModified = bChanged;

	if (m_parLexClassInfo)
		m_parLexClassInfo->m_bModified = bChanged;
}

int CXTPSyntaxEditPropertiesPageColor::PropExists(CXTPSyntaxEditLexPropInfoArray& arrProp, LPCTSTR lpszPropName)
{
	int iCount = (int)arrProp.GetSize();
	for (int iIndex = 0; iIndex < iCount; ++iIndex)
	{
		const XTP_EDIT_LEXPROPINFO& infoProp = arrProp.GetAt(iIndex);
		CString csPropName = MakeStr(infoProp.arPropName, _T(":"));

		if (csPropName.CompareNoCase(lpszPropName) == 0)
		{
			return iIndex;
		}
	}
	return -1;
}

BOOL CXTPSyntaxEditPropertiesPageColor::UpdateColorValue(CXTPSyntaxEditColorComboBox& combo, COLORREF& color, LPCTSTR lpszPropName)
{
	int iSel = m_lboxProp.GetCurSel();

	XTP_EDIT_LEXCLASSINFO* pInfoClass = (XTP_EDIT_LEXCLASSINFO*)m_lboxProp.GetItemData(iSel);
	if (!pInfoClass)
		return FALSE;

	int iIndex = PropExists(pInfoClass->arPropertyDesc, lpszPropName);

	// automatic color selected, remove from array.
	if (combo.GetAutoColor() == color)
	{
		if (iIndex != -1)
		{
			pInfoClass->arPropertyDesc.RemoveAt(iIndex);
			return TRUE;
		}
	}

	else
	{
		CString csColor;
		csColor.Format(_T("0x%06X"), XTP_EDIT_RGB_INT2CLR(color));

		if (iIndex == -1)
		{
			XTP_EDIT_LEXPROPINFO infoProp;
			infoProp.arPropValue.Add(csColor);
			PropPathSplit(lpszPropName, infoProp.arPropName);
			pInfoClass->arPropertyDesc.Add(infoProp);
		}
		else
		{
			XTP_EDIT_LEXPROPINFO& infoProp = pInfoClass->arPropertyDesc[iIndex];
			infoProp.arPropValue.RemoveAll();
			infoProp.arPropValue.Add(csColor);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CXTPSyntaxEditPropertiesPageColor::UpdateFontValue(BOOL& bValue, LPCTSTR lpszPropName)
{
	CString csValue(_T("1"));
	int iSel = m_lboxProp.GetCurSel();

	XTP_EDIT_LEXCLASSINFO* pInfoClass = (XTP_EDIT_LEXCLASSINFO*)m_lboxProp.GetItemData(iSel);
	if (!pInfoClass)
		return FALSE;

	int iIndex = PropExists(pInfoClass->arPropertyDesc, lpszPropName);
	if (iIndex != -1)
	{
		pInfoClass->arPropertyDesc.RemoveAt(iIndex);
	}

	if (bValue == TRUE)
	{
		XTP_EDIT_LEXPROPINFO infoProp;
		PropPathSplit(lpszPropName, infoProp.arPropName);
		infoProp.arPropValue.Add(csValue);
		pInfoClass->arPropertyDesc.Add(infoProp);
		return TRUE;
	}

	return FALSE;
}

void CXTPSyntaxEditPropertiesPageColor::OnDblClickSchema()
{
	int iIndex = m_lboxName.GetCurSel();
	if (iIndex == LB_ERR)
		return;

	XTP_EDIT_SCHEMAFILEINFO* pSchemaInfo = (XTP_EDIT_SCHEMAFILEINFO*)m_lboxName.GetItemData(iIndex);
	if (pSchemaInfo == NULL)
		return;

	if (!FILEEXISTS_S(pSchemaInfo->csValue))
		return;

	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFileName[_MAX_FNAME], szEx[_MAX_EXT];
	SPLITPATH_S(pSchemaInfo->csValue, szDrive, szDir, szFileName, szEx);

	CString csBuffer;
	XTPResourceManager()->LoadString(
		&csBuffer, XTP_IDS_EDIT_OPENSCHEMAMSG);

	CString csMessage;
	csMessage.Format(csBuffer, szFileName, szEx);
	if (AfxMessageBox(csMessage, MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		// close the options dialog.
		CPropertySheet* pWndParent = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());
		if (pWndParent)
			pWndParent->EndDialog(IDCANCEL);

		// open the document.
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp)
			pWinApp->OpenDocumentFile(pSchemaInfo->csValue);
	}
}

void CXTPSyntaxEditPropertiesPageColor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	UNREFERENCED_PARAMETER(pWnd);

	int iItem = m_lboxName.HitTest(point);
	if (iItem != LB_ERR)
	{
		m_lboxName.SetCurSel(iItem);
		OnSelChangeSchemaNames();

		CString csBuffer;
		XTPResourceManager()->LoadString(
			&csBuffer, XTP_IDS_EDIT_OPENSCHEMACMD);

		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(MF_STRING, ID_FILE_OPEN, csBuffer);

		UINT nFlags = TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD;
		if (menu.TrackPopupMenu(nFlags, point.x, point.y, this) == ID_FILE_OPEN)
		{
			OnDblClickSchema();
		}
	}
}

//===========================================================================
// CXTPSyntaxEditTipWnd
//===========================================================================

CXTPSyntaxEditTipWnd::CXTPSyntaxEditTipWnd()
: m_pListBox(NULL)
, m_iIndex(LB_ERR)
, m_uIDEvent1(1001)
, m_uIDEvent2(1002)
{
	RegisterWindowClass();
}

CXTPSyntaxEditTipWnd::~CXTPSyntaxEditTipWnd()
{
	DestroyWindow();
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditTipWnd, CWnd)
	//{{AFX_MSG_MAP(CXTPSyntaxEditTipWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_NCHITTEST_EX()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// CXTPSyntaxEditTipWnd diagnostics
//---------------------------------------------------------------------------

#ifdef _DEBUG
void CXTPSyntaxEditTipWnd::AssertValid() const
{
	CWnd::AssertValid();

	ASSERT(::IsWindow(m_hWnd));
	ASSERT_VALID(m_pListBox);
}

void CXTPSyntaxEditTipWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

//---------------------------------------------------------------------------
// CXTPSyntaxEditTipWnd registration
//---------------------------------------------------------------------------

BOOL CXTPSyntaxEditTipWnd::RegisterWindowClass(HINSTANCE hInstance /*= NULL*/)
{
	WNDCLASS wndcls;
	if (hInstance == NULL) hInstance = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInstance, XTP_EDIT_CLASSNAME_LBOXTIP, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style = CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInstance;
		wndcls.hIcon = NULL;
		wndcls.hCursor = ::LoadCursor(0, IDC_ARROW);
		wndcls.hbrBackground = (HBRUSH)(COLOR_INFOBK + 1);
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = XTP_EDIT_CLASSNAME_LBOXTIP;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
// CXTPSyntaxEditTipWnd paint routines
//---------------------------------------------------------------------------

BOOL CXTPSyntaxEditTipWnd::OwnerDrawTip(CDC* pDC, CRect rClient)
{
	ASSERT_VALID(this);
	HWND hWndLBox = m_pListBox->GetSafeHwnd();

	if (!::IsWindow(hWndLBox))
		return FALSE;

	DRAWITEMSTRUCT dis;
	::ZeroMemory(&dis, sizeof(DRAWITEMSTRUCT));

	dis.CtlType     = ODT_LISTBOX;
	dis.CtlID       = (UINT)m_pListBox->GetDlgCtrlID();
	dis.itemID      = (UINT)m_iIndex;
	dis.itemAction  = ODA_DRAWENTIRE;
	dis.hwndItem    = hWndLBox;
	dis.hDC         = pDC->GetSafeHdc();
	dis.rcItem      = rClient;
	dis.itemData    = m_pListBox->GetItemData(m_iIndex);

	if (m_pListBox->GetSel(m_iIndex) > 0)
		dis.itemState |= ODS_SELECTED;

	if (m_pListBox->GetCaretIndex() == m_iIndex)
		dis.itemState |= ODS_FOCUS;

	m_pListBox->DrawItem(&dis);

	return TRUE;
}

BOOL CXTPSyntaxEditTipWnd::DrawTip(CDC* pDC, CRect rClient)
{
	ASSERT_VALID(this);

	CString csItemText;
	m_pListBox->GetText(m_iIndex, csItemText);

	CXTPFontDC fontDC(pDC, m_pListBox->GetFont(), GetTextColor());

	pDC->SetBkColor(GetBackColor());
	pDC->FillSolidRect(rClient, pDC->GetBkColor());
	pDC->SetBkMode(TRANSPARENT);

	pDC->DrawText(csItemText, rClient,
		DT_VCENTER | DT_CENTER |DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS);

	return TRUE;
}

COLORREF CXTPSyntaxEditTipWnd::GetTextColor() const
{
	ASSERT_VALID(this);

	return ::GetSysColor((m_pListBox->GetSel(m_iIndex) > 0)?
		COLOR_HIGHLIGHTTEXT: COLOR_INFOTEXT);
}

COLORREF CXTPSyntaxEditTipWnd::GetBackColor() const
{
	ASSERT_VALID(this);

	return ::GetSysColor((m_pListBox->GetSel(m_iIndex) > 0)?
		COLOR_HIGHLIGHT: COLOR_INFOBK);
}

void CXTPSyntaxEditTipWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CXTPClientRect rClient(this);
	if (IsOwnerDrawn())
		OwnerDrawTip(&dc, rClient);
	else
		DrawTip(&dc, rClient);
}

void CXTPSyntaxEditTipWnd::OnNcPaint()
{
	CXTPWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	CWindowDC dc(this);
	dc.SetBkColor(GetBackColor());
	dc.SetTextColor(GetTextColor());
	dc.FillSolidRect(&rc, dc.GetBkColor());
	dc.DrawFocusRect(&rc);
}

//---------------------------------------------------------------------------
// CXTPSyntaxEditTipWnd message handlers
//---------------------------------------------------------------------------

void CXTPSyntaxEditTipWnd::OnTimer(UINT_PTR nIDEvent)
{
	ASSERT_VALID(this);

	if (nIDEvent == m_uIDEvent1)
	{
		HideTip();
	}

	if (nIDEvent == m_uIDEvent2)
	{
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);

		BOOL bOutside = FALSE;
		m_pListBox->ItemFromPoint(point, bOutside);

		if (bOutside)
		{
			HideTip();
		}
	}

	CWnd::OnTimer(nIDEvent);
}

LRESULT CXTPSyntaxEditTipWnd::OnNcHitTest(CPoint point)
{
	UNREFERENCED_PARAMETER(point);
	return (LRESULT)HTTRANSPARENT;
}

BOOL CXTPSyntaxEditTipWnd::Create(CListBox* pListBox)
{
	ASSERT_VALID(pListBox);
	m_pListBox = pListBox;

	if (!::IsWindow(m_pListBox->GetSafeHwnd()))
		return FALSE;

	// Already created?
	if (::IsWindow(m_hWnd))
		return TRUE;

	if (!CWnd::CreateEx(WS_EX_TOOLWINDOW, XTP_EDIT_CLASSNAME_LBOXTIP, NULL,
		WS_POPUP | WS_BORDER | WS_CLIPSIBLINGS, CXTPEmptyRect(), NULL, 0))
	{
		TRACE0("Failed to create tooltip window.\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CXTPSyntaxEditTipWnd::IsOwnerDrawn()
{
	ASSERT_VALID(this);

	HWND hWndLBox = m_pListBox->GetSafeHwnd();

	if (::IsWindow(hWndLBox))
	{
		return ((::GetWindowLong(hWndLBox, GWL_STYLE) &
			(LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) != 0);
	}
	return FALSE;
}

BOOL CXTPSyntaxEditTipWnd::ShowTip(int iIndex)
{
	ASSERT_VALID(this);

	if ((iIndex < 0) || (iIndex >= m_pListBox->GetCount()))
		return HideTip();

	CPoint ptCursor;
	::GetCursorPos(&ptCursor);

	if (m_iIndex == iIndex)
	{
		if (ptCursor == m_ptCursor)
		{
			RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
			return TRUE;
		}
	}

	m_ptCursor = ptCursor;

	// stop existing timer.
	KillTipTimer();

	CRect rItem;
	if (CalcItemRect(iIndex, rItem))
	{
		if ((m_rWindow == rItem) || m_rWindow.IsRectEmpty())
			return HideTip();

		m_rWindow.InflateRect(0,2);

		if (m_iIndex != iIndex)
		{
			m_iIndex = iIndex;
			RedrawWindow(NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
		}

		ShowWindow(SW_SHOWNA);
		MoveWindow(&m_rWindow);

		// set timer.
		SetTipTimer();

		return TRUE;
	}

	return FALSE;
}

BOOL CXTPSyntaxEditTipWnd::HideTip()
{
	if (::IsWindowVisible(m_hWnd))
	{
		// stop existing timer.
		KillTipTimer();

		ShowWindow(SW_HIDE);
		return TRUE;
	}
	return FALSE;
}

BOOL CXTPSyntaxEditTipWnd::CalcItemRect(int iItem, CRect& rItem)
{
	ASSERT_VALID(this);

	if (m_pListBox->GetItemRect(iItem, &rItem) == LB_ERR)
		return FALSE;

	if (IsOwnerDrawn())
		return FALSE;

	CString csItem;
	m_pListBox->GetText(iItem, csItem);
	m_pListBox->ClientToScreen(rItem);

	if (csItem.IsEmpty())
		return FALSE;

	CWindowDC dc(NULL);
	CXTPFontDC fontDC(&dc, m_pListBox->GetFont());
	int iAdjust = dc.GetTextExtent(csItem).cx + (::GetSystemMetrics(SM_CXEDGE)*2);

	m_rWindow.CopyRect(rItem);
	m_rWindow.right = max(rItem.right, rItem.left + iAdjust);

	return TRUE;
}

void CXTPSyntaxEditTipWnd::SetTipTimer()
{
	SetTimer(m_uIDEvent1, 5000, NULL); // 5 seconds.
	SetTimer(m_uIDEvent2, 100,  NULL); // 10th of a second.
}

void CXTPSyntaxEditTipWnd::KillTipTimer()
{
	// stop existing timers.
	KillTimer(m_uIDEvent1);
	KillTimer(m_uIDEvent2);
}

//===========================================================================
// CXTPSyntaxEditTipListBox
//===========================================================================

CXTPSyntaxEditTipListBox::CXTPSyntaxEditTipListBox()
{
	m_dwIdx = (DWORD)MAKELONG(-1,-2);
}

int CXTPSyntaxEditTipListBox::HitTest(LPPOINT pPoint /*= NULL*/) const
{
	CPoint point;
	if (pPoint == NULL)
	{
		::GetCursorPos(&point);
		ScreenToClient(&point);
		pPoint = &point;
	}

	BOOL bOutside;
	int iIndex = (int)ItemFromPoint(*pPoint, bOutside);

	if (bOutside)
		return LB_ERR;

	return iIndex;
}

int CXTPSyntaxEditTipListBox::HitTest(CPoint point, BOOL bIsClient /*= FALSE*/) const
{
	if (!bIsClient)
		ScreenToClient(&point);

	return HitTest(&point);
}

int CXTPSyntaxEditTipListBox::ShowTip(CPoint point, BOOL bIsClient /*= FALSE*/)
{
	if (!bIsClient)
		ScreenToClient(&point);

	if (!::IsWindow(m_wndInfoTip.m_hWnd))
		m_wndInfoTip.Create(this);

	int iIndex = HitTest(&point);
	if (iIndex != LB_ERR)
	{
		m_wndInfoTip.ShowTip(iIndex);
	}

	return iIndex;
}

BOOL CXTPSyntaxEditTipListBox::SelChanged() const
{
	return (LOWORD(m_dwIdx) != HIWORD(m_dwIdx));
}

BOOL CXTPSyntaxEditTipListBox::PreTranslateMessage(MSG* pMsg)
{
	UINT uMessage = pMsg->message;

	switch (uMessage)
	{
	case WM_MOUSEMOVE:
		ShowTip(pMsg->pt);
		break;

	case WM_LBUTTONDOWN:
		m_dwIdx = (DWORD)MAKELONG(GetCurSel(), HIWORD(m_dwIdx));
		ShowTip(pMsg->pt);
		break;

	case WM_LBUTTONUP:
		m_dwIdx = (DWORD)MAKELONG(LOWORD(m_dwIdx), GetCurSel());
		break;

	case WM_KEYDOWN:
	case WM_MOUSEWHEEL:
		m_wndInfoTip.HideTip();
		break;
	}

	return CListBox::PreTranslateMessage(pMsg);
}

//===========================================================================
// CXTPSyntaxEditTipComboBox
//===========================================================================

BEGIN_MESSAGE_MAP(CXTPSyntaxEditTipComboBox, CComboBox)
	//{{AFX_MSG_MAP(CXTPSyntaxEditTipComboBox)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CXTPSyntaxEditTipComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		if (m_wndListBox.GetSafeHwnd() == NULL)
			m_wndListBox.SubclassWindow(pWnd->GetSafeHwnd());
	}

	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CXTPSyntaxEditTipComboBox::OnDestroy()
{
	if (m_wndListBox.GetSafeHwnd() != NULL)
		m_wndListBox.UnsubclassWindow();

	CComboBox::OnDestroy();
}
