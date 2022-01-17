#include "AgsmBuddy.h"

#include "AgpmBuddy.h"
#include "AgsmCharacter.h"
#include "AgsmCharManager.h"
#include "AgpmCharacter.h"
#include "AgsdBuddy.h"
#include "ApmMap.h"
#include "AgsmPrivateTrade.h"
#include "AgsmParty.h"

#include <algorithm>

AgsmBuddy::AgsmBuddy()
{
	m_pcsAgpmBuddy		= NULL;
	m_pcsAgsmCharacter	= NULL;

	SetModuleName("AgsmBuddy");
	EnableIdle2(FALSE);
}

AgsmBuddy::~AgsmBuddy()
{
}

BOOL AgsmBuddy::OnAddModule()
{
	m_pcsAgpmBuddy = (AgpmBuddy*)GetModule("AgpmBuddy");
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmCharacter = (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmCharManager = (AgsmCharManager*)GetModule("AgsmCharManager");
	m_pcsApmMap = (ApmMap*)GetModule("ApmMap");
	m_pcsAgsmPrivateTrade = (AgsmPrivateTrade*)GetModule("AgsmPrivateTrade");
	m_pcsAgsmParty = (AgsmParty*)GetModule("AgsmParty");

	if (NULL == m_pcsAgpmBuddy || NULL == m_pcsAgsmCharacter || NULL == m_pcsAgsmCharManager ||
		NULL == m_pcsAgpmCharacter || NULL == m_pcsApmMap || NULL == m_pcsAgsmParty || NULL == m_pcsAgsmPrivateTrade)
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAdd(CBAddBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAddRequest(CBAddRequest, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackAddReject(CBAddReject, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackRemove(CBRemoveBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackOptions(CBOptionsBuddy, this))
		return FALSE;

	if (FALSE == m_pcsAgpmBuddy->SetCallbackRegion(CBRegion, this))
		return FALSE;

	// 클라이언트가 로긴서버에서 게임서버로 처음 접속할때 보내는 패킷
	// 이때 릴레이서버로 버디리스트를 요청한다.
	if (FALSE == m_pcsAgsmCharManager->SetCallbackConnectedChar(CBCharConnected, this))
		return FALSE;

	// 클라이언트가 맵을 로딩완료하고 게임 플레이 가능하다고 게임서버로 알리는 패킷
	if (FALSE == m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGameWorld, this))
		return FALSE;

	if (FALSE == m_pcsAgpmCharacter->SetCallbackRemoveChar(CBCharDisconnected, this))
		return FALSE;

	if (FALSE == m_pcsAgsmPrivateTrade->SetCallbackBuddyCheck(CBPrivateTradeCheck, this))
		return FALSE;

	if (FALSE == m_pcsAgsmParty->SetCallbackBuddyCheck(CBPartyCheck, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequest(CBMentorRequest, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequestAccept(CBMentorRequestAccept, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorRequestReject(CBMentorRequestReject, this))
		return FALSE;

	if(FALSE == m_pcsAgpmBuddy->SetCallbackMentorDelete(CBMentorDelete, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmBuddy::OnInit()
{
	return TRUE;
}

BOOL AgsmBuddy::OnDestroy()
{
	return TRUE;
}

BOOL AgsmBuddy::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmBuddy::CBCharConnected(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	// pCustData는 ulNID
	
	pThis->EnumCallback(AGSMBUDDY_DB_SELECT, pcsCharacter, NULL);

	return TRUE;
}

BOOL AgsmBuddy::CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	pThis->SendMyBuddyList(pcsCharacter);
	pThis->SendOnlineAllBuddy(pcsCharacter, TRUE);
	pThis->SendInitEnd(pcsCharacter);

	return TRUE;
}

BOOL AgsmBuddy::CBCharDisconnected(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	if (FALSE == pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBCharDisconnected"));

	pThis->SendOnlineAllBuddy(pcsCharacter, FALSE);

	return TRUE;
}

BOOL AgsmBuddy::CBPartyCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdCharacter* pcsTargetChar = (AgpdCharacter*)pCustData;

	return pThis->PartyCheck(pcsCharacter, pcsTargetChar);
}

BOOL AgsmBuddy::CBPrivateTradeCheck(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdCharacter* pcsTargetChar = (AgpdCharacter*)pCustData;

	return pThis->PrivateTradeCheck(pcsCharacter, pcsTargetChar);
}

BOOL AgsmBuddy::CBAddBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBAddRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddRequest(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBAddReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationAddReject(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBRemoveBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationRemoveBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBOptionsBuddy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationOptionsBuddy(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBRegion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy *pThis = (AgsmBuddy*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyRegion *pBuddyRegion = (AgpdBuddyRegion*)pCustData;
	return pThis->OperationRegion(pcsCharacter, pBuddyRegion);
}

BOOL AgsmBuddy::CBMentorRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequest(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorDelete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorDelete(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorRequestAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequestAccept(pcsCharacter, pcsBuddyElement);
}

BOOL AgsmBuddy::CBMentorRequestReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData || NULL == pCustData)
		return FALSE;

	AgsmBuddy* pThis = (AgsmBuddy*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdBuddyElement* pcsBuddyElement = (AgpdBuddyElement*)pCustData;

	return pThis->OperationMentorRequestReject(pcsCharacter, pcsBuddyElement);
}


// pcsCharacter 는 신청을 받은 사람이다.
// Yes 를 눌르면 아래 오퍼레이션이 온다.
BOOL AgsmBuddy::OperationAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// 신청받은 사람은 상대방이 이미 친구목록에 있을 수 있다.
	// 그에 대해서 다르게 처리 해야 한다.
	EnumAgpmBuddyMsgCode eMsgCode = IsValidBuddy(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	if(pcsBuddyElement->IsFriend())
	{
		// 신청한 쪽 처리
		AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
		if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
			return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST);

		AuAutoLock csLock(pcsTarget->m_Mutex);

		AgpdBuddyElement BuddyElement;
		BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

		EnumAgpmBuddyMsgCode eMsgCode2 = IsValidBuddy(pcsTarget, &BuddyElement);
		
		if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode2)
		{
			SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);	// 신청을 받은 캐릭터에게는 그냥 불가능 으로 보내고
			SendMsgCodeBuddy(pcsTarget, &BuddyElement, eMsgCode2);								// 신청을 했던 캐릭터에게는 제대로 보낸다.
			return TRUE;
		}

		AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTarget);
		if(!pcsBuddyADChar)
			return FALSE;

		if(pcsBuddyADChar->m_szLastRequestBuddy.CompareNoCase(pcsCharacter->m_szID) != COMPARE_EQUAL)
		{
			SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);	// 그냥 잘못된 오퍼레이션으로 보낸다.
			SendMsgCodeBuddy(pcsTarget, &BuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);		// 그냥 잘못된 오퍼레이션으로 보낸다.
			return TRUE;
		}

		// 신청한 쪽 처리
		if (m_pcsAgpmBuddy->AddBuddy(pcsTarget, &BuddyElement))
		{
			// DB insert
			EnumCallback(AGSMBUDDY_DB_INSERT, pcsTarget, &BuddyElement);

			// on-line 여부 확인을 IsValidBuddy에서 했으므로 무조건 온라인으로 설정
			BuddyElement.SetOnline(TRUE);

			// Client에게 통보
			SendAddBuddy(pcsTarget, &BuddyElement);
		}

		// 신청받은 쪽 처리.
		if (AGPMBUDDY_MSG_CODE_ALREADY_EXIST == eMsgCode)
		{
			// 이미 등록되어 있는 상대. 온라인으로만 해주면 된다.
			pcsBuddyElement->SetOnline(TRUE);
			SendBuddyOnlineToMe(pcsCharacter, pcsBuddyElement);
		}
		else if(m_pcsAgpmBuddy->AddBuddy(pcsCharacter, pcsBuddyElement))
		{
			// 새로 추가해준다.

			// DB insert
			EnumCallback(AGSMBUDDY_DB_INSERT, pcsCharacter, pcsBuddyElement);

			// on-line 여부 확인을 IsValidBuddy에서 했으므로 무조건 온라인으로 설정
			pcsBuddyElement->SetOnline(TRUE);

			// Client에게 통보
			SendAddBuddy(pcsCharacter, pcsBuddyElement);
		}
	}
	else
	{
		// 거부 리스트 등록
		//
		//

		// 친구 추가
		if (FALSE == m_pcsAgpmBuddy->AddBuddy(pcsCharacter, pcsBuddyElement))
			return FALSE;

		// DB insert
		EnumCallback(AGSMBUDDY_DB_INSERT, pcsCharacter, pcsBuddyElement);

		// on-line 여부 확인을 IsValidBuddy에서 했으므로 무조건 온라인으로 설정
		pcsBuddyElement->SetOnline(TRUE);

		// Client에게 통보
		SendAddBuddy(pcsCharacter, pcsBuddyElement);
	}

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::OperationAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	EnumAgpmBuddyMsgCode eMsgCode = IsValidBuddy(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	// 상대방에게 전달.
	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

	// 상대방도 체크
	EnumAgpmBuddyMsgCode eMsgCode2 = IsValidBuddy(pcsTarget, &BuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode2)
	{
		SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_USER);
		return TRUE;
	}

	// 신청한 사람을 저장한다.
	AgpdBuddyADChar* pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	pcsBuddyADChar->m_szLastRequestBuddy.SetText(pcsBuddyElement->m_szName);

	SendAddRequest(pcsTarget, &BuddyElement);

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::OperationAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	// 신청한 상대에게 전달
	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget || pcsTarget->m_bIsReadyRemove)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

	SendAddReject(pcsTarget, &BuddyElement);

	return TRUE;
}

BOOL AgsmBuddy::OperationRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(m_pcsAgpmBuddy->GetMentorStatus(pcsCharacter, pcsBuddyElement->m_szName) != AGSMBUDDY_MENTOR_NONE)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_DELETE_BECAUSE_MENTOR);

	// 친구 추가
	if (FALSE == m_pcsAgpmBuddy->RemoveBuddy(pcsCharacter, pcsBuddyElement))
		return FALSE;

	// DB delete
	EnumCallback(AGSMBUDDY_DB_REMOVE, pcsCharacter, pcsBuddyElement);

	// client에게 통보
	SendRemoveBuddy(pcsCharacter, pcsBuddyElement);

	if(pcsBuddyElement->IsFriend())
	{
		// 상대방도 지운다.
		AgpdBuddyElement BuddyElement;
		BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);

		AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
		if(pcsTarget)
		{
			AuAutoLock csLock(pcsTarget->m_Mutex);

			if(m_pcsAgpmBuddy->RemoveBuddy(pcsTarget, &BuddyElement))
			{
				EnumCallback(AGSMBUDDY_DB_REMOVE, pcsTarget, &BuddyElement);

				SendRemoveBuddy(pcsTarget, &BuddyElement);
			}
		}
		else
		{
			// 오프라인일 경우에는 DB 에서 직접 빼준다.
			EnumCallback(AGSMBUDDY_DB_REMOVE2, pcsBuddyElement->m_szName, &BuddyElement);
		}
	}

	return TRUE;
}

BOOL AgsmBuddy::OperationOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// 친구 추가
	if (FALSE == m_pcsAgpmBuddy->OptionsBuddy(pcsCharacter, pcsBuddyElement))
		return FALSE;

	// DB Update
	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, pcsBuddyElement);

	// client에게 통보
	SendOptionsBuddy(pcsCharacter, pcsBuddyElement);
	
	return TRUE;
}

BOOL AgsmBuddy::OperationRegion(AgpdCharacter* pcsCharacter, AgpdBuddyRegion* pcsBuddyRegion)
{
	AgpdCharacter *pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyRegion->m_BuddyElement.m_szName.GetBuffer());
	if (NULL == pcsTargetChar)
		return FALSE;

	INT16 nRegionIndex = m_pcsApmMap->GetRegion(pcsTargetChar->m_stPos.x, pcsTargetChar->m_stPos.z);
	if (-1 == nRegionIndex)
		return FALSE;

	return SendRegion(pcsCharacter, pcsTargetChar, (INT32)nRegionIndex);    
}

BOOL AgsmBuddy::OperationMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorRequest(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElement.SetMentorStatus(AGSMBUDDY_MENTOR_MENTOR);

	SendMentorRequest(pcsTarget, &BuddyElement);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorRequestAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement) //pcsCharacter가 pcsTarget의 Mentee가 된다.
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorAccept(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;

	AuAutoLock csLock(pcsTarget->m_Mutex);

	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_MENTEE);

	AgpdBuddyElement BuddyElementMentee;
	BuddyElementMentee.SetValues(pcsBuddyElement->GetOptions(), pcsBuddyElement->m_szName);
	BuddyElementMentee.SetMentorStatus(AGSMBUDDY_MENTOR_MENTOR);

	if(!(m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElementMentee) && m_pcsAgpmBuddy->MentorBuddy(pcsTarget, &BuddyElementMentor)))
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, &BuddyElementMentee);
	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsTarget, &BuddyElementMentor);

	//정상적으로 처리되었다고 클라에게 알린다.
	SendMentorRequestAccept(pcsCharacter, &BuddyElementMentee);
	SendMentorRequestAccept(pcsTarget, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorRequestReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(!pcsTarget)
		return FALSE;
	
	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_MENTEE);

	SendMentorRequestReject(pcsTarget, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::OperationMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	EnumAgpmBuddyMsgCode eMsgCode = IsValidMentorDelete(pcsCharacter, pcsBuddyElement);

	if (AGPMBUDDY_MSG_CODE_NONE != eMsgCode)
		return SendMsgCodeBuddy(pcsCharacter, pcsBuddyElement, eMsgCode);

	AgpdBuddyElement BuddyElementMentee;
	BuddyElementMentee.SetValues(pcsBuddyElement->GetOptions(), pcsBuddyElement->m_szName);
	BuddyElementMentee.SetMentorStatus(AGSMBUDDY_MENTOR_NONE);

	AgpdBuddyElement BuddyElementMentor;
	BuddyElementMentor.SetValues(pcsBuddyElement->GetOptions(), pcsCharacter->m_szID);
	BuddyElementMentor.SetMentorStatus(AGSMBUDDY_MENTOR_NONE);

	if(!m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElementMentee))
		return SendMsgCodeBuddy(pcsCharacter, &BuddyElementMentee, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

	EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsCharacter, &BuddyElementMentee);

	//정상적으로 처리되었다고 클라에게 알린다.
	SendMentorDelete(pcsCharacter, &BuddyElementMentee);

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if(pcsTarget)
	{
		AuAutoLock csLock(pcsTarget->m_Mutex);

		if(!m_pcsAgpmBuddy->MentorBuddy(pcsTarget, &BuddyElementMentor))
			return SendMsgCodeBuddy(pcsTarget, &BuddyElementMentor, AGPMBUDDY_MSG_CODE_INVALID_PROCESS);

		EnumCallback(AGSMBUDDY_DB_OPTIONS, pcsTarget, &BuddyElementMentor);

		SendMentorDelete(pcsTarget, &BuddyElementMentor);
	}
	else
		EnumCallback(AGSMBUDDY_DB_OPTIONS2, pcsBuddyElement->m_szName, &BuddyElementMentor);

	return TRUE;
}

BOOL AgsmBuddy::SetCallbackDBInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_INSERT, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_REMOVE, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBRemove2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_REMOVE2, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_OPTIONS, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBOptions2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_OPTIONS2, pfCallback, pClass);
}

BOOL AgsmBuddy::SetCallbackDBSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBUDDY_DB_SELECT, pfCallback, pClass);
}

BOOL AgsmBuddy::SendAddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddBuddy(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendAddRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddRequest(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendAddReject(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketAddReject(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendRemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRemoveBuddy(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendOptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketUpdateOptions(&nPacketLength, pcsCharacter, pcsBuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMsgCodeBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);
	ASSERT(AGPMBUDDY_MSG_CODE_NONE != eMsgCode);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMsgCode(&nPacketLength, pcsCharacter, pcsBuddyElement, eMsgCode);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendOnlineAllUser(AgpdCharacter* pcsCharacter, BOOL bOnline)
{
	ASSERT(NULL != pcsCharacter);

	if (!m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	if (m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsCharacter->m_szID);
	BuddyElement.SetOnline(bOnline);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, &BuddyElement);

    if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacketAllUser(pvPacket, nPacketLength);
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmBuddy::SendOnlineAllBuddy(AgpdCharacter* pcsCharacter, BOOL bOnline)
{
	ASSERT(NULL != pcsCharacter);

	if (!m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return FALSE;

	if (m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter) == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsCharacter->m_szID);
	BuddyElement.SetOnline(bOnline);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, &BuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(iter->m_szName);

		if(pcsTarget)
			SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsTarget->m_lID));
	}

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtBan.begin(); iter != pcsBuddyADChar->m_vtBan.end(); ++iter)
	{
		AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(iter->m_szName);

		if(pcsTarget)
			SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsTarget->m_lID));
	}

	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

// 2007.07.12. steeple
BOOL AgsmBuddy::SendBuddyOnlineToMe(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	if(!pcsCharacter || !pcsBuddyElement)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketOnline(&nPacketLength, pcsCharacter, pcsBuddyElement);
	if(!pvPacket || nPacketLength < 1)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendRegion(AgpdCharacter* pcsCharacter, AgpdCharacter* pcsTargetChar, INT32 lRegionIndex)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsTargetChar);

	INT16 nPacketLength = 0;
	AgpdBuddyElement BuddyElement;
	BuddyElement.m_szName.SetText(pcsTargetChar->m_szID);

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketRegion(&nPacketLength, pcsCharacter, &BuddyElement, lRegionIndex);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendInitEnd(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	INT16 nPacketLength = 0;

	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketInitEnd(&nPacketLength, pcsCharacter);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequest(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequest(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorDelete(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorDelete(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequestAccept(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequestAccept(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorRequestReject(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorRequestReject(&nPacketLength, pcsCharacter, pcsBuddyElement);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMentorUIOpen(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmBuddy->MakePacketMentorUIOpen(&nPacketLength, pcsCharacter);

	if (NULL == pvPacket)
	{
		m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmBuddy->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmBuddy::SendMyBuddyList(AgpdCharacter* pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		// online 여부 확인
		if (NULL == m_pcsAgpmCharacter->GetCharacter(iter->m_szName))
			iter->SetOnline(FALSE);
		else 
			iter->SetOnline(TRUE);

		SendAddBuddy(pcsCharacter, iter);
	}

	for (ApVectorBuddy::iterator iter = pcsBuddyADChar->m_vtBan.begin(); iter != pcsBuddyADChar->m_vtBan.end(); ++iter)
	{
		// online 여부 확인
		if (NULL == m_pcsAgpmCharacter->GetCharacter(iter->m_szName))
			iter->SetOnline(FALSE);
		else 
			iter->SetOnline(TRUE);

		SendAddBuddy(pcsCharacter, iter);
	}

	return TRUE;
}

// Relay Server로 부터 들어온 버디 리스트를 초기화한다.
BOOL AgsmBuddy::ReceiveBuddyDataFromRelay(AgpdCharacter* pcsCharacter, stBuddyRowset *pRowset)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pRowset);

	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);
	
	if (NULL == pcsBuddyADChar)
		return FALSE;

	for (UINT32 ul = 0; ul < pRowset->m_ulRows; ++ul)
	{
		AgpdBuddyElement BuddyElement;
		UINT32 ulCol = 0;
		CHAR *psz = NULL;
		
		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// char id
			continue;

		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// buddy id
			continue;

		BuddyElement.m_szName.SetText(psz);

		if (NULL == (psz = pRowset->Get(ul, ulCol++)))		// Options
			continue;

		BuddyElement.SetOptions(atoi(psz));

		if(NULL == (psz = pRowset->Get(ul, ulCol++)))		// MentorStatus
			continue;

		BuddyElement.SetMentorStatus(atoi(psz));

		if (BuddyElement.IsFriend())
			pcsBuddyADChar->m_vtFriend.push_back(BuddyElement);
		else
			pcsBuddyADChar->m_vtBan.push_back(BuddyElement);

		m_pcsAgpmBuddy->MentorBuddy(pcsCharacter, &BuddyElement);
	}

	return TRUE;
}

EnumAgpmBuddyMsgCode AgsmBuddy::IsValidBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	// 자기 자신은 등록할 수 없다.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// 이미 등록된 캐릭터 인가 확인
	if (TRUE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName.GetBuffer()))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST;

	// 현재 온라인 중인 캐릭터 인가 확인
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
    if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// PC인지 확인
	if (FALSE == m_pcsAgpmCharacter->IsPC(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// 아래 확인은 친구 초대 상태일때만 한다. 2007.07.25. steeple
	if(pcsBuddyElement->IsFriend())
	{
		// 거절 상태인지 확인. 2007.07.12. steeple
		if (pcsBuddyChar->m_lOptionFlag & AGPDCHAR_OPTION_REFUSE_BUDDY)
			return AGPMBUDDY_MSG_CODE_REFUSE;
	}

	return AGPMBUDDY_MSG_CODE_NONE;
}

//pcsCharacter가 Mentor
EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorRequest(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// 자기 자신은 등록할 수 없다.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// 친구목록에 있어야한다.
	if(FALSE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName, TRUE, FALSE))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// Mentor를 신청한자(pcsCharacter)가 다른 이의 Mentee일 경우에 Mentor가 될 수 없다.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE;

	// Mentor가 되기 부적합한 레벨.
	if(TRUE == m_pcsAgpmBuddy->UnderMentorLevel(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL;

	// pcsCharacter가 가진 Mentee 숫자가 한계 이상일 경우.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeNumber(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER;

	// 현재 온라인 중인 캐릭터 인가 확인
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// 요청받은 캐릭이 이미 다른 캐릭의 Mentee일 때.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR;

	// 요청받은 캐릭이 이미 다른 캐릭의 Mentor일 때.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentor(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR;

	// Mentee가 되기 부적합한 레벨.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeLevel(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL;

	return AGPMBUDDY_MSG_CODE_NONE;
}

//pcsCharacter가 Mentee가 되는거임.
EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorAccept(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// 자기 자신은 등록할 수 없다.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// 친구목록에 있어야한다.
	if(FALSE == m_pcsAgpmBuddy->IsExistBuddy(pcsCharacter, pcsBuddyElement->m_szName, TRUE, FALSE))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	// 자신이 이미 다른 캐릭의 Mentee일 때.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR;

	// 자신이 이미 다른 캐릭의 Mentor일 때.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentor(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR;

	// Mentee가 되기 부적합한 레벨.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeLevel(pcsCharacter))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL;

	// 현재 온라인 중인 캐릭터 인가 확인
	AgpdCharacter *pcsBuddyChar = m_pcsAgpmCharacter->GetCharacter(pcsBuddyElement->m_szName);
	if (NULL == pcsBuddyChar)
		return AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST;

	// Mentor예정자가 다른 이의 Mentee일 경우에 Mentor가 될 수 없다.
	if(TRUE == m_pcsAgpmBuddy->IsAlreadyMentee(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE;

	// Mentor가 되기 부적합한 레벨.
	if(TRUE == m_pcsAgpmBuddy->UnderMentorLevel(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL;

	// Mentor예정자가 가진 Mentee 숫자가 한계 이상일 경우.
	if(TRUE == m_pcsAgpmBuddy->OverMenteeNumber(pcsBuddyChar))
		return AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER;

	return AGPMBUDDY_MSG_CODE_NONE;
}

EnumAgpmBuddyMsgCode AgsmBuddy::IsValidMentorDelete(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	// 자기 자신에게 삭제할 수 없다.
	if (COMPARE_EQUAL == pcsBuddyElement->m_szName.CompareNoCase(pcsCharacter->m_szID))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	if(AGSMBUDDY_MENTOR_NONE == m_pcsAgpmBuddy->GetMentorStatus(pcsCharacter, pcsBuddyElement->m_szName))
		return AGPMBUDDY_MSG_CODE_INVALID_USER;

	return AGPMBUDDY_MSG_CODE_NONE;
}

BOOL AgsmBuddy::PartyCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar)
{
	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTargetChar);
	ASSERT(NULL != pcsBuddyADChar);

	ApString<AGPDCHARACTER_NAME_LENGTH> strCharName(pcsCharacter->m_szID);

	ApVectorBuddy::iterator iter = std::find(pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), strCharName.GetBuffer());
	if (iter == pcsBuddyADChar->m_vtBan.end())
		return TRUE;

	return !(iter->IsBlockInvitation());
}

BOOL AgsmBuddy::PrivateTradeCheck(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetChar)
{
	AgpdBuddyADChar *pcsBuddyADChar = m_pcsAgpmBuddy->GetAttachAgpdBuddyData(pcsTargetChar);
	ASSERT(NULL != pcsBuddyADChar);

	ApString<AGPDCHARACTER_NAME_LENGTH> strCharName(pcsCharacter->m_szID);

	ApVectorBuddy::iterator iter = std::find(pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), strCharName.GetBuffer());
	if (iter == pcsBuddyADChar->m_vtBan.end())
		return TRUE;

	return !(iter->IsBlockTrade());
}