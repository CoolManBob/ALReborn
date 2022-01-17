/******************************************************************************
Module:  AgsdDeath.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 11
******************************************************************************/

#if !defined(__AGSDDEATH_H__)
#define __AGSDDEATH_H__

#include "ApBase.h"
#include "AsDefine.h"

typedef struct
{
	UINT32				m_ulDeadTime;		// 죽은 시간 (game clock count 단위)
											// (죽기 대기시간, 완전히 죽은시간 : m_fCharStatus에 따라 자장하는 값이 달라진다.)

	BOOL				m_bIsNeedProcessDeath;	// 죽은 담에 Exp, 등등의 처리를 해야 하는지..

	INT64				m_llLastLoseExp;		// 마지막으로 잃은 Exp (유골을 성당등에 가져가서 Exp 복구 받을때 사용된다.)

	ApBaseExLock		m_csLastHitBase;	// 이넘한테 마지막으로 맞어서 죽었다. ㅡ.ㅡ

	BOOL				m_bDropItem;		// 죽은담에 아템을 떨궜는지 여부

	UINT32				m_ulRecoveryHPPointTime;	// HP Point 회복 Interval. 매번 바뀔 수 있어서 이렇게 분리. 2005.04.09. steeple
	UINT32				m_ulRecoveryMPPointTime;	// MP Point 회복 Interval. 매번 바뀔 수 있어서 이렇게 분리. 2005.04.09. steeple

	INT32				m_lDeadTargetCID;	// 2005.07.28. steeple. 누구 때문에 죽었는 지 알기 위해서.
	INT8				m_cDeadType;		// 2005.07.28. steeple. 부활 패널티 때문에 좀 바뀜.

	CTime				m_tCharismaGiveTime;	// 카리스마 포인트를 준 시간(어뷰징 방지용) - arycoat 2008.7.
} AgsdDeath;

typedef struct
{
	INT32				m_lDropRate;		// 죽었을때 Item을 Drop할 확률 (0~100000)
} AgsdDeathADItem;

#endif //__AGSDDEATH_H__
