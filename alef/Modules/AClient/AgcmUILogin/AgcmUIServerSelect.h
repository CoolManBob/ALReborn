#ifndef __AGCM_UI_SERVER_SELECT_H__
#define __AGCM_UI_SERVER_SELECT_H__




#include "ContainerUtil.h"
#include "ApDefine.h"
#include "ApBase.h"
#include "AgcdUIManager2.h"


#define	SERVER_LIST_MAX									50


struct stServerInfomation
{
	INT32												m_nServerIndex;
	CHAR												m_strServerName[ 64 ];
	void*												m_ppdWorld;
	BOOL												m_bHaveMyCharacter;
	INT16												m_nServerState;
	INT32												m_nServerAgeRate;
	INT32												m_nServerOpenState;

	stServerInfomation( void )
		: m_nServerIndex( -1 ), m_ppdWorld( NULL ), m_bHaveMyCharacter( FALSE ),
		m_nServerState( 0 ), m_nServerAgeRate( 0 ), m_nServerOpenState( 0 )

	{
		memset( m_strServerName, 0, sizeof( CHAR ) * 64 );
	}
};

struct stServerGroup
{
	INT32												m_nServerGroupIndex;
	CHAR												m_strServerGroupName[ 64 ];
	ContainerMap< INT32, stServerInfomation >			m_mapServer;
	BOOL												m_bIsTreeOpen;

	stServerGroup( void )
		: m_nServerGroupIndex( -1 ), m_bIsTreeOpen( TRUE )
	{
		memset( m_strServerGroupName, 0, sizeof( CHAR ) * 64 );
		m_mapServer.Clear();
	}

	BOOL					AddServer					( INT32 nServerIndex, void* pWorld, BOOL bHaveMyCharacter, INT16 nServerState, INT32 nAgeRate, INT32 nOpenState );
	BOOL					DeleteServer				( INT32 nServerIndex );
	stServerInfomation*		GetServer					( INT32 nServerIndex );
	stServerInfomation*		GetServerByIndex			( INT32 nIndex );
	stServerInfomation*		GetServerByName				( CHAR* pServerName );
};

class AgcmUIServerSelect
{
protected :
	ContainerMap< INT32, stServerGroup >				m_mapServerGroup;

	INT32												m_nSelectServerIndex;
	INT32												m_nServerIndexList[ SERVER_LIST_MAX ];

	void*												m_pcmUILogin;
	void*												m_pcdUserData;

	BOOL												m_bIsSelectDialogOpen;
	BOOL												m_bIsServerSelectComplete;

	INT32												m_nPrevUpdateTime;

public :
	AgcmUIServerSelect( void );
	virtual ~AgcmUIServerSelect( void );

public :
	BOOL					OnInitialize				( void* pUILogin );

	BOOL					OnAddUserData				( void* pUIManager );
	BOOL					OnAddFunction				( void* pUIManager );
	BOOL					OnAddDisplay				( void* pUIManager );
	BOOL					OnAddBoolean				( void* pUIManager );

	BOOL					OnRefresh					( void );
	BOOL					OnClear						( void );
	BOOL					OnLoadServerList			( void );
	BOOL					OnUpdateServerInfo			( INT32 nClockCount );

	BOOL					OnSelectServer				( INT32 nServerIndex, BOOL bIsSelect );
	BOOL					OnClickListGroup			( INT32 nClickIndex );
	BOOL					OnClickListServer			( INT32 nClickIndex );

	BOOL					OnChangeTreeState			( INT32 nGroupIndex, BOOL bIsOpen );
	BOOL					OnChangeServerState			( INT32 nServerIndex, INT16 nServerState );
	BOOL					OnChangeServerCharacter		( INT32 nServerIndex, BOOL bHaveMyCharacter );
	BOOL					OnChangeServerAgeRate		( INT32 nServerIndex, INT32 nAgeRate );
	BOOL					OnChangeServerOpen			( INT32 nServerIndex, INT32 nOpenState );

	BOOL					OnEnterCharacterSelect		( void* pWorld );
	BOOL					OnServerClose				( void );

public :
	void*					GetCurrentApgdWorld			( void );
	void*					GetServerAgpdWorld			( INT32 nServerIndex );
	INT32					GetGroupIndexByListIndex	( INT32 nListIndex );

	INT32					GetCurrentSelectIndex		( void ) { return m_nSelectServerIndex; }
	void					SetCurrentSelectIndex		( INT32 nIndex );

	BOOL					GetIsServerSelectDialogOpen	( void ) { return m_bIsSelectDialogOpen; }
	void					SetIsServerSelectDialogOpen	( BOOL bIsOpen ) { m_bIsSelectDialogOpen = bIsOpen; }

	BOOL					GetIsServerSelectComplete	( void ) { return m_bIsServerSelectComplete; }
	void					SetIsServerSelectComplete	( BOOL bIsComplete ) { m_bIsServerSelectComplete = bIsComplete; }

	CHAR*					GetServerGroupName			( INT32 nIndex );
	CHAR*					GetUIMessage				( CHAR* pMsg );

protected :
	BOOL					_DisableServerGroupFlag		( INT32 nServerIndex );
	BOOL					_ClearServerList			( INT32 nListIndex );

	stServerGroup*			_GetServerGroup				( INT32 nServerGroupIndex );
	stServerInfomation*		_GetServer					( INT32 nServerIndex );


public :
	static BOOL				CB_OnSelectServer			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CB_OnClickList				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CB_OnCloseServer			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CB_OnClickTree				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL				CB_OnDisplayServerName		( void* pClass, void* pData, AgcdUIDataType eType, INT32 lID, CHAR* pDisplay, INT32* pValue, AgcdUIControl* pControl );
	static BOOL				CB_OnDisplayServerState		( void* pClass, void* pData, AgcdUIDataType eType, INT32 lID, CHAR* pDisplay, INT32* pValue, AgcdUIControl* pControl );

	static BOOL				CB_IsActiveBtnBack			( void* pClass, void* pData, AgcdUIDataType eType, AgcdUIControl* pControl );
	static BOOL				CB_IsClickedBtnTree			( void* pClass, void* pData, AgcdUIDataType eType, AgcdUIControl* pControl );
};




#endif