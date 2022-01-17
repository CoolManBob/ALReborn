#include "stdafx.h"

#include "CustomProgressBar.h"
#include ".\customprogressbar.h"

CCustomStatic::CCustomStatic()
{
	SetRect( &m_cRect, 0, 0, 0, 0 );
	memset( m_strText, 0, sizeof(m_strText) );
}

CCustomStatic::~CCustomStatic()
{
}

bool CCustomStatic::init( RECT *pcRect )
{
	m_cRect = *pcRect;

	return true;
}

bool CCustomStatic::SetText( wchar_t *pstrText, CWnd *pcWnd )
{	
	swprintf( m_strText, L"%s", pstrText );

	if( pcWnd != NULL )
	{
		DrawText( pcWnd->GetDC() );
		pcWnd->InvalidateRect( &m_cRect, FALSE );
//		pcWnd->Invalidate( FALSE );
	}

	return true;
}

wchar_t *CCustomStatic::GetText()
{
	return m_strText;
}

bool CCustomStatic::DrawText( CDC *pDC )
{	
	CDC					oriDC;
//	CBitmap				*poldoriDC;

	oriDC.CreateCompatibleDC(pDC);
//	poldoriDC = oriDC.SelectObject( NULL );

	pDC->SetTextAlign( TA_LEFT );
	pDC->SetBkMode( TRANSPARENT );

	pDC->SetTextColor( RGB( 0,0,0) );
	::TextOutW(pDC->m_hDC, m_cRect.top+1, m_cRect.left+1, m_strText, (int) wcslen(m_strText) );
	pDC->SetTextColor( RGB( 255,255,255) );
	::TextOutW(pDC->m_hDC, m_cRect.top, m_cRect.left, m_strText, (int) wcslen(m_strText) );

//	oriDC.SelectObject(poldoriDC);

	return true;
}

RECT *CCustomStatic::GetRect()
{
	return &m_cRect;
}

CCustomProgressCtrl::CCustomProgressCtrl()
{
	InitializeCriticalSection( &m_cCriticalSection );

	m_iMaxCount = 0;
	m_iCurrentCount = 0;
}

CCustomProgressCtrl::~CCustomProgressCtrl()
{
	DeleteCriticalSection( &m_cCriticalSection );
}

void CCustomProgressCtrl::lock()
{
	EnterCriticalSection( &m_cCriticalSection );
}

void CCustomProgressCtrl::unlock()
{
	LeaveCriticalSection( &m_cCriticalSection );
}

bool CCustomProgressCtrl::SetProgressRect( RECT *pcRect )
{
	m_cRect = *pcRect;

	return true;
}

void CCustomProgressCtrl::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here	
}

void CCustomProgressCtrl::LoadBitmap(CString strFileName)
{
	// 이미지를 불러온다.
	m_hBitmap = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		strFileName,
		IMAGE_BITMAP,0,0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	// 이미지의 크기를 구하기 위함..
	(CBitmap::FromHandle(m_hBitmap))->GetBitmap(&m_BmpInfo);
	
	m_nWidth		= m_BmpInfo.bmWidth;
	m_nSliceWidth	= (int)(m_BmpInfo.bmWidth/4);
	m_nHeight		= m_BmpInfo.bmHeight;


	m_nMode = 1;
}

bool CCustomProgressCtrl::Init( RECT *pcRect, int iMaxCount )
{
	SetProgressRect( pcRect );
	//LoadBitmap( strFileName );

	//. 2005. 10. 17. Nonstopdj
	//. 앞-중간-뒤 비트맵그림을 사용하지 않고 현재는 IDB_Progress2만 사용한다.
	//. 새로운 패치클라이언트 그림은 
	//m_cBMPProgress.LoadBitmap( IDB_Progress1 );
	//m_cBMPProgress1.LoadBitmap( IDB_Progress2 );
	//m_cBMPProgress2.LoadBitmap( IDB_Progress3 );
	m_cBMPProgress.LoadBitmap( IDB_Progress2 );
	m_cBMPProgress1.LoadBitmap( IDB_Progress2 );

	m_iMaxCount = iMaxCount;

	return true;
}

bool CCustomProgressCtrl::SetCurrentCount( int iCount )
{
	lock();

	m_iCurrentCount = iCount;
//	m_cProgressCtrl.SetPos( iCount );

	unlock();

	return true;
}

bool CCustomProgressCtrl::SetMaxCount( int iMaxCount )
{
	lock();

	m_iMaxCount = iMaxCount;
//	m_cProgressCtrl.SetRange32( 0, iMaxCount );

	unlock();

	return true;
}

int CCustomProgressCtrl::GetCurrentCount()
{
	int			iCurrentCount;

	lock();

	iCurrentCount = m_iCurrentCount;

	unlock();

	return iCurrentCount;
}

int CCustomProgressCtrl::GetMaxCount()
{
	int			iMaxCount;

	lock();

	iMaxCount = m_iMaxCount;

	unlock();

	return iMaxCount;
}

RECT *CCustomProgressCtrl::GetRect()
{
	return &m_cRect;
}

void CCustomProgressCtrl::Draw(CDC *pDC)
{	
	lock();

	CDC					oriDC;
	CBitmap				*poldoriDC;

	int					iCount;
	int					iPercent;
	int					iCurrentCount;
	int					iMaxCount;
	float				fDotPerPercent;

	iCurrentCount = GetCurrentCount();
	iMaxCount = GetMaxCount();

	oriDC.CreateCompatibleDC(pDC);
	poldoriDC = oriDC.SelectObject( &m_cBMPProgress );

	BITMAP			cBitmap;
	m_cBMPProgress.GetBitmap( &cBitmap );

	iPercent = (int)((((float)iCurrentCount) / iMaxCount) * 100.0f);
	fDotPerPercent = 100.0f/((m_cRect.right - m_cRect.left)/cBitmap.bmWidth);

	if( fDotPerPercent <= 0.0f )
	{
		iCount = 0;
	}
	else
	{
		iCount = int(iPercent/fDotPerPercent);
	}

	for( int i = 0; i < iCount; i++ )
	{


		oriDC.SelectObject( &m_cBMPProgress1 );
		pDC->BitBlt( m_cRect.left + (cBitmap.bmWidth*i), m_cRect.top, cBitmap.bmWidth,cBitmap.bmHeight,&oriDC,0,0,SRCCOPY);
	}

	oriDC.SelectObject(poldoriDC);

	unlock();
}
BEGIN_MESSAGE_MAP(CCustomProgressCtrl, CView)
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//BOOL CCustomProgressCtrl::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	//return CView::OnEraseBkgnd(pDC);
//	return FALSE;
//}
