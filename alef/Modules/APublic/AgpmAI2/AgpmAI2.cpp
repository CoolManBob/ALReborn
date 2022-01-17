#include "AgpmAI2.h"
#include "ApMemoryTracker.h"
#include "ApModuleStream.h"

AgpmAI2::AgpmAI2()
{
	SetModuleName("AgpmAI2");
	
	ZeroMemory( &m_pcsApmMap				 , sizeof( m_pcsApmMap					 ) );
	ZeroMemory( &m_pcsAgpmPathFind			 , sizeof( m_pcsAgpmPathFind			 ) );
	ZeroMemory( &m_pcsAgpmFactors			 , sizeof( m_pcsAgpmFactors				 ) );
	ZeroMemory( &m_pcsAgpmCharacter			 , sizeof( m_pcsAgpmCharacter			 ) );
	ZeroMemory( &m_pcsAgpmItem				 , sizeof( m_pcsAgpmItem				 ) );
	ZeroMemory( &m_pcsAgpmEventSpawn		 , sizeof( m_pcsAgpmEventSpawn			 ) );
	ZeroMemory( &m_nCharacterAttachAI2Index	 , sizeof( m_nCharacterAttachAI2Index	 ) );
	ZeroMemory( &m_nSpawnAttachIndex		 , sizeof( m_nSpawnAttachIndex			 ) );
	ZeroMemory( &m_pcsAgpmMonsterPath		 , sizeof( m_pcsAgpmMonsterPath			 ) );
}

AgpmAI2::~AgpmAI2()
{
	AgpdAI2Template			*pcsTemplate;

	INT32			lIndex = 0;

	//템플릿을 쭈욱~ 읽어서 Delete한다.
	for (pcsTemplate = m_aAI2Template.GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_aAI2Template.GetTemplateSequence(&lIndex))
	{
		delete pcsTemplate;
	}
}

BOOL AgpmAI2::OnAddModule()
{
	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmPathFind = (AgpmPathFind *)GetModule("AgpmPathFind");
	m_pcsAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmSkill = (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmEventSpawn = (AgpmEventSpawn *) GetModule("AgpmEventSpawn");

	if( !m_pcsApmMap || !m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmSkill || !m_pcsAgpmEventSpawn)
	{
		return FALSE;
	}

	if (!m_pcsAgpmCharacter->SetCallbackUpdateActionStatus(CBUpdateActionStatus, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	m_nCharacterAttachAI2Index = m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgpdAI2ADChar), ConAgpdAI2ADChar, DestAgpdAI2ADChar );
	if (m_nCharacterAttachAI2Index < 0)
		return FALSE;

	m_aAI2Template.InitializeObject( sizeof(AgpdAI2Template *), 10000, NULL, NULL, NULL );

	if(!AddStreamCallback(AGPMAI2_DATA_TEMPLATE, CBAI2TemplateRead, CBAI2TemplateWrite, this))
		return FALSE;

	if (m_pcsAgpmEventSpawn)
	{
		m_nSpawnAttachIndex = m_pcsAgpmEventSpawn->AttachSpawnData(this, sizeof(AgpdAI2ADChar), NULL, NULL);
		if (m_nSpawnAttachIndex < 0)
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmAI2::OnInit()
{
	m_pcsAgpmMonsterPath = (AgpmMonsterPath *) GetModule("AgpmMonsterPath");

	if(!m_pcsAgpmMonsterPath)
		return FALSE;

	return TRUE;
}

BOOL AgpmAI2::ConAgpdAI2ADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	BOOL				bResult;

	bResult = FALSE;

	if (!pData || !pClass)
		return bResult;

	AgpmAI2 *pThis	= (AgpmAI2 *) pClass;
	AgpdAI2ADChar *pAttachedData = pThis->GetCharacterData((AgpdCharacter *) pData);

	if( pAttachedData )
	{
		// 여기서 데이타 세팅
		pAttachedData->m_pcsAgpdAI2Template = NULL;

		pAttachedData->m_ulLastAttackTime = 0;
		pAttachedData->m_lAttackInterval = 0;

		pAttachedData->m_ulPrevProcessTime = 0;
		pAttachedData->m_ulProcessInterval = 0;
		pAttachedData->m_ulLastPathFindTime = 0;
		pAttachedData->m_ulNextWanderingTime = 0;
		pAttachedData->m_ulNextDialogTargetPC = 0;

		pAttachedData->m_ulNextDecStaticTime = 0;
		pAttachedData->m_ulNextDecSlowTime = 0;
		pAttachedData->m_ulNextDecFastTime = 0;

		pAttachedData->m_bUseAI2 = FALSE;

		pAttachedData->m_pclPath = new CSpecializeListPathFind<int, AgpdPathFindPoint *>;

		pAttachedData->m_pclPath->allocSwapBuffer( AGPMPATHFIND_TRY_COUNT );
		pAttachedData->m_pclPath->resetIndex();

		pAttachedData->m_lTargetID = 0;
		pAttachedData->m_lReadyTargetID = 0;	// 2005.08.30. steeple

		pAttachedData->m_pcsBossCharacter = NULL;
		pAttachedData->m_lCurrentFollowers = 0;
		pAttachedData->m_lCurrentCumulativeFollower = 0;

		pAttachedData->m_lCastSkillLevel = 0;	// 2005.09.06. steeple

		pAttachedData->m_bScreamUsed = FALSE;
		pAttachedData->m_ulScreamStartTime = 0;

		for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i )
			pAttachedData->m_pcsFollower[i] = NULL;

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgpmAI2::DestAgpdAI2ADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	BOOL				bResult;

	bResult = FALSE;

	if (!pData || !pClass)
		return bResult;

	AgpmAI2 *pThis	= (AgpmAI2 *) pClass;
	AgpdAI2ADChar *pAttachedData = pThis->GetCharacterData((AgpdCharacter *) pData);

	if( pAttachedData )
	{
		// 여기서 데이타 세팅
		if (pAttachedData->m_pclPath)
		{
			pAttachedData->m_pclPath->removeSwapBuffer();

			delete pAttachedData->m_pclPath;
			pAttachedData->m_pclPath	= NULL;
		}

		bResult = TRUE;
	}

	return bResult;
}

AgpdAI2ADChar *AgpmAI2::GetCharacterData(AgpdCharacter *pcsCharacter)
{
	// Module Data 가져오기
	return (AgpdAI2ADChar *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nCharacterAttachAI2Index, (void *) pcsCharacter);
}

INT32 AgpmAI2::GetMaxFellowerWorkers( AgpdAI2Template *pcsTemplate )
{
	INT32				lResult;

	lResult = -1;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFellowWorker.m_lFellowWorkerTotalCount;
	}

	return lResult;
}

INT32 AgpmAI2::GetMaxFollowers( AgpdAI2Template *pcsTemplate )
{
	INT32				lResult;

	lResult = -1;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFollower.m_lFollowerTotalCount;
	}

	return lResult;
}

INT32 AgpmAI2::GetUpkeepFollowers( AgpdAI2Template *pcsTemplate )
{
	INT32				lResult;

	lResult = -1;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFollower.m_lMinUpkeepFollower;
	}

	return lResult;
}

INT32 AgpmAI2::GetCumulativeFollowers( AgpdAI2Template *pcsTemplate )
{
	INT32				lResult;

	lResult = -1;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFollower.m_lMaxCumulativeFollower;
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyTemplateID()
{
	AgpdAI2Template		*pcsTemplate;

	INT32			iIndex;

	iIndex = 1;

	while( 1 ) 
	{
		pcsTemplate = m_aAI2Template.GetAITemplate( iIndex );

		if( pcsTemplate == NULL )
			break;

		iIndex++;
	}

	return iIndex;
}

BOOL AgpmAI2::StreamWriteTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	AgpdAI2Template			*pcsTemplate;
	ApModuleStream			csStream;

	INT32				lIndex = 0;
	CHAR				szTID[80];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// 등록된 모든 Character Template에 대해서...
	for (pcsTemplate = m_aAI2Template.GetTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = m_aAI2Template.GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsTemplate->m_lTID );

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMAI2_DATA_TEMPLATE, pcsTemplate, this))
			return FALSE;
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgpmAI2::StreamReadTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	AgpdAI2Template		*pcsTemplate;

	ApModuleStream		csStream;

	UINT16				nNumKeys;
	INT32				i;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Section Name은 DropTemplateName 이다.
		pcsTemplate = new AgpdAI2Template;
		
		pcsTemplate->m_lTID = atoi( csStream.ReadSectionName(i) );

		// Template을 추가한다.
		m_aAI2Template.AddAITemplate( pcsTemplate );

		if (!pcsTemplate)
			return FALSE;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMAI2_DATA_TEMPLATE, pcsTemplate, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgpmAI2::CBAI2TemplateWrite(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI2			*pThis = (AgpmAI2 *) pClass;
	AgpdAI2Template	*pcsTemplate = (AgpdAI2Template *) pData;

	if( (pThis != NULL) && (pcsTemplate != NULL) && (pStream != NULL) )
	{
		INT32				lItemCount;
		INT32				lSkillCount;
		INT32				lCounter;

		lItemCount = 0;
		lSkillCount = 0;

		//Template이름
		pStream->WriteValue( AGPMAI2_TEMPLATENAME, pcsTemplate->m_strTemplateName );

		//NPCAI인가?
		pStream->WriteValue( AGPMAI2_NPC_AI_TYPE, pcsTemplate->m_eNPCAI2Type );

		//Class수정치
		pStream->WriteValue( AGPMAI2_ADJUST_WARRIOR, pcsTemplate->m_csClassAdjust.m_lWarrior );
		pStream->WriteValue( AGPMAI2_ADJUST_ARCHER, pcsTemplate->m_csClassAdjust.m_lArcher );
		pStream->WriteValue( AGPMAI2_ADJUST_MAGE, pcsTemplate->m_csClassAdjust.m_lMage );
		pStream->WriteValue( AGPMAI2_ADJUST_MONK, pcsTemplate->m_csClassAdjust.m_lMonk );

		//AggressivePoint
		pStream->WriteValue( AGPMAI2_AGGRESSIVE_POINT, pcsTemplate->m_lAgressivePoint );

		//Action수정치
		pStream->WriteValue( AGPMAI2_ACTION_ADJUST_DAMAGE, pcsTemplate->m_csAgroActionAdjust.m_lDamage );
		pStream->WriteValue( AGPMAI2_ACTION_ADJUST_TRY_ATTACK, pcsTemplate->m_csAgroActionAdjust.m_lTryAttack );
		pStream->WriteValue( AGPMAI2_ACTION_DEBUFF, pcsTemplate->m_csAgroActionAdjust.m_lDebuff );
		pStream->WriteValue( AGPMAI2_ACTION_TRYDEBUFF, pcsTemplate->m_csAgroActionAdjust.m_lTryDebuff );
		pStream->WriteValue( AGPMAI2_ACTION_P2P_HEAL, pcsTemplate->m_csAgroActionAdjust.m_lP2PHeal );
		pStream->WriteValue( AGPMAI2_ACTION_P2P_BUFF, pcsTemplate->m_csAgroActionAdjust.m_lP2PBuff );

		for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; ++lCounter )
		{
			if( pcsTemplate->m_csUseItem[lCounter].m_lItemID != 0 )
			{
				++lItemCount;
			}
		}

		for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_SKILL_COUNT; ++lCounter )
		{
			if( pcsTemplate->m_csUseSkill[lCounter].m_lSkillID != 0 )
			{
				++lSkillCount;
			}
		}

		pStream->WriteValue( AGPMAI2_USE_ITEM_COUNT, lItemCount );

		if( lItemCount > 0 )
		{
			for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; ++lCounter )
			{
				if( pcsTemplate->m_csUseItem[lCounter].m_lItemID != 0 )
				{
					pStream->WriteValue( AGPMAI2_ITEM_TID, pcsTemplate->m_csUseItem[lCounter].m_lItemID );
					pStream->WriteValue( AGPMAI2_TARGET_PARAMETER, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lTargetParameter );
					pStream->WriteValue( AGPMAI2_PARAMATER, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lParameter );
					pStream->WriteValue( AGPMAI2_PERCENT, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_bPercent );
					pStream->WriteValue( AGPMAI2_OPERATOR, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lOperator );
					pStream->WriteValue( AGPMAI2_CONDITION_CHECK, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lConditionCheck );
					pStream->WriteValue( AGPMAI2_TIMERCOUNT, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lTimerCount );
					pStream->WriteValue( AGPMAI2_PROBABLE, pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lProbable );
				}
			}
		}

		pStream->WriteValue( AGPMAI2_USE_SKILL_COUNT, lSkillCount );

		if( lSkillCount > 0 )
		{
			for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_SKILL_COUNT; ++lCounter )
			{
				if( pcsTemplate->m_csUseSkill[lCounter].m_lSkillID != 0 )
				{
					pStream->WriteValue( AGPMAI2_SKILL_TID, pcsTemplate->m_csUseSkill[lCounter].m_lSkillID );
					pStream->WriteValue( AGPMAI2_TARGET_PARAMETER, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lTargetParameter );
					pStream->WriteValue( AGPMAI2_PARAMATER, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lParameter );
					pStream->WriteValue( AGPMAI2_PERCENT, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_bPercent );
					pStream->WriteValue( AGPMAI2_OPERATOR, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lOperator );
					pStream->WriteValue( AGPMAI2_CONDITION_CHECK, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lConditionCheck );
					pStream->WriteValue( AGPMAI2_TIMERCOUNT, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lTimerCount );
					pStream->WriteValue( AGPMAI2_PROBABLE, pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lProbable );
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::CBAI2TemplateRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmAI2			*pThis = (AgpmAI2 *) pClass;
	AgpdAI2Template	*pcsTemplate = (AgpdAI2Template *) pData;

	const CHAR				*szValueName;

	if( (pThis != NULL) && (pcsTemplate != NULL) && (pStream != NULL) )
	{
		INT32			lCounter;
		INT32			lItemCount;
		INT32			lSkillCount;

		lItemCount = 0;
		lSkillCount = 0;

		while (pStream->ReadNextValue())
		{
			szValueName = pStream->GetValueName();

			if( !strcmp(szValueName,AGPMAI2_TEMPLATENAME ) )
			{
				pStream->GetValue( pcsTemplate->m_strTemplateName, sizeof(pcsTemplate->m_strTemplateName) );

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_NPC_AI_TYPE) )
				{
					INT32			lNPCAIType;

					pStream->GetValue( &lNPCAIType );

					pcsTemplate->m_eNPCAI2Type = (eAgpmAI2Type)lNPCAIType;
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ADJUST_WARRIOR) )
				{
					pStream->GetValue( &pcsTemplate->m_csClassAdjust.m_lWarrior );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ADJUST_ARCHER) )
				{
					pStream->GetValue( &pcsTemplate->m_csClassAdjust.m_lArcher );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ADJUST_MAGE) )
				{
					pStream->GetValue( &pcsTemplate->m_csClassAdjust.m_lMage );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ADJUST_MONK) )
				{
					pStream->GetValue( &pcsTemplate->m_csClassAdjust.m_lMonk );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_AGGRESSIVE_POINT) )
				{
					pStream->GetValue( &pcsTemplate->m_lAgressivePoint );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_ADJUST_DAMAGE) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lDamage );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_ADJUST_TRY_ATTACK) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lTryAttack );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_DEBUFF) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lDebuff );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_TRYDEBUFF) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lTryDebuff );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_P2P_HEAL) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lP2PHeal );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_ACTION_P2P_BUFF) )
				{
					pStream->GetValue( &pcsTemplate->m_csAgroActionAdjust.m_lP2PBuff );
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_USE_ITEM_COUNT) )
				{
					pStream->GetValue( &lItemCount );
				}

				for( lCounter=0; lCounter<lItemCount; lCounter++ )
				{
					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_ITEM_TID) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_lItemID );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_TARGET_PARAMETER) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lTargetParameter );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PARAMATER) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lParameter );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PERCENT) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_bPercent );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_OPERATOR) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lOperator );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_CONDITION_CHECK) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lConditionCheck );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_TIMERCOUNT) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lTimerCount );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PROBABLE) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseItem[lCounter].m_csTable.m_lProbable );
					}
				}

				pStream->ReadNextValue();
				szValueName = pStream->GetValueName();
				if(	!strcmp(szValueName, AGPMAI2_USE_SKILL_COUNT) )
				{
					pStream->GetValue( &lSkillCount );
				}

				for( lCounter=0; lCounter<lSkillCount; ++lCounter )
				{
					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_SKILL_TID) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_lSkillID );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_TARGET_PARAMETER) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lTargetParameter );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PARAMATER) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lParameter );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PERCENT) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_bPercent );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_OPERATOR) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lOperator );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_CONDITION_CHECK) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lConditionCheck );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_TIMERCOUNT) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lTimerCount );
					}

					pStream->ReadNextValue();
					szValueName = pStream->GetValueName();
					if(	!strcmp(szValueName, AGPMAI2_PROBABLE) )
					{
						pStream->GetValue( &pcsTemplate->m_csUseSkill[lCounter].m_csTable.m_lProbable );
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::LoadMonsterAITemplate( char *pstrFileName )
{
	BOOL bResult = false;

	if( pstrFileName )
	{
		if( m_csAuExcelTxtLib.OpenExcelFile( pstrFileName, TRUE) )		
		{
			INT32				lMaxRow, lMaxColumn;
			INT32				lReadTemplateCount  = 0;
			INT32				lStartRow = 1;

			lMaxRow				= m_csAuExcelTxtLib.GetRow();
			lMaxColumn			= m_csAuExcelTxtLib.GetColumn();

			char *pstrData, *pstrData2, *pstrData3, *pstrData4, *pstrData5;
			
			
			while( lMaxRow >= lReadTemplateCount )
			{
				AgpdAI2Template	*pcsAgpdAI2Template;
				char			*pstrColumnName;

				pcsAgpdAI2Template = new AgpdAI2Template;

				for( int lColumnCounter=0; lColumnCounter < lMaxColumn; ++lColumnCounter )
				{
					pstrData  = NULL;
					pstrData2 = NULL;
					pstrData3 = NULL;
					pstrData4 = NULL;
					pstrData5 = NULL;
 
					// 컬럼 제목을 하나 가져온다.
					pstrColumnName = m_csAuExcelTxtLib.GetData( lColumnCounter, 0 );

					if( pstrColumnName == NULL )
						continue;

					//AI의 TID를 읽는다.
					if( !StrCmpI( pstrColumnName, AGPMAI2R_TID ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_lTID = atoi( pstrData );
						}
					}
					//AI의 이름을 읽는다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_Name ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							strcat( pcsAgpdAI2Template->m_strTemplateName, pstrData );
							ASSERT(strlen(pcsAgpdAI2Template->m_strTemplateName) < 80);
						}
					}
					//UseNormalAttack(일반공격여부)을 읽는다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_UseNormalAttack ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_bUseNormalAttack = (BOOL)atoi( pstrData );
						}
					}
					// 이곳에서 HateRace, HateRacePercent, HateClass, HateClassPercent까지 다 처리한다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_HateRace ) )
					{
						INT32	   lHateRace, lHateClass, lHateRacePercent, lHateClassPercent;
						lHateRace = lHateClass = lHateRacePercent = lHateClassPercent = 0;

						// HateRace
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						// HateRacePercent
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter + 1, lStartRow );
						// HateClass
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter + 2, lStartRow );
						// HateClassPercent
						pstrData4 = m_csAuExcelTxtLib.GetData( lColumnCounter + 3, lStartRow );

						if( pstrData )
						{
							if( !StrCmpI( pstrData, AGPMAI2R_RaceHuman ) )
								lHateRace = AURACE_TYPE_HUMAN;
							else if( !StrCmpI( pstrData, AGPMAI2R_RaceOrc ) )
								lHateRace = AURACE_TYPE_ORC;
							else if( !StrCmpI( pstrData, AGPMAI2R_RaceMoonElf ) )
								lHateRace = AURACE_TYPE_MOONELF;
							else if( !StrCmpI( pstrData, AGPMAI2R_RaceScion ) )
								lHateRace = AURACE_TYPE_DRAGONSCION;
						}
						if( pstrData2 )
						{
							lHateRacePercent = atoi( pstrData2 );
						}
						if( pstrData3 )
						{
							if( !StrCmpI( pstrData3, AGPMAI2R_ClassWarrior ) )
								lHateClass = AUCHARCLASS_TYPE_KNIGHT;
							else if( !StrCmpI( pstrData3, AGPMAI2R_ClassArcher ) )
								lHateClass = AUCHARCLASS_TYPE_RANGER;
							else if( !StrCmpI( pstrData3, AGPMAI2R_ClassMage ) )
								lHateClass = AUCHARCLASS_TYPE_MAGE;
							else if( !StrCmpI( pstrData3, AGPMAI2R_ClassScion ) )
								lHateClass = AUCHARCLASS_TYPE_SCION;
						}
						if( pstrData4 )
						{
							lHateClassPercent = atoi( pstrData4 );
						}

						//Race Class둘 중 하나는 0보다 큰 값이어야한다.
						if( (lHateRace > 0) || (lHateClass > 0) )
						{
							//HateAdjust는 0보다 커야한다.
							if( lHateRacePercent > 0 || lHateClassPercent > 0)
							{
								SetEmptyRaceClassSlot( &pcsAgpdAI2Template->m_csRaceClassAdjust, lHateRace, lHateClass, lHateRacePercent, lHateClassPercent );
							}
						}
					}
					// FellowWorkerRefreshTime을 읽는다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_FellowWorkerRefreshTime) )
					{
						pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime = m_csAuExcelTxtLib.GetDataToInt( lColumnCounter, lStartRow );
					}
					// FellowWorker에 대한 정보를 읽는다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_FellowWorkerTID ) )
					{
						//스크림에 응답할 거리와 응답할 동족 몬스터의 수를 센다.
						pstrData  = m_csAuExcelTxtLib.GetData( lColumnCounter+2, lStartRow );
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+3, lStartRow );
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );	//동족 TID
						pstrData4 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lStartRow );//동족 이름

						if( pstrData && pstrData2 )
						{
							pcsAgpdAI2Template->m_csFellowWorker.m_lRange = atoi( pstrData ) * 100;
							pcsAgpdAI2Template->m_csFellowWorker.m_lCount = atoi( pstrData2 );
							pcsAgpdAI2Template->m_csFellowWorker.m_lFellowWorkerTotalCount = atoi( pstrData2 );
						}

						if( pstrData3 && pstrData4 )
						{
							AgpdCharacterTemplate		*pcsCharacterTemplate;
							INT32			lFellowWorkerTID;
							
							lFellowWorkerTID = atoi( pstrData );

							pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( lFellowWorkerTID );

							if( pcsCharacterTemplate )
							{
								if( !strcmp( pcsCharacterTemplate->m_szTName, pstrData2 ) )
								{
									pcsAgpdAI2Template->m_csFellowWorker.m_csFellowWorkerInfo.m_lFellowWorkerTID = lFellowWorkerTID;
									strcat(pcsAgpdAI2Template->m_csFellowWorker.m_csFellowWorkerInfo.m_strFellowWorkerName, pstrData2);
								}
							}
						}												
					}
					// FollowerWorker를 읽는다 (내가 모셔야하는 보스몹의 정보)
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_FollowerTID ) )
					{
						INT32			lMaxCumulativeFollower;
						INT32			lMinUpkeepFollower;
												
						//FollowerTID
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						// FollowerNumber
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lStartRow );

						if(pstrData && pstrData2)
						{
							// MonsterTID
							CHAR* token;
							// Monster MaxCount
							CHAR* token2;
							
							token = strtok(pstrData, ";");
							token2 = strtok(pstrData2, ";");
							while(token != NULL && token2 != NULL)
							{
								INT32 lFollowerTID = atoi(token);
								INT32 lFollowerCount = atoi(token2);
								if(lFollowerTID > 0)
								{
									SetEmptyFollowerSlot(&pcsAgpdAI2Template->m_csFollower, lFollowerTID, lFollowerCount);
								}
								token = strtok(NULL, ";");
								token2 = strtok(NULL, ";");
							}											
						}

						pstrData = pstrData2 = NULL;

						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter+2, lStartRow );

						if( pstrData )
						{
							lMaxCumulativeFollower = atoi( pstrData );

							if( lMaxCumulativeFollower > 0 )
							{
								pcsAgpdAI2Template->m_csFollower.m_lMaxCumulativeFollower = lMaxCumulativeFollower;
							}
						}

						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+3, lStartRow );

						if( pstrData2 )
						{
							lMinUpkeepFollower = atoi( pstrData );

							if( lMinUpkeepFollower > 0 )
							{
								pcsAgpdAI2Template->m_csFollower.m_lMinUpkeepFollower = lMinUpkeepFollower;
							}
						}
					}
					//스킬을 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_SKill1) || !StrCmpI( pstrColumnName, AGPMAI2R_SKill2) || 
						     !StrCmpI( pstrColumnName, AGPMAI2R_SKill3) || !StrCmpI( pstrColumnName, AGPMAI2R_SKill4) || 
							 !StrCmpI( pstrColumnName, AGPMAI2R_SKill5))
					{
						// Skill TID와 Level이 설정
						pstrData  = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						// Skill 발동 조건
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter + 1, lStartRow);
						// Skill 제한
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter + 2, lStartRow);
												
						if( pstrData && pstrData2 && pstrData3)
						{
							CHAR	*pstrSkillTID;
							CHAR	*pstrSkillLevel;
							CHAR	*pstrArg;
							CHAR	*pstrScalar;
							CHAR	*pstrOperator;
							CHAR	*pstrCountTime;
							CHAR	*pstrCount;
							
							pstrSkillTID = strtok(pstrData, ";");
							pstrSkillLevel = strtok(NULL, ";");
							pstrArg = strtok(pstrData2, ";");
							pstrScalar = strtok(NULL, ";");
							pstrOperator = strtok(NULL, ";");
							pstrCountTime = strtok(pstrData3, ";");
							pstrCount = strtok(NULL, ";");

							if(!pstrCount) pstrCount = "0";
							
							SetSkill(pcsAgpdAI2Template, pstrSkillTID, pstrSkillLevel, pstrArg, pstrScalar, pstrOperator, pstrCountTime, pstrCount);								
						}
					}
					//아이템을 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_Item1) )
					{
						// Item TID
						pstrData  = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						// Item 발동 조건
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter + 1, lStartRow);
						// Item 제한
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter + 2, lStartRow);

						if( pstrData && pstrData2 && pstrData3)
						{
							CHAR	*pstrItemTID;
							CHAR	*pstrArg;
							CHAR	*pstrScalar;
							CHAR	*pstrOperator;
							CHAR	*pstrCountTime;
							CHAR	*pstrCount;

							pstrItemTID = strtok(pstrData, ";");
							pstrArg = strtok(pstrData2, ";");
							pstrScalar = strtok(NULL, ";");
							pstrOperator = strtok(NULL, ";");
							pstrCountTime = strtok(pstrData3, ";");
							pstrCount = strtok(NULL, ";");

							SetItem(pcsAgpdAI2Template, pstrItemTID,  pstrArg, pstrScalar, pstrOperator, pstrCountTime, pstrCount);								
						}
					}
					//가드 AI지정.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_Guard ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							if( !strcmp( pstrData, "고정" ) )
							{
								pcsAgpdAI2Template->m_eNPCAI2Type = AGPMAI2_TYPE_FIXED_NPC;
							}
							else if( !strcmp( pstrData, "배회" ) )
							{
								pcsAgpdAI2Template->m_eNPCAI2Type = AGPMAI2_TYPE_PATROL_NPC;
							}
						}
					}
					//선제공격 플래그 설정
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_PreemptiveType ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_lPreemptiveType = atoi( pstrData );
						}
					}
					// 선제조건을 읽는다 여기부터 조건1,2,3
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_PreemptiveCondition1 ) || !StrCmpI(pstrColumnName, AGPMAI2R_PreemptiveCondition2) || !StrCmpI(pstrColumnName, AGPMAI2R_PreemptiveCondition2)) 
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							CHAR *pstrAggroTarget;
							CHAR *pstrArg;
							CHAR *pstrScalar;
							CHAR *pstrOperator;
							CHAR *pstrAccept;

							pstrAggroTarget = strtok(pstrData, ";");
							pstrArg = strtok(NULL, ";");
							pstrScalar = strtok(NULL, ";");
							pstrOperator = strtok(NULL, ";");
							pstrAccept = strtok(NULL, ";");

							SetPreemptive(pcsAgpdAI2Template, pstrAggroTarget, pstrArg, pstrScalar, pstrOperator, pstrAccept);
						}
					}
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_PreemptiveRange ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_fPreemptiveRange = (FLOAT)atof( pstrData ) * 100.f;
						}
					}

					// Attribute를 읽어드린다.
					else if( !StrCmpI(pstrColumnName, AGPMAI2R_Attribute ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData && !strcmp(pstrData, "고정"))
						{
							pcsAgpdAI2Template->m_bDontMove = TRUE;
						}
					}
					// 소환수에 대해서 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_SummonTID ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							CHAR* token;

							token = strtok(pstrData, ";");
							while (token != NULL)
							{
								INT32 lValue = atoi(token);
								if(lValue > 0)
									pcsAgpdAI2Template->m_csSummon.m_vtTID.push_back(lValue);
								token = strtok(NULL, ";");
							}
						}
					}
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_SummonMaxCount ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							INT32 lValue = atoi(pstrData);
							if(lValue > 0)
								pcsAgpdAI2Template->m_csSummon.m_lMaxCount = lValue;
						}
					}
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_SearchEnemy ) )
					{
						pcsAgpdAI2Template->m_eSearchEnemy = static_cast<eSearchEnemy>(m_csAuExcelTxtLib.GetDataToInt( lColumnCounter, lStartRow ));
					}
					else if( !StrCmpI( pstrColumnName, AGPMAI2_StopFight ) )
					{
						pcsAgpdAI2Template->m_eStopFight = static_cast<eStopFight>(m_csAuExcelTxtLib.GetDataToInt( lColumnCounter, lStartRow ) ); 
					}
					// 도망치는 몬스터를 위한 자료를 읽어드린다. (EscapeHP와 EscapeTime까지 다 설정한다.)
					else if( !StrCmpI( pstrColumnName, AGPMAI2_Escape ) )
					{
						pcsAgpdAI2Template->m_eEscape = static_cast<eEscape>(m_csAuExcelTxtLib.GetDataToInt( lColumnCounter, lStartRow ) );

						pstrData2 = m_csAuExcelTxtLib.GetData(lColumnCounter+1, lStartRow);
						pstrData3 = m_csAuExcelTxtLib.GetData(lColumnCounter+2, lStartRow);
						pstrData4 = m_csAuExcelTxtLib.GetData(lColumnCounter+3, lStartRow);

						if(pstrData2)
						{
							pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_lEscapeHP = atoi(pstrData2);
						}

						if(pstrData3)
						{
							pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_uIEscapeTime = atoi(pstrData3) * 1000;
						}

						if(pstrData4)
						{
							pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_ulEscapeCount = atoi(pstrData4);
						}
					}
					// LinkedMonster를 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_LinkMonsterTID ) )
					{
						pcsAgpdAI2Template->m_lLinkMonsterTID = m_csAuExcelTxtLib.GetDataToInt( lColumnCounter, lStartRow );
						pcsAgpdAI2Template->m_fLinkMonsterSight = m_csAuExcelTxtLib.GetDataToFloat( lColumnCounter+1, lStartRow ) * 100.0f;
					}
					// HideMonster를 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_HideMonster ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideCount = atoi(pstrData);
													
							pstrData2 = m_csAuExcelTxtLib.GetData(lColumnCounter+1, lStartRow);
							if(pstrData2)
							{
								pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideMonsterHP = atoi(pstrData2);
							}

							pstrData3 = m_csAuExcelTxtLib.GetData(lColumnCounter+2, lStartRow);
							if(pstrData3)
							{
								pcsAgpdAI2Template->m_csMonsterHideInfo.m_uIHideMonsterTime = atoi(pstrData3) * 1000;
							}
						}
					}
					// MainTenanceRange를 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_MaintenanceRange ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							pcsAgpdAI2Template->m_fMaintenanceRange = atof(pstrData) * 100.0f;
						}
					}
					// LootItem정보를 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_LootItem ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange = atof(pstrData) * 100.0f;

							pstrData2 = m_csAuExcelTxtLib.GetData(lColumnCounter+1, lStartRow);
							if(pstrData2)
							{
								pcsAgpdAI2Template->m_csLootItemInfo.m_uILootItemTime = atoi(pstrData2) * 1000;
							}
						}
					}
					// RandomTarget여부를 읽어드린다.
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_RandomTarget ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData && !strcmp(pstrData, "1") )
						{
							pcsAgpdAI2Template->m_bRandomTarget = TRUE;
						}
					}
					// PinchMonster 설정
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_PINCH_MONSTER ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							pcsAgpdAI2Template->m_ulPinchMonsterTID = atoi(pstrData);
						}
					}
					// Visibility 설정
					else if( !StrCmpI( pstrColumnName, AGPMAI2R_VISIBILITY) )
					{
						pstrData = m_csAuExcelTxtLib.GetData(lColumnCounter, lStartRow);

						if(pstrData)
						{
							pcsAgpdAI2Template->m_fVisibility = (float)atof(pstrData) * 100.f;
						}
					}
				}

				if( !m_aAI2Template.AddAITemplate(pcsAgpdAI2Template) )
				{
					delete pcsAgpdAI2Template;
					pcsAgpdAI2Template = NULL;
				}

				lReadTemplateCount++;
				lStartRow++;
			}			
		}
	}

	return TRUE;
}


bool AgpmAI2::LoadAITemplate( char *pstrFileName )
{
	bool			bResult;

	bResult = false;

	if( pstrFileName )
	{
		//엑셀파일을 연다.
		if( m_csAuExcelTxtLib.OpenExcelFile( pstrFileName, TRUE ) )
		{
			INT32				lMaxRow, lMaxColumn;
			INT32				lTemplateCount;
			INT32				lReadTemplateCount;

			char				*pstrData, *pstrData2, *pstrData3, *pstrData4, *pstrData5, *pstrData6, *pstrData7, *pstrData8, *pstrData9;

			lMaxRow = m_csAuExcelTxtLib.GetRow();
			lMaxColumn = m_csAuExcelTxtLib.GetColumn();

			INT32				lStartRow, lEndRow;
			char				*pstrTempBuffer;

			pstrData = NULL;
			pstrData2 = NULL;
			pstrData3 = NULL;
			pstrData4 = NULL;
			pstrData5 = NULL;
			pstrData4 = NULL;
			pstrData7 = NULL;
			pstrData8 = NULL;
			pstrData9 = NULL;

			lTemplateCount = 0;
			lReadTemplateCount = 0;

			lStartRow = 1;
			lEndRow = 0;

			//실제 갯수를 세어둔다.
			for( INT32 lRow=lStartRow; lRow<lMaxRow; ++lRow )
			{
				pstrTempBuffer = m_csAuExcelTxtLib.GetData( 0, lRow );

				if( pstrTempBuffer )
				{
					if( !strcmp( pstrTempBuffer, "-" ) )
					{
						lTemplateCount++;
					}
				}
			}

			while( 1 )
			{
				if( lReadTemplateCount >= lTemplateCount )
					break;

				for( INT32 lRow=lStartRow; lRow<lMaxRow; ++lRow )
				{
					pstrTempBuffer = m_csAuExcelTxtLib.GetData( 0, lRow );

					if( pstrTempBuffer )
					{
						if( !strcmp( pstrTempBuffer, "-" ) )
						{
							lEndRow = lRow;
							break;
						}
					}
				}

				//쭈욱 읽어보세나~
				AgpdAI2Template	*pcsAgpdAI2Template;
				char			*pstrColumnName;

				pcsAgpdAI2Template = new AgpdAI2Template;

				for( int lColumnCounter=0; lColumnCounter<lMaxColumn; ++lColumnCounter )
				{
					pstrColumnName = m_csAuExcelTxtLib.GetData( lColumnCounter, 0 );

					if( pstrColumnName == NULL )
						continue;

					//AI의 TID를 읽는다.
					if( !strcmpi( pstrColumnName, AGPMAI2_TID ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_lTID = atoi( pstrData );
						}
					}
					//AI의 이름을 읽는다.
					else if( !strcmpi( pstrColumnName, AGPMAI2_Name ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							strcat( pcsAgpdAI2Template->m_strTemplateName, pstrData );
							ASSERT(strlen(pcsAgpdAI2Template->m_strTemplateName) < 80);
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_UseNormalAttack ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_bUseNormalAttack = (BOOL)atoi( pstrData );
						}
					}
					//종족,클래스,그에 따른 적대가중치를 준다.
					else if( !strcmpi( pstrColumnName, AGPMAI2_HateRace ) )
					{
/*						INT32		lRace, lClass, lHateAdjust;

						for( int lRow=lStartRow; lRow<lEndRow; lRow++ )
						{
							lRace = lClass = lHateAdjust = 0;

							//Race
							pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lRow );
							//Condition
							pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lRow );
							//HateAdjust
							pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter+2, lRow );

							if( pstrData )
							{
								if( !strcmp( pstrData, AGPMAI2_RaceHuman ) )
									lRace = AURACE_TYPE_HUMAN;
								else if( !strcmp( pstrData, AGPMAI2_RaceOrc ) )
									lRace = AURACE_TYPE_ORC;
							}
							if( pstrData2 )
							{
								if( !strcmp( pstrData2, AGPMAI2_ClassWarrior ) )
									lClass = AUCHARCLASS_TYPE_KNIGHT;
								else if( !strcmp( pstrData2, AGPMAI2_ClassArcher ) )
									lClass = AUCHARCLASS_TYPE_RANGER;
								else if( !strcmp( pstrData2, AGPMAI2_ClassMage ) )
									lClass = AUCHARCLASS_TYPE_MAGE;
							}
							if( pstrData3 )
							{
								lHateAdjust = atoi( pstrData3 );
							}

							//Race Class둘 중 하나는 0보다 큰 값이어야한다.
							if( (lRace > 0) || (lClass > 0) )
							{
								//HateAdjust는 0보다 커야한다.
								if( lHateAdjust > 0)
								{
									SetEmptyRaceClassSlot( &pcsAgpdAI2Template->m_csRaceClassAdjust, lRace, lClass, lHateAdjust );
								}
							}
						}*/
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_HateAction ) )
					{
/*						INT32		lAction, lHateAdjust;

						for( int lRow=lStartRow; lRow<lEndRow; ++lRow )
						{
							//lAction
							pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lRow );
							//HateAdjust
							pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lRow );

							if( pstrData && pstrData2)
							{
								lAction = atoi( pstrData );
								lHateAdjust = atoi( pstrData2 );

								if( (lAction > 0) && (lHateAdjust > 0) )
								{
									SetEmptyActionSlot( &pcsAgpdAI2Template->m_csActionAdjust, (eAgpdAI2ActionAdjust)lAction, lHateAdjust );
								}
							}
						}*/
					}
					else if ( !strcmpi( pstrColumnName, AGPMAI2_FellowWorkerRefreshTime) )
					{
						pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime = m_csAuExcelTxtLib.GetDataToInt(lColumnCounter, lStartRow) * 1000;
					}
					//else if ( !strcmpi( pstrColumnName, AGPMAI2_FellowWorkerRange) )
					//{
					//	pcsAgpdAI2Template->m_csFellowWorker.m_lRange = m_csAuExcelTxtLib.GetDataToInt(lColumnCounter, lRow);
					//}
					else if( !strcmpi( pstrColumnName, AGPMAI2_FellowWorkerTID ) )
					{
						//스크림에 응답할 거리와 응답할 동족 몬스터의 수를 센다.
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter+2, lStartRow );
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+3, lStartRow );

						if( pstrData && pstrData2 )
						{
							pcsAgpdAI2Template->m_csFellowWorker.m_lRange = atoi( pstrData ) * 100;
							pcsAgpdAI2Template->m_csFellowWorker.m_lCount = atoi( pstrData2 );
							pcsAgpdAI2Template->m_csFellowWorker.m_lFellowWorkerTotalCount = atoi( pstrData2 );
						}

						for( int lRow=lStartRow; lRow<lEndRow; ++lRow )
						{
							pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lRow );	//동족 TID
							pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lRow );//동족 이름

							if( pstrData && pstrData2 )
							{
								AgpdCharacterTemplate		*pcsCharacterTemplate;
								INT32			lFellowWorkerTID;
								
								lFellowWorkerTID = atoi( pstrData );

								pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( lFellowWorkerTID );

								if( pcsCharacterTemplate )
								{
									if( !strcmp( pcsCharacterTemplate->m_szTName, pstrData2 ) )
									{
										SetEmptyFellowWorkerSlot( &pcsAgpdAI2Template->m_csFellowWorker, lFellowWorkerTID, pstrData2 );
									}
								}
							}
						}												
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_FollowerTID ) )
					{
						INT32			lFollowerTID, lFollowerCount;
						INT32			lMaxCumulativeFollower;
						INT32			lMinUpkeepFollower;
						INT32			lFollowerRoyalty;
						char			*pstrFollowerName;

						for( int lRow=lStartRow; lRow<lEndRow; ++lRow )
						{
							//FollowerTID
							pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lRow );
							pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter+1, lRow );
							pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter+2, lRow );

							if( pstrData && pstrData2 && pstrData3 )
							{
								lFollowerTID = atoi( pstrData );
								pstrFollowerName = pstrData2;
								lFollowerCount = atoi( pstrData3 );

								if( (lFollowerTID > 0 ) && (lFollowerCount > 0) )
								{
									SetEmptyFollowerSlot( &pcsAgpdAI2Template->m_csFollower, lFollowerTID, lFollowerCount );
								}
							}
						}

						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter+3, lStartRow );

						if( pstrData )
						{
							lMaxCumulativeFollower = atoi( pstrData );

							if( lMaxCumulativeFollower > 0 )
							{
								pcsAgpdAI2Template->m_csFollower.m_lMaxCumulativeFollower = lMaxCumulativeFollower;
							}
						}

						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter+4, lStartRow );

						if( pstrData )
						{
							lMinUpkeepFollower = atoi( pstrData );

							if( lMinUpkeepFollower > 0 )
							{
								pcsAgpdAI2Template->m_csFollower.m_lMinUpkeepFollower = lMinUpkeepFollower;
							}
						}

						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter+5, lStartRow );

						if( pstrData )
						{
							lFollowerRoyalty = atoi( pstrData );

							if( lFollowerRoyalty > 0 )
							{
								pcsAgpdAI2Template->m_csFollower.m_lMinUpkeepFollower = lFollowerRoyalty;
							}
						}
					}
					//이뮨은 아직 사용하고 있지 않음.
					else if( !strcmpi( pstrColumnName, AGPMAI2_Immunity ) )
					{
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_Attribute ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						if( !strcmp( pstrData, "고정" ) )
						{
							pcsAgpdAI2Template->m_bDontMove = TRUE;
						}
						else
						{
							pcsAgpdAI2Template->m_bDontMove = FALSE;
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_SKill1 ) || !strcmpi( pstrColumnName, AGPMAI2_SKill2 ) || !strcmpi( pstrColumnName, AGPMAI2_SKill3 ) || !strcmpi( pstrColumnName, AGPMAI2_SKill4 ) || !strcmpi( pstrColumnName, AGPMAI2_SKill5 ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+1 );
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+2 );
						pstrData4 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+3 );
						pstrData5 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+4 );
						pstrData6 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+5 );
						pstrData7 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+6 );
						pstrData8 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+7 );
						pstrData9 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+8 );

	//					SetSkill( pcsAgpdAI2Template, pstrData, pstrData2, pstrData3, pstrData4, pstrData5, pstrData6, pstrData7, pstrData8, pstrData9  );
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_Item1 ) || !strcmpi( pstrColumnName, AGPMAI2_Item2 ) || !strcmpi( pstrColumnName, AGPMAI2_Item3 ) || !strcmpi( pstrColumnName, AGPMAI2_Item4 ) || !strcmpi( pstrColumnName, AGPMAI2_Item5 ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+1 );
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+2 );
						pstrData4 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+3);
						pstrData5 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+4 );
						pstrData6 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+5 );
						pstrData7 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+6 );

					//	SetItem( pcsAgpdAI2Template, pstrData, pstrData2, pstrData3, pstrData4, pstrData5, pstrData6, pstrData7 );
					}
					//가드 AI지정.
					else if( !strcmpi( pstrColumnName, AGPMAI2_Guard ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							if( !strcmp( pstrData, "고정" ) )
							{
								pcsAgpdAI2Template->m_eNPCAI2Type = AGPMAI2_TYPE_FIXED_NPC;
							}
							else if( !strcmp( pstrData, "배회" ) )
							{
								pcsAgpdAI2Template->m_eNPCAI2Type = AGPMAI2_TYPE_PATROL_NPC;
							}
						}
					}
					//선제공격 플래그 설정
					else if( !strcmpi( pstrColumnName, AGPMAI2_PreemptiveType ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_lPreemptiveType = atoi( pstrData );
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_PreemptiveCondition1 ) || !strcmpi( pstrColumnName, AGPMAI2_PreemptiveCondition2 ) || !strcmpi( pstrColumnName, AGPMAI2_PreemptiveCondition3 ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );
						pstrData2 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+1 );
						pstrData3 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+2 );
						pstrData4 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+3 );
						pstrData5 = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow+4 );

						SetPreemptive( pcsAgpdAI2Template, pstrData, pstrData2, pstrData3, pstrData4, pstrData5 );
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_PreemptiveRange ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							pcsAgpdAI2Template->m_fPreemptiveRange = (FLOAT)atof( pstrData ) * 100.f;
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_SummonTID ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							CHAR* token;

							token = strtok(pstrData, ";");
							while (token != NULL)
							{
								INT32 lValue = atoi(token);
								if(lValue > 0)
									pcsAgpdAI2Template->m_csSummon.m_vtTID.push_back(lValue);
								token = strtok(NULL, ";");
							}
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_SummonMaxCount ) )
					{
						pstrData = m_csAuExcelTxtLib.GetData( lColumnCounter, lStartRow );

						if( pstrData )
						{
							INT32 lValue = atoi(pstrData);
							if(lValue > 0)
								pcsAgpdAI2Template->m_csSummon.m_lMaxCount = lValue;
						}
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_SearchEnemy ) )
					{
						pcsAgpdAI2Template->m_eSearchEnemy = static_cast<eSearchEnemy>(m_csAuExcelTxtLib.GetDataToInt(lColumnCounter, lStartRow));
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_StopFight ) )
					{
						pcsAgpdAI2Template->m_eStopFight = static_cast<eStopFight>(m_csAuExcelTxtLib.GetDataToInt(lColumnCounter, lStartRow));
					}
					else if( !strcmpi( pstrColumnName, AGPMAI2_Escape ) )
					{
						pcsAgpdAI2Template->m_eEscape = static_cast<eEscape>(m_csAuExcelTxtLib.GetDataToInt(lColumnCounter, lStartRow));
					}
				}

				if (!m_aAI2Template.AddAITemplate( pcsAgpdAI2Template ))
				{
					delete pcsAgpdAI2Template;
					pcsAgpdAI2Template	= NULL;
				}

				lStartRow = lEndRow + 1;
				lReadTemplateCount++;

				if( lStartRow >=lMaxRow )
				{
					break;
				}
			}

			//열었으니 닫아야지~
			bResult = true;
			m_csAuExcelTxtLib.CloseFile();
		}
	}


	return bResult;
}

INT32 AgpmAI2::GetEmptyRaceClassSlot( AgpdAI2RaceClassAdjust *pcsRaceClassAdjust )
{
	INT32			lResult = -1;

	if( pcsRaceClassAdjust )
	{
		for( int i=0; i<AGPDAI2_MAX_RACE_CLASS_ADJUST;++i )
		{
			if( pcsRaceClassAdjust->m_csInfo[i].m_lRace == 0 && pcsRaceClassAdjust->m_csInfo[i].m_lClass == 0 )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyActionSlot( AgpdAI2ActionAdjust *pcsActionAdjust )
{
	INT32			lResult = -1;

	if( pcsActionAdjust )
	{
		for( int i=0; i<AGPDAI2_MAX_ACTION_ADJUST;++i )
		{
			if( pcsActionAdjust->m_csInfo[i].m_eAction == AGPDAI2_ACTION_ADJUST_NONE )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyFellowWorkerSlot( AgpdAI2FellowWorker *pcsFellowWorker )
{
	INT32			lResult = -1;

	if( pcsFellowWorker )
	{
		for( int i=0; i<AGPDAI2_MAX_FELLOW_WORKER_COUNT;++i )
		{
			if( pcsFellowWorker->m_csFellowWorker[i].m_lFellowWorkerTID == 0 )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

BOOL AgpmAI2::IsFellowWorker( AgpdAI2FellowWorker *pcsFellowWorker, INT32 lTID )
{
	if (0 == GetEmptyFellowWorkerSlot(pcsFellowWorker))
	{
		// FellowWorker는 지정되어 있지 않은데 TotalCount가 지정되어 있다면
		// TID에 상관없이 스크림카운트만을 이용한다는 의미
		if (0 < pcsFellowWorker->m_lFellowWorkerTotalCount)
			return TRUE;
	}

	if( pcsFellowWorker )
	{
		for( int i=0; i<pcsFellowWorker->m_lFellowWorkerTotalCount;i++ )
		{
			if( pcsFellowWorker->m_csFellowWorker[i].m_lFellowWorkerTID == lTID )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

INT32 AgpmAI2::GetMaxFellowRange(AgpdAI2Template *pcsTemplate )
{
	INT32			lResult;

	lResult = 0;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFellowWorker.m_lRange;
	}

	return lResult;
}

INT32 AgpmAI2::GetMaxFellowAcceptWorkers(AgpdAI2Template *pcsTemplate )
{
	INT32			lResult;

	lResult = 0;

	if( pcsTemplate )
	{
		lResult = pcsTemplate->m_csFellowWorker.m_lCount;
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyFollowerSlot( AgpdAI2Follower *pcsFollower )
{
	INT32			lResult = -1;

	if( pcsFollower )
	{
		for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT;++i )
		{
			if( pcsFollower->m_lFollowerInfo[i].m_lFollowerTID == 0 )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptySKillSlot( AgpdAI2Template *pcsAI2Template )
{
	INT32			lResult = -1;

	if( pcsAI2Template )
	{
		for( int i=0; i<AGPDAI2_MAX_USABLE_SKILL_COUNT; ++i )
		{
			if( pcsAI2Template->m_csUseSkill[i].m_lSkillID == 0 )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyItemSlot( AgpdAI2Template *pcsAI2Template )
{
	INT32			lResult = -1;

	if( pcsAI2Template )
	{
		for( int i=0; i<AGPDAI2_MAX_USABLE_SKILL_COUNT; ++i )
		{
			if( pcsAI2Template->m_csUseSkill[i].m_lSkillID == 0 )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

INT32 AgpmAI2::GetEmptyPreemptiveSlot( AgpdAI2Template *pcsAI2Template )
{
	INT32			lResult = -1;

	if( pcsAI2Template )
	{
		for( int i=0; i<AGPDAI2_MAX_PREEMPTIVECONDITON; ++i )
		{
			if( pcsAI2Template->m_csPreemptiveCondition[i].m_bSet == FALSE )
			{
				lResult = i;
				break;
			}
		}
	}

	return lResult;
}

BOOL AgpmAI2::SetEmptyRaceClassSlot( AgpdAI2RaceClassAdjust *pcsRaceClassAdjust, INT32 lRace, INT32 lClass, INT32 lHateAdjust, INT32 lHateClassAdjust )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsRaceClassAdjust )
	{
		INT32					lIndex;
		lIndex = GetEmptyRaceClassSlot( pcsRaceClassAdjust );

		if( lIndex >= 0 && lIndex < AGPDAI2_MAX_RACE_CLASS_ADJUST)
		{
			pcsRaceClassAdjust->m_csInfo[lIndex].m_lRace = lRace;
			pcsRaceClassAdjust->m_csInfo[lIndex].m_lClass = lClass;
			pcsRaceClassAdjust->m_csInfo[lIndex].m_lHateRate = lHateAdjust;
			pcsRaceClassAdjust->m_csInfo[lIndex].m_lHateClassRate = lHateClassAdjust;

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgpmAI2::SetEmptyActionSlot( AgpdAI2ActionAdjust *pcsActionAdjust, eAgpdAI2ActionAdjust eAdjustAction, INT32 lHateAdjust )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsActionAdjust )
	{
		INT32					lIndex;
		lIndex = GetEmptyActionSlot( pcsActionAdjust );

		if( lIndex >= 0 && lIndex < AGPDAI2_MAX_ACTION_ADJUST )
		{
			pcsActionAdjust->m_csInfo[lIndex].m_eAction = eAdjustAction;
			pcsActionAdjust->m_csInfo[lIndex].m_lHateRate = lHateAdjust;

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgpmAI2::SetEmptyFellowWorkerSlot( AgpdAI2FellowWorker *pcsFellowWorker, INT32 lFellowWorkerTID, char *pstrCharName )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsFellowWorker )
	{
		INT32					lIndex;
		lIndex = GetEmptyFellowWorkerSlot( pcsFellowWorker );

		if( lIndex >= 0 && lIndex < AGPDAI2_MAX_FELLOW_WORKER_COUNT )
		{
			pcsFellowWorker->m_csFellowWorker[lIndex].m_lFellowWorkerTID = lFellowWorkerTID;
			strcat( pcsFellowWorker->m_csFellowWorker[lIndex].m_strFellowWorkerName, pstrCharName );
			pcsFellowWorker->m_lFellowWorkerTotalCount++;

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgpmAI2::SetEmptyFollowerSlot( AgpdAI2Follower *pcsFollower, INT32 lFollowerTID, INT32 lMaxFollowerCount )
{
	BOOL				bResult;

	bResult = FALSE;

	if( pcsFollower )
	{
		INT32					lIndex;
		lIndex = GetEmptyFollowerSlot( pcsFollower );

		if( lIndex >= 0 && lIndex < AGPDAI2_MAX_FOLLOWER_COUNT )
		{
			pcsFollower->m_lFollowerInfo[lIndex].m_lFollowerTID = lFollowerTID;
			pcsFollower->m_lFollowerInfo[lIndex].m_lMaxFollowerCount = lMaxFollowerCount;
			pcsFollower->m_lFollowerTotalCount += lMaxFollowerCount;

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgpmAI2::SetSkill( AgpdAI2Template *pcsTemplate, char *pstrSkillTID, char *pstrSkillLevel, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrCountTime, char *pstrCount )
{
	if( pcsTemplate && pstrSkillTID && pstrArg && pstrScalar && pstrOperator && pstrCountTime && pstrCount )
	{
		AgpdSkillTemplate			*pcsSkillTemplate;

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate( atoi(pstrSkillTID) );

		if( pcsSkillTemplate )
		{
			if( pcsSkillTemplate->m_lID == atoi(pstrSkillTID) )
			{
				AgpdAI2UseSkill		*pcsAI2UseSkill;
				INT32				lEmptySlot;
				INT32				lScalar, lScarlarLen;

				lEmptySlot = GetEmptySKillSlot( pcsTemplate );

				if( lEmptySlot >= 0 && lEmptySlot < AGPDAI2_MAX_USABLE_SKILL_COUNT )
				{
					pcsAI2UseSkill = &pcsTemplate->m_csUseSkill[lEmptySlot];

					pcsAI2UseSkill->m_lSkillID = pcsSkillTemplate->m_lID;
					pcsAI2UseSkill->m_lSkillLevel = atoi( pstrSkillLevel );

					//어떤 값을 연산할것인가?
					if( !strcmp( pstrArg, "HP" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_HP;
					}
					else if( !strcmp( pstrArg, "MP" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_MP;
					}
					else if( !strcmp( pstrArg, "SP" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_SP;
					}

					//스칼라값 설정.
					lScalar = atoi( pstrScalar );
					lScarlarLen = (INT32)strlen( pstrScalar );
					if( pstrScalar[lScarlarLen-1] == '%' )
					{
						pcsAI2UseSkill->m_csTable.m_bPercent = true;
					}

					pcsAI2UseSkill->m_csTable.m_lParameter = lScalar;

					//연산자 지정.
					if( !strcmp( pstrOperator, "같음" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lOperator = AGPMAI2_OPERATOR_EQUAL;
					}
					else if( !strcmp( pstrOperator, "틀림" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lOperator = AGPMAI2_OPERATOR_NOT_EQUAL;
					}
					else if( !strcmp( pstrOperator, "이상" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lOperator = AGPMAI2_OPERATOR_BIGGER_EQUAL;
					}
					else if( !strcmp( pstrOperator, "이하" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lOperator = AGPMAI2_OPERATOR_LESS_EQUAL;
					}

					//횟수, 타이머지정.
					if( !strcmp( pstrCountTime, "횟수" ) )
					{
						pcsAI2UseSkill->m_csTable.m_lConditionCheck = AGPMAI2_CONDITION_COUNT;
						pcsAI2UseSkill->m_csTable.m_lMaxUsableCount = atoi(pstrCount);
					}
					else if( !strcmp( pstrCountTime, "시간" ) )
					{
						AgpdSkillTemplate	*pcsAgpdSkillTemplate;

						pcsAgpdSkillTemplate = pcsAgpdSkillTemplate = this->m_pcsAgpmSkill->GetSkillTemplate( pcsAI2UseSkill->m_lSkillID );

						if( pcsAgpdSkillTemplate )
						{
							INT32			lSkillCastDelay;
							pcsAI2UseSkill->m_csTable.m_lConditionCheck = AGPMAI2_CONDITION_TIMER;

							lSkillCastDelay = m_pcsAgpmSkill->GetCastDelay(pcsAgpdSkillTemplate, 1) + m_pcsAgpmSkill->GetRecastDelay(pcsAgpdSkillTemplate,1);

							if( atoi(pstrCount) < lSkillCastDelay )
							{
								pcsAI2UseSkill->m_csTable.m_lTimerCount = lSkillCastDelay;
							}
						}
					}
					else if( !strcmp( pstrCountTime, "무한") )
					{
						AgpdSkillTemplate	*pcsAgpdSkillTemplate;

						pcsAgpdSkillTemplate = pcsAgpdSkillTemplate = this->m_pcsAgpmSkill->GetSkillTemplate( pcsAI2UseSkill->m_lSkillID );

						if( pcsAgpdSkillTemplate )
						{
							INT32			lSkillCastDelay;
							pcsAI2UseSkill->m_csTable.m_lConditionCheck = AGPMAI2_CONDITION_INFINITY;

							lSkillCastDelay = m_pcsAgpmSkill->GetCastDelay(pcsAgpdSkillTemplate, 1) + m_pcsAgpmSkill->GetRecastDelay(pcsAgpdSkillTemplate,1);

							if( atoi(pstrCount) < lSkillCastDelay )
							{
								pcsAI2UseSkill->m_csTable.m_lTimerCount = lSkillCastDelay;
							}
						}
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::SetItem( AgpdAI2Template *pcsTemplate, char *pstrItemTID, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrCountTime, char *pstrCount )
{
	if( pcsTemplate && pstrItemTID && pstrArg && pstrScalar && pstrOperator && pstrCountTime && pstrCount )
	{
		AgpdItemTemplate			*pcsItemTemplate;

		pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( atoi(pstrItemTID) );

		if( pcsItemTemplate )
		{
			if( pcsItemTemplate->m_lID == atoi(pstrItemTID) )
			{
				AgpdAI2UseItem		*pcsAI2UseItem;
				INT32				lEmptySlot;
				INT32				lScalar, lScarlarLen;

				lEmptySlot = GetEmptyItemSlot( pcsTemplate );

				if( lEmptySlot >= 0 && lEmptySlot < AGPDAI2_MAX_USABLE_ITEM_COUNT )
				{
					pcsAI2UseItem = &pcsTemplate->m_csUseItem[lEmptySlot];

					pcsAI2UseItem->m_lItemID = pcsItemTemplate->m_lID;

					//어떤 값을 연산할것인가?
					if( !strcmp( pstrArg, "HP" ) )
					{
						pcsAI2UseItem->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_HP;
					}
					else if( !strcmp( pstrArg, "MP" ) )
					{
						pcsAI2UseItem->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_MP;
					}
					else if( !strcmp( pstrArg, "SP" ) )
					{
						pcsAI2UseItem->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_SP;
					}

					//스칼라값 설정.
					lScalar = atoi( pstrScalar );
					lScarlarLen = (INT32)strlen( pstrScalar );
					if( pstrScalar[lScarlarLen-1] == '%' )
					{
						pcsAI2UseItem->m_csTable.m_bPercent = true;
					}

					pcsAI2UseItem->m_csTable.m_lParameter = lScalar;

					//연산자 지정.
					if( !strcmp( pstrOperator, "같음" ) )
					{
						pcsAI2UseItem->m_csTable.m_lOperator = AGPMAI2_OPERATOR_EQUAL;
					}
					else if( !strcmp( pstrOperator, "틀림" ) )
					{
						pcsAI2UseItem->m_csTable.m_lOperator = AGPMAI2_OPERATOR_NOT_EQUAL;
					}
					else if( !strcmp( pstrOperator, "이상" ) )
					{
						pcsAI2UseItem->m_csTable.m_lOperator = AGPMAI2_OPERATOR_BIGGER_EQUAL;
					}
					else if( !strcmp( pstrOperator, "이하" ) )
					{
						pcsAI2UseItem->m_csTable.m_lOperator = AGPMAI2_OPERATOR_LESS_EQUAL;
					}

					//횟수, 타이머지정.
					if( !strcmp( pstrCountTime, "횟수" ) )
					{
						pcsAI2UseItem->m_csTable.m_lConditionCheck = AGPMAI2_CONDITION_COUNT;
						pcsAI2UseItem->m_csTable.m_lMaxUsableCount = atoi(pstrCount);
					}
					else if( !strcmp( pstrCountTime, "시간" ) )
					{
						pcsAI2UseItem->m_csTable.m_lConditionCheck = AGPMAI2_CONDITION_TIMER;
						pcsAI2UseItem->m_csTable.m_lTimerCount = atoi(pstrCount);
					}
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::SetPreemptive( AgpdAI2Template *pcsTemplate, char *pstrAggroTarget, char *pstrArg, char *pstrScalar, char *pstrOperator, char *pstrAccept )
{
	if( pcsTemplate && pstrAggroTarget && pstrArg && pstrScalar && pstrOperator && pstrAccept )
	{
		AgpdAI2Preemptive	*pcsAgpdAI2Preemptive;
		INT32				lEmptySlot;
		INT32				lScalar, lScarlarLen;

		lEmptySlot = GetEmptyPreemptiveSlot( pcsTemplate );

		if( lEmptySlot >= 0 && lEmptySlot < AGPDAI2_MAX_PREEMPTIVECONDITON )
		{
			pcsAgpdAI2Preemptive = &pcsTemplate->m_csPreemptiveCondition[lEmptySlot];

			pcsAgpdAI2Preemptive->m_bSet = TRUE;

			//Set AggroTarget
			if( !strcmp( pstrAggroTarget, "타겟" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lAggroParameter = AGPMAI2_TARGET_AGGRO_TARGET;
			}
			else if( !strcmp( pstrAggroTarget, "몬스터" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lAggroParameter = AGPMAI2_TARGET_AGGRO_MOBSELF;
			}
			else if( !strcmp( pstrAggroTarget, "시간" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lAggroParameter = AGPMAI2_TARGET_AGGRO_TIME;
			}
			else if( !strcmp( pstrAggroTarget, "날씨" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lAggroParameter = AGPMAI2_TARGET_AGGRO_NATURE;
			}

			//Arg
			if( !strcmp( pstrArg, "LV" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_LV;
			}
			else if( !strcmp( pstrArg, "HP" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_HP;
			}
			else if( !strcmp( pstrArg, "SP" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_SP;
			}
			else if( !strcmp( pstrArg, "MP" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lTargetParameter = AGPMAI2_TARGET_PARAMETER_MP;
			}

			//스칼라값 설정.
			lScalar = atoi( pstrScalar );
			lScarlarLen = (INT32)strlen( pstrScalar );
			if( pstrScalar[lScarlarLen-1] == '%' )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_bPercent = true;
			}

			pcsAgpdAI2Preemptive->m_csTable.m_lParameter = lScalar;

			//연산자 지정.
			if( !strcmp( pstrOperator, "같음" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lOperator = AGPMAI2_OPERATOR_EQUAL;
			}
			else if( !strcmp( pstrOperator, "틀림" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lOperator = AGPMAI2_OPERATOR_NOT_EQUAL;
			}
			else if( !strcmp( pstrOperator, "이상" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lOperator = AGPMAI2_OPERATOR_BIGGER_EQUAL;
			}
			else if( !strcmp( pstrOperator, "이하" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_lOperator = AGPMAI2_OPERATOR_LESS_EQUAL;
			}

			if( !strcmp( pstrAccept, "금지" ) )
			{
				pcsAgpdAI2Preemptive->m_csTable.m_bAggroAccept = FALSE;
			}
			else
			{
				pcsAgpdAI2Preemptive->m_csTable.m_bAggroAccept = TRUE;
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::ResetRemoveBossAI(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	if( m_pcsAgpmCharacter->IsMonster( pcsCharacter ) )
	{
		AgpdAI2ADChar		*pcsAgpdAI2ADChar;
		AgpdAI2ADChar		*pcsAgpdAI2ADTempChar;
		AgpdCharacter		*pcsTempCharacter;

		pcsAgpdAI2ADChar = GetCharacterData( pcsCharacter );

		if( pcsAgpdAI2ADChar )
		{
			//보스인 경우.
			if( GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) > 0 && CheckIsBossMob(pcsAgpdAI2ADChar))
			{
				pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower = 0;
				pcsAgpdAI2ADChar->m_lCurrentFollowers = 0;

				for( int lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; lCounter++ )
				{
					pcsTempCharacter = pcsAgpdAI2ADChar->m_pcsFollower[lCounter];

					if( pcsTempCharacter && pcsTempCharacter->m_Mutex.WLock() )
					{
						//부하몹들의 Boss포인터를 NULL로 세팅한다.
						pcsAgpdAI2ADTempChar = GetCharacterData( pcsTempCharacter );

						if( pcsAgpdAI2ADTempChar )
						{
							pcsAgpdAI2ADTempChar->m_pcsBossCharacter = NULL;
						}

						pcsTempCharacter->m_Mutex.Release();
					}
				}
			}

			//보스가 있는 부하 몹인 경우
			if( pcsAgpdAI2ADChar->m_pcsBossCharacter && pcsAgpdAI2ADChar->m_pcsBossCharacter->m_Mutex.WLock() )
			{
				pcsAgpdAI2ADTempChar = GetCharacterData( pcsAgpdAI2ADChar->m_pcsBossCharacter );

				if( pcsAgpdAI2ADTempChar )
				{
					//보스몹의 부하 리스트에서 자신을 삭제한다.
					for( int lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; lCounter++ )
					{
						if( pcsAgpdAI2ADTempChar->m_pcsFollower[lCounter] == pcsCharacter )
						{
							pcsAgpdAI2ADTempChar->m_pcsFollower[lCounter] = NULL;
							pcsAgpdAI2ADTempChar->m_lCurrentFollowers--;
							break;
						}							
					}
				}
			
				pcsAgpdAI2ADChar->m_pcsBossCharacter->m_Mutex.Release();
			}
		}
	}

	return TRUE;
}

BOOL AgpmAI2::CBUpdateActionStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmAI2			*pThis			= (AgpmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT16			nOldStatus		= *(INT16 *)		pCustData;

	if (nOldStatus != AGPDCHAR_STATUS_DEAD &&
		pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		// 죽은 놈이다.
		pThis->ResetRemoveBossAI(pcsCharacter);
	}

	return TRUE;
}

BOOL AgpmAI2::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmAI2			*pThis			= (AgpmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	pThis->ResetRemoveBossAI(pcsCharacter);

	return TRUE;
}

BOOL AgpmAI2::CheckIsBossMob(AgpdAI2ADChar *pcsAgpdAI2ADChar)
{
	if(pcsAgpdAI2ADChar == NULL)
		return FALSE;

	BOOL bBoss = FALSE;

	// 부하를 한명이라도 데리고 있다면 이놈은 보스
	if(pcsAgpdAI2ADChar->m_lCurrentFollowers > 0)
	{
		bBoss = TRUE;
	}

	return bBoss;
}

AgpdCharacter* AgpmAI2::CheckHavePinchMonster(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return NULL;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = GetCharacterData(pcsCharacter);
	if(pcsAgpdAI2ADChar)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter)
			return pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter;
	}

	return NULL;
}

BOOL AgpmAI2::CheckHavePinchWantedMonster(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = GetCharacterData(pcsCharacter);
	if(pcsAgpdAI2ADChar)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.pRequestMobVector.empty() == FALSE)
			return TRUE;
	}

	return FALSE;
}

STMonsterPathInfo* AgpmAI2::GetMonsterPathInfo(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return NULL;

	STMonsterPathInfo *pstMonsterPathInfo = NULL;

	AgpdSpawnADChar *pcsSpawnADChar = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter);
	if(pcsSpawnADChar && pcsSpawnADChar->m_pstSpawn)
	{
		pstMonsterPathInfo = m_pcsAgpmMonsterPath->GetMonsterPath(pcsSpawnADChar->m_pstSpawn->m_szName);
	}
	
	return pstMonsterPathInfo;
}

eAgpmAI2StatePCAI AgpmAI2::GetPCAIState(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return AGPDAI2_STATE_PC_AI_NONE;

	eAgpmAI2StatePCAI eStatePCAI = AGPDAI2_STATE_PC_AI_NONE;

	if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_HOLD;
	}

	else if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_CONFUSION)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_CONFUSION;
	}

	else if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_BERSERK)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_BERSERK;
	}

	else if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_FEAR)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_FEAR;
	}

	else if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_SHRINK)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_SHRINK;
	}

	else if(pcsCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DISEASE)
	{
		eStatePCAI = AGPDAI2_STATE_PC_AI_DISEASE;
	}
	
	return eStatePCAI;
}

BOOL AgpmAI2::InitPCAIInfo(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = GetCharacterData(pcsCharacter);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI			= AGPDAI2_STATE_PC_AI_NONE;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount			= 0;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID		= 0;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam1	= 0;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam2	= 0;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam3	= 0;
	
	ZeroMemory(&pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS, sizeof(pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS));

	return TRUE;
}
