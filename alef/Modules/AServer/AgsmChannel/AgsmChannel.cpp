#include <list>
#include <algorithm>

#include "AgsmChannel.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"
#include "AgsmGuild.h"
#include "AgpmGuild.h"

AgsmChannel::AgsmChannel()
{
	SetModuleName("AgsmChannel");
	EnableIdle2(TRUE);

	m_pcsAgpmChannel	= NULL;
	m_pcsAgpmCharacter	= NULL;
	m_pcsAgsmCharacter	= NULL;
	m_pcsAgsmGuild		= NULL;
	m_pcsAgpmGuild		= NULL;

	m_ulTickCount		= 0;
}

AgsmChannel::~AgsmChannel()
{
}

BOOL AgsmChannel::OnAddModule()
{
	m_pcsAgpmChannel	= (AgpmChannel*)GetModule("AgpmChannel");
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgsmCharacter	= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pcsAgsmGuild		= (AgsmGuild*)GetModule("AgsmGuild");
	m_pcsAgpmGuild		= (AgpmGuild*)GetModule("AgpmGuild");

	if (!m_pcsAgpmChannel || !m_pcsAgpmCharacter || !m_pcsAgsmCharacter || !m_pcsAgsmGuild || !m_pcsAgpmGuild)
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackCreateChannel(CBCreateChannel, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackDestroyChannel(CBDestroyChannel, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackFindUser(CBFindUser, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackJoin(CBJoinChannel, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackLeave(CBLeaveChannel, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackChatMsg(CBChatMsg, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackPassword(CBPassword, this))
		return FALSE;

	if (FALSE == m_pcsAgpmChannel->SetCallbackInvitation(CBInvitation, this))
		return FALSE;

	if (FALSE == m_pcsAgpmCharacter->SetCallbackRemoveChar(CBDisconnect, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmChannel::OnInit()
{
	return TRUE;
}

BOOL AgsmChannel::OnIdle2(UINT32 ulClockCount)
{
	// AGSDCHANNEL_DELAY 만큼의 여유를 가진다.
	if (ulClockCount < m_ulTickCount + AGSDCHANNEL_DELAY)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("OnIdle2"));

	INT32 lIndex = 0;
	for (AgpdChannel **ppChannel = (AgpdChannel**)m_pcsAgpmChannel->m_csAdminChannel.GetObjectSequence(&lIndex); ppChannel; 
			ppChannel = (AgpdChannel**)m_pcsAgpmChannel->m_csAdminChannel.GetObjectSequence(&lIndex))
	{
		if (1 == (*ppChannel)->GetCurrentUser())
		{
			if (ulClockCount > (*ppChannel)->GetTickCount() + AGSDCHANNEL_DELAY_DESTROY)
				DestroyChannel(*ppChannel);
		}
	}

	m_ulTickCount = ulClockCount;

	return TRUE;
}

BOOL AgsmChannel::OnDestroy()
{
	return TRUE;
}

BOOL AgsmChannel::CBCreateChannel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationCreateChannel(pcsCharacter, pParam);
}

BOOL AgsmChannel::CBDestroyChannel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmChannel::CBFindUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationFindUser(pcsCharacter, pParam);
}

BOOL AgsmChannel::CBLeaveChannel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationLeaveChannel(pcsCharacter, pParam->m_strName);
}

BOOL AgsmChannel::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmChannel* pThis = (AgsmChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBDisconnect"));

	return pThis->DisconnectCharacter(pcsCharacter);
}

BOOL AgsmChannel::CBPassword(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationPassword(pcsCharacter, pParam);
}

BOOL AgsmChannel::CBChatMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationChatMsg(pcsCharacter, pParam);
}

BOOL AgsmChannel::CBJoinChannel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationJoinChannel(pcsCharacter, pParam);
}

BOOL AgsmChannel::CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmChannel *pThis = (AgsmChannel*)pClass;
	AgpdCharacter *pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam* pParam = (AgpdChannelParam*)pCustData;

	return pThis->OperationInvitation(pcsCharacter, pParam);
}

BOOL AgsmChannel::IsValidCreateChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	// 채널 이름 확인
	if (pParam->m_strName.IsEmpty())
		return FALSE;

	// 같은 이름의 채널이 존재 하는지 확인
	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	if (NULL != m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName))
	{
		// 같은 방이름이 존재
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_ALREADY_EXIST);
		return FALSE;
	}

	// 현재 참여중인 대화방이 5개 미만인지 확인
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
		return FALSE;

	if (pChannelADChar->m_vtJoinRooms.full())
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_FULL_CHANNEL);
		return FALSE;
	}

	// 길드방을 만들때
	if (AGPDCHANNEL_TYPE_GUILD == pParam->m_eType)
	{
		// 길드방을 만들고 싶다면 길드 마스터인지 확인
		if (FALSE == m_pcsAgsmGuild->IsGuildMaster(pcsCharacter))
		{
			SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_GUILDMASTER);
			return FALSE;
		}

		// 길드방을 이미 만들어 놓았다면 취소
		if (TRUE == pChannelADChar->GetCreatedGuildChannel())
		{
			SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_ALREADY_GUILDCHANNEL);
			return FALSE;
		}
	}

	// 비밀방을 만들고 싶다면 패스워드가 설정되어 있는지 확인
	if (AGPDCHANNEL_TYPE_PRIVATE == pParam->m_eType)
	{
		if (pParam->m_strPassword.IsEmpty())
		{
			SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_INVALID_PASSWORD);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmChannel::IsValidJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	// 채널 이름 확인
	if (pParam->m_strName.IsEmpty())
		return FALSE;

	// 같은 이름의 채널이 존재 하는지 확인
	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel *pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	if (NULL == pChannel)
	{
		// 같은 방이름이 존재
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHANNEL);
		return FALSE;
	}

	// 현재 참여중인 대화방이 5개 미만인지 확인
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
		return FALSE;

	if (pChannelADChar->m_vtJoinRooms.full())
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_FULL_CHANNEL);
		return FALSE;
	}

	// 비밀방이면 패스워드를 입력 요청
	if (AGPDCHANNEL_TYPE_PRIVATE == pChannel->GetChannelType())
	{
		SendPacketPassword(pcsCharacter, pChannel->m_strName);
		return FALSE;
	}

	// 길드방에 Join하고 싶다면 길드원인지 확인
	if (AGPDCHANNEL_TYPE_GUILD == pChannel->GetChannelType())
	{
		AgpdGuildADChar* pGuild = m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
		if (NULL == pGuild)
			return FALSE;

		// 해당 길드원이 아닐경우
		if (COMPARE_EQUAL != pChannel->m_strGuildName.CompareNoCase(pGuild->m_szGuildID))
		{
			SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_GUILDMEMBER);
			return FALSE;
		}
	}

	// 채널에 유저가 가득찼는지 확인
	if (pChannel->IsFull())
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_FULL_USER);
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmChannel::IsValidInvitation(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	// 채널 이름 확인
	if (pParam->m_strName.IsEmpty())
		return FALSE;

	// 같은 이름의 채널이 존재 하는지 확인
	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel *pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	if (NULL == pChannel)
	{
		// 같은 방이름이 존재
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHANNEL);
		return FALSE;
	}

	// 현재 참여중인 대화방이 5개 미만인지 확인
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
		return FALSE;

	if (pChannelADChar->m_vtJoinRooms.full())
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_FULL_CHANNEL);
		return FALSE;
	}

	// 채널에 유저가 가득찼는지 확인
	if (pChannel->IsFull())
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_FULL_USER);
		return FALSE;
	}

	// 상대방 유저가 온라인 상태인지 확인
	if (NULL == m_pcsAgpmCharacter->GetCharacter(pParam->m_strUserName))
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHARACTER);
		return FALSE;
	}

	// 이미 채널에 참여중인지 확인
	if (pChannel->FindUser(pParam->m_strUserName))
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_ALREADY_JOIN);
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmChannel::SetCreatedGuildChannel(AgpdCharacter *pcsCharacter, BOOL bSet, ApStrGuildName &rStrGuildName)
{
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
		return FALSE;

	pChannelADChar->SetCreatedGuildChannel(bSet);

	AgpdGuild* pGuild = m_pcsAgpmGuild->FindGuildLockFromCharID(pcsCharacter->m_szID);
	if (NULL == pGuild)
		return FALSE;

	rStrGuildName.SetText(pGuild->m_szID);
	pGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmChannel::DisconnectCharacter(AgpdCharacter *pcsCharacter)
{
	// 내가 참여하고 있는 모든 채널에서 삭제한다.
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	ASSERT(NULL != pChannelADChar);
	if (NULL == pChannelADChar)
		return FALSE;

	for (int i = 0; i < pChannelADChar->m_vtJoinRooms.size(); ++i)
	{
		OperationLeaveChannel(pcsCharacter, pChannelADChar->m_vtJoinRooms[i]);
	}

	return TRUE;
}

BOOL AgsmChannel::DestroyChannel(AgpdChannel *pChannel)
{
	{
		// 채널에 있던 사람들을 모두다 내보낸다.
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (!lock.Result()) return FALSE;

		for (AgpdChannel::iterChannelUser iter = pChannel->m_UserList.begin(); iter != pChannel->m_UserList.end(); ++iter)
		{
			AgpdCharacter *pcsUser = m_pcsAgpmCharacter->GetCharacter(iter->GetBuffer());
			if (NULL == pcsUser)
				continue;

			// 내가 참여한 채널 정보에서 뺀다.
			AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsUser);
			ASSERT(NULL != pChannelADChar);
			if (pChannelADChar)
			{
				AgpdJoinChannelADChar::iterChannelName iter = std::find(pChannelADChar->m_vtJoinRooms.begin(), 
														pChannelADChar->m_vtJoinRooms.end(), pChannel->m_strName);

				if (iter != pChannelADChar->m_vtJoinRooms.end())
				{
					pChannelADChar->m_vtJoinRooms.erase(iter);
				}
			}

			SendPacketLeaveChannel(pcsUser, pChannel->m_strName);
		}
	}

	// admin에서 삭제
	ApStrChannelName strChannelName = pChannel->m_strName;
	strChannelName.MakeUpper();

	m_pcsAgpmChannel->m_csAdminChannel.Remove(strChannelName);

	// 채널 삭제
	return m_pcsAgpmChannel->DestroyChannelData(pChannel);
}

BOOL AgsmChannel::OperationCreateChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	// 채널을 만드는데 적합한지 확인
	if (FALSE == IsValidCreateChannel(pcsCharacter, pParam))
		return FALSE;

	// 채널 생성
	AgpdChannel *pChannel = m_pcsAgpmChannel->CreateChannelData();
	if (NULL == pChannel)
		return FALSE;
 
	pChannel->m_CriticalSection.Lock();

	pChannel->Initialize(pParam->m_eType);
	pChannel->m_strName = pParam->m_strName;
	pChannel->m_strPassword = pParam->m_strPassword;

	pParam->m_strName.MakeUpper();
	if (FALSE == m_pcsAgpmChannel->m_csAdminChannel.Add(pParam->m_strName, pChannel))
	{
		pChannel->m_CriticalSection.Unlock();
		m_pcsAgpmChannel->DestroyChannelData(pChannel);
		return FALSE;
	}

	// 채널이 생성 완료 되었으므로 나를 채널에 참여시킨다.
	if (FALSE == pChannel->AddUser(pcsCharacter))
	{
		pChannel->m_CriticalSection.Unlock();
		m_pcsAgpmChannel->DestroyChannelData(pChannel);
		return FALSE;
	}

	// AgpdJoinChannelADChar에 채널을 추가한다.
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
	{
		pChannel->m_CriticalSection.Unlock();
		m_pcsAgpmChannel->DestroyChannelData(pChannel);
		return FALSE;
	}

	// 더이상 추가 할수 없다면
	if (false == pChannelADChar->m_vtJoinRooms.push_back(pChannel->m_strName))
	{
		pChannel->m_CriticalSection.Unlock();
		m_pcsAgpmChannel->DestroyChannelData(pChannel);
		return FALSE;
	}

	// 길드 채널을 만들경우 캐릭터에게도 정보를 저장
	if (AGPDCHANNEL_TYPE_GUILD == pParam->m_eType)
	{
		SetCreatedGuildChannel(pcsCharacter, TRUE, pChannel->m_strGuildName);
	}

	pChannel->SetTickCount(GetClockCount());
	pChannel->m_CriticalSection.Unlock();

	// Join Channel
	return SendPacketJoinChannel(pcsCharacter, pParam->m_strName, pParam->m_eType, pChannel);
}

BOOL AgsmChannel::OperationDestroyChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	
	return TRUE;
}

BOOL AgsmChannel::OperationFindUser(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	AgpdCharacter *pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(pParam->m_strUserName);
	if (NULL == pcsTargetChar || (FALSE == m_pcsAgpmCharacter->IsPC(pcsTargetChar)))
	{
		SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHARACTER);
	}
	else
	{
		SendPacketFindUser(pcsCharacter, pParam->m_strUserName);
	}

	return TRUE;
}

BOOL AgsmChannel::OperationLeaveChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName)
{
	// 채널에서 나를 뺀다.
	ApStrChannelName strChannelName = rChannelName;
	strChannelName.MakeUpper();

	AgpdChannel* pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	ASSERT(NULL != pChannel);
	if (pChannel)
	{
		pChannel->RemoveUser(pcsCharacter);
	}

	// 내가 참여한 채널 정보에서 뺀다.
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	ASSERT(NULL != pChannelADChar);
	if (pChannelADChar)
	{
		AgpdJoinChannelADChar::iterChannelName iter = std::find(pChannelADChar->m_vtJoinRooms.begin(), 
														pChannelADChar->m_vtJoinRooms.end(), rChannelName);

		if (iter != pChannelADChar->m_vtJoinRooms.end())
		{
			pChannelADChar->m_vtJoinRooms.erase(iter);
		}
	}

	// 채널에 있는 유저들에게 내가 나가는걸 알려준다.
	if (NULL == pChannel)
		return FALSE;

	{
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (lock.Result())
		{
			AgpdCharacter *pcsUser = NULL;
			for (std::list<ApStrCharacterName>::iterator iter = pChannel->m_UserList.begin(); iter != pChannel->m_UserList.end(); ++iter)
			{
				pcsUser = m_pcsAgpmCharacter->GetCharacter(iter->GetBuffer());
				if (pcsUser)
					SendPacketRemoveUser(pcsUser, pcsCharacter, rChannelName);
			}
		}
	}

	// Leave
	SendPacketLeaveChannel(pcsCharacter, rChannelName);

	// 길드채널이면 인원수에 상관없이 길드마스터가 나갈때 채널을 파괴시킨다.
	if (AGPDCHANNEL_TYPE_GUILD == pChannel->GetChannelType())
	{
		DestroyChannel(pChannel);
		pChannelADChar->SetCreatedGuildChannel(FALSE);
		return TRUE;
	}

	// 채널에 한명만 있다면 타이머를 설정한다. 한명인채로 5분동안 있다면 채널을 파괴한다.
	if (1 == pChannel->GetCurrentUser())
	{
		pChannel->SetTickCount(GetClockCount());
		return TRUE;
	}

	// 채널에 한명도 없다면 방을 파괴한다.
	if (0 == pChannel->GetCurrentUser())
	{
		DestroyChannel(pChannel);
		return TRUE;
	}

	return TRUE;
}

BOOL AgsmChannel::OperationPassword(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel* pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	ASSERT(NULL != pChannel);
	if (NULL == pChannel)
		return FALSE;
	
	if (AGPDCHANNEL_TYPE_PRIVATE != pChannel->GetChannelType())
		return FALSE;

	{
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (lock.Result())
		{
			// 채널에 입장가능한지 확인
			if (pChannel->IsFull())
			{
				SendPacketMsgCode(pcsCharacter, pParam->m_strName, AGPDCHANNEL_TYPE_PRIVATE, AGPMCHANNEL_MSG_CODE_FULL_USER);
				return FALSE;
			}

			// 패스워드 확인
			if (COMPARE_EQUAL != pChannel->m_strPassword.Compare(pParam->m_strPassword))
			{
				SendPacketMsgCode(pcsCharacter, pParam->m_strName, AGPDCHANNEL_TYPE_PRIVATE, AGPMCHANNEL_MSG_CODE_INVALID_PASSWORD);
				return FALSE;
			}
			else
			{
				SendPacketMsgCode(pcsCharacter, pParam->m_strName, AGPDCHANNEL_TYPE_PRIVATE, AGPMCHANNEL_MSG_CODE_VALID_PASSWORD);
			}

			// User 추가
			pChannel->AddUser(pcsCharacter);
		}
	}

	// 채널에 있는 사람들에게 입장한다고 알려준다.
	{
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (lock.Result())
		{
			for (AgpdChannel::iterChannelUser iter = pChannel->m_UserList.begin(); iter != pChannel->m_UserList.end(); ++iter)
			{
				AgpdCharacter *pcsUser = m_pcsAgpmCharacter->GetCharacter(iter->GetBuffer());
				if (pcsUser)
					SendPacketAddUser(pcsUser, pcsCharacter, pParam->m_strName);
			}
		}
	}

	// Join Channel
	return SendPacketJoinChannel(pcsCharacter, pParam->m_strName, AGPDCHANNEL_TYPE_PRIVATE, pChannel);
}

BOOL AgsmChannel::OperationChatMsg(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel* pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	ASSERT(NULL != pChannel);
	if (NULL == pChannel)
		return FALSE;

	AuAutoLock lock(pChannel->m_CriticalSection);
	if (!lock.Result()) return FALSE;

	// 채널에 있는 유저들에게 채팅 메세지 전송
	{
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (lock.Result())
		{
			AgpdCharacter *pcsUser = NULL;
			for (std::list<ApStrCharacterName>::iterator iter = pChannel->m_UserList.begin(); iter != pChannel->m_UserList.end(); ++iter)
			{
				pcsUser = m_pcsAgpmCharacter->GetCharacter(iter->GetBuffer());
				if (pcsUser)
					SendPacketChatMsg(pcsUser, pParam->m_strName, pParam->m_strUserName, pParam->m_strChatMsg);
			}
		}
	}

	return TRUE;
}

BOOL AgsmChannel::OperationJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	if (FALSE == IsValidJoinChannel(pcsCharacter, pParam))
		return FALSE;

	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel *pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	if (NULL == pChannel)
		return FALSE;

	AuAutoLock lock(pChannel->m_CriticalSection);
	if (!lock.Result()) return FALSE;
		
	// 채널에 참여시킨다.
	if (FALSE == pChannel->AddUser(pcsCharacter))
	{
		return FALSE;
	}

	// AgpdJoinChannelADChar에 채널을 추가한다.
	AgpdJoinChannelADChar *pChannelADChar = m_pcsAgpmChannel->GetAttachAgpdJoinChannelData(pcsCharacter);
	if (NULL == pChannelADChar)
	{
		return FALSE;
	}

	// 더이상 추가 할수 없다면
	if (false == pChannelADChar->m_vtJoinRooms.push_back(pChannel->m_strName))
	{
		return FALSE;
	}

	// 채널에 있는 사람들에게 입장한다고 알려준다.
	{
		AuAutoLock lock(pChannel->m_CriticalSection);
		if (lock.Result())
		{
			for (AgpdChannel::iterChannelUser iter = pChannel->m_UserList.begin(); iter != pChannel->m_UserList.end(); ++iter)
			{
				AgpdCharacter *pcsUser = m_pcsAgpmCharacter->GetCharacter(iter->GetBuffer());
				if (pcsUser)
					SendPacketAddUser(pcsUser, pcsCharacter, pParam->m_strName);
			}
		}
	}

	// Join Channel
	return SendPacketJoinChannel(pcsCharacter, pParam->m_strName, pParam->m_eType, pChannel);
}

BOOL AgsmChannel::OperationInvitation(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam)
{
	if (FALSE == IsValidInvitation(pcsCharacter, pParam))
		return FALSE;

	AgpdCharacter *pcsTargetChar = m_pcsAgpmCharacter->GetCharacter(pParam->m_strUserName);
	if (NULL == pcsTargetChar)
		return FALSE;

	ApStrChannelName strChannelName = pParam->m_strName;
	strChannelName.MakeUpper();

	AgpdChannel *pChannel = m_pcsAgpmChannel->m_csAdminChannel.Get(strChannelName);
	if (NULL == pChannel)
		return FALSE;

	// 길드방에 Join하고 싶다면 길드원인지 확인
	if (AGPDCHANNEL_TYPE_GUILD == pChannel->GetChannelType())
	{
		AgpdGuildADChar* pGuild = m_pcsAgpmGuild->GetADCharacter(pcsTargetChar);
		if (NULL == pGuild)
			return FALSE;

		// 해당 길드원이 아닐경우
		if (COMPARE_EQUAL != pChannel->m_strGuildName.CompareNoCase(pGuild->m_szGuildID))
		{
			SendPacketMsgCode(pcsCharacter, pParam->m_strName, pParam->m_eType, AGPMCHANNEL_MSG_CODE_NOT_GUILDMEMBER);
			return FALSE;
		}
	}
    
	return SendPacketInvitation(pcsTargetChar, pcsCharacter, pChannel->GetChannelType(), pParam->m_strName);
}

BOOL AgsmChannel::SendPacketCreateChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType)
{
	//INT16 nPacketLength = 0;
	//PVOID pvPacket = m_pcsAgpmChannel->MakePacketCreateChannel(&nPacketLength, pcsCharacter->m_lID, rChannelName.GetBuffer(), eType);

 //   if (NULL == pvPacket)
	//{
	//	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
	//	return FALSE;
	//}

	//BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	//m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	//return bResult;
	return TRUE;
}

BOOL AgsmChannel::SendPacketDestroyChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketDestroyChannel(&nPacketLength, pcsCharacter->m_lID, rChannelName.GetBuffer(), eType);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketMsgCode(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType, 
							  EnumAgpmChannelMsgCode eMsgCode)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketMsgCode(&nPacketLength, pcsCharacter->m_lID, rChannelName.GetBuffer(), eType, eMsgCode);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketFindUser(AgpdCharacter *pcsCharacter, ApStrCharacterName &rUserName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketFindUser(&nPacketLength, pcsCharacter->m_lID, rUserName);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketLeaveChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketLeaveChannel(&nPacketLength, pcsCharacter->m_lID, rChannelName);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketChatMsg(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, ApStrCharacterName &rUserName, 
										ApStrChannelChatMsg &rChatMsg)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketChatMsg(&nPacketLength, pcsCharacter->m_lID, rChannelName, rUserName, rChatMsg);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketAddUser(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsAddUser, ApStrChannelName &rChannelName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketAddUser(&nPacketLength, pcsCharacter->m_lID, rChannelName, pcsAddUser->m_szID);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketRemoveUser(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsRemoveUser, ApStrChannelName &rChannelName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketRemoveUser(&nPacketLength, pcsCharacter->m_lID, rChannelName, pcsRemoveUser->m_szID);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketPassword(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketPassword(&nPacketLength, pcsCharacter->m_lID, rChannelName, _T(""));

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketInvitation(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsInviteUser, EnumChannelType eType, ApStrChannelName &rChannelName)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketInvitation(&nPacketLength, pcsCharacter->m_lID, rChannelName, eType, pcsInviteUser->m_szID);

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmChannel::SendPacketJoinChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, 
								  EnumChannelType eType, AgpdChannel *pcsChannel)
{
	AgsdChannelUserList *pStrUserList = new AgsdChannelUserList;

	{
		AuAutoLock lock(pcsChannel->m_CriticalSection);
		if (lock.Result())
		{
			for (AgpdChannel::iterChannelUser iter = pcsChannel->m_UserList.begin(); iter != pcsChannel->m_UserList.end(); ++iter)
			{
				pStrUserList->m_strChannelUserList.AppendFormat(_T("%s;"), iter->GetBuffer());
			}
		}
	}

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmChannel->MakePacketJoinChannel(&nPacketLength, pcsCharacter->m_lID, pcsChannel->m_strName, 
												pcsChannel->GetChannelType(), (PVOID)(pStrUserList->m_strChannelUserList.GetBuffer()), 
												(INT16)pStrUserList->m_strChannelUserList.GetLength());

	delete pStrUserList;

    if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter->m_lID));
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

