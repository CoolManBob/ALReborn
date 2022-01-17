/******************************************************************************
Module:  AgsdFactors.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 10. 16
******************************************************************************/

#if !defined(__AGSDFACTORS_H__)
#define __AGSDFACTORS_H__

#include "ApBase.h"
#include "AsDefine.h"

#include "AgpmFactors.h"

class AgsdFactors {
public:
	UINT32				m_ulDeadTime;		// 죽은 시간 (game clock count 단위)
											// (죽기 대기시간, 완전히 죽은시간 : m_fCharStatus에 따라 자장하는 값이 달라진다.)

	INT16				m_nLastLoseExp;		// 마지막으로 잃은 Exp (유골을 성당등에 가져가서 Exp 복구 받을때 사용된다.)

	pstHITHISTORY		m_pstHitHistory;	// Hit History
};

#endif //__AGSDFACTORS_H__