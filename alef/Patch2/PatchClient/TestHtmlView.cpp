// TestHtmlView.cpp : implementation file
//

#include "stdafx.h"
//#include "PatchClientDlg.h"
#include "TestHtmlView.h"
#include ".\testhtmlview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestHtmlView

IMPLEMENT_DYNCREATE(CTestHtmlView, CHtmlView)

CTestHtmlView::CTestHtmlView()
{
	//{{AFX_DATA_INIT(CTestHtmlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_dwTried = 0;
	m_dwFailed = 0;
	m_nNumFailed = 0;
}

CTestHtmlView::~CTestHtmlView()
{
}

void CTestHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestHtmlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CTestHtmlView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestHtmlView diagnostics

#ifdef _DEBUG
void CTestHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CTestHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestHtmlView message handlers

void CTestHtmlView::OnDownloadComplete()
{
	CHtmlView::OnDownloadComplete();
}

void CTestHtmlView::OnDocumentComplete(LPCTSTR lpszURL)
{
	CHtmlView::OnDocumentComplete(lpszURL);

	if (m_dwTried < m_dwFailed)
	{
		if (m_nNumFailed < 10)
			Navigate(lpszURL,0,NULL,NULL);
	}
	else
		ShowWindow(SW_SHOW);
}

void CTestHtmlView::OnNavigateError(LPCTSTR lpszURL, LPCTSTR lpszFrame, DWORD dwError, BOOL *pbCancel)
{
	++m_nNumFailed;
	m_dwFailed = GetTickCount();

	ShowWindow(SW_HIDE);

	CHtmlView::OnNavigateError(lpszURL, lpszFrame, dwError, pbCancel);
}

void CTestHtmlView::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel)
{
	m_dwTried = GetTickCount();

	CHtmlView::OnBeforeNavigate2(lpszURL, nFlags, lpszTargetFrameName, baPostedData, lpszHeaders, pbCancel);
}

void CTestHtmlView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	SIZE		Size;

	Size.cx		=	0;
	Size.cy		=	0;

	SetScrollSizes( MM_TEXT , Size );

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

void CTestHtmlView::OnSize(UINT nType, int cx, int cy)
{

	CHtmlView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
