// View.h : interface of the CMDIView class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_H
#define VIEW_H

#include "XListCtrl.h"

class CMDIDoc;

class CMDIView : public CView
{
protected: // create from serialization only
	CMDIView();
	DECLARE_DYNCREATE(CMDIView)

// Attributes
public:
	CMDIDoc* GetDocument();
	CXListCtrl * m_pList;
	CStringArray m_sa1, m_sa2;
	static int m_nColWidths[];

// Operations
public:
	void FillListCtrl(CXListCtrl * pList);
	void InitListCtrl(CXListCtrl * pList);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMDIView)
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMDIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMDIView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in View.cpp
inline CMDIDoc* CMDIView::GetDocument()
   { return (CMDIDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //VIEW_H
