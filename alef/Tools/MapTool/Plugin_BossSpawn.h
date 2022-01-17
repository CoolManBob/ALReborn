#pragma once

#include "MyEngine.h"
#include "UITileList_PluginBase.h"
#include <vector>

// CPlugin_BossSpawn

class CPlugin_BossSpawn : public CUITileList_PluginBase , ApModule
{
	DECLARE_DYNAMIC(CPlugin_BossSpawn)

public:
	CPlugin_BossSpawn();
	virtual ~CPlugin_BossSpawn();

	static CPlugin_BossSpawn * m_spThis;
	static ApModule * GetClass()
	{
		if( NULL == CPlugin_BossSpawn::m_spThis )
			CPlugin_BossSpawn::m_spThis = new CPlugin_BossSpawn;
		return static_cast<ApModule*>( CPlugin_BossSpawn::m_spThis );
	}

public:
	int	m_nCharacterIDCounter;
	// 케릭터 아이디용.. 하나씩 증가시키면서 사용함.
	AgpdCharacter	*	m_pCurrentCharacter	;
	RpClump			*	m_pSelectedClump	;

	CComboBox		m_wndCombo			;

	RsMouseStatus	m_PrevMouseStatus	;
	BOOL			m_bLButtonDown		;

	// 팝업 메뉴 관리용.
	AgpdCharacter	*	m_pPopupCurrentCharacter	;
	RpClump			*	m_pPopupSelectedClump		;

	INT32			m_nLastReadSpawnCharacterID	;

	struct Spawn
	{
		INT32	nOID;	// 올라간 오브젝트 ID
		INT32	nCID;	// 올라간 케릭터 ID

		INT32	nTID;	// 케릭터 템플릿 ID

		Spawn() : nOID( 0 ) , nCID( 0 ) , nTID( 0 ) {}

		void	Delete(){ nOID = 0; nCID = 0; nTID = 0; }

		ApdObject		* GetObject		();
		AgpdCharacter	* GetCharacter	();
	};

	vector< Spawn >	m_vecSpawn;
	Spawn * GetSpawn( INT32 nCID )
	{
		for( int i = 0 ; i < static_cast<int> ( m_vecSpawn.size() ) ; i ++ )
		{
			if( m_vecSpawn[ i ].nCID == nCID ) return &m_vecSpawn[ i ];
		}
		return NULL;
	}

	Spawn * GetSpawnByObject( INT32 nOID )
	{
		for( int i = 0 ; i < static_cast<int> ( m_vecSpawn.size() ) ; i ++ )
		{
			if( m_vecSpawn[ i ].nOID == nOID ) return &m_vecSpawn[ i ];
		}
		return NULL;
	}

	// 새로 스폰 오브젝트를 생성할때 디폴트 값으로 오브젝트 추가와 이벤트 등록.
	BOOL	AddSpawnObject( AgpdCharacter * pCharacter );
	// 스폰오브젝트가 로드된후 , 그 정보로 케릭터를 추가하기 위한것.
	BOOL	AddCharacter( ApdObject * pObject , INT32 nTemplateID );

	// 위치,방향의 변경 정보를 가지고 스폰정보와 싱크를 맞춤.
	BOOL	UpdateCharacter( AgpdCharacter * pCharacter );

	// 케릭터를 삭제하면서 , 같이 등록된 스폰 오브젝트도 삭제함.
	// m_vecSpawn에는 두 인덱스를 0으로 설정해둬서 나중에 UpdateList 에서 삭제돼게함.
	BOOL	DeleteCharacter( AgpdCharacter * pCharacter );
	
	// m_vecSpawn에서 사라진 오브젝트들을 정리한다.
	void	UpdateList();

	// 세이브 로드
	static BOOL CBCheckSpawn(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReadSpawn(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSave ( DivisionInfo * pDivisionInfo , PVOID pData );
	static BOOL	CBLoad ( DivisionInfo * pDivisionInfo , PVOID pData );

	// NPC정보 스트리밍..
	static BOOL	BossInfoStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	BossInfoStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
public:

	virtual	BOOL OnSelectedPlugin		();
	virtual	BOOL OnDeSelectedPlugin		();
	virtual	BOOL OnLButtonDownGeometry	( RwV3d * pPos );
	virtual	BOOL OnLButtonUpGeometry	( RwV3d * pPos );
	virtual BOOL OnMouseMoveGeometry	( RwV3d * pPos );
	virtual	BOOL OnSelectObject			( RpClump * pClump );
	virtual BOOL OnQuerySaveData		( char * pStr );
	virtual BOOL OnSaveData				();
	virtual BOOL OnLoadData				();
	virtual	BOOL OnCleanUpData			();
	virtual BOOL Window_OnKeyDown		( RsKeyStatus *ks	);

	virtual	BOOL Window_OnLButtonDown	( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonUp		( RsMouseStatus *ms );
	virtual BOOL Window_OnMouseMove		( RsMouseStatus *ms );
	virtual	BOOL Window_OnLButtonDblClk	( RsMouseStatus *ms );
	virtual	BOOL Window_OnRButtonDown	( RsMouseStatus *ms );

	virtual BOOL OnWindowRender			();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


