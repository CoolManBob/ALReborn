// NewStatic.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "NewStatic.h"


// CNewStatic

IMPLEMENT_DYNAMIC(CNewStatic, CStatic)

CNewStatic::CNewStatic()
{
	m_TextData	=	"";
}

CNewStatic::~CNewStatic()
{
}


BEGIN_MESSAGE_MAP(CNewStatic, CStatic)
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CNewStatic::OnPaint( VOID )
{
	CPaintDC dc(this);

	UINT		nFormat = 0;
	CFont*		OldFont	= NULL;
	CRect		rect;
	ZeroMemory( rect , sizeof(rect) );
	GetClientRect( &rect );
	
	OldFont	=	(CFont*)dc.SelectObject( GetFont() );	//	폰트 지정

	dc.SetBkMode( TRANSPARENT );			//	배경 투명
	dc.SetTextColor( RGB(255,255,255) );	//	글자색 화이트
	
	DrawTextW( dc.m_hDC , m_TextData , m_TextData.GetLength() , &rect , DT_CENTER | DT_SINGLELINE  );

	dc.SelectObject( OldFont );
}

BOOL CNewStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return CStatic::OnEraseBkgnd(pDC);
}
