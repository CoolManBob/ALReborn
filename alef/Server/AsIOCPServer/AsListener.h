/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AsListener.h: interface for the AsListener class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASLISTENER_H__DA21B491_482B_4787_AEA7_714CD87CBBD0__INCLUDED_)
#define AFX_ASLISTENER_H__DA21B491_482B_4787_AEA7_714CD87CBBD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WINSOCK2.H>
#include <mswsock.h>
#include "ApDefine.h"
#include "AsObject.h"
#include "AsServerSocket.h"

class AsListener : public AsObject
{
private:
	SOCKET	m_hSocket;
	INT32	m_lBacklog;
	INT32	m_lPort;
	long	m_lActiveSocketCount;

	ApCriticalSection m_csCriticalSection;

	BOOL CreateListener(UINT16 lPort, INT32 lBacklog);

public:
	AsListener();
	virtual ~AsListener();

	UINT32 GetActiveSocketCount() {return m_lActiveSocketCount;}

	SOCKET GetHandle()	{return m_hSocket;}
	INT32 GetBacklog()	{return m_lBacklog;}
	INT32 GetPort()		{return m_lPort;}
	BOOL Initialize(UINT16 lPort, INT32 lBacklog = 30);
	BOOL AsyncAccept(AsServerSocket *pSocket);
	BOOL SetSocketOption(AsServerSocket *pSocket);
};

#endif // !defined(AFX_ASLISTENER_H__DA21B491_482B_4787_AEA7_714CD87CBBD0__INCLUDED_)
