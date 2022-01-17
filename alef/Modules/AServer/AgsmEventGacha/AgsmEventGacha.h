#pragma once

#include "AgpmEventGacha.h"
#include "AgsmCharacter.h"
#include "AgpdDropItem2.h"
#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"

class AgsmEventGacha : public AgsModule
{
public:
	AgpmCharacter		*m_pcsAgpmcharacter;
	AgpmItem			*m_pcsAgpmItem;
	AgpmDropItem2		*m_pcsAgpmDropItem2;
	AgpmEventGacha		*m_pcsAgpmEventGacha;

	AgsmCharacter		*m_pcsAgsmCharacter;
	AgsmItem			*m_pcsAgsmItem;
	AgsmItemManager		*m_pcsAgsmItemManager;

public:
	AgsmEventGacha();
	virtual ~AgsmEventGacha();

	// ... ApModule inherited		
	BOOL OnAddModule();

	BOOL IsGachaBlocked(AgpdCharacter* pcsCharacter);

	INT32	ProcessGacha( AgpdCharacter * pcsCharacter , INT32 nGachaType );
	// 리턴값은 성공한 Item TID
	// 가차 비용 지불함. 위에 함수와 이중적인 검사를함.
	// 아이템을 넣어주는 과정까지..

	static BOOL CBCheckAllBlock(PVOID pData, PVOID pClass, PVOID pCustData);

	// ... Callback
	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGacha(PVOID pData, PVOID pClass, PVOID pCustData);

	// 가챠 아이템 일정한 시간뒤에 넣어주기 위한 콜백.
	static BOOL	CBGachaItemUpdate(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
};
