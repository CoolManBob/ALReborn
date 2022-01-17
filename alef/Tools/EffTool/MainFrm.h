// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__5C95ED61_C56F_439D_8DDD_1F036F74F22F__INCLUDED_)
#define AFX_MAINFRM_H__5C95ED61_C56F_439D_8DDD_1F036F74F22F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Dlg/DlgListEffSet.h"
#include "./Dlg/DlgTreeEffSet.h"
#include "./Dlg/DlgStaticPropGrid.h"
#include "./Dlg/DlgEdit.h"

class AgcdEffSet;
class CMainFrame : public CXTPMDIFrameWnd//CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)

public:
	static CMainFrame*	PTHIS;
	static CMainFrame* bGetThis() { return PTHIS; };

public:
	CXTPDockingPaneManager* GetDockingPaneManager()		{	return &m_paneManager;		}

	CXTPDockingPaneManager	m_paneManager;
	CXTPTabClientWnd		m_MTIClientWnd;
	CXTPDockingPaneLayout*	m_pPreviewLayout;
	//Attributes

public:
	CDlgGridToolOption		m_dlgGridOption;
	CDlgGridEffSet			m_dlgGridEffSet;
	CDlgListEffSet			m_dlgListEffSet;
	CDlgTreeEffSet			m_dlgTreeEffSet;
	CDlgEdit				m_dlgEditEffTxt;
	CDlgEdit				m_dlgEditOutput;

	BOOL	ShowWindowEx(int nCmdShow);
	
private:
	CWnd*	CWndListEffSet(void);
	CWnd*	CWndTreeEffSet(void);
	CWnd*	CWndPropEffSet(void);
	CWnd*	CWndPropOption(void);
	CWnd*	CWndEditEffTxt(void);
	CWnd*	CWndEditOutput(void);

public:
	void	bOutputWnd(LPCTSTR strInfo);
	INT		bInsEffSetFromListToTree(AgcdEffSet* pEffSet);

public:
	CMainFrame();
	virtual ~CMainFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState);
	//}}AFX_VIRTUAL

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnEmptyCommand();

	LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__5C95ED61_C56F_439D_8DDD_1F036F74F22F__INCLUDED_)
