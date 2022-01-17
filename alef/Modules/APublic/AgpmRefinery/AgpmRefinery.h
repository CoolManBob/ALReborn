/*====================================================================

	AgpmRefinery.h

====================================================================*/

#ifndef _AGPM_REFINERY_H_
	#define _AGPM_REFINERY_H_


#include "ApModule.h"
#include "AuPacket.h"
#include "AgpdRefinery.h"
#include "AgpaRefinery.h"

#include "AgpmFactors.h"
#include "AgpmItem.h"
#include "AgpmGrid.h"
#include "AgpmCharacter.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGPMREFINERY_DATATYPE
	{
	AGPMREFINERY_DATATYPE_REFINE_TEMPLATE	= 0,
	AGPMREFINERY_DATATYPE_REFINE_ITEM,
	};


const enum eAGPMREFINERY_PACKET_OPERATION
	{
	AGPMREFINERY_PACKET_OPERATION_REFINE	= 0,
	AGPMREFINERY_PACKET_OPERATION_RESULT,
	AGPMREFINERY_PACKET_OPERATION_REFINE_ITEM,
	AGPMREFINERY_PACKET_OPERATION_REFINE_ITEM_RESULT,
	};


const enum eAGPMREFINERY_CALLBACK
	{
	AGPMREFINERY_CB_REFINE	= 0,
	AGPMREFINERY_CB_RESULT,
	AGPMREFINERY_CB_REFINE_ITEM,
	AGPMREFINERY_CB_REFINE_ITEM_RESULT,
	AGPMREFINERY_CB_BADBOY,
	};


const enum eAGPMREFINERY_RESULT_CODE
	{
	AGPMREFINERY_RESULT_NONE = -1,
	AGPMREFINERY_RESULT_SUCCESS = 0,
	AGPMREFINERY_RESULT_FAIL,
	AGPMREFINERY_RESULT_FAIL_DIFFERENT_ITEM,
	AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_ITEM,
	AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_MONEY,
	AGPMREFINERY_RESULT_FAIL_FULL_INVENTORY,
	AGPMREFINERY_RESULT_FAIL_SAME_STONE_ATTRIBUTE,
	AGPMREFINERY_RESULT_FAIL_INSUFFICIENT_OPTION_ITEM,
	};


const enum eAGPMREFINERY_ITEM_RESULT
	{
	AGPMREFINERY_ITEM_RESULT_VALID = 0,
	AGPMREFINERY_ITEM_RESULT_INVALID,
	AGPMREFINERY_ITEM_RESULT_INSUFFICIENT_QUANTITY,
	AGPMREFINERY_ITEM_RESULT_NOT_FOUND,
	};


const enum eAGPMREFINERY_OPTION_RESULT
	{
	AGPMREFINERY_OPTION_RESULT_VALID = 0,
	AGPMREFINERY_OPTION_RESULT_INVALID,
	AGPMREFINERY_OPTION_RESULT_INSUFFICIENT_QUANTITY,
	AGPMREFINERY_OPTION_RESULT_DUPLICATE_ATTRIBUTE,
	};



/****************************************************/
/*		The Definition of AgpmRefinery class		*/
/****************************************************/
//
class AgpmRefinery : public ApModule
{
	public:
		AgpmItem		*m_pAgpmItem;
		AgpmGrid		*m_pAgpmGrid;
		AgpmFactors		*m_pAgpmFactors;
		AgpmCharacter	*m_pAgpmCharacter;
        AgpdRefineItem2 *m_pAgpdResultItem;

	private:
		//	Admin
		//AgpaRefineTemplate		m_csAdminRefineTemplate;
        AgpaRefineItem			m_csAdminRefineItem;

	public:
		AuPacket		m_csPacket;

	public:
		AgpmRefinery();
		virtual ~AgpmRefinery();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnDestroy();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
        //  Make refine resource key 
        void                    MakeResKey(std::list<AgpdRefineItemRes>& list, string& strKey);

		//	Admin
		//BOOL					SetMaxRefineTemplate(INT32 lCount);
		AgpdRefineTemplate*		GetRefineTemplate(INT32 lID);

		BOOL					SetMaxRefineItem(INT32 lCount);
        AgpdRefineItem2*        GetRefineItem(const char* strKey);

		//	Stream
		//BOOL	StreamReadRefineTemplate(CHAR *pszFile, BOOL bDecryption);
        BOOL	StreamReadRefineItem2(CHAR *pszFile, BOOL bDecryption);

		//	Refine validation
		//BOOL	IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, AgpdItem *pSourceItems[], INT32 *plResult = NULL);
		//BOOL	IsValidStatus(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 lSourceItems[], INT32 *plResult = NULL);
		//BOOL	IsValidItemStatus(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lCount, AgpdItem *pSourceItems[]);
		//BOOL	IsValidItemStatus(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 lCount);
		//BOOL	IsValidMoney(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate);

		//	Item Refine Validation
        BOOL	IsValidStatusRefine(AgpdCharacter *pAgpdCharacter, INT32 lSourceItems[], INT32 *plResult = NULL, BOOL *pbSources = NULL);
        BOOL	IsValidStatusRefine(AgpdCharacter *pAgpdCharacter, AgpdItem *pSourceItems[], INT32 *plResult = NULL, BOOL *pbSources = NULL);
        //BOOL	IsValidMoney(AgpdCharacter *pAgpdCharacter, AgpdRefineItem2 *pAgpdRefineItem);
        AgpdRefineItem2* GetRefineResultItems(AgpdCharacter *pAgpdCharacter, AgpdItem *pSourceItems[]);
		
		//	Operation
		//BOOL	OnOperationRefine(AgpdCharacter *pAgpdCharacter, INT32 lItemTID, INT32 *plSourceItems);
		BOOL	OnOperationRefineItem(AgpdCharacter *pAgpdCharacter, INT32 *plSourceItems);

		//	Callback setting
		//BOOL	SetCallbackRefine(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackRefineItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		//BOOL	SetCallbackRefineItemResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBadboy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};
#endif
