#ifndef __AGCM_UI_CHARACTER_SELECT_H__
#define __AGCM_UI_CHARACTER_SELECT_H__




#include "ContainerUtil.h"
#include "rwcore.h"
#include "ApDefine.h"
#include "ApBase.h"
#include "AgcdUIManager2.h"
#include "AgcmLogin.h"



class AgcmUICharacterSelect
{
	struct stCharacterSlot
	{
		INT32										m_nSlotIndex;
		CHAR										m_strCharacterName[ 64 ];
		void*										m_ppdCharacter;
		AuPOS										m_vCharacterPos;
		float										m_fCharacterRotate;

		stCharacterSlot( void )	: m_nSlotIndex( -1 )
		{
			ClearSlot();
		}

		stCharacterSlot( INT32 nIndex ) : m_nSlotIndex( nIndex )
		{
			ClearSlot();
		}

		BOOL			IsHaveCharacter				( void )
		{
			return m_ppdCharacter ? TRUE : FALSE;
		}

		BOOL			SetCharacter				( CHAR* pName, void* ppdCharacter, RwV3d vPos, float fRotate );

		void			ClearSlot					( void )
		{
			m_ppdCharacter = NULL;

			m_vCharacterPos.x = 0.0f;
			m_vCharacterPos.y = 0.0f;
			m_vCharacterPos.z = 0.0f;

			memset( m_strCharacterName, 0, sizeof( CHAR ) * 64 );
			m_fCharacterRotate = 0.0f;
		}
	};

private :
	ContainerMap< INT32, stCharacterSlot >			m_mapCharacterSlot;

	INT32											m_nCurrSlot;
	INT32											m_nPrevSlot;

	void*											m_pcmUILogin;
	INT32											m_nCurrentCharacterID;

	AgcdLoginCompenMaster							m_cdLoginCompenMaster;

public :
	AgcmUICharacterSelect( void );
	virtual ~AgcmUICharacterSelect( void );

public :
	BOOL				OnInitialize				( void* pUILogin );

	BOOL				OnAddFunction				( void* pUIManager );
	BOOL				OnAddUserData				( void* pUIManager );
	BOOL				OnAddDisplay				( void* pUIManager );
	BOOL				OnAddCallBack				( void* pLogin );

	BOOL				OnSlotInitialize			( void );
	BOOL				OnSlotUpdate				( INT32 nSlotIndex );

	BOOL				OnCharacterAddInfo			( INT32 nSlotIndex, TCHAR* pCharaceterName );
	BOOL				OnCharacterAdd				( void* pUILogin, void* pCharacter );
	BOOL				OnCharacterSelect			( INT32 nCharacterID );
	BOOL				OnCharacterUnSelect			( INT32 nCharacterID );
	BOOL				OnCharacterUse				( INT32 nCharacterID );
	BOOL				OnCharacterRemove			( INT32 nSlotIndex );
	BOOL				OnCharacterRemove			( TCHAR* pCharacterName );

	BOOL				OnBtnCharacterCreate		( void* pUILogin );
	BOOL				OnBtnCharacterRemove		( void* pUILogin );
	BOOL				OnBtnReturnToSelectServer	( void* pUILogin );
	BOOL				OnBtnSlot					( INT32 nControlNumber );
	BOOL				OnBtnCompensation			( INT32 nControlNumber );
	BOOL				OnBtnExitGame				( void* pUILogin );

public :
	void*				GetModule					( CHAR* pModuleName );
	INT32				GetControlNumber			( AgcdUIControl* pControl );
	void*				GetSlotCharacter			( INT32 nSlotIndex );
	void*				GetSelectCharacter			( void );

	INT32				GetSelectCharacterID		( void ) { return m_nCurrentCharacterID; }
	void				SetSelectCharacterID		( INT32 nID ) { m_nCurrentCharacterID = nID; }

private :
	BOOL				_MakeCharacter				( void* pUILogin, stCharacterSlot* pSlot );

	INT32				_GetCharacterCount			( void );
	INT32				_GetSlotIndexByName			( CHAR* pName );
	stCharacterSlot*	_GetCharacterSlot			( INT32 nSlotIndex );
	stCharacterSlot*	_GetCharacterSlot			( CHAR* pName );
	stCharacterSlot*	_GetCurrCharacterSlot		( void );
	stCharacterSlot*	_GetPrevCharacterSlot		( void );

public :
	static BOOL			CB_OnBtnCharacterSelect		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnCharacterDelete		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL			CB_OnBtnSlotSelect			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnSlotInfoSelect		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnSlotSetFocus		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL			CB_OnBtnSlotKillFocus		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
};



#endif