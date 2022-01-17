#ifndef __AGCA_EFFECT_DATA_H__
#define __AGCA_EFFECT_DATA_H__

#include "ApAdmin.h"
#include "AgcdEffectData.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcaEffectDataD" )
#else
#pragma comment ( lib , "AgcaEffectData" )
#endif
#endif

/*
class AgcaEffectData : public ApAdmin
{
public:
	AgcaEffectData();
	virtual ~AgcaEffectData();

protected:
	INT32					m_lMax;
	INT32					m_lCount;

public:
	BOOL					Initialize(INT32 lMaxDataNum);

	AgcdUseEffectSetData	*CreateData();
	BOOL					RemoveAll();

};*/

class AgcaEffectData2 : public ApAdmin
{
public:
	AgcaEffectData2();
	virtual ~AgcaEffectData2();

protected:
	UINT32		m_ulCount;

public:
	AgcdUseEffectSetData	*Create(UINT32 ulIndex, AgcdUseEffectSetData *pcsSrc = NULL);
	VOID					Delete(AgcdUseEffectSetData *pcsData, BOOL bFree);
	VOID					Delete(AgcdUseEffectSet *pcsSet, AgcdUseEffectSetData *pcsDeleteData);
	VOID					DeleteAll(AgcdUseEffectSet *pcsSet);

	AgcdUseEffectSetData	*GetData(AgcdUseEffectSet *pcsSet, UINT32 ulFindIndex, BOOL bCreate = FALSE);
	VOID					CopyData(AgcdUseEffectSetData *pcsDest, AgcdUseEffectSetData *pcsSrc);
	BOOL					CopyDataSet(AgcdUseEffectSet *pcsDestSet, AgcdUseEffectSet *pcsSrcSet);

	VOID					Delete(AgcdUseEffectSetData *pcsData);	
};

#endif // __AGCA_EFFECT_DATA_H__