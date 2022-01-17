// ApModuleStream.cpp: implementation of the ApModuleStream class.
//
//////////////////////////////////////////////////////////////////////

#include "ApModuleStream.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApModuleStream::ApModuleStream()
{
	m_szModuleName = "ModuleData";
	m_szEnumEnd = "EnumEnd";

	//@{ 2006/12/08 burumal
	m_lModuleNameStrLen = (INT32) strlen(m_szModuleName);
	m_lEnumEndStrLen = (INT32) strlen(m_szEnumEnd);
	//@}

	m_lSectionID	= 0;
	m_lValueID		= -1;
	m_lModuleDataIndex	= 0;

	memset(m_szSection, 0, sizeof(m_szSection));
	m_szValueName	= NULL;
	m_szValue		= NULL;
}

ApModuleStream::~ApModuleStream()
{

}

BOOL ApModuleStream::Open(const CHAR *szFile, INT32 lPartIndex, BOOL bDecryption)
{
	m_csIniFile.SetPath(szFile);

	if (!m_csIniFile.ReadFile(lPartIndex, bDecryption))
		return FALSE;

	//m_csIniFile.SetPath(szFile);
	//m_csIniFile.ReadFile();

	return TRUE;
}

BOOL ApModuleStream::Parse(char* pBuffer, unsigned long nBufferSize, BOOL bDecryption)
{
	return m_csIniFile.ParseMemory( pBuffer, nBufferSize, bDecryption );
}

INT32 ApModuleStream::GetNumSections()
{
	return m_csIniFile.GetNumSection();
}

const CHAR * ApModuleStream::ReadSectionName(INT32 lSectionID)
{
	if (lSectionID >= GetNumSections())
		return NULL;

	m_lSectionID = lSectionID;

	//@{ 2006/07/27 burumal
	/*
	ZeroMemory(m_szSection, sizeof(CHAR) * 64);
	strncpy(m_szSection, m_csIniFile.GetSectionName(lSectionID), 63);
	*/
	memcpy(m_szSection, m_csIniFile.GetSectionName(lSectionID), 63);
	//@}

	// 마고자
	// 섹션을 새로 읽었으니 Value 인덱스를 초기화 한다.
	m_lValueID = -1;

	return m_szSection;
}

BOOL ApModuleStream::EnumReadCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule)
{
	INT32		nIndex;
//	INT32		nIndex2;
	const CHAR *	szValueName;
	CHAR *		szValue;
	//std::string	szValueName;
	//std::string	szValue;

	INT32		nTotalCount = m_csIniFile.GetNumKeys(m_lSectionID);

	//@{ 2006/12/08 burumal
	/*
	size_t		nModuleNameLen = strlen(m_szModuleName);
	size_t		nEnumEndLen = strlen(m_szEnumEnd);
	*/
	size_t		nModuleNameLen = m_lModuleNameStrLen;
	size_t		nEnumEndLen = m_lEnumEndStrLen;
	//@}

	//for (nIndex = 0; nIndex < m_csIniFile.GetNumKeys(m_lSectionID); ++nIndex)

	// 마고자
	// 최초에 읽을때는 ( 섹션을 읽은 직후 ) 무조건 m_lValueID 가 -1 이므로 0부터 시작하게 된다.
	// 반복해서 읽을 경우를 대비한 코드
	for ( nIndex = m_lValueID + 1; nIndex < nTotalCount; ++nIndex )
	{
		szValueName = m_csIniFile.GetKeyName(m_lSectionID, nIndex);

		//if (!strncmp(szValueName, m_szModuleName, strlen(m_szModuleName)))
		if ( !strncmp(szValueName, m_szModuleName, nModuleNameLen) )
		{
			szValue = m_csIniFile.GetValue(m_lSectionID, nIndex);
				
//			for (nIndex2 = 0; nIndex2 < pcsModule->m_nStream[nDataIndex]; ++nIndex2)
//			{
//				if (!strcmp(szValue, pcsModule->m_pvStreamCallbackClass[nDataIndex][nIndex2]->GetModuleName()))
//				{
//					m_lValueID = nIndex;
//
//					if (pcsModule->m_pfStreamReader[nDataIndex][nIndex2] && 
//						!pcsModule->m_pfStreamReader[nDataIndex][nIndex2](pData, pcsModule->m_pvStreamCallbackClass[nDataIndex][nIndex2], this))
//						return FALSE;
//
//					//nIndex = m_lValueID;		// 주석 by gemani(030512)
//
//					break;
//				}
//			}

			StreamData*		cur_data = pcsModule->m_listStream[nDataIndex];
			while(cur_data)
			{
				if (!strcmp(szValue, cur_data->pModule->GetModuleName()))
				{
					//@{ burumal 2006/02/01	현재 메모리 상태정보를 굳이 체크할 필요가 없다고 판단, 주석 처리해둠.
					/*
					MEMORYSTATUS	stMemory;
					SIZE_T			lAlloc;

					GlobalMemoryStatus(&stMemory);
					lAlloc = stMemory.dwAvailVirtual;
					*/
					//@}

					m_lValueID = nIndex;

					if (cur_data->pReadFunc && 
						!cur_data->pReadFunc(pData, cur_data->pModule, this))
					{
#ifdef _DEBUG
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "ApModuleStream::EnumReadCallback '%s' Module Callback fail\n" , szValue);
						AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
#endif
						return FALSE;
					}

					//@{ burumal 2006/02/01	현재 메모리 상태정보를 굳이 체크할 필요가 없다고 판단, 주석 처리해둠.
					/*
					GlobalMemoryStatus(&stMemory);
					if (lAlloc - stMemory.dwAvailVirtual > 10000)
						TRACE("ApModuleStream::EnumReadCallback() - %d Byte Allocated (%s - %s) !!!!\n", lAlloc - stMemory.dwAvailVirtual, cur_data->pModule->GetModuleName(), m_csIniFile.GetSectionName(m_lSectionID));
					*/
					//@}

					break;
				}

				cur_data = cur_data->next;
			}

		}
		//else if (!strncmp(szValueName, m_szEnumEnd, strlen(m_szEnumEnd)))
		else
		if ( !strncmp(szValueName, m_szEnumEnd, nEnumEndLen) )
		{
			m_lValueID = nIndex;
			return TRUE;
		}	
	}

	return TRUE;
}

BOOL ApModuleStream::EnumWriteCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule)
{
	CHAR		szValueName[64];

/*	for (INT32 nIndex = 0; nIndex < pcsModule->m_nStream[nDataIndex]; ++nIndex)
	{
		sprintf(szValueName, "%s%d", m_szModuleName, ++m_lModuleDataIndex);

		if (!WriteValue(szValueName, pcsModule->m_pvStreamCallbackClass[nDataIndex][nIndex]->GetModuleName()))
			return FALSE;

		if (pcsModule->m_pfStreamWriter[nDataIndex][nIndex] && 
			!pcsModule->m_pfStreamWriter[nDataIndex][nIndex](pData, pcsModule->m_pvStreamCallbackClass[nDataIndex][nIndex], this))
			return FALSE;
	}*/

	StreamData*		cur_data = pcsModule->m_listStream[nDataIndex];
	while(cur_data)
	{
		INT32	lLength	= sprintf(szValueName, "%s%d", m_szModuleName, ++m_lModuleDataIndex);
		ASSERT(lLength < 64);

		if (!WriteValue(szValueName, cur_data->pModule->GetModuleName() ) )
		{
			TRACE( "ApModuleStream::EnumWriteCallback '%s' Module WriteValue Fail\n" , cur_data->pModule->GetModuleName() );
			return FALSE;
		}

		if (cur_data->pWriteFunc && 
			!cur_data->pWriteFunc(pData, cur_data->pModule, this))
		{
			TRACE( "ApModuleStream::EnumWriteCallback '%s' Module Callback fail\n" , cur_data->pModule->GetModuleName() );
			return FALSE;
		}

		cur_data = cur_data->next;
	}

	if (!WriteValue((char *) m_szEnumEnd, 0))
		return FALSE;

	return TRUE;
}

const CHAR * ApModuleStream::GetValueName()
{
	return m_szValueName;
}

const CHAR * ApModuleStream::GetValue()
{
	return m_csIniFile.GetValue(m_lSectionID, m_lValueID);
}

BOOL ApModuleStream::GetValue(CHAR *szValue, INT16 nLength)
{
	m_szValue = m_csIniFile.GetValue(m_lSectionID, m_lValueID);
	
	//@{ 2006/07/27 burumal
	/*
	ZeroMemory(szValue, sizeof(CHAR) * nLength);
	strncpy(szValue, m_szValue, nLength - 1);
	*/	
	memcpy(szValue, m_szValue, nLength - 1);
	//@}

	return TRUE;
}

BOOL ApModuleStream::GetValue(INT32 *plValue)
{
	CHAR szTemp[16];

	GetValue(szTemp, 16);

	*plValue = atoi(szTemp);;

	return TRUE;
}

BOOL ApModuleStream::GetValue(INT64 *pllValue)
{
	CHAR szTemp[32];

	GetValue(szTemp, 16);

	*pllValue = _atoi64(szTemp);;

	return TRUE;
}

BOOL ApModuleStream::GetValue(FLOAT *pfValue)
{
	CHAR szTemp[16];

	GetValue(szTemp, 16);

	*pfValue = (FLOAT) atof(szTemp);

	return TRUE;
}

BOOL ApModuleStream::GetValue(AuPOS *pstPos)
{
	CHAR	szTemp[64];

	GetValue(szTemp, 64);

	sscanf(szTemp, "%f,%f,%f", &pstPos->x, &pstPos->y, &pstPos->z);

	return TRUE;
}


BOOL ApModuleStream::ReadNextValue()
{
	INT16 nKeyNum = m_csIniFile.GetNumKeys(m_lSectionID);

	if (m_lValueID >= nKeyNum - 1)
		return FALSE;

	m_szValueName = m_csIniFile.GetKeyName(m_lSectionID, m_lValueID + 1);

	//@{ 2006/12/08 burumal
	/*
	if (!strncmp(m_szValueName, m_szModuleName, strlen(m_szModuleName)) ||
		!strncmp(m_szValueName, m_szEnumEnd, strlen(m_szEnumEnd)))
		return FALSE;
	*/
	if (!strncmp(m_szValueName, m_szModuleName, m_lModuleNameStrLen) ||
		!strncmp(m_szValueName, m_szEnumEnd, m_lEnumEndStrLen))
		return FALSE;
	//@}

	++m_lValueID;

	return TRUE;
}

BOOL ApModuleStream::ReadPrevValue()
{
	if (m_lValueID <= 0)
		return FALSE;

	m_szValueName = m_csIniFile.GetKeyName(m_lSectionID, m_lValueID - 1);

	//@{ 2006/12/08 burumal
	/*
	if( !strncmp(m_szValueName, m_szModuleName, strlen(m_szModuleName)) ||
		!strncmp(m_szValueName, m_szEnumEnd, strlen(m_szEnumEnd)) )
		return FALSE;
	*/
	if( !strncmp(m_szValueName, m_szModuleName, m_lModuleNameStrLen) ||
		!strncmp(m_szValueName, m_szEnumEnd, m_lEnumEndStrLen) )
		return FALSE;
	//@}

	--m_lValueID;

	return TRUE;
}

BOOL ApModuleStream::WriteValue(const CHAR *szValueName, const CHAR *szValue)
{
	return m_csIniFile.SetValue(m_szSection, szValueName, szValue);
}

BOOL ApModuleStream::WriteValue(const CHAR *szValueName, INT32 lValue)
{
	return m_csIniFile.SetValueI(m_szSection, szValueName, lValue);
}

BOOL ApModuleStream::WriteValue(const CHAR *szValueName, INT64 llValue)
{
	return m_csIniFile.SetValueI64(m_szSection, szValueName, llValue);
}

BOOL ApModuleStream::WriteValue(const CHAR *szValueName, FLOAT fValue)
{
	return m_csIniFile.SetValueF(m_szSection, szValueName, fValue);
}

BOOL ApModuleStream::WriteValue(const CHAR *szValueName, AuPOS *pstPos)
{
	CHAR	szTemp[64];

	INT32	lLength	= sprintf(szTemp, "%f,%f,%f", pstPos->x, pstPos->y, pstPos->z);
	ASSERT(lLength < 64);

	return m_csIniFile.SetValue(m_szSection, szValueName, szTemp);
}

BOOL ApModuleStream::SetSection(CHAR *szSection)
{
	INT32	lIndex;

	//@{ 2006/07/27 burumal
	/*
	ZeroMemory(m_szSection, sizeof(CHAR) * 64);
	strncpy(m_szSection, szSection, 63);
	*/	
	memcpy(m_szSection, szSection, 63);
	//@}

	m_lModuleDataIndex = 0;
	m_lValueID = -1;

	for (lIndex = 0; lIndex < m_csIniFile.GetNumSection(); ++lIndex)
	{
		if (!strcmp(m_csIniFile.GetSectionName(lIndex), m_szSection))
		{
			m_lSectionID = lIndex;
			return TRUE;
		}
	}

	if (lIndex == m_csIniFile.GetNumSection())
	{
		m_lSectionID = m_csIniFile.AddSection(m_szSection);
	}

	return TRUE;
}

CHAR* ApModuleStream::GetSectionName()
{
	return m_szSection;
}

BOOL ApModuleStream::Write(const CHAR *szFile, INT32 lPartIndex, BOOL bEncryption)
{
	m_csIniFile.SetPath(szFile);

	return m_csIniFile.WriteFile(lPartIndex, bEncryption);
}

BOOL ApModuleStream::SetMode(ApModuleStreamMode eMode)
{
	return m_csIniFile.SetMode((AuIniManagerMode) eMode);
}

BOOL ApModuleStream::SetType(DWORD eType)
{
	return m_csIniFile.SetType(eType);
}

