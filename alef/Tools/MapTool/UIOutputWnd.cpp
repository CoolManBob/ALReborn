// UIOutputWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "UIOutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUIOutputWnd
#define	MESSAGE_COUNT_LOW_LIMIT	12

CUIOutputWnd::CUIOutputWnd()
{
	m_nMaxMessage = MESSAGE_COUNT_LOW_LIMIT ;
}

CUIOutputWnd::~CUIOutputWnd()
{
}


BEGIN_MESSAGE_MAP(CUIOutputWnd, CWnd)
	//{{AFX_MSG_MAP(CUIOutputWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUIOutputWnd message handlers

void CUIOutputWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	GetClientRect( rc );
	CBrush	brush;
	brush.CreateSolidBrush( RGB( 255 , 255 , 255 ) );
	dc.SelectObject( brush );
	dc.Rectangle( rc );

	AuNode< stMessage > *pNode;
	stMessage	*pMessage;
	int i = 0 ;
	for( pNode = m_listMessage.GetTailNode(); pNode != NULL; )
	{
		pMessage = &pNode->GetData();

		dc.SetTextColor( pMessage->color );
		dc.TextOut( 3 , rc.bottom - m_cyChar * ++i - 3 , pMessage->message );

		m_listMessage.GetPrev( pNode );
	}		
}

int CUIOutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetUp();

	AddMessage( "Output Window 창 메시지 출력 초기화 완료" );
	
	return 0;
}

void CUIOutputWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	SetUp();
}

void CUIOutputWnd::AddMessage( CString str , COLORREF color )
{
	stMessage	message;
	message.message	= str	;
	message.color	= color	;
	m_listMessage.AddTail( message );
	if( m_listMessage.GetCount() > m_nMaxMessage ) //MAX 메시지 
		m_listMessage.RemoveHead();
		
	RedrawWindow();
}

void CUIOutputWnd::SetUp()
{
	CDC *pDC = GetDC();

	TEXTMETRIC	tm;
	pDC->GetTextMetrics( & tm );

	m_cyChar = tm.tmHeight;

	CRect	rc;
	GetClientRect( rc );

	m_nMaxMessage = rc.Height() / m_cyChar;
	if( m_nMaxMessage < MESSAGE_COUNT_LOW_LIMIT )
		m_nMaxMessage = MESSAGE_COUNT_LOW_LIMIT;

	ReleaseDC( pDC );
}
