/*==================================================================

	AgsModuleDBStream.cpp

===================================================================*/

#include "AgsModuleDBStream.h"

#define APMODULE_DBSTREAM_SECTION_NAME		"SECTION"

/************************************************************/
/*		The Implementation of AgsModuleDBStream class		*/
/************************************************************/
//
AgsModuleDBStream::AgsModuleDBStream()
	{
	m_nSectionColumnIndex = -1;
	m_pDatabase		= NULL;
	m_pStatement	= NULL;
	m_pRowset		= NULL;
	}

AgsModuleDBStream::~AgsModuleDBStream()
	{
	_Close();
	m_csManager.Terminate();
	}

void AgsModuleDBStream::_Close()
	{
	if (NULL != m_pRowset)
		{
		m_pRowset->Close();
		m_pRowset->Release();
		}

	if (NULL != m_pStatement)
		{
		m_pStatement->Close();
		delete m_pStatement;
		}

	if (NULL != m_pDatabase)
		{
		m_pDatabase->Close();
		delete m_pDatabase;
		}
	}


//
//=============================================================
//
BOOL AgsModuleDBStream::Initialize(eAUDB_VENDER eVender)
	{
	// initialize DB manager
	BOOL bResult = FALSE;
	if (AUDB_VENDER_ORACLE == eVender)
	  #ifdef ORACLE_THROUGH_OCI
		bResult = m_csManager.Initialize(AUDB_API_OCI, eVender);
	  #else
		bResult = m_csManager.Initialize(AUDB_API_ODBC, eVender);
	  #endif
	else
		bResult = m_csManager.Initialize(AUDB_API_ODBC, eVender);
		
	return bResult;
	}




//		Open
//=============================================================
//
//		Connect to database and read table
//
BOOL AgsModuleDBStream::Open(CHAR *pszDatabase, CHAR *pszUser, CHAR *pszPass, CHAR *pszTable, INT32 lPartIndex)
	{
	CHAR	szQuery[1024];
	if (NULL == pszTable)
		return FALSE;

	// connect to database
	m_pDatabase = m_csManager.CreateDatabase();
	if (NULL == m_pDatabase)
		return FALSE;

	if (!m_pDatabase->Open(pszDatabase, pszUser, pszPass))
		return FALSE;
		
	// execute statement
	m_pStatement = m_csManager.CreateStatement(m_pDatabase);
	if (NULL == m_pStatement || !m_pStatement->Open())
		return FALSE;

	_stprintf(szQuery, "select * from %s", pszTable);
	if (!m_pStatement->Execute(szQuery))
		return FALSE;
	if (!m_pStatement->HasResult())
		return FALSE;

	// read rowset
	m_pRowset = m_csManager.CreateRowset(m_pStatement);
	if (NULL == m_pRowset || !m_pRowset->Bind(TRUE))
		return FALSE;

	while (!m_pRowset->IsEnd())
		{
		if (!m_pRowset->Fetch())
			{
			return FALSE;
			}
		}
	
	// set seciton column index
	for (INT16 i = 0; i < (INT16)m_pRowset->GetColCount(); i++)
		{
		if (0 == strnicmp(APMODULE_DBSTREAM_SECTION_NAME, m_pRowset->GetColumnInfo(i).m_szName, strlen(APMODULE_DBSTREAM_SECTION_NAME)))
			{
			m_nSectionColumnIndex = i;
			break;
			}
		}

	return TRUE;
	}

//		Read Method
//======================================================================
//
BOOL AgsModuleDBStream::ReadNextValue()
	{
	INT32 nNumCols = m_pRowset->GetColCount();

	if (m_lValueID >= nNumCols-1)
		return FALSE;

	m_szValueName = m_pRowset->GetColumnInfo(m_lValueID+1).m_szName;
	if (0 == strnicmp(m_szValueName, m_szModuleName, strlen(m_szModuleName)) ||
		0 == strnicmp(m_szValueName, m_szEnumEnd, strlen(m_szEnumEnd)))
		return FALSE;

	++m_lValueID;

	return TRUE;
	}

BOOL AgsModuleDBStream::ReadPrevValue()
	{
	if (m_lValueID <= 0)
		return FALSE;

	m_szValueName = m_pRowset->GetColumnInfo(m_lValueID-1).m_szName;

	if (0 == strnicmp(m_szValueName, m_szModuleName, strlen(m_szModuleName)) ||
		0 == strnicmp(m_szValueName, m_szEnumEnd, strlen(m_szEnumEnd)))
		return FALSE;

	--m_lValueID;

	return TRUE;
	}

const CHAR* AgsModuleDBStream::ReadSectionName(INT32 lSectionID)
	{
	if (lSectionID >= (INT32)m_pRowset->GetRowCount())
		return NULL;

	if (-1 == m_nSectionColumnIndex)
		{
		itoa(m_lSectionID, m_szSection, 10);
		}
	else
		{
		ZeroMemory(m_szSection, sizeof(CHAR) * 64);
		strncpy(m_szSection, (CHAR *)m_pRowset->Get(lSectionID, m_nSectionColumnIndex), 63);
		m_szSection[63]	= 0;
		}
	m_lSectionID = lSectionID;
	return m_szSection;
	}

const CHAR * AgsModuleDBStream::GetValueName()
	{
	return m_szValueName;
	}

const CHAR * AgsModuleDBStream::GetValue()
	{
	// section id to rowid, value id to column name
	// search r, c and return Value(r, c)
	if (NULL == m_pRowset)
		return NULL;
	
	ASSERT(m_lSectionID < (INT32)m_pRowset->GetRowCount());
	ASSERT(m_lValueID < (INT32)m_pRowset->GetColCount());
	 
	return (const CHAR *) m_pRowset->Get(m_lSectionID, m_lValueID);
	}

BOOL AgsModuleDBStream::GetValue(CHAR *szValue, INT16 nLength)
	{
	m_szValue = (CHAR *) GetValue();
	ZeroMemory(szValue, sizeof(CHAR) * nLength);
	strncpy(szValue, m_szValue, nLength - 1);

	return TRUE;
	}

BOOL AgsModuleDBStream::GetValue(INT32 *plValue)
	{
	CHAR szTemp[16];

	GetValue(szTemp, 16);
	*plValue = atoi(szTemp);;

	return TRUE;
	}

BOOL AgsModuleDBStream::GetValue(INT64 *plValue)
	{
	CHAR szTemp[32];

	GetValue(szTemp, 32);
	*plValue = _atoi64(szTemp);;

	return TRUE;
	}

BOOL AgsModuleDBStream::GetValue(FLOAT *pfValue)
	{
	CHAR szTemp[16];			// too small ???

	GetValue(szTemp, 16);
	*pfValue = (FLOAT) atof(szTemp);

	return TRUE;
	}

BOOL AgsModuleDBStream::GetValue(AuPOS *pstPos)
	{
	CHAR	szTemp[64];

	GetValue(szTemp, 64);
	sscanf(szTemp, "%f,%f,%f", &pstPos->x, &pstPos->y, &pstPos->z);

	return TRUE;
	}


//		Write Method
//==============================================================
//
BOOL AgsModuleDBStream::Write(const CHAR *szTable, INT32 lPartIndex)
	{
	ASSERT(!"쓰지마란말이얏!!!");
	// set table

	// make query

	// execute

	return FALSE;
	}

BOOL AgsModuleDBStream::SetSection(CHAR *pszSection)
	{
//	INT32	lIndex;

	ZeroMemory(m_szSection, sizeof(CHAR) * 64);
	strncpy(m_szSection, pszSection, 63);

	m_lValueID = -1;

	// section컬럼이 있는지 확인.
	// 없으면 ... -_-;

	return FALSE;
	}


BOOL AgsModuleDBStream::WriteValue(CHAR *szValueName, CHAR *szValue)
	{
	// make query
	// UPDATE %s SET %s = '%s' WHERE ID = '%s' , m_szTable, szValueName, szValue, m_szSectionID

	// execute
	//
	return FALSE;
	}

BOOL AgsModuleDBStream::WriteValue(CHAR *szValueName, INT32 lValue)
	{
	CHAR szTemp[16];

	itoa(lValue, szTemp, 10);
	return WriteValue(szValueName, szTemp);
	}

BOOL AgsModuleDBStream::WriteValue(CHAR *szValueName, INT64 lValue)
	{
	CHAR szTemp[32];

	_i64toa(lValue, szTemp, 10);
	return WriteValue(szValueName, szTemp);
	}

BOOL AgsModuleDBStream::WriteValue(CHAR *szValueName, FLOAT fValue)
	{
	CHAR szTemp[64];
		
	//_gcvt(fValue, 20, szTemp);
	sprintf(szTemp, "%f", fValue);
	return WriteValue(szValueName, szTemp);
	}

BOOL AgsModuleDBStream::WriteValue(CHAR *szValueName, AuPOS *pstPos)
	{
	CHAR	szTemp[64];

	sprintf(szTemp, "%f,%f,%f", pstPos->x, pstPos->y, pstPos->z);
	return WriteValue(szValueName, szTemp);
	}



//		Enum Callback Method
//===========================================================================
//
BOOL AgsModuleDBStream::EnumReadCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule)
	{
	INT32		nIndex;
	CHAR *		pszName;
	CHAR *		pszValue;

	if (NULL == m_pRowset)
		return TRUE;		// return FALSE ?

	// 모든 컬럼에서 모듈의 시작 포인트(컬럼명이 ModuleData*) 를 찾는다. 
	for (nIndex = 0; nIndex < (INT32)m_pRowset->GetColCount(); ++nIndex)
		{
		pszName = m_pRowset->GetColumnInfo(nIndex).m_szName;

		// if "ModuleData*"
		if (0 == strnicmp(pszName, m_szModuleName, strlen(m_szModuleName)))
			{
			// get module name
			pszValue = (CHAR *) m_pRowset->Get(m_lSectionID, nIndex);

			StreamData*		cur_data = pcsModule->m_listStream[nDataIndex];
			while (cur_data)
				{
				// 콜백리스트에 해당 모듈이 있으면 call
				if (0 == strcmp(pszValue, cur_data->pModule->GetModuleName()))
					{
					MEMORYSTATUS	stMemory;
					SIZE_T			lAlloc;

					GlobalMemoryStatus(&stMemory);
					lAlloc = stMemory.dwAvailVirtual;

					m_lValueID = nIndex;

					if (cur_data->pReadFunc && !cur_data->pReadFunc(pData, cur_data->pModule, this))
						{
#ifdef _DEBUG
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsModuleDBStream::EnumReadCallback '%s' Module Callback fail\n" , pszValue);
						AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
#endif
						return FALSE;
						}

					GlobalMemoryStatus(&stMemory);
					if (lAlloc - stMemory.dwAvailVirtual > 10000)
						TRACE("AgsModuleDBStream::EnumReadCallback() - %d Byte Allocated (%s - %s) !!!!\n", lAlloc - stMemory.dwAvailVirtual, cur_data->pModule->GetModuleName(), pszName);

					break;
					}

				cur_data = cur_data->next;
				}
			}
		else if (0 == strnicmp(pszName, m_szEnumEnd, strlen(m_szEnumEnd)))
			{
			m_lValueID = nIndex;
			return TRUE;
			}
		}

	return TRUE;
	}

BOOL AgsModuleDBStream::EnumWriteCallback(INT16 nDataIndex, PVOID pData, ApModule *pcsModule)
	{
	CHAR	szName[64];

	StreamData*		cur_data = pcsModule->m_listStream[nDataIndex];
	while (cur_data)
		{
		// make column name
		INT32	lLength	= sprintf(szName, "%s%d", m_szModuleName, ++m_lModuleDataIndex);
		ASSERT(lLength < 64);

		if (!WriteValue(szName, cur_data->pModule->GetModuleName()))
			{
			TRACE( "AgsModuleDBStream::EnumWriteCallback '%s' Module WriteValue Fail\n" , cur_data->pModule->GetModuleName() );
			return FALSE;
			}

		if (cur_data->pWriteFunc && !cur_data->pWriteFunc(pData, cur_data->pModule, this))
			{
			TRACE( "AgsModuleDBStream::EnumWriteCallback '%s' Module Callback fail\n" , cur_data->pModule->GetModuleName() );
			return FALSE;
			}

		cur_data = cur_data->next;
		}

	if (!WriteValue((char *) m_szEnumEnd, 0))
		return FALSE;

	return TRUE;
	}



//		Info
//===========================================================================
//
INT32 AgsModuleDBStream::GetNumSections()
	{
	return m_pRowset ? m_pRowset->GetRowCount() : 0;
	}

BOOL AgsModuleDBStream::SetMode(ApModuleStreamMode eMode)
	{
	return FALSE;
	}

BOOL AgsModuleDBStream::SetType(ApModuleStreamType eType)
	{
	return FALSE;
	}




//	Custom
//===========================================================================
//
BOOL AgsModuleDBStream::OpenCustom(CHAR *pszDSN, CHAR *pszUser, CHAR *pszPassword)
	{
	// connect to database
	m_pDatabase = m_csManager.CreateDatabase();
	if (NULL == m_pDatabase)
		return FALSE;
	
	if (!m_pDatabase->Open(pszDSN, pszUser, pszPassword))
		return FALSE;
		
	// execute statement
	m_pStatement = m_csManager.CreateStatement(m_pDatabase);
	if (NULL == m_pStatement || !m_pStatement->Open())
		return FALSE;

	// read rowset
	m_pRowset = m_csManager.CreateRowset(NULL);
	if (NULL == m_pRowset)
		return FALSE;

	return TRUE;
	}


BOOL AgsModuleDBStream::QueryCustom(CHAR *pszQuery)
	{
	// execute statement
	if (NULL == m_pStatement)
		return FALSE;
	if (!m_pStatement->Execute(pszQuery))
		return FALSE;
	if (!m_pStatement->HasResult())
		return FALSE;

	// read rowset
	m_pRowset->Set(*m_pStatement);
	if (!m_pRowset->Bind(TRUE))
		return FALSE;

	while (!m_pRowset->IsEnd())
		{
		if (!m_pRowset->Fetch())
			{
			return FALSE;
			}
		}
	return TRUE;
	}


CHAR* AgsModuleDBStream::GetCustomValue(UINT32 r, UINT32 c)
	{
	if (m_pRowset)
		return (CHAR *) m_pRowset->Get(r, c);
	
	return NULL;
	}