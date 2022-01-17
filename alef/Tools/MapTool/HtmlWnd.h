#if !defined(AFX_HTMLWND_H__BC996F72_63B6_40FE_89C0_866005B5FCA4__INCLUDED_)
#define AFX_HTMLWND_H__BC996F72_63B6_40FE_89C0_866005B5FCA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHtmlWnd window

class CHtmlWnd : public CWnd
{
protected:
	CWnd			m_wndBrowser	;
	IWebBrowser2*	m_pBrowserApp	;

// Construction
public:
	CHtmlWnd();

// Attributes
public:

// Operations
public:
	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	void Navigate(LPCTSTR URL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL,
		LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
		CByteArray& baPostedData,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);
	void Refresh();
	void Refresh2(int nLevel);
	void Stop();
	void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
	void PutProperty(LPCTSTR lpszPropertyName, double dValue);
	void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
	void PutProperty(LPCTSTR lpszPropertyName, long lValue);
	void PutProperty(LPCTSTR lpszPropertyName, short nValue);
	BOOL GetProperty(LPCTSTR lpszProperty, CString& strValue);
	COleVariant GetProperty(LPCTSTR lpszProperty);
	void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn,
		VARIANT* pvaOut);
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHtmlWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLWND_H__BC996F72_63B6_40FE_89C0_866005B5FCA4__INCLUDED_)
