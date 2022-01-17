// MyPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyPropertySheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet

IMPLEMENT_DYNAMIC(CMyPropertySheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CMyPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMyPropertySheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMyPropertySheet::CMyPropertySheet(CWnd* pWndParent)
	 : CPropertySheet(_T("XListCtrlTest"), pWndParent)
{
	AddPage(&m_Page1);
	AddPage(&m_Page2);
	AddPage(&m_Page3);
}

CMyPropertySheet::~CMyPropertySheet()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMyPropertySheet message handlers

BOOL CMyPropertySheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

	// ModifyStyleEx(WS_EX_DLGMODALFRAME, 0);	// enable sys menu and icon
	
	// set minimal size
	CRect rc;
	GetWindowRect(&rc);
	return TRUE;
}
