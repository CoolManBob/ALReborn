#ifndef __AGCM_UI_ACCOUNT_CHECK_H__
#define __AGCM_UI_ACCOUNT_CHECK_H__



#include "ApBase.h"
#include "AgcdUIManager2.h"



class AgcmUIAccountCheck
{
protected :
	void*										m_pcmUILogin;
	CHAR*										m_pChallengeString;

	void*										m_pcdUIUserDataKeyChallenge;

	INT32										m_nEventKeyOpen;
	INT32										m_nEventKeyClose;
	INT32										m_nEventNotProtected;

	INT32										m_nRetryCount;

public :
	AgcmUIAccountCheck( void );
	virtual ~AgcmUIAccountCheck( void );

public :
	BOOL			OnInitialize				( void* pUILogin );

	BOOL			OnAddEvent					( void* pUIManager );
	BOOL			OnAddFunction				( void* pUIManager );
	BOOL			OnAddUserData				( void* pUIManager );
	BOOL			OnAddDisplay				( void* pUIManager );
	BOOL			OnAddCallBack				( void* pLogin );

	BOOL			OnAddLoginRetryCount		( void* pUIManager );

public :
	// Event CallBack
	static BOOL		CB_OnClear					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnSubmit					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnCancel					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnKeyTab					( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnSendLogin				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnExitGame				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnOpenKey				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL		CB_OnCloseKey				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	// External CallBack
	static BOOL		CB_OnConnect				( void* pData, void* pClass, void* pCustomData );
	static BOOL		CB_OnDisConnect				( void* pData, void* pClass, void* pCustomData );
	static BOOL		CB_OnActiveKey				( void* pData, void* pClass, void* pCustomData );
	static BOOL		CB_OnLoginOK				( void* pData, void* pClass, void* pCustomData );	

	// Display
	static BOOL		CB_OnDisplayChallenge		( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue );
	
	virtual BOOL	ReConnectLoginServer		( VOID )	PURE;		// Login Server 접속 실패시 호출된다.

public :
	void*			GetUserDataKeyChallenge		( void ) { return m_pcdUIUserDataKeyChallenge; }
	CHAR*			GetChallengeString			( void ) { return m_pChallengeString; }

	INT32			GetEKeyOpen					( void ) { return m_nEventKeyOpen; }
	INT32			GetEKeyClose				( void ) { return m_nEventKeyClose; }
	INT32			GetENotProtected			( void ) { return m_nEventNotProtected; }

	INT32			GetLoginRetryCount			( void ) { return m_nRetryCount; }	
};



#endif