// ListBoxDrag.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ListBoxDrag.h"
#include "Patcher2Dlg.h"


// CListBoxDrag

IMPLEMENT_DYNAMIC(CListBoxDrag, CListBox)

CListBoxDrag::CListBoxDrag()
{
}

CListBoxDrag::~CListBoxDrag()
{
}


BEGIN_MESSAGE_MAP(CListBoxDrag, CListBox)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CListBoxDrag 메시지 처리기입니다.



void CListBoxDrag::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CPatcher2Dlg*		pParentDlg		=	static_cast< CPatcher2Dlg* >(GetParent());

	if( hDropInfo )
	{
		INT				nCount			=	DragQueryFile( hDropInfo , 0xFFFFFFFF , NULL , 0 );
		CHAR*			pFileName		=	NULL;
		DWORD			dwFileNameSize	=	0;

		for( INT i = 0 ; i < nCount ; ++i )
		{
			dwFileNameSize		=	DragQueryFile( hDropInfo, i , NULL , 0 );
			pFileName			=	new char[ dwFileNameSize+1 ];
			dwFileNameSize		=	DragQueryFile( hDropInfo , i , pFileName , dwFileNameSize+1 );

			pParentDlg->DragFileAdd( pFileName );

			delete [] pFileName;
			pFileName		=	NULL;
		}

	}

	CListBox::OnDropFiles(hDropInfo);
}
