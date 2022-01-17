#ifndef	_ACUIGRID_H_
#define	_ACUIGRID_H_

#include "AcUIBase.h"
#include "AgpmGrid.h"
#include "AcUITexture.h"

#define ACUIGRID_TWINKLE_TIMEGAP							500		// Grid Item Twinkle( 깜빡거림 ) Mode 시의 Time Gap

// End Drag가 왔을때 정보를 저장해 놓는다 
struct AgpdGridSelectInfo
{
	INT32													lGridRow;		// End Drag가 들어온 Row
	INT32													lGridColumn;	// End Drag가 들어온 Column
	
	AgpdGridItem*											pGridItem;		// End Drag로 들어온 
	AgcWindow*												pTargetWindow;	// DragDrop 시 Target Window 
	INT32													lX;				// DragDrop 시 Target Window위의 좌표
	INT32													lY;				// DragDrop 시 Target Window위의 좌표

	AgpdGridSelectInfo( void )
	{
		lGridRow = 0;
		lGridColumn	= 0;
	
		pGridItem = NULL;
		pTargetWindow =	NULL;
	}
};

// Command Message
enum 
{
	UICM_GRIDITEM_DRAG_START								= UICM_BASE_MAX_MESSAGE,
	UICM_GRIDITEM_DRAG_END,
	UICM_GRIDITEM_SET_FOCUS,
	UICM_GRIDITEM_KILL_FOCUS,
	UICM_GRIDITEM_MAX_MESSAGE
};

// Command Message
enum
{
	UICM_GRID_DRAG_START									= UICM_BASE_MAX_MESSAGE,
	UICM_GRID_DRAG_END,
	UICM_GRID_DRAG_DROP,
	UICM_GRID_LBUTTON_ITEM_CLICK,
	UICM_GRID_RBUTTON_ITEM_CLICK,
	UICM_GRID_GRIDITEM_SET_FOCUS,
	UICM_GRID_GRIDITEM_KILL_FOCUS,
	UICM_GRID_MAX_MESSAGE
};

/* 사용법
	1. SetAgpdGriPointer(~~) 를 통해서 AgpdGrid*를 먼저 넘겨준다 - GridItemWindow생성과 Child로 Add
	2. SetGridItemDrawInfo() 를 호출해 준다 - 이 정보를 가지고 Add된 Child를 MoveWindow로 배열한다 

	!!! 두 함수의 순서가 반드시 맞아야 함 !!!
*/
class AgcmUIControl;
class AcUIGrid;
class AcUIScroll;

class AcUIGridItem : public AcUITexture
{
public:
	AgpdGridItem*											m_ppdGridItem;
	AcUIGrid*												m_pParentGrid;

	static My2DVertex										m_vAlphaFan[ 7 ];

	AcUIGridItem( void )
	{
		m_ppdGridItem =	NULL;
		m_pParentGrid =	NULL;
		m_nType = TYPE_GRID_ITEM;
	}

public:
	virtual	BOOL 			OnLButtonDown					( RsMouseStatus *ms );
	virtual BOOL 			OnLButtonUp						( RsMouseStatus *ms );
	virtual BOOL 			OnDragDrop						( PVOID pParam1, PVOID pParam2 );
	virtual	void 			OnWindowRender					( void );
	virtual void 			OnSetFocus						( void );
	virtual void 			OnKillFocus						( void );

private:
	BOOL					DrawCoolTime( int lAbsoluteX, int lAbsoluteY, UINT8 nAlpha, RwTexture* pTexture, float fPercent );
};

class AcUIGrid : public AcUIBase
{
public:
	AcUIGrid( void );
	virtual ~AcUIGrid( void );

public:
	AgpdGrid*												m_pAgpdGrid;					// 이 클래스의 기본이 되는 Grid
	AcUIGridItem**											m_apGridItemWindow;				// Grid Item 에 해당하는 Window - SetAgpdGridPointer 함수 호출시 생성된다 	

	INT32													m_lGridItemWindowNum;			// Grid Item의 Num
	INT32													m_lNowLayer;					// 현재의 Layer

	INT32													m_lGridItemStart_x;
	INT32													m_lGridItemStart_y;
	INT32													m_lGridItemGap_x;
	INT32													m_lGridItemGap_y;
	INT32													m_lGridItemWidth;
	INT32													m_lGridItemHeight;

	BOOL													m_bDrawAreas;
	DWORD													m_dwAreaColor;
	UINT8													m_ucAreaAlpha;

	BOOL													m_bMoveItemCopy;				// default : FALSE - Item이 이동할때 그 자리에 같은 Window를 생성한다 
	AcUIGridItem											m_clCopyGridItem;				// 평소에는 Width, Height가 0이다. 

	BOOL													m_bGridItemMovable;				// Grid Item 을 움직일 수 있는가의 여부 - default 는 TRUE 

	BOOL													m_bDrawImageForeground;			// Grid Item 앞에 이미지를 그릴지 여부
	
	INT32													m_lItemToolTipX;				// Item Tool Tip 이 그려질 X 좌표 
	INT32													m_lItemToolTipY;				// Item Tool Tip 이 그려질 Y 좌표 
	INT32													m_lItemToolTipID;				// 그려질 Item Tool Tip의 IID 
	AgpdGridItem*											m_pToolTipAgpdGridItem;			// ToolTip에 사용될 AgpdGridItem Pointer
	
	BOOL													m_bGridItemBottomCountWrite;	// Grid Item 의 Left 또는 Right 하단에 Count를 쓸것인가 여부 

	INT32													m_lReusableDisplayTextureID;

	// Scroll 관련
	AcUIScroll *											m_pcsGridScroll;
	INT32													m_lGridItemStartRow;
	INT32													m_lGridItemVisibleRow;

public:
	void					SetGridItemMovable				( BOOL bMovable );																	// Grid Item 을 움직일수 있는가의 여부를 Setting 한다  
	void					SetGridItemBottomCountWrite		( BOOL bBottomCount ) { m_bGridItemBottomCountWrite = bBottomCount;	}				// Grid Item 에 Count를 쓸것인가의 여부 	
	void					SetGridItemMoveItemCopy			( BOOL bMoveItemCopy ) { m_bMoveItemCopy = bMoveItemCopy; }							// Move Item 을 Copy해 둘것인가의 여부
	void					SetGridItemDrawImageForeground	( BOOL bDrawImageForeground ) { m_bDrawImageForeground = bDrawImageForeground; }	// Move Item 을 Copy해 둘것인가의 여부
	BOOL					GetGridItemMovable				( void ) { return m_bGridItemMovable; }
	BOOL					GetGridItemBottomCountWrite		( void ) { return m_bGridItemBottomCountWrite; }
	BOOL					GetGridItemMoveItemCopy			( void ) { return m_bMoveItemCopy; }
	BOOL					GetGridItemDrawImageForeground	( void ) { return m_bDrawImageForeground; }
	
	void					SetAgpdGridPointer				( AgpdGrid* pAgpdGrid );
	void					SetNowLayer						( INT32 lNowLayer );		// 현재의 Layer를 Setting한다 - 초기값은 0
	void					SetGridItemDrawInfo				( INT32 lStartX, INT32 lStartY, INT32 lGapX, INT32 lGapY, INT32 lGridItemWidth, INT32 lGridItemHeight );

	AgpdGrid *				GetAgpdGrid						( void ) { return m_pAgpdGrid; }

	INT32					GetStartX						( void ) { return m_lGridItemStart_x; }
	INT32					GetStartY						( void ) { return m_lGridItemStart_y; }
	INT32					GetGapX							( void ) { return m_lGridItemGap_x; }
	INT32					GetGapY							( void ) { return m_lGridItemGap_y; }
	INT32					GetItemWidth					( void ) { return m_lGridItemWidth; }
	INT32					GetItemHeight					( void ) { return m_lGridItemHeight; }
	
	void					UpdateUIGrid					( void );	// m_pAgpdGrid Pointer를 이용해서 UI에 반영한다 
	AgpdGridSelectInfo*		GetDragDropMessageInfo			( void );	// 처리하지 않은 EndDragMessage가 없을때는 NULL을 리턴 
	AgpdGridSelectInfo* 	GetGridItemClickInfo			( void );
	BOOL					GetGridRowColumn				( RsMouseStatus* ms, INT32* lRow, INT32 * lColumn );
	BOOL					GetGridRowColumn				( INT32 lX, INT32 lY, INT32* lRow, INT32 * lColumn );
	BOOL					GetGridRowColumn				( INT32 lIndex, INT32* lRow, INT32* lColumn );

	void					SetDrawAreas					( BOOL bDrawAreas ) { m_bDrawAreas = bDrawAreas; }
	void					SetDrawColor					( DWORD dwColor, UINT8 ucAlpha ) { m_dwAreaColor = dwColor; m_ucAreaAlpha = ucAlpha; }

	void					OnWindowRender					( void );

	void					DeleteAllChild					( void );
	void					SetClickInfoTargetWindow		( AgcWindow* pWindow, INT32 lX, INT32 lY );

	BOOL					SetReusableDisplayImage			( INT32 lImageID );
	INT32					GetReusableDisplayImage			( void );

	// Scroll 관련
	VOID					SetScroll						( AcUIScroll *pcsScroll );

private:
	BOOL													m_bDragDropMessageExist;	// 처리하지 않은 EndDragMessage가 있을때 TRUE
	AgpdGridSelectInfo										m_stDragDropMessageInfo;
	BOOL													m_bGridItemClickInfoExist;	// 처리하지 않은 GridItemClickInfo 가 있다 
	AgpdGridSelectInfo										m_stGridItemClickInfo;		// Grid가 Click된 정보 

	BOOL					SetGridItemMemory				( void );
	void					AddGridItemWindow				( void );					// Grid Item Window를 Add해 준다 
	virtual void			MoveGridItemWindow				( void );					// Grid Item Draw Info를 가지고 배열한다 - SkillTree에서 형태를 변형하기 위해 virtual선언 
	void					SetGridItemTextureAndInfo		( void );					// Grid Item Window에 Texture를 Setting한다 
	INT32					GetIndex						( INT32 lRow, INT32 lColumn );
	AcUIGridItem*			GetGridItemWindow				( INT32 lRow, INT32 lColumn );
	AcUIGridItem*			GetGridItemWindowByPos			( INT32 x, INT32 y );
	void					SetCopyGridItem					( INT32 lGridItemIndex );
	void					ReleaseCopyGridItem				( void );

public:
	virtual BOOL			OnPostInit						( void );
	virtual void			OnClose							( void );
	virtual BOOL			OnDragDrop						( PVOID pParam1, PVOID pParam2 );
	virtual BOOL			OnCommand						( INT32	nID, PVOID pParam );
	virtual	BOOL			OnLButtonDown					( RsMouseStatus *ms );
	virtual	BOOL			OnLButtonUp						( RsMouseStatus *ms );
	virtual	BOOL			OnRButtonDown					( RsMouseStatus *ms );
	virtual	BOOL			OnRButtonUp						( RsMouseStatus *ms );
	virtual	BOOL			OnMouseWheel					( INT32	lDelta );

	virtual VOID			SetFocus						( VOID );
};

#endif			// _ACUIGRID_H_
