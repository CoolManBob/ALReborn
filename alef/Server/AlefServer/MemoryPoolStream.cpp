#include "MemoryPoolStream.h"
#include "AuGameEnv.h"

BOOL MemoryPoolStream::ReadPoolCount(const CHAR *szFile)
{
	if (!szFile || strlen(szFile) <= 0)
		return FALSE;

	GetGameEnv().InitEnvironment();

	FILE*		file	= fopen("Ini\\NotUseMemoryPool.txt", "rb");
	if (file || GetGameEnv().IsAlpha())
	{
		m_lCircularBuffer		= 30;
		m_lCircularOutBuffer	= 10;

		if(file)
			fclose(file);

		return TRUE;
	}

	//m_csIniFile.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	m_csIniFile.SetPath(szFile);

	if (!m_csIniFile.ReadFile(0, FALSE))
		return FALSE;

	for (int nIndex = 0; nIndex < m_csIniFile.GetNumKeys(0); ++nIndex)
	{
		const CHAR	*szValueName	= m_csIniFile.GetKeyName(0, nIndex);
		CHAR		*szValue		= m_csIniFile.GetValue(0, nIndex);

		if (!szValueName || strlen(szValueName) <= 0 ||
			!szValue || strlen(szValue) <= 0)
			continue;

		INT32	lValue	= atoi(szValue);

		if (strcmp(szValueName, "CircularBuffer") == 0)
			m_lCircularBuffer	= lValue;
		else if (strcmp(szValueName, "CircularOutBuffer") == 0)
			m_lCircularOutBuffer	= lValue;

		else if (strcmp(szValueName, "FactorTypeResult") == 0)
			m_lFactorTypeResult	= lValue;
		else if (strcmp(szValueName, "FactorTypeCharStatus") == 0)
			m_lFactorTypeCharStatus	= lValue;
		else if (strcmp(szValueName, "FactorTypeCharType") == 0)
			m_lFactorTypeCharType	= lValue;
		else if (strcmp(szValueName, "FactorTypeCharPoint") == 0)
			m_lFactorTypeCharPoint	= lValue;
		else if (strcmp(szValueName, "FactorTypeCharMaxPoint") == 0)
			m_lFactorTypeCharPointMax	= lValue;
		else if (strcmp(szValueName, "FactorTypeDamage") == 0)
			m_lFactorTypeDamage	= lValue;
		else if (strcmp(szValueName, "FactorTypeDefense") == 0)
			m_lFactorTypeDefense	= lValue;
		else if (strcmp(szValueName, "FactorTypeAttack") == 0)
			m_lFactorTypeAttack	= lValue;
		else if (strcmp(szValueName, "FactorTypeItem") == 0)
			m_lFactorTypeItem	= lValue;
		else if (strcmp(szValueName, "FactorTypeDirt") == 0)
			m_lFactorTypeDirt	= lValue;
		else if (strcmp(szValueName, "FactorTypePrice") == 0)
			m_lFactorTypePrice	= lValue;
		else if (strcmp(szValueName, "FactorTypeOwner") == 0)
			m_lFactorTypeOwner	= lValue;
		else if (strcmp(szValueName, "FactorTypeAgro") == 0)
			m_lFactorTypeAgro	= lValue;
		else if (strcmp(szValueName, "AgpdFactor") == 0)
			m_lAgpdFactor	= lValue;

		else if (strcmp(szValueName, "Character") == 0)
			m_lCharacter	= lValue;
		else if (strcmp(szValueName, "Item") == 0)
			m_lItem			= lValue;
		else if (strcmp(szValueName, "Skill") == 0)
			m_lSkill		= lValue;

		else if (strcmp(szValueName, "GridItem") == 0)
			m_lGridItem		= lValue;
		else if (strcmp(szValueName, "CharGrid") == 0)
			m_lGrid		= lValue;

		else if (strcmp(szValueName, "PvPArray") == 0)
			m_lPvPArray		= lValue;
		else if (strcmp(szValueName, "PvPCharInfo") == 0)
			m_lPvPCharInfo		= lValue;
		else if (strcmp(szValueName, "PvPGuildInfo") == 0)
			m_lPvPGuildInfo		= lValue;

		else if (strcmp(szValueName, "MapIdPos") == 0)
			m_lMapIdPos		= lValue;

		else if (strcmp(szValueName, "OptimizedPacket") == 0)
			m_lOptimizedPacket		= lValue;
	}

	return TRUE;
}