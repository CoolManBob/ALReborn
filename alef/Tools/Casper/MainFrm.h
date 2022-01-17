// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__17697086_7671_495B_BA54_ED303619F897__INCLUDED_)
#define AFX_MAINFRM_H__17697086_7671_495B_BA54_ED303619F897__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "UserListView.h"

#define	UIT_UILIST_WIDTH	200

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:
	BOOL SetTestEnv();
	void UpdateUserList(BOOL bBottom = FALSE);
	void ConnectToServer();
	void AdduserList(LPCTSTR lpCharName);
	
	void TimerSequenceLogin();
	void TimerTestAction();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CChildView    m_wndView;
	CUserListView *m_pwndUserList;

public:
	BOOL			m_bConnected;
	CString			m_strHigherID;
	INT32			m_lBeginID;
	INT32			m_lEndID;
	INT32			m_lCurrent;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnLoginSequence();
	afx_msg void OnRenderingToggle();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUserListRefresh();
	afx_msg void OnTestAction();
	afx_msg void OnLoginConcurrency();
	afx_msg void OnTestActionStop();
	afx_msg void OnLoginDirect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CMainFrame *	g_pMainFrame;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__17697086_7671_495B_BA54_ED303619F897__INCLUDED_)
