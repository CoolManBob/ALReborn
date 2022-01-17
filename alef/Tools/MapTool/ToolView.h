#if !defined(AFX_TOOLVIEW_H__5B687FBC_A08E_4985_9A79_529D49EF7F75__INCLUDED_)
#define AFX_TOOLVIEW_H__5B687FBC_A08E_4985_9A79_529D49EF7F75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolView view

class CToolView : public CView
{
protected:
	CToolView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CToolView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLVIEW_H__5B687FBC_A08E_4985_9A79_529D49EF7F75__INCLUDED_)
