#include "CallBackFunc.h"
#include "AgsmBillingChina.h"
#include "AgsmCashMall.h"

#include <iostream>

using std::cout;
using std::endl;

CCallBackFunc::CCallBackFunc()
{
}

CCallBackFunc::~CCallBackFunc()
{
}

void CCallBackFunc::PCallbackAuthorRes(GSBsipAuthorResDef *p )
{
} 

void CCallBackFunc::PCallbackAccountRes(GSBsipAccountResDef *p )
{
}

void CCallBackFunc::PCallbackAccountLockRes(GSBsipAccountLockResDef *p )
{
    //cout<<"\nBsipAccountLockResDef.result:"<<p->result;
    //cout<<"\nBsipAccountLockResDef.sd_id:"<<p->sd_id;
    //cout<<"\nBsipAccountLockResDef.pt_id:"<<p->pt_id;
    //cout<<"\nBsipAccountLockResDef.sess_id:"<<p->sess_id;
    //cout<<"\nBsipAccountLockResDef.pay_type:"<<p->pay_type;
    //cout<<"\nBsipAccountLockResDef.pt_balance:"<<p->pt_balance;
    //cout<<"\nBsipAccountLockResDef.order_id:"<<p->order_id;
    //cout<<"\nBsipAccountLockResDef.ip:"<<p->ip;
    //cout<<"\nBsipAccountLockResDef.log_time:"<<p->log_time;
    //cout<<"\nBsipAccountLockResDef.item_num:"<<p->item_num;
    //cout<<"\nBsipAccountLockResDef.item0_id:"<<p->item0_id;
    //cout<<"\nBsipAccountLockResDef.item0_num:"<<p->item0_num;
    //cout<<"\nBsipAccountLockResDef.item0_price:"<<p->item0_price;
    //cout<<"\nBsipAccountLockResDef.item1_id:"<<p->item1_id;
    //cout<<"\nBsipAccountLockResDef.item1_num:"<<p->item1_num;
    //cout<<"\nBsipAccountLockResDef.item1_price:"<<p->item1_price;
    //cout<<"\nBsipAccountLockResDef.item2_id:"<<p->item2_id;
    //cout<<"\nBsipAccountLockResDef.item2_num:"<<p->item2_num;
    //cout<<"\nBsipAccountLockResDef.item2_price:"<<p->item2_price;
    //cout<<"\nBsipAccountLockResDef.item3_id:"<<p->item3_id;
    //cout<<"\nBsipAccountLockResDef.item3_num:"<<p->item3_num;
    //cout<<"\nBsipAccountLockResDef.item3_price:"<<p->item3_price;
    //cout<<"\nBsipAccountLockResDef.item4_id:"<<p->item4_id;
    //cout<<"\nBsipAccountLockResDef.item4_num:"<<p->item4_num;
    //cout<<"\nBsipAccountLockResDef.item4_price:"<<p->item4_price;

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	{
		AgsdBillingItem *pcsBilling = dynamic_cast<AgsdBillingItem*>(g_billingBridge.Find(p->order_id));
		if (!pcsBilling) return;

		AgpdCharacter *pcsCharacter = pcsBilling->m_pAgpdCharacter;
		if (!pcsCharacter)
		{
			g_billingBridge.Remove(p->order_id);

			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();
			return;
		}

		if (0 != p->result)
		{
			g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
			g_billingBridge.Remove(p->order_id);

			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();
			return;
		}

		pAgpmCashItemInfo pCashItemInfo	= g_billingBridge.GetAgpmCashMall()->GetCashItem(pcsBilling->m_lProductID);
		if (!pCashItemInfo)
		{
			g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);

			g_billingBridge.Remove(p->order_id);
			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();
			return;
		}
		
		// 중국은 일단 세트아이템 지원 안하고 첫번째것만 지급한다.
		AgpdItem *pcsItem = g_billingBridge.GetAgsmItemManager()->CreateItem(pCashItemInfo->m_alItemTID[0],
																   pcsCharacter,
																   pCashItemInfo->m_alItemQty[0]);
		if (!pcsItem)
		{
			g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);

			g_billingBridge.Remove(p->order_id);
			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();
			return;
		}

		// insert item into cash inventory
		if (!g_billingBridge.GetAgpmItem()->AddItemToCashInventory(pcsCharacter, pcsItem))
		{
			g_billingBridge.GetAgpmItem()->RemoveItem(pcsItem, TRUE);
			g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
			
			g_billingBridge.Remove(p->order_id);
			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();
			return;
		}

		if (!g_billingBridge.GetAgpmChar()->SubCash(pcsCharacter, pCashItemInfo->m_llPrice))
		{
			g_billingBridge.GetAgpmItem()->RemoveItem(pcsItem, TRUE);
			g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);

			g_billingBridge.Remove(p->order_id);
			g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
			pcsBilling->Release();

			return;
		}

		// log
		g_billingBridge.GetAgsmBilling()->m_pAgsmBillingChina->AccountLockResLog(p->pt_id, p->ip, p->item0_id, p->item0_price, p->order_id);

		AgsdItem *pcsAgsdItem = g_billingBridge.GetAgsmItem()->GetADItem(pcsItem);
		g_billingBridge.GetAgsmItem()->UpdateCashItemBuyList_Complete2(pcsAgsdItem->m_ullDBIID, p->order_id, pcsBilling->m_ullListSeq);

		g_billingBridge.GetAgsmItem()->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_BUY, pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount, (INT32)pCashItemInfo->m_llPrice);

		g_billingBridge.GetAgsmCashMall()->SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_SUCCESS);

		// AccountUnlock
		g_billingBridge.GetAgsmBilling()->m_pAgsmBillingChina->SendAccountUnlock(p->pt_id, p->ip, p->item0_id, p->item0_price, p->order_id);
		g_billingBridge.GetAgsmBilling()->m_pAgsmBillingChina->AccountUnlockLog(p->pt_id, p->ip, p->item0_id, p->item0_price, p->order_id);

		// remove temp billing info
		g_billingBridge.Remove(p->order_id);
		g_billingBridge.GetAgsmBilling()->Remove(pcsBilling->m_lID);
		g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pcsBilling->m_lID);
		pcsBilling->Release();
	}
}

void CCallBackFunc::PCallbackAccountUnlockRes(GSBsipAccountUnlockResDef *p )
{
}

void CCallBackFunc::PCallbackAccountLockExRes(GSBsipAccountLockExResDef *p )
{
}

void CCallBackFunc::PCallbackAccountUnlockExRes(GSBsipAccountUnlockExResDef * p )
{
}

void CCallBackFunc::PCallbackAwardAckRes(GSBsipAwardAckResDef *p )
{
}

void CCallBackFunc::PCallbackGoldConsumeLockRes(GSBsipGoldConsumeLockResDef * p )
{
}

void CCallBackFunc::PCallbackGoldConsumeUnlockRes(GSBsipGoldConsumeUnlockResDef* p)
{
}

void CCallBackFunc::PCallbackDepositAckRes(GSBsipDepositAckResDef* p)
{
}

void CCallBackFunc::PCallbackDepositRes(GSBsipDepositResDef *p )
{
}

void CCallBackFunc::PCallbackAwardAuthenRes(GSBsipAwardAuthenResDef *p )
{
}

void CCallBackFunc::PCallbackGoldDepositReq(GSBsipGoldDepositReqDef *p )
{
}

void CCallBackFunc::PCallbackConsignLockRes(GSBsipConsignLockResDef * p )
{
}

void CCallBackFunc::PCallbackConsignUnlockRes(GSBsipConsignUnlockResDef *p )
{
}

void CCallBackFunc::PCallbackConsignDepositRes(GSBsipConsignDepositResDef *p )
{
}

void CCallBackFunc::PCallbackAccountAuthenRes(GSBsipAccountAuthenResDef * p)
{
	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	{
		AgsdBilling* pBilling = g_billingBridge.Find(p->unique_id);
		if (pBilling)
		{
			g_billingBridge.GetAgsmCashMall()->m_pcsAgpmCharacter->SetCash(pBilling->m_pAgpdCharacter, p->balance1);
			pBilling->Release();
			g_billingBridge.Remove(p->unique_id);
			g_billingBridge.GetAgsmBilling()->Remove(pBilling->m_lID);
			g_billingBridge.GetAgsmBilling()->m_GenerateID.AddRemoveID(pBilling->m_lID);

			g_billingBridge.GetAgsmBilling()->m_pAgsmBillingChina->AccountAuthenResLog(p->pt_id, p->balance1);
		}
	}
}

void CCallBackFunc::PCallbackNotifyRes(GSBsipNotifyResDef * p)
{
}

void CCallBackFunc::PCallbackREAlert( GSBsipAlertMsg *p )
{
}
