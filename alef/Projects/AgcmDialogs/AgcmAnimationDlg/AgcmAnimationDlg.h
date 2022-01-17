#pragma once

#include "ApModule.h"
#include "AgcmCharacter.h"
#include "AgcmItem.h"
#include "AgcmUIControl.h"
#include "AgcmEventEffect.h"

#define AGCMANIMATIONDLG_MAX_LINK_ANIMATION			10

enum eAgcmAnimationDlgCallbackPoint
{
	AGCMANIMATIONDLG_CB_ID_START_ANIMATION		= 0,
	AGCMANIMATIONDLG_CB_ID_GET_ANIMATION,
	AGCMANIMATIONDLG_CB_ID_CLOSE_CHAR_ANIM_DLG,
	AGCMANIMATIONDLG_CB_ID_ADD_ANIMATION,
	AGCMANIMATIONDLG_CB_ID_ADD_ANIM_DATA,
	AGCMANIMATIONDLG_CB_ID_REMOVE_ANIM_DATA,
	AGCMANIMATIONDLG_CB_ID_REMOVE_ANIMATION,
	AGCMANIMATIONDLG_CB_ID_REMOVE_ALL_ANIMATION,
	AGCMANIMATIONDLG_CB_ID_READ_RT_ANIM,
	AGCMANIMATIONDLG_CB_ID_START_ANIM,
	AGCMANIMATIONDLG_CB_ID_NUM
};

class AAD_ReadRtAnimParams
{
public:
	AAD_ReadRtAnimParams() :	
	 m_pcsSrcAnimData(NULL),
	 m_bObject(FALSE),
	 m_bRead(FALSE)
	{
	}

	AgcdAnimData2*	m_pcsSrcAnimData;
	BOOL			m_bObject;
	BOOL			m_bRead;
};

class AAD_AddAnimationParams
{
public:
	AAD_AddAnimationParams() :	
	 m_ppSrcAnimation(NULL),
	 m_pszSrcAnimation(NULL),
	 m_bObject(FALSE),
	 m_pDestData(NULL)
	{
	}

	AgcdAnimation2**	m_ppSrcAnimation;
	CHAR*				m_pszSrcAnimation;
	BOOL				m_bObject;

	AgcdAnimData2*		m_pDestData;
};

class AAD_RemoveAnimationParams
{
public:
	AAD_RemoveAnimationParams() :
	 m_pSrcAnimation(NULL),
	 m_szName(NULL),
	 m_bObject(FALSE),
	 m_bRemovedAnimation(FALSE),
	 m_bApply(TRUE)
	{
	}

	AgcdAnimation2*		m_pSrcAnimation;
	CHAR*				m_szName;
	BOOL				m_bObject;
	BOOL				m_bRemovedAnimation;
	BOOL				m_bApply;
};

class AAD_RemoveAllAnimationParams
{
public:
	AAD_RemoveAllAnimationParams() :
	 m_ppSrcAnimation(NULL),
	 m_bObject(FALSE),
	 m_bRemovedAnimation(FALSE),
	 m_bApply(TRUE)
	{
	}

	AgcdAnimation2**	m_ppSrcAnimation;
	BOOL				m_bObject;
	BOOL				m_bRemovedAnimation;
	BOOL				m_bApply;
};

class AAD_AddAnimDataParams
{
public:
	AAD_AddAnimDataParams() :
	 m_pszSrcAnimData(NULL),
	 m_pDestAnimData(NULL)
	{
	}

	CHAR*			m_pszSrcAnimData;
	AgcdAnimData2*	m_pDestAnimData;

};

class AAD_RemoveAnimDataParams
{
public:
	AAD_RemoveAnimDataParams() :
	 m_ppSrcAnimData(NULL),
	 m_bRemoved(FALSE)
	{
	}

	AgcdAnimData2**	m_ppSrcAnimData;
	BOOL			m_bRemoved;
};

class AAD_StartAnimationParams
{
public:
	AAD_StartAnimationParams() :
	 m_pcsAnimData(NULL),
	 m_lAnimType(-1),
	 m_lAnimType2(-1),
	 m_lSkillIndex(0)
	{
	}

	AgcdAnimData2*	m_pcsAnimData;
	INT32			m_lAnimType;
	INT32			m_lAnimType2;
	INT32			m_lSkillIndex;
};


//------------------- AgcmAnimationDlg ------------------
class AFX_EXT_CLASS AgcmAnimationDlg : public ApModule
{
protected :
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgcmItem*			m_pcsAgcmItem;
	AgcmEventEffect*	m_pcsAgcmEventEffect;

	CHAR				m_szFindCharAnimSoundPath[256];
	CHAR				m_szFindCharAnimPath1[256];
	CHAR				m_szFindCharAnimPath2[256];
	CHAR				m_szCharAnimPath[256];

	AgcdCharacter*		m_pcdCharacter;

public :
	AgcmAnimationDlg();
	virtual ~AgcmAnimationDlg();

	static AgcmAnimationDlg* GetInstance();

	BOOL	OnInit();
	BOOL	OnAddModule();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	AgcmEventEffect*	GetAgcmEventEffectModule()	{	return m_pcsAgcmEventEffect;	}
	AgcmCharacter*		GetAgcmCharacterModule()	{	return m_pcsAgcmCharacter;		}
	AgcmItem*			GetAgcmItemModule()			{	return m_pcsAgcmItem;			}

	CHAR*				GetFindCharAnimSoundPath()	{	return m_szFindCharAnimSoundPath;}
	CHAR*				GetFindCharAnimPath1()		{	return m_szFindCharAnimPath1;	}
	CHAR*				GetFindCharAnimPath2()		{	return m_szFindCharAnimPath2;	}
	CHAR*				GetCharAnimPath()			{	return m_szCharAnimPath;		}

	AgcdCharacter*		GetCurAgcdCharacter()		{	return m_pcdCharacter;			}


public :
	BOOL	OpenAnimFlags( AgcdAnimationFlag *pstFlags, ACA_AttachedData *pcsACA );
	BOOL	OpenCharAnim(AgcdCharacter *pcsAgcdCharacter, AgcdCharacterTemplate *pcsAgcdCharacterTemplate);
	BOOL	CloseCharAnim();

	BOOL	IsOpenCharAnim();

	// 초기화시 불러줍시다.
	VOID	SetFindCharAnimPath(CHAR *szPath1, CHAR *szPath2);
	VOID	SetFindCharAnimSoundPath(CHAR *szPath);
	VOID	SetCharAnimPath(CHAR *szPath);
	BOOL	SetCallbackCharStartAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCharGetAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCloseCharAnimDlg(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackAddAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackStartAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL	ReadRtAnim(AAD_ReadRtAnimParams *pcsParams);
	BOOL	AddAnimation(AAD_AddAnimationParams *pcsParams);
	BOOL	AddAnimData(AAD_AddAnimDataParams *pcsParams);
	BOOL	RemoveAnimData(AAD_RemoveAnimDataParams	*pcsParams);
	BOOL	RemoveAnimation(AAD_RemoveAnimationParams *pcsParams);
	BOOL	RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams);
	BOOL	StartAnimation(AAD_StartAnimationParams *pcsParams);

	CHAR**	GetAnimType2Name(INT32 lAnimType2);
	CHAR**	GetAnimTypeName();
	INT32	GetAnimType2Index(CString strCmp);
	INT32	GetAnimTypeIndex(CString strCmp);
};