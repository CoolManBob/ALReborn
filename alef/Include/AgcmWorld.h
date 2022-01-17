#ifndef _AGCM_WORLD_H_
#define _AGCM_WORLD_H_

#include "ApBase.h"
#include "AgcModule.h"
#include "AgpmWorld.h"
#include "AuPacket.h"
#include "AcSocketManager.h"

#include <vector>
#include <algorithm>
using namespace std;

const enum eAGCMWORLD_CB
{
	AGCMWORLD_CB_GET_CHAR_COUNT = 0,
};

//------------------------- AgcdWorldGroup ------------------------
typedef vector< AgpdWorld* >	WorldVec;
typedef WorldVec::iterator		WorldVecItr;

class AgcdWorldGroup
{
public:
	AgcdWorldGroup( AgpdWorld* pWorld )
	{	
		if( pWorld )
		{
			m_strName	= pWorld->m_szGroupName;
			m_nPriority	= pWorld->m_ulPriority;
			m_vecWorld.push_back( pWorld );
		}

		m_bExtend = FALSE;
	}
	~AgcdWorldGroup()		{	m_vecWorld.clear();		}

	bool	operator < ( const AgcdWorldGroup& lhs ) const		{	return m_nPriority < lhs.m_nPriority;	}
	bool	operator > ( const AgcdWorldGroup& lhs ) const		{	return m_nPriority > lhs.m_nPriority;	}
	bool	operator == ( const char* szName ) const			{	return m_strName == szName;				}
	bool	operator == ( const AgcdWorldGroup& lhs ) const		{	return m_strName == lhs.m_strName;		}

	void	ToggleExtend()	{	m_bExtend = m_bExtend ? FALSE : TRUE;	}

public:
	string				m_strName;
	UINT32				m_nPriority;
	WorldVec			m_vecWorld;
	BOOL				m_bExtend;	//UIServerList 사용될 변수
};
typedef vector< AgcdWorldGroup >	WorldGroupVec;
typedef WorldGroupVec::iterator		WorldGroupVecItr;

//------------------------- AgcdWorld ------------------------
class AgcdWorld
{
public :
	INT32		m_lCharacterCount;
};

//------------------------- AgcmWorld ------------------------
class AgcmWorld : public AgcModule
{
public:
	AgcmWorld();
	virtual ~AgcmWorld();

	BOOL	OnAddModule();

	AgcdWorld*	GetAD(ApBase *pApBase);

	BOOL	SendPacketGetWorld(TCHAR *pszWorld, UINT32 ulNID);
	BOOL	SendPacketGetCharCount(TCHAR *pszAccount, UINT32 ulNID);

	BOOL	SetCallbackGetCharCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	void	ResetWorldGroup();
	WorldGroupVec&	GetWorldGroup()		{	return m_vecWorldGroup;	}

	static BOOL	ConAgcdWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	DesAgcdWorld(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBGetCharCount(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBGetWorldAll(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	BOOL	_DecodeWorldCharCount();

public:
	static const DWORD s_dwWorldMax = 30;

public:
	AgpdWorld*	m_pAgpdWorldSelected;

protected:
	AgpmWorld*		m_pAgpmWorld;
	INT16			m_nIndexAD;

	TCHAR			m_szWorldCharCount[2048 + 1];
	BOOL			m_bReceiveWorld;

	WorldGroupVec	m_vecWorldGroup;
};

#endif // _AGCM_WORLD_H_
