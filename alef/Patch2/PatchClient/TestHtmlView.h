#if !defined(AFX_TESTHTMLVIEW_H__A58E499E_A684_43D2_BEE4_61BC9E79ADA6__INCLUDED_)
#define AFX_TESTHTMLVIEW_H__A58E499E_A684_43D2_BEE4_61BC9E79ADA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestHtmlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTestHtmlView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

#include "winuser.h"

class CTestHtmlView : public CHtmlView
{
protected:
	CTestHtmlView();           // protected constructor used by dynamic creation
		virtual ~CTestHtmlView();
	DECLARE_DYNCREATE(CTestHtmlView)

// html Data
public:
	//{{AFX_DATA(CTestHtmlView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	int			m_nNumFailed;
	DWORD		m_dwTried;
	DWORD		m_dwFailed;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestHtmlView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CTestHtmlView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDownloadComplete();
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
public:
	virtual void OnInitialUpdate();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTHTMLVIEW_H__A58E499E_A684_43D2_BEE4_61BC9E79ADA6__INCLUDED_)
