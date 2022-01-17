// AcUIWorldmap.h: interface for the AcUIWorldmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACUIWORLDMAP_H__1668CB36_2AEF_4857_8BFD_399E0F29E73E__INCLUDED_)
#define AFX_ACUIWORLDMAP_H__1668CB36_2AEF_4857_8BFD_399E0F29E73E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AcUIBase.h"
#include "ContainerUtil.h"

class AgcmMinimap;

class AcUIWorldmap : public AcUIBase  
{
public:
	/*
	enum WORLDPOSITIONINDEX
	{
		NOMAP		= -1	,
		THOULAN		= 0		,
		TRILGARD	= 1		,
		WORLDMAP_MAX= 2		
	};
	*/

	struct stWorldMap
	{
		// 이미지의 포함 범위의 좌표..
		INT32		nIndex	;
		FLOAT		xStart	;
		FLOAT		xEnd	;
		FLOAT		zStart	;
		FLOAT		zEnd	;
		FLOAT		fStepSize;

		INT32	nMapItemID	;	// -1 이면 NO ITEM . 아이템 없이도 표시됨.
		string		strName	;

		stWorldMap() : nMapItemID( -1 )
		{
			nIndex				= 0		;
			fStepSize = xStart	= xEnd = zStart = zEnd = 0.0f;

			strName				= "Untitled";
		}
	};

	BOOL	m_bThereIsNoWorldMapTemplate;
	std::vector< stWorldMap >	m_arrayWorldMap;	// 계배열을 저장
	//stWorldMap			m_WorldMapData[ WORLDMAP_MAX ]	;
	INT32				m_nCurrentWorldMapIndex			;
						// 현재 로딩중인 월드맵.
	BOOL				m_bIsOpenWorldMap;

	stWorldMap	*		GetCurrentWorldMap()
	{
		if( m_nCurrentWorldMapIndex < 0				||
			m_nCurrentWorldMapIndex >= ( int ) m_arrayWorldMap.size()	)	return NULL;
		else
			return &m_arrayWorldMap[ m_nCurrentWorldMapIndex ];
	}

	// 맵정보 설정..
	BOOL				SetWorldMapInfo(	ApmMap::WorldMap	* pWorldMap );

	BOOL				SetWorldMap			( AuPOS * pPos );
	BOOL				SetWorldMapByItem	( INT32 nTID );
	BOOL				SetWorldMapIndex	( INT32 nWorldMap );
		// 해당 포지션에 월드맵이 있는지 체크하고 , 
		// 로드 한경우 TRUE, 없거나 로드 에러면 FALSE 리턴

	AgcmMinimap	*		m_pAgcmMinimap		;
	AgpmCharacter *		m_pAgpmCharacter	;

	inline INT32 GetMMPosX( FLOAT fPosX );
	inline INT32 GetMMPosZ( FLOAT fPosZ );

protected:
	RsMouseStatus		m_prevMouseState	;

public:
	// 모듈 포인터 지정해둠.
	void	SetMinimapModule( AgcmMinimap * pMinimapModule ){ m_pAgcmMinimap= pMinimapModule;}
	void	SetCharacterModule( AgpmCharacter * pCharacterModule ){ m_pAgpmCharacter= pCharacterModule;}

	virtual	void OnWindowRender	()	;
	virtual BOOL OnInit			()	;

	virtual BOOL OnPostInit		()	;
	
	AcUIWorldmap();
	virtual ~AcUIWorldmap();

	virtual	BOOL OnLButtonDblClk	( RsMouseStatus *ms	);
	virtual	BOOL OnMouseMove		( RsMouseStatus *ms	);
	virtual BOOL OnRButtonUp		( RsMouseStatus *ms	);

public :
	enum AuCastleOwnerType
	{
		CastleOwner_Invalid							= 0,
		CastleOwner_Human,
		CastleOwner_Orc,
		CastleOwner_MoonElf,
		CastleOwner_DragonScion,
		CastleOwner_Archlord,
	};

	enum AuCastleInfoType
	{
		CastleInfo_UnKnown							= 0,
		CastleInfo_OwnerGuildName,
		CastleInfo_GuildMasterName,
		CastleInfo_GuildMark,
	};

private :
	float													m_fAlpha;
	float													m_fMinimumAlphaLimit;

	INT32													m_nEventGetSlideBar;

	INT32													m_nEventGetBtnGuildMarkHuman;
	INT32													m_nEventGetBtnGuildMarkHuman2;
	INT32													m_nEventGetBtnGuildMarkHuman3;

	INT32													m_nEventGetBtnGuildMarkOrc;
	INT32													m_nEventGetBtnGuildMarkOrc2;
	INT32													m_nEventGetBtnGuildMarkOrc3;

	INT32													m_nEventGetBtnGuildMarkMoonElf;
	INT32													m_nEventGetBtnGuildMarkMoonElf2;
	INT32													m_nEventGetBtnGuildMarkMoonElf3;

	INT32													m_nEventGetBtnGuildMarkDragonSion;
	INT32													m_nEventGetBtnGuildMarkDragonSion2;
	INT32													m_nEventGetBtnGuildMarkDragonSion3;

	INT32													m_nEventGetBtnGuildMarkArchlord;
	INT32													m_nEventGetBtnGuildMarkArchlord2;
	INT32													m_nEventGetBtnGuildMarkArchlord3;

	AcUIScroll*												m_pScrollAlpha;

	AcUIButton*												m_pCastleGuildMarkHuman;
	AcUIButton*												m_pCastleGuildMarkHuman2;
	AcUIButton*												m_pCastleGuildMarkHuman3;

	AcUIButton*												m_pCastleGuildMarkOrc;
	AcUIButton*												m_pCastleGuildMarkOrc2;
	AcUIButton*												m_pCastleGuildMarkOrc3;

	AcUIButton*												m_pCastleGuildMarkMoonElf;
	AcUIButton*												m_pCastleGuildMarkMoonElf2;
	AcUIButton*												m_pCastleGuildMarkMoonElf3;

	AcUIButton*												m_pCastleGuildMarkDragonSion;
	AcUIButton*												m_pCastleGuildMarkDragonSion2;
	AcUIButton*												m_pCastleGuildMarkDragonSion3;

	AcUIButton*												m_pCastleGuildMarkArchlord;
	AcUIButton*												m_pCastleGuildMarkArchlord2;
	AcUIButton*												m_pCastleGuildMarkArchlord3;

	AgcdUIUserData*											m_pcdUIUserData;
	CHAR													m_strDisplayText[ 32 ];
	BOOL													m_bViewCastleInfo;

public :
	void				OnAddEvent							( void* pUIManager );
	void				OnAddFunction						( void* pUIManager );
	void				OnAddDisplay						( void* pUIManager );
	void				OnAddUserData						( void* pUIManager );
	void				OnGetControls						( void* pUIManager );
	void				OnRefreshUserData					( void* pUIManager );

	void				OnUpdateWorldMapAlpha				( void );
	void				OnUpdateCastleInfo					( AuCastleOwnerType eCastleType, AuCastleInfoType eInfoType, void* pData );
	void				OnUpdateGuildMarkTexture			( AuCastleOwnerType eCastleType );

public :
	void				SetScrollBar						( AcUIScroll* pScroll );
	void				RenderButton						( AcUIButton* pBtn, void* pTexture );

	void*				GetCastleInfomation					( AuCastleOwnerType eCastleType );
	void*				GetCastleOwnerGuild					( AuCastleOwnerType eCastleType );
	CHAR*				GetCastleOwnerGuildName				( AuCastleOwnerType eCastleType );
	CHAR*				GetCastleOwnerGuildMasterName		( AuCastleOwnerType eCastleType );
	void*				GetCastleOwnerGuildMarkBG			( AuCastleOwnerType eCastleType );
	void*				GetCastleOwnerGuildMarkPattern		( AuCastleOwnerType eCastleType );
	void*				GetCastleOwnerGuildMarkSimbol		( AuCastleOwnerType eCastleType );

public :
	static BOOL			CB_GetSlideBar						( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_GetBtnCastleHuman				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleHuman2				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleHuman3				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_GetBtnCastleOrc					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleOrc2					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleOrc3					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_GetBtnCastleMoonElf				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleMoonElf2				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleMoonElf3				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_GetBtnCastleDragonSion			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleDragonSion2			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleDragonSion3			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_GetBtnCastleArchlord				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleArchlord2			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_GetBtnCastleArchlord3			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnDisplayOwnerGuildHuman			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayOwnerNameHuman			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );

	static BOOL			CB_OnDisplayOwnerGuildOrc			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayOwnerNameOrc			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );

	static BOOL			CB_OnDisplayOwnerGuildMoonElf		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayOwnerNameMoonElf		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );

	static BOOL			CB_OnDisplayOwnerGuildDragonScion	( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayOwnerNameDragonScion	( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );

	static BOOL			CB_OnDisplayOwnerGuildArchlord		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayOwnerNameArchlord		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
};

#endif // !defined(AFX_ACUIWORLDMAP_H__1668CB36_2AEF_4857_8BFD_399E0F29E73E__INCLUDED_)
