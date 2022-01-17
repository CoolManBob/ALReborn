#include "AgpmBuddy.h"
#include <algorithm>
#include <AuXmlParser.h>

#include "AgpmCharacter.h"

AgpmBuddy::AgpmBuddy()
{
	SetModuleName("AgpmBuddy");
	SetPacketType(AGPMBUDDY_PACKET_TYPE);
	SetModuleData(sizeof(AgpdBuddyADChar), AGPMBUDDY_DATA_BUDDY);

	EnableIdle2(FALSE);

	m_csPacket.SetFlagLength(sizeof(INT32));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,	// Operation
							AUTYPE_INT32,			1,	// CID
							AUTYPE_CHAR,			AGPDCHARACTER_NAME_LENGTH + 1,	// Buddy Name
							AUTYPE_UINT32,			1,	// Options -> Online도 여기서 보낸다.
							AUTYPE_INT32,			1,	// Region Index
							AUTYPE_INT32,			1,	// Mentor Status
							AUTYPE_INT32,			1,	// message code
							AUTYPE_END,				0
							);

	m_pcsAgpmCharacter = NULL;
	m_lIndexAttachData = -1;
}

AgpmBuddy::~AgpmBuddy()
{
}

BOOL AgpmBuddy::OnAddModule()
{
	m_pcsAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");

	if (NULL == m_pcsAgpmCharacter)
		return FALSE;

	m_lIndexAttachData = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdBuddyADChar), AgpdBuddyConstructor, AgpdBuddyDestructor);
	if (m_lIndexAttachData < 0)
		return FALSE;

	return TRUE;
}

BOOL AgpmBuddy::OnInit()
{
	return TRUE;
}

BOOL AgpmBuddy::OnIdle2(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmBuddy::OnDestroy()
{
	return TRUE;
}

BOOL AgpmBuddy::AgpdBuddyConstructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(NULL == pClass || NULL == pData)
		return FALSE;

	AgpmBuddy *pThis = (AgpmBuddy *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgpdBuddyADChar *pcsBuddy = (AgpdBuddyADChar *)pThis->GetAttachAgpdBuddyData(pstAgpdCharacter);

#ifdef new
#undef new
#endif
	new (pcsBuddy) AgpdBuddyADChar;
#ifdef new
#undef new	
#define new DEBUG_NEW
#endif	

	pcsBuddy->m_nCurrentMenteeNumber = 0;

	return TRUE;
}

BOOL AgpmBuddy::AgpdBuddyDestructor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(NULL == pClass || NULL == pData)
		return FALSE;

	AgpmBuddy *pThis = (AgpmBuddy *) pClass;
	AgpdCharacter *pstAgpdCharacter = (AgpdCharacter *) pData;
	AgpdBuddyADChar *pcsBuddy = (AgpdBuddyADChar *)pThis->GetAttachAgpdBuddyData(pstAgpdCharacter);

	pcsBuddy->~AgpdBuddyADChar();

	return TRUE;
}

BOOL AgpmBuddy::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8 cOperation	= -1;
	INT32 lCID		= -1;
	CHAR* szName	= NULL;
	DWORD dwOptions = 0;
	INT32 lRegionIndex = -1;
	INT32 lMentorStatus = AGSMBUDDY_MENTOR_NONE;
	EnumAgpmBuddyMsgCode eMsgCode = AGPMBUDDY_MSG_CODE_NONE;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&lCID,
						&szName,
						&dwOptions,
						&lRegionIndex,
						&lMentorStatus,
						&eMsgCode);

	if (AGPMBUDDY_OPERATION_MAX <= cOperation) 
		return FALSE;

	if (AGPMBUDDY_OPERATION_MSG_CODE == cOperation)
	{
		EnumCallback(AGPMBUDDY_CALLBACK_MSG_CODE, NULL, &eMsgCode);
		return TRUE;
	}

	if (AGPMBUDDY_OPERATION_INIT_END == cOperation)
	{
		EnumCallback(AGPMBUDDY_CALLBACK_INIT_END, NULL, NULL);
		return TRUE;
	}


	AgpdBuddyElement BuddyElement;
	BuddyElement.SetValues(dwOptions, szName);
	BuddyElement.SetMentorStatus(lMentorStatus);

	if (AGPMBUDDY_OPERATION_ONLINE == cOperation)
	{
		// Online여부는 모든 사용자에게 전송되는 패킷이기 때문에 lCID를 가지지 않는다.
		// AgcmUIBuddy에서 GetSelfCharacter()를 기준으로 삼는다.
		EnumCallback(AGPMBUDDY_CALLBACK_ONLINE, NULL, &BuddyElement);
		return TRUE;
	}

	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (NULL == pcsCharacter)
		return FALSE;

	switch (cOperation)
	{
	case AGPMBUDDY_OPERATION_ADD:
		EnumCallback(AGPMBUDDY_CALLBACK_ADD, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPERATION_ADD_REQUEST:
		EnumCallback(AGPMBUDDY_CALLBACK_ADD_REQUEST, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPERATION_ADD_REJECT:
		EnumCallback(AGPMBUDDY_CALLBACK_ADD_REJECT, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPERATION_REMOVE:
		EnumCallback(AGPMBUDDY_CALLBACK_REMOVE, pcsCharacter, &BuddyElement);	break;

	case AGPMBUDDY_OPERATION_OPTIONS:
		EnumCallback(AGPMBUDDY_CALLBACK_OPTIONS, pcsCharacter, &BuddyElement);	break;

	case AGPMBUDDY_OPERATION_REGION:
		{
			AgpdBuddyRegion BuddyRegion(BuddyElement, lRegionIndex);
			EnumCallback(AGPMBUDDY_OPERATION_REGION, pcsCharacter, &BuddyRegion);	
		}
		break;
	case AGPMBUDDY_OPERATION_MENTOR_REQUEST:
		EnumCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPERATION_MENTOR_DELETE:
		EnumCallback(AGPMBUDDY_CALLBACK_MENTOR_DELETE, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPRRATION_MENTOR_REQUEST_ACCEPT:
		EnumCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST_ACCEPT, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPRRATION_MENTOR_REQUEST_REJECT:
		EnumCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST_REJECT, pcsCharacter, &BuddyElement);		break;

	case AGPMBUDDY_OPRRATION_MENTOR_UI_OPEN:
		EnumCallback(AGPMBUDDY_CALLBACK_MENTOR_UI_OPEN, pcsCharacter, &BuddyElement);		break;

	default:
		ASSERT(!"Invalid Buddy Operation");
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

AgpdBuddyADChar* AgpmBuddy::GetAttachAgpdBuddyData(AgpdCharacter *pcsCharacter)
{
	return (AgpdBuddyADChar*)m_pcsAgpmCharacter->GetAttachedModuleData(m_lIndexAttachData, (PVOID)pcsCharacter);
}

PVOID AgpmBuddy::MakePacketBuddy(INT16 *pnPacketLength, EnumAgpmBuddyOperation eOperation, INT32 lCID, 
								 AgpdBuddyElement* pcsBuddyElement, INT32 lRegionIndex, EnumAgpmBuddyMsgCode eMsgCode)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(AGPMBUDDY_OPERATION_NONE != eOperation);
	ASSERT(0 < lCID);

	INT8 cOperation = (INT8)eOperation;
	CHAR* szBuddyName = NULL;
	DWORD dwOptions = 0;
	INT32 lMentorStatus = AGSMBUDDY_MENTOR_NONE;

	if (pcsBuddyElement)
	{
		szBuddyName = pcsBuddyElement->m_szName;
		dwOptions = pcsBuddyElement->GetOptions();
		lMentorStatus = pcsBuddyElement->GetMentorStatus();
	}

	return m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMBUDDY_PACKET_TYPE,
								&cOperation,
								&lCID,
								szBuddyName,
								&dwOptions,
								&lRegionIndex,
								&lMentorStatus,
								&eMsgCode);
}

PVOID AgpmBuddy::MakePacketAddBuddy(INT16 *pnPacketLength, AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_ADD, pcsCharacter->m_lID, pcsBuddyElement, -1, AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketAddRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_ADD_REQUEST, pcsCharacter->m_lID, pcsBuddyElement, -1, AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketAddReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_ADD_REJECT, pcsCharacter->m_lID, pcsBuddyElement, -1, AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketRemoveBuddy(INT16 *pnPacketLength, AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_REMOVE, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketUpdateOptions(INT16 *pnPacketLength, AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_OPTIONS, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketOnline(INT16 *pnPacketLength, AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_ONLINE, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMsgCode(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, 
									AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(AGPMBUDDY_MSG_CODE_NONE != eMsgCode);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_MSG_CODE, pcsCharacter->m_lID, pcsBuddyElement, -1,eMsgCode);
}

PVOID AgpmBuddy::MakePacketRegion(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement, INT32 lRegionIndex)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_REGION, pcsCharacter->m_lID, pcsBuddyElement, lRegionIndex, AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketInitEnd(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_INIT_END, pcsCharacter->m_lID, NULL, 0, AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMentorRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_MENTOR_REQUEST, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMentorDelete(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPERATION_MENTOR_DELETE, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMentorRequestAccept(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPRRATION_MENTOR_REQUEST_ACCEPT, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMentorRequestReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPRRATION_MENTOR_REQUEST_REJECT, pcsCharacter->m_lID, pcsBuddyElement, -1,AGPMBUDDY_MSG_CODE_NONE);
}

PVOID AgpmBuddy::MakePacketMentorUIOpen(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pnPacketLength);
	ASSERT(NULL != pcsCharacter);

	return MakePacketBuddy(pnPacketLength, AGPMBUDDY_OPRRATION_MENTOR_UI_OPEN, pcsCharacter->m_lID, NULL, -1,AGPMBUDDY_MSG_CODE_NONE);
}

BOOL AgpmBuddy::SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_ADD, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackAddRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_ADD_REQUEST, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackAddReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_ADD_REJECT, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_REMOVE, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_OPTIONS, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackOnline(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_ONLINE, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MSG_CODE, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackRegion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_REGION, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackInitEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_INIT_END, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMentorRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMentorDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MENTOR_DELETE, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMentorRequestAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST_ACCEPT, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMentorRequestReject(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MENTOR_REQUEST_REJECT, pfCallback, pClass);
}

BOOL AgpmBuddy::SetCallbackMentorUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMBUDDY_CALLBACK_MENTOR_UI_OPEN, pfCallback, pClass);
}

BOOL AgpmBuddy::_AddBuddy(ApVectorBuddy& rVector, AgpdBuddyElement* pcsBuddyElement)
{
	if (rVector.full())
		return FALSE;

	rVector.push_back(*pcsBuddyElement);

	return TRUE;
}

BOOL AgpmBuddy::_RemoveBuddy(ApVectorBuddy& rVector, AgpdBuddyElement* pcsBuddyElement)
{
	if (rVector.empty())
		return FALSE;

	ApVectorBuddy::iterator iter = std::find(rVector.begin(), rVector.end(), pcsBuddyElement->m_szName.GetBuffer());

	if (iter == rVector.end())
		return FALSE;

	rVector.erase(iter);

	return TRUE;
}

BOOL AgpmBuddy::_OptionsBuddy(ApVectorBuddy& rVector, AgpdBuddyElement* pcsBuddyElement)
{
	if (rVector.empty())
		return FALSE;

	ApVectorBuddy::iterator iter = std::find(rVector.begin(), rVector.end(), pcsBuddyElement->m_szName.GetBuffer());

	if (iter == rVector.end())
		return FALSE;

	iter->SetOptions(pcsBuddyElement->GetOptions());

	return TRUE;
}

BOOL AgpmBuddy::_MentorBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement)
{
	if (rVector.empty())
		return FALSE;

	ApVectorBuddy::iterator iter = std::find(rVector.begin(), rVector.end(), pcsBuddyElement->m_szName.GetBuffer());

	if (iter == rVector.end())
		return FALSE;

	iter->SetMentorStatus(pcsBuddyElement->GetMentorStatus());

	return TRUE;
}

BOOL AgpmBuddy::AddBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	if (pcsBuddyElement->IsFriend())
		return _AddBuddy(pcsBuddyADChar->m_vtFriend, pcsBuddyElement);
	else
		return _AddBuddy(pcsBuddyADChar->m_vtBan, pcsBuddyElement);
}

BOOL AgpmBuddy::RemoveBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	if (pcsBuddyElement->IsFriend())
		return _RemoveBuddy(pcsBuddyADChar->m_vtFriend, pcsBuddyElement);
	else
		return _RemoveBuddy(pcsBuddyADChar->m_vtBan, pcsBuddyElement);
}

BOOL AgpmBuddy::OptionsBuddy(AgpdCharacter* pcsCharacter, AgpdBuddyElement* pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	if (pcsBuddyElement->IsFriend())
		return _OptionsBuddy(pcsBuddyADChar->m_vtFriend, pcsBuddyElement);
	else
		return _OptionsBuddy(pcsBuddyADChar->m_vtBan, pcsBuddyElement);
}

BOOL AgpmBuddy::MentorBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != pcsBuddyElement);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	INT32 m_nMentorStatus = GetMentorStatus(pcsCharacter, pcsBuddyElement->m_szName);

	BOOL bResult = _MentorBuddy(pcsBuddyADChar->m_vtFriend, pcsBuddyElement);

	if(bResult && pcsBuddyElement->GetMentorStatus() == AGSMBUDDY_MENTOR_MENTEE)
		pcsBuddyADChar->m_nCurrentMenteeNumber++;

	if(m_nMentorStatus == AGSMBUDDY_MENTOR_MENTEE && pcsBuddyElement->GetMentorStatus() == AGSMBUDDY_MENTOR_NONE)
		pcsBuddyADChar->m_nCurrentMenteeNumber--;

	return bResult;
}

BOOL AgpmBuddy::IsExistBuddy(AgpdCharacter *pcsCharacter, CHAR *szName, BOOL bCheckBuddy, BOOL bCheckBan)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != szName);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	ApVectorBuddy::iterator iter;

	if (bCheckBuddy)
	{
		iter = std::find(pcsBuddyADChar->m_vtFriend.begin(), pcsBuddyADChar->m_vtFriend.end(), szName);

		if (iter != pcsBuddyADChar->m_vtFriend.end())
			return TRUE;
	}

	if (bCheckBan)
	{
		iter = std::find(pcsBuddyADChar->m_vtBan.begin(), pcsBuddyADChar->m_vtBan.end(), szName);

		if (iter != pcsBuddyADChar->m_vtBan.end())
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmBuddy::IsAlreadyMentor(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	if(pcsBuddyADChar->m_nCurrentMenteeNumber > 0)
		return TRUE;

	return FALSE;
}

BOOL AgpmBuddy::IsAlreadyMentee(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	ApVectorBuddy::iterator iter;

	for(iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		if(iter->GetMentorStatus() == AGSMBUDDY_MENTOR_MENTOR)
			return TRUE;
	}

	return FALSE;
}

BOOL AgpmBuddy::OverMenteeNumber(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	if(pcsBuddyADChar->m_nCurrentMenteeNumber > m_stBuddyConstrict.m_nMaxMenteeNumber)
		return TRUE;

	return FALSE;
}

BOOL AgpmBuddy::UnderMentorLevel(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(m_pcsAgpmCharacter->GetLevel(pcsCharacter) < m_stBuddyConstrict.m_nLimitMentorLevel)
		return TRUE;

	return FALSE;
}

BOOL AgpmBuddy::OverMenteeLevel(AgpdCharacter *pcsCharacter)
{
	ASSERT(NULL != pcsCharacter);

	if(m_pcsAgpmCharacter->GetLevel(pcsCharacter) > m_stBuddyConstrict.m_nLimitMenteeLevel)
		return TRUE;

	return FALSE;
}

INT32 AgpmBuddy::GetMentorStatus(AgpdCharacter *pcsCharacter, CHAR *szName)
{
	ASSERT(NULL != pcsCharacter);
	ASSERT(NULL != szName);

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	ApVectorBuddy::iterator iter;

	iter = std::find(pcsBuddyADChar->m_vtFriend.begin(), pcsBuddyADChar->m_vtFriend.end(), szName);

	if (iter == pcsBuddyADChar->m_vtFriend.end())
		return AGSMBUDDY_MENTOR_NONE;
	else
		return iter->GetMentorStatus();

	return AGSMBUDDY_MENTOR_NONE;
}

INT32 AgpmBuddy::GetMentorCID(AgpdCharacter *pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	AgpdBuddyADChar* pcsBuddyADChar = GetAttachAgpdBuddyData(pcsCharacter);
	ASSERT(NULL != pcsBuddyADChar);

	ApVectorBuddy::iterator iter;

	for(iter = pcsBuddyADChar->m_vtFriend.begin(); iter != pcsBuddyADChar->m_vtFriend.end(); ++iter)
	{
		if(iter->GetMentorStatus() == AGSMBUDDY_MENTOR_MENTOR)
		{
			AgpdCharacter *pcsMentor = m_pcsAgpmCharacter->GetCharacter(iter->m_szName);
			if(!pcsMentor)
				return 0;

			return pcsMentor->GetID();
		}
	}

	return 0;
}

BOOL AgpmBuddy::ReadMentorConstrinctFile(CHAR* szFileName)
{
	if(NULL == szFileName)
		return FALSE;

	AuXmlDocument *pDoc = new AuXmlDocument;

	if(pDoc)
	{
		BOOL bLoad = pDoc->LoadFile(szFileName);
		if(bLoad)
		{
			AuXmlNode *pRootNode = pDoc->FirstChild("MentorConstrict");
			if(pRootNode)
			{
				AuXmlElement* pElemMaxMenteeNumber = pRootNode->FirstChildElement("MaxMenteeNumber");
				if(pElemMaxMenteeNumber)
					m_stBuddyConstrict.m_nMaxMenteeNumber =	atoi(pElemMaxMenteeNumber->GetText());

				AuXmlElement* pElemLimitMentorLevel = pRootNode->FirstChildElement("LimitMentorLevel");
				if(pElemLimitMentorLevel)
					m_stBuddyConstrict.m_nLimitMentorLevel =	atoi(pElemLimitMentorLevel->GetText());

				AuXmlElement* pElemLimitMenteeLevel = pRootNode->FirstChildElement("LimitMenteeLevel");
				if(pElemLimitMenteeLevel)
					m_stBuddyConstrict.m_nLimitMenteeLevel =	atoi(pElemLimitMenteeLevel->GetText());
			}
			else
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	if(pDoc)
	{
		delete pDoc;
	}

	return TRUE;
}