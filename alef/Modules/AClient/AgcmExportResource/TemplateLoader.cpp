#pragma warning ( disable : 4819 )
#include "AgcmExportResource.h"
#include "AgcmSkill.h"
#include "AgcmEventEffect.h"
#include "AgcmGeometryDataManager.h"
#include <algorithm>
using namespace TSO;
using namespace std;

#include "AuFileFind.h"

class	SequenceNumberCompare
{
public:
	bool	operator()( const string	&strA , const string &strB )
	{
		CHAR	szName[MAX_PATH];
		CHAR	szExt[MAX_PATH];

		_splitpath( strA.c_str() , NULL , NULL, szName , szExt );
		INT32 nTidA	= atoi( szName  );
		INT32 nSeqA	= atoi( szExt + 1  );
		_splitpath( strB.c_str() , NULL , NULL, szName , szExt );
		INT32 nTidB	= atoi( szName );
		INT32 nSeqB	= atoi( szExt + 1 );

		if( nSeqA == nSeqB && nTidA != nTidB )
		{
			// 만에 하나를 위한 예외 처리.
			char	strMessage[ 1024 ];
			sprintf( strMessage , "'%s' 파일과 , '%s' 파일의 인덱스가 겹칩니다." , strA.c_str() , strB.c_str() );
			::MessageBox( NULL , strMessage , "Template Loader" , MB_OK  );
		}

		return nSeqA < nSeqB;
	}
};

BOOL	TSO::FindIniFiles( const char * pPath , vector< string >	&vecFile )
{
	AuFileFind	csFinder;
	char	strPath[ MAX_PATH ];
	sprintf( strPath , "%s\\*.*" , pPath );
	if (!csFinder.FindFile( strPath ) )
		return FALSE;

	CHAR	szFileName[MAX_PATH];
	CHAR	szName[MAX_PATH];
	CHAR	szExt[MAX_PATH];

	// 처음꺼
	csFinder.GetFileName(szFileName);
	_splitpath( szFileName , NULL , NULL, szName , szExt );
	INT32 nTID	= atoi( szName );
	INT32 nSeq	= atoi( szExt + 1 );

	if( nTID && nSeq )
	{
		vecFile.push_back( szFileName );
	}

	// 나머지..
	for (; csFinder.FindNextFile(); )
	{
		csFinder.GetFileName(szFileName);
		_splitpath( szFileName , NULL , NULL, szName , szExt );
		INT32 nTID	= atoi( szName );
		INT32 nSeq	= atoi( szExt + 1 );

		if( nTID && nSeq )
		{
			vecFile.push_back( szFileName );
		}
	}

	if( vecFile.size() )
	{
		sort( vecFile.begin() , vecFile.end() , SequenceNumberCompare() );
		return TRUE;
	}
	else					return FALSE;
}

INT32		TSO::GetSequenceNumber( const char *pPath )
{
	INT32	nSequence = 0;

	// 확장자에서 Seq Number를 뽑아낸다.
	char	szExt[ MAX_PATH ];
	_splitpath( pPath , NULL , NULL , NULL , szExt );
	if( strlen( szExt ) )
		nSequence = atoi( szExt + 1 );

	return nSequence;
}



CharacterLoader::CharacterLoader( ApModuleManager &csModuleManager ):
	LoaderBase< AgpdCharacterTemplate >( csModuleManager ),
	pcsAgpmCharacter			( NULL ),
	pcsAgcmCharacter			( NULL ),
	pcsAgcmSkill				( NULL ),
	pcsAgcmEventEffect			( NULL )
{
	pcsAgpmCharacter		= dynamic_cast< AgpmCharacter		*>( m_csModuleManager.GetModule( "AgpmCharacter"	) );
	pcsAgcmCharacter		= dynamic_cast< AgcmCharacter		*>( m_csModuleManager.GetModule( "AgcmCharacter"	) );
	pcsAgcmSkill			= dynamic_cast< AgcmSkill			*>( m_csModuleManager.GetModule( "AgcmSkill"		) );
	pcsAgcmEventEffect		= dynamic_cast< AgcmEventEffect		*>( m_csModuleManager.GetModule( "AgcmEventEffect") );
}

AgpdCharacterTemplate * CharacterLoader::AddTemplate( INT32 lTID	)	{ return pcsAgpmCharacter->AddCharacterTemplate(lTID); }
AgpdCharacterTemplate *	CharacterLoader::GetSequence( INT32	&lIndex	)	{ return pcsAgpmCharacter->GetTemplateSequence(&lIndex); }

BOOL	CharacterLoader::SaveTemplate( ApModuleStream	&csStream , AgpdCharacterTemplate * pcsTemplate )
{
	// Stream Enumerate 한다.
	// Public
	if (!csStream.EnumWriteCallback(AGPMCHAR_DATA_TYPE_TEMPLATE, pcsTemplate, pcsAgpmCharacter))
		return FALSE;

	// Client
	if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_TEMPLATE, pcsTemplate, pcsAgcmCharacter))
		return FALSE;

	// Animation
	if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_ANIMATION, pcsTemplate, pcsAgcmCharacter))
		return FALSE;

	// Customize
	if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_CUSTOMIZE, pcsTemplate, pcsAgcmCharacter))
		return FALSE;

	// Skill
	if (!csStream.EnumWriteCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL, pcsTemplate, pcsAgcmSkill))
		return FALSE;

	// Skill Sound
	if (!csStream.EnumWriteCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND, pcsTemplate, pcsAgcmSkill))
		return FALSE;

	// EventEffect
	if (!csStream.EnumWriteCallback(AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE, pcsTemplate, pcsAgcmEventEffect))
		return FALSE;

	// PreLOD Data
	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetCharacterPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	// pcsAgcmGeometryDataManager
	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame		*pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return FALSE;
	}

	return TRUE;
}

BOOL	CharacterLoader::Initialize( int nKeyNum )
{
	int nAlloc = pcsAgpmCharacter->m_csACharacterTemplate.GetCount();

	if( nAlloc < nKeyNum )
	{
		if( pcsAgpmCharacter->m_csACharacterTemplate.GetObjectCount() == 0 )
		{
			// 마고자 (2005-12-28 오전 11:28:06) : 
			// 모델툴이 아닌경우는 , 런타임중에 케릭터 템플릿이 변경돼는 경우가 없으니
			// 이렇게 해주어도 문제가 돼지 않는다.
			pcsAgpmCharacter->SetMaxCharacterTemplate( nKeyNum );
			nAlloc = nKeyNum;
		}
		else
		{
			ASSERT( !"아이템 템플릿 갯수초과!" );
		}
	}

	if (!pcsAgpmCharacter->m_csACharacterTemplate.InitializeObject( sizeof(AgpdCharacterTemplate *), nAlloc ) )
	{
	 	return FALSE;
	}

	return TRUE;
}


BOOL	CharacterLoader::LoadTemplate( ApModuleStream	&csStream , AgpdCharacterTemplate * pcsTemplate )
{
	// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
	if (!csStream.EnumReadCallback(AGPMCHAR_DATA_TYPE_TEMPLATE						, pcsTemplate, pcsAgpmCharacter)	)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMCHAR_DATA_TYPE_TEMPLATE						, pcsTemplate, pcsAgcmCharacter)	)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMCHAR_DATA_TYPE_ANIMATION						, pcsTemplate, pcsAgcmCharacter)	)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMCHAR_DATA_TYPE_CUSTOMIZE						, pcsTemplate, pcsAgcmCharacter)	)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL			, pcsTemplate, pcsAgcmSkill)		)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMSKILL_DATA_TYPE_CHAR_TEMPLATE_SKILL_SOUND	, pcsTemplate, pcsAgcmSkill)		)	return FALSE;
	if (!csStream.EnumReadCallback(AGCMEVENT_EFFECT_DATA_TYPE_CHAR_TEMPLATE			, pcsTemplate, pcsAgcmEventEffect)	)	return FALSE;

	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetCharacterPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::CharLodListStreamRead() Error (3) !!!\n");
			return FALSE;
		}
	}

	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame *pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_CHARACTER, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

ObjectLoader::ObjectLoader( ApModuleManager &csModuleManager ):
	LoaderBase< ApdObjectTemplate >( csModuleManager ),
	pcsApmObject				( NULL )
{
	pcsApmObject				= dynamic_cast< ApmObject	*>( m_csModuleManager.GetModule( "ApmObject"	) );
}

ApdObjectTemplate * ObjectLoader::AddTemplate( INT32 lTID	)	{ return pcsApmObject->AddObjectTemplate( lTID ); }
ApdObjectTemplate * ObjectLoader::GetSequence( INT32	&lIndex	)	{ return pcsApmObject->GetObjectTemplateSequence(&lIndex); }


BOOL	ObjectLoader::SaveTemplate( ApModuleStream	&csStream , ApdObjectTemplate * pcsTemplate )
{
	// Stream Enumerate 한다.
	if (!csStream.EnumWriteCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, pcsTemplate, pcsApmObject))
	{
		ASSERT(!"ApmObject::StreamWriteTemplate() EnumWriteCallback Failure !!!");
		return FALSE;
	}

	// PreLOD 정보
	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetObjectPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}
		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame	*pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return FALSE;
	}

	return TRUE;
}

BOOL	ObjectLoader::LoadTemplate( ApModuleStream	&csStream , ApdObjectTemplate * pcsTemplate )
{
	// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
	if (!csStream.EnumReadCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, pcsTemplate, pcsApmObject))
	{
		ASSERT(!"TSO::LoadObjectTemplateSeperated() EnumReadCallback Failure !!!");
		return FALSE;
	}

	// PreLOD
	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetObjectPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamRead() Error (2) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::ObjectLodListStreamRead() Error (3) !!!\n");
			return FALSE;
		}
	}

	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame		*pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_OBJECT, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return FALSE;
	}

	return TRUE;
}

BOOL	ObjectLoader::Initialize( int nKeyNum )
{
	ASSERT( nKeyNum <= pcsApmObject->m_clObjectTemplates.GetCount() );

	return TRUE;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

ItemLoader::ItemLoader( ApModuleManager &csModuleManager ):
	LoaderBase< AgpdItemTemplate >( csModuleManager ),
	pcsAgpmItem				( NULL )
{
	pcsAgpmItem				= dynamic_cast< AgpmItem	*>( m_csModuleManager.GetModule( "AgpmItem"	) );
}

AgpdItemTemplate * ItemLoader::AddTemplate( INT32 lTID	)	{ return pcsAgpmItem->AddItemTemplate( lTID ); }

BOOL	ItemLoader::SaveTemplate( ApModuleStream	&csStream , AgpdItemTemplate * pcsTemplate )
{
	// Stream Enumerate 한다.
	if (!csStream.EnumWriteCallback(AGPMITEM_DATA_TYPE_TEMPLATE, pcsTemplate, pcsAgpmItem))
	{
		OutputDebugString("SaveItemTemplateSeperated() Error (1) !!!\n");
		return FALSE;
	}

	// PreLOD
	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetItemPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamWrite() Error (1) !!!\n");
			return FALSE;
		}

		if(!csStream.EnumWriteCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamWrite() Error (2) !!!\n");
			return FALSE;
		}
	}

	// pcsAgcmGeometryDataManager
	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame	*pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return FALSE;

		if (!csStream.EnumWriteCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return FALSE;
	}	
	return TRUE;
}

BOOL	ItemLoader::LoadTemplate( ApModuleStream	&csStream , AgpdItemTemplate * pcsTemplate )
{
	// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
	if (!csStream.EnumReadCallback(AGPMITEM_DATA_TYPE_TEMPLATE, pcsTemplate, pcsAgpmItem))
	{
		ASSERT(!"TSO::LoadItemTemplateSeperated() EnumReadCallback Failure !!!");
		return LE_READFAILD;
	}


	// PreLOD
	{
		AgcdPreLOD *pcsAgcdPreLOD = pcsAgcmPreLODManager->GetItemPreLOD(pcsTemplate);
		if(!pcsAgcdPreLOD)
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamRead() Error (2) !!!\n");
			return LE_READFAILD;
		}

		if(!csStream.EnumReadCallback(AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM, pcsAgcdPreLOD, pcsAgcmPreLODManager))
		{
			OutputDebugString("AgcmPreLODManager::ItemLodListStreamRead() Error (3) !!!\n");
			return LE_READFAILD;
		}
	}

	// pcsAgcmGeometryDataManager
	if( pcsAgcmGeometryDataManager )
	{
		AgcdGeometryFrame *pcsFrame = pcsAgcmGeometryDataManager->GetGeometryFrame(pcsTemplate);
		if (!pcsFrame)
			return LE_READFAILD;

		if (!csStream.EnumReadCallback(E_AGCM_GEOM_DATA_STREAM_TYPE_ITEM, (PVOID)(pcsFrame), pcsAgcmGeometryDataManager))
			return LE_READFAILD;
	}	
	return TRUE;
}

BOOL	ItemLoader::Initialize( int nKeyNum )
{
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////


AgpmCharacter	*SequenceNumberManager::GetAgpmCharacter	()
{
	if( m_pcsAgpmCharacter ) return m_pcsAgpmCharacter;
	return m_pcsAgpmCharacter		= dynamic_cast< AgpmCharacter		*>( g_pEngine->GetModule( "AgpmCharacter"	) );
}

ApmObject		*SequenceNumberManager::GetApmObject		()
{
	if( m_pcsApmObject ) return m_pcsApmObject;
	return m_pcsApmObject		= dynamic_cast< ApmObject		*>( g_pEngine->GetModule( "ApmObject"	) );
}

AgpmItem		*SequenceNumberManager::GetAgpmItem		()
{
	if( m_pcsAgpmItem ) return m_pcsAgpmItem;
	return m_pcsAgpmItem		= dynamic_cast< AgpmItem		*>( g_pEngine->GetModule( "AgpmItem"	) );
}

BOOL	SequenceNumberManager::AttachTemplateSequenceData()
{
	m_nAttachDataIndexCharacter	= GetAgpmCharacter	()->AttachCharacterTemplateData	(this, sizeof(stSequenceNumber), OnSequenceNumberCreate< AgpdCharacterTemplate > , NULL );
	m_nAttachDataIndexObject	= GetApmObject		()->AttachObjectTemplateData	(this, sizeof(stSequenceNumber), OnSequenceNumberCreate< ApdObjectTemplate > , NULL );
	m_nAttachDataIndexItem		= GetAgpmItem		()->AttachItemTemplateData		(this, sizeof(stSequenceNumber), OnSequenceNumberCreate< AgpdItemTemplate > , NULL );
	return TRUE;
}

template< typename T >
BOOL SequenceNumberManager::OnSequenceNumberCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	SequenceNumberManager	* pThis	= (SequenceNumberManager *) pClass;
	T	*pcsTemplate		= (T *)(pData);
	SequenceNumberManager::stSequenceNumber	*pSeqNumber	= (SequenceNumberManager::stSequenceNumber *)pThis->GetAttachedData(pcsTemplate);
	pSeqNumber->Init();
	return TRUE;
}

SequenceNumberManager::stSequenceNumber	* SequenceNumberManager::GetAttachedData(	AgpdCharacterTemplate	* pcsTemplate )
{
	return	(SequenceNumberManager::stSequenceNumber*) GetAgpmCharacter()->GetAttachedModuleData(m_nAttachDataIndexCharacter, (void *) pcsTemplate);
}
SequenceNumberManager::stSequenceNumber	* SequenceNumberManager::GetAttachedData(	ApdObjectTemplate		* pcsTemplate )
{
	return	(SequenceNumberManager::stSequenceNumber*) GetApmObject()->GetAttachedModuleData(m_nAttachDataIndexObject, (void *) pcsTemplate);
}
SequenceNumberManager::stSequenceNumber	* SequenceNumberManager::GetAttachedData(	AgpdItemTemplate		* pcsTemplate )
{
	return	(SequenceNumberManager::stSequenceNumber*) GetAgpmItem()->GetAttachedModuleData(m_nAttachDataIndexItem, (void *) pcsTemplate);
}
