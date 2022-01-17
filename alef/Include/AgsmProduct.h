/*====================================================================

	AgsmProduct.h

====================================================================*/


#ifndef _AGSM_PRODUCT_H_
	#define _AGSM_PRODUCT_H_

#include "AgsEngine.h"
#include "AgpmProduct.h"
#include "AgpmFactors.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmCharacter.h"
#include "AuRandomNumber.h"
#include "AgsdProduct.h"
#include "AgsaProduct.h"
#include "AgpmTitle.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
	#pragma comment ( lib , "AgsmProductD" )
#else
	#pragma comment ( lib , "AgsmProduct" )
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMPRODUCT_RESULT_INTERVAL			500

const enum eAGSMPRODUCT_DATATYPE
	{
	AGSMPRODUCT_DATATYPE_ITEM_SET = 0,
	};


enum eAGSMPRODUCT_CB
	{
	AGSMPRODUCT_CB_UPDATE_DB = 0,
	};


/********************************************/
/*		The Definition of AgsmProduct		*/
/********************************************/
//
class AgsmProduct : public AgsModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmFactors			*m_pAgpmFactors;
		AgpmSkill			*m_pAgpmSkill;
		AgpmProduct			*m_pAgpmProduct;
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmItem			*m_pAgsmItem;
		AgsmItemManager		*m_pAgsmItemManager;
		AgsmSkill			*m_pAgsmSkill;
		AgpmTitle			*m_pAgpmTitle;
		MTRand				m_csRandom;

		//	Admin
		AgsaProductItemSet	m_csAdminItemSet;

		//	AD
		INT32				m_nIndexProductResultCAD;

	public:
		AgsmProduct();
		virtual ~AgsmProduct();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnIdle(UINT32 ulClockCount);
		BOOL	OnDestroy();

		//	Admin
		BOOL				SetMaxItemSet(INT32 lCount);
		AgsdProductItemSet*	GetItemSet(INT32 lID);

		//	Stream
		BOOL	StreamReadProductItemSet(CHAR* pszFile, BOOL bDecryption);

		//	Callbacks
		static BOOL	CBCompose(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGatherPrepare(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGather(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBDecodeCompose(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Operations (after callback)
		BOOL	OnCompose(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, AgpdComposeTemplate *pAgpdComposeTemplate, AgpdItem *pAgpdItem);
		BOOL	OnGatherPrepare(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBase *pTarget);
		BOOL	OnGather(AgpdCharacter *pAgpdCharacter, AgpdSkill *pAgpdSkill, ApBase *pTarget);

		//	Send Packet
		BOOL	SendPakcetProductResult(INT32 lCID, INT32 lSkillID, INT32 eResult,
										INT32 lItemTID, INT32 lExp, INT32 lItemCount = 1);
		BOOL	SendPacketComposeAll(AgpdCharacter *pAgpdCharacter, UINT32 ulNID);

		//	Compose Encode/Decode
		BOOL	EncodeCompose(AgpdCharacter *pAgpdCharacter, CHAR *pszCompose, INT32 lBufferSize);
		BOOL	DecodeCompose(AgpdCharacter *pAgpdCharacter, CHAR *pszCompose);

		//	DB Operation
		BOOL	UpdateDB(AgpdCharacter *pAgpdCharacter);
		BOOL	SetCallbackUpdateDB(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		
	protected:
		//	Helper
		INT32	EvaluateItem(INT32 lItemSetID, INT32 lSkillLevel);
		BOOL	DeleteComposeSourceItem(AgpdCharacter* pAgpdCharacter, AgpdComposeTemplate* pAgpdComposeTemplate,
										AgpdItem* pReceipe);
	};

#endif
