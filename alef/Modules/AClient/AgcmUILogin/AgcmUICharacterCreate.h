#ifndef __AGCM_UI_CHARACTER_CREATE_H__
#define __AGCM_UI_CHARACTER_CREATE_H__




#include "ContainerUtil.h"
#include "ApDefine.h"
#include "ApBase.h"
#include "AgcdUIManager2.h"



#define	AGCMUILOGIN_MY_CHARACTER_NUM			3
#define	AGCMUILOGIN_MAX_RACE_NAME				3
#define	AGCMUILOGIN_MAX_CLASS_NAME				3
#define	AGCMUILOGIN_MAX_HEAD_TYPE_NAME			20
#define	AGCMUILOGIN_MAX_FACE_TYPE_NAME			20
#define AGCMUILOGIN_MAX_NAME_LENGTH				32
#define AGCMUILOGIN_MAX_DESC_LENGTH				256



struct stSelectableParts
{
	INT32												m_nIndex;
	CHAR												m_strPartsName[ AGCMUILOGIN_MAX_NAME_LENGTH ];

	stSelectableParts( void )
		: m_nIndex( 0 )
	{
		memset( m_strPartsName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
	}
};

struct stSelectableClass
{
	AuRaceType											m_eRaceType;
	AuCharClassType										m_eClassType;

	INT32												m_nCharacterID;
	CHAR												m_strClassName[ AGCMUILOGIN_MAX_NAME_LENGTH ];

	ContainerMap< INT32, stSelectableParts >			m_mapSelectableHeadType;
	ContainerMap< INT32, stSelectableParts >			m_mapSelectableFaceType;

	INT32												m_nCurrentSelectHeadType;
	INT32												m_nCurrentSelectFaceType;

	stSelectableClass( void )
		: m_eRaceType( AURACE_TYPE_NONE ), m_eClassType( AUCHARCLASS_TYPE_NONE ), m_nCharacterID( 0 ),
			m_nCurrentSelectHeadType( -1 ), m_nCurrentSelectFaceType( -1 )
	{
		memset( m_strClassName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
		m_mapSelectableHeadType.Clear();
		m_mapSelectableFaceType.Clear();
	}

	void				SetClassType					( AuRaceType eRaceType, AuCharClassType eClassType );

	void				SelectHeadType					( INT32 nHeadType );
	void				SelectFaceType					( INT32 nFaceType );

	INT32				GetSelectHeadIndex				( void );
	INT32				GetSelectFaceIndex				( void );

	CHAR*				GetCurrentHeadName				( void );
	CHAR*				GetCurrentFaceName				( void );

	void				LoadHeadTypes					( void* pCharacter );
	void				LoadFaceTypes					( void* pCharacter );
};

struct stSelectableRace
{
	AuRaceType											m_eRaceType;
	AuCharClassType										m_eCurrentSelectClassType;
	
	CHAR												m_strRaceName[ AGCMUILOGIN_MAX_NAME_LENGTH ];
	ContainerMap< AuCharClassType, stSelectableClass >	m_mapSelectableClassType;

	stSelectableRace( void )
		: m_eRaceType( AURACE_TYPE_NONE ), m_eCurrentSelectClassType( AUCHARCLASS_TYPE_NONE )
	{
		memset( m_strRaceName, 0, sizeof( CHAR ) * AGCMUILOGIN_MAX_NAME_LENGTH );
		m_mapSelectableClassType.Clear();
	}

	void				SetRaceType						( AuRaceType eRaceType );
	BOOL				SelectClassType					( AuCharClassType eClassType );	 

	stSelectableClass*	GetSelectClass					( void );
	stSelectableClass*	GetClass						( AuCharClassType eClassType );

	void				_LoadClassTypes					( void );
	void				_LoadClassType					( AuCharClassType eClassType );
};

class AgcmUICharacterCreate
{
	enum UpdateUILevel
	{
		NoUpdate										= 0,
		Race,
		Class,
		Head,
		Face,
		Rotate,
		Zoom,
	};

protected :
	ContainerMap< AuRaceType, stSelectableRace >		m_mapSelectableRaceTypes;
	AuRaceType											m_eCurrentSelectRaceType;

	BOOL												m_bIsZoomIn;
	INT32												m_nCreateCharacterID;

public :
	static void*										g_pcmUILogin;
	static void*										g_pcmUIManager;
	static void*										g_pcmUICharacter;
	static void*										g_ppmCharacter;
	static void*										g_pcmCharacter;
	static void*										g_pcmLogin;
	static void*										g_ppmFactors;
	static void*										g_ppmEventCharCustomize;	

	void*												m_pcdUIUserDataHeadName;
	void*												m_pcdUIUserDataFaceName;
	void*												m_pcdUIUserDataRaceDescTitle;
	void*												m_pcdUIUserDataClassDescTitle;

	CHAR												m_strUserDataHeadName[ AGCMUILOGIN_MAX_NAME_LENGTH ];
	CHAR												m_strUserDataFaceName[ AGCMUILOGIN_MAX_NAME_LENGTH ];
	
	CHAR												m_strUserDataRaceDescTitle[ AGCMUILOGIN_MAX_NAME_LENGTH ];
	CHAR												m_strUserDataClassDescTitle[ AGCMUILOGIN_MAX_NAME_LENGTH ];

	AgcdUIControl*										m_pBtnSelectClassKnight;
	AgcdUIControl*										m_pBtnSelectClassArcher;
	AgcdUIControl*										m_pBtnSelectClassMage;
	AgcdUIControl*										m_pBtnSelectClassBerserker;
	AgcdUIControl*										m_pBtnSelectClassHunter;
	AgcdUIControl*										m_pBtnSelectClassSocerer;
	AgcdUIControl*										m_pBtnSelectClassSwashBuckler;
	AgcdUIControl*										m_pBtnSelectClassRanger;
	AgcdUIControl*										m_pBtnSelectClassElementalist;
	AgcdUIControl*										m_pBtnSelectClassScion;

	AcUIEdit*											m_pEditRaceDesc;
	AcUIEdit*											m_pEditClassDesc;

	INT32												m_nEventGetBtnClassKnight;
	INT32												m_nEventGetBtnClassArcher;
	INT32												m_nEventGetBtnClassMage;
	INT32												m_nEventGetBtnClassBerserker;
	INT32												m_nEventGetBtnClassHunter;
	INT32												m_nEventGetBtnClassSocerer;
	INT32												m_nEventGetBtnClassSwashBuckler;
	INT32												m_nEventGetBtnClassRanger;
	INT32												m_nEventGetBtnClassElementalist;
	INT32												m_nEventGetBtnClassScion;

	INT32												m_nEventGetEditRaceDesc;
	INT32												m_nEventGetEditClassDesc;

protected :
	void*												m_ppdCurrentCharacter;

public :
	AgcmUICharacterCreate( void );
	virtual ~AgcmUICharacterCreate( void );

public :
	BOOL				OnInitialize					( void* pcmUILogin );
	BOOL				OnGetBtnControls				( void* pUIManager );

	BOOL				OnAddEvent						( void* pUIManager );
	BOOL				OnAddUserData					( void* pUIManager );
	BOOL				OnAddFunction					( void* pUIManager );
	BOOL				OnAddDisplay					( void* pUIManager );
	BOOL				OnAddCallBack					( void* pLogin );

	BOOL				OnClear							( void );
	BOOL				OnCheckNickName					( TCHAR* pNickName );

	BOOL				OnSelectRace					( INT32 nSelectRaceType );
	BOOL				OnSelectClass					( INT32 nSelectClassType );

	BOOL				OnSelectHead					( INT32 nSelectHeadType );
	BOOL				OnSelectPrevHead				( void );
	BOOL				OnSelectNextHead				( void );

	BOOL				OnSelectFace					( INT32 nSelectFaceType );
	BOOL				OnSelectPrevFace				( void );
	BOOL				OnSelectNextFace				( void );

	BOOL				OnToggleZoom					( void );

	//BOOL				OnBtnRaceDragonSion				( void );

protected :
	BOOL				_LoadSelectableRace				( AuRaceType eRaceType );
	BOOL				_ChangeBtnSetSelectClass		( AuRaceType eRaceType );

	stSelectableRace*	_GetSelectRace					( void );
	stSelectableClass*	_GetSelectClass					( void );

	AuRaceType			_GetSelectRaceType				( void );
	AuCharClassType		_GetSelectClassType				( void );

	stSelectableRace*	_GetCreatableRace				( AuRaceType eRaceType );
	stSelectableClass*	_GetCreatableClass				( AuRaceType eRaceType, AuCharClassType eClassType );

	BOOL				_UpdateCharacterCreateUI		( UpdateUILevel eUpdateLevel );
	BOOL				_UpdateHead						( void );
	BOOL				_UpdateFace						( void );
	BOOL				_UpdateDescription				( void );

	BOOL				_IsCurrentRaceType				( AuRaceType eRaceType );
	BOOL				_IsCurrentClassType				( AuCharClassType eClassType );

public :
	INT32				GetCreateCharacterID			( void ) { return m_nCreateCharacterID; }

	INT32				GetSelectHair					( void );
	INT32				GetSelectFace					( void );

	CHAR*				GetCurrentHeadName				( void ) { return m_strUserDataHeadName; }
	CHAR*				GetCurrentFaceName				( void ) { return m_strUserDataFaceName; }
	CHAR*				GetRaceDescTitle				( void ) { return m_strUserDataRaceDescTitle; }
	CHAR*				GetClassDescTitle				( void ) { return m_strUserDataClassDescTitle; }

	BOOL				IsZoomIn						( void ) { return m_bIsZoomIn; }
	CHAR*				GetBtnToggleZoomText			( void );
	BOOL				IsCurrentUI						( void* pControl );

public :
	static BOOL			CB_OnBtnCheckNickName			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnSelectRaceHuman			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectRaceOrc				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectRaceMoonElf			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectRaceDragonScion		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnSelectClassKnight			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectClassArcher			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectClassMage			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnSelectClassMonk			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnGetBtnClassKnight			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassArcher			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassMage			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassBerserker		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassHunter			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassSocerer			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassSwashBuckler	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassRanger			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassElementalist	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetBtnClassScion			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnGetEditRaceDesc			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnGetEditClassDesc			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnBtnSelectHeadPrev			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnSelectHeadNext			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnBtnSelectFacePrev			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnSelectFaceNext			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnBtnRotateLeft				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnRotateRight				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnRotateEnd				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnToggleZoom				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnCharacterCreateOK			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnCharacterCreateNo			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	
	static BOOL			CB_OnDisplayHeadName			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayFaceName			( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayRaceDescTitle		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayClassDescTitle		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	static BOOL			CB_OnDisplayBtnToggleZoom		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );

	static BOOL			CB_OnGetBaseCharacterOfRace		( void* pData, void* pClass, void* pCustomData );
};




#endif