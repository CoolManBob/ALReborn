#pragma once

/**************************************************************************************************

작성일: 2008-07-08
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 빌링서버를 사용하는 클라이언트에게 수신된 패킷을 전달해 주기 위한 이벤트 객체 
      이벤트를 받는쪽(빌링클라이언트)에서 상속받아 사용해야 한다.

**************************************************************************************************/

#include "ClientSession.h"
#include "protocol_billing.h"

class CBillEventHandler : public CClientSession
{
public:
	CBillEventHandler(void);
	virtual ~CBillEventHandler(void);

	//초기화 함수 (프로그램 실행하고 한번만 사용해야 한다.)
	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);

	//접속 성공후 로그인 한다.
	bool UserLogin(DWORD dwAccountGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType);

	//접속 해제후 로그아웃 한다.
	bool UserLogout(long dwBillingGUID);

	//사용자 정보를 조회한다.
	bool InquireUser(long dwBillingGUID);	

	//PC 방이 멀티 유저를 지원하는지 조회한다.
	bool InquireMultiUser(long AccountGUID, long RoomGUID);	

	// 개인의 게임 별 정액, 정량 보유 여부를 조회한다.
	bool InquirePersonDeduct(long AccountGUID, long GameCode);

	// 특정 PC방의 게임 별 정량 보유 여부를 조회한다.
	bool InquirePCRoomPoint(long AccountGUID, long RoomGUID, long GameCode);

protected:	
	void OnReceive( PBYTE buffer, int size );	

	//빌링서버의 연결 성공 여부 (새로 연결되면 현재 차감이 필요한 모든 유저의 정보를 다시 로그인해야한다.)
	//		success: 연결 성공 여부
	//		error: 에러코드
	void OnConnect(bool success, DWORD error);

	//빌링서버로 보낸 데이타 전송 여부
	//		size: 전송된 패킷 사이즈
	void OnSend( int size );

	//빌링서버와 세션이 끊겼을때 (빌링서버가 죽은것으로 처리 해야한다.)	
	//		error: 에러코드
	void OnClose( DWORD error );	


protected:
	
	//유저의 결재 정보가 변경되었을 때
			//dwAccountGUID: 로그인 할때 전달한 계정 GUID
			//dwBillingGUID: 빌링 유니크 아이디
			//RealEndDate: 정액 사용 종료 일시 (__time32_t) 
			//EndDate: 정액 실제 종료 일시
			//dwRestPoint: 정량 잔여 포인트 
			//dRestTime; 정량 잔연 시간(초)
			//nDeductType: 사용자 차감 유형
			//nAccessCheck: 접근 체크 결과
			//nResultCode: 처리결과
	virtual void OnUserStatus(DWORD dwAccountGUID, 
							  long dwBillingGUID, 
							  DWORD RealEndDate, 
							  DWORD EndDate, 
							  double dRestPoint, 
							  double dRestTime, 
							  short nDeductType, 
							  short nAccessCheck, 
							  short nResultCode) = 0;

	// 개인의 게임 별 정액, 정량 보유 여부를 조회한다.
	virtual void OnInquirePersonDeduct(long AccountGUID, 
									   long GameCode, 
									   long ResultCode) = 0;

	// 특정 PC방의 게임 별 정량 보유 여부를 조회한다.
	virtual void OnInquirePCRoomPoint(long AccountGUID, 
									  long RoomGUID, 
									  long GameCode, 
									  long ResultCode) = 0;

	//PC 방이 멀티 유저를 지원하는지 조회한다.
	virtual void OnInquireMultiUser(long AccountID, long RoomGUID, long Result) = 0;	

	// 2010.09.07 빌링에 로그인한 유저 정보를 인증서버에 전송
	// 실제로 인증서버에 로그인 했는지 체크하도록 알려주는 인터페이스이다.
	// 로그인 후 3분에 한번씩 전송 된다.
	// 상품이 변경되는 시점과, 로그아웃 되면 전송되지 않는다.
	virtual void OnCheckLoginUser(long AccountID, long BillingGUID) = 0;

	//서버에 연결한 결과를 알려줍니다.
	virtual void OnNetConnect(bool success, DWORD error) = 0;
	
	//서버에 데이타를 전송한 결과를 알려줍니다.
	virtual void OnNetSend( int size ) = 0;
	
	//서버와 접속이 종료되었을때 발생합니다.
	virtual void OnNetClose( DWORD error ) = 0;	
	
	//로그를 써야할때 보내는 이벤트 메소드
	virtual void WriteLog(char* szMsg) = 0;

private:
	//로그를 사용한다.
	void WriteLog(const char* szFormat, ...);
};
