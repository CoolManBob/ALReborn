// XTPSyntaxEditFindReplaceDlg.cpp : implementation file
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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

// common includes
#include "Common/XTPDrawHelpers.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"
#include "Common/XTPVc50Helpers.h"
#include "Common/XTPResourceManager.h"

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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR XTP_EDIT_FINDDLG_MATCHWHOLEWORD[]      = _T("MatchWholeWord");
const TCHAR XTP_EDIT_FINDDLG_MATCHCASE[]           = _T("MatchCase");
const TCHAR XTP_EDIT_FINDDLG_SEARCHDIRECTION[]     = _T("SearchDirection");
const TCHAR XTP_EDIT_FINDDLG_FINDHISTORYCOUNT[]    = _T("FindHistoryCount");
const TCHAR XTP_EDIT_FINDDLG_FINDHISTORYITEM[]     = _T("FindHistory_%d");
const TCHAR XTP_EDIT_FINDDLG_REPLACEHISTORYCOUNT[] = _T("ReplaceHistoryCount");
const TCHAR XTP_EDIT_FINDDLG_REPLACEHISTORYITEM[]  = _T("ReplaceHistory_%d");

//===========================================================================
// CXTPSyntaxEditFindReplaceDlg
//===========================================================================

CXTPSyntaxEditFindReplaceDlg::CXTPSyntaxEditFindReplaceDlg(CWnd* pParentWnd /*=NULL*/)
: //CDialog(CXTPSyntaxEditFindReplaceDlg::IDD, pParentWnd),
m_ptWndPos(CPoint(-1,-1))
, m_pEditView(NULL)
, m_bReplaceDlg(FALSE)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParentWnd);

	//{{AFX_DATA_INIT(CXTPSyntaxEditFindReplaceDlg)
	m_csFindText = _T("");
	m_csReplaceText = _T("");
	m_bMatchWholeWord = FALSE;
	m_bMatchCase = FALSE;
	m_nSearchDirection = 1;
	//}}AFX_DATA_INIT
}

void CXTPSyntaxEditFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPSyntaxEditFindReplaceDlg)
	DDX_Text(pDX, XTP_IDC_EDIT_COMBO_FIND, m_csFindText);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, XTP_IDC_EDIT_CHK_MATCH_WHOLE_WORD, m_bMatchWholeWord);
	DDX_Control(pDX, XTP_IDC_EDIT_COMBO_FIND, m_wndFindCombo);
	DDX_Control(pDX, XTP_IDC_EDIT_BTN_FIND_NEXT, m_btnFindNext);
	DDX_Control(pDX, XTP_IDC_EDIT_RADIO_UP, m_btnRadioUp);
	DDX_Control(pDX, XTP_IDC_EDIT_RADIO_DOWN, m_btnRadioDown);
	//}}AFX_DATA_MAP

	if (m_bReplaceDlg)
	{
		DDX_Text(pDX, XTP_IDC_EDIT_COMBO_REPLACE, m_csReplaceText);
		DDX_Control(pDX, XTP_IDC_EDIT_BTN_REPLACE, m_btnReplace);
		DDX_Control(pDX, XTP_IDC_EDIT_BTN_REPLACE_ALL, m_btnReplaceAll);
		DDX_Control(pDX, XTP_IDC_EDIT_COMBO_REPLACE, m_wndReplaceCombo);
	}
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditFindReplaceDlg, CDialog)
	//{{AFX_MSG_MAP(CXTPSyntaxEditFindReplaceDlg)
	ON_CBN_EDITCHANGE(XTP_IDC_EDIT_COMBO_FIND, OnEditChangeComboFind)
	ON_CBN_EDITCHANGE(XTP_IDC_EDIT_COMBO_REPLACE, OnEditChangeComboReplace)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMBO_REPLACE, OnSelendOkComboReplace)
	ON_CBN_SELENDOK(XTP_IDC_EDIT_COMBO_FIND, OnSelendOkComboFind)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_FIND_NEXT, OnBtnFindNext)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_REPLACE, OnBtnReplace)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_REPLACE_ALL, OnBtnReplaceAll)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_MATCH_WHOLE_WORD, OnChkMatchWholeWord)
	ON_BN_CLICKED(XTP_IDC_EDIT_CHK_MATCH_CASE, OnChkMatchCase)
	ON_BN_CLICKED(XTP_IDC_EDIT_RADIO_UP, OnRadioUp)
	ON_BN_CLICKED(XTP_IDC_EDIT_RADIO_DOWN, OnRadioDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditFindReplaceDlg message handlers

void CXTPSyntaxEditFindReplaceDlg::UpdateHistoryCombo(const CString& csText, CStringArray& arHistory)
{
	if (csText.IsEmpty())
	{
		return;
	}

	int nCount = (int)arHistory.GetSize();
	for (int i = nCount-1; i >= 0; i--)
	{
		if (arHistory[i] == csText)
		{
			arHistory.RemoveAt(i);
		}
	}
	arHistory.InsertAt(0, csText);
}

void CXTPSyntaxEditFindReplaceDlg::UpdateHistoryCombo(const CString& csText, CStringArray& arHistory, CComboBox& wndCombo)
{
	UpdateHistoryCombo(csText, arHistory);

	if (::IsWindow(wndCombo.m_hWnd))
	{
		CString strLBText;
		int nCount = wndCombo.GetCount();
		for (int i = nCount-1; i >= 0; i--)
		{
			wndCombo.GetLBText(i, strLBText);
			if (strLBText == csText)
			{
				wndCombo.DeleteString(i);
			}
		}
		wndCombo.InsertString(0, csText);
		wndCombo.SetWindowText(csText);
	}
}

void CXTPSyntaxEditFindReplaceDlg::InitHistoryCombo(CStringArray& arHistory, CComboBox& wndCombo)
{
	wndCombo.ResetContent();

	int nCount = (int)arHistory.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		wndCombo.AddString(arHistory[i]);
	}
}

BOOL CXTPSyntaxEditFindReplaceDlg::ShowDialog(CXTPSyntaxEditView* pEditView, BOOL bReplaceDlg/*=FALSE*/)
{
	ASSERT_VALID(pEditView);

	m_pEditView = pEditView;
	m_bReplaceDlg = bReplaceDlg;

	if (!::IsWindow(pEditView->GetSafeHwnd()))
		return FALSE;

	// already created, bring to foreground.
	if (::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd, SW_RESTORE);
		::BringWindowToTop(m_hWnd);
		::SetForegroundWindow(m_hWnd);
	}
	else
	{
		UINT uID = bReplaceDlg? XTP_IDD_EDIT_SEARCH_REPLACE: XTP_IDD_EDIT_SEARCH_FIND;
		if (!CreateIndirect(XTPResourceManager()->LoadDialogTemplate2(MAKEINTRESOURCE(uID)), pEditView))
		{
			TRACE0("Error creating find replace dialog.\n");
			return FALSE;
		}

		LoadHistory();
		UpdateData(FALSE);
		EnableControls();

		m_btnRadioUp.SetCheck(m_nSearchDirection == 0);
		m_btnRadioDown.SetCheck(m_nSearchDirection == 1);

		CXTPWindowRect rc(this);
		::MoveWindow(m_hWnd, m_ptWndPos.x, m_ptWndPos.y, rc.Width(), rc.Height(), FALSE);
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	m_wndFindCombo.SetFocus();
	m_wndFindCombo.SetEditSel(0,-1);

	return TRUE;
}

void CXTPSyntaxEditFindReplaceDlg::EnableControls()
{
	m_btnFindNext.EnableWindow(
		!m_csFindText.IsEmpty());

	if (m_bReplaceDlg)
	{
		m_btnReplace.EnableWindow(
			!m_csReplaceText.IsEmpty());

		m_btnReplaceAll.EnableWindow(
			!m_csReplaceText.IsEmpty());
	}
}

void CXTPSyntaxEditFindReplaceDlg::OnCancel()
{
	SaveHistory();

	CDialog::OnCancel();

	if (::IsWindow(m_hWnd))
		::DestroyWindow(m_hWnd);
}

BOOL CXTPSyntaxEditFindReplaceDlg::LoadHistory()
{
	CWinApp* pWinApp = AfxGetApp();
	if (!pWinApp)
		return FALSE;

	m_bMatchWholeWord = pWinApp->GetProfileInt(
		XTP_EDIT_REG_SETTINGS, XTP_EDIT_FINDDLG_MATCHWHOLEWORD, m_bMatchWholeWord);

	m_bMatchCase = pWinApp->GetProfileInt(
		XTP_EDIT_REG_SETTINGS, XTP_EDIT_FINDDLG_MATCHCASE, m_bMatchCase);

	m_nSearchDirection = pWinApp->GetProfileInt(
		XTP_EDIT_REG_SETTINGS, XTP_EDIT_FINDDLG_SEARCHDIRECTION, m_nSearchDirection);

	if (::IsWindow(m_wndFindCombo.m_hWnd))
	{
		int nCount = pWinApp->GetProfileInt(
			XTP_EDIT_REG_SETTINGS, XTP_EDIT_FINDDLG_FINDHISTORYCOUNT, 0);

		for (int i = 0; i < nCount; i++)
		{
			CString csEntry;
			csEntry.Format(XTP_EDIT_FINDDLG_FINDHISTORYITEM, i);

			CString csValue;
			csValue = pWinApp->GetProfileString(XTP_EDIT_REG_SETTINGS, csEntry);

			if (!csValue.IsEmpty())
			{
				if (m_wndFindCombo.FindStringExact(-1, csValue) == CB_ERR)
				{
					m_wndFindCombo.AddString(csValue);
				}
			}
		}
	}

	if (::IsWindow(m_wndReplaceCombo.m_hWnd))
	{
		int nCount = pWinApp->GetProfileInt(
			XTP_EDIT_REG_SETTINGS, XTP_EDIT_FINDDLG_REPLACEHISTORYCOUNT, 0);

		for (int i = 0; i < nCount; i++)
		{
			CString csEntry;
			csEntry.Format(XTP_EDIT_FINDDLG_REPLACEHISTORYITEM, i);

			CString csValue;
			csValue = pWinApp->GetProfileString(XTP_EDIT_REG_SETTINGS, csEntry);

			if (!csValue.IsEmpty())
			{
				if (m_wndReplaceCombo.FindStringExact(-1, csValue) == CB_ERR)
				{
					m_wndReplaceCombo.AddString(csValue);
				}
			}
		}
	}

	if (::IsWindow(m_hWnd))
	{
		if ((m_ptWndPos.x == -1 || m_ptWndPos.y == -1))
		{
			CXTPWindowRect r1(this);
			CXTPWindowRect r2(m_pEditView);

			m_ptWndPos.x = r2.left + ((r2.Width()-r1.Width())/2);
			m_ptWndPos.y = r2.top + ((r2.Height()-r1.Height())/2);
		}
	}

	return TRUE;
}

BOOL CXTPSyntaxEditFindReplaceDlg::SaveHistory()
{
	CWinApp* pWinApp = AfxGetApp();
	if (!pWinApp)
		return FALSE;

	pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS,
		XTP_EDIT_FINDDLG_MATCHWHOLEWORD, m_bMatchWholeWord);

	pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS,
		XTP_EDIT_FINDDLG_MATCHCASE, m_bMatchCase);

	pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS,
		XTP_EDIT_FINDDLG_SEARCHDIRECTION, m_nSearchDirection);

	if (::IsWindow(m_wndFindCombo.m_hWnd))
	{
		int nCount = m_wndFindCombo.GetCount();
		pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS,
			XTP_EDIT_FINDDLG_FINDHISTORYCOUNT, nCount);

		for (int i = 0; i < nCount; i++)
		{
			CString csEntry;
			csEntry.Format(XTP_EDIT_FINDDLG_FINDHISTORYITEM, i);

			CString csValue;
			m_wndFindCombo.GetLBText(i, csValue);

			pWinApp->WriteProfileString(XTP_EDIT_REG_SETTINGS,
				csEntry, csValue);
		}
	}

	if (::IsWindow(m_wndReplaceCombo.m_hWnd))
	{
		int nCount = m_wndReplaceCombo.GetCount();
		pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS,
			XTP_EDIT_FINDDLG_REPLACEHISTORYCOUNT, nCount);

		for (int i = 0; i < nCount; i++)
		{
			CString csEntry;
			csEntry.Format(XTP_EDIT_FINDDLG_REPLACEHISTORYITEM, i);

			CString csValue;
			m_wndReplaceCombo.GetLBText(i, csValue);

			pWinApp->WriteProfileString(XTP_EDIT_REG_SETTINGS,
				csEntry, csValue);
		}
	}

	if (::IsWindow(m_hWnd))
	{
		CXTPWindowRect rc(this);
		m_ptWndPos = rc.TopLeft();
	}

	return TRUE;
}

void CXTPSyntaxEditFindReplaceDlg::OnEditChangeComboFind()
{
	UpdateData();
	EnableControls();
}

void CXTPSyntaxEditFindReplaceDlg::OnSelendOkComboFind()
{
	m_wndFindCombo.GetLBText(
		m_wndFindCombo.GetCurSel(), m_csFindText);

	EnableControls();
}

void CXTPSyntaxEditFindReplaceDlg::OnEditChangeComboReplace()
{
	UpdateData();
	EnableControls();
}

void CXTPSyntaxEditFindReplaceDlg::OnSelendOkComboReplace()
{
	m_wndReplaceCombo.GetLBText(
		m_wndReplaceCombo.GetCurSel(), m_csReplaceText);

	EnableControls();
}

void CXTPSyntaxEditFindReplaceDlg::OnChkMatchWholeWord()
{
	m_bMatchWholeWord = !m_bMatchWholeWord;
}

void CXTPSyntaxEditFindReplaceDlg::OnChkMatchCase()
{
	m_bMatchCase = !m_bMatchCase;
}

void CXTPSyntaxEditFindReplaceDlg::OnRadioUp()
{
	m_nSearchDirection = 0;
}

void CXTPSyntaxEditFindReplaceDlg::OnRadioDown()
{
	m_nSearchDirection = 1;
}

void CXTPSyntaxEditFindReplaceDlg::OnBtnFindNext()
{
	if (::IsWindow(m_pEditView->GetSafeHwnd()))
	{
		BOOL bFound = m_pEditView->GetEditCtrl().Find(m_csFindText,
			m_bMatchWholeWord, m_bMatchCase, m_nSearchDirection);

		if (bFound)
		{
			m_pEditView->SetDirty();
		}
		else
		{
			AfxMessageBox(XTPResourceManager()->LoadString(XTP_IDS_EDIT_MSG_FSEARCH));
		}

		if (m_wndFindCombo.FindStringExact(-1, m_csFindText) == CB_ERR)
		{
			m_wndFindCombo.AddString(m_csFindText);
		}
	}
}

void CXTPSyntaxEditFindReplaceDlg::OnBtnReplace()
{
	CWaitCursor wait;

	if (::IsWindow(m_pEditView->GetSafeHwnd()))
	{
		CString csSelectedText;
		m_pEditView->GetEditCtrl().GetSelectionText(csSelectedText);

		csSelectedText = csSelectedText.SpanExcluding(_T("\r\n"));
		REPLACE_S(csSelectedText, _T("\t"), _T("    "));

		BOOL bFoundText = FALSE;
		if (m_bMatchCase)
		{
			bFoundText = (csSelectedText.Compare(m_csFindText) == 0);
		}
		else
		{
			bFoundText = (csSelectedText.CompareNoCase(m_csFindText) == 0);
		}

		BOOL bFoundTextPrior = bFoundText;
		if (!bFoundText)
		{
			bFoundText = m_pEditView->GetEditCtrl().Find(
				m_csFindText, m_bMatchWholeWord, m_bMatchCase, m_nSearchDirection);
		}

		if (!bFoundText)
		{
			AfxMessageBox(XTPResourceManager()->LoadString(XTP_IDS_EDIT_MSG_FSEARCH));
		}
		else
		{
			if (bFoundTextPrior)
			{
				m_pEditView->GetEditCtrl().ReplaceSel(m_csReplaceText);

				bFoundText = m_pEditView->GetEditCtrl().Find(m_csFindText,
					m_bMatchWholeWord, m_bMatchCase, m_nSearchDirection);

				if (!bFoundText)
				{
					AfxMessageBox(XTPResourceManager()->LoadString(XTP_IDS_EDIT_MSG_FSEARCH));
				}
			}
			m_pEditView->SetDirty();
		}

		if (m_wndReplaceCombo.FindStringExact(-1, m_csReplaceText) == CB_ERR)
		{
			m_wndReplaceCombo.AddString(m_csReplaceText);
		}
	}
}

void CXTPSyntaxEditFindReplaceDlg::OnBtnReplaceAll()
{
	CWaitCursor wait;

	if (::IsWindow(m_pEditView->GetSafeHwnd()))
	{
		int nMatchFound = m_pEditView->ReplaceAll(
			m_csFindText, m_csReplaceText, m_bMatchWholeWord, m_bMatchCase);

		m_pEditView->SetDirty();

		CString csMatchFound;
		csMatchFound.Format(XTPResourceManager()->LoadString(XTP_IDS_EDIT_MSG_FREPLACED), nMatchFound);

		AfxMessageBox(csMatchFound);

		if (m_wndReplaceCombo.FindStringExact(-1, m_csReplaceText) == CB_ERR)
		{
			m_wndReplaceCombo.AddString(m_csReplaceText);
		}
	}
}

BOOL CXTPSyntaxEditFindReplaceDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		if (!m_csReplaceText.IsEmpty())
		{
			OnBtnReplace();
		}
		else if (!m_csFindText.IsEmpty())
		{
			OnBtnFindNext();
		}

		return TRUE; // don't close dialog.
	}

	return CDialog::PreTranslateMessage(pMsg);
}
