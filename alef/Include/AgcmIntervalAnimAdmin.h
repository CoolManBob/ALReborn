#ifndef __AGCM_INTERVAL_ANIM_ADMIN_H__
#define __AGCM_INTERVAL_ANIM_ADMIN_H__

#include <windows.h>

#include "rwcore.h"
#include "rpworld.h"
#include "rtquat.h"
#include "rtanim.h"
#include "rphanim.h"

#include "AgcdAnimation.h"

/*#define AGCM_IAA_INTERVAL_ANIM_DATA_NUM			5

typedef BOOL (*__IntervalAnimationCB)(PVOID pvData);

class IntervalAnimData
{
public:
	FLOAT					m_fTime;
	BOOL					m_bLoop;
	__IntervalAnimationCB	m_pfCallback;
	PVOID					m_pavData[AGCM_IAA_INTERVAL_ANIM_DATA_NUM];

	IntervalAnimData()
	{
		m_fTime			= 0.0f;
		m_bLoop			= FALSE;
		m_pfCallback	= NULL;

		for(INT32 lCount = 0; lCount < AGCM_IAA_INTERVAL_ANIM_DATA_NUM; ++lCount)
			m_pavData[lCount] = (PVOID)(-1);
	}
};

class AgcmIntervalAnimAdmin;

class IntervalAnimList
{
public:
	IntervalAnimList();
	virtual ~IntervalAnimList();

public:
	AgcdAnimationCallbackData	m_csData;
	IntervalAnimList			*m_pcsNext;

	AgcmIntervalAnimAdmin		*m_pcsParentModule;
};*/
/*
class AgcmIntervalAnimAdmin
{
public:
	AgcmIntervalAnimAdmin();
	virtual ~AgcmIntervalAnimAdmin();

protected:
	static RtAnimInterpolator	*IntervalAnimationCB(RtAnimInterpolator *animInstance, PVOID pvData);

	BOOL						AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData);
	AgcdAnimationCallbackList	*HeadMoveToTail(AgcdAnimationCallbackList *pstList);
	AgcdAnimationCallbackList	*RemoveAnimList(AgcdAnimationCallbackList *pstCurList);
	BOOL						SetIntervalAnimationCallback();

	static PVOID				SetLoopAnimCB(PVOID pvData);
	static PVOID				SetEndAnimCB(PVOID pvData);
	static PVOID				SetSetNextAnimCB(PVOID pvData);

public:
	// Initialization...
//	VOID	SetIntervalAnimCallback(__IntervalAnimationCB pfCallback);
	VOID	SetIntervalAnimHierarchy(RpHAnimHierarchy **ppstHierarchy);
	VOID	SetMaxIntervalData(INT32 lMax); // 지정하지 않는 경우, 무한으로 리스트를 늘린다.
	// Operations...
	BOOL	AddIntervalData(AgcdAnimationCallbackData *pcsData);
	VOID	ResetIntervalData();
	// Attributes...
	INT32	GetIntervalDataNum()	{return m_lCurListNum;}	

protected:
	RpHAnimHierarchy			**m_ppstHierarchy;
	AgcdAnimationCallbackList	*m_pcsList;
//	__IntervalAnimationCB	m_pfIntervalAnimationCB;

	INT32						m_lCurListNum;
	INT32						m_lMaxListNum;
};
*/
#endif // __AGCM_INTERVAL_ANIM_ADMIN_H__