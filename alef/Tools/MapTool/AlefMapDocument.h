// AlefMapDocument.h: interface for the CAlefMapDocument class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_)
#define AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAlefMapDocument  
{
public:
	void	NewDocument	();
	BOOL	Save		();
	BOOL	SaveAs		();
	BOOL	Load		( CString filename );

	BOOL	SetModified	( BOOL bModify );
	BOOL	IsModified	();

public:
	// Operations..
	void	ToggleGrid	();
	
	// View Option..
	
	
public:
	// 저장돼지 않는 설정들.
	int		m_nSelectedDetail		;

	BOOL	m_bShowObjectBlocking	;	// 오브젝트  블러킹 표시 온오프.
	BOOL	m_bShowGeometryBlocking	;	// 지형 블러킹  표시 온오프.

	BOOL	m_bShowObject			;	// 오브젝트 표시.

	bool	m_bUseFirstPersonView	;	// 1인칭 시점 뷰 사용.
	BOOL	m_bFPSEditMode			;	// 1인칭 시점에서 에디트 사용..
	
	int		m_nSelectedMode			;	// 현재 작업하고 있는 모드 설정을 얻어낸다.
	int		m_nShowGrid				;	// 그리드 표시

	int		m_nCurrentTileLayer		;	// 작업중인 타일 레이어.

	float	m_fBrushRadius			;	// 브러시의 반지름.
	int		m_nBrushType			;	// 현재 사용중인 브러시 타입.
	
	UINT32	m_uRunSpeed				;

	FLOAT	m_fCameraMovingSpeed	;	
	
	RwMatrix	m_MatrixCamera		;	// 메인카메라 위치 저장함..
	void		SaveCameraPosition		();
	void		RestoreCameraPosition	();

	// 서브디비전 이전 설정 저장..
	int		m_nSubDivisionDetail	;
	BOOL	m_bSubDivisionTiled		;
	BOOL	m_bSubDivisionUpperTiled;

	// 타일 설정
	AuList< int >	m_listSelectedTileIndex	;
	int				GetSelectedTileIndex()	;
	int				m_nCurrentAlphaTexture	;	// 현재 선택중인 알파 텍스쳐.


	bool	m_bToggleShadingMode;
	BOOL	m_bShowSystemObject	;
	UINT32	m_uAtomicFilter		;	// 오브젝트 필터 옵션..
	UINT32	m_uFilterEventID	;
	enum	ATOMICFILTER
	{
		AF_GEOMTERY			= 0x0001	,	// 지형표시..
		AF_OBJECTALL		= 0x0002	,
		AF_SYSTEMOBJECT		= 0x0004	,	// 시스테 오브젝트 표시..
		AF_OBJECT_RIDABLE	= 0x0008	,
		AF_OBJECT_BLOCKING	= 0x0010	,
		AF_OBJECT_EVENT		= 0x0020	,
		AF_OBJECT_GRASS		= 0x0040	,
		AF_OBJECT_OTHERS	= 0x0080	,
		AF_OBJECT_DOME		= 0x0100	,
		AF_SHOWALL			= 0x01ff	,
		AF_OBJECTMASK		= 0x01fc	,
	};

	int		m_nDWSectorSetting;

	enum	RangeSphereType
	{
		RST_NONE		,
		RST_SPAWN		,
		RST_NATURE		,
		RST_MAX
	};

	INT32	m_nRangeSphereType;

public:
	
	bool	IsInFirstPersonViewMode		() { return m_bUseFirstPersonView; }
	void	ToggleFirstPersonViewMode	( INT32	nTemplateID	= 1 ) ;

protected:
	BOOL	m_bModified		; // 모디파이드 체크.

	// 저장돼는 설정들

public:
	CAlefMapDocument();
	virtual ~CAlefMapDocument();

};

#endif // !defined(AFX_ALEFMAPDOCUMENT_H__651448DD_2163_4A1D_BFA3_3458BE6DD263__INCLUDED_)
