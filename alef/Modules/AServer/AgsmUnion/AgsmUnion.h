#ifndef	__AGSMUNION_H__
#define	__AGSMUNION_H__

#include "ApBase.h"
#include "AgsEngine.h"

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmUnion.h"

#include "AgsmFactors.h"
#include "AgsmCharacter.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmUnionD" )
#else
#pragma comment ( lib , "AgsmUnion" )
#endif
#endif


class AgsmUnion : public AgsModule {
private:
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmUnion			*m_pcsAgpmUnion;

	AgsmFactors			*m_pcsAgsmFactors;
	AgsmCharacter		*m_pcsAgsmCharacter;

public:
	AgsmUnion();
	~AgsmUnion();

	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	//////////////////////////////////////////////////////////////////////
	// Union Rank (point) 처리 함수들
	//////////////////////////////////////////////////////////////////////
	INT32				GetBonusUnionRank(AgpdCharacter *pcsCharacter, INT32 lBonusExp, AgpdItem *pcsSkull);
	INT32				GetPenaltyUnionRankByUvU(AgpdCharacter *pcsCharacter);
	INT32				GetPenaltyUnionRankByNormal(AgpdCharacter *pcsCharacter);
	INT32				GetRestoreExpBySkull(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull);

	BOOL				CheckUnionKiller(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull);

	static BOOL			CBUpdateUnionRankPoint(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBUpdateUnionRank(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGSMUNION_H__