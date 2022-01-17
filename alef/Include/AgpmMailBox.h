/*====================================================================

	AgpmMailBox.h
	
====================================================================*/


#ifndef _AGPM_MAILBOX_H_
	#define _AGPM_MAILBOX_H_


#include "ApBase.h"
#include "ApModule.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AuGenerateID.h"
#include "AgpdMailBox.h"
#include "AgpaMailBox.h"
#include "AuTimeStamp.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGPMMAILBOX_OPERATION
	{
	AGPMMAILBOX_OPERATION_NONE = -1,
	AGPMMAILBOX_OPERATION_ADD_MAIL = 0,
	AGPMMAILBOX_OPERATION_REMOVE_MAIL,
	AGPMMAILBOX_OPERATION_READ_MAIL,
	AGPMMAILBOX_OPERATION_WRITE_MAIL,
	AGPMMAILBOX_OPERATION_ITEM_SAVE,
	AGPMMAILBOX_OPERATION_REMOVE_ALL_MAIL,
	AGPMMAILBOX_OPERATION_MAX
	};


enum eAGPMMAILBOX_CB
	{
	AGPMMAILBOX_CB_NONE = -1,
	AGPMMAILBOX_CB_ADD_MAIL = 0,
	AGPMMAILBOX_CB_REMOVE_MAIL,
	AGPMMAILBOX_CB_READ_MAIL,
	AGPMMAILBOX_CB_WRITE_MAIL,
	AGPMMAILBOX_CB_ITEM_SAVE,
	AGPMMAILBOX_CB_MAX
	};


enum eAGPMMAILBOX_RESULT
	{
	AGPMMAILBOX_RESULT_NONE = -1,
	AGPMMAILBOX_RESULT_SUCCESS = 0,					// 0 ¼º°ø
	AGPMMAILBOX_RESULT_FAIL,
	AGPMMAILBOX_RESULT_CHAR_NOT_FOUND,
	AGPMMAILBOX_RESULT_MAIL_NOT_FOUND,
	AGPMMAILBOX_RESULT_NO_LETTER,
	AGPMMAILBOX_RESULT_INVALID_ITEM,
	AGPMMAILBOX_RESULT_INVEN_FULL,
	AGPMMAILBOX_RESULT_REVERTED_ITEM,
	AGPMMAILBOX_RESULT_NOT_IN_INVEN,
	AGPMMAILBOX_RESULT_USED_CASH_ITEM,
	AGPMMAILBOX_RESULT_CANT_ATT_CASH_ITEM,
	AGPMMAILBOX_RESULT_MAX,
	};


enum eAGPMMAILBOX_DATATYPE
	{
	AGPMMAILBOX_DATATYPE_MAIL = 0,
	};




/************************************************/
/*		The Definition of AgpmMailBox class		*/
/************************************************/
//
class AgpmMailBox : public ApModule
	{
	private:
		//	Related modules
		ApmMap			*m_pApmMap;
		AgpmCharacter	*m_pAgpmCharacter;
		AgpmFactors		*m_pAgpmFactors;
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;

		//	Admin
		AgpaMailBox		m_csAdmin;

	public:
		//	Packet
		AuPacket		m_csPacket;
		AuPacket		m_csPacketMail;

		//	CAD
		INT16			m_nIndexCharacterAD;

	public:
		AgpmMailBox();
		~AgpmMailBox();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		BOOL	OnInit();
		BOOL	OnDestroy();
		
		//	Admin
		BOOL			SetMaxMail(INT32 lCount);
		AgpdMail*		CreateMail();
		void			DestroyMail(AgpdMail* pAgpdMail);
		AgpdMail*		GetMail(INT32 lID);
		BOOL			AddMail(AgpdMail *pAgpdMail);
		BOOL			RemoveMail(AgpdMail *pAgpdMail);

		//	CAD
		static BOOL		ConAgpdMailCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		DesAgpdMailCAD(PVOID pData, PVOID pClass, PVOID pCustData);
		AgpdMailCAD*	GetCAD(ApBase *pApBase);

		//	CAD
		BOOL			AddMailToCAD(AgpdCharacter *pAgpdCharacter,  AgpdMail *pAgpdMail, BOOL bHead, INT32 *plRemovedID = NULL);
		BOOL			RemoveMailFromCAD(AgpdCharacter *pAgpdCharacter, AgpdMail *pAgpdMail);
		BOOL			RemoveMailFromCAD(AgpdCharacter *pAgpdCharacter, INT32 lMailID);
		BOOL			RemoveAllMail(AgpdCharacter *pAgpdCharacter, AuGenerateID *pGenerateID = NULL);
		AgpdMail*		FindMailFromCAD(AgpdCharacter *pAgpdCharacter, UINT64 ullDBID);

		//	Validation, ...
		BOOL			GetLetterCount(AgpdCharacter *pAgpdCharacter, INT32 &lNormal, INT32 &lPremium);
		BOOL			SubLetter(AgpdCharacter *pAgpdCharacter, BOOL bNormal = TRUE);
		BOOL			IsValidAttachItem(AgpdItem *pAgpdItem, INT32 *plResult = NULL);
		BOOL			IsWesterPreOrderItem(INT32 lTID);

		//	Mail packet
		BOOL			ParsePacketMail(PVOID pvPacketMail, AgpdMail *pAgpdMail);
		PVOID			MakePacketMail(AgpdMail *pAgpdMail, INT16 *pnPakcetLength);
		PVOID			MakePacketMailOnlyID(INT32 lMailID, INT16 *pnPacketLength);

		//	Callback setting
		BOOL	SetCallbackAddMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackRemoveMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackReadMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackWriteMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItemSave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	};


#endif