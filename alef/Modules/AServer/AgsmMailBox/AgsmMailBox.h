/*============================================================================

	AgsmMailBox.h

============================================================================*/


#ifndef _AGSM_MAILBOX_H_
	#define _AGSM_MAILBOX_H_

#include "AuGenerateID.h"
#include "ApBase.h"
#include "AgsEngine.h"
#include "AgpmMailBox.h"
#include "ApmMap.h"
#include "AgpmGrid.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmServerManager2.h"
#include "AgsmInterServerLink.h"
#include "AgsmReturnToLogin.h"
#include "AgsdBuddy.h"					// for rowset... but informal


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGSMMAILBOX_CB
	{
	AGSMMAILBOX_CB_WRITE_MAIL = 0,
	AGSMMAILBOX_CB_UPDATE_MAIL,
	AGSMMAILBOX_CB_DELETE_MAIL,
	AGSMMAILBOX_CB_SELECT_MAIL,
	AGSMMAILBOX_CB_ITEM_SAVE,
	};




/************************************************/
/*		The Definition of AgsmMailBox class		*/
/************************************************/
//
class AgsmMailBox : public AgsModule
	{
	private:
		AgpmGrid				*m_pAgpmGrid;
		ApmMap					*m_pApmMap;
		AgpmCharacter			*m_pAgpmCharacter;
		AgpmFactors				*m_pAgpmFactors;
		AgpmItem				*m_pAgpmItem;
		AgpmItemConvert			*m_pAgpmItemConvert;
		AgpmMailBox				*m_pAgpmMailBox;

		AgsmCharacter			*m_pAgsmCharacter;
		AgsmCharManager			*m_pAgsmCharManager;
		AgsmItem				*m_pAgsmItem;
		AgsmItemManager			*m_pAgsmItemManager;
		AgsmServerManager		*m_pAgsmServerManager;
		AgsmInterServerLink		*m_pAgsmInterServerLink;
		AgsmReturnToLogin		*m_pAgsmReturnToLogin;
		
		AgpmLog					*m_pAgpmLog;

		AuGenerateID			m_GenerateID;
		
	public:
		AgsmMailBox();
		~AgsmMailBox();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Login (received mail) related callback
		static BOOL CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Logout related callback
		static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBReturnToLogin(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Request callbacks
		static BOOL	CBWrite(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRemove(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRead(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBItemSave(PVOID pData, PVOID pClass, PVOID pCustData);
	
		//	Request processing
		BOOL	OnWrite(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg);
		BOOL	OnRemove(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg);
		BOOL	OnRead(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg);
		BOOL	OnItemSave(AgpdCharacter *pAgpdCharacter, AgpdMailArg *pMailArg);
	
		//	Result processing
		BOOL	OnSelectResult(AgpdCharacter *pAgpdCharacter, stBuddyRowset *pRowset, BOOL bEnd = FALSE);
		BOOL	OnWriteResult(AgpdCharacter *pAgpdCharacter, INT32 lID, UINT64 ullDBID, INT32 lItemID, INT32 lResult);
		BOOL	OnItemSaveResult(AgpdCharacter *pAgpdCharacter, INT32 lMailID, UINT64 ullItemSeq,
								INT32 lItemTID, INT32 lItemQty, INT16 nStatus, INT32 lNeedLevel,
								CHAR *pszConvert, INT32 lDurability, INT32 lMaxDurability, INT32 lFlag,
								CHAR *pszOption, CHAR *pszSkillPlus, INT32 lInUse, INT32 lUseCount,
								INT64 lRemainTime, CHAR *pszExpireDate, INT64 llStaminaRemainTime,
								INT32 lResult);

		//	Arrange
		BOOL	ArrangeMailBox(AgpdCharacter *pAgpdCharacter);
		BOOL	ArrangeMailBox(AgpdMailCAD *pAgpdMailCAD);

		//	Packet send
		BOOL	SendAddMail(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, UINT32 ulNID);
		BOOL	SendRemoveMail(AgpdCharacter *pAgpdCharacter, INT32 lID, INT32 lResult);
		BOOL	SendRemoveAllMail(AgpdCharacter *pAgpdCharacter);
		BOOL	SendWriteResult(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail, INT32 lResult);
		BOOL	SendItemSaveResult(AgpdCharacter *pAgpdCharacter, INT32 lID, INT32 lResult);
		BOOL	SendAllMail(AgpdCharacter *pAgpdCharacter, UINT32 ulNID);
		
		//	Callback setting
		BOOL	SetCallbackWriteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUpdateMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDeleteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSelectMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItemSave(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	protected:
		//	Helper methods
		BOOL	_IsFullInventory(AgpdCharacter *pcsAgpdCharacter);
		UINT32	_GetCharacterNID(INT32 lCID);
		UINT32	_GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		BOOL	_RemoveAndDestroyMail(AgpdMail *pAgpdMail, BOOL bDB = TRUE);

		//	Log
	};


#endif

