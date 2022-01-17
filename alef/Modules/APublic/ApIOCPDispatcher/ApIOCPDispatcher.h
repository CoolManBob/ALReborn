/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// ApIOCPQueue.h: interface for the ApIOCPQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APIOCPDISPATCHER_H__E190C2C8_7DA2_4668_B2A6_763E4986F80F__INCLUDED_)
#define AFX_APIOCPDISPATCHER_H__E190C2C8_7DA2_4668_B2A6_763E4986F80F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApIOCP.h"
#include "ApPacket.h"
#include "AsDefine.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "ApIOCPDispatcherD" )
#else
#pragma comment ( lib , "ApIOCPDispatcher" )
#endif
#endif

class ApIOCPDispatcher : public ApIOCP
{
public:
	ApIOCPDispatcher();
	virtual ~ApIOCPDispatcher();
	
	BOOL CreateWalkerThread(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam, CHAR *pThreadName);
	VOID DestroyWalkerThread();

	BOOL Dispatch(pstCOMMAND pstCommand);
	BOOL Dispatch(BOOL bServer, INT32 lSocketOwnerID, UINT16 unType, UINT32 dpnid, UINT16 unDataSize, PVOID pData, PVOID pvObject = NULL);

	BOOL Dispatch(PVOID pvBuffer, DWORD dwBytes = 0);
};

#endif // !defined(AFX_APIOCPDISPATCHER_H__E190C2C8_7DA2_4668_B2A6_763E4986F80F__INCLUDED_)
