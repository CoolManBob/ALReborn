#if !defined(AFX_MAPDOCUMENT_H__372F2FC9_C46F_407D_8079_32F5B5D4644E__INCLUDED_)
#define AFX_MAPDOCUMENT_H__372F2FC9_C46F_407D_8079_32F5B5D4644E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapDocument.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapDocument document

class CMapDocument : public CDocument
{
protected:
	CMapDocument();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMapDocument)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapDocument)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMapDocument();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMapDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPDOCUMENT_H__372F2FC9_C46F_407D_8079_32F5B5D4644E__INCLUDED_)
