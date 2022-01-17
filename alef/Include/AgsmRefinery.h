/*====================================================================

	AgsmRefinery.h

====================================================================*/


#ifndef _AGSM_REFINERY_H_
	#define _AGSM_REFINERY_H_


#include "AgsEngine.h"
#include "AgpmRefinery.h"
#include "AgpmFactors.h"
#include "AgpmDropItem2.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmCharacter.h"
#include "AuRandomNumber.h"
#include "AgsdRefinery.h"
#include "hash_map"

//using namespace stdext;


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMREFINERY_RESULT_INTERVAL			500

typedef pair<INT32, AgsdRefineOptionGroup>					OptionGroupPair;
typedef hash_map<INT32, AgsdRefineOptionGroup>::iterator	OptionGroupIter;


typedef pair<INT32, AgsdRefineSocket>					SocketPair;
typedef hash_map<INT32, AgsdRefineSocket>::iterator		SocketIter;




/********************************************/
/*		The Definition of AgsmRefinery		*/
/********************************************/
//
class AgsmRefinery : public AgsModule
	{
	private:
		//	Related modules
		AgpmRefinery		*m_pAgpmRefinery;
		AgpmDropItem2		*m_pAgpmDropItem2;
		AgsmItem			*m_pAgsmItem;
		AgsmItemManager		*m_pAgsmItemManager;
		AgsmCharacter		*m_pAgsmCharacter;
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmFactors			*m_pAgpmFactors;
		AgpmItemConvert		*m_pAgpmItemConvert;
		MTRand				m_csRandom;

		//	Map
		hash_map<INT32, AgsdRefineOptionGroup>	m_MapOptionGroup;
		hash_map<INT32, AgsdRefineSocket>		m_MapSocket;
		

	public:
		AgsmRefinery();
		virtual ~AgsmRefinery();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();
		BOOL	OnIdle(UINT32 ulClockCount);
		BOOL	OnDestroy();

		//	Map
		AgsdRefineOptionGroup*	GetOptionGroup(INT32 lID);
		AgsdRefineSocket*		GetSocket(INT32 lID);

		//	Stream
		//BOOL	StreamReadOptionGroup(CHAR *pszFile, BOOL bDecryption);
		//BOOL	StreamReadSocketInchant(CHAR *pszFile, BOOL bDecryption);

		//	Callbacks
		//static BOOL	CBRefine(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBRefineItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBadboy(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Operations (after callback)
		//BOOL	OnRefine(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 *plSourceItems);
		BOOL	OnRefineItem(AgpdCharacter *pAgpdCharacter, INT32* lSourceItems);
		//	Send Packet
		BOOL	SendPakcetRefineResult(INT32 lCID, INT32 lItemTID, INT32 eResult, INT32 lResultItemTID);

	protected:
		//	Helper
		BOOL	DeleteSourceItem(AgpdCharacter *pAgpdCharacter, AgpdRefineTemplate *pAgpdRefineTemplate, INT32 *plSourceItems);
		BOOL	DeleteSourceItem(AgpdCharacter *pAgpdCharacter, INT32 lSourceItems[], INT32 lSourceItemsCount[]);
        BOOL    ApplyOption(AgpdRefineMakeItem* item, AgpdItem *pAgpdItem, AgpdCharacter *pcsCharacter);
		BOOL	ApplySocket(AgpdRefineMakeItem* item, AgpdItem *pAgpdItem);
		BOOL	ApplyStamina(AgpdRefineMakeItem* item, AgpdItem* pAgpdItem, AgpdItem* pcsSourceItems[]);
	};




#endif
