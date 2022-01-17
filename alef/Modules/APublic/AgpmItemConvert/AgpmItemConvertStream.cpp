#include "AgpmItemConvert.h"
#include "AgpmItemConvertStream.h"

BOOL AgpmItemConvert::StreamReadConvertTable(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmItemConvert::StreamReadConvertTable() Error (1) !!!\n");
		return FALSE;
	}

	INT32			lRow			= csExcelTxtLib.GetRow();
	INT32			lColumn			= csExcelTxtLib.GetColumn();

	INT32			lCurRow			= 0;
	INT32			lCurColumn		= 0;

	for ( ; ; )
	{
		CHAR			*szTableName	= csExcelTxtLib.GetData(0, lCurRow);
		if (!szTableName || !strlen(szTableName))
		{
			++lCurRow;

			if (lCurRow > lRow)
				break;

			continue;
		}

		if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL, strlen(AGPMITEMCONVERT_TABLE_PHYSICAL)) == 0)
		{
			if (!StreamConvertPhysicalTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strcmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL_SUCCESS) == 0)
		{
			if (!StreamConvertPhysicalSuccessTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strcmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_NONE) == 0)
		{
			if (!StreamConvertPhysicalFailTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strcmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_INIT) == 0)
		{
			if (!StreamConvertPhysicalInitTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strcmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL_DESTROY) == 0)
		{
			if (!StreamConvertPhysicalDestroyTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		/*
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL, strlen(AGPMITEMCONVERT_TABLE_PHYSICAL_FAIL)) == 0)
		{
			if (!StreamConvertPhysicalFailTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_ADD_SOCKET, strlen(AGPMITEMCONVERT_TABLE_ADD_SOCKET)) == 0)
		{
			if (!StreamConvertAddSocketTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_ADD_SOCKET_FAIL, strlen(AGPMITEMCONVERT_TABLE_ADD_SOCKET_FAIL)) == 0)
		{
			if (!StreamConvertAddSocketFailTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		*/
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_SPIRITSTONE, strlen(AGPMITEMCONVERT_TABLE_SPIRITSTONE)) == 0)
		{
			if (!StreamConvertSpiritStoneTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_BONUS, strlen(AGPMITEMCONVERT_TABLE_BONUS)) == 0)
		{
			if (!StreamConvertAddBonusTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_RUNE, strlen(AGPMITEMCONVERT_TABLE_RUNE)) == 0)
		{
			if (!StreamConvertRuneTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
		else if (strncmp(szTableName, AGPMITEMCONVERT_TABLE_RUNE_FAIL, strlen(AGPMITEMCONVERT_TABLE_RUNE_FAIL)) == 0)
		{
			if (!StreamConvertRuneFailTable(&csExcelTxtLib, &lCurRow))
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertPhysicalTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lMaxColumn			= pcsExcelTxtLib->GetColumn();

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lRank			= atoi(szColumnName);
		if (lRank < 1 || lRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
			return FALSE;

		for (int i = 1; i < lMaxColumn; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (!szColumnName || !szColumnName[0])
				break;

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_ADD, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_ADD)) == 0)
				m_astTablePhysical[lRank].lWeaponAddValue	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			//else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_PROB, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_WEAPON_PROB)) == 0)
			//	m_astTablePhysical[lLevel].lWeaponProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_ADD, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_ADD)) == 0)
				m_astTablePhysical[lRank].lArmourAddValue	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			//else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_PROB, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_ARMOUR_PROB)) == 0)
			//	m_astTablePhysical[lLevel].lArmourProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_RANK) == 0)
			{
				CHAR	*pszRankName	= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszRankName && pszRankName[0])
					strcpy(m_astTablePhysical[lRank].szRank, pszRankName);
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_1) == 0)
			{
				CHAR	*pszMark		= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszMark && pszMark[0])
					m_astTablePhysical[lRank].bIsConvertableSpirit[1]	= TRUE;
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_2) == 0)
			{
				CHAR	*pszMark		= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszMark && pszMark[0])
					m_astTablePhysical[lRank].bIsConvertableSpirit[2]	= TRUE;
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_3) == 0)
			{
				CHAR	*pszMark		= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszMark && pszMark[0])
					m_astTablePhysical[lRank].bIsConvertableSpirit[3]	= TRUE;
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_4) == 0)
			{
				CHAR	*pszMark		= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszMark && pszMark[0])
					m_astTablePhysical[lRank].bIsConvertableSpirit[4]	= TRUE;
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_SPIRIT_5) == 0)
			{
				CHAR	*pszMark		= pcsExcelTxtLib->GetData(i, *pnCurRow);
				if (pszMark && pszMark[0])
					m_astTablePhysical[lRank].bIsConvertableSpirit[5]	= TRUE;
			}
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertPhysicalSuccessTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lMaxColumn			= pcsExcelTxtLib->GetColumn();

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lRank			= atoi(szColumnName);
		if (lRank < 1 || lRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
			return FALSE;

		for (int i = 1; i < lMaxColumn; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (!szColumnName || !szColumnName[0])
				break;

			INT32	lConvertStep	= atoi(szColumnName);
			if (lConvertStep < 1 || lConvertStep > AGPDITEMCONVERT_MAX_ITEM_RANK)
				return FALSE;

			m_astTablePhysical[lRank].lSuccessProb[lConvertStep]	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertPhysicalFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lMaxColumn			= pcsExcelTxtLib->GetColumn();

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lRank			= atoi(szColumnName);
		if (lRank < 1 || lRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
			return FALSE;

		for (int i = 1; i < lMaxColumn; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (!szColumnName || !szColumnName[0])
				break;

			INT32	lConvertStep	= atoi(szColumnName);
			if (lConvertStep < 1 || lConvertStep > AGPDITEMCONVERT_MAX_ITEM_RANK)
				return FALSE;

			m_astTablePhysical[lRank].lFailProb[lConvertStep]	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertPhysicalInitTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lMaxColumn			= pcsExcelTxtLib->GetColumn();

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lRank			= atoi(szColumnName);
		if (lRank < 1 || lRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
			return FALSE;

		for (int i = 1; i < lMaxColumn; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (!szColumnName || !szColumnName[0])
				break;

			INT32	lConvertStep	= atoi(szColumnName);
			if (lConvertStep < 1 || lConvertStep > AGPDITEMCONVERT_MAX_ITEM_RANK)
				return FALSE;

			m_astTablePhysical[lRank].lInitProb[lConvertStep]	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));

		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertPhysicalDestroyTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lMaxColumn			= pcsExcelTxtLib->GetColumn();

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lRank			= atoi(szColumnName);
		if (lRank < 1 || lRank > AGPDITEMCONVERT_MAX_ITEM_RANK)
			return FALSE;

		for (int i = 1; i < lMaxColumn; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (!szColumnName || !szColumnName[0])
				break;

			INT32	lConvertStep	= atoi(szColumnName);
			if (lConvertStep < 1 || lConvertStep > AGPDITEMCONVERT_MAX_ITEM_RANK)
				return FALSE;

			m_astTablePhysical[lRank].lDestroyProb[lConvertStep]	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));

		}

		++*pnCurRow;
	}

	return TRUE;
}

/*
BOOL AgpmItemConvert::StreamConvertPhysicalFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szColumnName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_PHYSICAL_CONVERT)
			return FALSE;

		for (int i = 1; i < 5; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_NONE)) == 0)
				m_astTablePhysicalFail[lLevel].lKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT_SAME)) == 0)
				m_astTablePhysicalFail[lLevel].lInitializeSame	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_INIT)) == 0)
				m_astTablePhysicalFail[lLevel].lInitialize	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_PHYSICAL_FAIL_DESTROY)) == 0)
				m_astTablePhysicalFail[lLevel].lDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertAddSocketTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szColumnName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;

		for (int i = 1; i < 7; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON)) == 0)
				m_astTableSocket[lLevel].lWeaponProb		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_COST, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_COST)) == 0)
				m_astTableSocket[lLevel].lWeaponCost		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR)) == 0)
			{
				if (lLevel <= AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
					m_astTableSocket[lLevel].lArmourProb	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			}
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_COST, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_COST)) == 0)
			{
				if (lLevel <= AGPDITEMCONVERT_MAX_ARMOUR_SOCKET)
					m_astTableSocket[lLevel].lArmourCost	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			}
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC)) == 0)
			{
				if (lLevel <= AGPDITEMCONVERT_MAX_ETC_SOCKET)
					m_astTableSocket[lLevel].lEtcProb		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			}
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_COST, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_COST)) == 0)
			{
				if (lLevel <= AGPDITEMCONVERT_MAX_ETC_SOCKET)
					m_astTableSocket[lLevel].lEtcCost		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			}
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertAddSocketFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szColumnName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;

		for (int i = 1; i < 13; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_NONE)) == 0)
				m_astTableSocketFail[lLevel].lWeaponKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT_SAME)) == 0)
				m_astTableSocketFail[lLevel].lWeaponInitializeSame	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_INIT)) == 0)
				m_astTableSocketFail[lLevel].lWeaponInitialize	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_WEAPON_FAIL_DESTROY)) == 0)
				m_astTableSocketFail[lLevel].lWeaponDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_NONE)) == 0)
				m_astTableSocketFail[lLevel].lArmourKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT_SAME)) == 0)
				m_astTableSocketFail[lLevel].lArmourInitializeSame	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_INIT)) == 0)
				m_astTableSocketFail[lLevel].lArmourInitialize	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ARMOUR_FAIL_DESTROY)) == 0)
				m_astTableSocketFail[lLevel].lArmourDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_NONE)) == 0)
				m_astTableSocketFail[lLevel].lEtcKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT_SAME)) == 0)
				m_astTableSocketFail[lLevel].lEtcInitializeSame		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_INIT)) == 0)
				m_astTableSocketFail[lLevel].lEtcInitialize		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_ADD_SOCKET_ETC_FAIL_DESTROY)) == 0)
				m_astTableSocketFail[lLevel].lEtcDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}
*/

BOOL AgpmItemConvert::StreamConvertSpiritStoneTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szColumnName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_SPIRIT_STONE)
			return FALSE;

		for (int i = 1; i < 5; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_ADD, strlen(AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_ADD)) == 0)
				m_astTableSpiritStone[lLevel].lWeaponAddValue		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_PROB, strlen(AGPMITEMCONVERT_COLUMN_SPIRITSTONE_WEAPON_PROB)) == 0)
				m_astTableSpiritStone[lLevel].lWeaponProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_ADD, strlen(AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_ADD)) == 0)
				m_astTableSpiritStone[lLevel].lArmourAddValue		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_PROB, strlen(AGPMITEMCONVERT_COLUMN_SPIRITSTONE_ARMOUR_PROB)) == 0)
				m_astTableSpiritStone[lLevel].lArmourProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertAddBonusTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	CHAR			*szValue	= pcsExcelTxtLib->GetData(1, *pnCurRow + 1);
	if (!szValue || !strlen(szValue))
		return FALSE;

	m_stTableSameAttrBonus.lWeaponBonus		= atoi(szValue);

	szValue	= pcsExcelTxtLib->GetData(2, *pnCurRow + 1);
	if (!szValue || !strlen(szValue))
		return FALSE;

	m_stTableSameAttrBonus.lArmourBonus		= atoi(szValue);

	*pnCurRow += 2;

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertRuneTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szColumnName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szColumnName || !strlen(szColumnName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szColumnName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;

		for (int i = 1; i < 4; ++i)
		{
			szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_PROB, strlen(AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_PROB)) == 0)
				m_astTableRune[lLevel].lWeaponProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_PROB, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_PROB)) == 0)
				m_astTableRune[lLevel].lArmourProbability	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ETC_PROB, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ETC_PROB)) == 0)
				m_astTableRune[lLevel].lEtcProbability		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamConvertRuneFailTable(AuExcelLib *pcsExcelTxtLib, INT32 *pnCurRow)
{
	if (!pcsExcelTxtLib || !pnCurRow)
		return FALSE;

	INT32	lColumnNameIndex	= *pnCurRow;

	++*pnCurRow;

	for ( ; ; )
	{
		CHAR			*szLevelName	= pcsExcelTxtLib->GetData(0, *pnCurRow);
		if (!szLevelName || !strlen(szLevelName))
		{
			++*pnCurRow;

			break;
		}

		INT32			lLevel			= atoi(szLevelName);
		if (lLevel < 1 || lLevel > AGPDITEMCONVERT_MAX_WEAPON_SOCKET)
			return FALSE;

		for (int i = 1; i < 13; ++i)
		{
			CHAR	*szColumnName	= pcsExcelTxtLib->GetData(i, lColumnNameIndex);

			if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_NONE)) == 0)
				m_astTableRuneWeaponFail[lLevel].lWeaponKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT_SAME)) == 0)
				m_astTableRuneWeaponFail[lLevel].lWeaponInitializeSame	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_INIT)) == 0)
				m_astTableRuneWeaponFail[lLevel].lWeaponInitialize	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_RUNE_WEAPON_FAIL_DESTROY)) == 0)
				m_astTableRuneWeaponFail[lLevel].lWeaponDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_NONE)) == 0)
				m_astTableRuneWeaponFail[lLevel].lArmourKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT_SAME)) == 0)
				m_astTableRuneWeaponFail[lLevel].lArmourInitializeSame	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_INIT)) == 0)
				m_astTableRuneWeaponFail[lLevel].lArmourInitialize	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_FAIL_DESTROY)) == 0)
				m_astTableRuneWeaponFail[lLevel].lArmourDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_NONE, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_NONE)) == 0)
				m_astTableRuneWeaponFail[lLevel].lEtcKeepCurrent	= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT_SAME, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT_SAME)) == 0)
				m_astTableRuneWeaponFail[lLevel].lEtcInitializeSame		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_INIT)) == 0)
				m_astTableRuneWeaponFail[lLevel].lEtcInitialize		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else if (strncmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_DESTROY, strlen(AGPMITEMCONVERT_COLUMN_RUNE_ETC_FAIL_DESTROY)) == 0)
				m_astTableRuneWeaponFail[lLevel].lEtcDestroy		= atoi(pcsExcelTxtLib->GetData(i, *pnCurRow));
			else
				return FALSE;
		}

		++*pnCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamReadRuneAttribute(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmItemConvert::StreamReadConvertTable() Error (1) !!!\n");
		return FALSE;
	}

	INT32			lRow			= csExcelTxtLib.GetRow();
	INT32			lColumn			= csExcelTxtLib.GetColumn();

	INT32			lCurRow			= 1;
	INT32			lCurColumn		= 0;

	for ( ; ; )
	{
		CHAR			*szRuneTID	= csExcelTxtLib.GetData(1, lCurRow);
		if (!szRuneTID || !strlen(szRuneTID))
		{
			++lCurRow;

			if (lCurRow > lRow)	break;

			continue;
		}

		AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(atoi(szRuneTID));
		if (!pcsItemTemplate)
		{
			++lCurRow;
			continue;
		}

		AgpdItemConvertADItemTemplate	*pcsAttachTemplateData	= GetADItemTemplate(pcsItemTemplate);

		CHAR	*szColumnName	= NULL;
		CHAR	*szValue		= NULL;

		for (int i = 2; i < lColumn; ++i)
		{
			szColumnName	= csExcelTxtLib.GetData(i, 0);

			if (!szColumnName || !strlen(szColumnName))
				break;

			if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ANTI_TYPE_NUMBER) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_lAntiTypeNumber	= atoi(szValue);
				}
			}
			else if (strcmp(szColumnName, AGPMTTEMCONVERT_COLUMN_RUNE_ARMOUR_BODY) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_BODY]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMOUR_LEGS) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_LEGS]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_WEAPON) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_WEAPON_INDEX]	= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_SHIELD) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_SHIELD_INDEX]	= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_HEAD) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_HEAD]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_RING) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_RING_INDEX]		= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_ACCESSORY_RING1]	= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_NECKLACE) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_NECKLACE_INDEX]			= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_ACCESSORY_NECKLACE]	= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_ARMS) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_ARMS]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_FOOT) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_FOOT]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_HANDS) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_bRuneConvertableEquipKind[AGPMITEM_EQUIP_KIND_ARMOUR_INDEX]	= TRUE;
					pcsAttachTemplateData->m_bRuneConvertableEquipPart[AGPMITEM_PART_HANDS]			= TRUE;
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_PROBABILITY) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_lRuneSuccessProbability	= atoi(szValue);
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_RESTRICT_LEVEL) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_lRuneRestrictLevel			= atoi(szValue);
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_DESCRIPTION) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					ZeroMemory(pcsAttachTemplateData->m_szDescription, sizeof(CHAR) * AGPMSKILL_MAX_SPECIALIZE_DESCRIPTION);
					strncpy_s(pcsAttachTemplateData->m_szDescription, AGPMSKILL_MAX_SPECIALIZE_DESCRIPTION, szValue, _TRUNCATE);
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_SKILL_NAME) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(szValue);
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_SKILL_LEVEL) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_lSkillLevel	= atoi(szValue);
				}
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_RUNE_SKILL_PROBABILITY) == 0)
			{
				szValue		= csExcelTxtLib.GetData(i, lCurRow);

				if (szValue && szValue[0])
				{
					pcsAttachTemplateData->m_lSkillProbability	= atoi(szValue);
				}
			}
		}

		++lCurRow;
	}

	return TRUE;
}

BOOL AgpmItemConvert::StreamReadConvertPointTable(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !szFile[0])
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgpmItemConvert::StreamReadConvertTable() Error (1) !!!\n");
		return FALSE;
	}

	INT32			lRow			= csExcelTxtLib.GetRow();
	INT32			lColumn			= csExcelTxtLib.GetColumn();

	for (int lCurRow = 1; lCurRow < lRow; ++lCurRow)
	{
		for (int lCurColumn = 0; lCurColumn < lColumn; ++lCurColumn)
		{
			CHAR	*szData	= csExcelTxtLib.GetData(lCurColumn, lCurRow);
			if (!szData || !strlen(szData))
				continue;

			CHAR	*szColumnName	= csExcelTxtLib.GetData(lCurColumn, 0);

			if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_POINT_PHYSICAL_POINT) == 0)
			{
				m_stConvertPoint.lPhysicalPoint[lCurRow]	= atoi(szData);
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_POINT_SPIRITSTONE_POINT) == 0)
			{
				m_stConvertPoint.lSpiritStonePoint[lCurRow]	= atoi(szData);
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_POINT_RUNE_POINT) == 0)
			{
				m_stConvertPoint.lRunePoint[lCurRow]		= atoi(szData);
			}
			else if (strcmp(szColumnName, AGPMITEMCONVERT_COLUMN_POINT_SOCKET_POINT) == 0)
			{
				m_stConvertPoint.lSocketPoint[lCurRow]		= atoi(szData);
			}
		}
	}

	return TRUE;
}