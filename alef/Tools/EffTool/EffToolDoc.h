// EffToolDoc.h : interface of the CEffToolDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFTOOLDOC_H__34B44981_6CCB_4782_9F79_08E6494E456D__INCLUDED_)
#define AFX_EFFTOOLDOC_H__34B44981_6CCB_4782_9F79_08E6494E456D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#pragma warning(disable: 4786)

class CEffToolDoc : public CDocument
{
protected: // create from serialization only
	CEffToolDoc();
	DECLARE_DYNCREATE(CEffToolDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffToolDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CEffToolDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFTOOLDOC_H__34B44981_6CCB_4782_9F79_08E6494E456D__INCLUDED_)
