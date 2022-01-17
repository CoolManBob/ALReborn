// AgcmCasper.cpp: implementation of the AgcmCasper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "casper.h"
#include "AgcmCasper.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMainFrame *	g_pMainFrame;

INT32	g_alMinActionTime[AGCMCASPER_MAX_ACTION_TYPE] =
{
	0,
	0,
	0,
	10000,
	0
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmCasper::AgcmCasper()
{
	SetModuleName("AgcmCasper");

	m_lMaxUsers		=	AGCM_MAX_USER;

	memset(m_astUsers, 0, sizeof(AgcdCasperUser) * AGCM_MAX_USER);
}

AgcmCasper::~AgcmCasper()
{

}

BOOL	AgcmCasper::OnAddModule()
{
	m_pcsApmMap		=	(ApmMap *)		GetModule("ApmMap")		;
	if (!m_pcsApmMap)
		return FALSE;

	m_pcsAgpmCharacter		=	(AgpmCharacter *)		GetModule("AgpmCharacter")		;
	if (!m_pcsAgpmCharacter)
		return FALSE;

	m_pcsAgpmItem			=	(AgpmItem *)			GetModule("AgpmItem")		;
	if (!m_pcsAgpmItem)
		return FALSE;

	m_pcsAgpmCasper			=	(AgpmCasper *)			GetModule("AgpmCasper")			;
	if (!m_pcsAgpmCasper)
		return FALSE;

	//m_pcsAgpmStartupEncryption = (AgpmStartupEncryption*)GetModule("AgpmStartupEncryption");
	//if (!m_pcsAgpmStartupEncryption)
	//	return FALSE;

	m_pcsAgpmOptimizedPacket2	= (AgpmOptimizedPacket2 *)	GetModule("AgpmOptimizedPacket2");
	if (!m_pcsAgpmOptimizedPacket2)
		return FALSE;

	m_pcsAgcmConnectManager	=	(AgcmConnectManager *)	GetModule("AgcmConnectManager")	;
	if (!m_pcsAgcmConnectManager)
		return FALSE;

	//m_pcsAgcmStartupEncryption = (AgcmStartupEncryption*)GetModule("AgpmStartupEncryption");
	//if (!m_pcsAgcmStartupEncryption)
	//	return FALSE;

	if (!m_pcsAgpmCasper->SetCallbackConnectionSuccess(CBInitSuccess, this))
		return FALSE;

	if (!m_pcsAgpmCasper->SetCallbackConnectionFail(CBInitFailure, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->AddPreReceiveModule(this))
		return FALSE;

	if (!m_pcsAgpmItem->AddPreReceiveModule(this))
		return FALSE;

	return TRUE;
}

BOOL	AgcmCasper::OnInit()
{
	srand(time(NULL));

	return TRUE;
}

BOOL	AgcmCasper::OnDestroy()
{
	INT32	lIndex;

	for (lIndex = 0; lIndex < m_lMaxUsers; ++lIndex)
	{
		if (m_astUsers[lIndex].m_lCID)
			m_astUsers[lIndex].m_pcsSocket->Close();
	}

	return TRUE;
}

BOOL	AgcmCasper::OnIdle(UINT32	ulClockCount)
{
	return TRUE;
}

BOOL	AgcmCasper::OnPreReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	PROFILE("AgpmItem::OnReceive");

	INT8		cOperation = -1;
	INT32		lCID;
	INT32		lIID;

	if (ulType == AGPMCHARACTER_PACKET_TYPE)
	{
		m_pcsAgpmCharacter->m_csPacket.GetField(TRUE, pvPacket, nSize, 
							&cOperation, 
							&lCID,
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL, 
							NULL);

		switch (cOperation)
		{
//		case AGPMCHAR_PACKET_OPERATION_REMOVE:
		case AGPMCHAR_PACKET_OPERATION_REMOVE_FOR_VIEW:
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	if (ulType == AGPMITEM_PACKET_TYPE)
	{
		return FALSE;

		m_pcsAgpmItem->m_csPacket.GetField(TRUE, pvPacket, nSize,  
							&cOperation,
							NULL,
							&lIID,
							&lCID,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL);

		switch(cOperation)
		{
		case AGPMITEM_PACKET_OPERATION_REMOVE:
			{
				AgpdItem *	pcsItem = m_pcsAgpmItem->GetItem(lIID);

				if (pcsItem && pcsItem->m_pcsCharacter && !strncmp(pcsItem->m_pcsCharacter->m_szID, "Casper", strlen("Casper")))
					return FALSE;

				AgpdCharacter *	pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

				if (pcsCharacter && !strncmp(pcsCharacter->m_szID, "Casper", strlen("Casper")))
					return FALSE;
			}
		}

		return TRUE;
	}

	return TRUE;
}

BOOL	AgcmCasper::SetMaxUsers(INT32 lMaxUsers)
{
	if (lMaxUsers > AGCM_MAX_USER)
		return FALSE;

	m_lMaxUsers = lMaxUsers;

	g_pEngine->SetMaxConnection(lMaxUsers, 1024000);		// Client Socket 갯수 설정

	return TRUE;
}

BOOL	AgcmCasper::ConnectDirectToGameServer()
{
	INT32	lIndex;
	CHAR	szServer[64];
	UINT32	ulNID;

	sprintf(szServer, "%s:%d", m_szServerIP, m_ulServerPort);

	for (lIndex = 0; lIndex < m_lMaxUsers; ++lIndex)
	{
		if (AGCDCASPER_STATUS_INIT == m_astUsers[lIndex].m_eStatus)
		{
			m_astUsers[lIndex].m_eStatus = AGCDCASPER_STATUS_CONNECTING;

			ulNID = m_pcsAgcmConnectManager->Connect(
						szServer ,
						ACDP_SERVER_TYPE_GAMESERVER ,
						this,
						CBSocketOnConnect,
						CBSocketOnDisConnect,
						CBSocketOnError );

			if (ulNID < 0)
			{
				continue;
			}

			Sleep(10);
		}
	}

	return TRUE;
}

INT32	AgcmCasper::GetEmptyIndex()
{
	INT32	lIndex;

	for (lIndex = 0; lIndex < m_lMaxUsers; ++lIndex)
	{
		if (!m_astUsers[lIndex].m_lCID)
			return lIndex;
	}

	return -1;
}

BOOL	AgcmCasper::SetServerInfo(const CHAR *szIP, UINT32 ulPort)
{
	strcpy(m_szServerIP, szIP);
	m_ulServerPort = ulPort;

	return TRUE;
}

VOID	AgcmCasper::ActionAllUsers()
{
	static BOOL				bToggle		= TRUE;
	INT32					lIndex		;
	AgcdCasperActionType	eAction		;
	INT32					lTickCount	= GetTickCount();

	for (lIndex = 0; lIndex < m_lMaxUsers; ++lIndex)
	{
		if (m_astUsers[lIndex].m_lCID)
		{
			m_astUsers[lIndex].m_pcsCharacter = m_pcsAgpmCharacter->GetCharacter(m_astUsers[lIndex].m_lCID);
			if (!m_astUsers[lIndex].m_pcsCharacter)
			{
				continue;
			}

			// 만약 현재 Action의 최소 시간이 안지났으면 그냥 넘긴다.
			if (lTickCount - m_astUsers[lIndex].m_lLastActionTick < g_alMinActionTime[m_astUsers[lIndex].m_eCurrentAction])
				eAction = m_astUsers[lIndex].m_eCurrentAction;
			else
			{
				eAction = (AgcdCasperActionType) (rand() % AGCMCASPER_MAX_ACTION_TYPE);

				m_astUsers[lIndex].m_eCurrentAction		= eAction;
				m_astUsers[lIndex].m_lLastActionTick	= lTickCount;
			}

			// Action 하기 전에 각자에 맞는 Server Socket을 사용해야 한다.
			g_pEngine->SetGameServerID(m_astUsers[lIndex].m_pcsSocket->GetIndex());

			switch (eAction)
			{
			case AGCMCASPER_ACTION_TYPE_MOVE:
				MoveCharacter(m_astUsers + lIndex);
				break;

			/*
			case AGCMCASPER_ACTION_TYPE_EQUIP:
				EquipItem(m_astUsers + lIndex);
				break;
			*/

			case AGCMCASPER_ACTION_TYPE_EQUIP:

			case AGCMCASPER_ACTION_TYPE_ATTACK:
			case AGCMCASPER_ACTION_TYPE_LOGOUT:
				AttackCharacter(m_astUsers + lIndex);
				break;

				/*
			case AGCMCASPER_ACTION_TYPE_LOGOUT:
				LogoutCharacter(m_astUsers + lIndex);
				break;
				*/
			}
		}
	}

	ConnectDirectToGameServer();

	bToggle = !bToggle;
}

BOOL	AgcmCasper::MoveCharacter(AgcdCasperUser *	pstUser)
{
	PVOID		pvPacket	;
	PVOID		pvPacketMove;
	INT16		nSize		;
	INT8		cOperation	= AGPMCHAR_PACKET_OPERATION_UPDATE;
	INT8		cMoveFlag	;
	AuPOS		stTarget	;

	stTarget = pstUser->m_stSpawnPosition;

	stTarget.x += (rand() % 2000) - 1000;
	stTarget.z += (rand() % 2000) - 1000;

	cMoveFlag = AGPMCHARACTER_MOVE_FLAG_FAST;

	pvPacket	= m_pcsAgpmOptimizedPacket2->MakePacketCharMove(pstUser->m_lCID, cMoveFlag, MD_NODIRECTION, &stTarget, &nSize);

	if (!pvPacket) return FALSE;

	if (!SendPacket(pvPacket, nSize))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL	AgcmCasper::EquipItem(AgcdCasperUser *	pstUser)
{
	AgpdItem *			pcsItem				;
	AgpdItemTemplate *	pcsTemplate			= m_pcsAgpmItem->GetItemTemplate("단검");
	PVOID				pvPacket			;
	PVOID				pvEquipPacket		= NULL;
	PVOID				pvInventoryPacket	= NULL;
	INT16				nSize				;
	INT8				cOperation			= AGPMITEM_PACKET_OPERATION_UPDATE;
 	INT8				cStatus				= AGPDITEM_STATUS_EQUIP;

	pcsItem = m_pcsAgpmItem->GetEquipWeapon(pstUser->m_pcsCharacter);
	if (pcsItem)
	{
		cStatus = AGPDITEM_STATUS_INVENTORY;

		pvInventoryPacket = m_pcsAgpmItem->m_csPacketInventory.MakePacket(FALSE, &nSize, AGPMITEM_PACKET_TYPE, 
																		  NULL, // inventory tab
																		  NULL, // inventory row
																		  NULL  // inventory column
																		  );
	}
	else
	{
		pcsItem = m_pcsAgpmItem->GetInventoryItemByTID(pstUser->m_pcsCharacter, pcsTemplate->m_lID);
		if (pcsItem)
		{
			cStatus = AGPDITEM_STATUS_EQUIP;

			pvEquipPacket = m_pcsAgpmItem->m_csPacketEquip.MakePacket(FALSE, &nSize, AGPMITEM_PACKET_TYPE);
		}
		else
			return FALSE;
	}

	pvPacket = m_pcsAgpmItem->m_csPacket.MakePacket(TRUE, &nSize, AGPMITEM_PACKET_TYPE, 
		                                           &cOperation,                                        // operation
												   &cStatus,											// status
												   &pcsItem->m_lID,												// item i.d.
												    NULL,												// item template i.d.
													&pstUser->m_lCID,												// Item Owner ID
													NULL,												// Item Count
													NULL,												// field(packet)
													pvInventoryPacket,									// inventory(packet)
													NULL,												// bank
													pvEquipPacket,										// equip(packet)
													NULL,												// factor
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,
													NULL,												// Skill Plus
													NULL,
													NULL
													);

	if (pvEquipPacket)
		m_pcsAgpmItem->m_csPacket.FreePacket(pvEquipPacket);

	if (pvInventoryPacket)
		m_pcsAgpmItem->m_csPacket.FreePacket(pvInventoryPacket);

	if(!SendPacket(pvPacket, nSize))
	{
		m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL	AgcmCasper::AttackCharacter(AgcdCasperUser *	pstUser)
{

	if (!pstUser->m_lTargetCID || !m_pcsAgpmCharacter->GetCharacter(pstUser->m_lTargetCID))
	{
		INT32	alCID[100];
		INT32	alCID2[100];
		INT32	lTarget;
		INT32	lMonsters;

		lMonsters = m_pcsApmMap->GetNPCList(pstUser->m_pcsCharacter->m_nDimension, pstUser->m_pcsCharacter->m_stPos, 2000, alCID, 100, alCID2, 100);
		if (!lMonsters)
		{
			pstUser->m_eCurrentAction = AGCMCASPER_ACTION_TYPE_NONE;
			return FALSE;
		}

		lTarget	= rand() % lMonsters;
		pstUser->m_lTargetCID = alCID2[lTarget];
	}

	PVOID		pvPacket		;
	PVOID		pvPacketAction	;
	INT16		nSize			;
	INT8		cOperation		= AGPMCHAR_PACKET_OPERATION_UPDATE;
	UINT8		nType			= AGPDCHAR_ACTION_TYPE_ATTACK;

	pvPacket	= m_pcsAgpmOptimizedPacket2->MakePacketCharAction(pstUser->m_lCID, pstUser->m_lTargetCID, FALSE, TRUE, 0, 0, &nSize);

	/*
	pvPacketAction = m_pcsAgpmCharacter->m_csPacketAction.MakePacket(FALSE, &nSize, AGPMCHARACTER_PACKET_TYPE,  
															&nType, 
															&pstUser->m_lTargetCID, 
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL);

	pvPacket = m_pcsAgpmCharacter->m_csPacket.MakePacket(TRUE, &nSize, AGPMCHARACTER_PACKET_TYPE,
														&cOperation,
														&pstUser->m_lCID,
														NULL,
														NULL,
														NULL,
														NULL,
														pvPacketAction,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL);

	m_pcsAgpmCharacter->m_csPacketAction.FreePacket(pvPacketAction);
	*/

	if (!SendPacket(pvPacket, nSize))
	{
		m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	m_pcsAgpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL	AgcmCasper::LogoutCharacter(AgcdCasperUser *	pstUser)
{
	if (pstUser->m_pcsSocket)
	{
		m_pcsAgcmConnectManager->Disconnect(pstUser->m_pcsSocket->GetIndex());
		memset(pstUser, 0, sizeof(AgcdCasperUser));
	}

	return TRUE;
}

BOOL	AgcmCasper::SendRequestInit(AgcdCasperUser *	pstUser)
{
	INT16		nPacketLength = 0;
	PVOID		pvPacket;

	pvPacket	=	m_pcsAgpmCasper->MakePacketRequestConnection(&nPacketLength);
	if (!pvPacket)
		return FALSE;

	g_pEngine->SetGameServerID(pstUser->m_pcsSocket->GetIndex());
	SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmCasper->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL	AgcmCasper::CBSocketOnConnect(PVOID pvData, PVOID pvThis, PVOID pvCustData)
{
	AgcmCasper *		pThis		=	(AgcmCasper *)	pvThis;
	AcClientSocket *	pcsSocket	=	(AcClientSocket *)	pvData;

	memset(pThis->m_astUsers + pcsSocket->GetIndex(), 0, sizeof(AgcdCasperUser));

	pThis->m_astUsers[pcsSocket->GetIndex()].m_pcsSocket	=	pcsSocket;
	pThis->m_astUsers[pcsSocket->GetIndex()].m_eStatus = AGCDCASPER_STATUS_CONNECTED;

	TRACE("----------- Socket Connected (%d)\n", pcsSocket->GetIndex());

//	g_pMainFrame->SetTimer(pcsSocket->GetIndex() + 1000, 500 * pcsSocket->GetIndex(), NULL);

	// 암호화를 한다.

	pThis->SendRequestInit(pThis->m_astUsers + pcsSocket->GetIndex());

	return TRUE;
}

BOOL	AgcmCasper::CBSocketOnDisConnect(PVOID pvData, PVOID pvThis, PVOID pvCustData)
{
	AgcmCasper *		pThis		=	(AgcmCasper *)	pvThis;
	AcClientSocket *	pcsSocket	=	(AcClientSocket *)	pvData;

	memset(pThis->m_astUsers + pcsSocket->GetIndex(), 0, sizeof(AgcdCasperUser));
	pThis->m_astUsers[pcsSocket->GetIndex()].m_eStatus = AGCDCASPER_STATUS_INIT;

	TRACE("----------- Socket Disconnected (%d)\n", pcsSocket->GetIndex());

	return TRUE;
}

BOOL	AgcmCasper::CBSocketOnError(PVOID pvData, PVOID pvThis, PVOID pvCustData)
{
	AgcmCasper *		pThis		=	(AgcmCasper *)	pvThis;
	AcClientSocket *	pcsSocket	=	(AcClientSocket *)	pvData;

	memset(pThis->m_astUsers + pcsSocket->GetIndex(), 0, sizeof(AgcdCasperUser));
	pThis->m_astUsers[pcsSocket->GetIndex()].m_eStatus = AGCDCASPER_STATUS_INIT;

	TRACE("----------- Socket Error (%d)\n", pcsSocket->GetIndex());

	return TRUE;
}

BOOL	AgcmCasper::CBInitSuccess(PVOID pvData, PVOID pvThis, PVOID pvCustData)
{
	AgcmCasper *		pThis		=	(AgcmCasper *)	pvThis;
	INT32 *				plCID		=	(INT32 *)	pvData;
	UINT32 *			pulNID		=	(UINT32 *)	pvCustData;

	pThis->m_astUsers[*pulNID].m_lCID = *plCID;
	if (!pThis->m_astUsers[*pulNID].m_lCID)
	{
		ASSERT(!"AgcmCasper::CBInitSuccess() Error getting character!!!");
		return FALSE;
	}

	pThis->m_astUsers[*pulNID].m_pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(pThis->m_astUsers[*pulNID].m_lCID);
	if (pThis->m_astUsers[*pulNID].m_pcsCharacter)
		pThis->m_astUsers[*pulNID].m_stSpawnPosition = pThis->m_astUsers[*pulNID].m_pcsCharacter->m_stPos;

	pThis->m_astUsers[*pulNID].m_eStatus = AGCDCASPER_STATUS_LOGIN;

	TRACE("=========== Succeeded Init (%d, %d)\n", *plCID, pThis->m_astUsers[*pulNID].m_lCID);

	return TRUE;
}

BOOL	AgcmCasper::CBInitFailure(PVOID pvData, PVOID pvThis, PVOID pvCustData)
{
	AgcmCasper *		pThis		=	(AgcmCasper *)	pvThis;
	UINT32 *			pulNID		=	(UINT32 *)	pvData;

	pThis->m_astUsers[*pulNID].m_lCID = 0;

	pThis->m_pcsAgcmConnectManager->Disconnect(*pulNID);
	pThis->m_astUsers[*pulNID].m_eStatus = AGCDCASPER_STATUS_INIT;

	TRACE("=========== Failed Init (%d)\n", *pulNID);

	return TRUE;
}
