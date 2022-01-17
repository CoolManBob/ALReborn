// XTPSyntaxEditDoc.cpp : implementation file
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
#include "Common/XTPImageManager.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPNotifyConnection.h"
#include "Common/XTPSmartPtrInternalT.h"

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

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditDoc

IMPLEMENT_DYNCREATE(CXTPSyntaxEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CXTPSyntaxEditDoc, CDocument)
	//{{AFX_MSG_MAP(CXTPSyntaxEditDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXTPSyntaxEditDoc::CXTPSyntaxEditDoc()
: m_bAutoReload(FALSE)
, m_bNewFile(TRUE)
, m_bOpened(FALSE)
, m_bDocCreated(FALSE)
, m_iTopRow(1)
, m_iHScrollPos(0)
, m_bReloading(FALSE)
{
	::ZeroMemory(&m_curFileData, sizeof(WIN32_FIND_DATA));

	CWinApp* pWinApp = AfxGetApp();
	if (pWinApp != NULL)
	{
		m_bAutoReload = pWinApp->GetProfileInt(
			XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_AUTORELOAD, m_bAutoReload);
	}
}

CXTPSyntaxEditDoc::~CXTPSyntaxEditDoc()
{
}

BOOL CXTPSyntaxEditDoc::IsReadonly()
{
	return ((m_curFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY);
}

BOOL CXTPSyntaxEditDoc::SaveFileData(LPCTSTR lpszPathName)
{
	HANDLE hFind = ::FindFirstFile(lpszPathName, &m_curFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	::FindClose(hFind);

//  if (IsReadonly())
//  {
//      TRACE(_T("Cannot write to a read only file"));
//      return FALSE;
//  }

	return TRUE;
}

BOOL CXTPSyntaxEditDoc::OnNewDocument()
{
	CWaitCursor wait;

	if (!CDocument::OnNewDocument())
	{
		return FALSE;
	}
	wait.Restore();

	m_bOpened = FALSE;
	m_bNewFile = TRUE;
	m_bDocCreated = TRUE;

	::ZeroMemory(&m_curFileData, sizeof(WIN32_FIND_DATA));

	if (m_ptrDataMan)
	{
		m_ptrDataMan->InsertText(_T(""), 1,1, FALSE);
	}

	UpdateAllViews(NULL, xtpEditHintRefreshView);

	return TRUE;
}

BOOL CXTPSyntaxEditDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;

	m_bNewFile = FALSE;

	BOOL bRes = CDocument::OnOpenDocument(lpszPathName);
	wait.Restore();
	if (bRes)
	{
		UpdateAllViews(NULL, xtpEditHintRefreshView);
	}

	if (!SaveFileData(lpszPathName))
		return FALSE;

	return bRes;
}

BOOL CXTPSyntaxEditDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CWaitCursor wait;

	if (!CDocument::OnSaveDocument(lpszPathName))
		return FALSE;

	if (!SaveFileData(lpszPathName))
		return FALSE;

	UpdateAllViews(NULL, xtpEditHintRefreshView);

	return TRUE;
}

BOOL CXTPSyntaxEditDoc::ReOpen()
{
	return OnOpenDocument(GetPathName());
}

void CXTPSyntaxEditDoc::OnCloseDocument()
{
	CWaitCursor wait;

	if (m_ptrDataMan)
	{
		m_ptrDataMan->Close();
	}

	CDocument::OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditDoc serialization

void CXTPSyntaxEditDoc::Serialize(CArchive& ar)
{
	if (m_ptrDataMan)
	{
		CWaitCursor wait;
		m_ptrDataMan->Serialize(ar);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditDoc diagnostics

#ifdef _DEBUG
void CXTPSyntaxEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CXTPSyntaxEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXTPSyntaxEditDoc commands

void CXTPSyntaxEditDoc::OnChangedViewList()
{
	if (!m_bDocCreated)
		return;

	int iTopRow = 1;
	BOOL bFirst = TRUE;
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CXTPSyntaxEditView *pView = (CXTPSyntaxEditView *)GetNextView(pos);

		if (!pView->IsKindOf(RUNTIME_CLASS(CXTPSyntaxEditView)))
			continue;

		if (bFirst)
		{
			iTopRow = pView->GetTopRow();
			bFirst = FALSE;
		}
		else
		{
			pView->SetTopRow(iTopRow);
		}
	}

	CDocument::OnChangedViewList();
}


CXTPSyntaxEditBufferManager * CXTPSyntaxEditDoc::GetDataManager()
{
	return m_ptrDataMan;
}

void CXTPSyntaxEditDoc::SetDataManager(CXTPSyntaxEditBufferManager *pDataMan)
{
	m_ptrDataMan.SetPtr(pDataMan, TRUE);
}

void CXTPSyntaxEditDoc::SetConfigFile(LPCTSTR szPath)
{
	if (m_ptrDataMan)
	{
		m_ptrDataMan->SetConfigFile(szPath);
	}
}

BOOL CXTPSyntaxEditDoc::CheckFileModified(LPCTSTR lpszPathName)
{
	if (!GetAutoReload())
		return FALSE;

	WIN32_FIND_DATA newFileData;
	HANDLE hFound = ::FindFirstFile(lpszPathName, &newFileData);
	if (hFound != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFound);

		// If file attributes have changed or file write time has changed, reload file.
		if (!m_bReloading && (
			(m_curFileData.ftLastWriteTime.dwLowDateTime != newFileData.ftLastWriteTime.dwLowDateTime) ||
			(m_curFileData.ftLastWriteTime.dwHighDateTime != newFileData.ftLastWriteTime.dwHighDateTime) ||
			(m_curFileData.dwFileAttributes != newFileData.dwFileAttributes)))
		{
			m_bReloading = TRUE;

			CString csWarning;
			csWarning.Format(XTP_IDS_EDIT_MSG_WARN_RELOAD, lpszPathName);
			if (AfxMessageBox(csWarning, MB_YESNO|MB_ICONQUESTION) == IDNO)
			{
				m_bReloading = FALSE;
				return FALSE;
			}

			if (ReOpen())
			{
				::memcpy(&m_curFileData, &newFileData, sizeof(WIN32_FIND_DATA));
				m_bReloading = FALSE;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CXTPSyntaxEditDoc::GetAutoReload() const
{
	return m_bAutoReload;
}

BOOL CXTPSyntaxEditDoc::SetAutoReload(BOOL bAutoReload, BOOL bUpdateReg/*=FALSE*/)
{
	m_bAutoReload = bAutoReload;

	if (bUpdateReg)
	{
		CWinApp* pWinApp = AfxGetApp();
		if (pWinApp != NULL)
		{
			if (pWinApp->WriteProfileInt(XTP_EDIT_REG_SETTINGS, XTP_EDIT_REG_AUTORELOAD, (int)bAutoReload))
				return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

CXTPSyntaxEditView* CXTPSyntaxEditDoc::GetFirstView()
{
	POSITION pos = GetFirstViewPosition();
	return DYNAMIC_DOWNCAST(CXTPSyntaxEditView, GetNextView(pos));
}
