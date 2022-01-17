#pragma once

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpdBuddy.h"

class AgpmCharacter;
class AgpdCharacter;

enum EnumAgpmBuddyDataType
{
	AGPMBUDDY_DATA_BUDDY,
};

enum EnumAgpmBuddyCallback
{
	AGPMBUDDY_CALLBACK_NONE			= 0,
	AGPMBUDDY_CALLBACK_ADD,
	AGPMBUDDY_CALLBACK_ADD_REQUEST,
	AGPMBUDDY_CALLBACK_ADD_REJECT,
	AGPMBUDDY_CALLBACK_REMOVE,
	AGPMBUDDY_CALLBACK_OPTIONS,
	AGPMBUDDY_CALLBACK_ONLINE,
	AGPMBUDDY_CALLBACK_REGION,
	AGPMBUDDY_CALLBACK_MSG_CODE,
	AGPMBUDDY_CALLBACK_INIT_END,
	AGPMBUDDY_CALLBACK_MAX,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST,
	AGPMBUDDY_CALLBACK_MENTOR_DELETE,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST_ACCEPT,
	AGPMBUDDY_CALLBACK_MENTOR_REQUEST_REJECT,
	AGPMBUDDY_CALLBACK_MENTOR_UI_OPEN,
};

enum EnumAgpmBuddyOperation
{
	AGPMBUDDY_OPERATION_NONE		= 0,
	AGPMBUDDY_OPERATION_ADD,					// 버디 추가
	AGPMBUDDY_OPERATION_ADD_REQUEST,			// 버디 추가 요청
	AGPMBUDDY_OPERATION_ADD_REJECT,				// 버디 추가 거절
	AGPMBUDDY_OPERATION_REMOVE,					// 버디 삭제
	AGPMBUDDY_OPERATION_OPTIONS,				// 속성 업데이트
	AGPMBUDDY_OPERATION_ONLINE,					// On/Off line
	AGPMBUDDY_OPERATION_REGION,					// 위치 정보
	AGPMBUDDY_OPERATION_MSG_CODE,				// message code
	AGPMBUDDY_OPERATION_INIT_END,				// 초기화 정보 전송 완료
	//밑으로는 친구초대 시스템 A - 초대하는 캐릭터(Mentor로 지칭) B - 초대받는 캐릭터(Mentee로 지칭)
	AGPMBUDDY_OPERATION_MENTOR_REQUEST,			// Mentor가 되기를 요청
	AGPMBUDDY_OPERATION_MENTOR_DELETE,			// Mentor 삭제
	AGPMBUDDY_OPRRATION_MENTOR_REQUEST_ACCEPT,	// 요청 수락
	AGPMBUDDY_OPRRATION_MENTOR_REQUEST_REJECT,	// 요청 거부
	AGPMBUDDY_OPRRATION_MENTOR_UI_OPEN,			// 사제창 오픈.
	AGPMBUDDY_OPERATION_MAX,
};

enum EnumAgpmBuddyMsgCode
{
	AGPMBUDDY_MSG_CODE_NONE		= 0,
	AGPMBUDDY_MSG_CODE_ALREADY_EXIST,		// 이미 등록되어 있는 캐릭터를 등록하려고 할때
	AGPMBUDDY_MSG_CODE_OFFLINE_OR_NOTEXIST,	// 오프라인 이거나 존재하지 않는 캐릭터
	AGPMBUDDY_MSG_CODE_INVALID_USER,		// 등록할 수 없는 캐릭터
	AGPMBUDDY_MSG_CODE_REFUSE,				// 거절 상태 메시지
	// 밑으로는 친구초대 시스템 메시지
	AGPMBUDDY_MSG_CODE_INVALID_MENTOR_LEVEL,	// 지정된 Mentor레벨 구간에서 벗어난 경우
	AGPMBUDDY_MSG_CODE_INVALID_MENTEE_LEVEL,	// 지정된 Mentee레벨 구간에서 벗어난 경우
	AGPMBUDDY_MSG_CODE_FULL_MENTEE_NUMBER,		// Mentee숫자가 꽉찾을 경우
	AGPMBUDDY_MSG_CODE_ALREADY_EXIST_MENTOR,	// 이미 Mentor가 존재할 경우
	AGPMBUDDY_MSG_CODE_RECEIVER_IS_MENTOR,		// 제자가 될 캐릭이 이미 Mentor일 경우.
	AGPMBUDDY_MSG_CODE_REQUESTOR_IS_MENTEE,		// 신청자가 다른이의 Mentee일 경우
	AGPMBUDDY_MSG_CODE_ALREADY_REQUEST,			// 원하는 프로세스를 이미 요청한 경우
	AGPMBUDDY_MSG_CODE_INVALID_PROCESS,			// 잘못된 요청.
	AGPMBUDDY_MSG_CODE_NOT_DELETE_BECAUSE_MENTOR,// 친구삭제 요청시 Mentor관계로 인해 삭제 불가능하다.
	AGPMBUDDY_MSG_CODE_NOT_INVITE_BUDDY,		// 요청 불가능한 상태임을 알린다.
};

class AgpmBuddy : public ApModule
{
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	INT32			m_lIndexAttachData;
	AgpdBuddyConstrict m_stBuddyConstrict;

public:
	AuPacket		m_csPacket;

private:
	PVOID MakePacketBuddy(INT16 *pnPacketLength, EnumAgpmBuddyOperation eOperation, INT32 lCID, 
							AgpdBuddyElement *pcsBuddyElement, INT32 lRegionIndex, EnumAgpmBuddyMsgCode eMsgCode);

	// ApSafeArray에서 operator & 를 사용하지 못하게 지정했기 때문에 레퍼런스로 함수호출을 한다.
	BOOL _AddBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _RemoveBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _OptionsBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);
	BOOL _MentorBuddy(ApVectorBuddy &rVector, AgpdBuddyElement *pcsBuddyElement);

public:
	AgpmBuddy();
	virtual ~AgpmBuddy();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL AgpdBuddyConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AgpdBuddyDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdBuddyADChar* GetAttachAgpdBuddyData(AgpdCharacter *pcsCharacter);

	BOOL ReadMentorConstrinctFile(CHAR* szFileName);

	PVOID MakePacketAddBuddy(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketAddRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketAddReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketRemoveBuddy(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketUpdateOptions(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketOnline(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMsgCode(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, 
							AgpdBuddyElement* pcsBuddyElement, EnumAgpmBuddyMsgCode eMsgCode);
	PVOID MakePacketRegion(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement, INT32 lRegionIndex);
	PVOID MakePacketInitEnd(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter);

	//Mentor 패킷
	PVOID MakePacketMentorRequest(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorDelete(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorRequestAccept(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorRequestReject(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	PVOID MakePacketMentorUIOpen(INT16 *pnPacketLength, AgpdCharacter *pcsCharacter);

	BOOL SetCallbackAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOptions(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOnline(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRegion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInitEnd(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackMentorRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorRequestAccept(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorRequestReject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMentorUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL AddBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL RemoveBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL OptionsBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL MentorBuddy(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);
	BOOL SetOnline(AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement);

	BOOL IsExistBuddy(AgpdCharacter *pcsCharacter, CHAR *szName, BOOL bCheckBuddy = TRUE, BOOL bCheckBan = TRUE);
	BOOL IsAlreadyMentor(AgpdCharacter *pcsCharacter);
	BOOL IsAlreadyMentee(AgpdCharacter *pcsCharacter);
	BOOL OverMenteeNumber(AgpdCharacter *pcsCharacter);
	BOOL UnderMentorLevel(AgpdCharacter *pcsCharacter);
	BOOL OverMenteeLevel(AgpdCharacter *pcsCharacter);
	INT32 GetMentorStatus(AgpdCharacter *pcsCharacter, CHAR *szName);
	INT32 GetMentorCID(AgpdCharacter *pcsCharacter);
};