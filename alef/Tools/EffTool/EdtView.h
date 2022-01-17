#if !defined(AFX_EDTVIEW_H__BF5244F6_6225_4E94_BF2E_F6127D5E0E4A__INCLUDED_)
#define AFX_EDTVIEW_H__BF5244F6_6225_4E94_BF2E_F6127D5E0E4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EdtView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEdtView view

class CEdtView : public CEditView
{
protected:
	CEdtView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEdtView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEdtView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEdtView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEdtView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDTVIEW_H__BF5244F6_6225_4E94_BF2E_F6127D5E0E4A__INCLUDED_)
