#ifndef _ACUIBBS_H_
#define _ACUIBBS_H_

#include "AcUIBase.h"
#include "AcUIModalImage.h"
#include "AcUIBbsContent.h"

#define ACUIBBS_COLUMN_NAME_MAX									64
#define ACUIBBS_CELL_CONTENT_LENGTH_MAX							64
#define	ACUIBBS_COLUMN_NUM_MAX									16
#define	ACUIBBS_CALLBACKPOINT_MAX_NUM							10
#define	ACUIBBS_PAGEMANAGER_NUM									12

enum 
{
	UICM_BBS_CHANGE_PAGE										= UICM_BASE_MAX_MESSAGE,
	UICM_BBS_CLICK_CONTENT,
	UICM_BBS_MAX_MESSAGE
};

enum HANFONT_ARRANGETYPE
{
	HANFONT_LEFTARRANGE											= 0,
	HANFONT_CENTERARRANGE,
	HANFONT_RIGHTARRANGE
};

typedef enum AcUIBbsCallbackPoint
{
	ACUIBBS_CLICK_CONTENT										= 0,
	ACUIBBS_CHANGE_PAGE,
	ACUIBBS_WINDOW_CLOSE,
} AcUIBbsCallbackPoint;

struct stAgcUIBbsColumnInfo 
{
	char szColumnName[ ACUIBBS_COLUMN_NAME_MAX ];
	UINT16 nColumnWidth;
	HANFONT_ARRANGETYPE eArrayType;
};

struct stAgcUIBbsCellContent
{
	UINT32				lIndex;									// 각 Row가 가지고 있는 고유 Index - 화면상에는 표시되지 않는다. - Row의 각 Cell은 같은 Index를 갖는다. 
	char				szCellContent[ ACUIBBS_CELL_CONTENT_LENGTH_MAX ];	// Cell에 담긴 String Info 
	RwRect				rectMyArea;
	RwRect				rectSelectArea;
};

struct stAcUIBbsInfo
{
	UINT8				m_cColumnNum;							// 0 ~ ACUIBBS_COLUMN_NUM_MAX
	RwV2d				m_v2dCellStart;							//CellStartPointer
	UINT16				m_nCellHeight;							//CellHekght
	UINT8				m_cRowNum;								//이미지상의 Image 갯수 

	UINT16				m_nAllPageNum;							//총 페이지 수 
	UINT16				m_nNowPageNum;							//현재의 페이지 
	UINT16				m_nGapColumnCell;						//Column Name과 Cell Content 사이의 Gap 
	UINT16				m_nColumnGap;							//Column 사이의 Gap
};

struct stAgcUIBbsPageManager
{
	INT32				lPageIndex;								//Page Index 단 -2 : PrePage, -1 : NextPage 0이면 그리지 않음 
	char				szPageIndex[ 10 ];				
	RwRect				rectPageSelectArea;				
};

// Content Click등이 일어났을때 Command Message로 함께 날린다. 
struct stAgcUIBbsCommandMessageInfo
{
	INT32				lControlID;
	UINT32				lSelectedIndex;
	UINT8				lSelectedRow;
	UINT8				lSelectedColumn;
};


/**

-- 2008. 06. 12. 현재 쓰이는 곳은 AcUIRecruit 에서만 사용중..
   멤버로 갖고 있는 AcUIBbsContent 도 AcUIBbs 내에서만 쓰이나 실제로 사용되고 있지는 않는듯..
   추후 사용하지 않는 것이 확인되면 상의후 삭제 조치할까 함.. by 성일

*/
class AcUIBbs : public AcUIBase
{
public:
	AcUIBbs();
	virtual ~AcUIBbs();

	// Cell정보가 담길 포인터 
	stAgcUIBbsCellContent*								m_pstCellContent;
	stAgcUIBbsCommandMessageInfo						m_stCommandMessageInfo;

private:
	// 초기 Setting이 되었는지 여부 
	BOOL												m_bInitSetting;
		
	// Column 정보 
	stAgcUIBbsColumnInfo								m_stColumnInfo[ ACUIBBS_COLUMN_NUM_MAX ];

	// 각종 게시판에 필요한 Data들 
	stAcUIBbsInfo										m_stBbsInfo;

	// 게시판 내용이 써질 Window
	AcUIBbsContent										m_clContentWindow;

	// Page 매니저
	stAgcUIBbsPageManager								m_stPageManager[ ACUIBBS_PAGEMANAGER_NUM ];

public:
	// 초기화 및 해제용
	void					InitInfoInput				( stAcUIBbsInfo *pstBbsInfo );
	BOOL					Destroy						( void );
	void					PageManagerSetting			( void );

	// 내용입력..
	BOOL					SetColumnInfo				( UINT8 cColumnIndex, char *pszColumnName, UINT16 nColumnWidth, HANFONT_ARRANGETYPE eArrayType = HANFONT_LEFTARRANGE );
	BOOL					SetColumnInfo				( UINT8 cColumnIndex, stAgcUIBbsColumnInfo *pstBbsColumnInfo );
	void					SetPageInfo					( UINT16 nAllPage,	UINT16 nNowPage );
	BOOL					SetCellText					( UINT8 nRow, UINT8 nColumn, char* pszContent );

	// 내용삭제.. 해당 칸이 사라지거나 하는건 아니고 Cell 에 들어있는 내용만 지워짐
	void					ClearAllCellText			( void );							// 몽땅 삭제
	void					ClearCellText_ByRow			( UINT8 nRow );						// 한줄 삭제
	void					ClearCellText				( UINT32 nRow, UINT32 nColumn );	// Cell 하나 삭제

private:
	// Cell 메모리 생성 및 해제
	void					_CreateCellContentMemory	( void );
	void					_DestroyCellContentMemory	( void );

	// 지정 인덱스가 실제로 존재하는지 검사한다.
	BOOL					_IsValidArrayIndex			( UINT16 nArrayIndex );
	BOOL					_IsValidRowIndex			( UINT8 nRow );
	BOOL					_IsValidColumnIndex			( UINT8 nCol );
	BOOL					_IsValidPageIndex			( void );
	BOOL					_IsValidCallBackIndex		( UINT32 nCallBackIndex );

	// 지정 영역에 들어왔는지를 검사한다.
	BOOL					_IsInAreaPageManager		( stAgcUIBbsPageManager* pPageManager, INT32 nMouseX, INT32 nMouseY );
	BOOL					_IsInAreaCell				( stAgcUIBbsCellContent* pCell, INT32 nMouseX, INT32 nMouseY );

	// 지정 Cell 을 반환한다.
	stAgcUIBbsCellContent*	_GetCell					( UINT32 nRow, UINT32 nCol );
	stAgcUIBbsCellContent*	_GetCell					( UINT16 nArrayIndex );

	// 메세지를 전송
	BOOL					_SendChangePage				( INT32 nNewPageNumber );
	BOOL					_SendClickCell				( UINT32 lIndex, INT32 nRow, INT32 nColumn );

	// Cell 그리기
	void					_DrawColumnName				( RwRect* pRect, INT32 nColCount );
	void					_DrawCell					( RwRect* pRect, INT32 nRowCount, INT32 nColCount );
	void					_DrawBbsContent				( void );
	void					_DrawPageManager			( void );

	//virtual function
public:
	virtual	void			OnPostRender				( RwRaster *raster );
	virtual	void			OnWindowRender				( void );
	virtual	BOOL			OnLButtonDown				( RsMouseStatus *ms	);	
	virtual	void			OnClose						( void );
	virtual BOOL			OnInit						( void );
};

#endif _ACUIBBS_H_

/*
		*게시판 사용순서
			Instance 지정
			AddImage
			AddImageForContentWindow
			InitInfoInput
			SetColumn
			.
			.
			.
			Destroy

	

		* 각 Cell은 자신의 영역인 rectMyArea를 가진다									->	각 content에 해당하는 String을 쓸때 사용 
		* 각 Cell은 DrawText이후 돌려받은 String이 쓰여진 영역 rectSelectArea를 가진다.	->  각 content를 마우스로 선택할때 체크 하기 위하여 사용 

*/