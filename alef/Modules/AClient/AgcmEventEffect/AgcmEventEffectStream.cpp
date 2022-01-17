#include "AgcmEventEffect.h"
#include "ApModuleStream.h"

BOOL AgcmEventEffect::StreamReadTemplate( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
{
	if( szFile == NULL )
		return FALSE;

	ApModuleStream			csStream;
	UINT16					nNumKeys;
	INT32					i;
	INT32					lTID;

	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	// szFile을 읽는다.
	csStream.Open(szFile, 0, bDecryption);
	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for( i = 0; i < nNumKeys; ++i )
	{
		// Section Name은 TID 이다.
		lTID = atoi( csStream.ReadSectionName(i) );

		AgpdCharacterTemplate* pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate( lTID );

		if( pcsAgpdCharacterTemplate == NULL ) {
			TRACE( "AgcmSkill::StreamReadTemplateSkill 템플릿을 찾을 수 없습니다.\n" );
			continue;;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);

			TRACE( "AgcmSkill::StreamReadTemplateSkill Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmEventEffect::StreamWriteTemplate(CHAR *szFile, BOOL bEncryption)
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
	for( pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); 
		pcsAgpdCharacterTemplate; 
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}


/******************************************************************************
* Purpose :
*
* 020403. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::CharTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdCharacterTemplate		*pcsAgpdCharacterTemplate		= (AgpdCharacterTemplate *)(pData);
	AgcmEventEffect				*pThis							= (AgcmEventEffect *)(pClass);

	AgcdUseEffectSet			*pstAgcdUseEffectSet			= pThis->GetUseEffectSet(pcsAgpdCharacterTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamRead(pStream, pstAgcdUseEffectSet, CBStreamReadCharacterAnimationAttachedData, (ApBase *)(pcsAgpdCharacterTemplate));
}

/******************************************************************************
* Purpose :
*
* 020403. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::CharTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdCharacterTemplate		*pcsAgpdCharacterTemplate		= (AgpdCharacterTemplate *)(pData);
	AgcmEventEffect				*pThis							= (AgcmEventEffect *)(pClass);

	if (!pThis->StreamWriteCharacterAnimationAttachedData(pcsAgpdCharacterTemplate, pStream)) return FALSE;

	AgcdUseEffectSet			*pstAgcdUseEffectSet				= pThis->GetUseEffectSet(pcsAgpdCharacterTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamWrite(pStream, pstAgcdUseEffectSet);
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::ObjTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	ApdObjectTemplate	*pcsApdObjectTemplate	= (ApdObjectTemplate *)(pData);
	AgcmEventEffect		*pThis					= (AgcmEventEffect *)(pClass);

	AgcdUseEffectSet	*pstAgcdUseEffectSet	= pThis->GetUseEffectSet(pcsApdObjectTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamRead(pStream, pstAgcdUseEffectSet);
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::ObjTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	ApdObjectTemplate		*pcsApdObjectTemplate		= (ApdObjectTemplate *)(pData);
	AgcmEventEffect			*pThis						= (AgcmEventEffect *)(pClass);

	AgcdUseEffectSet		*pstAgcdUseEffectSet		= pThis->GetUseEffectSet(pcsApdObjectTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamWrite(pStream, pstAgcdUseEffectSet);
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::ItemTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdItemTemplate	*pcsAgpdItemTemplate	= (AgpdItemTemplate *)(pData);
	AgcmEventEffect		*pThis					= (AgcmEventEffect *)(pClass);

	AgcdUseEffectSet	*pstAgcdUseEffectSet	= pThis->GetUseEffectSet(pcsAgpdItemTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamRead(pStream, pstAgcdUseEffectSet);
}

/******************************************************************************
* Purpose :
*
* 020703. Bob Jung
******************************************************************************/
BOOL AgcmEventEffect::ItemTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpdItemTemplate	*pcsAgpdItemTemplate	= (AgpdItemTemplate *)(pData);
	AgcmEventEffect		*pThis					= (AgcmEventEffect *)(pClass);

	AgcdUseEffectSet	*pstAgcdUseEffectSet	= pThis->GetUseEffectSet(pcsAgpdItemTemplate);
	if(!pstAgcdUseEffectSet) return FALSE;

	return pThis->UseEffectSetStreamWrite(pStream, pstAgcdUseEffectSet);
}

//@{ 2006/12/05 burumal
BOOL AgcmEventEffect::StreamReadExclusiveEffectMappingTable( RwChar* szFile, BOOL bDecryption )
{
	if ( szFile == NULL )
		return FALSE;

	AuExcelTxtLib cExcelTxtLib;

	if ( !cExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption) )
	{
		OutputDebugString("AgcmEventEffect::StreamReadExclusiveEffectMappingTable() Error (1) !!!\n");
		cExcelTxtLib.CloseFile();
		return FALSE;
	}
	
	const INT16 nKeyRow = 0;
	const INT16 nKeyColumn = 0;	

	char* pszData = NULL;

	for ( INT16 nRow = 1; nRow < cExcelTxtLib.GetRow(); ++nRow )
	{
		pszData = cExcelTxtLib.GetData(nKeyColumn, nRow);
		if ( !pszData )
			continue;

		INT32 nNewDominance = atoi(pszData);
		if ( nNewDominance < 0 )
			continue;		
					
		if ( m_ExclusiveEffectsMgr.FindDominance(nNewDominance) )
			continue;

		m_ExclusiveEffectsMgr.m_vecDominanceTable.push_back(nNewDominance);

		for ( INT16 nCol = 1; nCol < cExcelTxtLib.GetColumn(); ++nCol )
		{
			pszData = cExcelTxtLib.GetData(nCol, nKeyRow);
			if ( !pszData )
				continue;
			
			if ( !strnicmp(pszData, "ExcEffID", strlen("ExcEffID")) )
			{
				pszData = cExcelTxtLib.GetData(nCol, nRow);
				if ( pszData )
				{
					INT32 nNewRecessiveness = atoi(pszData);				
					
					std::map<RwInt32, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>::iterator itR2D = 
						m_ExclusiveEffectsMgr.m_mapRecessivenessToDominanceTable.find(nNewRecessiveness);
					
					if (  itR2D == m_ExclusiveEffectsMgr.m_mapRecessivenessToDominanceTable.end() )
					{
						AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pNewDominanceArray = 
							new AgcmEventEffectExclusiveEffectMgr::CustomIntVector;

						if ( pNewDominanceArray )
						{
							pNewDominanceArray->m_vecArray.push_back(nNewDominance);
							m_ExclusiveEffectsMgr.m_mapRecessivenessToDominanceTable.insert(
								pair<RwInt32, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>(nNewRecessiveness, pNewDominanceArray));
						}
					}
					else
					{
						AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pFound = itR2D->second;						
						if ( pFound )
						{
							if ( m_ExclusiveEffectsMgr.FindDominance(nNewDominance, pFound) == FALSE )
								pFound->m_vecArray.push_back(nNewDominance);
						}
					}
				}
			}
		}
	}

	return TRUE;
}

VOID AgcmEventEffect::CheckExclusiveEffect(RpClump* pClump, UINT32 dwEffID)
{
	if ( !pClump || dwEffID < 0 )
		return;

	std::map<RpClump*, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>::iterator itFound;
	itFound = m_ExclusiveEffectsMgr.m_mapPerClumpDominances.find(pClump);

	if ( itFound == m_ExclusiveEffectsMgr.m_mapPerClumpDominances.end() )
	{
		if ( m_ExclusiveEffectsMgr.FindDominance(dwEffID) )
		{
			AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pNewDominanceArray = 
				new AgcmEventEffectExclusiveEffectMgr::CustomIntVector;

			if ( pNewDominanceArray )
			{
				pNewDominanceArray->m_vecArray.push_back(dwEffID);

				m_ExclusiveEffectsMgr.m_mapPerClumpDominances.insert(
					pair<RpClump*, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>(pClump, pNewDominanceArray));
			}
		}
	}
	else
	{
		AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pCurDominanceArray = itFound->second;
		if ( pCurDominanceArray )
		{
			if ( m_ExclusiveEffectsMgr.FindDominance(dwEffID) )
			{
				if ( m_ExclusiveEffectsMgr.FindDominance(dwEffID, pCurDominanceArray) == FALSE )
				{
					pCurDominanceArray->m_vecArray.push_back(dwEffID);
			
				#ifdef _DEBUG
					char pBuff[128];
					sprintf(pBuff, "망토겹: %x  ->  %d\n", pClump, pCurDominanceArray->m_vecArray.size());
					OutputDebugString(pBuff);
				#endif
				}
			}
		}
	}
}

BOOL AgcmEventEffect::IsExclusiveEffect(RpClump* pClump, UINT32 dwEffID)
{
	if ( !pClump || dwEffID < 0 )
		return FALSE;

	if ( m_ExclusiveEffectsMgr.FindDominance(dwEffID) )
		return FALSE;

	
	std::map<RwInt32, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>::iterator itR2D = 
		m_ExclusiveEffectsMgr.m_mapRecessivenessToDominanceTable.find(dwEffID);

	if ( itR2D == m_ExclusiveEffectsMgr.m_mapRecessivenessToDominanceTable.end() )
		return FALSE;

	AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pCurEffDominanceArray = itR2D->second;

	if ( pCurEffDominanceArray )
	{
		std::map<RpClump*, AgcmEventEffectExclusiveEffectMgr::CustomIntVector*>::iterator itCD = 
			m_ExclusiveEffectsMgr.m_mapPerClumpDominances.find(pClump);

		if ( itCD == m_ExclusiveEffectsMgr.m_mapPerClumpDominances.end() )
			return FALSE;

		AgcmEventEffectExclusiveEffectMgr::CustomIntVector* pCurClumpEffDominanceArray = itCD->second;

		if ( pCurClumpEffDominanceArray )
		{
			for ( UINT32 nClumpIdx = 0; nClumpIdx < m_ExclusiveEffectsMgr.m_mapPerClumpDominances.size(); ++nClumpIdx )
			{
				if ( m_ExclusiveEffectsMgr.FindDominance(pCurClumpEffDominanceArray->m_vecArray[nClumpIdx], pCurEffDominanceArray) )
					return TRUE;
			}
		}
	}

	return FALSE;
}

VOID AgcmEventEffect::ClearExclusiveEffectInfoPerFrame()
{
	m_ExclusiveEffectsMgr.Clear();
}

RwBool AgcmEventEffectExclusiveEffectMgr::FindDominance(RwInt32 dwNewDominance)
{
	UINT32 nIdx;
	for ( nIdx = 0; nIdx < m_vecDominanceTable.size(); ++nIdx )
		if ( m_vecDominanceTable[nIdx] == dwNewDominance )
			return TRUE;

	return FALSE;
}

RwBool AgcmEventEffectExclusiveEffectMgr::FindDominance(RwInt32 dwNewDominance, CustomIntVector* pDominanceTable)
{
	if ( !pDominanceTable )
		return FALSE;

	UINT32 nIdx;
	for ( nIdx = 0; nIdx < pDominanceTable->m_vecArray.size(); ++nIdx )
		if ( pDominanceTable->m_vecArray[nIdx] == dwNewDominance )
			return TRUE;

	return FALSE;
}

void AgcmEventEffectExclusiveEffectMgr::Clear()
{	
	std::map<RpClump*, CustomIntVector*>::iterator itCurCD;
	while( (itCurCD = m_mapPerClumpDominances.begin()) != m_mapPerClumpDominances.end() )
	{
		ASSERT(itCurCD->second);
		delete itCurCD->second;
		m_mapPerClumpDominances.erase(itCurCD);
	}	
	m_mapPerClumpDominances.clear();
}

void AgcmEventEffectExclusiveEffectMgr::Relase()
{
	m_vecDominanceTable.clear();
	
	std::map<RwInt32, CustomIntVector*>::iterator itCurR2D;
	while( (itCurR2D = m_mapRecessivenessToDominanceTable.begin()) != m_mapRecessivenessToDominanceTable.end() )
	{
		ASSERT(itCurR2D->second);
		delete itCurR2D->second;
		m_mapRecessivenessToDominanceTable.erase(itCurR2D);
	}
	m_mapRecessivenessToDominanceTable.clear();	
	
	std::map<RpClump*, CustomIntVector*>::iterator itCurCD;
	while( (itCurCD = m_mapPerClumpDominances.begin()) != m_mapPerClumpDominances.end() )
	{
		ASSERT(itCurCD->second);
		delete itCurCD->second;
		m_mapPerClumpDominances.erase(itCurCD);
	}	
	m_mapPerClumpDominances.clear();
}
//@}
