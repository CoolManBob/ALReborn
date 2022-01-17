// ApIOCPDispatcher.cpp: implementation of the ApIOCPQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "ApIOCPDispatcher.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApIOCPDispatcher::ApIOCPDispatcher()
{

}

ApIOCPDispatcher::~ApIOCPDispatcher()
{

}

BOOL ApIOCPDispatcher::CreateWalkerThread(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam, CHAR *ThreadName)
{
	return Create(lThreadCount, WalkerThread, pvParam, ThreadName);
}

VOID ApIOCPDispatcher::DestroyWalkerThread()
{
	Destroy();
}

BOOL ApIOCPDispatcher::Dispatch(pstCOMMAND pstCommand)
{
	return PostStatus((ULONG_PTR)pstCommand);
}

BOOL ApIOCPDispatcher::Dispatch(BOOL bServer, INT32 lSocketOwnerID, UINT16 unType, UINT32 dpnid, UINT16 unDataSize, PVOID pData, PVOID pvObject)
{
	pstCOMMAND pstCommand = new stCOMMAND;
	pstCommand->stCheckArg.bReceivedFromServer	= bServer;
	pstCommand->stCheckArg.lSocketOwnerID		= lSocketOwnerID;
	pstCommand->unType = unType;
	pstCommand->dpnid = dpnid;
	pstCommand->pvObject = pvObject;
	pstCommand->unDataSize = unDataSize;
	CopyMemory(pstCommand->szData, pData, unDataSize);

	return Dispatch(pstCommand);
}

// 2003.12.05. ±èÅÂÈñ - PVOID Çü Ãß°¡
BOOL ApIOCPDispatcher::Dispatch(PVOID pvBuffer, DWORD dwBytes)
{
	return PostStatus((ULONG_PTR)pvBuffer, dwBytes);
}