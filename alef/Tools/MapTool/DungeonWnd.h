#if !defined(AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_)
#define AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bmp.h"
#include "DungeonToolBar.h"
#include <vector>

#define DUNGEON_TEMPLATE_FILE	"dungeontemplate.ini"
#define DUNGEON_DATA_FILE		"Cave%04d.ini"

// DungeonWnd.h : header file
//

#define DTM_COMBO_H_START	20
#define DTM_COMBO_HEIGHT	40
#define DTM_TOOL_MENU_WIDTH	50

enum ToolbarMenu
{
	TBM_FLOOR	,	// 바닥
	TBM_SLOPE	,	// 경사
	TBM_STAIRS	,	// 계단
	TBM_WALL2	,	// 벽
	TBM_WALL4	,	// 벽
	TBM_FENCE	,	// 난간
	TBM_PILLAR	,	// 기둥
	TBM_DOME4	,	// 천장..
	TBM_DOME	,
	TBM_SELECT		// 선택 또는 맥스값..
};

const int TB_SIZES[] = 
{
	1 , //TBM_FLOOR	,	// 바닥
	1 , //TBM_SLOPE	,	// 경사
	1 , //TBM_STAIRS	,	// 계단
	2 , //TBM_WALL2	,	// 벽
	4 , //TBM_WALL4	,	// 벽
	1 , //TBM_FENCE	,	// 난간
	1 , //TBM_PILLAR	,	// 기둥
	4 , //TBM_DOME4	,	// 천장.. 천장은 4타일이 기본..
	1 , //TBM_DOME ,    // 1타일 짜리 땜빵
};
 
enum DOME_TYPE
{
	DT_BASE		= 0,	// 민자
	DT_SIDE		= 1,	// 사이드
	DT_CORNAR	= 2// 모서리
};

enum	DungeonTurnAngle
{
	DTA_0_CW	,
	DTA_90_CW	,
	DTA_180_CW	,
	DTA_270_CW	
};

enum	Direction
{
	DIR_UP			,
	DIR_DOWN		,
	DIR_LEFT		,
	DIR_RIGHT
};

#define MAX_DUNGEON_INDEX_COUNT	3
#define DUNGEON_LAYER_GAP		30.0f
#define MAX_DUNGEON_HEIGHT_COUNT	12

struct CDungeonTemplate
{
public:
	struct TemplateInfo
	{
		UINT32	uIndex		;
		UINT32	uSize		;

		TemplateInfo():uIndex( 0 ),uSize( 1 ) {}
	};

	UINT32			uTID	;	// 템플릿  ID
	CString			strName	;	//템플릿 이름

	BOOL			bDomeHeight	;
	FLOAT			fDomeHeight	;
	TemplateInfo	uTemplateIndex[ TBM_SELECT ][ MAX_DUNGEON_INDEX_COUNT ];//바닥오브젝트 템플릿 인덱스 ( 4개씩 )

	// 기준 높이..
	FLOAT			afSampleHeight[ MAX_DUNGEON_HEIGHT_COUNT ];

	CDungeonTemplate():fDomeHeight( 0.0f )
	{
		// 초기화,,
		uTID	= 0;
		strName	= "이름엄스염";
		bDomeHeight	= FALSE;

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			afSampleHeight[ i ] = 100.0f * ( FLOAT ) i;
		}
	}

	void	copy( const CDungeonTemplate & param	)
	{
		this->uTID		= param.uTID	;
		this->strName	= param.strName	;

		for( int i = 0 ; i < TBM_SELECT ; ++ i )
		{
			for( int j = 0 ; j < MAX_DUNGEON_INDEX_COUNT ; ++j )
			{
				this->uTemplateIndex[ i ][ j ] = param.uTemplateIndex[ i ][ j ];
			}
		}

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			afSampleHeight[ i ] = param.afSampleHeight[ i ];
		}
	}
};

//class	CDungeonSectorData
//{
//	class CSegment
//	{
//		UINT32	uTemplateIndex	[ MAX_DUNGEON_INDEX_COUNT ]; // 한타일에 4개까지 들어감.. 바닥 , 벽 , 기둥 , 난간.. 정도?...
//		UINT32	uTurnAngle		[ MAX_DUNGEON_INDEX_COUNT ]; // 각 오브젝트별 회전 각도.. 0 1 2 3 시계방향으로 사용함..
//		FLOAT	fHeight			; // 높이..
//	};
//
//	CSegment	pTile[ 1024 ]; // 32 * 32 .. 섹터를 32등분함..
//};

/////////////////////////////////////////////////////////////////////////////
// CDungeonWnd window


class CDungeonWnd : public CWnd , ApModule
{
// Construction
public:
	CDungeonWnd();

// Attributes
public:

	AuList< CDungeonTemplate >	m_listTemplate;
	CDungeonTemplate *			GetTemplate				( int TID );
	CDungeonTemplate *			GetCurrentTemplate		();
	UINT32						GetCurrentTemplateID	();

	UINT32						GetSize( int nType )
	{
		CDungeonTemplate * pTemplate = GetCurrentTemplate();
		if( pTemplate )
		{
			if( pTemplate->uTemplateIndex[ nType ]->uSize > 0)
				return pTemplate->uTemplateIndex[ nType ]->uSize;
			else
				return TB_SIZES[ nType ] * 2;
		}
		else
			return TB_SIZES[ nType ] * 2;
	}

	void						RemoveAllElement		();

	BOOL						LoadTemplate();
	BOOL						SaveTemplate();

	BOOL						LoadDungeon();
	BOOL						SaveDungeon();

	struct DungeonElement
	{
		UINT32	uTemplateID		;	// 던젼 템플릿 ID
		UINT32	uType			;	// 타입..

		INT32	nObjectIndex	;	// 링크됀 오브젝트 인덱스.

		UINT32	uIndex			;	// 해당 템플릿의 인덱스 ( 0~2 사이 일듯.. )

		// 위치정보..
		UINT32	uOffsetX		;
		UINT32	uOffsetZ		;
		UINT32	uWidth			;	// 범위
		UINT32	uHeight			;	// 범위

		FLOAT	fHeight			;	// 높이
		UINT32	uTurnAngle		;	

		CDungeonTemplate	* pTemplate;

		DungeonElement()
		{
			Clean();
		}

		void Clean()
		{
			uTemplateID		= 0		;
			uType			= 0		;
			uIndex			= 0		;
			nObjectIndex	= 0		;	// 설정됀게 없음..

			uOffsetX		= 0		;
			uOffsetZ		= 0		;
			fHeight			= 0.0f	;
			uTurnAngle		= 0		;

			uWidth			= 1		;
			uHeight			= 1		;

			pTemplate		= NULL	;
		}
	};

	AuList< DungeonElement >	m_listDungeonElement[ TBM_SELECT ];

	UINT32						m_uSelectedType	;
	AuNode< DungeonElement >	* m_pSelectedElement;

	enum PORTION
	{
		LEFTTOP		,
		RIGHTTOP	,
		LEFTBOTTOM	,
		RIGHTBOTTOM
	};
	PORTION						m_eSelectedPortion;
	
	// 더블 버퍼링..
	CDC				m_memDC		;
	CBitmap			m_memBitmap	;
	CRect			m_memRect	;	//
	CDC *			GetMemDC()	;

	CBmp			m_bmpDivision	;
	CBmp			m_bmpHeight		;

	void			UpdateDungeonPreviewBitmap	( BOOL bReload = TRUE );
	void			UpdateDungeonHeightBitmap	();

	int			m_nGridWidth	;	// 그리드 넓이
									// 한 타일 거리를 나타냄.
	int			m_nStepSize		;	// 

	INT32		m_nLeftX		;	// 좌상 좌표..
	INT32		m_nTopZ			;
			// 위의 그리드 넓이와 연동됨
			// 픽셀 단위 옵셋으로 처리되므로.
			// 그리드 위스가 바뀔경우 이것도 비율대로 변경돼어야함..

	COLORREF	m_colorGrid		;
	COLORREF	m_colorBack		;
	COLORREF	m_colorGridEdge	;
	COLORREF	m_colorGridTile	;
	COLORREF	m_colorGridHalf	;

	// 마고자 (2005-05-10 오전 11:17:47) : 
	// 천장그리기용 셀렉션 정보..
	vector< POINT >	m_vectorSelectedPoint;
	INT32	GetMovement( INT32 nOffset );

public:
	// 계산 용 백업..
	BOOL		m_bRbuttonDown	;	// 오른쪽 버튼 누르고 있음..
	CPoint		m_pointLastPress;

public:
	int			GetGridWidth() { return m_nGridWidth;}
	int			SetGridWidth( int nWidth );

	// 그리드 표시용
	enum DETYPE
	{
		DET_NORMAL		,
		DET_SELECTED	,
		DET_HEIGHT		,
		DET_MAX
	};
	BOOL		DrawElement( CDC * pDC , UINT32 uType , DungeonElement * pElement , UINT32 uSelected = DET_NORMAL );

	int			GetXOffset( int nScreenX , int * pDep = NULL)
	{
		int	nDelta = ( nScreenX - m_nLeftX );
		if( pDep )
		{
			* pDep = ( nDelta - ( nDelta / m_nGridWidth ) * m_nGridWidth ) * 100 / m_nGridWidth;
		}

		return nDelta / m_nGridWidth;
	}
	int			GetZOffset( int nScreenY , int * pDep = NULL )
	{	
		int	nDelta = ( nScreenY - m_nTopZ );
		if( pDep )
		{
			* pDep = ( nDelta - ( nDelta / m_nGridWidth ) * m_nGridWidth ) * 100 / m_nGridWidth;
		}

		return nDelta / m_nGridWidth;
	}

	int			GetXOffsetLocal( int nScreenX )
	{
		int	nOffset			= GetXOffset( nScreenX );
		int nDelta			= ( nScreenX - m_nLeftX ) - nOffset * m_nGridWidth;
		int nLocalOffset	= ( nDelta * 3 ) / m_nGridWidth;

		ASSERT( nLocalOffset >= 0 );
		if( nLocalOffset >= 3 ) nLocalOffset = 2;
		return nLocalOffset;
	}
	
	int			GetZOffsetLocal( int nScreenY )
	{
		int	nOffset			= GetZOffset( nScreenY );
		int nDelta			= ( nScreenY - m_nTopZ ) - nOffset * m_nGridWidth;
		int nLocalOffset	= ( nDelta * 3 ) / m_nGridWidth;

		ASSERT( nLocalOffset >= 0 );
		if( nLocalOffset >= 3 ) nLocalOffset = 2;
		return nLocalOffset;
	}

	int			GetXOffsetStep( int nScreenX ){	return GetXOffset( nScreenX ) - GetXOffset( nScreenX ) % m_nStepSize;}
	int			GetZOffsetStep( int nScreenY ){	return GetZOffset( nScreenY ) - GetZOffset( nScreenY ) % m_nStepSize;}

	int			GetScreenX( int nOffsetX ){	return m_nLeftX + nOffsetX * m_nGridWidth;}
	int			GetScreenY( int nOffsetZ ){	return m_nTopZ + nOffsetZ * m_nGridWidth;}

	BOOL		AddElement		( UINT32 uType , DungeonElement * pEle );
	BOOL		RemoveElement	( UINT32 uType , AuNode< DungeonElement > * pNode );
	BOOL		UpdateElement	( UINT32 uType , DungeonElement * pEle );
		// DungeonElement 기준으로 실 오브젝트를 업데이트함 ( 주로 이동 )

	FLOAT	__GetWorldXFromElement( UINT32 uType , DungeonElement * pElement );
	FLOAT	__GetWorldZFromElement( UINT32 uType , DungeonElement * pElement );
	FLOAT	__GetWorldDeltaYFromElement( UINT32 uType , DungeonElement * pElement );

	void		Apply( BOOL bAdjustHeight = FALSE );

	FLOAT		GetXPosFromOffset( UINT32 uOffsetX );
	FLOAT		GetZPosFromOffset( UINT32 uOffsetZ );
	INT32		GetXOffsetFromPos( FLOAT fPosX );
	INT32		GetZOffsetFromPos( FLOAT fPosZ );

	void		MoveToPosition( RwV3d * pPos );

	void		Idle();

	BOOL		m_bControlState;

	AuNode< DungeonElement > *	GetElement( UINT32 uType , int nOffsetX , int nOffsetZ , int nDeltaX = 0 , int nDeltaZ = 0 );
	DungeonElement *			GetElementByObjectIndex( INT32 nObjectIndex );
	BOOL		DeleteSelected();
public:
	// Selection 계산용..
	enum	SELECTMODE
	{
		SM_NONE		,
		SM_RANGE	,
		SM_LINE
	};
	SELECTMODE	m_nSelectMode			;
	int			m_SelectedPosX1			;
	int			m_SelectedPosZ1			;
	int			m_SelectedPosX2			;
	int			m_SelectedPosZ2			;
	
// Operations
public:

	// 던젼 정보 스트리밍..
	static BOOL	DungeonObjectStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	DungeonObjectStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDungeonWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDungeonWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};

// 클럼프의 전체 높이를 구해낸다..
// 벽의 높이를 알아내려고 할때 사용함..
FLOAT	__GetClumpHeight( RpClump * pClump );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUNGEONWND_H__2E636F4E_A7C5_4C21_9DDF_6A5BCE6EEAE4__INCLUDED_)

/*
 *	
 던젼 윈도우 프로그램 구상..

  데이타 구조..
  class CDungeonTemplate
  {
	템플릿 이름
	바닥오브젝트 템플릿 인덱스 ( 4개씩 )
	벽 오브젝트 템플릿 인덱스 
	난간 오브젝트 템플릿 인덱스..
	벽
	계단
	경사
	기둥
  }

  던젼 데이타는 섹터 데이타 단위로 1:1로 생성되며
  타일은 기존 타일에 두배로 나뉘어 사용한다.
  섹터데이타에 추가데이타로 붙으면 Ok 일려나? ;;
  class	CDungeonSectorData
  {
	class CSegment
	{
		uTemplateIndex[ 4 ]; // 한타일에 4개까지 들어감.. 바닥 , 벽 , 기둥 , 난간.. 정도?...
		uTurnAngle[ 4 ]; // 각 오브젝트별 회전 각도.. 0 1 2 3 시계방향으로 사용함..
		FLOAT	fHeight	; // 높이..
	};

	CSegment	pTile[ 32 * 32 ];

	GetSegment~
	등등..
  }


  1, 템플릿 에디트 창
  각각 리스트와 템플릿 인덱스를 지정함..
  그걸로 충분..
  템플릿을 지정하면 저장을 함.
  Map/DungeonTemplate.ini
  에다가 저장함..
  포멧은 그냥 평범하게.

  2, 툴바..
  템플릿 선택하고 메뉴정리..
  선택 하는 것만 있으니 니부분은 간단함..

  오늘까지 이까지 구현..
 */
