/******************************************************************************
Module:  AgpmEventFactors.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 12
******************************************************************************/

#include "AgpmEventFactors.h"

AgpmEventFactors::AgpmEventFactors()
{
	SetModuleName("AgpmEventFactors");
}

AgpmEventFactors::~AgpmEventFactors()
{
}

BOOL AgpmEventFactors::OnAddModule()
{
	m_papmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");

	if (!m_papmEventManager || !m_pagpmFactors || !m_pagpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgpmEventFactors::OnInit()
{
	return TRUE;
}

BOOL AgpmEventFactors::OnDestroy()
{
	return TRUE;
}

//		ProcessUpdateEventFactors
//	Functions
//		- nConditionType에 따라 character를 가져와 pcsUpdateFactor로 업데이트 한다.
//		- 업데이트 결과를 pfnProcessResult로 넘겨준다.
//		- 아따 과정이 많다. ㅡ.ㅡ 에궁...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition이 포함되어 있는 event
//		- pcsUpdateFactor	: update할 내용이 들어있는 point factor
//		- pcsUpdateFactorPercent	: update할 내용이 들어있는 percent factor
//		- pfnCheckCondition	: factor를 반영할때 조건을 검사할 필요가 있을때 처리할 함수 포인터
//		- pfnCalcFactor		: pcsUpdateFactor 말고 검색된 Base 별로 따로 UpdateFactor를 계산할 필요가 있을때 처리할 함수
//								(즉, 모든 base의 factor를 pcsUpdateFactor로 바꾸려면 이 함수 포인터를 NULL로
//								 그렇지 않을경우엔 여기에 처리 함수를 넘겨준다. 이때 pcsUpdateFactor는 다시 초기화된다)
//		- pfnProcessResult	: update한 후 update된 factor packet을 받아서 처리할 함수 포인터
//		- pvClass			: 이 함수를 호출한 class (module) pointer
//		- pvCustData		: pfnProcessResult 함수에 인자로 넘겨줘야 편한 데이타가 있음 여기에 포인터를 넘겨준다.
//		- bIsUpdateFactor	: UpdateFactor를 할지 CalcFactor를 할지의 여부
//		- bMakePacket		: MakePacket()을 할지 여부
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::ProcessUpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bIsUpdateFactor,
										 BOOL bMakePacket)
{
	if (!pcsEvent || !pcsUpdateFactor)
		return FALSE;

	switch (nConditionType) {
	case AGPM_EVENT_FACTORS_TYPE_AREA:
		{
			INT_PTR	alCID[AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS];
			INT32	alCID2[AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS];

			ZeroMemory(alCID, sizeof(INT32) * AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS);
			ZeroMemory(alCID2, sizeof(INT32) * AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS);

			// 2005/02/21 마고자
			// 일단 계인덱스를 0으로 지정..

			INT16	nNumCharacter = 0;
			if ((nNumCharacter = m_papmEventManager->GetAreaCharacters( 0 , APMMAP_CHAR_TYPE_PC | APMMAP_CHAR_TYPE_NPC | APMMAP_CHAR_TYPE_MONSTER, pcsEvent, alCID, AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS, alCID2, AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS)) < 1)
				return FALSE;
			
			if (nNumCharacter > AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS)
				nNumCharacter	= AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS;

			for (int i = 0; i < nNumCharacter; ++i)
			{
				if (alCID[i] == AP_INVALID_CID)
					continue;

				AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacter((INT32)alCID[i]);
				if (!pcsCharacter)
					continue;

				INT16	nUpdateType = 0;

				// 조건 검사한다.
				if (pfnCheckCondition)
				{
					// 조건 검사결과가 FALSE면 넘어간다.
					nUpdateType = pfnCheckCondition(pcsCharacter, pvClass, pvCustData);
					if (nUpdateType == 0)
						continue;
				}

				stAgpmEventFactorsCBArg		stCBArg;

				stCBArg.pcsEvent		= pcsEvent;
				stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
				stCBArg.pcsUpdateFactor	= pcsUpdateFactor;
				stCBArg.pcsUpdateFactorPercent	= pcsUpdateFactorPercent;
				stCBArg.pvCustData		= pvCustData;

				// pfnCalcFactor가 널이 아니면 호출해서 pcsUpdateFactor값을 계산하게 한다)
				if (pfnCalcFactor)
				{
					if (!pfnCalcFactor(pcsCharacter, pvClass, &stCBArg))
						continue;
				}

				PVOID	pvPacketFactor = NULL;

				switch (nUpdateType) {
				case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, TRUE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
					{
						if (bIsUpdateFactor)
						{
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
						}
						else
						{
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
						}
					}
					break;
				}

				if (pfnProcessResult && pvClass)
				{
					stCBArg.pvPacketFactor	= pvPacketFactor;

					pfnProcessResult(&stCBArg, pvClass, NULL);
				}

				if (pvPacketFactor)
					delete [] (BYTE*)pvPacketFactor;
//					GlobalFree(pvPacketFactor);
			}

			return TRUE;
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_FACTOR:
		{
			stAgpmEventFactorsArg	stArg;

			stArg.pfnCheckCondition	= pfnCheckCondition;
			stArg.pfnCalcFactor		= pfnCalcFactor;
			stArg.pfnProcessor		= pfnProcessResult;
			stArg.pvClass			= pvClass;
			stArg.pcsEvent			= pcsEvent;
			stArg.pvUpdateFactor	= pcsUpdateFactor;
			stArg.pvUpdateFactorPercent	= pcsUpdateFactorPercent;
			stArg.pvCustData		= pvCustData;
			stArg.bMakePacket		= bMakePacket;

			if (bIsUpdateFactor)
				return m_papmEventManager->GetTargetFactorCharacters(pcsEvent, UpdateFactor, this, &stArg);
			else
				return m_papmEventManager->GetTargetFactorCharacters(pcsEvent, CalcFactor, this, &stArg);
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_CHARACTER:
		{
			if (!pcsEvent->m_pstCondition ||
				!pcsEvent->m_pstCondition->m_pstTarget || 
				pcsEvent->m_pstCondition->m_pstTarget->m_lCID == AP_INVALID_CID)
				return FALSE;

			AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(pcsEvent->m_pstCondition->m_pstTarget->m_lCID);
			if (!pcsCharacter)
				return FALSE;

			INT16	nUpdateType = 0;

			// 조건 검사한다.
			if (pfnCheckCondition)
			{
				// 조건 검사결과가 FALSE면 넘어간다.
				nUpdateType = pfnCheckCondition(pcsCharacter, pvClass, pvCustData);
				if (nUpdateType == 0)
					return FALSE;
			}

			stAgpmEventFactorsCBArg		stCBArg;

			stCBArg.pcsEvent		= pcsEvent;
			stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
			stCBArg.pcsUpdateFactor	= pcsUpdateFactor;
			stCBArg.pcsUpdateFactorPercent	= pcsUpdateFactorPercent;
			stCBArg.pvCustData		= pvCustData;

			// pfnCalcFactor가 널이 아니면 호출해서 pcsUpdateFactor값을 계산하게 한다)
			if (pfnCalcFactor)
			{
				if (!pfnCalcFactor(pcsCharacter, pvClass, &stCBArg))
					return FALSE;
			}

			PVOID	pvPacketFactor = NULL;

			switch (nUpdateType) {
			case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, TRUE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
				{
					if (bIsUpdateFactor)
					{
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
					}
					else
					{
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
					}
				}
				break;
			}

			if (pfnProcessResult && pvClass)
			{
				stCBArg.pvPacketFactor	= pvPacketFactor;

				pfnProcessResult(&stCBArg, pvClass, NULL);
			}

			if (pvPacketFactor)
				delete [] (BYTE*)pvPacketFactor;
//				GlobalFree(pvPacketFactor);

			return TRUE;
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_ITEM:
		{
			stAgpmEventFactorsArg stArg;

			stArg.pfnCheckCondition	= pfnCheckCondition;
			stArg.pfnCalcFactor		= pfnCalcFactor;
			stArg.pfnProcessor		= pfnProcessResult;
			stArg.pvClass			= pvClass;
			stArg.pcsEvent			= pcsEvent;
			stArg.pvUpdateFactor	= pcsUpdateFactor;
			stArg.pvUpdateFactorPercent	= pcsUpdateFactorPercent;
			stArg.pvCustData		= pvCustData;
			stArg.bMakePacket		= bMakePacket;

			if (bIsUpdateFactor)
				return m_papmEventManager->GetTargetItemCharacters(pcsEvent, UpdateFactor, this, &stArg);
			else
				return m_papmEventManager->GetTargetItemCharacters(pcsEvent, CalcFactor, this, &stArg);
		}
		break;
	};

	return FALSE;
}

//		CalcEventFactors
//	Functions
//		- nConditionType에 따라 character를 가져와 pcsUpdateFactor로 CalcFactor() 한다.
//		- 업데이트 결과를 pfnProcessResult로 넘겨준다.
//		- 아따 과정이 많다. ㅡ.ㅡ 에궁...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition이 포함되어 있는 event
//		- pcsUpdateFactor	: calculate할 내용이 들어있는 point factor
//		- pcsUpdateFactorPercent	: calculate할 내용이 들어있는 percent factor
//		- pfnCheckCondition	: factor를 반영할때 조건을 검사할 필요가 있을때 처리할 함수 포인터
//		- pfnCalcFactor		: pcsUpdateFactor 말고 검색된 Base 별로 따로 UpdateFactor를 계산할 필요가 있을때 처리할 함수
//								(즉, 모든 base의 factor를 pcsUpdateFactor로 바꾸려면 이 함수 포인터를 NULL로
//								 그렇지 않을경우엔 여기에 처리 함수를 넘겨준다. 이때 pcsUpdateFactor는 다시 초기화된다)
//		- pfnProcessResult	: update한 후 update된 factor packet을 받아서 처리할 함수 포인터
//		- pvClass			: 이 함수를 호출한 class (module) pointer
//		- pvCustData		: pfnProcessResult 함수에 인자로 넘겨줘야 편한 데이타가 있음 여기에 포인터를 넘겨준다.
//		- bMakePacket		: 말그대로
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket)
{
	return ProcessUpdateEventFactors(nConditionType,
									 pcsEvent,
									 pcsUpdateFactor,
									 pcsUpdateFactorPercent,
									 pfnCheckCondition,
									 pfnCalcFactor,
									 pfnProcessResult,
									 pvClass,
									 pvCustData,
									 FALSE,
									 bMakePacket);
}

//		UpdateEventFactors
//	Functions
//		- nConditionType에 따라 character를 가져와 pcsUpdateFactor로 업데이트 한다.
//		- 업데이트 결과를 pfnProcessResult로 넘겨준다.
//		- 아따 과정이 많다. ㅡ.ㅡ 에궁...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition이 포함되어 있는 event
//		- pcsUpdateFactor	: update할 내용이 들어있는 factor
//		- pcsUpdateFactor	: update할 내용이 들어있는 percent factor
//		- pfnCheckCondition	: factor를 반영할때 조건을 검사할 필요가 있을때 처리할 함수 포인터
//		- pfnCalcFactor		: pcsUpdateFactor 말고 검색된 Base 별로 따로 UpdateFactor를 계산할 필요가 있을때 처리할 함수
//								(즉, 모든 base의 factor를 pcsUpdateFactor로 바꾸려면 이 함수 포인터를 NULL로
//								 그렇지 않을경우엔 여기에 처리 함수를 넘겨준다. 이때 pcsUpdateFactor는 다시 초기화된다)
//		- pfnProcessResult	: update한 후 update된 factor packet을 받아서 처리할 함수 포인터
//		- pvClass			: 이 함수를 호출한 class (module) pointer
//		- pvCustData		: pfnProcessResult 함수에 인자로 넘겨줘야 편한 데이타가 있음 여기에 포인터를 넘겨준다.
//		- bMakePacket		: 말그대로
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::UpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket)
{
	return ProcessUpdateEventFactors(nConditionType,
									 pcsEvent,
									 pcsUpdateFactor,
									 pcsUpdateFactorPercent,
									 pfnCheckCondition,
									 pfnCalcFactor,
									 pfnProcessResult,
									 pvClass,
									 pvCustData,
									 TRUE,
									 bMakePacket);
}

BOOL AgpmEventFactors::ProcessUpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData, BOOL bIsUpdateFactor)
{
	if (!pvData || !pvUpdateData)
		return FALSE;

	AgpdCharacter			*pcsCharacter	= (AgpdCharacter *)			pvData;
	pstAgpmEventFactorsArg	pstArg			= (pstAgpmEventFactorsArg)	pvUpdateData;

	INT16	nUpdateType = 0;

	// 조건 검사한다.
	if (pstArg->pfnCheckCondition)
	{
		// 조건 검사결과가 FALSE면 넘어간다.
		nUpdateType = pstArg->pfnCheckCondition(pcsCharacter, pstArg->pvClass, pstArg->pvCustData);
		if (nUpdateType == 0)
			return FALSE;
	}

	// pfnCalcFactor가 널이 아니면 호출해서 pcsUpdateFactor값을 계산하게 한다)
	if (pstArg->pfnCalcFactor)
	{
		if (!pstArg->pfnCalcFactor(pcsCharacter, pvClass, pstArg))
			return FALSE;
	}

	PVOID	pvPacketFactor = NULL;

	switch (nUpdateType) {
	case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), FALSE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
		{
			if (bIsUpdateFactor)
			{
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), pstArg->bMakePacket);
			}
			else
			{
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), FALSE, pstArg->bMakePacket);
			}
		}
		break;
	}

	if (pstArg->pfnProcessor && pstArg->pvClass)
	{
		stAgpmEventFactorsCBArg		stCBArg;
		ZeroMemory(&stCBArg, sizeof(stCBArg));

		stCBArg.pcsEvent		= pstArg->pcsEvent;
		stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
		stCBArg.pcsUpdateFactor	= (AgpdFactor *) pstArg->pvUpdateFactor;
		stCBArg.pcsUpdateFactorPercent	= (AgpdFactor *) pstArg->pvUpdateFactorPercent;
		stCBArg.pvPacketFactor	= pvPacketFactor;
		stCBArg.pvCustData		= pstArg->pvCustData;

		pstArg->pfnProcessor(&stCBArg, pstArg->pvClass, NULL);
	}

	if (pvPacketFactor)
		delete [] (BYTE*)pvPacketFactor;
//		GlobalFree(pvPacketFactor);

	return TRUE;
}

BOOL AgpmEventFactors::CalcFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData)
{
	AgpmEventFactors	*pThis = (AgpmEventFactors *) pvClass;

	return pThis->ProcessUpdateFactor(pvData, pvClass, pvUpdateData, FALSE);
}

BOOL AgpmEventFactors::UpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData)
{
	AgpmEventFactors	*pThis = (AgpmEventFactors *) pvClass;

	return pThis->ProcessUpdateFactor(pvData, pvClass, pvUpdateData, TRUE);
}