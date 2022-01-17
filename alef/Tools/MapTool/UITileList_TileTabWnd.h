#if !defined(AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_)
#define AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UITileList_TileTabWnd1.h : header file
//

#include "bmp.h"
#include "AgcmMap.h"

/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd window

// 화면 구성.
// 넓이 300
// 카테고리1| 타일1  , 타일2 , 타일3 , 타일4 |  <- 한라인에 타일 4개 표시.. 
//          |                                |  <- 타일 숏컷 싸이즈는 50*50 으로 처음에 자작된다.
// 카테고리2| 쭈우우욱...                    |
//          |                                |
//-------------------------------------------|
// 카테고리는 손으로 직접 편집한다.
// 카테고리 찍고 마우스 오른쪽 버튼을 누르면 팝업 메뉴가 뜨면서 메뉴등장 ( 카테고리에디트 )
//
//----------------------------------------------------------------------------------
// 기본동작
//----------------------------------------------------------------------------------
// 1, 카테고리를 선택
//      - 카테고리가 하이라이트 돼면서 , 오른쪽 창에 등록됀 타일들이 표시된다.
//      - 표시돼는 타일들은 이름과 함께 표시됀다.
// 2, 타일 선택
//      - 타일이 하이라이트 됀다.
//      - 이후에 찍을 타일로서 이 타일이 선택됀다.
// 3, 카테고리에서 마우스 오른쪽 버튼 클릭.
//      - 카테고리 편집 매뉴를 부른다.
//      - Add,Delete,Edit 를 불러낸다.
// 4, 타일매뉴에서 마우스 오른쪽 버튼 클릭
//      - 타일 편집 메뉴를 띄운다.
//      - 여기서 타일을 삭제/이름변경 을 할 수 있다.
// 5, 타일을 카테고리로 드래그&드롭
//      - 카테고리 변경 기능을 수행한다.


// 소스데이타는 따로 저장하지않고 , 머지한것만 저장한다.
// 소스데이타를 입력받고 , 알파 이미지와 함께 머징을 해서
// 클라이언트 데이타를 생성한다.

class CUITileList_TileTabWnd : public CWnd
{
// Construction
public:
	struct	stTileInfo
	{
		CString	name		;// 타일이름..
		int		index		;// 파일 인덱스.. 생성됨..
		CBmp	*pBmp		;// 이미지..
		int		applyalpha	;// 사용할 알파 이미지.
	};

	struct stTileCategory
	{
		CString					name	;	// 카테고리 이름
		int						category;	// 카테고리가 가지는 타일 타입 ID
		AuList< stTileInfo >	list	;
	};

	// 타일리스트에 있는 녀석을 잘 구슬려서 데리고 산다.
	AuList< stTileCategory * >		m_listCategory		;
	stTileCategory *				m_pSelectedCategory	;	// 선탠됀거 포인터로 가지고 잇는다.
	stTileInfo *					m_pSelectedTile		;	// 선택됀 타일 정보..
	int								m_nSelectedLayer	;	// 현재 작업중인 레이어 정보.
	
	CBitmap			m_backgroundbitmap	;
	CBrush			m_backgroundbrush	;
	CBitmap			m_aBitmapLayer[ TILELAYER_COUNT ];

	CBitmap			m_bitmapCategoryBoxSelected	;
	CBitmap			m_bitmapCategoryBoxUnselect	;
	CBitmap			m_bitmapTileSelect			;
	CBitmap			m_bitmapTileSelect2			;

	CRect			m_rectCursor				;
	CDC				m_MemDC						;
	CBitmap			m_MemBitmap					;	// 더블버퍼링용..

	
	CUITileList_TileTabWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUITileList_TileTabWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ChangeLayer	( int layer		);
	void SelectTile		( int tileindex	);
	BOOL SaveScript		();
	void LoadScript		();

	// 마고자 (2004-05-31 오후 1:06:41) : 타일이 사용돼었는지 검사.
	BOOL IsUsedTileName	( stTileInfo * pTInfo );

	BOOL CreateTextureList	( AcTextureList *				pList		);

	BOOL LoadBitmap			( stTileInfo  * pTileInfo			, BOOL bForce = FALSE );

	BOOL AddCategory		( stTileCategory *					pCategory	);
	BOOL AddTile			( int index , char * comment					);

	void UpdateDimensino	();

	stTileCategory *	FindCategory			( int		index );
	stTileCategory *	GetCategoryFromPoint	( CPoint	point );
	stTileInfo *		GetTileFromPoint		( CPoint	point );

	virtual ~CUITileList_TileTabWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUITileList_TileTabWnd)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTilewindowAddcategory();
	afx_msg void OnTilewindowDeleteCategory();
	afx_msg void OnTilewindowEditCategory();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnTilewindowEditTile();
	afx_msg void OnTilewindowDeleteTile();
	afx_msg void OnTilewindowChangeTile();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UITILELIST_TILETABWND1_H__DCB98719_2659_4C2B_BE2A_4B8C23FC7184__INCLUDED_)
