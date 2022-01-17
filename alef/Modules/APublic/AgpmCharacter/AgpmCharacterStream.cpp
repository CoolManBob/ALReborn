#include "AgpmCharacter.h"
#include "ApModuleStream.h"
#include "AuExcelBinaryLib.h"

//extern INT32					g_llLevelUpExp[AGPMCHAR_MAX_LEVEL + 1];		// 캐릭터가 level up 하는데 필요한 exp
extern	ApSafeArray<INT64, AGPMCHAR_MAX_LEVEL + 1>	g_llLevelUpExp;
extern	ApSafeArray<INT64, AGPMCHAR_MAX_LEVEL + 1>	g_llDownExpPerLevel;		//JK_레벨별 경험치 패널티

// 2005/02/24 마고자
static BOOL g_sbCrashSafe = FALSE;

//		StreamRead / StreamWrite
//	Functions
//		- Character Data를 Streaming 한다.
//			* 현재 구현되어 있지 않다. 앞으로두?. 나중에 DB 루 할꺼다(Server Module에서)
//			  
//	Arguments
//		- szFile		: File 이름
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::StreamRead(CHAR *szFile)
{
	return TRUE;
}

BOOL AgpmCharacter::StreamWrite(CHAR *szFile)
{
	return TRUE;
}

//		StreamReadTemplate / StreamWriteTemplate
//	Functions
//		- Character Template Data를 Streaming 한다.
//			* File 에서 ApModuleStream을 이용해서 Read/Write 한다.
//
//	Arguments
//		- szFile		: File 이름
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::StreamReadTemplate(CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();

	// 마고자 (2005-12-28 오전 11:26:04) : 템플릿 갯수관련 디버그..
	// ASSERT( nNumKeys <= m_csACharacterTemplate.GetCount() );

	int nAlloc = m_csACharacterTemplate.GetCount();

	if( nAlloc < nNumKeys )
	{
		if( m_csACharacterTemplate.GetObjectCount() == 0 )
		{
			// 마고자 (2005-12-28 오전 11:28:06) : 
			// 모델툴이 아닌경우는 , 런타임중에 케릭터 템플릿이 변경돼는 경우가 없으니
			// 이렇게 해주어도 문제가 돼지 않는다.
			SetMaxCharacterTemplate( nNumKeys );
			nAlloc = nNumKeys;
		}
		else
		{
			ASSERT( !"아이템 템플릿 갯수초과!" );
		}
	}

	if (!m_csACharacterTemplate.InitializeObject( sizeof(AgpdCharacterTemplate *), nAlloc ) )
	{
	 	return FALSE;
	}

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pcsAgpdCharacterTemplate = AddCharacterTemplate(lTID);
		if (!pcsAgpdCharacterTemplate)
		{
			TRACE( "AgpmCharacter::StreamReadTemplate 템플릿을 찾을 수 없습니다.\n" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmCharacter::StreamReadTemplate 템플릿을 찾을 수 없습니다.\n" );
			return FALSE;
		}

		if (lTID > m_lMaxTID)
			m_lMaxTID	= lTID;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, pcsAgpdCharacterTemplate, this))
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);

			TRACE( "AgpmCharacter::StreamReadTemplate Callback returns FALSE!.\n" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmCharacter::StreamReadTemplate Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	EnumCallback(AGPMCHAR_CB_ID_READ_ALL_CHARACTER_TEMPLATE, NULL, NULL);

	return TRUE;
}

BOOL AgpmCharacter::StreamWriteTemplate(CHAR *szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[AGPACHARACTERT_MAX_TEMPLATE_NAME + 1];
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	// 등록된 모든 Character Template에 대해서...
	for (pcsAgpdCharacterTemplate = GetTemplateSequence(&lIndex); pcsAgpdCharacterTemplate; pcsAgpdCharacterTemplate = GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

//		StreamReadStaticCharacter / StreamWriteStaticCharacter
//	Functions
//		- Static Character Data를 Streaming 한다.
//			* File 에서 ApModuleStream을 이용해서 Read/Write 한다.
//			  
//	Arguments
//		- szFile		: File 이름
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::StreamReadStaticCharacter(CHAR *szFile , BOOL bCrashSafe)
{
	if (!szFile)
		return FALSE;

	// 2005/02/24 마고자
	// 변수저장..
	g_sbCrashSafe = bCrashSafe;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	AgpdCharacter		*pcsAgpdCharacter = NULL;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; ++i)
	{
		CHAR* szSection = (CHAR*)csStream.ReadSectionName(i);

		pcsAgpdCharacter	= CreateCharacter();

		// Lock 해준다. 할필요는 없지만, 통일성을 위해서. 2006.05.04. steeple
		pcsAgpdCharacter->m_Mutex.WLock();
		
		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMCHAR_DATA_TYPE_STATIC, pcsAgpdCharacter, this))
		{
			TRACE( "AgpmCharacter::StreamReadStaticCharacter Callback returns FALSE!.\n" );
//			TRACEFILE(ALEF_ERROR_FILENAME, "AgpmCharacter::StreamReadStaticCharacter Callback returns FALSE!.\n" );
			pcsAgpdCharacter->m_Mutex.Release();
			DestroyCharacter(pcsAgpdCharacter);
			return FALSE;
		}

		// 마을별 NPC list 생성
		AddRegionNPC(pcsAgpdCharacter);

		pcsAgpdCharacter->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgpmCharacter::StreamReadOneStaticCharacter(CHAR *szFile , CHAR *szCharName, BOOL bCrashSafe)
{
	if (!szFile)
		return FALSE;

	// 2005/02/24 마고자
	// 변수저장..
	g_sbCrashSafe = bCrashSafe;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	AgpdCharacter		*pcsAgpdCharacter = NULL;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; ++i)
	{
		csStream.ReadSectionName(i);

		pcsAgpdCharacter	= CreateCharacter();

		// Lock 해준다. 할필요는 없지만, 통일성을 위해서. 2006.05.04. steeple
		pcsAgpdCharacter->m_Mutex.WLock();

		PVOID	pvBuffer[2];
		pvBuffer[0]	= (PVOID)	pcsAgpdCharacter;
		pvBuffer[1]	= (PVOID)	szCharName;

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMCHAR_DATA_TYPE_STATIC_ONE, pvBuffer, this))
		{
			pcsAgpdCharacter->m_Mutex.Release();
			DestroyCharacter(pcsAgpdCharacter);
			continue;
		}

		// 마을별 NPC list 생성
		AddRegionNPC(pcsAgpdCharacter);

		pcsAgpdCharacter->m_Mutex.Release();

		break;
	}

	return TRUE;
}

BOOL AgpmCharacter::AddRegionNPC(AgpdCharacter *pcsAgpdCharacter)
{
	INT16 nRegionIndex = m_pcsApmMap->GetRegion(pcsAgpdCharacter->m_stPos.x, pcsAgpdCharacter->m_stPos.z);
	ApmMap::RegionTemplate *pstRegionTemplate = m_pcsApmMap->GetTemplate(nRegionIndex);
	if (NULL != pstRegionTemplate && pstRegionTemplate->ti.stType.uFieldType == ApmMap::FT_TOWN)
	{
		map<INT16, vector<AgpdCharacter*> >::iterator itr = m_mapTownToNPCList.find(nRegionIndex);
		if (itr == m_mapTownToNPCList.end())
		{
			vector<AgpdCharacter*> tmpVector; tmpVector.push_back(pcsAgpdCharacter);
			m_mapTownToNPCList.insert(pair<INT16, vector<AgpdCharacter*> >(nRegionIndex, tmpVector));
		}
		else
		{
			vector<AgpdCharacter*> &tmpVector = itr->second;
			int n = (int)tmpVector.size();
			tmpVector.push_back(pcsAgpdCharacter);
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void AgpmCharacter::PrintRegionNPCList()
{
	map<INT16, vector<AgpdCharacter*> >::iterator itr = m_mapTownToNPCList.begin();
	while(m_mapTownToNPCList.end() != itr)
	{
		ApmMap::RegionTemplate *pstRegionTemplate = m_pcsApmMap->GetTemplate(itr->first);
		printf("[%s]\n", pstRegionTemplate->pStrName);
		for (int i = 0; i < (int)(itr->second.size()); i++)
		{
			printf("%s\n", itr->second[i]->m_szID);
		}

		itr++;
	}
}

BOOL AgpmCharacter::StreamWriteStaticCharacter(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szSectionName[32];
	AgpdCharacter		*pcsAgpdCharacter;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// 등록된 모든 Character Template에 대해서...
	for (pcsAgpdCharacter = GetCharSequence(&lIndex); pcsAgpdCharacter; pcsAgpdCharacter = GetCharSequence(&lIndex))
	{
//		if (!IsNPC(pcsAgpdCharacter))
//			continue;

		sprintf(szSectionName, "%d", lIndex);

		csStream.SetSection(szSectionName);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMCHAR_DATA_TYPE_STATIC, pcsAgpdCharacter, this))
			return FALSE;
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgpmCharacter::StaticCharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	=	static_cast< AgpdCharacter* >(pData);
	AgpmCharacter	*pThis				=	static_cast< AgpmCharacter* >(pClass);

	// Character Template의 값들을 Write한다.
	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_NAME, pcsAgpdCharacter->m_szID))
		return FALSE;

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_TID, pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID))
		return FALSE;

	CHAR				szBuffer[128];
	ZeroMemory(szBuffer, sizeof(CHAR) * 128);

	sprintf(szBuffer, "%f,%f,%f", pcsAgpdCharacter->m_stPos.x,
								  pcsAgpdCharacter->m_stPos.y,
								  pcsAgpdCharacter->m_stPos.z);

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_POSITION, szBuffer))
		return FALSE;

	ZeroMemory(szBuffer, sizeof(CHAR) * 128);
	sprintf(szBuffer, "%f,%f", pcsAgpdCharacter->m_fTurnX,
							   pcsAgpdCharacter->m_fTurnY);

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_DEGREE, szBuffer))
		return FALSE;


	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_MINIMAP, szBuffer))
		return FALSE;
	
	return TRUE;
}

BOOL AgpmCharacter::StaticCharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpdCharacter	*pcsAgpdCharacter	=	(AgpdCharacter *)	pData;
	AgpmCharacter	*pThis				=	(AgpmCharacter *)	pClass;

	const CHAR *szValueName;
	CHAR		szValue[ 128 ];


	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMCHAR_INI_NAME_NAME))
		{
			pStream->GetValue(pcsAgpdCharacter->m_szID, AGPDCHARACTER_MAX_ID_LENGTH);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_TID))
		{
			pStream->GetValue(szValue, 64);

			pcsAgpdCharacter->m_lTID1		= atoi(szValue);

			pcsAgpdCharacter->m_pcsCharacterTemplate	= pThis->GetCharacterTemplate(pcsAgpdCharacter->m_lTID1);

			if( g_sbCrashSafe && NULL == pcsAgpdCharacter->m_pcsCharacterTemplate )
			{
				pcsAgpdCharacter->m_pcsCharacterTemplate	= pThis->GetCharacterTemplate( 1 );
			}
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_POSITION))
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacter->m_stPos.x,
										&pcsAgpdCharacter->m_stPos.y,
										&pcsAgpdCharacter->m_stPos.z);

			// NPC의 RegionIndex 할당, 2005.06.28 by kelovon
			pcsAgpdCharacter->m_nBindingRegionIndex
				= pThis->m_pcsApmMap->GetRegion(pcsAgpdCharacter->m_stPos.x, pcsAgpdCharacter->m_stPos.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_DEGREE))
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f", &pcsAgpdCharacter->m_fTurnX,
									 &pcsAgpdCharacter->m_fTurnY);
		}

		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_MINIMAP))
		{
			pStream->GetValue(szValue, 64 );
			pcsAgpdCharacter->m_bNPCDisplayForMap	=	atoi(szValue);
		}

		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_NAMEBOARD))
		{
			pStream->GetValue(szValue, 64 );
			pcsAgpdCharacter->m_bNPCDisplayForNameBoard	=	atoi(szValue);
		}
	}

	// 이넘은 좀 특이한 구조다.... 여기서 일단 AddCharacter()를 해줘야 할듯하다.
	INT32	lNewCID	= AP_INVALID_CID;

	pThis->EnumCallback(AGPMCHAR_CB_ID_GET_NEW_CID, &lNewCID, NULL);

	if (lNewCID != AP_INVALID_CID)
		pcsAgpdCharacter->m_lID	= lNewCID;
	else
		pcsAgpdCharacter->m_lID	= pThis->m_lStaticCharacterIndex++;

	if (pThis->AddCharacter(pcsAgpdCharacter))
	{
		pcsAgpdCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_IN_GAME_WORLD;

		pcsAgpdCharacter->m_ulCharType = AGPMCHAR_TYPE_NPC;

		pThis->UpdateInit(pcsAgpdCharacter);

		pThis->UpdatePosition(pcsAgpdCharacter, NULL);

		pcsAgpdCharacter->m_bIsProtectedNPC	= TRUE;
		pcsAgpdCharacter->m_nNPCID			= atoi((CHAR*)pStream->GetSectionName());

		pThis->EnumCallback(AGPMCHAR_CB_ID_ADD_STATIC_CHARACTER, pcsAgpdCharacter , NULL );
	}
	else
	{
		ASSERT(!".\\Ini\\NPC.ini (Failed AddCharacter())");
	}

	return TRUE;
}



BOOL AgpmCharacter::StaticOneCharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	return TRUE;
}

BOOL AgpmCharacter::StaticOneCharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	PVOID			*ppvBuffer			= (PVOID *)			pData;

	AgpdCharacter	*pcsAgpdCharacter	= (AgpdCharacter *)	ppvBuffer[0];
	CHAR			*pszCharName		= (CHAR *)			ppvBuffer[1];

	AgpmCharacter	*pThis				= (AgpmCharacter *)	pClass;

	const CHAR *szValueName;
	CHAR szValue[128];

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPMCHAR_INI_NAME_NAME))
		{
			pStream->GetValue(pcsAgpdCharacter->m_szID, AGPDCHARACTER_MAX_ID_LENGTH);

			if (strcmp(pszCharName, pcsAgpdCharacter->m_szID) != 0)
				return FALSE;
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_TID))
		{
			pStream->GetValue(szValue, 64);

			pcsAgpdCharacter->m_lTID1		= atoi(szValue);

			pcsAgpdCharacter->m_pcsCharacterTemplate	= pThis->GetCharacterTemplate(pcsAgpdCharacter->m_lTID1);

			// 2005/02/24 마고자
			// 크래시 안전 코드..
			if( g_sbCrashSafe && NULL == pcsAgpdCharacter->m_pcsCharacterTemplate )
			{
				pcsAgpdCharacter->m_pcsCharacterTemplate	= pThis->GetCharacterTemplate( 1 );
			}
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_POSITION))
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacter->m_stPos.x,
				&pcsAgpdCharacter->m_stPos.y,
				&pcsAgpdCharacter->m_stPos.z);

			// NPC의 RegionIndex 할당, 2005.06.28 by kelovon
			pcsAgpdCharacter->m_nBindingRegionIndex
				= pThis->m_pcsApmMap->GetRegion(pcsAgpdCharacter->m_stPos.x, pcsAgpdCharacter->m_stPos.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_DEGREE))
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f", &pcsAgpdCharacter->m_fTurnX,
				&pcsAgpdCharacter->m_fTurnY);
		}

		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_MINIMAP))
		{
			pStream->GetValue(szValue, 64);
			pcsAgpdCharacter->m_bNPCDisplayForMap	=	atoi(szValue);
		}

		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_NAMEBOARD))
		{
			pStream->GetValue(szValue, 64 );
			pcsAgpdCharacter->m_bNPCDisplayForNameBoard	=	atoi(szValue);
		}

	}

	// 이넘은 좀 특이한 구조다.... 여기서 일단 AddCharacter()를 해줘야 할듯하다.
	INT32	lNewCID	= AP_INVALID_CID;

	pThis->EnumCallback(AGPMCHAR_CB_ID_GET_NEW_CID, &lNewCID, NULL);

	if (lNewCID != AP_INVALID_CID)
		pcsAgpdCharacter->m_lID	= lNewCID;
	else
		pcsAgpdCharacter->m_lID	= pThis->m_lStaticCharacterIndex++;

	if (pThis->AddCharacter(pcsAgpdCharacter))
	{
		pcsAgpdCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_IN_GAME_WORLD;

		pThis->UpdateInit(pcsAgpdCharacter);

		pThis->UpdatePosition(pcsAgpdCharacter, NULL);

		pcsAgpdCharacter->m_bIsProtectedNPC	= TRUE;

		pThis->EnumCallback(AGPMCHAR_CB_ID_ADD_STATIC_CHARACTER, pcsAgpdCharacter, NULL);
	}
	else
	{
		ASSERT(!".\\Ini\\NPC.ini (Failed AddCharacter())");
	}

	return TRUE;
}



//		CharacterReadCB / CharacterWriteCB
//	Functions
//		- Character Stream Callback Function이다.
//			* 실제 구현은 되어있지 않다.
//			  
//	Arguments
//		- pData		: Character Data
//		- pClass	: Callback 이니깐 Class 있어야 된다. (this 랑 같다. 등록할때만 잘 했다면)
//		- pStream	: 실제 Streaming 하기 위한 Class
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::CharacterReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{

	return TRUE;
}

BOOL AgpmCharacter::CharacterWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{

	return TRUE;
}

BOOL AgpmCharacter::StreamWriteCharKind(CHAR *szFile)
{
	if (!szFile || !szFile[0])
		return FALSE;

	ApModuleStream		csStream;
	INT32				lIndex = 0;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	CHAR	szTemp[2];
	ZeroMemory(szTemp, sizeof(CHAR) * 2);
	sprintf(szTemp, "%d", 1);

	csStream.SetSection(szTemp);

	// Stream Enumerate 한다.
	if (!csStream.EnumWriteCallback(AGPMCHAR_DATA_TYPE_CHAR_KIND, NULL, this))
		return FALSE;

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgpmCharacter::StreamReadCharKind(CHAR *szFile)
{
	if (!szFile || !szFile[0])
		return FALSE;

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
		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMCHAR_DATA_TYPE_CHAR_KIND, NULL, this))
			return FALSE;
	}

	return TRUE;
}

//		TemplateReadCB / TemplateWriteCB
//	Functions
//		- Character Template Stream Callback Function이다.
//			* 여기서 실제 Data 들을 읽고 쓴다.
//			  
//	Arguments
//		- pData		: Character Data
//		- pClass	: Callback 이니깐 Class 있어야 된다. (this 랑 같다. 등록할때만 잘 했다면)
//		- pStream	: 실제 Streaming 하기 위한 Class
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCharacter::TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR *szValueName;
	CHAR szValue[128];
//	UINT32 ulTemp1, ulTemp2;
	AgpmCharacter *pThis = (AgpmCharacter *) pClass;
	AgpdCharacterTemplate *pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;

	//@{ 2006/08/17 burumal
	pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth		= 0;
	pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight		= 0;
	pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius	= 50.0f; // default 값은 magoja씨의 요청임
	pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX		= 0;
	pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ		= 0;
	//@}

	//@{ 2006/09/08 burumal
	pcsAgpdCharacterTemplate->m_bSelfDestructionAttackType	= FALSE;
	//@}

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, AGPM_FACTORS_INI_START))
		{
			if (pThis->m_pcsAgpmFactors)
				pThis->m_pcsAgpmFactors->StreamRead(&pcsAgpdCharacterTemplate->m_csFactor, pStream);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_NAME))
		{
			pStream->GetValue(pcsAgpdCharacterTemplate->m_szTName, AGPACHARACTER_MAX_ID_STRING);

			pThis->m_csACharacterTemplate.AddTemplateStringKey(pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_BTYPE))
		{
			pStream->GetValue(szValue, 64);

			if (pcsAgpdCharacterTemplate->m_nBlockInfo >= AGPDCHARACTER_MAX_BLOCK_INFO)
				return FALSE;

			pcsAgpdCharacterTemplate->m_nBlockInfo += 1;

			if (!strcmp(szValue, AGPMCHAR_INI_NAME_BOX))
				pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_BOX;
			else if (!strcmp(szValue, AGPMCHAR_INI_NAME_SPHERE))
				pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_SPHERE;
			else if (!strcmp(szValue, AGPMCHAR_INI_NAME_CYLINDER))
				pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_CYLINDER;
			else
				pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_NONE;
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_BOX_INF) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.inf.x, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.inf.y, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.inf.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_BOX_SUP) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)
		{
			pStream->GetValue(szValue, 64);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.sup.x, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.sup.y, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.box.sup.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_SPHERE_CENTER) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(szValue, 128);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.sphere.center.x, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.sphere.center.y, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.sphere.center.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_SPHERE_RADIUS) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(&pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.sphere.radius);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_CYLINDER_CENTER) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(szValue, 128);

			sscanf(szValue, "%f,%f,%f", &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.cylinder.center.x, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.cylinder.center.y, &pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.cylinder.center.z);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_CYLINDER_HEIGHT) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.cylinder.height);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_CYLINDER_RADIUS) && pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsAgpdCharacterTemplate->m_astBlockInfo[pcsAgpdCharacterTemplate->m_nBlockInfo - 1].data.cylinder.radius);
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_CHAR_TYPE))
		{
			// 2007.02.09. steeple
			// ModelTool 데이터는 읽지 않는다.
			//
			//INT32	lCharType	= 0;
			//pStream->GetValue(&lCharType);

			//pcsAgpdCharacterTemplate->m_ulCharType	= (UINT32) lCharType;
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_FACE_NUM))
		{
			INT32	lFaceNum	= 0;
			pStream->GetValue(&lFaceNum);

			pcsAgpdCharacterTemplate->m_lFaceNum = (UINT32) lFaceNum;
		}
		else if (!strcmp(szValueName, AGPMCHAR_INI_NAME_HAIR_NUM))
		{
			INT32	lHairNum	= 0;
			pStream->GetValue(&lHairNum);

			pcsAgpdCharacterTemplate->m_lHairNum = (UINT32) lHairNum;
		}		
		//@{ 2006/09/08 burumal
		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_SELF_DESTRUCTION_ATTACK_TYPE) )
		{
			INT32 nSelfDest = FALSE;
			pStream->GetValue(&nSelfDest);
			pcsAgpdCharacterTemplate->m_bSelfDestructionAttackType = (nSelfDest > 0) ? TRUE : FALSE;
		}
		//@}
		//@{ 2006/08/17 burumal
		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_SWCO_BOX) )
		{
			pStream->GetValue(szValue, 128);

			FLOAT fTemp1, fTemp2;
			sscanf(szValue, "%f:%f", &fTemp1, &fTemp2);

			pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth = fTemp1;
			pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight = fTemp2;
		}
		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_SWCO_SPHERE) )
		{			
			FLOAT fValue;
			pStream->GetValue(&fValue);
			pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius = fValue;
		}
		//@}
		//@{ 2006/10/18 burumal
		else if ( !strcmp(szValueName, AGPMCHAR_INI_NAME_SWCO_OFFSET) )
		{
			pStream->GetValue(szValue, 128);

			FLOAT fTemp1, fTemp2;
			sscanf(szValue, "%f:%f", &fTemp1, &fTemp2);

			pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX = fTemp1;
			pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ = fTemp2;
		}
		//@}
	}

	return TRUE;
}

BOOL AgpmCharacter::TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	CHAR					szTemp[128];
	CHAR					szTemp2[128];
	AgpdCharacterTemplate *	pcsAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	INT16					nIndex;
	AgpmCharacter			*pThis = (AgpmCharacter *) pClass;

	// Character Template의 값들을 Write한다.
	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_NAME, pcsAgpdCharacterTemplate->m_szTName))
		return FALSE;

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_CHAR_TYPE, (INT32) pcsAgpdCharacterTemplate->m_ulCharType))
		return FALSE;

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_FACE_NUM, (INT32) pcsAgpdCharacterTemplate->m_lFaceNum ))
		return FALSE;

	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_HAIR_NUM, (INT32) pcsAgpdCharacterTemplate->m_lHairNum ))
		return FALSE;

	//@{ 2006/09/08 burumal
	if (!pStream->WriteValue(AGPMCHAR_INI_NAME_SELF_DESTRUCTION_ATTACK_TYPE, (INT32) pcsAgpdCharacterTemplate->m_bSelfDestructionAttackType ))
		return FALSE;
	//@}

	//@{ 2006/10/20 burumal			
	sprintf(szTemp, "%s", AGPMCHAR_INI_NAME_SWCO_BOX);
	print_compact_format(szTemp2, "%f:%f", pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth, pcsAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight);
	pStream->WriteValue(szTemp, szTemp2);

	sprintf(szTemp, "%s", AGPMCHAR_INI_NAME_SWCO_SPHERE);
	print_compact_format(szTemp2, "%f", pcsAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius);
	pStream->WriteValue(szTemp, szTemp2);
	
	sprintf(szTemp, "%s", AGPMCHAR_INI_NAME_SWCO_OFFSET);
	print_compact_format(szTemp2, "%f:%f", pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetX, pcsAgpdCharacterTemplate->m_fSiegeWarCollObjOffsetZ);
	pStream->WriteValue(szTemp, szTemp2);
	//@}

	// Blocking
	for (nIndex = 0; nIndex < pcsAgpdCharacterTemplate->m_nBlockInfo; ++nIndex)
	{
		switch (pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].type)
		{
		case AUBLOCKING_TYPE_BOX:
			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BTYPE, nIndex);
			if (!pStream->WriteValue(szTemp2, AGPMCHAR_INI_NAME_BOX))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BOX_INF, nIndex);
			sprintf(szTemp, "%f,%f,%f", pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.inf.x, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.inf.y, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.inf.z);
			if (!pStream->WriteValue(szTemp2, szTemp))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BOX_SUP, nIndex);
			sprintf(szTemp, "%f,%f,%f", pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.sup.x, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.sup.y, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.box.sup.z);
			if (!pStream->WriteValue(szTemp2, szTemp))
				return FALSE;

			break;
		case AUBLOCKING_TYPE_SPHERE:
			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BTYPE, nIndex);
			if (!pStream->WriteValue(AGPMCHAR_INI_NAME_BTYPE, AGPMCHAR_INI_NAME_SPHERE))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_SPHERE_CENTER, nIndex);
			sprintf(szTemp, "%f,%f,%f", pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.sphere.center.x, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.sphere.center.y, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.sphere.center.z);
			if (!pStream->WriteValue(szTemp2, szTemp))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_SPHERE_RADIUS, nIndex);
			if (!pStream->WriteValue(szTemp2, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.sphere.radius))
				return FALSE;

			break;
		case AUBLOCKING_TYPE_CYLINDER:
			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BTYPE, nIndex);
			if (!pStream->WriteValue(szTemp2, AGPMCHAR_INI_NAME_CYLINDER))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_CYLINDER_CENTER, nIndex);
			sprintf(szTemp, "%f,%f,%f", pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.cylinder.center.x, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.cylinder.center.y, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.cylinder.center.z);
			if (!pStream->WriteValue(szTemp2, szTemp))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_CYLINDER_HEIGHT, nIndex);
			if (!pStream->WriteValue(szTemp2, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.cylinder.height))
				return FALSE;

			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_CYLINDER_RADIUS, nIndex);
			if (!pStream->WriteValue(szTemp2, pcsAgpdCharacterTemplate->m_astBlockInfo[nIndex].data.cylinder.radius))
				return FALSE;

			break;
		default:
			sprintf(szTemp2, "%s%d", AGPMCHAR_INI_NAME_BTYPE, nIndex);
			if (!pStream->WriteValue(szTemp2, AGPMCHAR_INI_NAME_NONE))
				return FALSE;

			break;
		}
	}

	//. 2005.09.08 Nonstopdj
	//. 더이상 Charactor Factor Data를 Ini파일에 기록하지 않는다.
	//. 기존의 누적되어 있는 데이터(factorstart = 0 ~ factorend = 0 사이의 데이터들)도 처리됨. -_-V
	//.	2005.09.09 Nonstopdj
	//. attack range와 hitrange의 사용으로 일단 다시 출력한다 -_-
	VERIFY(pThis->m_pcsAgpmFactors->StreamWrite(&pcsAgpdCharacterTemplate->m_csFactor, pStream));

	return TRUE;
}

BOOL AgpmCharacter::StreamReadLevelUpExpTxt(CHAR *szFile)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();

	for (int i = 1; i < AGPMCHAR_MAX_LEVEL + 1; ++i)
	{
		CHAR		*szData			= csExcelTxtLib.GetData(1, i);
		if (!szData || !strlen(szData))
		{
			csExcelTxtLib.CloseFile();
			return FALSE;
		}

		g_llLevelUpExp[i] = (INT64) _atoi64(szData);
	}

	csExcelTxtLib.CloseFile();
	return TRUE;
}
//JK_레벨별 경험치패널티
BOOL AgpmCharacter::StreamReadDownExpPerLevelTxt(CHAR *szFile)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE))
		return FALSE;

	INT16			nRow			= csExcelTxtLib.GetRow();

	for (int i = 1; i < AGPMCHAR_MAX_LEVEL + 1; ++i)
	{
		CHAR		*szData			= csExcelTxtLib.GetData(1, i);
		if (!szData || !strlen(szData))
		{
			csExcelTxtLib.CloseFile();
			return FALSE;
		}

		g_llDownExpPerLevel[i] = (INT64) _atoi64(szData);
	}

	csExcelTxtLib.CloseFile();
	return TRUE;
}

BOOL AgpmCharacter::StreamReadCharGrowUpTxt(CHAR *szFile, BOOL bDecryption)
{
	if (!szFile || !strlen(szFile))
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if (!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		csExcelTxtLib.CloseFile();
		return FALSE;
	}
#ifdef _DEBUG
	printf_s("Start!! Reading GrowUp Factor...\n");
#endif
	INT16			nColumn			= csExcelTxtLib.GetColumn();
	INT16			nCurRow			= 0;
	CHAR			*szData			= csExcelTxtLib.GetData(0, nCurRow++);

	//첫라인에는 반드시 캐릭터ID가 와야 한다. 고정된 포맷이므로 항상 규격을 맞춰야함.

	while (szData)
	{
		AgpdCharacterTemplate *pcsTemplate = GetCharacterTemplate(atoi(szData));
		if (NULL == pcsTemplate)
		{
			printf( "Error!! Can't load Character Template by CharacterID.\n");
			csExcelTxtLib.CloseFile();
			return FALSE;
		}

		++nCurRow;

		for( int k = 1; k < AGPMCHAR_MAX_LEVEL; ++k )
		{
			for( int j = 1; j < nColumn; j++ )
			{
				if( NULL != csExcelTxtLib.GetData(j, 1) )
				{
					char* szCol = csExcelTxtLib.GetData(j, 1);
					char* szVal = csExcelTxtLib.GetData(j, nCurRow);

					if( !SetCharGrowUpFactor( &pcsTemplate->m_csLevelFactor[k], szCol, szVal ) )
						break;
					else
					{
						m_pcsAgpmFactors->SetValue(&pcsTemplate->m_csLevelFactor[k], (INT32) k, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
						m_pcsAgpmFactors->SetMaxExp(&pcsTemplate->m_csLevelFactor[k], g_llLevelUpExp[k]);
					}
				}
			}
			++nCurRow;
		}

		++nCurRow;

		szData = csExcelTxtLib.GetData(0, nCurRow++);
	}

#ifdef _DEBUG
	printf_s("Done!! Reading GrowUp Factor.\n");
#endif

	csExcelTxtLib.CloseFile();
	return TRUE;
}

BOOL AgpmCharacter::StreamReadImportData(CHAR *szFile, CHAR *szDebugString, BOOL bDecryption)
{
	if (!szFile || !szDebugString)
		return FALSE;

	using namespace AuExcel;

	AuExcelLib * pExcel = LoadExcelFile( szFile , bDecryption );
	AuAutoPtr< AuExcelLib >	ptrExcel = pExcel;

	if( pExcel == NULL)
	{
		TRACE("AgpmCharacter::StreamReadImportData() Error (1) !!!\n");
		return FALSE;
	}

	enum	IMPORT_VALUE
	{
		IV_CHAR_TYPE								=1,
		IV_CHAR_RACE								,
		IV_CHAR_GENDER								,
		IV_CHAR_CLASS								,
		IV_CHAR_LEVEL								,
		IV_CHAR_STR								,
		IV_CHAR_DEX								,
		IV_CHAR_INT								,
		IV_CHAR_WIS								,
		IV_CHAR_CON								,
		IV_CHAR_CHA								,
		IV_CHAR_AC									,
		IV_CHAR_HP_MAX								,
		IV_CHAR_SP_MAX								,
		IV_CHAR_MP_MAX								,
		IV_CHAR_MOVEMENT_WALK						,
		IV_CHAR_MOVEMENT_RUN						,
		IV_CHAR_MIN_DMG							,
		IV_CHAR_MAX_DMG							,
		IV_CHAR_ATTACK_RANGE						,
		IV_CHAR_DEFENSE_RATE						,
		IV_CHAR_ATK_SPEED							,
		//IV_CHAR_ATK_POINT						,
		//IV_CHAR_MAGIC_ATK_POINT					,
		IV_CHAR_AR									,
		IV_CHAR_DR									,
		IV_CHAR_MAR								,
		IV_CHAR_MDR								,
		IV_CHAR_EXP								,
		IV_CHAR_MAGIC_INTENSITY					,
		IV_CHAR_LAND_ATTACH_TYPE					,
		/*
		IV_CHAR_FIRE_DMG							,
		IV_CHAR_WATER_DMG							,
		IV_CHAR_AIR_DMG							,
		IV_CHAR_EARTH_DMG							,
		IV_CHAR_MAGIC_DMG							,
		*/
		IV_CHAR_FIRE_MIN_DMG						,
		IV_CHAR_WATER_MIN_DMG						,
		IV_CHAR_AIR_MIN_DMG						,
		IV_CHAR_EARTH_MIN_DMG						,
		IV_CHAR_MAGIC_MIN_DMG						,
		IV_CHAR_POISON_MIN_DMG						,
		IV_CHAR_ICE_MIN_DMG						,
		IV_CHAR_THUNDER_MIN_DMG					,
		IV_CHAR_FIRE_MAX_DMG						,
		IV_CHAR_WATER_MAX_DMG						,
		IV_CHAR_AIR_MAX_DMG						,
		IV_CHAR_EARTH_MAX_DMG						,
		IV_CHAR_MAGIC_MAX_DMG						,
		IV_CHAR_POISON_MAX_DMG						,
		IV_CHAR_ICE_MAX_DMG						,
		IV_CHAR_THUNDER_MAX_DMG					,

		IV_CHAR_PHYSICAL_RES						,
		IV_CHAR_FIRE_RES							,
		IV_CHAR_WATER_RES							,
		IV_CHAR_AIR_RES							,
		IV_CHAR_EARTH_RES							,
		IV_CHAR_MAGIC_RES							,
		IV_CHAR_POISON_RES							,
		IV_CHAR_ICE_RES							,
		IV_CHAR_THUNDER_RES						,

		IV_CHAR_SKILL_BLOCK						,

		IV_CHAR_GHELLD_MIN							,
		IV_CHAR_GHELLD_MAX							,
		IV_CHAR_DROP_TID							,

		IV_CHAR_SLOW_PERCENT						,
		IV_CHAR_SLOW_TIME							,
		IV_CHAR_FAST_PERCENT						,
		IV_CHAR_FAST_TIME							,

		IV_CHAR_ATTRIBUTE_TYPE						,

		IV_CHAR_ID_COLOR							,

		IV_CHAR_TAMABLE								,

		IV_CHAR_RANGE_TYPE							,
		IV_CHAR_CHAR_STAMINA_POINT					,
		IV_CHAR_PET_TYPE							,
		IV_CHAR_START_STAMINA_POINT					,

		IV_CHAR_HEROIC_MAX_DAMAGE					,
		IV_CHAR_HEROIC_MIN_DAMAGE					,
		IV_CHAR_HEROIC_DEFENSE_POINT				,
		IV_CHAR_HEROIC_MELEE_RESISTANCE				,
		IV_CHAR_HEROIC_RANGED_RESISTANCE			,
		IV_CHAR_HEROIC_MAGIC_RESISTANCE				,


		IV_MAX = 256,
	};

	int	aColumn[ IV_MAX ] = { 0 , };
	const INT16				nKeyRow						= 0;

	for(INT16 nCol = 0; nCol < pExcel->GetColumn(); ++nCol)
	{
		char * pszData = pExcel->GetData(nCol, nKeyRow);
		if(!pszData)
			continue;
		if(!strcmp(pszData,			AGPMCHAR_IMPORT_CHAR_TYPE								)) aColumn[ nCol] = IV_CHAR_TYPE								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_RACE								)) aColumn[ nCol] = IV_CHAR_RACE								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_GENDER								)) aColumn[ nCol] = IV_CHAR_GENDER								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_CLASS								)) aColumn[ nCol] = IV_CHAR_CLASS								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_LEVEL								)) aColumn[ nCol] = IV_CHAR_LEVEL								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_STR								)) aColumn[ nCol] = IV_CHAR_STR									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_DEX								)) aColumn[ nCol] = IV_CHAR_DEX									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_INT								)) aColumn[ nCol] = IV_CHAR_INT									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_WIS								)) aColumn[ nCol] = IV_CHAR_WIS									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_CON								)) aColumn[ nCol] = IV_CHAR_CON									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_CHA								)) aColumn[ nCol] = IV_CHAR_CHA									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AC									)) aColumn[ nCol] = IV_CHAR_AC									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HP_MAX								)) aColumn[ nCol] = IV_CHAR_HP_MAX								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_SP_MAX								)) aColumn[ nCol] = IV_CHAR_SP_MAX								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MP_MAX								)) aColumn[ nCol] = IV_CHAR_MP_MAX								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MOVEMENT_WALK						)) aColumn[ nCol] = IV_CHAR_MOVEMENT_WALK						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MOVEMENT_RUN						)) aColumn[ nCol] = IV_CHAR_MOVEMENT_RUN						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MIN_DMG							)) aColumn[ nCol] = IV_CHAR_MIN_DMG								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAX_DMG							)) aColumn[ nCol] = IV_CHAR_MAX_DMG								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ATTACK_RANGE						)) aColumn[ nCol] = IV_CHAR_ATTACK_RANGE						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_DEFENSE_RATE						)) aColumn[ nCol] = IV_CHAR_DEFENSE_RATE						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ATK_SPEED							)) aColumn[ nCol] = IV_CHAR_ATK_SPEED							;
		//else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ATK_POINT							)) aColumn[ nCol] = IV_CHAR_ATK_POINT							;
		//else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_ATK_POINT					)) aColumn[ nCol] = IV_CHAR_MAGIC_ATK_POINT					;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AR									)) aColumn[ nCol] = IV_CHAR_AR									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_DR									)) aColumn[ nCol] = IV_CHAR_DR									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAR								)) aColumn[ nCol] = IV_CHAR_MAR									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MDR								)) aColumn[ nCol] = IV_CHAR_MDR									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPROT_CHAR_EXP								)) aColumn[ nCol] = IV_CHAR_EXP									;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_INTENSITY					)) aColumn[ nCol] = IV_CHAR_MAGIC_INTENSITY						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_LAND_ATTACH_TYPE					)) aColumn[ nCol] = IV_CHAR_LAND_ATTACH_TYPE					;
		/*																										
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FIRE_DMG							)) aColumn[ nCol] = IV_CHAR_FIRE_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_WATER_DMG							)) aColumn[ nCol] = IV_CHAR_WATER_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AIR_DMG							)) aColumn[ nCol] = IV_CHAR_AIR_DMG								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_EARTH_DMG							)) aColumn[ nCol] = IV_CHAR_EARTH_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_DMG							)) aColumn[ nCol] = IV_CHAR_MAGIC_DMG							;
		*/																										
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FIRE_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_FIRE_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_WATER_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_WATER_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AIR_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_AIR_MIN_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_EARTH_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_EARTH_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_MAGIC_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_POISON_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_POISON_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ICE_MIN_DMG						)) aColumn[ nCol] = IV_CHAR_ICE_MIN_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_THUNDER_MIN_DMG					)) aColumn[ nCol] = IV_CHAR_THUNDER_MIN_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FIRE_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_FIRE_MAX_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_WATER_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_WATER_MAX_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AIR_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_AIR_MAX_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_EARTH_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_EARTH_MAX_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_MAGIC_MAX_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_POISON_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_POISON_MAX_DMG						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ICE_MAX_DMG						)) aColumn[ nCol] = IV_CHAR_ICE_MAX_DMG							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_THUNDER_MAX_DMG					)) aColumn[ nCol] = IV_CHAR_THUNDER_MAX_DMG						;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_PHYSICAL_RES						)) aColumn[ nCol] = IV_CHAR_PHYSICAL_RES						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FIRE_RES							)) aColumn[ nCol] = IV_CHAR_FIRE_RES							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_WATER_RES							)) aColumn[ nCol] = IV_CHAR_WATER_RES							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_AIR_RES							)) aColumn[ nCol] = IV_CHAR_AIR_RES								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_EARTH_RES							)) aColumn[ nCol] = IV_CHAR_EARTH_RES							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_MAGIC_RES							)) aColumn[ nCol] = IV_CHAR_MAGIC_RES							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_POISON_RES							)) aColumn[ nCol] = IV_CHAR_POISON_RES							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ICE_RES							)) aColumn[ nCol] = IV_CHAR_ICE_RES								;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_THUNDER_RES						)) aColumn[ nCol] = IV_CHAR_THUNDER_RES							;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_SKILL_BLOCK						)) aColumn[ nCol] = IV_CHAR_SKILL_BLOCK							;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_GHELLD_MIN							)) aColumn[ nCol] = IV_CHAR_GHELLD_MIN							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_GHELLD_MAX							)) aColumn[ nCol] = IV_CHAR_GHELLD_MAX							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_DROP_TID							)) aColumn[ nCol] = IV_CHAR_DROP_TID							;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_SLOW_PERCENT						)) aColumn[ nCol] = IV_CHAR_SLOW_PERCENT						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_SLOW_TIME							)) aColumn[ nCol] = IV_CHAR_SLOW_TIME							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FAST_PERCENT						)) aColumn[ nCol] = IV_CHAR_FAST_PERCENT						;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_FAST_TIME							)) aColumn[ nCol] = IV_CHAR_FAST_TIME							;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ATTRIBUTE_TYPE						)) aColumn[ nCol] = IV_CHAR_ATTRIBUTE_TYPE						;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_ID_COLOR							)) aColumn[ nCol] = IV_CHAR_ID_COLOR							;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_TAMABLE							)) aColumn[ nCol] = IV_CHAR_TAMABLE								;
																												
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_RANGE_TYPE							)) aColumn[ nCol] = IV_CHAR_RANGE_TYPE							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_STAMINA_POINT						)) aColumn[ nCol] = IV_CHAR_CHAR_STAMINA_POINT					;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_PET_TYPE							)) aColumn[ nCol] = IV_CHAR_PET_TYPE							;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_START_STAMINA_POINT				)) aColumn[ nCol] = IV_CHAR_START_STAMINA_POINT					;

		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_MAX_DAMAGE					)) aColumn[ nCol] = IV_CHAR_HEROIC_MAX_DAMAGE					;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_MIN_DAMAGE					)) aColumn[ nCol] = IV_CHAR_HEROIC_MIN_DAMAGE					;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_DEFENSE_POINT				)) aColumn[ nCol] = IV_CHAR_HEROIC_DEFENSE_POINT				;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_MELEE_RESISTANCE			)) aColumn[ nCol] = IV_CHAR_HEROIC_MELEE_RESISTANCE				;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_RANGED_RESISTANCE			)) aColumn[ nCol] = IV_CHAR_HEROIC_RANGED_RESISTANCE				;
		else if(!strcmp(pszData,	AGPMCHAR_IMPORT_CHAR_HEROIC_MAGIC_RESISTANCE			)) aColumn[ nCol] = IV_CHAR_HEROIC_MAGIC_RESISTANCE				;

	}

	const INT16				nTNameColumn				= 0;
	char					*pszData					= NULL;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= NULL;

	for(INT16 nRow = 1; nRow < pExcel->GetRow(); ++nRow)
	{
		if( pExcel->IsEmpty(nTNameColumn, nRow) ) continue;

		INT32	nTID = pExcel->GetDataToInt( nTNameColumn, nRow );
		pcsAgpdCharacterTemplate = GetCharacterTemplate( nTID );
		if(!pcsAgpdCharacterTemplate)
		{
			sprintf(szDebugString, "AgpmCharacter::StreamReadImportData() %d이(가) 없습니다!!!\n", nTID);
			OutputDebugString(szDebugString);

			sprintf(szDebugString, "WARNNING > %d이(가) 업습니다!!!", nTID);

			EnumCallback(AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA_ERROR_REPORT, szDebugString, NULL);

			continue;
		}

		//sprintf(szDebugString, "FAILED > %s !!!", pszData);

		for(INT16 nCol = 1; nCol < pExcel->GetColumn(); ++nCol)
		{
			pszData = pExcel->GetData(nCol, nKeyRow);
			if(!pszData)
				continue;
			switch( aColumn[ nCol ] )
			{
			case IV_CHAR_TYPE:
				{
					pcsAgpdCharacterTemplate->m_ulCharType	= (UINT32) pExcel->GetDataToInt(nCol, nRow);
				}
				break;
			case IV_CHAR_RACE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (4) !!!\n");
						return FALSE;
					}

					if (pcsAgpdCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_PC)
						EnumCallback(AGPMCHAR_CB_ID_INIT_TEMPLATE_DEFAULT_VALUE, pcsAgpdCharacterTemplate, NULL);
				}
				break;
			case IV_CHAR_GENDER:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_GENDER))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (5) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_CLASS:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (6) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_LEVEL:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (7) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_STR:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (9) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_DEX:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (10) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_INT:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (11) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_WIS:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (12) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_CON:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (13) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_CHA:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (14) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_AC:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (15) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HP_MAX:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (16) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_SP_MAX:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (17) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MP_MAX:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (18) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MOVEMENT_RUN:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (19) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MOVEMENT_WALK:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (19) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (20) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (21) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_ATTACK_RANGE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (22) !!!\n");
						return FALSE;
					}
				}
				break;
			//case IV_CHAR_DEFENSE_RATE:
			//{
			//	if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
			//	{
			//		pExcel->CloseFile();
			//		OutputDebugString("AgpmCharacter::StreamReadImportData() Error (23) !!!\n");
			//		return FALSE;
			//	}
			//}
			case IV_CHAR_ATK_SPEED:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (24) !!!\n");
						return FALSE;
					}
				}
				break;
/*			case IV_CHAR_ATK_POINT:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_AP))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (25) !!!\n");
					return FALSE;
				}
			}
			case IV_CHAR_MAGIC_ATK_POINT:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MAP))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (26) !!!\n");
					return FALSE;
				}
			}*/
			case IV_CHAR_AR:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (25) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_DR:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (26) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAR:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (25) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MDR:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MDR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (26) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_EXP:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_BASE_EXP))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (26) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAGIC_INTENSITY:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MI))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (27) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_FIRE_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-1) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_WATER_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-2) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_AIR_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-3) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_EARTH_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-4) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAGIC_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-5) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_POISON_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-5) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_ICE_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-5) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_THUNDER_MIN_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-5) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_FIRE_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-6) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_WATER_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-7) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_AIR_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-8) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_EARTH_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-9) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAGIC_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-10) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_POISON_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-10) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_ICE_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-10) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_THUNDER_MAX_DMG:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-10) !!!\n");
						return FALSE;
					}
				}
				break;
			/*
			case IV_CHAR_FIRE_DMG:
			{
				// 얘네들은 min, max가 없으므로 둘 다 설정해버린당~ 아행행~ -_-;;;
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-1) !!!\n");
					return FALSE;
				}

				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (28-2) !!!\n");
					return FALSE;
				}
			}
			case IV_CHAR_WATER_DMG:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (29-1) !!!\n");
					return FALSE;
				}

				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (29-2) !!!\n");
					return FALSE;
				}
			}
			case IV_CHAR_AIR_DMG:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (30-1) !!!\n");
					return FALSE;
				}

				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (30-2) !!!\n");
					return FALSE;
				}
			}
			case IV_CHAR_EARTH_DMG:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (31-1) !!!\n");
					return FALSE;
				}

				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (31-2) !!!\n");
					return FALSE;
				}
			}
			case IV_CHAR_MAGIC_DMG:
			{
				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (32-1) !!!\n");
					return FALSE;
				}

				if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
				{
					OutputDebugString("AgpmCharacter::StreamReadImportData() Error (32-2) !!!\n");
					return FALSE;
				}
			}
			*/
			case IV_CHAR_PHYSICAL_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (33) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_FIRE_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (33) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_WATER_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (34) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_AIR_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (35) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_EARTH_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (36) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_MAGIC_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_POISON_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_POISON))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_ICE_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_ICE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_THUNDER_RES:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_SKILL_BLOCK:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_MAX_DAMAGE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_MIN_DAMAGE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_DEFENSE_POINT:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_MELEE_RESISTANCE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_MELEE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_RANGED_RESISTANCE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_RANGE))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_HEROIC_MAGIC_RESISTANCE:
				{
					if(!SetStreamFactor(pExcel, &pcsAgpdCharacterTemplate->m_csFactor, nRow, nCol, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_MAGIC))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (37) !!!\n");
						return FALSE;
					}
				}
				break;
			case IV_CHAR_GHELLD_MIN:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lGhelldMin = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_GHELLD_MAX:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lGhelldMax = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_DROP_TID:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lDropTID = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_SLOW_PERCENT:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_csDecreaseData.m_lSlowPercent = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_SLOW_TIME:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_csDecreaseData.m_lSlowTime = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_FAST_PERCENT:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_csDecreaseData.m_lFastPercent = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_FAST_TIME:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_csDecreaseData.m_lFastTime = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_ATTRIBUTE_TYPE:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_eAttributeType = (AgpdFactorAttributeType) pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_ID_COLOR:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						char			*pstrData = pExcel->GetData( nCol, nRow );
						CHAR	*szEnd	= NULL;
						pcsAgpdCharacterTemplate->m_ulIDColor = strtoul( pstrData, &szEnd, 16 );
					}
				}
				break;
			case IV_CHAR_TAMABLE:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_eTamableType = (AgpdCharacterTamableType)pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_RANGE_TYPE:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_eRangeType = (AgpdCharacterRangeType)pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_CHAR_STAMINA_POINT:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lStaminaPoint = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_PET_TYPE:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lPetType = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_START_STAMINA_POINT:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lStartStaminaPoint = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			case IV_CHAR_LAND_ATTACH_TYPE:
				{
					if( !pExcel->IsEmpty( nCol , nRow )  )
					{
						pcsAgpdCharacterTemplate->m_lLandAttachType = pExcel->GetDataToInt( nCol, nRow );
					}
				}
				break;
			default:
				{
					PVOID	pvBuffer[3];
					pvBuffer[0]				= (PVOID)	pExcel;
					pvBuffer[1]				= (PVOID)	nRow;
					pvBuffer[2]				= (PVOID)	nCol;

					if (!EnumCallback(AGPMCHAR_CB_ID_STREAM_READ_IMPORT_DATA, pcsAgpdCharacterTemplate, pvBuffer))
					{
						pExcel->CloseFile();
						OutputDebugString("AgpmCharacter::StreamReadImportData() Error (300) !!!\n");
						return FALSE;
					}
				}
				break;
			}
		}
	}

	pExcel->CloseFile();
	return TRUE;
}

/******************************************************************************
* Purpose : Factor를 설정한다.
*
* Params :	pcsAgpdFactor		-	설정할 Factor
			nRow				-	값을 파싱할 행.
*			nColumn				-	파싱할 열.
*			eType				-	Factor type.
*			lSubType...			-	Factor sub type...
*
* 032403. Bob Jung
******************************************************************************/
BOOL AgpmCharacter::SetStreamFactor(AuExcelLib *pcsExcelTxtLib, AgpdFactor *pcsAgpdFactor, INT16 nRow, INT16 nColumn, eAgpdFactorsType eType, INT32 lSubType1, INT32 lSubType2, INT32 lSubType3)
{
	if (!pcsExcelTxtLib)
		return FALSE;

	INT32					lValue		= 0;
	FLOAT					fValue		= 0.0f;
	char					*pszData	= NULL;

	pszData = pcsExcelTxtLib->GetData(nColumn, nRow);
	if(pszData)
	{
		if (eType == AGPD_FACTORS_TYPE_PRICE &&
			(lSubType1 == AGPD_FACTORS_PRICE_TYPE_NPC_PRICE || lSubType1 == AGPD_FACTORS_PRICE_TYPE_PC_PRICE))
		{
			FLOAT			fPrice		= (FLOAT)atof(pszData);

			CopyMemory(&lValue, &fPrice, sizeof(INT32));
		}
		else
		{
			lValue = atoi(pszData);
			fValue = (FLOAT)atof(pszData);
		}
	}

	// Factor를 할당한다.
	if(!m_pcsAgpmFactors->SetFactor(pcsAgpdFactor, NULL, eType))
	{
		OutputDebugString("AgpmCharacter::SetFactor() Error (4) !!!\n");
		return FALSE;
	}

	// Factor를 설정한다.
	if (m_pcsAgpmFactors->IsFloatValue(eType, lSubType1, lSubType2, lSubType3))
	{
		if(!m_pcsAgpmFactors->SetValue(pcsAgpdFactor, fValue, eType, lSubType1, lSubType2, lSubType3))
		{
			OutputDebugString("AgpmCharacter::SetFactor() Error (5) !!!\n");
			return FALSE;
		}
	}
	else
	{
		if(!m_pcsAgpmFactors->SetValue(pcsAgpdFactor, lValue, eType, lSubType1, lSubType2, lSubType3))
		{
			OutputDebugString("AgpmCharacter::SetFactor() Error (5) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmCharacter::StreamReadCharismaTitle(CHAR *pszFile, BOOL bDecryption)
	{
	if (NULL == pszFile || '\0' == *pszFile)
		return FALSE;
	
	AuExcelTxtLib Excel;
	if (!Excel.OpenExcelFile(pszFile, TRUE, bDecryption))
		return FALSE;
	
	// init
	if (m_pAgpdCharisma)
		{
		delete [] m_pAgpdCharisma;
		m_pAgpdCharisma = NULL;
		}
	m_lTotalCharisma = 0;
	
	// alloc
	m_pAgpdCharisma = new AgpdCharisma[Excel.GetRow() - 1];
	
	for (INT32 lRow = 1; lRow < Excel.GetRow(); lRow++)
		{
		CHAR *psz = NULL;
		// max point
		psz = Excel.GetData(1, lRow);
		if (!psz)
			continue;
		m_pAgpdCharisma[m_lTotalCharisma].m_lPoint = atoi(psz);

		// title		
		psz = Excel.GetData(2, lRow);
		strcpy(m_pAgpdCharisma[m_lTotalCharisma].m_szTitle, psz ? psz : "");
		
		// skill

		m_lTotalCharisma++;		
		}

  #ifdef _DEBUG
	ASSERT(m_lTotalCharisma == (Excel.GetRow() - 1));
	
	for (INT32 i = 1; i < m_lTotalCharisma; i++)
	{
		ASSERT(m_pAgpdCharisma[i].m_lPoint > m_pAgpdCharisma[i-1].m_lPoint);
	}
  #endif
	
	return TRUE;
	}
