/*****************************************************************************
* File : AgcaAnimation.h
*
* Desc :
*
* 041503 Bob Jung.
*****************************************************************************/

#ifndef __AGCA_ANIMATION_H__
#define	__AGCA_ANIMATION_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmAnimationD" )
#else
#pragma comment ( lib , "AgcmAnimation" )
#endif
#endif

#include "ApBase.h"
#include "ApAdmin.h"
//#include "ApModule.h"
#include "AcDefine.h"
#include "ApMemory.h"
#include "AgcdAnimation.h"

#define		D_AGCA_ANIMATION_MAX_ATTACHED_DATA						5
//#define		D_AGCA_ANIMATION_MAX_ATTACHED_DATA_KEY					32

typedef enum AgcaAnimationCallbackPoint
{
	AGCAANIMATION_CB_POINT_REMOVE_ANIMATION,
	AGCAANIMATION_MAX_CB_POINT
} AgcaAnimationCallbackPoint;

class AgcdAnimationCallbackList : public ApMemory<AgcdAnimationCallbackList, 20000>
{
public:
	AgcdAnimationCallbackData	m_csData;
	AgcdAnimationCallbackList	*m_pcsNext;

	PVOID						m_pvClass;

	__AnimationCallback			m_pfLoopFunc;
	__AnimationCallback			m_pfEndFunc;
	__AnimationCallback			m_pfCustFunc;



	AgcdAnimationCallbackList()
	{
		m_pcsNext			= NULL;
		m_pvClass			= NULL;
		m_pfLoopFunc		= NULL;
		m_pfEndFunc			= NULL;
		m_pfCustFunc		= NULL;
	}
	
	virtual ~AgcdAnimationCallbackList() {}
};

class AgcdAnimationTime
{
public:
	AgcdAnimationTime();
	virtual ~AgcdAnimationTime();

protected:
	AgcdAnimationCallbackList	*m_pcsList;

	INT32						m_lNumCurCB;
	INT32						m_lNumMaxCB;

public:
	FLOAT						m_fCurrentTime;
	FLOAT						m_fDuration;

protected:
	BOOL						AddAnimList(AgcdAnimationCallbackList *pstPreList, AgcdAnimationCallbackList **ppstCurList, AgcdAnimationCallbackList *pstNextList, AgcdAnimationCallbackData *pcsData);
	AgcdAnimationCallbackList	*HeadMoveToTail(AgcdAnimationCallbackList *pstList);
	AgcdAnimationCallbackList	*RemoveAnimList(AgcdAnimationCallbackList *pstCurList);

public:
	///////////////////////////////////
	// Inializations
	// 지정하지 않는 경우 무한으로 Callback을 설정한다!
	VOID	SetMaxCallback(INT32 lMax);

	///////////////////////////////////
	// Operations
	// 내부적으로 ResetAnimCB을 부른다.
	VOID	InitializeAnimation(FLOAT fDuration);

	///////////////////////////////////
	// Attributes
	BOOL	AddAnimCB(AgcdAnimationCallbackData *pcsData);
	VOID	ResetAnimCB(BOOL InfluenceNextAnimation = TRUE);

	INT32	ParseAnimPoint(CHAR *szAnimPoint, INT32 *plDest);

	INT32	GetNumCallback()	{return m_lNumCurCB;}
	FLOAT	GetDuration()		{return m_fDuration;}

	///////////////////////////////////
	// Update
	BOOL	AddTime(FLOAT fTime);
};

class AgcaAnimation2
{
protected:
	ApAdmin			m_csAdminRtAnim;
	ApAdmin			m_csAdminFlags;

	UINT32			m_ulNumAttachedData;
	CHAR			*m_apszKeyAttachedData[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];
	UINT32			m_aulSizeAttachedData[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];
	AcCallbackData2	m_pafRemoveAttachedDataCB[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];
	PVOID			m_pavRemoveAttachedDataCBData[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];

	CHAR			*m_pszPath;
	CHAR			*m_pszExtension;

	UINT32			m_ulNumFlags;

	ApCriticalSection	m_csCSection;

public:
	AgcaAnimation2();
	virtual ~AgcaAnimation2();

	BOOL			SetMaxRtAnim(UINT32 ulMaxAnimation);
	BOOL			SetMaxFlags(UINT32 ulMaxFlags);
	BOOL			SetAttachedData(CHAR *pszKey, UINT32 ulDataSize, AcCallbackData2 pfRemoveCB, PVOID pvRemoveCBData);
	VOID			SetAnimationPathAndExtension(CHAR *pszPath, CHAR *pszExtension);

	BOOL			CopyAnimation(AgcdAnimation2 *pcsSrc, AgcdAnimation2 **ppcsDest);
	AgcdAnimData2	*AddAnimation(AgcdAnimation2 **ppcsAnimation2, CHAR *pszName);
	BOOL			RemoveAnimation(AgcdAnimation2 *pcsAnimation2, CHAR *pszName);
	BOOL			RemoveAllAnimation(AgcdAnimation2 **ppcsAnimation2);

	AgcdAnimData2	*GetAnimData(AgcdAnimation2 *pcsAnimation2, INT32 lIndex);
	INT32			GetAnimDataIndex(AgcdAnimation2 *pcsAnimation2, AgcdAnimData2 *pcsAnimData2);

	AgcdAnimData2	*AddAnimData(CHAR *pszName, BOOL bAttachData);
	BOOL			RemoveAnimData(AgcdAnimData2 **ppcsAnimData);

	BOOL			RemoveAllRtAnim();

	BOOL			ReadRtAnim(AgcdAnimation2 *pcsAnimation2);
	BOOL			ReadRtAnim(AgcdAnimData2 *pcsAnim);

	BOOL			RemoveRtAnim(AgcdAnimation2 *pcsAnimation2);
	BOOL			RemoveRtAnim(AgcdAnimData2 *pcsAnim);

	BOOL			AddFlags(AgcdAnimationFlag **ppFlags);

	PVOID			GetAttachedData(CHAR *pszKey, AgcdAnimData2 *pcsData);

protected:
	AgcdAnimData2	*AddAnimation(AgcdAnimation2 *pcsAnimation2, AgcdRtAnim *pcsRtAnim, CHAR *pszName);

	AgcdAnimData2	*CreateAnimData(AgcdRtAnim *pcsRtAnim, BOOL bAttachData);
	BOOL			DestroyAnimData(AgcdAnimData2 *pcsAnimData);

	AgcdRtAnim		*AddRtAnim(CHAR *pszName);
	AgcdRtAnim		*GetRtAnim(CHAR *pszName, BOOL bAdd = TRUE);
};

/*class AgcaAnimation : public ApAdmin
{
protected:
	ApAdmin			m_csAnimDataAdmin;

	INT32			m_lAllocAnimData;
	INT32			m_lNumAttachedData;
	UINT32			m_aulAttachedDataSize[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];
	CHAR			*m_apszAttachedDataKey[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];

	PVOID					m_pavRemoveAnimationCBClass[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];
	ApModuleDefaultCallBack	m_pafRemoveAnimationCB[D_AGCA_ANIMATION_MAX_ATTACHED_DATA];

public:
	AgcaAnimation();
	~AgcaAnimation();

	// 초기화시 꼭 불러준당...
	BOOL			Initialize(INT32 lMaxAdminAnimation, INT32 lAllocAnimData);
	BOOL			SetCallbackRemoveAnimation(CHAR *szKey, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	AgcdAnimation	*GetAnimation(CHAR *szName, BOOL bAdd = FALSE);
	// szPath인자를 줄 경우, 찾는 데이터가 없으면 추가한다.
	AgcdAnimData	*GetAnimData(CHAR *szName, CHAR *szPath = NULL);

	BOOL			RemoveAnimData(CHAR *szKey);
	BOOL			RemoveAnimation(CHAR *szKey);
	BOOL			RemoveAll();

	BOOL			AttachData(CHAR *szKey, UINT32 ulDataSize);
	PVOID			GetAttachedData(AgcdAnimation *pstAnim, CHAR *szKey);	

protected:
	INT32			GetAttachedDataIndex(CHAR *szKey);

	VOID			RemoveAnimation(AgcdAnimation *pstAnim, CHAR *szKey = NULL);
	VOID			RemoveAnimData(AgcdAnimData *pstAnimData, CHAR *szKey = NULL);

	AgcdAnimation	*AddAnimation(CHAR *szName);
	AgcdAnimData	*AddAnimData(CHAR *szName, CHAR *szPath);

	BOOL			AllocateAttachedData(AgcdAnimation *pstAnimation);
};*/

#endif // __AGCA_ANIMATION_H__

/******************************************************************************
******************************************************************************/