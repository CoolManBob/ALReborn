#pragma once
#include <string>

class AgsmBillingJapan
{
public:
	AgsmBillingJapan( void );
	bool InitBillingModule( void );
	const char* GetBillingNumber( const char* accountId );
	bool GetBalance(const char* account, const char* billingNum, int& balance);
	bool BuyItem(const char* account,		// Account
				 const char* billingNum,	// User billing Number
				 const char* itemCode,		// Item Code
				 const char* itemName,		// Item Name
				 int price,					// Item Price
				 const char* clientip,		// Client IP
				 char* orderString);

	void SetOrderSeed(const char* order) { m_order = order; }

private:
	void PrintError(int errCode);

private:
	bool m_isDebugTest;
	long m_orderSequence;
	std::string m_order;
	char m_szBillNo[32]; //처리를 위한 임시값임 요청올때마다 수시로 바뀜. supertj@090116
};
