#include "BillAPI.h"

#include <Imagehlp.h>

#include <stdio.h>

// ?
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
CBillAPI::CBillAPI()
{
#ifdef WIN64
	#ifdef EM64T
		m_fileName = "bsipclient_em64t.dll";
	#else
		m_fileName = "bsipclient_amd64.dll";
	#endif
#else
	#ifdef _DEBUG
		m_fileName = "bsipclientd.dll";
	#else
		m_fileName = "bsipclient.dll";
	#endif
#endif
}

CBillAPI::~CBillAPI()
{
}

int CBillAPI::IntSockEnv()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	
	wVersionRequested = MAKEWORD( 2, 2 ); 
	if (0 != WSAStartup( wVersionRequested, &wsaData ))
	{
		return BSIP_ERROR;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return BSIP_ERROR;
	}
	return BSIP_OK;
}

int CBillAPI::LoadDll()
{
	PLOADED_IMAGE	pImage	= ImageLoad((PSTR) m_fileName.c_str(), NULL);

	m_hDll = LoadLibrary(m_fileName.c_str());
	if(m_hDll==NULL )
	{
		printf("Dll file load fail: %s\n", m_fileName.c_str());
		return BSIP_ERROR;
	}

	return BSIP_OK;
}

int CBillAPI::LoadDll(std::string fileName)
{
	m_fileName = fileName;
	return LoadDll();
}

int CBillAPI::GetFunc()
{
	if(m_hDll==NULL )
	{
		return BSIP_ERROR;
	}
	m_pGSInitializeEx = (FunGSInitializeEx)GetProcAddress(m_hDll, "GSInitializeEx"); 
	m_pGSInitialize = (FunGSInitialize)GetProcAddress(m_hDll, "GSInitialize");             
	m_pUninitialize =  (FunUninitialize)GetProcAddress(m_hDll, "Uninitialize");                
	m_pSendAuthorRequest  =  (FunSendAuthorRequest)GetProcAddress(m_hDll, "SendAuthorRequest");           
	m_pSendAccountRequest  =  (FunSendAccountRequest)GetProcAddress(m_hDll, "SendAccountRequest");          
	m_pSendAccountAuthenRequest =  (FunSendAccountAuthenRequest)GetProcAddress(m_hDll, "SendAccountAuthenRequest");     
	m_pSendAccountLockRequest =  (FunSendAccountLockRequest)GetProcAddress(m_hDll, "SendAccountLockRequest");       
	m_pSendAccountUnlockRequest  =  (FunSendAccountUnlockRequest)GetProcAddress(m_hDll, "SendAccountUnlockRequest");    
	m_pSendAccountLockExRequest  =  (FunSendAccountLockExRequest)GetProcAddress(m_hDll, "SendAccountLockExRequest");    
	m_pSendAccountUnlockExRequest =  (FunSendAccountUnlockExRequest)GetProcAddress(m_hDll, "SendAccountUnlockExRequest");   
	m_pSendAwardAuthenRequest  =  (FunSendAwardAuthenRequest)GetProcAddress(m_hDll, "SendAwardAuthenRequest");      
	m_pSendAwardAck  =  (FunSendAwardAck)GetProcAddress(m_hDll, "SendAwardAck");                
	m_pSendConsignLockRequest =  (FunSendConsignLockRequest)GetProcAddress(m_hDll, "SendConsignLockRequest");       
	m_pSendConsignUnlockRequest =  (FunSendConsignUnlockRequest)GetProcAddress(m_hDll, "SendConsignUnlockRequest");     
	m_pSendConsignDepositRequest =  (FunSendConsignDepositRequest)GetProcAddress(m_hDll, "SendConsignDepositRequest");    
	m_pSendGoldDepositResponse  =  (FunSendGoldDepositResponse)GetProcAddress(m_hDll, "SendGoldDepositResponse");     
	m_pSendGoldConsumeLockRequest =  (FunSendGoldConsumeLockRequest)GetProcAddress(m_hDll, "SendGoldConsumeLockRequest");   
	m_pSendGoldConsumeUnlockRequest=  (FunSendGoldConsumeUnlockRequest)GetProcAddress(m_hDll, "SendGoldConsumeUnlockRequest");  
	m_pSendDepositRequest   =  (FunSendDepositRequest)GetProcAddress(m_hDll, "SendDepositRequest");         
	m_pSendDepositAck  =  (FunSendDepositAck)GetProcAddress(m_hDll, "SendDepositAck");              
	m_pSendNotifyRequest  =  (FunSendNotifyRequest)GetProcAddress(m_hDll, "SendNotifyRequest"); 
	
	m_pSendLoginRequest	=   (FunPlayerLogin)GetProcAddress(m_hDll, "Login"); 
	m_pSendLogoutRequest =	(FunPlayerLogout)GetProcAddress(m_hDll, "Logout");
	m_pSendReLoginRequest	= (FunPlayerReLogin)GetProcAddress(m_hDll, "ReLogin");
	
	m_pGetNewId  =  (FunGetNewId)GetProcAddress(m_hDll, "GetNewId");                   
	m_pGetNewIdByParam =  (FunGetNewIdByParam)GetProcAddress(m_hDll, "GetNewIdByParam");
	if( 
		(m_pGSInitializeEx				==NULL)||
		(m_pGSInitialize                ==NULL)||
		(m_pUninitialize                ==NULL)||
		(m_pSendAuthorRequest           ==NULL)||
		(m_pSendAccountRequest          ==NULL)||
		(m_pSendAccountAuthenRequest    ==NULL)||
		(m_pSendAccountLockRequest      ==NULL)||
		(m_pSendAccountUnlockRequest    ==NULL)||
		(m_pSendAccountLockExRequest    ==NULL)||
		(m_pSendAccountUnlockExRequest  ==NULL)||
		(m_pSendAwardAuthenRequest      ==NULL)||
		(m_pSendAwardAck                ==NULL)||
		(m_pSendConsignLockRequest      ==NULL)||
		(m_pSendConsignUnlockRequest    ==NULL)||
		(m_pSendConsignDepositRequest   ==NULL)||
		(m_pSendGoldDepositResponse     ==NULL)||
		(m_pSendGoldConsumeLockRequest  ==NULL)||
		(m_pSendGoldConsumeUnlockRequest==NULL)||
		(m_pSendDepositRequest          ==NULL)||
		(m_pSendDepositAck              ==NULL)||
		(m_pSendNotifyRequest           ==NULL)||
		(m_pSendLoginRequest			==NULL)||
		(m_pSendLogoutRequest			==NULL)||
		(m_pSendReLoginRequest			==NULL)||
		(m_pGetNewId                    ==NULL)||
		(m_pGetNewIdByParam             ==NULL)
	)
	{
		printf("Can't get process address\n");
		return BSIP_ERROR;
	}
	return BSIP_OK;;
}

int CBillAPI::SetCallBackFunc()
{
	m_funCallBack.CallbackAuthorRes 		=	CCallBackFunc::PCallbackAuthorRes;
	m_funCallBack.CallbackAccountRes		=	CCallBackFunc::PCallbackAccountRes;
	m_funCallBack.CallbackAccountAuthenRes	=	CCallBackFunc::PCallbackAccountAuthenRes;
	m_funCallBack.CallbackAccountLockRes	=	CCallBackFunc::PCallbackAccountLockRes;
	m_funCallBack.CallbackAccountUnlockRes	=	CCallBackFunc::PCallbackAccountUnlockRes;
	m_funCallBack.CallbackAccountLockExRes	=	CCallBackFunc::PCallbackAccountLockExRes;
	m_funCallBack.CallbackAccountUnlockExRes=	CCallBackFunc::PCallbackAccountUnlockExRes;
	m_funCallBack.CallbackAwardAuthenRes	=	CCallBackFunc::PCallbackAwardAuthenRes;
	m_funCallBack.CallbackAwardAckRes		=	CCallBackFunc::PCallbackAwardAckRes;
	m_funCallBack.CallbackConsignLockRes	=	CCallBackFunc::PCallbackConsignLockRes;
	m_funCallBack.CallbackConsignUnlockRes	=	CCallBackFunc::PCallbackConsignUnlockRes;
	m_funCallBack.CallbackConsignDepositRes	=	CCallBackFunc::PCallbackConsignDepositRes;
	m_funCallBack.CallbackGoldDepositReq	=	CCallBackFunc::PCallbackGoldDepositReq;
	
	m_funCallBack.CallbackGoldConsumeLockRes	=	CCallBackFunc::PCallbackGoldConsumeLockRes;
	m_funCallBack.CallbackGoldConsumeUnlockRes	=	CCallBackFunc::PCallbackGoldConsumeUnlockRes;

	m_funCallBack.CallbackDepositRes	=	CCallBackFunc::PCallbackDepositRes;
	m_funCallBack.CallbackDepositAckRes	=	CCallBackFunc::PCallbackDepositAckRes;
	m_funCallBack.CallbackNotifyRes		=	CCallBackFunc::PCallbackNotifyRes;
	
	m_funCallbackBEAlert.CallbackAlertRes = CCallBackFunc::PCallbackREAlert;

	return BSIP_OK;
}

int CBillAPI::FreeDll()
{
	if( (m_hDll==NULL) || !FreeLibrary(m_hDll))
		return BSIP_ERROR;
	
	m_hDll = NULL;
	return BSIP_OK;
}
