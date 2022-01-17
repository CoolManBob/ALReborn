#include "AgpmSkill.h"
#include "ApModuleStream.h"
#include "AuExcelBinaryLib.h"

BOOL AgpmSkill::StreamWriteTemplate(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[32];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	AgpdSkillTemplate	**ppcsSkillTemplate = NULL;

	// 등록된 모든 Character Template에 대해서...
	for (ppcsSkillTemplate = (AgpdSkillTemplate **) m_csAdminTemplate.GetObjectSequence(&lIndex); 
		 ppcsSkillTemplate; 
		 ppcsSkillTemplate = (AgpdSkillTemplate **) m_csAdminTemplate.GetObjectSequence(&lIndex))
	{
		sprintf(szTID, "%d", (*ppcsSkillTemplate)->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMSKILL_DATA_TYPE_TEMPLATE, *ppcsSkillTemplate, this))
			return FALSE;
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgpmSkill::StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption, BOOL bIsNewTID)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		if (bIsNewTID)
			lTID	= ++m_lMaxAddedTID;
		else
		{
			if (m_lMaxAddedTID < lTID)
				m_lMaxAddedTID	= lTID;
		}

		// Template을 추가한다.
		AgpdSkillTemplate *pcsSkillTemplate = AddSkillTemplate(lTID);
		if (!pcsSkillTemplate)
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]스킬템플릿이 널입니다.", lTID);

			TRACE( "AgpmSkill::StreamReadTemplate 스킬템플릿(%d)이 널입니다.\n" , lTID );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::StreamReadTemplate 스킬템플릿(%d)이 널입니다.\n" , lTID );
			return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMSKILL_DATA_TYPE_TEMPLATE, pcsSkillTemplate, this))
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsSkillTemplate->m_lID, pcsSkillTemplate->m_szName);

			TRACE( "AgpmSkill::StreamReadTemplate Callback returns fail!(%d).\n" , lTID );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::StreamReadTemplate Callback returns fail!(%d).\n" , lTID );
			return FALSE;
		}

		EnumCallback(AGPMSKILL_CB_INIT_SKILL_TEMPLATE, pcsSkillTemplate, NULL);
	}

	return TRUE;
}

BOOL AgpmSkill::StreamWriteSpecialize(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[32];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	AgpdSkillSpecializeTemplate	**ppcsSkillSpecializeTemplate = NULL;

	// 등록된 모든 Character Template에 대해서...
	for (ppcsSkillSpecializeTemplate = (AgpdSkillSpecializeTemplate **) m_csAdminSpecializeTemplate.GetObjectSequence(&lIndex); 
		 ppcsSkillSpecializeTemplate; 
		 ppcsSkillSpecializeTemplate = (AgpdSkillSpecializeTemplate **) m_csAdminSpecializeTemplate.GetObjectSequence(&lIndex))
	{
		sprintf(szTID, "%d", (*ppcsSkillSpecializeTemplate)->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMSKILL_DATA_TYPE_SPECIALIZE, *ppcsSkillSpecializeTemplate, this))
			return FALSE;
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgpmSkill::StreamReadSpecialize(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; i++)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		AgpdSkillSpecializeTemplate *pcsSpecialize = AddSpecialize(lTID);
		if (!pcsSpecialize)
			return FALSE;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMSKILL_DATA_TYPE_SPECIALIZE, pcsSpecialize, this))
			return FALSE;

		EnumCallback(AGPMSKILL_CB_INIT_SPECIALIZE, pcsSpecialize, NULL);
	}

	return TRUE;
}

BOOL AgpmSkill::TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpdSkillTemplate		*pcsSkillTemplate	= (AgpdSkillTemplate *)	pData;
	AgpmSkill				*pThis				= (AgpmSkill *)			pClass;

	// Skill Template의 값들을 Write한다.
	if (!pStream->WriteValue(AGPMSKILL_INI_NAME_NAME, pcsSkillTemplate->m_szName))
		return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_TYPE, pcsSkillTemplate->m_nType))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_CLASS, pcsSkillTemplate->m_nClass))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_RACE, pcsSkillTemplate->m_nRace))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_UPGRADE_DURATION, pcsSkillTemplate->m_stUpgradeDIRT.nDuration))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_UPGRADE_INTENSITY, pcsSkillTemplate->m_stUpgradeDIRT.nIntensity))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_UPGRADE_RANGE, pcsSkillTemplate->m_stUpgradeDIRT.nRange))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_UPGRADE_TARGET, pcsSkillTemplate->m_stUpgradeDIRT.nTarget))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_PHASE, pcsSkillTemplate->m_nPhase))
	//	return FALSE;
	//if (!pStream->WriteValue(AGPMSKILL_INI_NAME_DESCRIPTION, pcsSkillTemplate->m_szDescription))
	//	return FALSE;

	return TRUE;
}

BOOL AgpmSkill::TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR			*szValueName;
	AgpmSkill			*pThis				= (AgpmSkill *)			pClass;
	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *)	pData;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMSKILL_INI_NAME_NAME))
		{
			pStream->GetValue(pcsSkillTemplate->m_szName, AGPMSKILL_MAX_SKILL_NAME);
			pThis->m_csAdminTemplate.AddTemplateStringKey(pcsSkillTemplate->m_lID, pcsSkillTemplate->m_szName);
		}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_TYPE))
		//{
		//	pStream->GetValue(&pcsSkillTemplate->m_nType);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_CLASS))
		//{
		//	pStream->GetValue(&pcsSkillTemplate->m_nClass);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_RACE))
		//{
		//	pStream->GetValue(&pcsSkillTemplate->m_nRace);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_UPGRADE_DURATION))
		//{
		//	pStream->GetValue((INT32 *) &pcsSkillTemplate->m_stUpgradeDIRT.nDuration);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_UPGRADE_INTENSITY))
		//{
		//	pStream->GetValue((INT32 *) &pcsSkillTemplate->m_stUpgradeDIRT.nIntensity);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_UPGRADE_RANGE))
		//{
		//	pStream->GetValue((INT32 *) &pcsSkillTemplate->m_stUpgradeDIRT.nRange);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_UPGRADE_TARGET))
		//{
		//	pStream->GetValue((INT32 *) &pcsSkillTemplate->m_stUpgradeDIRT.nTarget);
		//}
		//else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_PHASE))
		//{
		//	pStream->GetValue(&pcsSkillTemplate->m_nPhase);
		//}
		else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_DESCRIPTION))
		{
			//pStream->GetValue(pcsSkillTemplate->m_szDescription, AGPMSKILL_MAX_SKILL_DESCRIPTION);
		}
	}

	return TRUE;
}

BOOL AgpmSkill::SpecializeWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmSkill					*pThis				= (AgpmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	if (!pStream->WriteValue(AGPMSKILL_INI_NAME_SPECIALIZE_NAME, pcsSpecialize->m_szName))
		return FALSE;
	if (!pStream->WriteValue(AGPMSKILL_INI_NAME_SPECIALIZE_DESCRIPTION, pcsSpecialize->m_szDescription))
		return FALSE;
	if (!pStream->WriteValue(AGPMSKILL_INI_NAME_SPECIALIZE_RESTRICTION, pcsSpecialize->m_szRestriction))
		return FALSE;
	if (!pStream->WriteValue(AGPMSKILL_INI_NAME_SPECIALIZE_DURATION, pcsSpecialize->m_lDuration))
		return FALSE;

	return TRUE;
}

BOOL AgpmSkill::SpecializeReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR					*szValueName		= NULL;
	AgpmSkill					*pThis				= (AgpmSkill *)						pClass;
	AgpdSkillSpecializeTemplate	*pcsSpecialize		= (AgpdSkillSpecializeTemplate *)	pData;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMSKILL_INI_NAME_SPECIALIZE_NAME))
		{
			pStream->GetValue(pcsSpecialize->m_szName, AGPMSKILL_MAX_SPECIALIZE_NAME);
		}
		else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_SPECIALIZE_DESCRIPTION))
		{
			pStream->GetValue(pcsSpecialize->m_szDescription, AGPMSKILL_MAX_SPECIALIZE_DESCRIPTION);
		}
		else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_SPECIALIZE_RESTRICTION))
		{
			pStream->GetValue(pcsSpecialize->m_szRestriction, AGPMSKILL_MAX_SPECIALIZE_RESTRICTION);
		}
		else if (!strcmp(szValueName, AGPMSKILL_INI_NAME_SPECIALIZE_DURATION))
		{
			pStream->GetValue(&pcsSpecialize->m_lDuration);
		}
	}

	return TRUE;
}

BOOL AgpmSkill::AttachedTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
	{
		TRACE( "AgpmSkill::AttachedTemplateWriteCB 입력데이타에 이상이 있습니다." );
//		TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::AttachedTemplateWriteCB 입력데이타에 이상이 있습니다." );
		return FALSE;
	}

	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;
	AgpmSkill	*pThis				= (AgpmSkill *)	pClass;

	AgpdSkillTemplateAttachData *pcsTemplateAttachData = pThis->GetAttachSkillTemplateData(pcsBaseTemplate);

	for (int i = 0; i < AGPMSKILL_MAX_DEFAULT_SKILL; i++)
	{
//		if (!strlen(pcsTemplateAttachData->m_aszDefaultTName[i]))
		if((!pcsTemplateAttachData->m_aszDefaultTName[i]) || (!strlen(pcsTemplateAttachData->m_aszDefaultTName[i])))
			break;

		CHAR	szName[32];

		sprintf(szName, "%s%d", AGPMSKILL_INI_NAME_DEFAULT_SKILL_TNAME, i);

		if (!pStream->WriteValue(szName, pcsTemplateAttachData->m_aszDefaultTName[i]))
		{
			TRACE( "AgpmSkill::AttachedTemplateWriteCB WriteValue 실패1" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::AttachedTemplateWriteCB WriteValue 실패1" );
			return FALSE;
		}
	}

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; i++)
	{
//		if (!strlen(pcsTemplateAttachData->m_aszUsableSkillTName[i]))		
		if(!pcsTemplateAttachData->m_aszUsableSkillTName[i])
			continue;

		CHAR	szName[32];

		sprintf(szName, "%s%d", AGPMSKILL_INI_NAME_USABLE_SKILL_TNAME, i);

		if (!pStream->WriteValue(szName, pcsTemplateAttachData->m_aszUsableSkillTName[i]))
		{
			TRACE( "AgpmSkill::AttachedTemplateWriteCB WriteValue 실패2" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::AttachedTemplateWriteCB WriteValue 실패2" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmSkill::AttachedTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
	{
		TRACE( "AgpmSkill::AttachedTemplateReadCB 입력데이타에 이상이 있습니다." );
		return FALSE;
	}

	const CHAR	*szValueName;
	AgpmSkill	*pThis				= (AgpmSkill *)	pClass;
	ApBase		*pcsBaseTemplate	= (ApBase *)	pData;

	AgpdSkillTemplateAttachData *pcsTemplateAttachData = pThis->GetAttachSkillTemplateData(pcsBaseTemplate);

	int nDefaultSkillTID = 0;
	int	nUsableSkillTID = 0;

	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strncmp(szValueName, AGPMSKILL_INI_NAME_DEFAULT_SKILL_TNAME, strlen(AGPMSKILL_INI_NAME_DEFAULT_SKILL_TNAME)))
			pStream->GetValue(pcsTemplateAttachData->m_aszDefaultTName[nDefaultSkillTID++], AGPMSKILL_MAX_SKILL_NAME);
		else if (!strncmp(szValueName, AGPMSKILL_INI_NAME_USABLE_SKILL_TNAME, strlen(AGPMSKILL_INI_NAME_USABLE_SKILL_TNAME)))
		{
			CHAR	szBuffer[128];
			ZeroMemory(szBuffer, sizeof(CHAR) * 128);

			pStream->GetValue(szBuffer, AGPMSKILL_MAX_SKILL_NAME);

			AgpdSkillTemplate	*pcsSkillTemplate	= pThis->GetSkillTemplate(szBuffer);
			if (!pcsSkillTemplate)
			{
				TRACE( "AgpmSkill::AttachedTemplateReadCB '%s'스킬 템플릿이 존재하지 않습니다." , szBuffer );
//				TRACEFILE(ALEF_ERROR_FILENAME, "AgpmSkill::AttachedTemplateReadCB '%s'스킬 템플릿이 존재하지 않습니다." , szBuffer );
				//return FALSE;

				pcsTemplateAttachData->m_aszUsableSkillTName[ nUsableSkillTID ] = "InvalidSkillName";
				nUsableSkillTID++;
				continue;
			}

			pcsTemplateAttachData->m_aszUsableSkillTName[nUsableSkillTID++] = pcsSkillTemplate->m_szName;
		}
	}

	return TRUE;
}

BOOL AgpmSkill::ReadSkillConstTxt(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	using namespace AuExcel;

	AuExcelLib * pExcel = LoadExcelFile( szFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmSkill::ReadSkillConstTxt() Error (1) !!!\n");
		return FALSE;
	}

	INT16			nRow			= pExcel->GetRow();
	INT16			nColumn			= pExcel->GetColumn();

	INT16			nNumLevel		= 0;

	INT16			nCurRow			= 1;
	INT16			nCurColumn		= 1;

	INT16			nReadSkill		= 0;

	for ( ;nCurRow < nRow  ; )
	{
		CHAR			*szSkillName	= pExcel->GetData(0, nCurRow);
		if (!szSkillName || !strlen(szSkillName))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdSkillTemplate	*pcsSkillTemplate	= GetSkillTemplate(szSkillName);
		if (!pcsSkillTemplate)
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		CHAR			*szBuffer		= NULL;

		//++nCurRow;

		szBuffer = pExcel->GetData(nCurColumn, nCurRow);
		if (!szBuffer || !strlen(szBuffer))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		INT32	lStartLevel	= atoi(szBuffer);

//		INT32	lAddNextRow	= 0;
		int		nCount		= 1;
		for (nNumLevel = lStartLevel; nNumLevel < AGPMSKILL_MAX_SKILL_CAP; ++nNumLevel)
		{
			if( nCurRow + nCount >= nRow ) break;
			szBuffer = pExcel->GetData(0, nCurRow + nCount);

			if (szBuffer && strlen(szBuffer))
			{
//				lAddNextRow = 1;
				break;
			}

			szBuffer = pExcel->GetData(1, nCurRow + nCount);

			if (!szBuffer)
				break;

			++nCount;
		}

		BOOL	bIsFactorPercent = FALSE;
		BOOL	bIsFactorMagnity = FALSE;

		nCurColumn	= 2;
		CHAR			*szColumnName	= pExcel->GetData(nCurColumn, 0);
		while (true)
		{
			// 2005.12.09. steeple 변경
			if(!szColumnName || strlen(szColumnName) < 1)
			{
				if (nCurColumn > nColumn)
					break;

				szColumnName = pExcel->GetData(++nCurColumn, 0);
				continue;
			}

			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_FACTOR_PERCENT, strlen(AGPMSKILL_EXCEL_COLUMN_FACTOR_PERCENT)))
			{
				bIsFactorPercent = TRUE;
				bIsFactorMagnity = FALSE;
			}

			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_FACTOR_MAGNIFY, strlen(AGPMSKILL_EXCEL_COLUMN_FACTOR_MAGNIFY)))
			{
				bIsFactorPercent = FALSE;
				bIsFactorMagnity = TRUE;
			}

			// 2005.09.06. steeple
			// CreateTID 는 따로 읽음.
			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_CREATURE_TID, strlen(AGPMSKILL_EXCEL_COLUMN_CREATURE_TID)))
			{
				ReadSkillConstCreatureTID(pcsSkillTemplate, *pExcel, nCurColumn, nCurRow, lStartLevel, nNumLevel);
				if(nCurColumn > nColumn)
					break;

				szColumnName = pExcel->GetData(++nCurColumn, 0);
				continue;
			}
			
			//////////////////////////////////////////////////////////////////////////
			// additional skill tid - arycoat 2008.05.
			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_ADDITIONAL_SKILLTID, strlen(AGPMSKILL_EXCEL_COLUMN_ADDITIONAL_SKILLTID)))
			{
				ReadSkillConstAdditionalSkillTID(pcsSkillTemplate, *pExcel, nCurColumn, nCurRow, lStartLevel, nNumLevel);
				if(nCurColumn > nColumn)
					break;

				szColumnName = pExcel->GetData(++nCurColumn, 0);
				continue;
			}
			
			INT16	nColumnIndex	= GetConstFactorIndex(szColumnName);
			INT16	nOriginalColumnIndex	= nColumnIndex;
			if (nColumnIndex >= 0)
			{
				if (bIsFactorPercent)
					nColumnIndex += AGPMSKILL_CONST_PERCENT_START;
				else if (bIsFactorMagnity)
				{
					nColumnIndex = ConvertFactorMagnifyIndex(nColumnIndex);
				}

				INT32	lDataIndex	= 0;
				for (int i = lStartLevel; i <= nNumLevel; ++i)
				{
					szBuffer = pExcel->GetData(nCurColumn, nCurRow + lDataIndex++);
					if (!szBuffer || !strlen(szBuffer))
						continue;
						
					pcsSkillTemplate->m_fUsedConstFactor[nColumnIndex][i] = (float) atof(szBuffer);

					if (nColumnIndex == AGPMSKILL_CONST_RANGE ||
						nColumnIndex == AGPMSKILL_CONST_TARGET_AREA_R ||
						nColumnIndex == AGPMSKILL_CONST_TARGET_AREA_F1 ||
						nColumnIndex == AGPMSKILL_CONST_TARGET_AREA_F2 ||
						nColumnIndex == AGPMSKILL_CONST_MOVE_DISTANCE)
						pcsSkillTemplate->m_fUsedConstFactor[nColumnIndex][i] *= 100;

					if (nOriginalColumnIndex == AGPMSKILL_CONST_POINT_MOVEMENT)
						pcsSkillTemplate->m_fUsedConstFactor[nColumnIndex + 1][i] = pcsSkillTemplate->m_fUsedConstFactor[nColumnIndex][i];

					if (nColumnIndex == AGPMSKILL_CONST_COST_HP)
						pcsSkillTemplate->m_lCostType |= AGPMSKILL_COST_HP;
					if (nColumnIndex == AGPMSKILL_CONST_COST_MP)
						pcsSkillTemplate->m_lCostType |= AGPMSKILL_COST_MP;
					if (nColumnIndex == AGPMSKILL_CONST_COST_SP)
						pcsSkillTemplate->m_lCostType |= AGPMSKILL_COST_SP;
					if (nColumnIndex == AGPMSKILL_CONST_COST_ARROW)
						pcsSkillTemplate->m_lCostType |= AGPMSKILL_COST_ARROW;

					// end effect
					if (nColumnIndex == AGPMSKILL_CONST_ENDSKILL_COST_HP)
					{
						//pcsSkillTemplate->m_lEffectType[0] |= AGPMSKILL_ENDSKILL_CONSUME_HP;
						pcsSkillTemplate->m_lEndEffectType |= AGPMSKILL_ENDSKILL_CONSUME_HP;
					}
				}
			}

			if (nCurColumn > nColumn)
				break;

			szColumnName = pExcel->GetData(++nCurColumn, 0);
		}

		++nReadSkill;

		EnumCallback(AGPMSKILL_CB_SET_CONST_FACTOR, pcsSkillTemplate, (PVOID) nNumLevel);

		nCurRow += nNumLevel;/* - lStartLevel  + lAddNextRow*/;
		nCurColumn = 1;

		if (nCurRow > nRow)
			break;
	}

	if (nReadSkill < 1)
		return FALSE;

	return TRUE;
}

// 2007.10.18. steeple
// Const2 작업.
BOOL AgpmSkill::ReadSkillConst2Txt(CHAR* szFile, BOOL bDecryption)
{
	if(!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16 nRow = csExcelTxtLib.GetRow();
	INT16 nColumn = csExcelTxtLib.GetColumn();

	INT16 nNumLevel = 0;

	INT16 nCurRow = 1;
	INT16 nCurColumn = 1;

	INT16 nReadSkill = 0;

	while(true)
	{
		CHAR* szSkillName = csExcelTxtLib.GetData(0, nCurRow);
		if(!szSkillName || !strlen(szSkillName))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdSkillTemplate* pcsSkillTemplate = GetSkillTemplate(szSkillName);
		if(!pcsSkillTemplate)
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		CHAR* szBuffer = NULL;
		szBuffer = csExcelTxtLib.GetData(nCurColumn, nCurRow);
		if(!szBuffer || !strlen(szBuffer))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		INT32 lStartLevel = atoi(szBuffer);
		int nCount = 1;
		for(nNumLevel = lStartLevel; nNumLevel < AGPMSKILL_MAX_SKILL_CAP; ++nNumLevel)
		{
			szBuffer = csExcelTxtLib.GetData(0, nCurRow + nCount);
			if(szBuffer && strlen(szBuffer))
			{
				break;
			}

			szBuffer = csExcelTxtLib.GetData(1, nCurRow + nCount);

			if(!szBuffer)
				break;

			++nCount;
		}

		// Const2 에서는 Magnify 뺐다.
		BOOL bIsFactorPercent = FALSE;

		nCurColumn	= 2;
		CHAR* szColumnName = csExcelTxtLib.GetData(nCurColumn, 0);
		while(true)
		{
			// 2005.12.09. steeple 변경
			if(!szColumnName || strlen(szColumnName) < 1)
			{
				if (nCurColumn > nColumn)
					break;

				szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
				continue;
			}

			if(!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_FACTOR_PERCENT, strlen(AGPMSKILL_EXCEL_COLUMN_FACTOR_PERCENT)))
			{
				bIsFactorPercent = TRUE;
			}

			if(!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_FACTOR_IGNORE_PHYSICAL_DEFENCE_PERCENT, strlen(AGPMSKILL_EXCEL_COLUMN_FACTOR_IGNORE_PHYSICAL_DEFENCE_PERCENT)))
			{
				bIsFactorPercent = FALSE;
			}

			// 2007.11.14. steeple
			// 개별로 LevelUp 해주는 스킬 콘스트
			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_SPECIFIC_SKILL_LEVELUP, strlen(AGPMSKILL_EXCEL_COLUMN_SPECIFIC_SKILL_LEVELUP)))
			{
				ReadSkillConstLevelUpSkillTID(pcsSkillTemplate, csExcelTxtLib, nCurColumn, nCurRow, lStartLevel, nNumLevel);
				if(nCurColumn > nColumn)
					break;

				szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
				continue;
			}

			INT16 nColumnIndex	= GetConstFactorIndex(szColumnName, 2);
			INT16 nOriginalColumnIndex	= nColumnIndex;
			if(nColumnIndex >= 0)
			{
				if(bIsFactorPercent)
					nColumnIndex += AGPMSKILL_CONST_PERCENT_START;

				INT32 lDataIndex	= 0;
				for(int i = lStartLevel; i <= nNumLevel; ++i)
				{
					szBuffer = csExcelTxtLib.GetData(nCurColumn, nCurRow + lDataIndex++);
					if(!szBuffer || !strlen(szBuffer))
						continue;

					pcsSkillTemplate->m_fUsedConstFactor2[nColumnIndex][i] = (float) atof(szBuffer);

					if(nOriginalColumnIndex == AGPMSKILL_CONST_POINT_MOVEMENT)
						pcsSkillTemplate->m_fUsedConstFactor2[nColumnIndex + 1][i] = pcsSkillTemplate->m_fUsedConstFactor2[nColumnIndex][i];
				}
			}

			if(nCurColumn > nColumn)
				break;

			szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
		}

		++nReadSkill;

		//EnumCallback(AGPMSKILL_CB_SET_CONST_FACTOR, pcsSkillTemplate, (PVOID) nNumLevel);

		nCurRow += nNumLevel;
		nCurColumn = 1;

		if(nCurRow > nRow)
			break;
	}

	if(nReadSkill < 1)
		return FALSE;

	return TRUE;
}

BOOL AgpmSkill::ReadSkillSpecTxt(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	INT16			nCurRow			= 1;
	INT16			nCurColumn		= 0;

	INT16			nReadSkill		= 0;

	for ( ; ; )
	{
		CHAR			*szSkillName	= csExcelTxtLib.GetData(0, nCurRow);
		if (!szSkillName || !strlen(szSkillName))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdSkillTemplate	*pcsSkillTemplate	= GetSkillTemplate(szSkillName);
		if (!pcsSkillTemplate)
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		CHAR			*szBuffer				= NULL;
		CHAR			*szSpecialStatusArg		= NULL;

		nCurColumn	= 1;

		INT16			nIndex			= (-1);
		CHAR			*szColumnName	= csExcelTxtLib.GetData(nCurColumn, 0);
		while (szColumnName && strlen(szColumnName))
		{
			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_CONDITION, strlen(AGPMSKILL_EXCEL_COLUMN_CONDITION)))
				++nIndex;

			if (!strncmp(szColumnName, AGPMSKILL_EXCEL_COLUMN_SPECIAL_STATUS, strlen(AGPMSKILL_EXCEL_COLUMN_SPECIAL_STATUS)))
			{
				szSpecialStatusArg = csExcelTxtLib.GetData(nCurColumn+1, nCurRow);
			}

			szBuffer	= csExcelTxtLib.GetData(nCurColumn, nCurRow);
			if (szBuffer && strlen(szBuffer))
			{
				SetSkillType(pcsSkillTemplate, szColumnName, szBuffer, nIndex, szSpecialStatusArg);
			}

			if (nCurColumn > nColumn)
				break;

			szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
		}

		++nReadSkill;

		++nCurRow;

		if (nCurRow > nRow)
			break;
	}

	if (nReadSkill < 1)
		return FALSE;

	return TRUE;
}

//BOOL AgpmSkill::ReadSkillMasteryTxt(CHAR *szFile, BOOL bDecryption)
//{
//	if (!szFile || !strlen(szFile))
//		return FALSE;
//
//	AuExcelTxtLib	csExcelTxtLib;
//
//	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
//		return FALSE;
//
//	INT16			nRow			= csExcelTxtLib.GetRow();
//	INT16			nColumn			= csExcelTxtLib.GetColumn();
//
//	INT16			nCurRow			= 0;
//	INT16			nCurColumn		= 0;
//
//	for ( ; ; )
//	{
//		CHAR			*szClassName	= csExcelTxtLib.GetData(0, nCurRow++);
//		if (!szClassName || !strlen(szClassName))
//		{
//			if (nCurRow > nRow)
//				break;
//
//			continue;
//		}
//
//		for (int nIndex	= 0; nIndex < strlen(szClassName); ++nIndex)
//		{
//			if (szClassName[nIndex] == '/')
//			{
//				break;
//			}
//		}
//
//		if (nIndex == strlen(szClassName))
//			continue;
//
//		CHAR	szRaceName[32];
//		ZeroMemory(szRaceName, sizeof(CHAR) * 32);
//
//		strncpy(szRaceName, szClassName, nIndex);
//
//		// szRaceName을 보고 lRace 타입을 가져온다.
//		INT32	lRaceType	= m_pagpmFactors->FindCharacterRaceIndex(szRaceName);
//		if (lRaceType >= AURACE_TYPE_MAX || lRaceType <= AURACE_TYPE_NONE)
//		{
//			if (nCurRow > nRow)
//				break;
//
//			continue;
//		}
//
//		// szClassName을 보고 lClass 타입을 가져온다.
//		INT32	lClassType	= m_pagpmFactors->FindCharacterClassIndex(szClassName + nIndex + 1);
//		if (lClassType >= AUCHARCLASS_TYPE_MAX || lClassType <= AUCHARCLASS_TYPE_NONE)
//		{
//			if (nCurRow > nRow)
//				break;
//
//			continue;
//		}
//
//		m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_lCharRaceType	= lRaceType;
//		m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_lCharClassType	= lClassType;
//		m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_ucNumMastery	= 0;
//
//		CHAR	*szBuffer		= csExcelTxtLib.GetData(0, nCurRow++);
//		while (szBuffer && strlen(szBuffer))
//		{
//			AgpdMasteryTree	*pcsMasteryTree = &m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_csMastery[m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_ucNumMastery];
//
//			strncpy(pcsMasteryTree->m_szMasteryName, szBuffer, AGPMSKILL_MAX_MASTERY_NAME);
//
//			CHAR	*szMasteryPassiveName	= csExcelTxtLib.GetData(1, nCurRow - 1);
//
//			szBuffer = NULL;
//			szBuffer = csExcelTxtLib.GetData(0, nCurRow);
//			INT32	lNumSkill = 0;
//			while (szBuffer && strlen(szBuffer) && !strncmp("스킬트리", szBuffer, strlen("스킬트리")))
//			{
//				INT32	lDepth = 0;
//				INT32	lSPForActive	= 0;
//				INT32	lNodeIndex	= 0;
//
//				szBuffer = NULL;
//				szBuffer = csExcelTxtLib.GetData(0, ++nCurRow);
//				while (szBuffer && strlen(szBuffer) && szBuffer[0] >= '0' && szBuffer[0] <= '9')
//				{
//					lSPForActive	= atoi(szBuffer);
//
//					//pcsMasteryTree->m_csSkillTree[lNumSkill][lNodeIndex].m_ucSPForActive	= lSPForActive;
//
//					/*
//					for (int i = 1; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//						pcsMasteryTree->m_stMasterySkillTree[lNumSkill].m_csSkillNode[lDepth].m_lNeedSP[i] = pcsMasteryTree->m_stMasterySkillTree[lNumSkill].m_csSkillNode[lDepth].m_lNeedSP[0];
//					*/
//
//					//pcsMasteryTree->m_stMasterySkillTree[lNumSkill].m_csSkillNode[lDepth].m_lBaseNeedSP = pcsMasteryTree->m_stMasterySkillTree[lNumSkill].m_csSkillNode[lDepth].m_lNeedSP[0];
//					if (lDepth > 0)
//					{
//						//if (!strcmp(pcsMasteryTree->m_szMasteryName, AGPMSKILL_MASTERY_NAME_PASSIVE))
//						if (szMasteryPassiveName &&
//							szMasteryPassiveName[0] &&
//							!strcmp(szMasteryPassiveName, AGPMSKILL_MASTERY_NAME_PASSIVE))
//						{
//							for (int i = 0; i < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++i)
//							{
//								if (pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucDepth == (UINT8) (lDepth - 1))
//								{
//									pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP = pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucSPForActive;
//								}
//							}
//						}
//						else
//						{
//							for (int i = 0; i < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++i)
//							{
//								if (pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucDepth == (UINT8) (lDepth - 1))
//								{
//									pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP = lSPForActive - 1;
//								}
//							}
//						}
//					}
//
//					INT32	lNodeColumn = 1;
//
//					CHAR	*szSkillName = NULL;
//
//					AgpdSkillTemplate *pcsPrevSkillTemplate	= NULL;
//
//					BOOL	bPrevSkill	= FALSE;
//
//					for (int i = 0; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//					{
//						szSkillName = NULL;
//						szSkillName = csExcelTxtLib.GetData(lNodeColumn++, nCurRow);
//						if (!szSkillName || !strlen(szSkillName))
//							break;
//
//						if (!strncmp(szSkillName, AGPMSKILL_BELOW_LEVEL_SKILL_NAME, strlen(AGPMSKILL_BELOW_LEVEL_SKILL_NAME)))
//						{
//							bPrevSkill	= TRUE;
//
//							if (pcsPrevSkillTemplate)
//								strcpy(pcsPrevSkillTemplate->m_szBelowLevelSkillName, szSkillName + strlen(AGPMSKILL_BELOW_LEVEL_SKILL_NAME));
//
//							continue;
//						}
//
//						pcsPrevSkillTemplate = GetSkillTemplate(szSkillName);
//
//						pcsMasteryTree->m_csSkillTree[lNumSkill][lNodeIndex + i].m_ucDepth			= (UINT8) lDepth;
//						pcsMasteryTree->m_csSkillTree[lNumSkill][lNodeIndex + i].m_ucSPForActive	= (UINT8) lSPForActive;
//
//						if (pcsPrevSkillTemplate)
//							pcsMasteryTree->m_csSkillTree[lNumSkill][lNodeIndex + i].m_lSkillTID		= pcsPrevSkillTemplate->m_lID;
//					}
//
//					lNodeIndex	+= i;
//
//					if (bPrevSkill)
//						--lNodeIndex;
//
//					++lDepth;
//					szBuffer = csExcelTxtLib.GetData(0, ++nCurRow);
//				}
//
//				if (lDepth - 1 > 1)
//				{
//					for (int i = 0; i < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++i)
//					{
//						if (pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucDepth == (UINT8) (lDepth - 1))
//						{
//							//if (strcmp(pcsMasteryTree->m_szMasteryName, AGPMSKILL_MASTERY_NAME_PASSIVE))
//							if (!szMasteryPassiveName ||
//								!szMasteryPassiveName[0] ||
//								strcmp(szMasteryPassiveName, AGPMSKILL_MASTERY_NAME_PASSIVE))
//							{
//								/*
//								pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP	= 
//									pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucSPForActive +
//									pcsMasteryTree->m_csSkillTree[lNumSkill][0].m_ucMaxInputSP + 1 -
//									pcsMasteryTree->m_csSkillTree[lNumSkill][0].m_ucSPForActive;
//								*/
//
//								if (pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP = 0)
//									pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP	= 100;
//							}
//							else
//							{
//								pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP	=
//									pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucSPForActive;
//							}
//
//							if (pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP > 100)
//								pcsMasteryTree->m_csSkillTree[lNumSkill][i].m_ucMaxInputSP = 100;
//						}
//					}
//				}
//
//				++lNumSkill;
//			}
//
//			++m_csSkillMasteryTemplate[(lRaceType - 1) * AUCHARCLASS_TYPE_MAX + lClassType].m_ucNumMastery;
//			++nCurRow;
//		}
//
//		if (nCurRow > nRow)
//			break;
//
////		++nCurRow;
//
//	}
//
//	return TRUE;
//}

BOOL AgpmSkill::ReadSkillTooltipTxt(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();
	INT16			nColumn			= csExcelTxtLib.GetColumn();

	INT16			nCurRow			= 1;
	INT16			nCurColumn		= 0;

	INT16			nReadSkill		= 0;

	for ( ; ; )
	{
		CHAR			*szSkillName	= csExcelTxtLib.GetData(0, nCurRow);
		if (!szSkillName || !strlen(szSkillName))
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdSkillTemplate	*pcsSkillTemplate	= GetSkillTemplate(szSkillName);
		if (!pcsSkillTemplate)
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		AgpdSkillTooltipTemplate	*pcsTooltip	= AddTooltipTemplate(szSkillName);
		if (!pcsTooltip)
		{
			++nCurRow;

			if (nCurRow > nRow)
				break;

			continue;
		}

		CHAR			*szBuffer		= NULL;

		nCurColumn	= 1;

		INT16			nIndex			= (-1);
		CHAR			*szColumnName	= csExcelTxtLib.GetData(nCurColumn, 0);

		BOOL			bIsFactorPoint	= TRUE;

		while (nCurColumn <= nColumn)
		{
			if(!szColumnName || strlen(szColumnName) == 0)
			{
				szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
				continue;
			}

			if (!strcmp(szColumnName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_TYPE_PERCENT))
				bIsFactorPoint	= FALSE;

			szBuffer	= csExcelTxtLib.GetData(nCurColumn, nCurRow);
			if (szBuffer && strlen(szBuffer))
			{
				// szBuffer의 column name을 보고 해당되는 부분에 데이타를 세팅한다
				SetTooptipType(pcsTooltip, szColumnName, szBuffer, bIsFactorPoint);
			}

			szColumnName = csExcelTxtLib.GetData(++nCurColumn, 0);
		}

		++nReadSkill;

		++nCurRow;

		if (nCurRow > nRow)
			break;
	}

	if (nReadSkill < 1)
		return FALSE;

	return TRUE;
}

BOOL AgpmSkill::ReadSkillConstCreatureTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel)
{
	if(!pcsSkillTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	// strtok 는 multi-thread 에서 문제가 있긴 하지만,
	// 이 함수는 처음 뜰 때 한번만 사용하게 되므로 문제가 발생하지 않는다.

	INT32 lDataIndex = 0;
	for(INT32 lIndex = lStartLevel; lIndex <= lEndLevel; lIndex++)
	{
		szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow + lDataIndex++);
		if(!szBuffer || !strlen(szBuffer))
			continue;

		INT32 lIndex2 = 0;

		// 문자열 형식은 xxx;xxx;xxx 이런식임
		szToken = strtok(szBuffer, szSeps);
		while(szToken)
		{
			pcsSkillTemplate->m_allCreatureTID[lIndex][lIndex2++] = atoi(szToken);
			szToken = strtok(NULL, szSeps);
		}
	}

	return TRUE;
}

BOOL AgpmSkill::ReadSkillConstAdditionalSkillTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel)
{
	if(!pcsSkillTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	// strtok 는 multi-thread 에서 문제가 있긴 하지만,
	// 이 함수는 처음 뜰 때 한번만 사용하게 되므로 문제가 발생하지 않는다.

	INT32 lDataIndex = 0;
	for(INT32 lIndex = lStartLevel; lIndex <= lEndLevel; lIndex++)
	{
		szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow + lDataIndex++);
		if(!szBuffer || !strlen(szBuffer))
			continue;
			
		if(strlen(szBuffer) > 3)
			printf("");

		INT32 lIndex2 = 0;

		// 문자열 형식은 xxx;xxx;xxx 이런식임
		szToken = strtok(szBuffer, szSeps);
		while(szToken)
		{
			pcsSkillTemplate->m_allAdditionalSkillTID[lIndex].push_back((INT32)atoi(szToken));
			szToken = strtok(NULL, szSeps);
		}
	}

	return TRUE;
}

BOOL AgpmSkill::ReadSkillConstLevelUpSkillTID(AgpdSkillTemplate* pcsSkillTemplate, AuExcelLib& csExcelTxtLib, INT32 lCurCol, INT32 lCurRow, INT32 lStartLevel, INT32 lEndLevel)
{
	if(!pcsSkillTemplate)
		return FALSE;

	CHAR* szBuffer = NULL;
	CHAR* szToken = NULL;
	CHAR szSeps[] = ";";

	// strtok 는 multi-thread 에서 문제가 있긴 하지만,
	// 이 함수는 처음 뜰 때 한번만 사용하게 되므로 문제가 발생하지 않는다.

	INT32 lDataIndex = 0;
	for(INT32 lIndex = lStartLevel; lIndex <= lEndLevel; lIndex++)
	{
		szBuffer = csExcelTxtLib.GetData(lCurCol, lCurRow + lDataIndex++);
		if(!szBuffer || !strlen(szBuffer))
			continue;

		INT32 lIndex2 = 0;

		// 문자열 형식은 xxx;xxx;xxx 이런식임
		szToken = strtok(szBuffer, szSeps);
		while(szToken)
		{
			pcsSkillTemplate->m_allLevelUpSkillTID[lIndex][lIndex2++] = atoi(szToken);
			szToken = strtok(NULL, szSeps);
		}
	}

	return TRUE;
}

namespace StringAppend
{
	static CHAR szData[255];
	const char* Append(const char* szLeft, const char* szRight)
	{
		_stprintf(szData, "%s%s", szLeft, szRight);
		return szData;
	}
};

INT16 AgpmSkill::GetConstFactorIndex(const CHAR *szName, INT32 lType)
{
	if (!szName || !strlen(szName))
		return (-1);

	CHAR szAppend[32] = {0,};
	if(lType != 0)
		_stprintf(szAppend, "%d", lType);

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_SKILL_TYPE, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_TYPE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_SKILL_TYPE2, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_TYPE2;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_BUFF_TYPE, szAppend)) == 0)
		return AGPMSKILL_CONST_BUFF_TYPE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_KNIGHT, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_KNIGHT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_ARCHER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_ARCHER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_WIZARD, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_WIZARD;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_BERSERKER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_BERSERKER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_HUNTER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_HUNTER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_SORCERER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_SORCERER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_SWASHBUCKLER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_SWASHBUCKLER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_RANGER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_RANGER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_ELEMENTALER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_ELEMENTALER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_SLAYER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_SLAYER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_OBITER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_OBITER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_SUMMONER, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_SUMMONER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_CLASS_SCION, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CLASS_SCION;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_LEVEL, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_LEVEL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REQUIRE_POINT, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_POINT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_LEVEL, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_LEVEL;

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_A, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_A;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_B, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_B;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_WATER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_THUNDER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_C, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_C;
	
	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_A_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_A_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_MAGIC_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_MAGIC_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_WATER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_WATER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_AIR_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_AIR_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_EARTH_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_EARTH_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_FIRE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_FIRE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_POISON_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_POISON_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_ICE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_ICE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_THUNDER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_THUNDER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_C_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_C_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_RESIST_A, szAppend)) == 0)
		return AGPMSKILL_CONST_RESIST_A;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COST_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_COST_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COST_MP, szAppend)) == 0)
		return AGPMSKILL_CONST_COST_MP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COST_SP, szAppend)) == 0)
		return AGPMSKILL_CONST_COST_SP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COST_ARROW, szAppend)) == 0)
		return AGPMSKILL_CONST_COST_ARROW;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_ENDSKILL_COST_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_ENDSKILL_COST_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CAST_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_CAST_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_RECAST_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_RECAST_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SEMI_RECAST_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_SEMI_RECAST_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DURATION, szAppend)) == 0)
		return AGPMSKILL_CONST_DURATION;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_INTERVAL, szAppend)) == 0)
		return AGPMSKILL_CONST_INTERVAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SHOW_DAMAGE, szAppend)) == 0)
		return AGPMSKILL_CONST_SHOW_DAMAGE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_RANGE, szAppend)) == 0)
		return AGPMSKILL_CONST_RANGE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_TARGET_AREA_R, szAppend)) == 0)
		return AGPMSKILL_CONST_TARGET_AREA_R;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_TARGET_AREA_F1, szAppend)) == 0)
		return AGPMSKILL_CONST_TARGET_AREA_F1;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_TARGET_AREA_F2, szAppend)) == 0)
		return AGPMSKILL_CONST_TARGET_AREA_F2;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DMG_A, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_A;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DMG_B, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_B;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_WATER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_THUNDER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_HEROIC, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_HEROIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DMG_A_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_A_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_MAGIC_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_MAGIC_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_WATER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_WATER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_AIR_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_AIR_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_EARTH_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_EARTH_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_FIRE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_FIRE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_POISON_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_POISON_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_ICE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_ICE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_THUNDER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_THUNDER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_HEROIC_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DMG_HEROIC_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_MOVE_DISTANCE, szAppend)) == 0)
		return AGPMSKILL_CONST_MOVE_DISTANCE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REFLECT_MAX, szAppend)) == 0)
		return AGPMSKILL_CONST_REFLECT_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_HEROIC_REFLECT_MAX, szAppend)) == 0)
		return AGPMSKILL_CONST_HEROIC_REFLECT_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DEFENSE_MAX, szAppend)) == 0)
		return AGPMSKILL_CONST_DEFENSE_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REFLECT_CON, szAppend)) == 0)
		return AGPMSKILL_CONST_REFLECT_CON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DEFENSE_CON, szAppend)) == 0)
		return AGPMSKILL_CONST_DEFENSE_CON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_SHIELD, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_SHIELD;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COUNTER_UP, szAppend)) == 0)
		return AGPMSKILL_CONST_COUNTER_UP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COUNTER_REDUCE, szAppend)) == 0)
		return AGPMSKILL_CONST_COUNTER_REDUCE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_HOT, szAppend)) == 0)
		return AGPMSKILL_CONST_HOT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DMG_ADJUST, szAppend)) == 0)
		return AGPMSKILL_CONST_DMG_ADJUST;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DMG_SHOW_DIVIDE, szAppend)) == 0)
		return AGPMSKILL_CONST_DMG_SHOW_DIVIDE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_HP_RECOVERY, szAppend)) == 0)
		return AGPMSKILL_CONST_HP_RECOVERY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_MP_RECOVERY, szAppend)) == 0)
		return AGPMSKILL_CONST_MP_RECOVERY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SP_RECOVERY, szAppend)) == 0)
		return AGPMSKILL_CONST_SP_RECOVERY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_WARRIOR_LEVEL, szAppend)) == 0)
		return AGPMSKILL_CONST_WARRIOR_LEVEL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CHANGE_MONSTER_TID, szAppend)) == 0)
		return AGPMSKILL_CONST_CHANGE_MONSTER;

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_CON, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_CON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_STR, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_STR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_INT, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_INT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEX, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_CHA, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_CHA;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_WIS, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_WIS;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_MOVEMENT, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_MOVEMENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_MP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_MP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_SP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_SP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_AP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_AP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_AGRO, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_AGRO;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_MAX_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_HP_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_MAX_MP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_MP_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_MAX_SP, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_SP_MAX;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_ATK_SPEED, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_ATKSPEED;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_ATK_RANGE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_ATKRANGE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_SKILL_CAST, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_SKILL_CAST;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_SKILL_DELAY, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_SKILL_DELAY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_HIT_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_HIT_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_EVADE_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_EVADE_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DODGE_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DODGE_RATE;

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_PHYSICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_PHYSICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_WATER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_LIGHTENING;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MIN_HEROIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MIN_HEROIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_PHYSICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_PHYSICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_WATER;
	
	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_LIGHTENING;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DMG_MAX_HEROIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DMG_MAX_HEROIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_PHYSICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_PHYSICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_WATER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_LIGHTENING;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_POINT_HEROIC, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_POINT_HEROIC;

	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_PHYSICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_PHYSICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_PHYSICAL_BLOCK, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_PHYSICAL_BLOCK;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_SKILL_BLOCK, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_SKILL_BLOCK;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_HEROIC_MELEE_RESISTANCE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_MELEE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_HEROIC_RANGED_RESISTANCE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_RANGED;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DEF_RATE_HEROIC_MAGIC_RESISTANCE, szAppend)) == 0)
		return AGPMSKILL_CONST_POINT_DEF_RATE_HEROIC_MAGIC;

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BLOCK_POINT, szAppend)) == 0)
		return AGPMSKILL_CONST_BLOCK_POINT;
	//else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_BLOCK) === 0)
	//	return AGPMSKILL_CONST_SKILL_BLOCK;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_STUN_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_STUN_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_STUN_GENERATE_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_STUN_GENERATE_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CRITICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_CRITICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_ATK_RANGE, szAppend)) == 0)
		return AGPMSKILL_CONST_ATK_RANGE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_RANGE, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_RANGE;
	//else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EVADE_RATE) === 0)
	//	return AGPMSKILL_CONST_EVADE_RATE;
	//else if(_stricmp(szName, StringAppend::Append(APMGSKILL_EXCEL_COLUMN_DODGE_RATE) === 0)
	//	return AGPMSKILL_CONST_DODGE_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_CONVERT_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_CONVERT_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DAMAGE_CONVERT_MP, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_CONVERT_MP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REGEN_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_REGEN_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_REGEN_MP, szAppend)) == 0)
		return AGPMSKILL_CONST_REGEN_MP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CHARGE_1, szAppend)) == 0)
		return AGPMSKILL_CONST_CHARGE_1;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CHARGE_2, szAppend)) == 0)
		return AGPMSKILL_CONST_CHARGE_2;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CHARGE_3, szAppend)) == 0)
		return AGPMSKILL_CONST_CHARGE_3;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_AGGRODOLL_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_AGGRODOLL_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DOT_DAMAGE_TIME, szAppend)) == 0)
		return AGPMSKILL_CONST_DOT_DAMAGE_TIME;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_COST_MP_DECREASE, szAppend)) == 0)
		return AGPMSKILL_CONST_COST_MP_DECREASE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CAST_TIME_DECREASE, szAppend)) == 0)
		return AGPMSKILL_CONST_CAST_TIME_DECREASE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_LEVELUP, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_LEVELUP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_MP_CONVERT_HP, szAppend)) == 0)
		return AGPMSKILL_CONST_MP_CONVERT_HP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DEATH, szAppend)) == 0)
		return AGPMSKILL_CONST_DEATH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_CONNECTION_TID, szAppend)) == 0)
		return AGPMSKILL_CONST_CONNECTION_TID;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_UNION, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_UNION;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_UNION_CONTROL, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_UNION_CONTROL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_MAX_SKILL_UNION, szAppend)) == 0)
		return AGPMSKILL_CONST_MAX_SKILL_UNION;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_COST, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_COST;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_UPGRADE_COST, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_UPGRADE_COST;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_CLASSIFY_ID, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_CLASSIFY_ID;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SKILL_CLASSIFY_LEVEL, szAppend)) == 0)
		return AGPMSKILL_CONST_SKILL_CLASSIFY_LEVEL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_VELOCITY, szAppend)) == 0)
		return AGPMSKILL_CONST_VELOCITY;

	// 2005.07.20. steeple
	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SUMMONS_TID, szAppend)) == 0)
		return AGPMSKILL_CONST_SUMMONS_TID;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_SUMMONS_COUNT, szAppend)) == 0)
		return AGPMSKILL_CONST_SUMMONS_COUNT;
	/*else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_ADDITIONAL_SKILLTID, szAppend)) == 0)
		return AGPMSKILL_CONST_ADDITIONAL_SKILLTID;*/
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_ADDITIONAL_DURATION, szAppend)) == 0)
		return AGPMSKILL_CONST_ADDITIONAL_DURATION;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_MAX_TARGET_NUM, szAppend)) == 0)
		return AGPMSKILL_CONST_MAX_TARGET_NUM;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_LIMIT_QUANTITY, szAppend)) == 0)
		return AGPMSKILL_CONST_LIMIT_QUANTITY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BONUS_EXP, szAppend)) == 0)
		return AGPMSKILL_CONST_BONUS_EXP;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BONUS_MONEY, szAppend)) == 0)
		return AGPMSKILL_CONST_BONUS_MONEY;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BONUS_DROP_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_BONUS_DROP_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BONUS_DROP_RATE2, szAppend)) == 0)
		return AGPMSKILL_CONST_BONUS_DROP_RATE2;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_BONUS_CHARISMA_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_BONUS_CHARISMA_RATE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_LEVEL_DIFF, szAppend)) == 0)
		return AGPMSKILL_CONST_LEVEL_DIFF;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_DIVIDE_RATE, szAppend)) == 0)
		return AGPMSKILL_CONST_DIVIDE_RATE;

	if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_PHYSICAL, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_PHYSICAL;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_MAGIC, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_MAGIC;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_WATER, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_WATER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_AIR, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_AIR;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_EARTH, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_EARTH;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_FIRE, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_FIRE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_POISON, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_POISON;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_ICE, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_ICE;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_THUNDER, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_THUNDER;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_C, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_C;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_PHYSICAL_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_PHYSICAL_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_MAGIC_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_MAGIC_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_WATER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_WATER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_AIR_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_AIR_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_EARTH_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_EARTH_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_FIRE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_FIRE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_POISON_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_POISON_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_ICE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_ICE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_THUNDER_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_THUNDER_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_EA_DMG_C_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_EA_DMG_C_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_IGNORE_PHYSICAL_DEFENCE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_IGNORE_ATTRIBUTE_DEFENCE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_CRITICAL_DEFENCE_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DURABILITY_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_ARMOR_DURABILITY_PERCENT;
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DURABILITY_NUM, szAppend)) == 0)
		return AGPMSKILL_CONST_ARMOR_DURABILITY_NUM;	

	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_REQUIRE_HEROIC_POINT, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_HEROIC_POINT;	
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_REQUIRE_CHARISMA_POINT, szAppend)) == 0)
		return AGPMSKILL_CONST_REQUIRE_CHARISMA_POINT;	
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DAMAGE_HEROIC, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_HEROIC;	
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_DAMAGE_HEROIC_PERCENT, szAppend)) == 0)
		return AGPMSKILL_CONST_DAMAGE_HEROIC_PERCENT;	

	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_TIME_CONTROL, szAppend)) == 0)
		return AGPMSKILL_CONST_TIME_CONTROL;	
	else if(_stricmp(szName, StringAppend::Append(AGPMSKILL_EXCEL_COLUMN_FACTOR_ITEM_PARTS, szAppend)) == 0)
		return AGPMSKILL_CONST_ITEM_PARTS;	

	return (-1);
}

// SpecialType만 따로 함수로 구성
BOOL AgpmSkill::SetSpecialStatus(AgpdSkillTemplate *pcsSkillTemplate, INT32 lValue)
{
	if(NULL == pcsSkillTemplate)
		return FALSE;

	switch(lValue)
	{
	case 1:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_STUN;
		break;

	case 2:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_FREEZE;
		break;

	case 3:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_SLOW;
		break;

	case 4:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_INVINCIBLE;
		break;

	case 5:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE;
		break;

	case 6:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_NOT_ADD_AGRO;
		break;

	case 7:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_HIDE_AGRO;
		break;

	case 8:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT;
		break;

	case 9:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_HALT;
		break;

	case 10:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_TRANSPARENT;
		break;

	case 11:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_HALF_TRANSPARENT;
		break;

	case 12:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE;
		break;

	case 13:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE;
		break;

	case 14:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_DISABLE_SKILL;
		break;

	case 15:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_DISABLE_NORMAL_ATK;
		break;

	case 16:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_SLEEP;
		break;

	case 17:	// Stun Protect, Slow Invincible
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT;
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_SLOW_INVINCIBLE;
		break;

	case 18:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_DISARMAMENT;
		break;

	case 19:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_DISABLE_CHATTING;
		break;

	case 20:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_HOLD;
		break;

	case 21:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_CONFUSION;
		break;

	case 22:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_FEAR;
		break;

	case 23:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_DISEASE;
		break;

	case 24:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_BERSERK;
		break;

	case 25:
		pcsSkillTemplate->m_lSpecialStatus |= AGPMSKILL_SPECIAL_STATUS_SHRINK;
		break;
	}

	return TRUE;
}

// 2005.06.29. steeple
// 참고로 nIndex 로 넘어오는 값은 무조건 0 이다.
// MAX_CODITION 이 1이기 때문에 무조건 0. 앞으로도 바꾸지 말자 -_-
BOOL AgpmSkill::SetSkillType(AgpdSkillTemplate *pcsSkillTemplate, CHAR *szTypeName, CHAR *szValue, INT16 nIndex, CHAR *szArg)
{
	if (!pcsSkillTemplate || !szTypeName || !szValue || !strlen(szTypeName) || !strlen(szValue))
		return FALSE;

	if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_TARGET, strlen(AGPMSKILL_EXCEL_COLUMN_TARGET)))
	{
		INT32	lTargetType	= atoi(szValue);

		if (lTargetType >= 100)
		{
			pcsSkillTemplate->m_lTargetType |= AGPMSKILL_TARGET_SELF_ONLY;
			lTargetType -= 100;
		}
		if (lTargetType >= 10)
		{
			pcsSkillTemplate->m_lTargetType |= AGPMSKILL_TARGET_FRIENDLY_UNITS;
			lTargetType -= 10;
		}
		if (lTargetType >= 1)
		{
			pcsSkillTemplate->m_lTargetType |= AGPMSKILL_TARGET_ENEMY_UNITS;
		}
	}

	if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CHECK_RANGE, strlen(AGPMSKILL_EXCEL_COLUMN_CHECK_RANGE)))
	{
		switch (atoi(szValue)) {
		case 1:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_CHECK_RANGE_NORMAL_RANGE;
			break;

		case 2:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_CHECK_RANGE_NOT_CHECK;
			break;
		
		case 3:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_CHECK_RANGE_SERVER_CONTROL;
			break;

		default:
			return FALSE;
			break;
		}
	}

	if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ATTRIBUTE, strlen(AGPMSKILL_EXCEL_COLUMN_ATTRIBUTE)))
	{
		INT32	lAttribute = atoi(szValue);

		if(lAttribute >= 40000)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_SAVE_DURATION;
			lAttribute -= 40000;
		}

		if(lAttribute >= 20000)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_SAVE_EXPIREDATE;
			lAttribute -= 20000;
		}

		if (lAttribute >= 10000)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_NOT_SEND_CAST;
			lAttribute -= 10000;
		}

		if (lAttribute >= 1000)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_TYPE_MAGIC;
			lAttribute -= 1000;
		}

		// 소환수 이면서 버프인 놈과 아닌놈을 구별하기 위해서 숫자가 커짐. 2005.10.05. steeple
		//
		// 소환수 속성 2005.07.17. steeple
		if(lAttribute >= 200)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_SUMMONS;
			lAttribute -= 200;
		}

		if (lAttribute >= 100)
		{
			//pcsSkillTemplate->m_nAttribute = AGPMSKILL_SKILL_ATTR_TYPE_MAGIC;
			lAttribute -= 100;
		}

		if (lAttribute >= 40)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_ETC;
			lAttribute -= 40;
		}

		if (lAttribute >= 20)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_DEBUFF;
			lAttribute -= 20;
		}
		else if (lAttribute >= 10)
		{
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_BUFF;
			lAttribute -= 10;
		}

		switch (lAttribute) {
		case 1:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_PHYSICAL_DMG;
			break;

		case 2:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_MAGIC_DMG_MAGIC;
			break;

		case 3:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_MAGIC_DMG_FIRE;
			break;

		case 4:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_MAGIC_DMG_EARTH;
			break;

		case 5:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_MAGIC_DMG_AIR;
			break;

		case 6:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_MAGIC_DMG_WATER;
			break;

		case 8:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_PRODUCT;
			break;

		case 9:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_PASSIVE;
			break;

		default:
			return FALSE;
			break;
		}
	}

	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_UPGRADABLE, strlen(AGPMSKILL_EXCEL_COLUMN_UPGRADABLE)))
	{
		switch (atoi(szValue)) {
		case 1:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_CAN_UPGRADE;
			break;

		case 2:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_CANNOT_UPGRADE;
			break;
			
		// 2005.10.20. steeple
		// 업그레이드 한도치가 있는 스킬.
		case 3:
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_CAN_UPGRADE;
			pcsSkillTemplate->m_nAttribute |= AGPMSKILL_SKILL_ATTR_LIMITED_MAX_LEVEL;
			break;
		}
	}
	
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_REQUIREMENT, strlen(AGPMSKILL_EXCEL_COLUMN_REQUIREMENT)))
	{
		switch (atoi(szValue)) {
		case 1:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			break;

		case 2:
			// 2004.12.14. steeple 변경
			// 활도 그냥 무기로 치고, 아이템 검사에서 검사함.
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			//pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_ARROW;
			break;

		case 3:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_SHIELD;
			break;

		case 4:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_PRODUCT_TOOL;
			break;

		// 2004.12.10. steeple. 추가
		case 5:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_ONE_HAND_SWORD;
			break;

		case 6:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_ONE_HAND_AXE;
			break;

		case 7:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_BLUNT;
			break;

		case 8:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_TWO_HAND_SLASH;
			break;

		case 9:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_STAFF;
			break;

		case 10:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_WAND;
			break;

		case 11:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_BOW;
			break;

		case 12:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_CROSS_BOW;
			break;

		case 13:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_KATARIYA;
			break;

		case 14:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_CHAKRAM;
			break;

		case 15:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_LEFT_HAND_SWORD;
			break;

		case 16:
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_RIGHT_WEAPON_ONLY;
			break;
		
		case 17:
		{
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_CHARON;
		} break;

		case 18:
		{
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_ZENON;
		} break;
		
		case 19:
		{
			pcsSkillTemplate->m_lRequirementType = AGPMSKILL_REQUIREMENT_EQUIP_WEAPON;
			pcsSkillTemplate->m_lRequirementType |= AGPMSKILL_REQUIREMENT_EQUIP_CHARON_AND_ZENON;
		} break;

		default:
			return FALSE;
			break;
		}
	}

	// DIRT
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DIRT, strlen(AGPMSKILL_EXCEL_COLUMN_DIRT)))
	{
		INT32	lDIRT = atoi(szValue);

		if (lDIRT >= 1000)
		{
			pcsSkillTemplate->m_stUseDIRTPoint.nDuration = 1;
			lDIRT -= 1000;
		}

		if (lDIRT >= 100)
		{
			pcsSkillTemplate->m_stUseDIRTPoint.nIntensity = 1;
			lDIRT -= 100;
		}

		if (lDIRT >= 10)
		{
			pcsSkillTemplate->m_stUseDIRTPoint.nRange = 1;
			lDIRT -= 10;
		}

		if (lDIRT == 1)
			pcsSkillTemplate->m_stUseDIRTPoint.nTarget = 1;
	}

	// range
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_RANGE, strlen(AGPMSKILL_EXCEL_COLUMN_RANGE)))
	{
		INT32	lValue	= atoi(szValue);
		if (lValue > 1000)
		{
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_BASE_TARGET;
			lValue -= 1000;
		}
		else
		{
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_BASE_SELF;
		}

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SELF_ONLY;
			break;

		case 2:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			break;

		case 201:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY_INVOLVE_SELF;
			break;

		case 202:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY_ENEMY_UNITS;
			break;

		case 203:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY_FRIENDLY_UNITS;
			break;

		case 204:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_DEAD;
			break;

			// target (target + target race)
		case 205:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_RACE;
			break;

			// target (target + target party)
		case 206:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_PARTY;
			break;

			// target (target + target race + region(involve parent region))
		case 207:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_RACE;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION_INVOLVE_PARENT;
			break;

			// target (target + target race + region(involve parent region))
		case 208:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_PARTY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION_INVOLVE_PARENT;
			break;	

			// target (target + target race + region)
		case 209:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_RACE;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION;
			break;
			
			// target (target + target party + region)
		case 210:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_PARTY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION;
			break;	

			// target (target + target region)
		case 211:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_TARGET_ONLY;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION_INVOLVE_PARENT;
			break;

		case 3:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_USE_CONSTANT;
			break;

		case 301:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_USE_ATK_RANGE;
			break;

		case 302:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_INVOLVE_SELF;
			break;

		case 303:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SELF_ONLY;	// 2005.01.10. steeple - 파티일때 자기자신도 추가
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_PARTY_MEMBER;
			break;

		case 304:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_ENEMY_UNITS;
			break;

		case 305:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_FRIENDLY_UNITS;
			break;

		case 306:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_UNDEAD_UNITS;
			break;

		case 307:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_SUMMONS;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_INVOLVE_SELF;
			break;

		case 308:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_SUMMONS;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_INVOLVE_SELF;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SPHERE_ONLY_PARTY_MEMBER;
			break;

		// 2007.09.06. steeple
		case 309:
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SPHERE;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_ONLY_GUILD_MEMBERS;
			break;

		case 4:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_BOX;
			break;

		case 404:
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_BOX;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_BOX_ONLY_ENEMY_UNITS;
			break;

		case 5:		// only own summons or owner's summons
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SUMMONS_ONLY;
			break;

		case 501:	// only own summons or owner's summons + self
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SUMMONS_ONLY;
			pcsSkillTemplate->m_lRangeType |= AGPMSKILL_RANGE_SELF_ONLY;
			break;

		// Siege War 부터는 RangeType2 를 사용한다.
		case 600:	// Siege War
			break;

		case 601:	// Siege War to all attackers
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SIEGE_WAR_TO_ALL_ATTACKERS;
			break;

		case 602:	// Siege War to all defenders
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SIEGE_WAR_TO_ALL_DEFENDERS;
			break;

		case 611:	// Siege War Area Sphere to all attackers
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SIEGE_WAR_TO_ALL_ATTACKERS;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SPHERE;
			break;

		case 612:	// Siege War Area Sphere to all defenders
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SIEGE_WAR_TO_ALL_DEFENDERS;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SPHERE;
			break;

		case 7:		// Ground
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_GROUND;
			break;

		case 701:	// Ground area sphere to enemy units
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_GROUND;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_ONLY_ENEMY_UNITS;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SPHERE;
			break;

		case 702:	// Ground area sphere to guild members
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_GROUND;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_ONLY_GUILD_MEMBERS;
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_SPHERE;
			break;

		case 8:	// Region 
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION;
			break;

		case 801:	// Region involve Parent
			pcsSkillTemplate->m_lRangeType2 |= AGPMSKILL_RANGE2_TARGET_REGION_INVOLVE_PARENT;
			break;

		default:
			return FALSE;
			break;
		}
	}

	// cost
	//
	//
	////////////////////////////////////////////////////////////////////

	// condition
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CONDITION, strlen(AGPMSKILL_EXCEL_COLUMN_CONDITION)))
	{
		INT32	lValue	= atoi(szValue);

		pcsSkillTemplate->m_lConditionType[nIndex] = 0;
		
		if (lValue >= 4000)
		{
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_SERVER_CONTROL;
			lValue -= 4000;
		}

		if (lValue >= 2000)
		{
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_SKIP_TARGET_NOT_IN_RANGE;
			lValue -= 2000;
		}

		if (lValue >= 1000)
		{
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_SKIP_CHECK_FIRST;
			lValue -= 1000;
		}

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK;
			break;

		case 101:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK_EQUIP_WEAPON;
			break;

		case 102:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK_EQUIP_ARROW;
			break;

		case 103:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK_EQUIP_SHIELD;
			break;

		case 104:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MELEE_ATTACK_USE_FORMULA1;
			break;

		case 2:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MAGIC_ATTACK;
			break;

		case 201:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MAGIC_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MAGIC_ATTACK_USE_FORMULA1;
			break;

		case 202:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MAGIC_ATTACK;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_MAGIC_ATTACK_USE_FORMULA2;
			break;

		case 3:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_DEBUFF;
			break;

		case 4:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_FRIENDLY_UNIT;
			break;

		case 5:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_LEVEL;
			break;

		case 6:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_KILL_GUARDIAN;
			break;

		case 7:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_CHAR_TYPE;
			break;

		case 8:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_OWN_ITEM;
			break;

		case 9:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_PARTY_MEMBER;
			break;

		case 901:
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_PARTY_MEMBER;
			pcsSkillTemplate->m_lConditionType[nIndex] |= AGPMSKILL_CONDITION_SHOUT_WORD;
			break;

		default:
			return FALSE;
			break;
		}
	}

	// condition2 - 2008.04.07. steeple
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CONDITION2, strlen(AGPMSKILL_EXCEL_COLUMN_CONDITION2)))
	{
		INT32 lValue = atoi(szValue);
		pcsSkillTemplate->m_ulCondition2 = lValue;

		m_vcCondition2SkillTID.push_back(pcsSkillTemplate->m_lID);
	}

	// condition argument
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ARG1, strlen(AGPMSKILL_EXCEL_COLUMN_ARG1)))
		pcsSkillTemplate->m_stConditionArg[nIndex].lArg1 = atoi(szValue);
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ARG2, strlen(AGPMSKILL_EXCEL_COLUMN_ARG2)))
		pcsSkillTemplate->m_stConditionArg[nIndex].lArg2 = atoi(szValue);
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ARG3, strlen(AGPMSKILL_EXCEL_COLUMN_ARG3)))
		pcsSkillTemplate->m_stConditionArg[nIndex].lArg3 = atoi(szValue);
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ARG4, strlen(AGPMSKILL_EXCEL_COLUMN_ARG4)))
		pcsSkillTemplate->m_stConditionArg[nIndex].lArg4 = atoi(szValue);
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ARG_STRING, strlen(AGPMSKILL_EXCEL_COLUMN_ARG_STRING)))
		strncpy(pcsSkillTemplate->m_stConditionArg[nIndex].szArgString, szValue, strlen(AGPMSKILL_EXCEL_COLUMN_ARG_STRING));

	// effect

	// check interval process skill effect.
	INT32	lValue	= (INT32) atoi(szValue);
	BOOL	bProcessInterval	= FALSE;

	if (lValue / AGPMSKILL_EFFECT_PROCESS_INTERVAL_VALUE)
	{
		bProcessInterval = TRUE;
		lValue = lValue % AGPMSKILL_EFFECT_PROCESS_INTERVAL_VALUE;
	}

	// melee attack
	if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_MELEE_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_MELEE_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG;

		switch (lValue) {
		case 101:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
			break;

		case 102:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2;
			break;

		case 103:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3;
			break;

//		case 104:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4;
//			if (bProcessInterval)
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4;
//			break;
//
//		case 105:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5;
//			if (bProcessInterval)
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5;
//			break;

		case 106:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			break;

		case 107:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH;
			break;

		case 108:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6;
			break;

		case 109:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			if (bProcessInterval)
			{
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			}
			break;

		case 110:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3;
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			if (bProcessInterval)
			{
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3;
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;
			}
			break;

		case 111:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY;
			if (bProcessInterval)
			{
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1;
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY;
			}
			break;

		case 112:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC;
			if (bProcessInterval)
			{
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC;
			}
			break;
			
		default:
			return FALSE;
			break;
		}
	}

	// magic attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_MAGIC_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_MAGIC_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG;

		BOOL	bDMG_Adjust = FALSE;
		if (lValue / AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST_FLAG)
		{
			bDMG_Adjust = TRUE;
			lValue = lValue % AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST_FLAG;
		}

		switch (lValue) {
//		case 102:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC;
//			if (bDMG_Adjust)
//				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//
//			if (bProcessInterval)
//			{
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC;
//				if (bDMG_Adjust)
//					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//			}
//			break;
//
//		case 103:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE;
//			if (bDMG_Adjust)
//				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//
//			if (bProcessInterval)
//			{
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE;
//				if (bDMG_Adjust)
//					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//			}
//			break;
//
//		case 104:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH;
//			if (bDMG_Adjust)
//				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//
//			if (bProcessInterval)
//			{
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH;
//				if (bDMG_Adjust)
//					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//			}
//			break;
//
//		case 105:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR;
//			if (bDMG_Adjust)
//				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//
//			if (bProcessInterval)
//			{
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR;
//				if (bDMG_Adjust)
//					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//			}
//			break;
//
//		case 106:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER;
//			if (bDMG_Adjust)
//				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//
//			if (bProcessInterval)
//			{
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER;
//				if (bDMG_Adjust)
//					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_ADJUST;
//			}
//			break;

		case 109:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;
			if (bDMG_Adjust)
				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;

			if (bProcessInterval)
			{
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;
				if (bDMG_Adjust)
					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;
			}
			break;

		default:
			return FALSE;
			break;
		}
	}

	// count
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_COUNT, strlen(AGPMSKILL_EXCEL_COLUMN_COUNT)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_COUNT;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_COUNT;
	}

	// 2004.08.25. steeple 변경
	// Special Status 가 생겼고, 그 안에 stun, freeze, slow, 무적, 속성 무적 등이 있다.
	// 2004.12.10. steeple 변경
	// SkillTemplate 에 m_lSpecialStatus 추가. 그곳에다가 저장하는 식으로 변경
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_SPECIAL_STATUS, strlen(AGPMSKILL_EXCEL_COLUMN_SPECIAL_STATUS)))
	{
		if(lValue >= 5000)
		{
			if(lValue == 5001)
			{
				pcsSkillTemplate->m_bSpecialStatusMatchingAll = TRUE;
			}
			else
			{
				pcsSkillTemplate->m_bSpecialStatusMatchingAll = FALSE;
			}

			
			if(szArg && strlen(szArg))
			{
				CHAR *szToken = NULL;
				CHAR szSeps[] = ";";
				szToken = strtok(szArg, szSeps);
				while(szToken)
				{
					INT32 lValue = (INT32)atoi(szToken);
					switch(lValue)
					{
					case 1:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_STUN;
						break;

					case 2:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_FREEZE;
						break;

					case 3:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_SLOW;
						break;

					case 4:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_INVINCIBLE;
						break;

					case 5:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE;
						break;

					case 6:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_NOT_ADD_AGRO;
						break;

					case 7:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_HIDE_AGRO;
						break;

					case 8:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT;
						break;

					case 9:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_HALT;
						break;

					case 10:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_TRANSPARENT;
						break;

					case 11:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_HALF_TRANSPARENT;
						break;

					case 12:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE;
						break;

					case 13:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE;
						break;

					case 14:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_DISABLE_SKILL;
						break;

					case 15:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_DISABLE_NORMAL_ATK;
						break;

					case 16:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_SLEEP;
						break;

					case 17:	// Stun Protect, Slow Invincible
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT;
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_SLOW_INVINCIBLE;
						break;

					case 18:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_DISARMAMENT;
						break;

					case 19:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_DISABLE_CHATTING;
						break;

					case 20:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_HOLD;
						break;

					case 21:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_CONFUSION;
						break;

					case 22:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_FEAR;
						break;

					case 23:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_DISEASE;
						break;

					case 24:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_BERSERK;
						break;

					case 25:
						pcsSkillTemplate->m_lSpecialStatusMatching |= AGPMSKILL_SPECIAL_STATUS_SHRINK;
						break;
					}

					szToken = strtok(NULL, szSeps);
				}

			}
		}
		else
		{
			// 2007.11.15. steeple
			if(lValue >= 1000)
			{
				pcsSkillTemplate->m_lSpecialStatusAdjustType = 1;
				lValue -= 1000;
			}
		
			// lvalue가 0일때에는 다중 상태이상을 셋팅할 수 있다.
			// 단, 0일때는 ProcessSkillEffectSpecialStatus 함수로는 이용하지 않는다.
			if(lValue == 0)
			{
				CHAR *szToken = NULL;
				CHAR szSeps[] = ";";
				szToken = strtok(szArg, szSeps);
				while(szToken)
				{
					INT32 lValue = (INT32)atoi(szToken);
	
					SetSpecialStatus(pcsSkillTemplate, lValue);
					
					szToken = strtok(NULL, szSeps);
				}
			}
			else
			{
				pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SPECIAL_STATUS;
				if (bProcessInterval)
					pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SPECIAL_STATUS;

				SetSpecialStatus(pcsSkillTemplate, lValue);
			}			
		}
	}

	// update factor
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_UPDATE_FACTOR, strlen(AGPMSKILL_EXCEL_COLUMN_UPDATE_FACTOR)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR;

		BOOL	bUpdateForAWhile = FALSE;
		if (lValue > 110)
		{
			lValue = lValue - 10;
			bUpdateForAWhile = TRUE;
		}
		else if (lValue < 100)
			bUpdateForAWhile = TRUE;

		if (bUpdateForAWhile)
		{
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE;
		}

		switch (lValue) {
//		case 101:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY;
//			if (bProcessInterval)
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY;
//			break;
//
//		case 102:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_STATUS;
//			if (bProcessInterval)
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_STATUS;
//			break;
//
//		case 103:
//			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_MAGIC_DEFENSE;
//			if (bProcessInterval)
//				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_MAGIC_DEFENSE;
//			break;

		case 105:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT;
			break;

		case 106:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY;
			break;
			
		case 107:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM;
			break;

		case 108:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME;
			break;

		case 10:
			break;

		default:
			return FALSE;
			break;
		}
	}

	// reflect melee attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_REFLECT_MELEE_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_REFLECT_MELEE_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK;
		
		// 2004.09.21. steeple 변경
		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
			break;

		case 2:	// 받은 데미지를 무시한다. 2004.09.21. steeple
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD;
			break;
		}

		// reflect_melee_attack expand : bit mask 2008.06.10. iluvs 
		if(lValue & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;
			
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;

			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;
		}

	}

	// reflect magic attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_REFLECT_MAGIC_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_REFLECT_MAGIC_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK;

		switch (lValue) {
		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
			break;
		}
	}

	// reflect heroic attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_REFLECT_HEROIC_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_REFLECT_HEROIC_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK;
	}
	

	// defense melee attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DEFENSE_MELEE_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_DEFENSE_MELEE_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

		switch (lValue) {
		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK;
			break;

		case 3:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_EVADE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_EVADE;
			break;

		case 4:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT;
			break;
		}
	}

	// defense magic attack
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DEFENSE_MAGIC_ATTACK, strlen(AGPMSKILL_EXCEL_COLUMN_DEFENSE_MAGIC_ATTACK)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK;
	}

	// magic resist level up
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_MAGIC_RESIST_LEVEL_UP, strlen(AGPMSKILL_EXCEL_COLUMN_MAGIC_RESIST_LEVEL_UP)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP;
	}

	// despell magic
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DISPEL_MAGIC, strlen(AGPMSKILL_EXCEL_COLUMN_DISPEL_MAGIC)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DISPEL_MAGIC;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DISPEL_MAGIC;

		switch(lValue)
		{
		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DISPEL_MAGIC_CURE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DISPEL_MAGIC_CURE;
			break;
		}

		// 2007.06.05. steeple
		// dispel 이 확장되었다.
		if(lValue & AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] |= AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS;
		}
	}

	// transform target
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_TRANSFORM_TARGET, strlen(AGPMSKILL_EXCEL_COLUMN_TRANSFORM_TARGET)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET;

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1;
			break;

		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2;
			break;

		case 3:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3;
			break;

		case 4:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4;
			break;

		case 5:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5;
			break;
		}
	}
	
	// life protection
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_LIFE_PROTECTION, strlen(AGPMSKILL_EXCEL_COLUMN_LIFE_PROTECTION)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_LIFE_PROTECTION;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_LIFE_PROTECTION;
	}

	// DOT
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DOT, strlen(AGPMSKILL_EXCEL_COLUMN_DOT)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE;

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL;
			break;

		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE;
			break;
		
		case 3:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC;
			break;
		}
	}

	// infect around
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_INFECT_AROUNT, strlen(AGPMSKILL_EXCEL_COLUMN_INFECT_AROUNT)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_INFECT_AROUND;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_INFECT_AROUND;
	}

	// add spell count
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ADD_SPELLCOUNT, strlen(AGPMSKILL_EXCEL_COLUMN_ADD_SPELLCOUNT)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_ADD_SPELLCOUNT;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_ADD_SPELLCOUNT;

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_ADD_SPELLCOUNT_MASTERY_POINT;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_ADD_SPELLCOUNT_MASTERY_POINT;
			break;
		}
	}

	// damage adjust
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DMG_ADJUST, strlen(AGPMSKILL_EXCEL_COLUMN_DMG_ADJUST)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_DMG_ADJUST;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_DMG_ADJUST;
	}

	// skill factor adjust
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_SKILL_FACTOR_ADJUST, strlen(AGPMSKILL_EXCEL_COLUMN_SKILL_FACTOR_ADJUST)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST;

		switch (lValue) {
		case 1:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP;
			break;

		case 2:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP;
			break;

		case 3:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP;
			break;

		case 4:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE;
			break;

		case 5:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME;
			break;

		case 6:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME;
			if (bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME;
			break;

		case 7:
			pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION;
			if(bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION;
			break;

		}
	}
	

	//////////////////////////////////////////////////////////////////////////
	// 여기서 부터는 m_lEffectType2, m_lProcessIntervalEffectType2 에 저장된다.

	// 2007.11.18. steeple
	// move target 에서 move position 으로 변경하였다. 그러면서 EffectType2 로 변경.
	// move position
	else if (!strncmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_MOVE_POS, strlen(AGPMSKILL_EXCEL_COLUMN_MOVE_POS)))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_MOVE_POS;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_MOVE_POS;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] |= AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME;
		}
	}

	// update combat point
	else if (!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_UPDATE_COMBAT_POINT))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;

		switch(lValue)
		{
			case 1:
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE1;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE1;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1;
				break;

			case 2:
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE2;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE2;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2;
				break;

			case 3:
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE3;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT_TYPE3;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3;
				break;
		}
	}

	// regen relation. 2005.10.03. steeple 살짝 변경
	else if (!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_REGEN_HP))
	{
		switch(lValue)
		{
			case 1:		// regen hp
				pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_HP;
				if(bProcessInterval)
					pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_HP;
				break;

			case 2:		// regen mp
				pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_MP;
				if(bProcessInterval)
					pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_MP;
				break;

			case 3:		// regen hp, mp
				pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_HP;
				pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_MP;
				if(bProcessInterval)
				{
					pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_HP;
					pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_REGEN_MP;
				}
				break;

		}
	}

	// convert type
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CONVERT_TYPE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT;

		switch(lValue)
		{
			case 1:	// 데미지를 HP 로 변환
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_DAMAGE_TO_HP;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_DAMAGE_TO_HP;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP;
				break;

			case 2:	// 데미지를 MP 로 변환
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_DAMAGE_TO_MP;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_DAMAGE_TO_MP;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP;
				break;

			case 3:	// MP 를 HP 로 변환
				//pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_MP_TO_HP;
				//if(bProcessInterval)
				//	pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CONVERT_MP_TO_HP;
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_MP_TO_HP;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_MP_TO_HP;
				break;

			// 2005.07.12. steeple
			case 4:	// 공격 데미지를 HP 로 변환
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP;
				break;

			// 2005.07.12. steeple
			case 5:	// 공격 데미지를 HP 로 변환
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP;
				break;

			// 2007.07.04. steeple
			case 6:	// 공격하는 데미지를 바로 HP 로 % 만큼 전환. 버프형이 아니라 한방형이다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP_INSTANT;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP_INSTANT;
				break;
		}
	}
	
	// charge
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CHARGE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CHARGE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CHARGE;
	}

	// level up
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_SKILL_LEVELUP))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SKILL_LEVELUP;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SKILL_LEVELUP;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] |= AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3;
		}
	}

	// product
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_PRODUCT))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_PRODUCT;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_PRODUCT;
	}
	
	// Twice Packet
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_TWICE_PACKET))
	{
		pcsSkillTemplate->m_bTwicePacket = TRUE;

		// 2005.06.29. steeple 추가
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TWICE_PACKET;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TWICE_PACKET;

		switch(lValue)
		{
			case 1:	// 거리에 비례하는 Interval
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] |= AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_DISTANCE;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] |= AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_DISTANCE;
				break;

			case 2:	// 그냥 정해진 시간이 되면 발동하는 Interval
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] |= AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_STATIC_TIME;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET] |= AGPMSKILL_EFFECT_DETAIL_TWICE_PACKET_BY_STATIC_TIME;
				break;
		}
	}

	// auto attack skill
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_AUTO_ATTACK))
	{
		pcsSkillTemplate->m_bIsAutoAttack = TRUE;
	}
	
	// dynamic target
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DYNAMIC_TARGET))
	{
		// 2005.06.29. steeple 추가
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DYNAMIC_TARGET;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DYNAMIC_TARGET;
	}

	// action on action
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ACTION_ON_ACTION))
	{
		// 2005.06.29. steeple 추가
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;

		switch(lValue)
		{
			case 1:	// 버프가 끝나는 시점에 데미지를 준다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1;
				break;

			case 2:	// 캐스팅이 끝나는 시점에 타겟에 텔레포트 시키고 공격한다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2;
				break;

			case 3:	// 공격을 당했을 때 공격한 놈에게 스킬 건다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3;
				break;

			case 4:	// 공격이나 스킬을 사용했을 때 대상에 이펙트를 터뜨린다. ex)렌즈스톤 2005.12.13. steeple 추가
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4;
				break;

			case 5:	// 버프 체크를 하면서 원래 시전자가 죽으면 버프를 해제한다. 주로 공성쪽.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5;
				break;

			case 6:	// 죽기 전에 Additional Skill TID 를 시전해준다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6;
				break;

			case 7:	// 상대방에게 버프된 스킬을 체크해서 2차 팩터값을 추가 하는 스킬.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE7;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE7;
				break;

			case 8:	// 인비지블 상태일 때 사용하면 2차 팩터값을 추가 하는 스킬.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE8;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE8;
				break;
		}
	}

	// AT Field
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_AT_FIELD))
	{
		// 2005.06.29. steeple 추가
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_AT_FIELD;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_AT_FIELD;

		switch(lValue)
		{
			case 1:	// 타겟 주위로 공격형 버프를 건다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK;
				break;

			case 2:	// 타겟 주위의 공격대상 타겟에게 Attack 한다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2;
				break;

			case 3:	// 타겟 주위로 오라가 펼쳐진다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] |= AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA;
				break;
		}
	}

	// summons
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_SUMMONS))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SUMMONS;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SUMMONS;

		switch(lValue)
		{
			case 1:	// Summon Walk & Run Summons
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1;
				break;

			case 2:	// Summon StandAlone Summons
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2;
				break;

			case 3:	// Taming (Bad Friend)
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3;
				break;

			case 4:	// Increase Max Summons
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4;
				break;

			case 5:	// Cast skill at any summons to summon, skill learn and skill upgrade.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5;
				break;

			case 6:	// Summon Walk & Run Summons for Siege War
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6;
				break;

			case 7:	// Spawn Monster
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7;
				break;

			case 8:	// Pet
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8;
				break;

			case 9:	// Attack Spawn (Same Type1 but Difference with summon pos and time of dead)
				{
					pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4;
					pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7;
					if(bProcessInterval)
					{
						pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4;
						pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7;
					}

				}

			case 10:
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10;
				break;
		}
	}

	// action passive
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ACTION_PASSIVE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ACTION_PASSIVE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ACTION_PASSIVE;
	}

	// duration type
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DURATION_TYPE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DURATION_TYPE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DURATION_TYPE;

		switch(lValue)
		{
			case 1:	// 거리에 따라서 스킬 지속시간이 바뀐다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE] |= AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE1;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE] |= AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE1;
				break;

			case 2:	// 지속시간이 패시브처럼 무한대이다.
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE] |= AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE2;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE] |= AGPMSKILL_EFFECT_DETAIL_DURATION_TYPE2;
				break;
		}
	}

	// limited max level
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_LIMITED_MAX_LEVEL))
	{
		if(lValue > 0 && lValue <= AGPMSKILL_MAX_UPGRADE_SP)
			pcsSkillTemplate->m_lLimitedMaxLevel = lValue;
	}

	// Release Target
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_RELEASE_TARGET))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RELEASE_TARGET;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RELEASE_TARGET;
	}

	// Visible Effect
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_VISIBLE_EFFECT_TYPE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_VISIBLE_EFFECT_TYPE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_VISIBLE_EFFECT_TYPE;
	}

	// Game Bonus
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_GAME_BONUS))
	{
		if(lValue != 0)
		{
			pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_GAME_BONUS;
			if(bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_GAME_BONUS;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2;;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] |= AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE;
		}

	}

	// Item Use Type
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_ITEM_USE_TYEPE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ITEM_USE_TYPE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_ITEM_USE_TYPE;
	}

	// Cash
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_CASH))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CASH;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_CASH;
	}

	// Level Diff Type
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_LEVEL_DIFF_TYPE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_LEVEL_DIFF_TYPE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_LEVEL_DIFF_TYPE;
	}

	// Detect
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DETECT))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DETECT;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DETECT;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] |= AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] |= AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] |= AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] |= AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2;
		}
	}

	// Ride
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_RIDE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RIDE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RIDE;
	}

	// Game Effect
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_GAME_EFFECT))
	{
		if(lValue != 0)
		{
			pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_GAME_EFFECT;
			if(bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_GAME_EFFECT;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] |= AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW;
		}
	}

	// Force Skill
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_FORCE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_FORCE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_FORCE;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE1)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE1;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE1;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE2)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE2;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE2;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE3)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE3;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_FORCE] |= AGPMSKILL_EFFECT_DETAIL_FORCE_TYPE3;
		}
	}

	// Divide
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DIVIDE))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DIVIDE;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DIVIDE;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL;
		}
	}

	// Resurrection
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_RESURRECTION))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RESURRECTION;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_RESURRECTION;
	}

	// Skill Union
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_SKILL_UNION_SPEC))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SKILL_UNION;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_SKILL_UNION;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] |= AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] |= AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1;
		}

		if(lValue & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] |= AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] |= AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2;
		}
	}

	// Target Restriction
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_TARGET_RESTRICTION))
	{
		pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TARGET_RESTRICTION;
		if(bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TARGET_RESTRICTION;

		if(lValue & AGPMSKILL_EFFECT_DETAIL_TARGET_RESTRICTION_SIEGE_OBJECT)
		{
			pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TARGET_RESTRICTION] |= AGPMSKILL_EFFECT_DETAIL_TARGET_RESTRICTION_SIEGE_OBJECT;
			if(bProcessInterval)
				pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TARGET_RESTRICTION] |= AGPMSKILL_EFFECT_DETAIL_TARGET_RESTRICTION_SIEGE_OBJECT;
		}
	}

	// Target Restriction
	else if(!strcmp(szTypeName, AGPMSKILL_EXCEL_COLUMN_DISTURB_CHARACTER_ACTION))
	{

		if(lValue > 1000)
		{
			pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION;
			if(bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION;

			lValue = lValue - 1000;

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL;
			}
		}
		else
		{
			pcsSkillTemplate->m_lEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION;
			if(bProcessInterval)
				pcsSkillTemplate->m_lProcessIntervalEffectType2[nIndex] |= AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION;

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM;
			}

			if(lValue & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
			{
				pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL;
				if(bProcessInterval)
					pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] |= AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL;
			}
		}	
		
	}

	/*
	// end effect
	else if (!strncmp(szTypeName, AGSMSKILL_EXCEL_COLUMN_ENDSKILL_COST_HP, strlen(AGSMSKILL_EXCEL_COLUMN_ENDSKILL_COST_HP)))
	{
		pcsSkillTemplate->m_lEffectType[nIndex] |= AGPMSKILL_ENDSKILL_CONSUME_HP;
		if (bProcessInterval)
			pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex] |= AGPMSKILL_ENDSKILL_CONSUME_HP;
	}
	*/

	return TRUE;
}

BOOL AgpmSkill::SetTooptipType(AgpdSkillTooltipTemplate *pcsTooltip, CHAR *szTypeName, CHAR *szValue, BOOL bIsFactorPoint)
{
	if (!pcsTooltip || !szTypeName || !szTypeName[0] || !szValue || !szValue[0])
		return FALSE;


	// skill description
	///////////////////////////////////////////////////////////////////
	if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_SKILL_DESCRIPTION))
		strncpy(pcsTooltip->m_szSkillDescription, szValue, AGPMSKILL_MAX_SKILL_DESCRIPTION);
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_SKILL_MINI_DESCRIPTION))
		strncpy(pcsTooltip->m_szSkillMiniDescription, szValue, AGPMSKILL_MAX_SKILL_MINI_DESCRIPTION);

	//	define basic attribute type
	/////////////////////////////////////////////////////////////////////
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_LEVEL))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_LEVEL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_TYPE))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_TYPE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_DETAIL_TYPE))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_DETAIL_TYPE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_KNIGHT))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_KNIGHT;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_ARCHER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ARCHER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_WIZARD))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_WIZARD;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_BERSERKER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_BERSERKER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_HUNTER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_HUNTER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_SORCERER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SORCERER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_ASSASSIN))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ASSASSIN;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_RANGER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_RANGER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_ELEMENTALLIST))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_ELEMENTALLIST;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_SLAYER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SLAYER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_SCOUTER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SCOUTER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_SUMMONER))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SUMMONER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CLASS_SCION))
		pcsTooltip->m_ullBasicAttributeType		|=  AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CLASS_SCION;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_LEVEL))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_LEVEL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_POINT))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_POINT;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_HEROIC))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_HEROIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BASIC_ATTR_REQUIRE_CHARISMA))
		pcsTooltip->m_ullBasicAttributeType		|=	AGPMSKILL_TOOLTIP_BASIC_ATTR_REQUIRE_CHARISMA;
	
	//	define basic effect type
	/////////////////////////////////////////////////////////////////////
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_CAST_TIME))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_CAST_TIME;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RECAST_TIME))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_RECAST_TIME;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DURATION))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_DURATION;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RANGE_DISTANCE))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_RANGE_DISTANCE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RANGE_RADIUS))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_RANGE_RADIUS;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RANGE_BOX_X))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_RANGE_BOX_X;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RANGE_BOX_Y))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTIP_RANGE_BOX_Y;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_INTERVAL))
		pcsTooltip->m_ullBasicEffectType		|=	AGPMSKILL_TOOLTOP_INTERVAL;

	//	define cost attribute type
	/////////////////////////////////////////////////////////////////////
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_COST_HP))
		pcsTooltip->m_ullCostAttributeType		|=	AGPMSKILL_TOOLTIP_COST_HP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_COST_MP))
		pcsTooltip->m_ullCostAttributeType		|=	AGPMSKILL_TOOLTIP_COST_MP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_COST_ARROW))
		pcsTooltip->m_ullCostAttributeType		|=	AGPMSKILL_TOOLTIP_COST_ARROW;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_ENDSKILL_COST_HP))
		pcsTooltip->m_ullCostAttributeType		|=	AGPMSKILL_TOOLTIP_ENDSKILL_COST_HP;

	//	define damage attribute type
	/////////////////////////////////////////////////////////////////////
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PHYSICAL))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PHYSICAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_MAGIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_MAGIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_WATER))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_WATER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_AIR))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_AIR;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_EARTH))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_EARTH;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_FIRE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_FIRE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_POISON))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_POISON;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_ICE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_ICE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_LIGHTENING))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_LIGHTENING;

	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_PHYSICAL))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_PHYSICAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_MAGIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_MAGIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_WATER))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_WATER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_AIR))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_AIR;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_EARTH))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_EARTH;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_FIRE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_FIRE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_POISON))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_POISON;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_ICE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_ICE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_LIGHTENING))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_LIGHTENING;

	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_HEROIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_HEROIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_PERCENT_HEROIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DAMAGE_PERCENT_HEROIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_HEROIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_HEROIC;

	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_PHYSICAL))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_PHYSICAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_MAGIC))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_MAGIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_WATER))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_WATER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_AIR))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_AIR;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_EARTH))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_EARTH;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_FIRE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_FIRE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_POISON))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_POISON;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_ICE))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_ICE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_DAMAGE_LIGHTENING))
		pcsTooltip->m_ullDamageAttributeType		|=	AGPMSKILL_TOOLTIP_DOT_DAMAGE_LIGHTENING;

	//	define special effect type
	/////////////////////////////////////////////////////////////////////
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_MOVE_DISTANCE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_MOVE_DISTANCE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_REFLECT_DAMAGE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_REFLECT_DAMAGE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BLOCK_RATE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_BLOCK_RATE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_STUN_TIME))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_STUN_TIME;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_CRITICAL))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_CRITICAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_ADJUST_SKILL_RANGE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_ADJUST_SKILL_RANGE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_SKILL_RATE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_SKILL_RATE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_CONVERT_HP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DAMAGE_CONVERT_HP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DAMAGE_CONVERT_MP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DAMAGE_CONVERT_MP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_MP_CONVERT_HP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_MP_CONVERT_HP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_REGEN_HP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_REGEN_HP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_REGEN_MP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_REGEN_MP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_CHARGE_DAMAGE_1))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_1;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_CHARGE_DAMAGE_2))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_2;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_CHARGE_DAMAGE_3))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_CHARGE_DAMAGE_3;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_AGRODOLL_HP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_AGRODOLL_HP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DOT_INTERVAL))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DOT_INTERVAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DECREASE_COST_MP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DECREASE_COST_MP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DECREASE_CAST_TIME))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DECREASE_CAST_TIME;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_ADJUST_SKILL_LEVEL))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_ADJUST_SKILL_LEVEL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DEATH))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_DEATH;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_LEARN_COST))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_LEARN_COST;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_SUMMONS))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_SUMMONS;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_ADDITIONAL_DURATION))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_ADDITIONAL_DURATION;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_MAX_TARGET_NUM))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_MAX_TARGET_NUM;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_LIMIT_QUANTITY))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_LIMIT_QUANTITY;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_SUMMONS_COUNT))
	{
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_SUMMONS_COUNT;
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_LIMIT_QUANTITY;
	}
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BONUS_EXP))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_BONUS_EXP;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BONUS_MONEY))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_BONUS_MONEY;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_BONUS_DROP_RATE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_BONUS_DROP_RATE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_DIVIDE_RATE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_BONUS_DIVIDE_RATE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RATE_CRITICAL))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_RATE_CRITICAL_ACTIVITY;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_RATE_STUN))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_RATE_STUN_ACTIVITY;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_INCREASE_SUMMON_LEVEL))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_ADJUST_SUMMON_LEVEL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EFFECT_NUMBER))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_EFFECT_SUMMONS_QUANTITY;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_NUMBER_ITEM))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_QUANTITY_ITEM;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_PERCENT_DURABILITY))
		pcsTooltip->m_ullSpecialEffectType		|=  AGPMSKILL_TOOLTIP_DURABILITY;
	
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_PHYSICAL))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_PHYSICAL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_MAGIC))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_MAGIC;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_WATER))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_WATER;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_AIR))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_AIR;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_EARTH))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_EARTH;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_FIRE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_FIRE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_POISON))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_POISON;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_ICE))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_ICE;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_EA_DMG_POINT_LIGHTENING))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_EA_DMG_POINT_LIGHTENING;

	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_STUN_GENERATE_TIME ))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_STUN_GENERATE_TIME;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_TIME_CONTROL))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_TIME_CONTROL;
	else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_ITEM_PART))
		pcsTooltip->m_ullSpecialEffectType		|=	AGPMSKILL_TOOLTIP_ITEM_PART;

	if (bIsFactorPoint)
	{
		//	define factor point type
		/////////////////////////////////////////////////////////////////////
		if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_CON))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_CON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_STR))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_STR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_INT))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_INT;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEX))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEX;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_CHA))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_CHA;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_WIS))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_WIS;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_MOVEMENT))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_MOVEMENT;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_HP))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_HP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_MP))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_MP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_AGRO))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_AGRO;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_MAX_HP))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_MAX_HP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_MAX_MP))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_MAX_MP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_ATK_SPEED))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_ATK_SPEED;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_ATK_RANGE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_ATK_RANGE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_SKILL_CAST))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_SKILL_CAST;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_SKILL_DELAY))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_SKILL_DELAY;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_HIT_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_HIT_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_EVADE_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_EVADE_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DODGE_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DODGE_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_PHYSICAL))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_MAGIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_WATER))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_AIR))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_EARTH))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_FIRE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_POISON))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_ICE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_LIGHTENING))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_PHYSICAL))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_MAGIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_WATER))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_AIR))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_EARTH))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_FIRE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_POISON))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_ICE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_LIGHTENING))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_PHYSICAL))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_MAGIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_WATER))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_AIR))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_EARTH))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_FIRE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_POISON))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_ICE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_LIGHTENING))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_RATE_PHYSICAL))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_PHYSICAL_BLOCK))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_PHYSICAL_BLOCK;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_RATE_SKILL_BLOCK))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_RATE_SKILL_BLOCK;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MIN_HEROIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MIN_HEROIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DMG_MAX_HEROIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DMG_MAX_HEROIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_POINT_DEF_POINT_HEROIC))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_POINT_DEF_POINT_HEROIC;

		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_MELEEHEROIC_RESISTANCE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MEELEHEROIC_RESISTANCE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_RANGEHEROIC_RESISTANCE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_RANGEHEROIC_RESISTANCE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_MAGICHEROIC_RESISTANCE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MAGICHEROIC_RESISTANCE;

	}
	else
	{
		//	define factor percent type
		/////////////////////////////////////////////////////////////////////
		if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_CON))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_CON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_STR))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_STR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_INT))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_INT;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEX))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEX;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_CHA))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_CHA;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_WIS))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_WIS;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_MOVEMENT))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MOVEMENT;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_HP))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_HP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_MP))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_AGRO))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_AGRO;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_MAX_HP))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MAX_HP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_MAX_MP))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_MAX_MP;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_ATK_SPEED))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_ATK_SPEED;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_ATK_RANGE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_ATK_RANGE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_SKILL_CAST))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_SKILL_CAST;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_SKILL_DELAY))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_SKILL_DELAY;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_HIT_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_HIT_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_EVADE_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_EVADE_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DODGE_RATE))
			pcsTooltip->m_ullFactorPointType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DODGE_RATE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_PHYSICAL))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_MAGIC))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_WATER))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_AIR))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_EARTH))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_FIRE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_POISON))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_ICE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MIN_LIGHTENING))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MIN_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_PHYSICAL))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_MAGIC))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_WATER))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_AIR))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_EARTH))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_FIRE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_POISON))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_ICE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DMG_MAX_LIGHTENING))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DMG_MAX_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_PHYSICAL))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_MAGIC))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_MAGIC;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_WATER))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_WATER;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_AIR))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_AIR;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_EARTH))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_EARTH;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_FIRE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_FIRE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_POISON))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_POISON;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_ICE))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_ICE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_POINT_LIGHTENING))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_POINT_LIGHTENING;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_RATE_PHYSICAL))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_PHYSICAL_BLOCK))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_PHYSICAL_BLOCK;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_RATE_SKILL_BLOCK))
			pcsTooltip->m_ullFactorPercentType		|=	AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_RATE_SKILL_BLOCK;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_IGNORE_PHYSICAL))
			pcsTooltip->m_ullFactorPercentType		|=  AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_IGNORE_PHYSICAL;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_IGNORE_ATTRIBUTE))
			pcsTooltip->m_ullFactorPercentType		|=  AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_IGNORE_ATTRIBUTE;
		else if (!strcmp(szTypeName, AGPMSKILL_TOOLTIP_COLUMN_FACTOR_PERCENT_DEF_CRITICAL))
			pcsTooltip->m_ullFactorPercentType		|=  AGPMSKILL_TOOLTIP_FACTOR_PERCENT_DEF_CRITICAL;
	}

	return TRUE;
}

INT16 AgpmSkill::ConvertFactorMagnifyIndex(INT16 nIndex)
{
	switch (nIndex) {
	case AGPMSKILL_CONST_POINT_MOVEMENT:
		return AGPMSKILL_CONST_MAGNIFY_MOVEMENT;
		break;

	case AGPMSKILL_CONST_POINT_ATKSPEED:
		return AGPMSKILL_CONST_MAGNIFY_ATKSPEED;
		break;
	}

	return (-1);
}

