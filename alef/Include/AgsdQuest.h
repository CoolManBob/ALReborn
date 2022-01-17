#pragma once

#include "ApBase.h"

// AGSDQUEST_EXPAND_BLOCK
const INT32 AGSDQUEST_MAX_FLAG		= 1024;
const INT32 AGSDQUEST_BITS			= 8;
// AGSDQUEST_EXPAND_BLOCK
	const INT32 AGSDQUEST_BLOCK_COUNT			= 2;

class AgsdQuest
{
private:
	BYTE	m_btQuest[AGSDQUEST_MAX_FLAG];
// AGSDQUEST_EXPAND_BLOCK
	BYTE	m_btQuestSE[AGSDQUEST_MAX_FLAG];

public:
	AgsdQuest();
	~AgsdQuest();

	// ID가 유효한 범위내에 있는지 확인
	BOOL IsValid(INT32 lQuestDBID);

	// ID에 해당하는 Bit의 값을 가져온다.
	BOOL GetValueByDBID(INT32 lQuestDBID);

	// ID에 해당하는 Bit의 값을 설정
	BOOL SetValueByDBID(INT32 lQuestDBID, BOOL bMaster);

/*
public:
	// 퀘스트의 ID를 이용하여 배열에 접근하듯이 쓸수있게 하기위해
	// Proxy Class를 해결책으로 선택
	class AgpdQuestProxy
	{
	private:
		BYTE *m_pByte;
		INT32 m_lQuestID;
		BOOL m_bValid;		// AgpdQuest의 [] 연산자에서 AgpdQuestProxy의 인스턴스를 리턴해야 하기 때문에
							// Quest ID를 잘못 넣어도 알려줄수 있는 해결책이 없다(exception은 제외). 
							// 그래서 현재의 데이터가 유효한지를 확인할수 있는 m_bValid를 추가
							// 깔끔하지 않은 해결책.....

	public:
		AgpdQuestProxy(BYTE* pByte, INT32 lQuestID, BOOL bValid);
		AgpdQuestProxy& operator=(const AgpdQuestProxy& rProxy);
		AgpdQuestProxy& operator=(BOOL bMaster);
		operator BOOL() const;

		BOOL IsValid()	{return m_bValid;}
	};

public:

	// []를 이용하여 ID에 해당하는 Bit의 값을 가져온다.
	AgpdQuestProxy operator[](INT32 lQuestID);

	friend class AgpdQuestProxy;
*/
};