#ifndef __AGCU_UI_CONTROL_CHARACTER_H__
#define __AGCU_UI_CONTROL_CHARACTER_H__



#include "ContainerUtil.h"
#include "rwcore.h"
#include "AgcmUILoginSettingCamera.h"



class AgcuUIControlCharacter
{
	enum AuCharacterState
	{
		NoState											= -1,
		Ready											= 0,
		Selected,
		MoveTo,
		RunAnimation,
		MoveBack,
		UnSelected,		
	};

	enum AuCharacterRotateState
	{
		NoRotate										= 0,
		RotateLeft,
		RotateRight,
	};

	struct stCharacterController
	{
		INT32											m_nCharacterID;
		AuCharacterState								m_eCharacterState;

		void*											m_ppdCharacter;
		void*											m_pcdCharacter;

		RwV3d											m_vSelectPos;
		RwV3d											m_vCreatePos;

		stCharacterController( void );
		stCharacterController( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vSelectedPos, RwV3d vPos );
		stCharacterController( const stCharacterController& rSource );

		bool			IsMyCharacterID					( INT32 nCharacterID );
		bool			IsMyCharacterName				( CHAR* pCharacterName );

		void			ChangeState						( AuCharacterState eNewState );
		RwFrame*		GetFrame						( void );

		INT32			MoveForward						( void* ppmCharacter, float fElapsed );
		INT32			MoveBackward					( void* ppmCharacter, float fElapsed );
	};

public :
	void*												m_pcmCharacter;
	void*												m_ppmCharacter;
	void*												m_pcmEventEffect;

private :
	INT32												m_nLoginMode;
	AuCharacterRotateState								m_eRotateState;

	stCharacterController								m_CreateCharacter;

	ContainerMap< INT32, stCharacterController >		m_mapSelectCharacters;
	INT32												m_nCurrSelectCharacterID;

	static AgcuUIControlCharacter*						m_pSingletonPtr;

public :
	AgcuUIControlCharacter( void );
	virtual ~AgcuUIControlCharacter( void )				{	};

public :
	INT32					OnUpdate					( RwReal fElapsed );
	INT32					OnChangeMode				( INT32 nLoginMode );
	INT32					OnClearCharacter			( void );
	INT32					OnAddCharacter				( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vSelectedPos, RwV3d vCreatePos );
	INT32					OnDeleteCharacter			( INT32 nCharacterID );
	INT32					OnDeleteCharacter			( CHAR* pCharacterName );
	INT32					OnDoubleClickCharacter		( INT32 nCharacterID );
	INT32					OnRotateStartLeft			( void );
	INT32					OnRotateStartRight			( void );
	INT32					OnRotateEnd					( void );

public :
	INT32					InitializeCreateCharacter	( INT32 nCharacterID, void* ppdCharacter, void* pcdCharacter, RwV3d vCreatePos, float fMoveOffset );
	void					SetExternalClassPointers	( void* pcmCharacter, void* ppmCharacter, void* pcmEventEffect );
	stCharacterController*	GetCurrSelectCharacter		( void );
	stCharacterController*	GetCharacterController		( CHAR* pCharacterName );
	INT32					GetSelectAnimationType		( void* ppdCharacter, void* pcdCharacter, void* pFactorModule, void* pItemModule, BOOL bIsReturnToBack = FALSE );
	void					UnSelectCharacter			( void );

public :
	static void*			CallBackSelectAnimationEnd	( void* pData1, void* pData2, void* pData3 );

private :
	void					_UpdateModeCharacterCreate	( RwReal fElapsed );
	void					_SelectCharacter			( void );
	void					_UnSelectCharacter			( void );
};



#endif