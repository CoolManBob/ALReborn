/*============================================================================

	AgsmLogin.h
	
		Game server side Login module

============================================================================*/

#ifndef _AGSM_LOGIN_H_
	#define _AGSM_LOGIN_H_


#include "ApBase.h"
#include "AgsEngine.h"
#include "AgpmLogin.h"
#include "AgpmCharacter.h"
#include "AgsmServerManager2.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmAccountManager.h"
#include "AgpmLogin.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMLOGIN_CB
	{
	AGSMLOGIN_CB_CHARACTER_RENAMED = 0,	// when character rename on login server
	AGSMLOGIN_CB_MAX
	};




/************************************************/
/*		The Definition of AgsmLogin class		*/
/************************************************/
//
class AgsmLogin : public AgsModule
	{
	private :
		//	Related modules
		AgpmLogin			*m_pAgpmLogin;
		AgpmCharacter		*m_pAgpmCharacter;
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmServerManager	*m_pAgsmServerManager;
		AgsmCharManager		*m_pAgsmCharManager;
		AgsmAccountManager	*m_pAgsmAccountManager;

	public:
		AgsmLogin();
		virtual ~AgsmLogin();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		//JK_중복로그인
		BOOL	SendRemoveDuplicateAccountForLoginServer(TCHAR *pszAccountID, INT32 lCID, UINT32 ulServerNID);


		//	Callback
		static BOOL	CBDisconnectFromGameServer(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Callback setting
		BOOL	SetCallbackCharacterRenamed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif