// FindFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmfilelistdlg.h"
#include "FindFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindFileDlg dialog


CFindFileDlg::CFindFileDlg(CHAR *pszDest, CWnd* pParent /*=NULL*/)
	: CDialog(CFindFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindFileDlg)
	m_strFindFile = _T("");
	//}}AFX_DATA_INIT

	m_pszDest = pszDest;
}


void CFindFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindFileDlg)
	DDX_Text(pDX, IDC_EDIT_FIND, m_strFindFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindFileDlg, CDialog)
	//{{AFX_MSG_MAP(CFindFileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFileDlg message handlers

void CFindFileDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if(m_pszDest)
	{
		strcpy(m_pszDest, (LPSTR)(LPCSTR)(m_strFindFile));
	}

	CDialog::OnOK();
}
