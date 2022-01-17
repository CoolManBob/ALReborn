#include "AuHangameTPack.h"

#ifdef _AREA_KOREA_
#ifdef _HANGAME_
#include "AgsmAccountManager.h"
#include "../Server/HanGameForServer/HangameTPack/DORIAN_Connector/Include/DORIAN_3.0.h"

AuHangameTPack::AuHangameTPack()
{
	m_csMutex.Init();
}

AuHangameTPack::~AuHangameTPack()
{
	m_csMutex.Destroy();
}

BOOL AuHangameTPack::Create(INT32 lServerID)
{
	CHAR szError[255+1] = { 0, };

	Dorian::GAMESERVER_INFO pServerInfo;
	memset(&pServerInfo, 0, sizeof(Dorian::GAMESERVER_INFO));
	strcpy_s(pServerInfo.szGameID, Dorian::eMAX_GAME_ID, "K_ARCHLORD");
	_itoa_s(lServerID, pServerInfo.szGameServerID, Dorian::eMAX_SERVER_ID);

	Dorian::FUNCTION_POINTER_INFO fpPointers;
	memset(&fpPointers, 0, sizeof(Dorian::FUNCTION_POINTER_INFO));

	//////////////////////////////////////////////////////////////////////////
	// Register Function Pointer

	fpPointers.fpNotiBillInfo = (Dorian::FP_NotifyBillingEvent)AgsmAccountManager::OnBillingEventNotified;

	//////////////////////////////////////////////////////////////////////////

	if(!Dorian::InitModule(pServerInfo, fpPointers, szError, 255))
	{
		printf("%s\n", szError);

		{
			CHAR strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian::InitModule fail - %s\n", szError);
			AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);
		}

		return FALSE;
	}

	{
		CHAR strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian::InitModule success\n");
		AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);
	}
	
	return TRUE;
}

void AuHangameTPack::Destroy()
{
	Dorian::UninitModule();
}

BOOL AuHangameTPack::CheckIn(CHAR* szAccount, CHAR* szIP)
{
	if(!szAccount || !szIP)
		return FALSE;

	BOOL bResult = FALSE;
	Dorian::REQUEST_CHECKIN		req;
	Dorian::RESPONSE_CHECKIN	res;
	memset(&req, 0, sizeof(req));
	memset(&res, 0, sizeof(res));
	memcpy(req.szUserID, szAccount, Dorian::eMAX_USER_ID);
	memcpy(req.szUserIP, szIP, Dorian::eMAX_USER_IP);

	Dorian::Checkin(&req, &res);

	switch(res.nResult)
	{
		case Dorian::RESULT_OK:
			{
				CHAR strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian Checkin result RESULT_OK - [%s], [%d], [%s]\n", 
					req.szUserID, res.nProductID, res.szCRM);
				AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);

				// 중복 로그인이 아닐경우에만 Check_IN이 성공이다.
				if(res.bAlreadyUsedPCCafeIP == FALSE)
				{
					bResult = TRUE;
				}

			} break;
		case Dorian::RESULT_NO_PRODUCT:
			{
				CHAR strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian Checkin result RESULT_NO_PRODUCT - [%s], [%d]\n", 
					req.szUserID, res.nProductID);
				AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);

			} break;
		default:
			{
				CHAR strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian Checkin result error - %s, %s\n", req.szUserID, res.szErrMsg);
				AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);
			} break;
	}

	return bResult;
}

BOOL AuHangameTPack::CheckOut(CHAR* szAccount)
{
	if(!szAccount)
		return FALSE;

	Dorian::REQUEST_CHECKOUT	req;
	Dorian::RESPONSE_CHECKOUT	res;
	memset(&req, 0, sizeof(req));
	memset(&res, 0, sizeof(res));
	memcpy(req.szUserID, szAccount, Dorian::eMAX_USER_ID);

	Dorian::Checkout(&req, &res);
	
	{
		CHAR strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "Dorian Checkout result error - %s, %s\n", req.szUserID, res.szErrMsg);
		AuLogFile_s("LOG\\HangameTPack.txt", strCharBuff);
	}

	return TRUE;
}
#endif //_HANGAME_
#endif