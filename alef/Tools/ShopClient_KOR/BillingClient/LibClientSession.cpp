#include "stdafx.h"
#include "LibClientSession.h"

CLibClientSession::CLibClientSession(void)
{
	::InitializeCriticalSection(&m_cs);
}

CLibClientSession::~CLibClientSession(void)
{
	::DeleteCriticalSection(&m_cs);
}

void CLibClientSession::OnNetConnect(bool success, DWORD error)
{
	if(success) AfxMessageBox(_T("OnConnect succeed"));
	else AfxMessageBox(_T("OnConnect fali"));
}

void CLibClientSession::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CLibClientSession::OnNetClose( DWORD error )
{
	AfxMessageBox(_T("OnClose"));
}

void CLibClientSession::OnInquireMultiUser(long AccountID, long RoomGUID, long Result)
{
	CString strMsg;

	strMsg.Format(_T("계정 순번: %d, 피시방 GUID: %d, 결과: %d\r\n"), AccountID, RoomGUID, Result);

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

void CLibClientSession::OnInquirePCRoomPoint(long AccountID, 
											 long RoomGUID, 
											 long GameCode, 
											 long ResultCode)
{
	CString strMsg;

	strMsg.Format(_T("AccountID: %d, RoomGUID: %d, GameCode: %d, ResultCode: %d\r\n"), 
				AccountID, RoomGUID, GameCode, ResultCode);

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

void CLibClientSession::OnUserStatus(DWORD dwAccountGUID, 
									 long dwBillingGUID, 
									 DWORD RealEndDate, 
									 DWORD EndDate, 
									 double dRestPoint, 
									 double dRestTime, 
									 short nDeductType, 
									 short nAccessCheck, 
									 short nResultCode)
{
	CString strMsg;

	struct tm* tRealEndDate1;
	struct tm* tEndDate1;

	struct tm tRealEndDate;
	struct tm tEndDate;
	
	tRealEndDate.tm_hour = 0;
	tRealEndDate.tm_isdst = 0;
	tRealEndDate.tm_mday = 0;
	tRealEndDate.tm_min = 0;
	tRealEndDate.tm_mon = 0;
	tRealEndDate.tm_sec = 0;
	tRealEndDate.tm_wday = 0;
	tRealEndDate.tm_yday = 0;
	tRealEndDate.tm_year = 0;

	tEndDate.tm_hour = 0;
	tEndDate.tm_isdst = 0;
	tEndDate.tm_mday = 0;
	tEndDate.tm_min = 0;
	tEndDate.tm_mon = 0;
	tEndDate.tm_sec = 0;
	tEndDate.tm_wday = 0;
	tEndDate.tm_yday = 0;
	tEndDate.tm_year = 0;

#ifdef VERSION_VS60
	tRealEndDate1 = localtime((time_t*)&RealEndDate);
	tEndDate1 = localtime((time_t*)&EndDate);

	if(tRealEndDate1) tRealEndDate = *tRealEndDate1;
	if(tEndDate1) tEndDate = *tEndDate1;
#elif VERSION_VS2003
	tRealEndDate1 = localtime((time_t*)&RealEndDate);
	tEndDate1 = localtime((time_t*)&EndDate);

	if(tRealEndDate1) tRealEndDate = *tRealEndDate1;
	if(tEndDate1) tEndDate = *tEndDate1;
#else
	_localtime32_s(&tRealEndDate, (__time32_t*)&RealEndDate);
	_localtime32_s(&tEndDate, (__time32_t*)&EndDate);
#endif


	CString strend, strRend;

	strend.Format(_T("%d-%d-%d %d:%d:%d"), tEndDate.tm_year+1900, tEndDate.tm_mon+1, tEndDate.tm_mday, tEndDate.tm_hour, tEndDate.tm_min, tEndDate.tm_sec);
	strRend.Format(_T("%d-%d-%d %d:%d:%d"), tRealEndDate.tm_year+1900, tRealEndDate.tm_mon+1, tRealEndDate.tm_mday, tRealEndDate.tm_hour, tRealEndDate.tm_min, tRealEndDate.tm_sec);

	strMsg.Format(_T("개인계정: %d\r\n빌링계정: %d\r\n차감유형: %d\r\n접근체크결과: %d\r\n처리결과코드: %d\r\n잔여포인트: %f\r\n잔여시간: %f\r\n종료시간: ")
		, dwAccountGUID, dwBillingGUID, nDeductType, nAccessCheck, nResultCode, dRestPoint, dRestTime);

	strMsg += strend;
	strMsg += _T("\r\n진짜종료시간: ");
	strMsg += strRend;
	strMsg += _T("\r\n");

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

void CLibClientSession::WriteLog(char* szMsg)
{
	::EnterCriticalSection(&m_cs);

	TCHAR szModule[MAX_PATH] = {0};
	TCHAR szFilePath[MAX_PATH] = {0};

	::GetModuleFileName( NULL, szModule, MAX_PATH );

	*(_tcsrchr( szModule, '\\' ) ) = 0;


	StringCchPrintf(szFilePath, STRSAFE_MAX_CCH , _T("%s\\Billing.log"), szModule );

	CTime t = CTime::GetCurrentTime();   	
	
	CString str;
	str.Format(_T("[%s] %s"), t.Format(_T("%Y-%m-%d %H:%M:%S")), szMsg);

	
	HANDLE	hFile;	
	DWORD	dwBytesWritten, dwPos;  

	hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //File 생성 
	if (hFile != INVALID_HANDLE_VALUE)
	{ 
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_END); 

		LockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0); 
		WriteFile(hFile, str.GetBuffer(0), str.GetLength(), &dwBytesWritten, NULL); 
		str.ReleaseBuffer();
		UnlockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0);     
		CloseHandle(hFile); 
	}  	

	::LeaveCriticalSection(&m_cs);

	OutputDebugString(str);
}

void CLibClientSession::OnInquirePersonDeduct( long AccountGUID, long GameCode, long ResultCode )
{
	CString str;
	str.Format("OnInquirePersonDeduct - AccountGUID[%d], ResultCode[%d]\n", AccountGUID, ResultCode);
	
	WriteLog((LPSTR)(LPCTSTR)str);
}
//void CLibClientSession::OnInquireDeductType(long AccountID, long Result)
//{
//	CString str;
//
//	str.Format("사용자 계정: %d, 결과: %d\r\n", AccountID, Result);
//
//	WriteLog(str.GetBuffer(0));
//}
