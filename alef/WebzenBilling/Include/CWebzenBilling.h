#ifndef __CLASS_WEBZEN_BILLING_H__
#define __CLASS_WEBZEN_BILLING_H__




#include "LibClientSession.h"




enum eBillingNoticeType
{
	BillingNotice_Login,
	BillingNotice_AccountCheck,
	BillingNotice_AccountType,
};

class CWebzenBilling : public CLibClientSession
{
private :
	char										m_strServerAddress[ 64 ];
	int											m_nServerPort;

	int											m_nGameServerType;

	BOOL										m_bIsStarted;
	BOOL										m_bIsLogined;

	DWORD										m_dwAccountGUID;
	long										m_nBillingGUID;

	DWORD										m_dwEndDate;
	DWORD										m_dwEndDateReal;

	double										m_dRemainPoint;
	double										m_dRemainTime;

	short										m_nUserType;

public :
	CWebzenBilling( void );
	virtual ~CWebzenBilling( void );

public :
	BOOL			OnCreateSession				( void );
	BOOL			OnWebzenBillingConnect		( char* pServerAddress, int nPort );
	BOOL			OnWebzenBillingDisConnect	( void );

	BOOL			OnWebzenBillingLogin		( DWORD dwAccountID, long nGameCode, DWORD dwUserIP, DWORD dwPCBangID );
	BOOL			OnWebzenBillingLogout		( void );

	BOOL			OnRequestUserInfo			( void );
	BOOL			OnCheckLoginStatus			( void );

public :
	virtual void	OnNetConnect				( bool bIsSuccess, DWORD dwError );
	virtual void	OnNetClose					( DWORD dwError );
	virtual void	OnNetSend					( int nSendSize );

	virtual void	OnUserStatus				( DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode );

public :
	static void		fnErrorHandler				( DWORD dwLastError, TCHAR* pErrorMsg );

private :
	void			_NotifyMessage				( eBillingNoticeType eType, short nNoticeCode );

public :
	char*			GetBillingServerAddress		( void ) { return m_strServerAddress; }
	int				GetBillingServerPort		( void ) { return m_nServerPort; }

	int				GetServerType				( void ) { return m_nGameServerType; }

	BOOL			IsBillingStarted			( void ) { return m_bIsStarted; }
	BOOL			IsLogin						( void ) { return m_bIsLogined; }

	DWORD			GetEndDate					( void ) { return m_dwEndDate; }
	DWORD			GetEndDateReal				( void ) { return m_dwEndDateReal; }

	double			GetRemainPoint				( void ) { return m_dRemainPoint; }
	double			GetRemainTime				( void ) { return m_dRemainTime; }

	short			GetUserAccountType			( void ) { return m_nUserType; }
};




#endif