#ifndef __CLASS_AGCM_TITLE_H__
#define __CLASS_AGCM_TITLE_H__



#include "AgcdUIManager2.h"
#include "ContainerUtil.h"
#include <string>

#include "AgpdTitleQuest.h"


#define STRING_LENGTH_TITLE_NAME									64
#define STRING_LENGTH_TITLE_DESC									1024

#define TITLELIST_CATEGOGRY_ITEMCOUNT_MAX							30
#define TITLELIST_TITLE_ITEMCOUNT_MAX								30
#define TiTLELIST_TITLE_ITEMCOUNT_PER_PAGE							15
#define TITLELIST_BUTTON_PAGE_COUNT									5


enum eTitleUiType
{
	TitleUi_UnKnown = 0,
	TitleUi_ByNPC,
	TitleUi_ByHotKey,
	TitleUi_Mini,
};

enum eTitleStateType
{
	TitleState_UnKnown = 0,
	TitleState_Ready,
	TitleState_Running,
	TitleState_Complete,
	TitleState_Enable,
	TitleState_Disable,
	TitleState_Activate,
	TitleState_Failed,
};

struct stTitleEntry
{
	int																m_nTitleID;

	char															m_strName[ STRING_LENGTH_TITLE_NAME ];
	char															m_strDesc[ STRING_LENGTH_TITLE_DESC ];
	char															m_strDescEffect[ STRING_LENGTH_TITLE_DESC ];

	eTitleStateType													m_eState;

	int																m_nCounterCur;
	int																m_nCounterMax;

	unsigned int													m_nTimeStamp;

	stTitleEntry( void )
	{
		m_nTitleID = 0;
		memset( m_strName, 0, sizeof( char ) * STRING_LENGTH_TITLE_NAME );
		memset( m_strDesc, 0, sizeof( char ) * STRING_LENGTH_TITLE_DESC );
		memset( m_strDescEffect, 0, sizeof( char ) * STRING_LENGTH_TITLE_DESC );

		m_eState = TitleState_UnKnown;

		m_nCounterCur = 0;
		m_nCounterMax = 0;

		m_nTimeStamp = 0;
	}
};

struct stTitleRunningEntry
{
	stTitleEntry*													m_pTitle;

	stTitleRunningEntry( void )
	{
		m_pTitle = NULL;
	}
};

struct stTitleCategory
{
	char															m_strName[ STRING_LENGTH_TITLE_NAME ];
	int																m_nCategoryID;
	ContainerMap< int, stTitleEntry >								m_mapTitle;

	stTitleCategory( void )
	{
		m_nCategoryID = 0;
		memset( m_strName, 0, sizeof( char ) * STRING_LENGTH_TITLE_NAME );
	}
};

struct stTitleActivationLog
{
	char															m_strCharacterName[ 64 ];
	unsigned int													m_nActivateTime;

	stTitleActivationLog( void )
	{
		memset( m_strCharacterName, 0, sizeof( char ) * 64 );
		m_nActivateTime = 0;
	}
};

struct stTitleFontColorEntry
{
	eTitleStateType													m_eTitleState;
	DWORD															m_dwColor;

	stTitleFontColorEntry( void )
	{
		m_eTitleState = TitleState_UnKnown;
		m_dwColor = 0xFFFFFFFF;
	}
};

struct stTitleActivateTimeLimit
{
	int																m_nYear;
	int																m_nMonth;
	int																m_nDay;
	int																m_nHour;
	int																m_nMinute;
	int																m_nSecond;

	stTitleActivateTimeLimit( void )
	{
		m_nYear = 0;
		m_nMonth = 0;
		m_nDay = 0;
		m_nHour = 0;
		m_nMinute = 0;
		m_nSecond = 0;
	}
};

struct stCharacterTitleActivate
{
	int																m_nCharacterID;
	char															m_strCharacterName[ 64 ];
	int																m_nTitleID;

	stCharacterTitleActivate( void )
	{
		m_nCharacterID = 0;
		m_nTitleID = 0;
		memset( m_strCharacterName, 0, sizeof( char ) * 64 );
	}
};

class AgcmTitle
{
private :
	BOOL															m_bIsInitialized;
	ContainerMap< int, stTitleCategory >							m_mapCategory;
	ContainerVector< stTitleRunningEntry >							m_vecRunningTitle;
	
	stTitleCategory*												m_pCurrentSelectCategory;
	stTitleEntry*													m_pCurrentSelectTitle;
	stTitleEntry*													m_pCurrentSelectTitleCurrent;
	stTitleEntry*													m_pCurrentSelectTitleMini;
	stTitleEntry*													m_pCurrentActivateTitle;

	int																m_nCurrentTitleListPage;
	ContainerMap< std::string, stTitleActivationLog >				m_mapActivateLog;

	void*															m_pBtnListPage[ TITLELIST_BUTTON_PAGE_COUNT ];

	void*															m_pBtnSetTargetTitle;
	void*															m_pBtnActivateTitle;
	void*															m_pBtnCompleteTitle;

	void*															m_pEditTitlePage;
	void*															m_pEditTitleName;
	void*															m_pEditTitleInfo;
	void*															m_pEditTitleEffect;
	void*															m_pEditTitleNameCurrent;
	void*															m_pEditTitleInfoCurrent;
	void*															m_pEditTitleEffectCurrent;

	AgcdUIUserData*													m_pUserDataCategoryIndex;
	AgcdUIUserData*													m_pUserDataTitleIndex;

	int																m_nCategoryIndex[ TITLELIST_CATEGOGRY_ITEMCOUNT_MAX ];
	int																m_nTitleIndex[ TITLELIST_TITLE_ITEMCOUNT_MAX ];

	AgcdUIUserData*													m_pUserDataTitleIndexCurrent;
	int																m_nTitleIndexCurrent[ AGPDTITLE_MAX_TITLE_QUEST_NUMBER ];

	AgcdUIUserData*													m_pUserDataTitleIndexMini;
	int																m_nTitleIndexMini[ AGPDTITLE_MAX_TITLE_QUEST_NUMBER ];

	ContainerMap< eTitleStateType, stTitleFontColorEntry >			m_mapTitleFontColor;
	stTitleActivateTimeLimit										m_stTitleTimeLimit;

	ContainerMap< std::string, stCharacterTitleActivate >			m_mapCharacterTitleActivate;

public :
	AgcmTitle( void );
	virtual ~AgcmTitle( void );

// 모듈 제어 관련
public :
	BOOL					OnInitialize							( void );
	BOOL					OnUnInitialize							( void );
	BOOL					OnClearTitleState						( void );

// 데이터 로딩
public :
	BOOL					OnInitTitleSetting						( void );
	BOOL					OnLoadTitleSetting						( char* pFileName );

// 타이틀 창 제어 관련
public :
	BOOL					OnOpenUI								( eTitleUiType eUiType );
	BOOL					OnUpateUI								( eTitleUiType eUiType );
	BOOL					OnCloseUI								( eTitleUiType eUiType );

// 패킷 송신 관련
public :
	BOOL					OnSendSetTargetTitle					( void );
	BOOL					OnSendTitleActivate						( void );
	BOOL					OnSendSurrenderTitle					( eTitleUiType eUiType );
	BOOL					OnSendRequestTitleList					( void );
	BOOL					OnSendCompleteTitle						( void );

// 타이틀 창 UI 액션 관련
public :
	BOOL					OnChangeTitleState						( int nTitleID, eTitleStateType eState, void* pCharacter, BOOL bIsSelfCharacter = TRUE, BOOL bResult = TRUE, int nCounterValue = -1 );
	BOOL					OnUpdateTitleCounter					( int nTitleID, int nCounterValue );
	BOOL					OnUpdateTitleTimeStamp					( char* pCharacterName, int nTitleID, unsigned int nTimeStamp );

	BOOL					OnSelectCategory						( char* pCategoryName );
	BOOL					OnSelectCategoryByID					( int nID );
	BOOL					OnSelectCategoryByIndex					( int nIndex );

	BOOL					OnSelectTitle							( char* pTitleName );
	BOOL					OnSelectTitleByID						( int nID );
	BOOL					OnSelectTitleByIndex					( int nIndex );
	BOOL					OnSelectTitleCurrentByIndex				( int nIndex );
	BOOL					OnSelectTitleMiniByIndex				( int nIndex );

	BOOL					OnSetSelfCharacter						( void* pCharacter );

	BOOL					OnActivateTitle							( stTitleEntry* pTitle, void* pCharacter, BOOL bIsSelfCharacter );
	BOOL					OnActivateTitle							( int nTitleID, void* pCharacter, BOOL bIsSelfCharacter );

	BOOL					OnDeActivateTitle						( stTitleEntry* pTitle, void* pCharacter, BOOL bIsSelfCharacter );
	BOOL					OnDeActivateTitle						( int nTitleID, void* pCharacter, BOOL bIsSelfCharacter );

	BOOL					OnClearTitleStateAll					( void );
	BOOL					OnClearTitleIDBoard						( void );
	BOOL					OnClearTitleCategorySelection			( void );
	BOOL					OnClearTitleSelection					( eTitleUiType eType );
	BOOL					OnClearTitleDescription					( eTitleUiType eType );

	BOOL					OnUpdateTitleTextBoard					( void* pCharacter, char* pTitleName, DWORD dwColor, BOOL bIsSelfCharacter );

// 내부처리용
private :
	BOOL					_RegisterCallBack						( void );
	BOOL					_GetDialogControls						( void );
	BOOL					_LoadAllTitleCategoryInfo				( void );

	stTitleCategory*		_GetCategoryByID						( int nCategoryID );
	stTitleCategory*		_GetCategoryByName						( char* pCategoryName );
	stTitleCategory*		_GetCategoryByIndex						( int nIndex );

	stTitleEntry*			_GetTitleByID							( int nTitleID );
	stTitleEntry*			_GetTitleByID							( stTitleCategory* pCategory, int nTitleID );
	stTitleEntry*			_GetTitleByName							( stTitleCategory* pCategory, char* pTitleName );
	stTitleEntry*			_GetTitleByIndex						( stTitleCategory* pCategory, int nIndex );
	stTitleEntry*			_GetTitleCurrentByIndex					( int nIndex );

	void					_ParseTimeStamp							( unsigned int nTime, int* pYear, int* pMonth, int* pDay, int* pHour, int* pMinute, int* pSecond );
	unsigned int			_GetLastActivateTime					( char* pCharacterName );
	void					_SaveTitleActivateLog					( char* pCharacterName, unsigned int nTimeStamp );

	BOOL					_UpdateCategoryList						( void );
	BOOL					_UpdateTitleList						( eTitleUiType eType );
	BOOL					_UpdateTitleListPage					( void );

	BOOL					_AddRunningTitle						( stTitleEntry* pTitle );
	stTitleRunningEntry*	_GetRunningTitleByID					( int nTitleID );
	BOOL					_DeleteRunningTitle						( int nTitleID );

	BOOL					_UpdateTitleInfo						( eTitleUiType eType, stTitleEntry* pTitle );
	BOOL					_UpdateTitleEffect						( eTitleUiType eType, stTitleEntry* pTitle );

	BOOL					_UpdateDialogButtonState				( eTitleUiType eType );
	BOOL					_UpdateDialogButtonStateReady			( eTitleUiType eType );
	BOOL					_UpdateDialogButtonStateRunning			( eTitleUiType eType );
	BOOL					_UpdateDialogButtonStateComplete		( eTitleUiType eType );
	BOOL					_UpdateDialogButtonStateEnable			( eTitleUiType eType );
	BOOL					_UpdateDialogButtonStateActivate		( eTitleUiType eType );

	BOOL					_ReSetDialogButtonState					( eTitleUiType eType );

	BOOL					_CheckTitleExchangeTimeLimit			( char* pCharacterName );
	BOOL					_CheckIsMaxRunningTitle					( void );
	BOOL					_CheckIsMaxEnableTitle					( void );
	BOOL					_CheckIsMaxEnableAndRunningTitle		( void );
	BOOL					_CheckIsValidTitleRequirement			( void );
	BOOL					_CheckIsValidRequireLevel				( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireGuildMaster			( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireArchlord			( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireClassType			( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireCon					( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireStr					( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireInt					( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireDex					( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireWis					( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireCharisma			( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireGheld				( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireCash				( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireItem				( void* pCharacter, void* pTitleTemplate );
	BOOL					_CheckIsValidRequireTitle				( void* pCharacter, void* pTitleTemplate );
	BOOL					_ShowErrorMessageBoxOK					( char* pMessageKeyString );

// 데이터 인터페이스
public :
	char*					GetCategoryNameByIndex					( int nIndex );

	char*					GetTitleNameByID						( int nTitleID );
	char*					GetTitleNameByIndex						( int nIndex );
	char*					GetTitleNameByIndexWithPage				( int nIndex );

	eTitleStateType			GetTitleStateByIndexWidthPage			( int nIndex );
	eTitleStateType			GetCurrTitleStateByIndex				( int nIndex );

	char*					GetCurrTitleNameByIndexWithPage			( int nIndex );

	int						GetTitleCountCurrentCategory			( void );
	int						GetTitleCountByState					( eTitleStateType eState );

	int						GetTitlePageIndex						( void ) { return m_nCurrentTitleListPage; }
	void					SetTitlePageIndex						( int nPageIndex );

	int						GetCurrentActivateTitleID				( void );
	char*					GetCurrentActivateTitleName				( char* pCharacterName );

	DWORD					GetTitleFontColor						( eTitleStateType eType );
	BOOL					IsOpenUI								( eTitleUiType eUiType );

// 패킷콜백용
public :
	static BOOL				CallBack_RecvTitleAdd					( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleUse					( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleList					( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleDelete				( void* pData, void* pClass, void* pCustData );

	static BOOL				CallBack_RecvTitleQuestRequest			( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleQuestCheck			( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleQuestComplete			( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleQuestList				( void* pData, void* pClass, void* pCustData );

	static BOOL				CallBack_RecvTitleUseNear				( void* pData, void* pClass, void* pCustData );
	static BOOL				CallBack_RecvTitleOpenUI				( void* pData, void* pClass, void* pCustData );

// 내 캐릭터가 지정되었을때..
public :
	static BOOL				CallBack_OnRecvSelfCharacter			( void* pData, void* pClass, void* pCustData );

// 내 캐릭터의 이동에 의해 UI 자동닫힘
public :
	static BOOL				CallBack_AutoCloseByCharacterMove		( void* pData, void* pClass, void* pCustData );

// UI 콜백용
public :
	static BOOL				CallBack_OnOpenUIAll					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnOpenUICurrent				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnOpenUIMini					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL				CallBack_DisplayCategoryName			( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl );
	static BOOL				CallBack_DisplayTitleName				( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl );
	static BOOL				CallBack_DisplayTitleNameCurrent		( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl );
	static BOOL				CallBack_DisplayTitleNameMini			( void* pClass, void* pData, AgcdUIDataType eType, int nID, char* pDisplay, int* pValue, AgcdUIControl *pControl );

	static BOOL				CallBack_OnClickCategoryItem			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickTitleItem				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickTitleItemCurrent		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickTitleItemMini			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL				CallBack_OnClickPagePrev				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickPageNext				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickPage					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL				CallBack_OnClickSetTargetTitle			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickActivateTitle			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickSurrenderTitleCurrent	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickSurrenderTitleMini		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL				CallBack_OnClickCompleteTitle			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

// 유틸리티
public :
	static BOOL				LoadXMLFileToDocument					( void* pXmlDoc, char* pFileName, BOOL bIsEncrypt );
};


AgcmTitle* GetAgcmTitle( void );



#endif