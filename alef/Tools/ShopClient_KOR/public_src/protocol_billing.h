#pragma once
#pragma pack(1)

/**************************************************************************************************

작성일: 2008-07-18
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 빌링서버 프로토콜 정의

**************************************************************************************************/

#define MAX_ACCOUNTID							51
#define MAX_QUREY_LENGTH						1024
#define MAX_PCROOM_GAMECODE_LENGTH				3

#define PROTOCOL_CLIENT_LOGIN					0x01
#define PROTOCOL_SERVER_USERSTATUS				0x02
#define PROTOCOL_CLIENT_LOGOUT					0x03
#define PROTOCOL_CLIENT_INQUIRE					0x04
#define PROTOCOL_CLIENT_INQUIRE_MULTIUSER		0x05
#define PROTOCOL_SERVER_INQUIRE_MULTIUSER		0x06
#define PROTOCOL_CLIENT_INQUIRE_PERSON_DEDUCT	0x07
#define PROTOCOL_SERVER_INQUIRE_PERSON_DEDUCT	0x08
#define PROTOCOL_CLIENT_INQUIRE_PCROOM_DEDUCT	0x09
#define PROTOCOL_SERVER_INQUIRE_PCROOM_DEDUCT	0x0A
#define PROTOCOL_SERVER_CHECK_LOGIN_USER		0x0B

namespace BillingProtocol
{

template <class _subcls>
class MSG_BASE
{
public:
	MSG_BASE() {}
	MSG_BASE(DWORD dwID) : dwProtocolID(dwID), dwPacketSize(sizeof( _subcls )) {}
	
public:
	DWORD 	dwPacketSize; 					//패킷의 전체 사이즈
	DWORD	dwProtocolID;					//패킷 아이디
};

//(C->S) 로그인 할때 보내는 패킷
class MSG_CLIENT_LOGIN : public MSG_BASE<MSG_CLIENT_LOGIN>	
{
public:
	MSG_CLIENT_LOGIN() : MSG_BASE<MSG_CLIENT_LOGIN>(PROTOCOL_CLIENT_LOGIN), 
		dwAccountGUID(0), 
		dwIPAddress(0), 
		dwRoomGUID(0), 
		dwGameCode(0), 
		dwServerType(0)
	{}

public:
	DWORD 	dwAccountGUID;				 	//아이디 GUID	
	DWORD	dwIPAddress;					//클라이언트 아이피 주소
	DWORD 	dwRoomGUID;						//PC 방 GUID
	DWORD	dwGameCode;						//게임 코드
	DWORD	dwServerType;					//테섭인지 라이브인지 구분 코드	
};

//(S->C) 유저 상태 정보 변경
class MSG_SERVER_USERSTATUS : public MSG_BASE<MSG_SERVER_USERSTATUS>	
{
public:
	MSG_SERVER_USERSTATUS() : MSG_BASE<MSG_SERVER_USERSTATUS>(PROTOCOL_SERVER_USERSTATUS), 	
		dwAccountGUID(0), 
		dwBillingGUID(0), 
		RealEndDate(0), 
		EndDate(0), 
		dRestPoint(0), 
		dRestTime(0), 
		sDeductType(0), 
		sAccessCheck(0), 
		sResultCode(0)
	{}

public:
	DWORD 	dwAccountGUID;				 	//아이디 GUID	
	long	dwBillingGUID;					//빌링에서 사용하는 GUID (로그아웃시 보내줘야 한다.)
	DWORD	RealEndDate;					//정액일경우 실제 종료일자 (특정 시간 정액일 경우 적용)
	DWORD	EndDate;						//정액일경우 결제 종료일자
	double	dRestPoint;						//정량일 경우 잔여포인트
	double	dRestTime;						//정량일 경우 시간 (Sec)
	short	sDeductType;					//사용자 차감 유형
	short	sAccessCheck;					//접근 체크 결과
	short	sResultCode;					//처리 결과 코드
};

//(C->S) 로그아웃 할때 보내는 패킷
class MSG_CLIENT_LOGOUT : public MSG_BASE<MSG_CLIENT_LOGOUT>
{
public:
	MSG_CLIENT_LOGOUT() : MSG_BASE<MSG_CLIENT_LOGOUT>(PROTOCOL_CLIENT_LOGOUT), 
		dwBillingGUID(0)
	{}

public:
	long	dwBillingGUID;					//로그인 결과로 받은 GUID
};

//(C->S) 사용자 정보 조회
class MSG_CLIENT_INQUIRE : public MSG_BASE<MSG_CLIENT_INQUIRE>
{
public:
	MSG_CLIENT_INQUIRE() : MSG_BASE<MSG_CLIENT_INQUIRE>(PROTOCOL_CLIENT_INQUIRE), 	
		dwBillingGUID(0)
		{}

public:
	long	dwBillingGUID;					//로그인 결과로 받은 GUID
};

//(C->S) 멀티유저 가능한지 조회
class MSG_CLIENT_INQUIRE_MULTIUSER : public MSG_BASE<MSG_CLIENT_INQUIRE_MULTIUSER>	
{
public:
	MSG_CLIENT_INQUIRE_MULTIUSER() : MSG_BASE<MSG_CLIENT_INQUIRE_MULTIUSER>(PROTOCOL_CLIENT_INQUIRE_MULTIUSER), 	
		dwRoomGUID(0)
		{}

public:
	DWORD   dwAccountID;
	DWORD	dwRoomGUID;						//게임방 고유 번호
};

//(S->C) 멀티유저 가능한지 조회
class MSG_SERVER_INQUIRE_MULTIUSER : public MSG_BASE<MSG_SERVER_INQUIRE_MULTIUSER>	
{
public:
	MSG_SERVER_INQUIRE_MULTIUSER() : MSG_BASE<MSG_SERVER_INQUIRE_MULTIUSER>(PROTOCOL_SERVER_INQUIRE_MULTIUSER), 	
		dwRoomGUID(0)
		{}

public:
	DWORD   dwAccountID;
	DWORD	dwRoomGUID;						//게임방 고유 번호
	long	result;							//조회 결과
};

//(C->S) 개인의 게임 별 사용 가능한 정액,정량 보유 여부를 조회한다.
class MSG_CLIENT_INQUIRE_PERSON_DEDUCT : public MSG_BASE<MSG_CLIENT_INQUIRE_PERSON_DEDUCT>	
{
public:
	MSG_CLIENT_INQUIRE_PERSON_DEDUCT() 
		: MSG_BASE<MSG_CLIENT_INQUIRE_PERSON_DEDUCT>(PROTOCOL_CLIENT_INQUIRE_PERSON_DEDUCT), 
		dwAccountGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD	dwGameCode;
};

//(S->C) 개인의 게임 별 사용 가능한 정액,정량 보유 여부를 조회한다.
class MSG_SERVER_INQUIRE_PERSON_DEDUCT : public MSG_BASE<MSG_SERVER_INQUIRE_PERSON_DEDUCT>	
{
public:
	MSG_SERVER_INQUIRE_PERSON_DEDUCT() 
		: MSG_BASE<MSG_SERVER_INQUIRE_PERSON_DEDUCT>(PROTOCOL_SERVER_INQUIRE_PERSON_DEDUCT), 	
		dwAccountGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD	dwGameCode;
	long	ResultCode;
};

//(C->S) 특정 PC방의 게임 별 정량 보유 여부를 조회한다.
class MSG_CLIENT_INQUIRE_PCROOM_DEDUCT : public MSG_BASE<MSG_CLIENT_INQUIRE_PCROOM_DEDUCT>
{
public:
	MSG_CLIENT_INQUIRE_PCROOM_DEDUCT() 
		: MSG_BASE<MSG_CLIENT_INQUIRE_PCROOM_DEDUCT>(PROTOCOL_CLIENT_INQUIRE_PCROOM_DEDUCT), 
		dwAccountGUID(0), dwRoomGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD   dwRoomGUID;
	DWORD	dwGameCode;
};

//(S->C) 특정 PC방의 게임 별 정량 보유 여부를 조회한다.
class MSG_SERVER_INQUIRE_PCROOM_DEDUCT: public MSG_BASE<MSG_SERVER_INQUIRE_PCROOM_DEDUCT>
{
public:
	MSG_SERVER_INQUIRE_PCROOM_DEDUCT() 
		: MSG_BASE<MSG_SERVER_INQUIRE_PCROOM_DEDUCT>(PROTOCOL_SERVER_INQUIRE_PCROOM_DEDUCT),
		dwAccountGUID(0), dwRoomGUID(0), dwGameCode(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	DWORD   dwRoomGUID;
	DWORD	dwGameCode;
	long	ResultCode;
};


//(S->C) 체크 로그인 유저 
// 2010.09.07 빌링에 로그인한 유저 정보를 인증서버에 전송
// 실제로 인증서버에 로그인 했는지 체크하도록 알려주는 인터페이스이다.
class MSG_SERVER_CHECK_LOGIN_USER: public MSG_BASE<MSG_SERVER_CHECK_LOGIN_USER>
{
public:
	MSG_SERVER_CHECK_LOGIN_USER() 
		: MSG_BASE<MSG_SERVER_CHECK_LOGIN_USER>(PROTOCOL_SERVER_CHECK_LOGIN_USER),
		dwAccountGUID(0), dwBillingGUID(0)
		{
		}

public:
	DWORD	dwAccountGUID;
	long	dwBillingGUID;
};

}

#pragma pack()