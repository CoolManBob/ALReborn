// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__138E41E8_F2FA_4C09_8C27_755419B8FD85__INCLUDED_)
#define AFX_MAINFRM_H__138E41E8_F2FA_4C09_8C27_755419B8FD85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildView.h"
#include "UIListView.h"

#define	UIT_UILIST_WIDTH	150

typedef enum
{
	UIT_VIEW_WINDOW	= 0,
	UIT_VIEW_1024_768,
	UIT_VIEW_1280_1024,
	UIT_VIEW_1600_1200,
	UIT_MAX_VIEW
} UITView;

extern CSize	g_acsViewSize[];

class CMainFrame : public CFrameWnd
{
public:
	void UpdateUIList();

public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

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

public:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;
	CToolBar		m_wndToolBarControl;
	CChildView		m_wndView;
	CUIListView *	m_pwndUIList;

	UITView			m_eView;

	BOOL			m_bConnected;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnNewUI();
	afx_msg void OnControlButton();
	afx_msg void OnControlCombo();
	afx_msg void OnControlEdit();
	afx_msg void OnControlList();
	afx_msg void OnControlStatic();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUITSave();
	afx_msg void OnUITLoad();
	afx_msg void OnModeEdit();
	afx_msg void OnModeRun();
	afx_msg void OnUITEditEvent();
	afx_msg void OnModeArea();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnControlGrid();
	afx_msg void OnControlBar();
	afx_msg void OnConnectServer();
	afx_msg void OnUpdateModeArea(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModeRun(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConnectServer(CCmdUI* pCmdUI);
	afx_msg void OnControlSkilltree();
	afx_msg void OnUITEditHotkey();
	afx_msg void OnUITModeViewGrid();
	afx_msg void OnControlScroll();
	afx_msg void OnControlCustom();
	afx_msg void OnSetUIMode();
	afx_msg void OnControlTree();
	afx_msg void OnControlClock();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnView1024();
	afx_msg void OnView1280();
	afx_msg void OnViewFittowindowsize();
	afx_msg void OnView1600();
	afx_msg void OnUpdateViewFittowindowsize(CCmdUI *pCmdUI);
	afx_msg void OnUpdateView1024(CCmdUI *pCmdUI);
	afx_msg void OnUpdateView1280(CCmdUI *pCmdUI);
	afx_msg void OnUpdateView1600(CCmdUI *pCmdUI);
	afx_msg void OnUitCopyUi();
};

extern CMainFrame *	g_pMainFrame;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__138E41E8_F2FA_4C09_8C27_755419B8FD85__INCLUDED_)
