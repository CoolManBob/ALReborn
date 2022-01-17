#ifndef __AGCM_ANIMATION_H__
#define __AGCM_ANIMATION_H__

//#include "ApModule.h"
//#include "AgcaAnimation.h"
//#include "AgcmIntervalAnimAdmin.h"

/*class AgcmAnimation
{
public:
	AgcmAnimation();
	virtual ~AgcmAnimation();

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
};*/

#endif // __AGCM_ANIMATION_H__