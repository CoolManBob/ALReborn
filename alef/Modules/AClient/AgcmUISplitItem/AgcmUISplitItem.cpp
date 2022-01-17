#include "AgcmUISplitItem.h"
#include "AuStrTable.h"

AgcmUISplitItem::AgcmUISplitItem()
{
	SetModuleName("AgcmUISplitItem");

	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgcmItem = NULL;
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgcmCharacter = NULL;

	m_lMinStackCount = 0; 
	m_lMaxStackCount = 0;

	m_lStackCount = 0;

	m_bIsModalStatus = FALSE;
	m_eTitleCondition = SplitItem_Item;
	m_lStatus = AGPDITEM_STATUS_NONE;

	ZeroMemory(m_szTitle,sizeof(m_szTitle));

	m_fpCallbackFunction = NULL;
	m_pvClass = NULL;

	m_lDummyData = 0;

	m_pcsUserDataDummy = NULL;

	m_lEventOpenSplitItem = 0;
	m_lEventGetStackCountEditControl = 0;
	m_lEventChangeSplitItemTitle = 0;
	m_lEventChangeSplitItemTitle = 0;
}

AgcmUISplitItem::~AgcmUISplitItem()
{
}

BOOL AgcmUISplitItem::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");

	m_pcsAgcmItem			= (AgcmItem *)			GetModule("AgcmItem");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	
	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgcmItem ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmCharacter)
		return FALSE;

	if (!AddDisplay())
		return FALSE;
	if (!AddUserData())
		return FALSE;
	if (!AddFunction())
		return FALSE;
	if (!AddEvent())
		return FALSE;

	return TRUE;
}

BOOL AgcmUISplitItem::OnInit()
{
	m_pcsAgcmUISystemMessage = (AgcmUISystemMessage*)GetModule("AgcmUISystemMessage");

	return TRUE;
}

BOOL AgcmUISplitItem::AddDisplay()
{
	return TRUE;
}

BOOL AgcmUISplitItem::AddUserData()
{
	m_pcsUserDataDummy = m_pcsAgcmUIManager2->AddUserData("SplitItemDummy", &m_lDummyData, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pcsUserDataDummy)
		return FALSE;

	return TRUE;
}

BOOL AgcmUISplitItem::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonOK", CBButtonOK, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonCancel", CBButtonCancel, 0))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonUp", CBButtonUp, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonDown", CBButtonDown, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey0", CBButtonKey0, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey1", CBButtonKey1, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey2", CBButtonKey2, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey3", CBButtonKey3, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey4", CBButtonKey4, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey5", CBButtonKey5, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey6", CBButtonKey6, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey7", CBButtonKey7, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey8", CBButtonKey8, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKey9", CBButtonKey9, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKeyClear", CBButtonKeyClear, 1, "StackCount Edit Control"))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ButtonKeyBack", CBButtonKeyBack, 1, "StackCount Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_GetStackCountEditControl", CBGetStackCountEditControl, 1, "StackCount Edit Control"))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SplitItem_ChangeTitleEditControl", CBChangeTitleEditControl, 1, "Title Edit Countrol"))
		return FALSE;

	return TRUE;
}

BOOL AgcmUISplitItem::AddEvent()
{
	m_lEventOpenSplitItem = m_pcsAgcmUIManager2->AddEvent("SplitItem_OpenUI");
	if (m_lEventOpenSplitItem < 0)
		return FALSE;

	m_lEventGetStackCountEditControl = m_pcsAgcmUIManager2->AddEvent("SplitItem_GetStackCountControl");
	if (m_lEventGetStackCountEditControl < 0)
		return FALSE;

	m_lEventChangeSplitItemTitle = m_pcsAgcmUIManager2->AddEvent("SplitItem_ChangeTitle");
	if (m_lEventChangeSplitItemTitle < 0)
		return FALSE;

	m_lEventCloseUI = m_pcsAgcmUIManager2->AddEvent("SplitItem_CloseUI");
	if (m_lEventCloseUI < 0)
		return FALSE;

	return TRUE;
}

BOOL AgcmUISplitItem::SetCondition(eSplitItemCondition titleCondition, ApModuleDefaultCallBack fpCallback, PVOID pvClass,
								   INT32 lMinStackCount, INT32 lMaxStackCount, INT32 lStartCount, BOOL bIsModalStatus, INT32 lStatus)
{
	if (!fpCallback)
		return FALSE;

	//ZeroMemory(m_szTitle, sizeof(CHAR) * (AGCMUISPLITITEM_MAX_TITLE + 1));

	//if (lTitleLength < AGCMUISPLITITEM_MAX_TITLE)
	//	CopyMemory(m_szTitle, szTitle, sizeof(CHAR) * lTitleLength);
	//else
	//	CopyMemory(m_szTitle, szTitle, sizeof(CHAR) * AGCMUISPLITITEM_MAX_TITLE);

	m_fpCallbackFunction	= fpCallback;
	m_pvClass				= pvClass;

	m_lMinStackCount		= lMinStackCount;
	m_lMaxStackCount		= lMaxStackCount;
	m_lStackCount			= lStartCount;

	m_bIsModalStatus		= bIsModalStatus;
	m_eTitleCondition		= titleCondition;
	m_lStatus				= lStatus;

	return TRUE;
}

BOOL AgcmUISplitItem::OpenSplitItemUI()
{
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsUserDataDummy);

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventChangeSplitItemTitle);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventGetStackCountEditControl);
	m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenSplitItem);

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_fpCallbackFunction && pcsEditControl->m_pcsBase)
	{
		const CHAR	*szPrice	= ((AcUIEdit *) pcsEditControl->m_pcsBase)->GetText();

		if (szPrice && szPrice[0])
			pThis->m_lStackCount	= atoi(szPrice);

		if (pThis->m_lStackCount > pThis->m_lMaxStackCount)
			pThis->m_lStackCount	= pThis->m_lMaxStackCount;
		else if (pThis->m_lStackCount < pThis->m_lMinStackCount)
			pThis->m_lStackCount	= pThis->m_lMinStackCount;

		if (SplitItem_Item != pThis->m_eTitleCondition)
		{
			const INT64 llRegisterFee = INT64(pThis->m_lStackCount * AGPMITEM_REGISTER_FEE_RATIO);
			char tempBuffer[256] = {0, };
			sprintf(tempBuffer, ClientStr().GetStr(STI_SELL_WARNING), pThis->m_lStackCount, llRegisterFee);

			// 정보 요약해서 보여주기
			if (1 == pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog(tempBuffer))
			{
				INT64 llInvenMoney = 0;
				pThis->m_pcsAgpmCharacter->GetMoney( pThis->m_pcsAgcmCharacter->m_lSelfCID, &llInvenMoney );

				// 수수료 부족
				if (llInvenMoney < llRegisterFee)
				{
					pThis->m_eTitleCondition = SplitItem_SalesWrong;
					pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventChangeSplitItemTitle);
					pThis->m_pcsAgcmUISystemMessage->AddSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage( "NotEnoughCharge" ), 0xffffffff );

					return TRUE;
				}
			}
			else
				return TRUE;
		}
	
		pThis->m_fpCallbackFunction((PVOID) &pThis->m_lStackCount, pThis->m_pvClass, &pThis->m_lStatus);
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseUI);

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	((AgcmUISplitItem *)pClass)->m_lStackCount = 0;

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount < pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		++pThis->m_lStackCount;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount > pThis->m_lMinStackCount &&
		pcsEditControl->m_pcsBase)
	{
		--pThis->m_lStackCount;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey0(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount *= 10;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey1(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 1 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 1;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey2(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 2 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 2;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey3(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 3 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 3;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey4(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 4 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 4;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey5(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 5 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 5;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey6(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 6 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 6;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey7(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 7 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 7;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey8(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 8 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 8;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKey9(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount * 10 + 9 <= pThis->m_lMaxStackCount &&
		pcsEditControl->m_pcsBase)
	{
		pThis->m_lStackCount = pThis->m_lStackCount * 10 + 9;

		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKeyClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	pThis->m_lStackCount	= 0;

	if (pcsEditControl->m_pcsBase)
	{
		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBButtonKeyBack(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pThis->m_lStackCount > 0 &&
		(INT32) (pThis->m_lStackCount / 10) >= pThis->m_lMinStackCount)
	{
		pThis->m_lStackCount = (INT32) (pThis->m_lStackCount / 10);

		if (pcsEditControl->m_pcsBase)
		{
			CHAR	szBuffer[16];
			ZeroMemory(szBuffer, sizeof(CHAR) * 16);

			sprintf(szBuffer, "%d", pThis->m_lStackCount);

			((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);
		}
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBGetStackCountEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem			*pThis				= (AgcmUISplitItem *)	pClass;
	AgcdUIControl			*pcsEditControl		= (AgcdUIControl *)		pData1;

	if (pcsEditControl->m_pcsBase)
	{
		CHAR	szBuffer[16];
		ZeroMemory(szBuffer, sizeof(CHAR) * 16);

		sprintf(szBuffer, "%d", pThis->m_lStackCount);

		((AcUIEdit *) pcsEditControl->m_pcsBase)->SetText(szBuffer);

		pcsEditControl->m_pcsParentUI->m_pcsUIWindow->m_Property.bModal	= pThis->m_bIsModalStatus;
	}

	return TRUE;
}

BOOL AgcmUISplitItem::CBChangeTitleEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUISplitItem *pThis = (AgcmUISplitItem*)pClass;
	AcUIEdit *pec = (AcUIEdit *)((AgcdUIControl*)pData1)->m_pcsBase;

	if (SplitItem_Item == pThis->m_eTitleCondition)
		pec->SetText(pThis->m_pcsAgcmUIManager2->GetUIMessage("Item_Split_Stack_Count"));
	else if (SplitItem_SalesTrue == pThis->m_eTitleCondition)
		pec->SetText(ClientStr().GetStr(STI_INPUT_SELL_PRICE));
	else
	{
		pec->SetText(ClientStr().GetStr(STI_LACK_OF_MONEY));
		pThis->m_eTitleCondition = SplitItem_SalesTrue;
	}

	return TRUE;
}