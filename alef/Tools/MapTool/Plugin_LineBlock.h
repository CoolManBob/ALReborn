#pragma once

#include "UITileList_PluginBase.h"
#include "myengine.h"
#include <vector>

using namespace std;

// CPlugin_LineBlock

#define	LINE_BLOCK_FILE_VERSION	0x0001

class CPlugin_LineBlock : public CUITileList_PluginBase
{
	DECLARE_DYNAMIC(CPlugin_LineBlock)

public:
	CPlugin_LineBlock();
	virtual ~CPlugin_LineBlock();

	CUITileList_PluginBase * m_pPluginBlocking;

public:

	struct AuLineMaptool : public AuLine
	{
	public:
		BOOL	bSelected;

		enum	TYPE
		{
			SERVER,
			CLIENTONLY
		};

		TYPE	eType;
		AuLineMaptool():bSelected( FALSE ),eType( SERVER ) {}
	};

	// Data Structure
	class Division
	{
	public:
		INT32					nIndex		;
		vector< AuLineMaptool >	vecLine		;

		BOOL	Load();
		BOOL	Save( char * pFileName );
	};

	vector< Division * >		m_vecDivision;

	Division *		GetDivision( INT32 nDivision );
	Division *		AddDivision( INT32 nDivision );

public:
	void	CleanUp();

	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();

	BOOL	Save( INT32 nDivision , char * pFileName );
	BOOL	Load( INT32 nDivision );

	static BOOL	CBDivisionSaveCallback ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBDivisionLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBExportCallback ( DivisionInfo * pDivisionInfo , PVOID pData );

public:
	FLOAT	m_fBlockingVisibleHeight;
	FLOAT	m_fBlockingAlphaValue	;

	BOOL			m_bDrawingNow			;
	AuLineMaptool	m_lineDrawing			;

	BOOL	m_bGridSnap				;
	BOOL	m_bBackfaceCull			;
	BOOL	m_bShowServer			;
	BOOL	m_bShowClient			;
	BOOL	m_bShowGeometryBlock	;

	// Selection Info
	BOOL	m_bSelectMode			;
	BOOL	m_bExportServer			;

	/*
	vector< AuLine >::iterator	m_iterVecLineBegin	;
	vector< AuLine >::iterator	m_iterVecLineEnd	;
	*/

	void	ClearSelection();

	enum TYPELINE
	{
		NORMAL_BLOCKING	,
		SELECTING_NOW	,
		SELECTED		
	};

	void	DrawBlocking( AuLineMaptool * pLine , TYPELINE eType = NORMAL_BLOCKING );
	BOOL	CollisionCheck( AuLineMaptool * pLine , RsMouseStatus *ms , FLOAT * pfDistance );

	// For UI
	CRect	m_rectBackfaceCull	;
	CRect	m_rectGridSnap		;
	CRect	m_rectServerData	;
	CRect	m_rectClientOnly	;
	CRect	m_rectGeomtryBlock	;

public:
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL OnWindowRender			();
	virtual BOOL OnExport				( BOOL bServer );
	virtual	BOOL OnCleanUpData			();

protected:
	//{{AFX_MSG(CPlugin_Water)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


