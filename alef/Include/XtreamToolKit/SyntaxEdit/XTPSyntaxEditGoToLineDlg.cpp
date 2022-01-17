// XTPSyntaxEditGoToLineDlg.cpp : implementation file
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
#include "XTPSyntaxEditCtrl.h"
#include "XTPSyntaxEditGoToLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
// CXTPSyntaxEditGoToLineDlg
//===========================================================================

CXTPSyntaxEditGoToLineDlg::CXTPSyntaxEditGoToLineDlg(CWnd* pParent /*=NULL*/)
: //CDialog(CXTPSyntaxEditGoToLineDlg::IDD, pParent),
m_pEditCtrl(NULL)
, m_iMaxLineNo(0)
, m_ptWndPos(CPoint(-1,-1))
, m_bHideOnFind(TRUE)
, m_bSelectLine(FALSE)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(IDD), pParent);

	//{{AFX_DATA_INIT(CXTPSyntaxEditGoToLineDlg)
	m_iLineNo = 0;
	m_csLineNo = _T("");
	//}}AFX_DATA_INIT
}

void CXTPSyntaxEditGoToLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXTPSyntaxEditGoToLineDlg)
	DDX_Control(pDX, XTP_IDC_EDIT_LINENO, m_wndEditLineNo);
	DDX_Text(pDX, XTP_IDC_EDIT_LINENO, m_iLineNo);
	DDX_Text(pDX, XTP_IDC_EDIT_TXT_LINENO, m_csLineNo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXTPSyntaxEditGoToLineDlg, CDialog)
	//{{AFX_MSG_MAP(CXTPSyntaxEditGoToLineDlg)
	ON_EN_CHANGE(XTP_IDC_EDIT_LINENO, OnChangeEditLineNo)
	ON_BN_CLICKED(XTP_IDC_EDIT_BTN_GOTO, OnBtnGoTo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditGoToLineDlg message handlers

void CXTPSyntaxEditGoToLineDlg::OnChangeEditLineNo()
{
	UpdateData();
}

BOOL CXTPSyntaxEditGoToLineDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	ASSERT_VALID(m_pEditCtrl);

	m_iMaxLineNo = m_pEditCtrl->GetRowCount();
	m_iLineNo    = m_pEditCtrl->GetCurRow();

	m_csLineNo.Format(XTPResourceManager()->LoadString(XTP_IDS_EDIT_LINENO), m_iMaxLineNo);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CXTPSyntaxEditGoToLineDlg::ShowDialog(CXTPSyntaxEditCtrl* pEditCtrl, BOOL bSelectLine /*=FALSE*/, BOOL bHideOnFind /*=FALSE*/)
{
	ASSERT_VALID(pEditCtrl);

	if (!::IsWindow(pEditCtrl->GetSafeHwnd()))
		return FALSE;

	m_pEditCtrl = pEditCtrl;
	m_bSelectLine = bSelectLine;
	m_bHideOnFind = bHideOnFind;

	// already created, bring to foreground.
	if (::IsWindow(m_hWnd))
	{
		::ShowWindow(m_hWnd, SW_RESTORE);
		::BringWindowToTop(m_hWnd);
		::SetForegroundWindow(m_hWnd);
	}
	else
	{
		if (!CreateIndirect(XTPResourceManager()->LoadDialogTemplate2(MAKEINTRESOURCE(IDD)), m_pEditCtrl))
		{
			TRACE0("Error creating go to dialog.\n");
			return FALSE;
		}

		UpdateData(FALSE);
		LoadPos();

		CXTPWindowRect rc(this);
		::MoveWindow(m_hWnd, m_ptWndPos.x, m_ptWndPos.y, rc.Width(), rc.Height(), FALSE);
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	m_wndEditLineNo.SetFocus();
	m_wndEditLineNo.SetSel(0,-1);

	return TRUE;
}

BOOL CXTPSyntaxEditGoToLineDlg::LoadPos()
{
	if (!::IsWindow(m_hWnd))
		return FALSE;

	if (m_ptWndPos.x != -1 && m_ptWndPos.y != -1)
		return FALSE;

	CXTPWindowRect r1(this);
	CXTPWindowRect r2(m_pEditCtrl);

	m_ptWndPos.x = r2.left + ((r2.Width()-r1.Width())/2);
	m_ptWndPos.y = r2.top + ((r2.Height()-r1.Height())/2);

	return TRUE;
}

BOOL CXTPSyntaxEditGoToLineDlg::SavePos()
{
	if (!::IsWindow(m_hWnd))
		return FALSE;

	CXTPWindowRect rc(this);
	m_ptWndPos = rc.TopLeft();

	return TRUE;
}

BOOL CXTPSyntaxEditGoToLineDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		OnBtnGoTo();
		return TRUE; // don't close dialog.
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CXTPSyntaxEditGoToLineDlg::OnBtnGoTo()
{
	if (m_iLineNo > m_iMaxLineNo)
	{
		m_iLineNo = m_iMaxLineNo;
		UpdateData(FALSE);
	}

	else if (m_iLineNo < 1)
	{
		m_iLineNo = 1;
		UpdateData(FALSE);
	}

	m_pEditCtrl->GoToRow(m_iLineNo, m_bSelectLine);

	if (!m_bHideOnFind && !m_bSelectLine)
		m_pEditCtrl->SetFocus();

	else if (m_bHideOnFind)
		OnCancel();

	else
		m_wndEditLineNo.SetSel(0,-1);
}

void CXTPSyntaxEditGoToLineDlg::OnCancel()
{
	SavePos();

	CDialog::OnCancel();

	if (::IsWindow(m_hWnd))
		::DestroyWindow(m_hWnd);
}
