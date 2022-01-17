#ifndef __AGCD_ANIMATION_H__
#define __AGCD_ANIMATION_H__


#include "rwcore.h"
#include "rphanim.h"
#include "rpskin.h"

#define AGCD_MAX_ANIMATION_ANIM_POINT	10
//#define AGCD_MAX_ANIMATION				10
#define AGCD_ANIMATION_NAME_LENGTH		64
#define AGCD_ANIMATION_DEFAULT_EXT		"RWS"

typedef enum
{
	AGCD_ANIMATION_FLAG_TYPE_LOOP				= 0,
	AGCD_ANIMATION_FLAG_TYPE_BLEND,
//	AGCD_ANIMATION_FLAG_TYPE_END_THEN_CHANGE,
	AGCD_ANIMATION_FLAG_TYPE_LINK
} eAgcdAnimationFlagType;

typedef enum
{
	AGCD_ANIMATION_FLAG_EMPTY					= 0, // default
	AGCD_ANIMATION_FLAG_LOOP					= 1 << AGCD_ANIMATION_FLAG_TYPE_LOOP,
	AGCD_ANIMATION_FLAG_BLEND					= 1 << AGCD_ANIMATION_FLAG_TYPE_BLEND,
//	AGCD_ANIMATION_FLAG_END_THEN_CHANGE			= 1 << AGCD_ANIMATION_FLAG_TYPE_END_THEN_CHANGE,
	AGCD_ANIMATION_FLAG_LINK					= 1 << AGCD_ANIMATION_FLAG_TYPE_LINK
} eAgcdAnimationFlag;

class AgcdAnimationFlag
{
public:
	UINT16			m_unAnimFlag;
	UINT16			m_unPreference;

	AgcdAnimationFlag()
	{
		m_unAnimFlag	= 0;
		m_unPreference	= 0;
	}
};
/*
typedef struct
{
//	CHAR				m_szANMName[AGCD_ANIMATION_NAME_LENGTH];
	RtAnimAnimation		*m_pstAnim;
	UINT32				m_ulRefCount;
} AgcdAnimData;

typedef struct
{
//	AgcdAnimData		*m_pastAnimData[AGCD_MAX_ANIMATION];
//	stAgcdAnimationFlag	m_stAnimFlag;
//	INT32				m_alActiveRate[AGCD_MAX_ANIMATION];
//	INT32				m_alCustAnimType[AGCD_MAX_ANIMATION];
//	CHAR				*m_paszAnimPoint[AGCD_MAX_ANIMATION];

	AgcdAnimData		**m_ppastAnimData;
//	AgcdAnimData		**m_ppastBlendingAnimData;
	PVOID				*m_pavAttachedData;
} AgcdAnimation;
*/

class AgcdRtAnim
{
public:
	RtAnimAnimation*	m_pstAnimation;
	INT32				m_lReference;

	AgcdRtAnim()
	{
		m_pstAnimation	= NULL;
		m_lReference	= 0;
	}
};

class AgcdAnimData2
{
public:
	AgcdAnimData2*		m_pcsNext;

	AgcdRtAnim*			m_pcsRtAnim;
	CHAR*				m_pszRtAnimName;

	PVOID*				m_pavAttachedData;

	AgcdAnimData2()
	{
		m_pcsNext			= NULL;
		m_pcsRtAnim			= NULL;
		m_pszRtAnimName		= NULL;
		m_pavAttachedData	= NULL;
	}
};

class AgcdAnimation2
{
public:
	AgcdAnimData2		*m_pcsHead;
	AgcdAnimData2		*m_pcsTail;

	AgcdAnimation2()
	{
		m_pcsHead			= NULL;
		m_pcsTail			= NULL;
	}
};

#define AGCD_ANIM_CB_PARAMS_CUST_DATA_NUM			10
typedef PVOID (*__AnimationCallback)(PVOID pvData);

class AgcdAnimationCallbackData
{
public:
	FLOAT					m_fTime;
	FLOAT					m_fPlaySpeed;
	BOOL					m_bLoop;
	BOOL					m_bInfluenceNextAnimation;
	__AnimationCallback		m_pfCallback;
	PVOID					m_pavData[AGCD_ANIM_CB_PARAMS_CUST_DATA_NUM];

	AgcdAnimationCallbackData()
	{
		m_fTime						= 0.0f;
		m_fPlaySpeed				= 1.0f;
		m_bLoop						= FALSE;
		m_bInfluenceNextAnimation	= FALSE;
		m_pfCallback				= NULL;

		for(INT32 lCount = 0; lCount < AGCD_ANIM_CB_PARAMS_CUST_DATA_NUM; ++lCount)
			m_pavData[lCount] = (PVOID)(-1);
	}
};



#endif // __AGCD_ANIMATION_H__