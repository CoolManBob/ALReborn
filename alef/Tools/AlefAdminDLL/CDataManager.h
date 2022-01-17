#ifndef __CLASS_DATA_MANAGER_H__
#define __CLASS_DATA_MANAGER_H__



#include "ContainerUtil.h"
#include "AgpdAdmin.h"
#include "AgpmAdmin.h"



class CDataManager
{
private :
	void*													m_pAgpmAdmin;
	void*													m_pAgcmAdmin;

	BOOL													m_bIsUseAdmin;
	BOOL													m_bIsDialogOpen;

	stAgpdAdminInfo											m_stSelfAdminInfo;
	stAgpdAdminSearch										m_stOldSearchData;
	stAgpdAdminCharData										m_stReceivedCharacterData;
	stAgpdAdminPickingData									m_stSelectCharacterData;

	DWORD													m_dwLastRequestCurrentUserTickCount;
	ContainerVector< stAgpdAdminServerInfo >				m_vecServerInfo;

public :
	CDataManager( void );
	~CDataManager( void );

public :
	BOOL				OnRegisterCallBack					( void );
	BOOL				OnIdle								( void );

	BOOL				OpenMainDlg							( void );
	BOOL				CloseMainDlg						( void );
	
	BOOL				OnSelectCharacter					( INT32 nType, INT32 nCID );
	BOOL				OnSelectGround						( void* pPos );

	BOOL				OnAddAdmin							( void* pCharacter, void* pAdminInfo );
	
	BOOL				OnAddCharacterData					( void* pCharacter );
	BOOL				OnAddCharacterDataSub				( void* pCharacterSub );
	BOOL				OnAddCharacterSkill					( void* pSkill );
	BOOL				OnAddCharacterItem					( void* pItem );

	BOOL				OnAddServerInfo						( void* pServerInfo );

public :
	BOOL				SetAdminModule						( void* pModule );
	void*				GetAgcmAdmin						( void ) { return m_pAgcmAdmin; }
	void*				GetAgpmAdmin						( void ) { return m_pAgpmAdmin; }

	BOOL				ProcessRequestCurrentUserCount		( void );

	void				SetUseAdmin							( BOOL bIsUse ) { m_bIsUseAdmin = bIsUse; }
	BOOL				GetUseAdmin							( void ) { return m_bIsUseAdmin; }

	void				SetSelfAdminInfo					( void* pSelfAdminInfo );

	void				SetOldSearchData					( stAgpdAdminSearch* pSearchData );
	stAgpdAdminSearch*	GetOldSearchData					( void ) { return &m_stOldSearchData; }

	void				SetReceivedCharData					( stAgpdAdminCharData* pCharData );
	void				SetReceivedCharDataSub				( stAgpdAdminCharDataSub* pCharDataSub );

	BOOL				SetItemData							( void* pItem, void* pItemData );

	BOOL				FindServerInfo						( void* pServerInfo );
	CHAR*				GetUIMessageString					( CHAR* pKeyString );

public :
	static BOOL			CBSelectCharacter					( void* pData, void* pClass, void* pCustData );
	static BOOL			CBSelectGround						( void* pData, void* pClass, void* pCustData );

	static BOOL			CBAddAdmin							( void* pData, void* pClass, void* pCustData );

	static BOOL			CBReceiveSearchResult				( void* pData, void* pClass, void* pCustData );

	static BOOL			CBReceiveCharacterData				( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveCharacterDataSub			( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveCharacterModifiied			( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveCharacterSkill				( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveCharacterItem				( void* pData, void* pClass, void* pCustData );

	static BOOL			CBReceiveServerInfo					( void* pData, void* pClass, void* pCustData );

	static BOOL			CBDlgSendSearchCharacter			( void* pData, void* pClass, void* pCustData );

	static BOOL			CBNumCurrentUser					( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveCustom						( void* pData, void* pClass, void* pCustData );
	static BOOL			CBReceiveItemOperationResult		( void* pData, void* pClass, void* pCustData );

public :
	BOOL				SendRequestCurrentUserCount			( void );
	BOOL				SendSearchCharacter					( void* pSearchData );
	BOOL				SendCharacterModification			( void* pModify, void* pString = NULL );
	BOOL				SendBan								( void* pBan );
	BOOL				SendItemOperation					( void* pItemOperation );
	BOOL				SendGuildOperation					( void* pModify );
	BOOL				SendTitleOperation					( void* pTitleOperation );

private :
	INT32				_GetFactorValue						( void* pFactor, INT32 nFactorType, INT32 nFactorSubType1 = 0, INT32 nFactorSubType2 = 0 );
};



#endif