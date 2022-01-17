#include "AgpmCharacter.h"

//돈을 세팅한다.
BOOL AgpmCharacter::SetMoney( INT32 lCID, INT64 llMoney )
{
	return SetMoney( GetCharacter(lCID), llMoney );
}

//돈을 세팅한다.
BOOL AgpmCharacter::SetMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney )
{
	if (!pcsAgpdCharacter)
		return FALSE;

	if (llMoney > AGPDCHARACTER_MAX_INVEN_MONEY)
		llMoney	= AGPDCHARACTER_MAX_INVEN_MONEY;
	else if (llMoney < 0)
		llMoney	= 0;

	pcsAgpdCharacter->m_llMoney = llMoney;

	EnumCallback( ACPMCHAR_CB_ID_UI_SET_MONEY, (PVOID)pcsAgpdCharacter, NULL );
	EnumCallback(AGPMCHAR_CB_ID_UPDATE_MONEY, pcsAgpdCharacter, NULL);

	return TRUE;
}

//현재 가진돈을 얻어낸다.
BOOL AgpmCharacter::GetMoney( INT32 lCID, INT64 *pllMoney )
{
	return GetMoney( GetCharacter(lCID), pllMoney );
}

//현재 가진돈을 얻어낸다.
BOOL AgpmCharacter::GetMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 *pllMoney )
{
	if (!pcsAgpdCharacter || !pllMoney)
		return FALSE;

	*pllMoney = pcsAgpdCharacter->m_llMoney;

	return TRUE;
}

//
// 돈을 추가할 수 있어?
BOOL AgpmCharacter::CheckMoneySpace(INT32 lCID, INT64 llMoney )
{
	return CheckMoneySpace(GetCharacter(lCID), llMoney);
}

BOOL AgpmCharacter::CheckMoneySpace(AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney )
{
	if (pcsAgpdCharacter && (pcsAgpdCharacter->m_llMoney + llMoney <= AGPDCHARACTER_MAX_INVEN_MONEY))
		return TRUE;

	return FALSE;
}

//돈을 추가한다.
BOOL AgpmCharacter::AddMoney( INT32 lCID, INT64 llMoney )
{
	return AddMoney( GetCharacter(lCID), llMoney );
}

//돈을 추가한다.
BOOL AgpmCharacter::AddMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney )
{
	if (pcsAgpdCharacter)
	{
		// 소지 한도보다 크면 소지한도로 만들고,
		if (pcsAgpdCharacter->m_llMoney + llMoney > AGPDCHARACTER_MAX_INVEN_MONEY)
		{
			llMoney	= AGPDCHARACTER_MAX_INVEN_MONEY - pcsAgpdCharacter->m_llMoney;
		}
		// 0보다 작아지면 0으로 만들자.
		else if (pcsAgpdCharacter->m_llMoney + llMoney < 0)
		{
			llMoney	= -pcsAgpdCharacter->m_llMoney;
		}

		pcsAgpdCharacter->m_llMoney += llMoney;

		EnumCallback(AGPMCHAR_CB_ID_UPDATE_MONEY, pcsAgpdCharacter, NULL);

		return TRUE;
	}

	return FALSE;
}

//돈을 뺀다.
BOOL AgpmCharacter::SubMoney( INT32 lCID, INT64 lMoney )
{
	return SubMoney( GetCharacter(lCID), lMoney );
}

//돈을 뺀다.
BOOL AgpmCharacter::SubMoney( AgpdCharacter  *pcsAgpdCharacter, INT64 llMoney )
{
	if (!pcsAgpdCharacter)
		return FALSE;

	if (pcsAgpdCharacter->m_llMoney - llMoney < 0)
		return FALSE;
		
	else if (pcsAgpdCharacter->m_llMoney - llMoney > AGPDCHARACTER_MAX_INVEN_MONEY)
		llMoney	= pcsAgpdCharacter->m_llMoney - AGPDCHARACTER_MAX_INVEN_MONEY;

	pcsAgpdCharacter->m_llMoney -= llMoney;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_MONEY, pcsAgpdCharacter, NULL);

	return TRUE;
}

BOOL AgpmCharacter::SetBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney)
{
	if (!pcsCharacter)
		return FALSE;

	if (llBankMoney > AGPDCHARACTER_MAX_BANK_MONEY)
		llBankMoney	= AGPDCHARACTER_MAX_BANK_MONEY;
	else if (llBankMoney < 0)
		llBankMoney	= 0;

	pcsCharacter->m_llBankMoney = llBankMoney;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_BANK_MONEY, pcsCharacter, NULL);

	return TRUE;
}

INT64 AgpmCharacter::GetBankMoney(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return (-1);

	return pcsCharacter->m_llBankMoney;
}

INT64 AgpmCharacter::AddBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney)
{
	if (!pcsCharacter || llBankMoney < 0)
		return (-1);

	INT64 llAddMoney = llBankMoney;

	if (pcsCharacter->m_llBankMoney + llBankMoney >= AGPDCHARACTER_MAX_BANK_MONEY)
		llAddMoney = AGPDCHARACTER_MAX_BANK_MONEY - pcsCharacter->m_llBankMoney;

	pcsCharacter->m_llBankMoney += llAddMoney;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_BANK_MONEY, pcsCharacter, NULL);

	return llAddMoney;
}

INT64 AgpmCharacter::SubBankMoney(AgpdCharacter *pcsCharacter, INT64 llBankMoney)
{
	if (!pcsCharacter || llBankMoney <= 0)
		return 0;

	INT64 llSubMoney = llBankMoney;

	if (pcsCharacter->m_llBankMoney >= llBankMoney)
	{
		pcsCharacter->m_llBankMoney -= llBankMoney;
		EnumCallback(AGPMCHAR_CB_ID_UPDATE_BANK_MONEY, pcsCharacter, NULL);
	}
	else
	{
		llSubMoney = 0;
	}

	return llSubMoney;
}

BOOL AgpmCharacter::SetCash(AgpdCharacter *pcsCharacter, INT64 llCash)
{
	if (!pcsCharacter)
		return FALSE;

	if (llCash > AGPDCHARACTER_MAX_CASH)
		llCash	= AGPDCHARACTER_MAX_CASH;
	else if (llCash < 0)
		llCash	= 0;

	pcsCharacter->m_llCash = llCash;

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_CASH, pcsCharacter, NULL);

	return TRUE;
}

INT64 AgpmCharacter::GetCash(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return (-1);

	return pcsCharacter->m_llCash;
}

INT64 AgpmCharacter::AddCash(AgpdCharacter *pcsCharacter, INT64 llCash)
{
	if (!pcsCharacter)
		return (-1);

	INT64	llAddMoney = 0;

	if (pcsCharacter->m_llCash + llCash >= AGPDCHARACTER_MAX_CASH)
	{
		llAddMoney = AGPDCHARACTER_MAX_CASH - pcsCharacter->m_llCash;

		pcsCharacter->m_llCash = AGPDCHARACTER_MAX_CASH;
	}
	else
	{
		llAddMoney = llCash;

		pcsCharacter->m_llCash += llCash;
	}

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_CASH, pcsCharacter, NULL);

	return llAddMoney;
}

INT64 AgpmCharacter::SubCash(AgpdCharacter *pcsCharacter, INT64 llCash)
{
	// 2008.01.21. steeple
	// llCash 가 0 원이어도 가능하게 한다. 즉, 공짜 아이템.
	if (!pcsCharacter ||
		llCash < 0)
		return 0;

	INT64	llSubMoney	= 0;

	if (pcsCharacter->m_llCash < llCash)
	{
		return 0;
	}
	else
	{
		llSubMoney = llCash;

		pcsCharacter->m_llCash -= llCash;
	}

	EnumCallback(AGPMCHAR_CB_ID_UPDATE_CASH, pcsCharacter, NULL);

	// llCash 가 0원이어도 성공으로 리턴해주어야 한다. -_-; 이 무슨 코드냐!!!
	if(llSubMoney == 0)
		llSubMoney = 1;

	return llSubMoney;
}

BOOL AgpmCharacter::SetCallbackUpdateMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_MONEY, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_BANK_MONEY, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUpdateCash(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_UPDATE_CASH, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackMoveBankMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMCHAR_CB_ID_MOVE_BANK_MONEY, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackUISetMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ACPMCHAR_CB_ID_UI_SET_MONEY, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackPayTax(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ACPMCHAR_CB_ID_PAY_TAX, pfCallback, pClass);
}

BOOL AgpmCharacter::SetCallbackGetTaxRatio(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ACPMCHAR_CB_ID_GET_TAX_RATIO, pfCallback, pClass);
}

// lBankMoney가 0 보다 큰 경우 인벤토리 -> 뱅크,
// 작은 경우는 뱅크 -> 인벤토리
BOOL AgpmCharacter::OnOperationMoveBankMoney(DispatchArg *pstCheckArg, INT32 lCID, INT64 llBankMoney)
{
	if (lCID == AP_INVALID_CID ||
		llBankMoney == 0 ||
		(!pstCheckArg->bReceivedFromServer &&
		 pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
		 lCID != pstCheckArg->lSocketOwnerID))
		return FALSE;

	AgpdCharacter *pcsCharacter = GetCharacterLock(lCID);
	if (!pcsCharacter)
	{
		return FALSE;
	}

	INT64 llInvenMoney = 0;
	if ( GetMoney(pcsCharacter, &llInvenMoney) )
	{
		if (llBankMoney > 0)
		{
			if (llInvenMoney >= llBankMoney)
			{
				if (SubMoney(pcsCharacter, llBankMoney))
				{
					INT64 llAddMoney = AddBankMoney(pcsCharacter, llBankMoney);
					if (llAddMoney < llBankMoney)
						AddMoney(pcsCharacter, llBankMoney - llAddMoney);

					INT32 lMoney = (INT32) llAddMoney;
					EnumCallback(AGPMCHAR_CB_ID_MOVE_BANK_MONEY, pcsCharacter, &lMoney);

					return pcsCharacter->m_Mutex.Release();
				}
			}
		}
		else
		{
			// 창고 금액보다 많은 금액을 출금하려고 할때
			INT64 llCurrentBankMoney = GetBankMoney(pcsCharacter);
			if (llCurrentBankMoney >= -llBankMoney)
			{
				//출금으로 인해 소지 금액이 초과되는 경우
				if ( llInvenMoney + (-llBankMoney) <= AGPDCHARACTER_MAX_INVEN_MONEY )
				{
					// bank에서 inventory로 이동하는 경우
					INT64 llSubMoney = SubBankMoney(pcsCharacter, -llBankMoney);
					if (llSubMoney > 0)
					{
						AddMoney(pcsCharacter, llSubMoney);

						INT32 lMoney = (INT32) llSubMoney;
						lMoney = -lMoney;
						EnumCallback(AGPMCHAR_CB_ID_MOVE_BANK_MONEY, pcsCharacter, &lMoney);

						return pcsCharacter->m_Mutex.Release();
					}
				}
			}
		}
	}

	pcsCharacter->m_Mutex.Release();
	return FALSE;
}

BOOL AgpmCharacter::PayTax(AgpdCharacter *pcsCharacter, INT64 llTax)
{
	if (!pcsCharacter || 0 > llTax)
		return FALSE;
	
	INT16 nBindingRegion = m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x ,pcsCharacter->m_stPos.z);
	ApmMap::RegionTemplate *pRegionTemplate	= m_pcsApmMap->GetTemplate(nBindingRegion);
	if (!pRegionTemplate)
		return FALSE;
	
	return EnumCallback(ACPMCHAR_CB_ID_PAY_TAX, pRegionTemplate->pStrName, &llTax);
}

INT32 AgpmCharacter::GetTaxRatio(AgpdCharacter *pcsCharacter)
{
	INT32 lRatio = 0;
	if (!pcsCharacter)
		return lRatio;
	
	INT32 lParamRatio = 0;
	INT16 nBindingRegion = m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x ,pcsCharacter->m_stPos.z);
	ApmMap::RegionTemplate *pRegionTemplate	= m_pcsApmMap->GetTemplate(nBindingRegion);
	if (pRegionTemplate &&
		EnumCallback(ACPMCHAR_CB_ID_GET_TAX_RATIO, pRegionTemplate->pStrName, &lParamRatio))
		lRatio = lParamRatio;

	return lRatio;		
}