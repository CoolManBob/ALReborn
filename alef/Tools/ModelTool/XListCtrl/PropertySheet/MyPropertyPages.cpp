// MyPropertyPages.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyPropertyPages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMyPropertyPage1, CPropertyPage)
IMPLEMENT_DYNCREATE(CMyPropertyPage2, CPropertyPage)
IMPLEMENT_DYNCREATE(CMyPropertyPage3, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage1 property page

CMyPropertyPage1::CMyPropertyPage1() : CPropertyPage(CMyPropertyPage1::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage1)
	//}}AFX_DATA_INIT
}

CMyPropertyPage1::~CMyPropertyPage1()
{
}

void CMyPropertyPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPropertyPage1)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyPropertyPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CMyPropertyPage1)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CMyPropertyPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	InitListCtrl(&m_List);
	FillListCtrl(&m_List);
	return TRUE;
}

BOOL CMyPropertyPage1::OnSetActive() 
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

void CMyPropertyPage1::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (!::IsWindow(m_List.m_hWnd))
		return;

	m_List.LockWindowUpdate();

	m_List.MoveWindow(0, 0, cx, cy);

	CRect rect;
	m_List.GetClientRect(&rect);

	int w = rect.Width();

	int total_cx = 0;
	int n = m_List.GetColumns();
	int nPad = m_List.GetCellPadding();
	w = w - n * 2 * nPad;
	int nBar = ::GetSystemMetrics(SM_CXVSCROLL);

	// adjust columns
	for (int i = 0; i < n; i++)
	{
		int colwidth = (i == (n - 1)) ? w - total_cx - nBar : (w * m_nColWidths[i]) / 64;
		total_cx += colwidth;
		m_List.SetColumnWidth(i, colwidth);
	}

	m_List.UnlockWindowUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage2 property page

BEGIN_MESSAGE_MAP(CMyPropertyPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CMyPropertyPage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMyPropertyPage2::CMyPropertyPage2() : CPropertyPage(CMyPropertyPage2::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage2)
	//}}AFX_DATA_INIT
}

CMyPropertyPage2::~CMyPropertyPage2()
{
}

void CMyPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPropertyPage2)
	//}}AFX_DATA_MAP
}

BOOL CMyPropertyPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	return TRUE;
}

BOOL CMyPropertyPage2::OnSetActive() 
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

/////////////////////////////////////////////////////////////////////////////
// CMyPropertyPage3 property page

BEGIN_MESSAGE_MAP(CMyPropertyPage3, CPropertyPage)
	//{{AFX_MSG_MAP(CMyPropertyPage3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMyPropertyPage3::CMyPropertyPage3() : CPropertyPage(CMyPropertyPage3::IDD)
{
	//{{AFX_DATA_INIT(CMyPropertyPage3)
	//}}AFX_DATA_INIT
}

CMyPropertyPage3::~CMyPropertyPage3()
{
}

void CMyPropertyPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyPropertyPage3)
	//}}AFX_DATA_MAP
}

BOOL CMyPropertyPage3::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	return TRUE;
}

BOOL CMyPropertyPage3::OnSetActive() 
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	return CPropertyPage::OnSetActive();
}
