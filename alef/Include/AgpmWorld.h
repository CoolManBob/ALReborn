#ifndef _AGPM_WORLD_H_
#define _AGPM_WORLD_H_

#include "ApBase.h"
#include "ApAdmin.h"
#include "ApDefine.h"
#include "ApModule.h"
#include "AuPacket.h"

#include <string>
#include <vector>
using namespace std;

#define AGPDWORLD_MAX_WORLD_NAME		32
#define AGPDWORLD_MAX_SERVER_IN_WORLD	32
#define AGPMWORLD_DELIM1				_T('=')
#define AGPMWORLD_DELIM2				_T(';')
#define AGPMWORLD_MAX_ENCODED_LENGTH	2048

enum eAGPDWORLD_STATUS
{
	AGPDWORLD_STATUS_UNKNOWN = 0,
	AGPDWORLD_STATUS_GOOOOOOD,
	AGPDWORLD_STATUS_ABOVE_NORMAL,
	AGPDWORLD_STATUS_NORMAL,
	AGPDWORLD_STATUS_BELOW_NORMAL,
	AGPDWORLD_STATUS_BAD,
	AGPDWORLD_STATUS_NUM,
};

enum eAGPMWORLD_FLAG
{
	AGPMWORLD_FLAG_NONE			= 0x00,
	AGPMWORLD_FLAG_NEW			= 0x01,
	AGPMWORLD_FLAG_NC17			= 0x02,
	AGPMWORLD_FLAG_EVENT		= 0x04,
	AGPMWORLD_FLAG_AIM_EVENT	= 0x08,
};

enum eAgpmWorldPacketOperation
{
	AGPMWORLD_PACKET_OPERATION_GETWOLRD			= 0,
	AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD,
	AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD_ALL,
	AGPMWORLD_PACKET_OPERATION_GET_CHAR_COUNT,
	AGPMWORLD_PACKET_OPERATION_RESULT_CHAR_COUNT,
};

enum eAgpmWorldDataType
{
	AGPMWORLD_DATA_TYPE_PUBLIC = 0,
	AGPMWORLD_DATA_TYPE_CLIENT,
};


enum eAgpmWorldCallbackPoint
{
	AGPMWORLD_CB_UPDATE_WORLD = 0,
	AGPMWORLD_CB_RESULT_GET_WORLD_ALL,
	AGPMWORLD_CB_GET_WORLD,
	AGPMWORLD_CB_GET_CHAR_COUNT,
	AGPMWORLD_CB_GET_RESULT_CHAR_COUNT,
};

extern INT16	g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_NUM];

//----------------------- AgpdWorld --------------------------
class AgpdWorld : public ApBase	//sort를 하게 해줘야 하는데..
{
public:
	void	Initialize()
	{
		m_Mutex.Init(this);
		ZeroMemory( m_szName, sizeof( m_szName ) );
		ZeroMemory( m_szGroupName, sizeof( m_szGroupName ) );
		m_nStatus	= AGPDWORLD_STATUS_UNKNOWN;
		m_ulPriority= 0;
		m_ulFlag	= AGPMWORLD_FLAG_NONE;
		m_nWorldGroupIndex	= 0;
	}

	void	Cleanup()
	{
		m_Mutex.Destroy();
	}

	void	SetNew(BOOL bSet)		{	_SetFlag(AGPMWORLD_FLAG_NEW, bSet);		}
	void	SetNC17(BOOL bSet)		{	_SetFlag(AGPMWORLD_FLAG_NC17, bSet);	}
	void	SetEvent(BOOL bSet)		{	_SetFlag(AGPMWORLD_FLAG_EVENT,	bSet);	}
	void	SetAimEvent(BOOL bSet)	{	_SetFlag(AGPMWORLD_FLAG_AIM_EVENT,	bSet);}

	BOOL	IsNew()				{	return _GetFlag(AGPMWORLD_FLAG_NEW);	}
	BOOL	IsNC17()			{	return _GetFlag(AGPMWORLD_FLAG_NC17);	}
	BOOL	IsEvent()			{	return _GetFlag(AGPMWORLD_FLAG_EVENT);	}
	BOOL    IsAimEvent()		{	return _GetFlag(AGPMWORLD_FLAG_AIM_EVENT);}

private:
	void	_SetFlag(eAGPMWORLD_FLAG eFlag, BOOL bSet)
	{
		if (bSet)	m_ulFlag |= eFlag;
		else		m_ulFlag &= ~eFlag;
	}

	BOOL	_GetFlag(eAGPMWORLD_FLAG eFlag)
	{
		return (m_ulFlag & eFlag);
	}

public:
	TCHAR			m_szName[AGPDWORLD_MAX_WORLD_NAME + 1];
	TCHAR			m_szGroupName[AGPDWORLD_MAX_WORLD_NAME + 1];
	INT16			m_nStatus;
	UINT32			m_ulPriority;
	UINT32			m_ulFlag;
	INT16			m_nWorldGroupIndex;
	vector<PVOID>	m_ServerList;		//worst case로 status를 산출하기 위해서...
};

//----------------------- AgpaWorld --------------------------
class AgpaWorld : public ApAdmin
{
public:
	AgpaWorld()				{		}
	virtual ~AgpaWorld()	{		}

	AgpdWorld*		AddWorld(AgpdWorld *pcsWorld, TCHAR* pszName);
	BOOL			RemoveWorld(TCHAR* pszName);

	AgpdWorld*		GetWorld(TCHAR* pszName);
	AgpdWorld*		GetWorld(INT32 lIndex);
};

//----------------------------- AgpmWorld -----------------------------
class AgpmWorld : public ApModule
{
public:
	AgpmWorld();
	virtual ~AgpmWorld()		{		}

	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnDestroy();
	BOOL		OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL		SetMaxWorld(INT16 nCount)	{	return m_csAgpaWorld.SetCount(nCount);		}
	AgpdWorld*	GetWorld(TCHAR* pszWorld)	{	return m_csAgpaWorld.GetWorld(pszWorld);	}
	AgpdWorld*	GetWorld(INT32 lIndex)		{	return m_csAgpaWorld.GetWorld(lIndex);		}
	INT32		GetWorldCount()				{	return m_csAgpaWorld.GetObjectCount();		}

	AgpdWorld*	UpdateWorld(TCHAR* pszWorld, INT16 nStatus, TCHAR* pszGroup = NULL, UINT32 m_ulPriority = 0, UINT32 ulFlag = AGPMWORLD_FLAG_NONE, BOOL bEnum = TRUE);	// if world ne then add

	BOOL		SetCallbackUpdateWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackResultGetWorldAll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackGetWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackGetCharacterCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackResultGetCharacterCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);

protected:
	AgpdWorld*	_CreateWorldData();
	BOOL		_DestroyWorldData(AgpdWorld* pcsWorld);
	BOOL		_OnResultGetWorldAll(TCHAR* pszEncodedWorld, INT32 lEncodedWorldLength);

public:
	AuPacket		m_csPacket;

protected:
	AgpaWorld		m_csAgpaWorld;
};

#endif // _AGPM_WORLD_H_
