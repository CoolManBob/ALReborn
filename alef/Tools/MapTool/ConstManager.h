// ConstManager.h: interface for the CConstManager class.
//
// 프로그램에서 사용하는 Constance Value를 정리한다.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_)
#define AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConstManager
{
public:
// 저장돼는것
	float	m_fSectorWidth			;	// 섹터 넓이 
	int		m_nHoriSectorCount		;	// 가로 세로 갯수.
	int		m_nVertSectorCount		;
	int		m_nSectorDepth			;	// 섹터 하나의 디테일.

// 저장 안돼는것.
	char	m_strCurrentDirectory[ 1024 ];
	int		m_nPreviewMapSelectSize	;	// 

	int		m_nGeometryLockMode		;
	int		m_nVertexColorLockMode	;
	
	BOOL	m_bCameraBlocking		;	// 카메라가 땅에 걸리게 하기.
	
	BOOL	m_bShowCollisionAtomic	;	// 콜리젼아토믹 화면에 표시.

	CBitmap	m_bitmapLoadNormal		;
	CBitmap	m_bitmapLoadExport		;
	CBrush	m_brushLoadNormal		;
	CBrush	m_brushLoadExport		;

	// 오브젝트 바운딩 스피어 표시
	BOOL m_bShowObjectBoundingSphere	;
	// 오브젝트 콜리젼 아토믹 표시
	BOOL m_bShowObjectCollisionAtomic;
	// 오브젝트 피킹 아토믹 표시
	BOOL m_bShowObjectPickingAtomic	;
	// 오브젝트 실  포리건 정보 표시
	BOOL m_bShowObjectRealPolygon	;
	// 오브젝트 창에서 옥트리 정보 표시
	BOOL m_bShowOctreeInfo			;

// 레지스트리에 저장돼는것..

	// 마고자 (2004-06-28 오후 6:27:09) : 로드 타입..
	DWORD	m_nGlobalMode			;	// 노멀인지 익스포트인지.
	
	DWORD	m_nLoading_range_x1		;
	DWORD	m_nLoading_range_y1		;
	DWORD	m_nLoading_range_x2		;
	DWORD	m_nLoading_range_y2		;

	BOOL	m_bUsePolygonLock		;
	DWORD	m_bUseBrushPolygon		;
	
	//Save 설정..
	DWORD	m_bMapInfo			;
	DWORD	m_bTileVertexColor	;
	DWORD	m_bTileList			;
	DWORD	m_bObjectList		;

	DWORD	m_bObjectTemplate	;
	DWORD	m_bDungeonData		;
	

	DWORD	m_bUseEffect		;
	
	// 스카이 설정..
	FLOAT	m_fSkyWidth			;
	FLOAT	m_fFogFarClip		;
	FLOAT	m_fFogDistance		;
	DWORD	m_bUseFog			;
	DWORD	m_bUseSky			;

	DWORD	m_nLoadRange_Data	;
	DWORD	m_nLoadRange_Rough	;
	DWORD	m_nLoadRange_Detail	;

public:

	// 익스포트 자동화에서 사용함.
	struct	ExportAutomation
	{
		BOOL	bCompactData	;
		BOOL	bMapDetail		;
		BOOL	bObject			;
		BOOL	bMapRough		;
		BOOL	bTile			;
		BOOL	bMinimap		;
		BOOL	bServer			;

		CString	strFilename		;
		INT32	nDivision		;

		ExportAutomation()
		{
			bCompactData	= FALSE	;
			bMapDetail		= FALSE	;
			bObject			= FALSE	;
			bMapRough		= FALSE	;
			bTile			= FALSE	;
			bMinimap		= FALSE ;
			bServer			= FALSE	;
			nDivision		= 0		;
		}
	};
	
	ExportAutomation	m_stExportAutomateInfo;

public:

	
	BOOL	Save	();
	BOOL	Load	();

	BOOL	Update	();	// 뭔가 설정이 바뀌면 호출 요망.

	BOOL	StoreRegistry();

	void	LoadBmpResource();
	
	CConstManager();
	virtual ~CConstManager();
};

extern CConstManager g_Const;

#endif // !defined(AFX_CONSTMANAGER_H__5800F305_4B80_48E4_BA5F_44D250EB6DA1__INCLUDED_)
