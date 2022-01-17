/******************************************************************************
Module:  AgsmObject.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#if !defined(__AGSMOBJECT_H__)
#define __AGSMOBJECT_H__

#include "ApBase.h"

#include "ApmMap.h"
#include "ApmObject.h"

#include "AgsEngine.h"
#include "AgsmAOIFilter.h"
#include "AgsmCharacter.h"

#include "AgsdObject.h"

#include "AuPacket.h"

typedef enum _eAgsmObjectPacketOperation {
	AGSMOBJECT_OPERATION_UPDATE					= 0,
} eAgsmObjectPacketOperation;

class AgsmObject : public AgsModule {
private:
	ApmMap				*m_papmMap;
	ApmObject			*m_papmObject;

	AgsmAOIFilter		*m_pagsmAOIFilter;
	AgsmCharacter		*m_pagsmCharacter;

	INT16				m_nIndexADSector;

public:
	AuPacket			m_csPacket;

public:
	AgsmObject();
	~AgsmObject();

	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();
	BOOL				OnIdle(UINT32 ulClockCount);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL			CBUpdateStatus(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBSendSectorInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			ConAgsdObjectADSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgsdObjectADSector(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdObjectADSector*	GetADSector(PVOID pData);

	PVOID				MakePacketObjectStatus(ApdObject *pcsObject, INT16 *pnPacketLength);
};

#endif	//__AGSMOBJECT_H__
