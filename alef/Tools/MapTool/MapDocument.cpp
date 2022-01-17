// MapDocument.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "MapDocument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapDocument

IMPLEMENT_DYNCREATE(CMapDocument, CDocument)

CMapDocument::CMapDocument()
{
}

BOOL CMapDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CMapDocument::~CMapDocument()
{
}


BEGIN_MESSAGE_MAP(CMapDocument, CDocument)
	//{{AFX_MSG_MAP(CMapDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapDocument diagnostics

#ifdef _DEBUG
void CMapDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CMapDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapDocument serialization

void CMapDocument::Serialize(CArchive& ar)
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
// CMapDocument commands
