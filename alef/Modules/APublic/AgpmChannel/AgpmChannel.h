#pragma once

#include "ApModule.h"
#include "AgpdChannel.h"
#include "AgpaChannel.h"

class AgpmCharacter;

enum EnumAgpmChannelDataType
{
	AGPMCHANNEL_DATA_CHANNEL,
};

enum EnumAgpmChannelOperation
{
	AGPMCHANNEL_OPERATION_NONE			= 0,
	AGPMCHANNEL_OPERATION_CREATE,			// 채팅방을 생성
	AGPMCHANNEL_OPERATION_DESTROY,			// 채팅방을 종료
	AGPMCHANNEL_OPERATION_JOIN,				// 채널 입장
	AGPMCHANNEL_OPERATION_LEAVE,			// 채널 퇴장
	AGPMCHANNEL_OPERATION_ADD_USER,			// 유저 입장
	AGPMCHANNEL_OPERATION_REMOVE_USER,		// 유저 퇴장
	AGPMCHANNEL_OPERATION_INVITATION,		// 초대
	AGPMCHANNEL_OPERATION_PASSWORD,			// 패스워드를 요구
	AGPMCHANNEL_OPERATION_CHATMSG,			// 채팅 메세지 전송
	AGPMCHANNEL_OPERATION_FIND_USER,		// 사용자를 찾음
	AGPMCHANNEL_OPERATION_MSG_CODE,			// Message code
	AGPMCHANNEL_OPERATION_MAX,
};

enum EnumAgpmChannelCallback
{
	AGPMCHANNEL_CB_NONE					= 0,
	AGPMCHANNEL_CB_CREATE,		
	AGPMCHANNEL_CB_DESTROY,		
	AGPMCHANNEL_CB_JOIN,			
	AGPMCHANNEL_CB_LEAVE,		
	AGPMCHANNEL_CB_ADD_USER,		
	AGPMCHANNEL_CB_REMOVE_USER,	
	AGPMCHANNEL_CB_INVITATION,	
	AGPMCHANNEL_CB_PASSWORD,		
	AGPMCHANNEL_CB_CHATMSG,		
	AGPMCHANNEL_CB_FIND_USER,	
	AGPMCHANNEL_CB_MSG_CODE,		
	AGPMCHANNEL_CB_MAX,
};

enum EnumAgpmChannelMsgCode
{
	AGPMCHANNEL_MSG_CODE_NONE			=0,
	AGPMCHANNEL_MSG_CODE_ALREADY_EXIST,			// 이미 존재하는 채널
	AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHANNEL,		// 존재하지 않는 채널
	AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHARACTER,	// 찾는 캐릭터가 존재하지 않거나 Off-line
	AGPMCHANNEL_MSG_CODE_FULL_CHANNEL,			// 동시에 입장 가능한 채널을 가득찼다.
	AGPMCHANNEL_MSG_CODE_NOT_GUILDMASTER,		// 길드 마스터가 아니다.
	AGPMCHANNEL_MSG_CODE_NOT_GUILDMEMBER,		// 길드 멤버가 아니다.
	AGPMCHANNEL_MSG_CODE_ALREADY_GUILDCHANNEL,	// 이미 길드 채널을 만들었다.
	AGPMCHANNEL_MSG_CODE_INVALID_PASSWORD,		// 패스워드가 유효하지 않다.
	AGPMCHANNEL_MSG_CODE_VALID_PASSWORD,		// 유효하다고 인증된 패스워드
	AGPMCHANNEL_MSG_CODE_FULL_USER,				// 채널에 입장 가능한 유저가 가득찼다.
	AGPMCHANNEL_MSG_CODE_ALREADY_JOIN			// 이미 참여중인 채널
};

struct AgpdChannelParam : public ApMemory<AgpdChannelParam, 20>
{
	ApStrChannelName		m_strName;
	ApStrChannelPassword	m_strPassword;
	EnumChannelType			m_eType;
	EnumAgpmChannelMsgCode	m_eMsgCode;
	ApStrChannelChatMsg		m_strChatMsg;
	ApStrCharacterName		m_strUserName;
	PVOID					m_pvUserList;
	UINT16					m_unByteLength;
};

class ApmMap;
class AgpmSystemMessage;

class AgpmChannel : public ApModule
{
private:
	AgpmCharacter*		m_pcsAgpmCharacter;	
	INT32				m_lIndexAttachData;

	ApmMap*				m_papmMap;
	AgpmSystemMessage*	m_pcsAgpmSystemMessage;

public:
	AuPacket		m_csPacket;
	AgpaChannel		m_csAdminChannel;

private:
	PVOID MakePacketChannel(INT16 *pnPacketLength, EnumAgpmChannelOperation eOperation, INT32 lCID, TCHAR *szChannelName, 
							TCHAR *szPassword, TCHAR *szCharName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode, 
							TCHAR *szChatMsg, PVOID pvUserList, UINT16 unMessageLength);

public:
	AgpmChannel();
	virtual ~AgpmChannel();

	BOOL SetMaxChannel(INT32 lMaxChannel);

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL AgpdJoinChannelConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AgpdJoinChannelDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdChannel* CreateChannelData();
	BOOL DestroyChannelData(AgpdChannel *pcsChannel);

	// callback functions
	BOOL SetCallbackCreateChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDestroyChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJoin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChatMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackFindUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// packet functions
	//. 2005. 11. 23 Nonstopdj
	//. 비밀번호 인자추가.
	PVOID MakePacketCreateChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, TCHAR* szPassword, EnumChannelType eType);

	PVOID MakePacketDestroyChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, EnumChannelType eType);
	PVOID MakePacketMsgCode(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode);
	PVOID MakePacketJoinChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, PVOID pvUserList, UINT16 unMessageLength);
	PVOID MakePacketLeaveChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName);
	PVOID MakePacketAddUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName);
	PVOID MakePacketRemoveUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName);
	PVOID MakePacketChatMsg(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName, TCHAR *szChatMsg);
	PVOID MakePacketPassword(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szPassword);
	PVOID MakePacketInvitation(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, TCHAR *szUserName);
	PVOID MakePacketFindUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szUserName);

	AgpdJoinChannelADChar* GetAttachAgpdJoinChannelData(AgpdCharacter *pcsCharacter);
};