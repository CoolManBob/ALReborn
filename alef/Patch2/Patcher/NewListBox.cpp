// NewListBox.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "NewListBox.h"


// CNewListBox

IMPLEMENT_DYNAMIC(CNewListBox, CListBox)

CNewListBox::CNewListBox()
{

}

CNewListBox::~CNewListBox()
{
}


BEGIN_MESSAGE_MAP(CNewListBox, CListBox)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CNewListBox 메시지 처리기입니다.



BOOL CNewListBox::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	return CListBox::OnEraseBkgnd(pDC);
}
