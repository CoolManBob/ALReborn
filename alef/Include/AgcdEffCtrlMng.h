#if !defined(AFX_AGCDEFFCTRLMNG_H__C15303F3_046C_4F6F_A0FD_7C6384EE4368__INCLUDED_)
#define AFX_AGCDEFFCTRLMNG_H__C15303F3_046C_4F6F_A0FD_7C6384EE4368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>
#include "AgcdEffCtrl.h"
#include "ContainerUtil.h"

class AgcdEffCtrlMng  
{
	enum eWorkType
	{
		E_WORK_INSERT		= 0,
		E_WORK_TIMEUPDATE	,
		E_WORK_UPDATE		,
		E_WORK_RENDER		,
	};

	struct stEffectCtrlSetEntry
	{
		AgcdEffCtrl_Set*						m_pcdEffCtrl_Set;
		INT32									m_nEffectID;
		INT32									m_nCreateID;

		stEffectCtrlSetEntry( void )
		{
			m_pcdEffCtrl_Set	=	NULL;
			m_nEffectID			=	-1;
			m_nCreateID			=	-1;
		}
	};

	struct stEffectPreSetEntry
	{
		AgcdPreEffSet*							m_pcdPreSet;
		INT32									m_nEffectID;
		INT32									m_nCreateID;

		stEffectPreSetEntry( void )
		{
			m_pcdPreSet = NULL;
			m_nEffectID = -1;
			m_nCreateID = -1;
		}
	};

	struct stEffectPreLoadEntry
	{
		INT32									m_nEffectID;
		BOOL									m_bIsPreLoadComplete;

		stEffectPreLoadEntry( void )
		{
			m_nEffectID = -1;
			m_bIsPreLoadComplete = FALSE;
		}
	};

public:

	typedef std::map< INT32 , stEffectCtrlSetEntry	>	mapCtrlSet;
	typedef std::map< INT32 , stEffectPreSetEntry	>	mapPreSet;
	typedef std::map< INT32 , stEffectPreLoadEntry	>	mapPreLoad;

	typedef	mapCtrlSet::iterator						mapCtrlSetIter;
	typedef mapPreSet::iterator							mapPreSetIter;
	typedef mapPreLoad::iterator						mapPreLoadIter;

private:

	RwUInt32						m_dwInsID;
	mapCtrlSet						m_mapCtrlSet;
	mapPreSet						m_mapPreSet;
	mapPreLoad						m_mapPreLoad;
	
public:
	AgcdEffCtrlMng						( VOID );
	virtual ~AgcdEffCtrlMng				( VOID );

	void		ClearCtrlSet			( VOID );
		
	void		InsertCtrlSet			( LPEFFCTRL_SET pEffCtrl_Set );

	BOOL		RemoveCtrl				( LPEFFCTRL_SET		pEffCtrlSet );
	BOOL		RemoveEffSet			( LPEFFCTRL_SET		pEffCtrlSet		, BOOL bCallCB=TRUE				);

	void		InsertPreEff			( LPEFFSET pEffSet, stEffUseInfo* Info, AgcdEffCtrl_Set* pNewEffCtrl_Set);

	void		TimeUpdate				( RwUInt32 dwDifTick);
	
	RwInt32		GetCurrCnt				( VOID )			{ return m_mapCtrlSet.size(); };
	RwUInt32	GetLastInsertID			( VOID ) const		{ return m_dwInsID; };

	void		ToneDown				( VOID );
	void		ToneRestore				( VOID );

	bool		ValidParentClump		( stEffUseInfo* pInfo );
	void		RemoveAllEffectByClump	( RpClump* pClump );
	void		UpdateLoadedEffCtrlList	( VOID );
	void		ClearPreSet				( VOID );
	//void		DeletePreSet			( AgcdEffCtrl_Set* pcdEffCtrl_Set );
	
	RwInt32		ForTool_DeleteEff		( RwUInt32 dwEffSetID);
	BOOL		FindEffectSet			( AgcdEffCtrl_Set* pcdEffCtrl_Set );
	BOOL		IsVaildCtrlSet			( AgcdEffCtrl_Set* pcdEffCtrl_Set );

};

#endif