/*=========================================================

	AgsdDatabase.cpp

=========================================================*/

#include "AgsdDatabase.h"

/********************************************************/
/*		The Implementation of Query Parameter class		*/
/********************************************************/
//
//	==========		Base		==========
//
AgsdDBParam::AgsdDBParam()
	{
	m_nParam = AGSMDATABASE_PARAM_CUSTOM;
	m_eOperation = AGSMDATABASE_OPERATION_UNKNOWN;
	m_ulNID = 0;
	m_nForcedPoolIndex = 0;
	}


AgsdDBParam::~AgsdDBParam()
	{
	}


void AgsdDBParam::Release()
	{
	}


BOOL AgsdDBParam::SetParam(AuStatement* pStatement)
	{
	BOOL bResult = TRUE;

	switch (m_eOperation)
		{
		case AGSMDATABASE_OPERATION_UPDATE :
			bResult = SetParamUpdate(pStatement);
			break;

		case AGSMDATABASE_OPERATION_INSERT :
			bResult = SetParamInsert(pStatement);
			break;

		case AGSMDATABASE_OPERATION_DELETE :
			bResult = SetParamDelete(pStatement);
			break;

		case AGSMDATABASE_OPERATION_SELECT :
			bResult = SetParamSelect(pStatement);
			break;

		case AGSMDATABASE_OPERATION_EXECUTE :
			bResult = SetParamExecute(pStatement);

		default :
			break;
		}
	
	return bResult;
	}


BOOL AgsdDBParam::SetParamExecute(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdDBParam::SetParamUpdate(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdDBParam::SetParamInsert(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdDBParam::SetParamDelete(AuStatement* pStatement)
	{
	return TRUE;
	}


BOOL AgsdDBParam::SetParamSelect(AuStatement* pStatement)
	{
	return TRUE;
	}


void AgsdDBParam::Dump(CHAR *pszOp)
	{
	}


void AgsdDBParam::DumpParam()
	{
	switch (m_eOperation)
		{
		case AGSMDATABASE_OPERATION_UPDATE :
			Dump(_T("UPD"));
			break;

		case AGSMDATABASE_OPERATION_INSERT :
			Dump(_T("INS"));
			break;

		case AGSMDATABASE_OPERATION_DELETE :
			Dump(_T("DEL"));
			break;

		case AGSMDATABASE_OPERATION_SELECT :
			Dump(_T("SEL"));
			break;

		case AGSMDATABASE_OPERATION_EXECUTE :
			Dump(_T("EXE"));

		default :
			break;
		}		
	}


void AgsdDBParam::SetStaticQuery(CHAR *pszDest, CHAR *pszFmt)
	{
	}




//
//	==========		Custom		==========
//
BOOL AgsdDBParamCustom::SetParamExecute(AuStatement* pStatement)
	{
	return SetParamCustom(pStatement);
	}

BOOL AgsdDBParamCustom::SetParamUpdate(AuStatement* pStatement)
	{
	return SetParamCustom(pStatement);
	}

BOOL AgsdDBParamCustom::SetParamInsert(AuStatement* pStatement)
	{
	return SetParamCustom(pStatement);
	}

BOOL AgsdDBParamCustom::SetParamDelete(AuStatement* pStatement)
	{
	return SetParamCustom(pStatement);
	}

BOOL AgsdDBParamCustom::SetParamSelect(AuStatement* pStatement)
	{
	return SetParamCustom(pStatement);
	}

BOOL AgsdDBParamCustom::SetParamCustom(AuStatement* pStatement)
	{
	// not support more than 1 row
	ASSERT(m_ulRows < 2);

	if (0 == m_ulRows)	// no binding parameters
		return TRUE;

	for (INT32 i=0; i < (INT32)m_ulCols; i++)
		{
		pStatement->SetParam(i, Get(0, i));
		}

	return TRUE;
	}

CHAR* AgsdDBParamCustom::Get(UINT32 ulRow, UINT32 ulCol)
	{
	ASSERT(m_ulRows > ulRow);
	ASSERT(m_ulCols > ulCol);

	return (CHAR *)m_pBuffer + m_lOffsets[ulCol] + (ulRow * m_ulRowBufferSize);
	}



/************************************************/
/*		The Implementation of Query class		*/
/************************************************/
//
//	==========		Base		==========
//
AgsdQuery::AgsdQuery()
	:m_pfCallback(NULL), m_pClass(NULL), m_pData(NULL), m_nIndex(0), m_pfCallbackFail(NULL)
	{
	}

AgsdQuery::~AgsdQuery()
	{
	}

void AgsdQuery::Release()
	{
	}

INT16 AgsdQuery::GetIndex()
	{
	return m_nIndex;
	}

BOOL AgsdQuery::BindParam(AuStatement *pStatement)
	{
	return TRUE;
	}

void AgsdQuery::Dump()
	{
	}

//
//	==========		Parameterized		==========
//
AgsdQueryWithParam::AgsdQueryWithParam()
	:m_pszQuery(NULL), m_pParam(NULL)
	{
	}

AgsdQueryWithParam::~AgsdQueryWithParam()
	{
	}	

void AgsdQueryWithParam::Release()
	{
	delete this;
	}

CHAR* AgsdQueryWithParam::Get()
	{
	return m_pszQuery;
	}

void AgsdQueryWithParam::Set(CHAR* pszQuery)
	{
	m_pszQuery = pszQuery;
	}

BOOL AgsdQueryWithParam::BindParam(AuStatement *pStatement)
	{
	if (m_pParam)
		return m_pParam->SetParam(pStatement);

	return TRUE;
	}

void AgsdQueryWithParam::Dump()
	{
	if (m_pParam)
		{
		m_pParam->DumpParam();
		}
	}

//
//	==========		Static		==========
//
AgsdQueryStatic::~AgsdQueryStatic()
	{
	Init();
	}

AgsdQueryStatic::AgsdQueryStatic()
	{
	Init();
	}

void AgsdQueryStatic::Release()
	{
	delete this;
	}

CHAR* AgsdQueryStatic::Get()
	{
	return m_szQuery;
	}

void AgsdQueryStatic::Set(CHAR* pszQuery)
	{
	strncpy(m_szQuery, pszQuery, _MAX_QUERY_LENGTH - 1);
	}


#define _LOG_FILENAME "db_static.txt"
void AgsdQueryStatic::Dump()
	{
	//char strCharBuff[1024] = { 0, };
	//sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsdQueryStatic\nQuery=[%s]\n", m_szQuery);
	//AuLogFile_s(_LOG_FILENAME, strCharBuff);
	}
#undef _LOG_FILENAME
