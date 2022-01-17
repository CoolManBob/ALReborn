#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRenderView view

class CRenderView : public CView
{
protected:
	CRenderView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRenderView)

public:
	CRenderView(CWnd *pcsParent, RECT &stInitRect);

	BOOL	IsMouseLButton()	{	return m_bLButtonDown;	}
	BOOL	IsMouseRButton()	{	return m_bRButtonDown;	}
	BOOL	IsMouseMButton()	{	return m_bMButtonDown;	}
	
protected:
	VOID		InitializeMember();
	RwV2d		CountDelta(CPoint point, float fFactor = 0.5f);
	void		KeyDown( WPARAM wParam );

protected:
	BOOL	m_bLButtonDown;
	BOOL	m_bRButtonDown;
	BOOL	m_bMButtonDown;
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderView)
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderView();

protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
