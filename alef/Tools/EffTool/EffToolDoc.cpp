// EffToolDoc.cpp : implementation of the CEffToolDoc class
//

#include "stdafx.h"
#include "EffTool.h"

#include "EffToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffToolDoc

IMPLEMENT_DYNCREATE(CEffToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffToolDoc, CDocument)
	//{{AFX_MSG_MAP(CEffToolDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffToolDoc construction/destruction

CEffToolDoc::CEffToolDoc()
{
	// TODO: add one-time construction code here

}

CEffToolDoc::~CEffToolDoc()
{
}

BOOL CEffToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEffToolDoc serialization

void CEffToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffToolDoc diagnostics

#ifdef _DEBUG
void CEffToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffToolDoc commands
