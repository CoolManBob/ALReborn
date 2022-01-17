//	AgpmEventBinding.h
//////////////////////////////////////////////////////////////////////

#ifndef	__AGPMEVENTBINDING_H__
#define	__AGPMEVENTBINDING_H__

#include "ApmMap.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "ApmEventManager.h"
#include "AuGenerateID.h"

#include "AgpaEventBinding.h"
#include "AgpdEventBinding.h"

#include "AgpmCharacter.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventBindingD" )
#else
#pragma comment ( lib , "AgpmEventBinding" )
#endif
#endif
//@} Jaewon

class AgpmBattleGround;

class AgpmEventBinding : public ApModule
{
private:
	AgpmFactors				*m_pcsAgpmFactors;
	ApmMap					*m_pcsApmMap;
	AgpmCharacter			*m_pcsAgpmCharacter;
	ApmEventManager			*m_pcsApmEventManager;
	AgpmBattleGround*		m_pcsAgpmBattleGround;

	AgpaEventBinding		m_csAdminBinding;

	AuGenerateID			m_csGenerateID;
	
public:
	AgpmEventBinding();
	virtual ~AgpmEventBinding();

	// Virtual Function ต้
	BOOL					OnAddModule();
	BOOL					OnInit();
	BOOL					OnDestroy();

	AgpdBinding*			CreateBindingData();
	BOOL					DestroyBindingData(AgpdBinding *pcsBinding);

	BOOL					SetMaxBindingData(INT32 lCount);

	AgpdBinding*			AddBinding(CHAR *szBindingName, CHAR *szTownName, AuPOS *pstBasePos, UINT32 ulRadius, AgpdBindingType eBindingType);
	BOOL					RemoveBinding(INT32 lID);
	BOOL					RemoveBinding(CHAR *szBindingName);
	AgpdBinding*			GetBinding(INT32 lID);
	AgpdBinding*			GetBinding(CHAR *szBindingName);
	AgpdBinding*			GetBindingByTown(CHAR *szTownName, AgpdBindingType eBindingType);
	INT32					GetBindingsByTown(CHAR *szTownName, AgpdBindingType eBindingType, AgpdBinding * apBindingList[]);
	INT32					GetBindingsByTown2(CHAR *szTownName, AgpdBindingType eBindingType, AgpdBinding * apBindingList[], BOOL bInner, AuPOS *pstBasePos);
	AgpdBinding*			GetBindingForNewCharacter(INT32 lRace, INT32 lClass);

	BOOL					GetBindingPositionForResurrection(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos);
	BOOL					GetBindingPositionForNewCharacter(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos);
	BOOL					GetBindingPositionForSiegeWarResurrection(AgpdCharacter *pcsCharacter, AuPOS *pstDestPos, CHAR *szCastle, BOOL bOffense, BOOL bInner, INT32 lIndex = 0, BOOL bCheckBlocking = TRUE);
	BOOL					GetBindingForArchlord(AuPOS *pstDestPos, CHAR *szCastle);
	BOOL					GetBindingForArchlordAttacker(AuPOS *pstDestPos, CHAR *szCastle);

	static BOOL				CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	INT32					GetLength(AuPOS *pstPos1, AuPOS *pstPos2);
};

#endif	//__AGPMEVENTBINDING_H__