/******************************************************************************
Module:  AgsdAccount.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 27
******************************************************************************/

#if !defined(__AGSDACCOUNT_H__)
#define __AGSDACCOUNT_H__

#include "ApBase.h"
#include "ApDefine.h"
#include "AgpdCharacter.h"
#include "AgpmLogin.h"
#include "AgpdBillInfo.h"
#include "AgpmWorld.h"

const int AGSMACCOUNT_MAX_ACCOUNT_NAME			= 32;
const int AGSMACCOUNT_MAX_ACCOUNT_CHARACTER		= 5;
const int AGSMACCOUNT_MAX_SOCIAL_NUMBER			= 32;

const int AGSMACCOUNT_MAX_COUPON				= 32;
const int AGSMACCOUNT_MAX_COUPONNO				= 64;

typedef struct _CouponInfo {
	BOOL		m_bEnable;
	CHAR		m_szCouponNo[AGSMACCOUNT_MAX_COUPONNO + 1];
	INT32		m_lCouponType;
	CHAR		m_szCharName[AGPACHARACTER_MAX_ID_STRING + 1];
	CHAR		m_szWorldName[AGPDWORLD_MAX_WORLD_NAME + 1];
} CouponInfo;

typedef struct _AccountCoupon {
	INT32		m_lNumCoupon;
	CouponInfo	*m_pcsCouponInfo;
} AccountCoupon;

class AgsdAccount : public ApBase {
public:
	CHAR			m_szName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];

	INT16			m_nNumChar;

	//INT32			m_lCID[AGSMACCOUNT_MAX_ACCOUNT_CHARACTER];
	ApSafeArray<INT32, AGSMACCOUNT_MAX_ACCOUNT_CHARACTER>		m_lCID;
	CHAR			m_szCharName[AGSMACCOUNT_MAX_ACCOUNT_CHARACTER][AGPACHARACTER_MAX_ID_STRING + 1];

	AgpdLoginStep	m_eLoginStep;

	UINT32			m_ulNID;

	CHAR			m_szSocialNumber[AGSMACCOUNT_MAX_SOCIAL_NUMBER + 1];

	AgpdBillInfo	m_csBillInfo;

	AccountCoupon	m_csCoupon;

	UINT32			m_ulRemoveTimeMSec;
};

#endif //__AGSDACCOUNT_H__