#pragma once

/**************************************************************************************************

작성일: 2008-07-10
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 라이브러리의 세션관련 작업을 하는 객체

**************************************************************************************************/

#ifdef VERSION_VS60
	#ifdef _DEBUG
		#pragma  comment(lib, "ClientStub_VS60_d.lib")
	#else
		#pragma  comment(lib, "ClientStub_VS60.lib")
	#endif
#elif VERSION_VS2003
	#ifdef _UNICODE
		#ifdef _DEBUG
			#pragma  comment(lib, "ClientStub_VS2003_d.lib")
		#else
			#pragma  comment(lib, "ClientStub_VS2003.lib")
		#endif	
	#else
		#ifdef _DEBUG
			#pragma  comment(lib, "ClientStub_VS2003_MB_d.lib")
		#else
			#pragma  comment(lib, "ClientStub_VS2003_MB.lib")
		#endif	
	#endif
#elif VERSION_VS2005
	#ifdef _WIN64
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_64_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_64.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_64_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_64_MB.lib")
			#endif	
		#endif
	#else
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_MB.lib")
			#endif	
		#endif
	#endif
#elif VERSION_VS2005_NOSP
	#ifdef _WIN64
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_64_MB.lib")
			#endif	
		#endif
	#else
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2005_NOSP_MB.lib")
			#endif	
		#endif
	#endif
#elif VERSION_VS2008
	#ifdef _WIN64
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2008_64_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2008_64.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2008_64_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2008_64_MB.lib")
			#endif	
		#endif
	#else
		#ifdef _UNICODE
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2008_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2008.lib")
			#endif	
		#else
			#ifdef _DEBUG
				#pragma  comment(lib, "ClientStub_VS2008_MB_d.lib")
			#else
				#pragma  comment(lib, "ClientStub_VS2008_MB.lib")
			#endif	
		#endif
	#endif
#endif

#include <Winsock2.h>
#include <Windows.h>
#include <WBANetwork.h>
#include <util/Stream.h>
#include "BillEventHandler.h"

class CLibClientSession : public CBillEventHandler
{
public:
	CLibClientSession(void);
	virtual  ~CLibClientSession(void);

protected:		
	//빌링서버의 로그를 작성 이벤트 이다.
	//함수내부에서 로그를 남길수 있도록 해야한다.
	void WriteLog(char* szMsg);

	//빌링서버의 연결 성공 여부 (새로 연결되면 현재 차감이 필요한 모든 유저의 정보를 다시 로그인해야한다.)
	//		success: 연결 성공 여부
	//		error: 에러코드
	void OnNetConnect(bool success, DWORD error);
	
	//빌링서버로 보낸 데이타 전송 여부
	//		size: 전송된 패킷 사이즈
	void OnNetSend( int size );
	
	//빌링서버와 세션이 끊겼을때 (빌링서버가 죽은것으로 처리 해야한다.)	
	//		error: 에러코드
	void OnNetClose( DWORD error );	

	//유저의 결재 정보가 변경되었을 때
	//		dwBillingGUID: 빌링 유니크 아이디
	//		RealEndDate: 정액 사용 종료 일시 (__time32_t) 
	//		EndDate: 정액 실제 종료 일시
	//		dwRestPoint: 정량 잔여 포인트 
	//		nDeductType: 사용자 차감 유형
	//		nAccessCheck: 접근 체크 결과
	//		nResultCode: 처리결과
	void OnUserStatus(DWORD dwAccountGUID, 
					  long dwBillingGUID, 
					  DWORD RealEndDate, 
					  DWORD EndDate, 
					  double dRestPoint, 
					  double dRestTime, 
					  short nDeductType, 
					  short nAccessCheck, 
					  short nResultCode);

	//PC 방 멀티 유저 조회 결과 
	//		결과 코드
	//		1 : 가능
	//		2 : 불가
	//		-1 : PC방 정보 없음 
	void OnInquireMultiUser(long AccountID, long RoomGUID, long Result);

	// 특정 PC방의 게임 별 정량 보유 여부를 조회한다.
	void OnInquirePCRoomPoint(long AccountID, 
							  long RoomGUID, 
							  long GameCode, 
							  long ResultCode);

	void OnInquirePersonDeduct(long AccountGUID, long GameCode, long ResultCode);

	//// 차감 타입 조회
	//void OnInquireDeductType(long AccountID, long Result);

private:
	CRITICAL_SECTION m_cs;

};
