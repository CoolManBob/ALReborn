// ServerListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ServerListCtrl.h"


// CServerListCtrl

IMPLEMENT_DYNAMIC(CServerListCtrl, CListCtrl)

CServerListCtrl::CServerListCtrl()
{

}

CServerListCtrl::~CServerListCtrl()
{
}


BEGIN_MESSAGE_MAP(CServerListCtrl, CListCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// CServerListCtrl 메시지 처리기입니다.



void CServerListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu	menu;
	CMenu	*pMenu;
	POINT	point;

	menu.LoadMenu( IDR_LISTMENU );
	pMenu	=	menu.GetSubMenu(0);

	GetCursorPos( &point );

	pMenu->TrackPopupMenu( TPM_LEFTBUTTON , point.x , point.y , GetParent()  );
	
}
