// AgsmAI.h: interface for the AgsmAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMAI_H__3BBE3A89_783F_49AB_8130_D6E279AC6899__INCLUDED_)
#define AFX_AGSMAI_H__3BBE3A89_783F_49AB_8130_D6E279AC6899__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsEngine.h"
#include "AgpmEventSpawn.h"
#include "AgsmEventSpawn.h"
#include "AgpmAI.h"
#include "AuRandomNumber.h"
#include "AuMath.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmAID" )
#else
#pragma comment ( lib , "AgsmAI" )
#endif
#endif

#define AGSMAI_MAX_PROCESS_TARGET	100

struct AgsdAITargetBaseData
{
	ApBase *		m_pcsTargetBase;
	INT32			m_lCalcWeight;
};

class AgsmAI : public AgsModule  
{
private:
	ApmMap *				m_pcsApmMap;
	AgpmFactors *			m_pcsAgpmFactors;
	ApmObject *				m_pcsApmObject;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmGrid *				m_pcsAgpmGrid;
	AgpmItem *				m_pcsAgpmItem;
	AgpmAI *				m_pcsAgpmAI;
	AgsmCharacter *			m_pcsAgsmCharacter;
	ApmEventManager *		m_pcsApmEventManager;
	AgpmEventSpawn *		m_pcsAgpmEventSpawn;
	AgsmEventSpawn *		m_pcsAgsmEventSpawn;

	MTRand					m_csRand;

public:
	AgsmAI();
	virtual ~AgsmAI();

	BOOL			OnAddModule();

	VOID			PreProcessAI(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount);
	VOID			ProcessAI(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount);
	BOOL			SortActions(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount);
	BOOL			ProcessActions(AgpdCharacter *pcsCharacter, AgpdAIADChar *pstAIADChar, UINT32 ulClockCount);
	BOOL			ProcessAction(AgpdCharacter *pcsCharacter, AgpdAIActionType eActionType, AgpdAIAction *pstAction, UINT32 ulClockCount);

	INT32			GetTargetBase(AgpdCharacter *pcsCharacter, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount, ApBase *apcsTarget[]);
	BOOL			GetTargetPosition(AgpdCharacter *pcsCharacter, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount, AuPOS *pstTarget);

	BOOL			CalcTargetWeight(AgpdCharacter *pcsCharacter, AgsdAITargetBaseData *pstTargetData, AgpdAI *pstAI, AgpdAIAction *pstAction, UINT32 ulClockCount);

	VOID			AddActionToResult(AgpdAIADChar *pstAIADChar, AgpdAIFactor *pstAIFactor);

	BOOL			IsFactorApplicable(AgpdCharacter *pcsCharacter, AgpdAIFactor *pstAIFactor, UINT32 ulClockCount);

	static BOOL		CBUpdateCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL		CBSendCharacterAllInfo(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	BOOL			SendPacketAIADChar(AgpdCharacter *pcsCharacter, UINT32 ulDPNID);

	INT32			GetHelpMobList( AgpdCharacter *pcsAgpdCharacter, INT32 *plaCharList, int lCount );
	BOOL			ProcessHelpScreamL1(AgpdCharacter *pcsCharacter);
	BOOL			ProcessHelpScreamL2(AgpdCharacter *pcsCharacter);
	BOOL			ProcessHelpScreamL3(AgpdCharacter *pcsCharacter);

	//static INT32	QSortCompare(const PVOID pvData1, const PVOID pvData2);
	void __cdecl	QSort(void *, unsigned int, unsigned int, int (__cdecl *)(const void *, const void *));
};

#endif // !defined(AFX_AGSMAI_H__3BBE3A89_783F_49AB_8130_D6E279AC6899__INCLUDED_)
