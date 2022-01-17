#if !defined(AFX_REGIONVIEWSTATIC_H__FF20FC59_0062_4078_AFDB_86995B5BE97C__INCLUDED_)
#define AFX_REGIONVIEWSTATIC_H__FF20FC59_0062_4078_AFDB_86995B5BE97C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RegionViewStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRegionViewStatic window

#define MAP_IMAGE_WIDTH	512
#define MAX_DIVISION	100

#define REGION_NO_TEMPLATE	(-1)
#define MAX_REGION_BITMAP_CASH	20

COLORREF __GetColor( int nColor );

enum	DataUpdatedFlag
{
	DUF_NONE	= 0x0000,
	DUF_SPAWN	= 0x0001,
	DUF_SKY		= 0x0002,
	DUF_BLOCK	= 0x0004
};

enum	DivisionInWindow
{
	DI_OUTSIDE	,	// 바깥
	DI_OVERLAP	,	// 경계에 걸림
	DI_INSIDE		// 안에 있음.
};

class ApdObject;

class CRegionViewStatic : public CStatic
{
protected:
// Construction
	AuPOS			m_posLastClick		;

public:
	CRegionViewStatic();

// Attributes
public:

	struct DivisionImageData
	{
		CBmp	bmp		;
		UINT32	uFlag	;
		UINT32	uLock	;

		//CBmp	bmpRegion		;
		CBmp	bmpBlocking		;
		CBmp	bmpSkyBlocking	;

		BOOL	bRegionDataLoaded	;
		DivisionInWindow	eViewState	;

		int		nStartX;
		int		nStartY;

		vector< ApdObject * >	m_vecIndex;	// 들어있는 데이타들 리스팅..

		DivisionImageData() :	bRegionDataLoaded	( FALSE			) , 
								eViewState			( DI_OUTSIDE	) ,
								nStartX				( 0				) ,
								nStartY				( 0				)
		{
			uFlag	= DUF_NONE;
			uLock	= DUF_NONE;
		}

		void	Release()
		{
			bmp			.Release();
			//bmpRegion	.Release();

			if( uFlag & DUF_BLOCK )
			{
				// 스카이 플래그가 있으면 날리지 않음.
			}
			else
			{
				bmpSkyBlocking.Release();
			}
			bRegionDataLoaded	= FALSE;
		}

		bool	AddIndex( ApdObject * pObject )
		{
			m_vecIndex.push_back( pObject );
			return true;
		}
		void	ResetIndex()
		{
			m_vecIndex.clear();
		}
	};

	// 이미지 파일 리스트
	DivisionImageData	m_bmpMap[ MAX_DIVISION ][ MAX_DIVISION ];
	DivisionImageData *	m_pBitmapCash[ MAX_REGION_BITMAP_CASH ];
	vector< DivisionImageData * > m_vecDisplayedDivision;	// 현재 출력돼고 있는 디비젼.
	DivisionImageData *	GetMap( int x , int z , BOOL bImageLoad = TRUE );

	void		UnsavedDataCheck( UINT32 uDivisionIndex , UINT32 uFlag );
	void		SetAllUnsavedDataCheck( UINT32 uFlag );
	void		ClearUnsavedDataCheck( UINT32 uFlag );

	void	ClearAllIndex();

public:
	void	MoveTo( int nDivision );
	void	LoadCompactData( int x , int z );

	FLOAT	GetZoom() { return m_fScale;}
	void	SetZoom( FLOAT fZoom );

	FLOAT	REGION_OFFSET_TO_POSITION( INT32 nOffset	)
	{
		return ( ( FLOAT ) ( nOffset ) * ( 200.0f / m_fScale ) - MAP_SECTOR_WIDTH	* 400.0f );
	}
	INT32	REGION_POSITION_TO_OFFSET( FLOAT fPos		)
	{
		return ( INT32 ) ( ( ( fPos ) + MAP_SECTOR_WIDTH	* 400.0f ) / ( 200.0f / m_fScale ) );
	}

	FLOAT	REGION_OFFSET_TO_LENGTH( INT32 nOffset )
	{
		return ( ( FLOAT )( nOffset ) * ( 200.0f / m_fScale ) );
	}
	INT32	REGION_LENGTH_TO_OFFSET( FLOAT fLength )
	{
		return ( INT32 )( ( FLOAT )( fLength ) / ( 200.0f / m_fScale ) );
	}

public:

	CBitmap	m_memBitmap	;
	CDC		m_memDC		;
	CRect	m_memRect	;
	BOOL	IsUpdateMemDC();	// 출력 싸이즈가 변경되었는지 확인한다.
	void	UpdateMemDC();		// 메모리 DC 재생성.

	int		m_nOffsetX	;
	int		m_nOffsetZ	;
	FLOAT	m_fScale	;

	BOOL		m_bDivisionGrid;

	// 계산 용 백업..
	BOOL		m_bRbuttonDown	;	// 오른쪽 버튼 누르고 있음..
	CPoint		m_pointLastPress;

	enum	DISPLAYMODE
	{
		DEFAULT,
		SKYBLOCK
	};
	
protected:
	DISPLAYMODE	m_eDisplayMode;
public:
	DISPLAYMODE	GetDisplayMode(){ return m_eDisplayMode; }
	void		SetDisplayMode( DISPLAYMODE eMode )
	{
		if( m_eDisplayMode != eMode ) Invalidate( FALSE );
		m_eDisplayMode = eMode; 
	}
public:

	POINT		m_aDivisionInRender[ 256 ];
	UINT32		m_nDivisionInRender;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegionViewStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRegionViewStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRegionViewStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIONVIEWSTATIC_H__FF20FC59_0062_4078_AFDB_86995B5BE97C__INCLUDED_)
