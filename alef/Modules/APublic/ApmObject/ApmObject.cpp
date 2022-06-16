#include <stdio.h>

#include "ApmObject.h"
#include "MagDebug.h"

#include "AuMath.h"
#include "ApModuleStream.h"

#include "dbghelp.h"
extern MINIDUMP_TYPE	g_eMiniDumpType	;//


ApmObject::ApmObject()
{
	SetModuleName("ApmObject");
	EnableIdle(TRUE);

	// Data�� ũ�⸦ �����Ѵ�. (Object�� Object Template)
	SetModuleData(sizeof(ApdObject), APMOBJECT_DATA_OBJECT);
	SetModuleData(sizeof(ApdObjectTemplate), APMOBJECT_DATA_OBJECT_TEMPLATE);

	m_ulPrevRemoveClockCount	= 0;
	m_bUseObjectIndexUpdate		= FALSE;

	sprintf( m_strDir, "%s", "Ini\\" );
}

BOOL ApmObject::OnAddModule()
{
	// ���� Data�� Callback ���� ���� Module�� ���δ�.
	// Streaming�� ���� Callback���� �����Ѵ�. (�ڱ� �ڽ��� Callback, ApModuleStream�� ����ϱ� ���ؼ�)
	if (!AddStreamCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, ObjectTemplateReadCB, ObjectTemplateWriteCB, this))
		return FALSE;

	if (!AddStreamCallback(APMOBJECT_DATA_OBJECT, ObjectReadCB, ObjectWriteCB, this))
		return FALSE;

	m_pcsApmMap = (ApmMap *) GetModule("ApmMap");

	return TRUE;
}

BOOL ApmObject::OnInit()
{
	if (!m_clObjects.InitializeObject(sizeof(ApdObject *), m_clObjects.GetCount()))
		return FALSE;

	if (!m_clObjectTemplates.InitializeObject(sizeof(ApdObjectTemplate *), m_clObjectTemplates.GetCount()))
		return FALSE;

	if (!m_csAdminRemove.InitializeObject(sizeof(ApdObject *), m_csAdminRemove.GetCount()))
		return FALSE;

	return TRUE;
}

BOOL ApmObject::OnDestroy()
{
	ProcessRemove(0);

	INT32 lIndex = 0;
	for( ApdObject* pcsObject = GetObjectSequence(&lIndex); pcsObject; pcsObject = GetObjectSequence(&lIndex))
		DeleteObject(pcsObject, TRUE, TRUE);
	m_clObjects.RemoveObjectAll();

	lIndex = 0;
	for (ApdObjectTemplate* pcsTemplate = GetObjectTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = GetObjectTemplateSequence(&lIndex))
		DestroyTemplate(pcsTemplate);
	m_clObjectTemplates.RemoveObjectAll();

	return TRUE;
}

BOOL	ApmObject::OnIdle(UINT32	ulClockCount)
{
	PROFILE("ApmObject::OnIdle");

	return ProcessRemove(ulClockCount);;
}

//---------------------------- ApaObject Manager -------------------------
ApdObject * ApmObject::GetObjectSequence(INT32 *plIndex)
{
	ApdObject** pcsObject = (ApdObject **) m_clObjects.GetObjectSequence(plIndex);
	return pcsObject ? *pcsObject : NULL;
}

INT16 ApmObject::AttachObjectData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, APMOBJECT_DATA_OBJECT, nDataSize, pfConstructor, pfDestructor);
}

BOOL ApmObject::SetMaxObject(INT32 lCount)
{
	return m_clObjects.SetCount(lCount);
}

ApdObject * ApmObject::CreateObject()
{
	ApdObject*	pcsObject = (ApdObject *) CreateModuleData(APMOBJECT_DATA_OBJECT);
	ASSERT( pcsObject );
	if( !pcsObject )		return NULL;

	pcsObject->Init();

	return pcsObject;
}

BOOL ApmObject::DestroyObject(ApdObject *pcsObject)
{
	pcsObject->m_Mutex.WLock();
	pcsObject->m_Mutex.Destroy();

	OcTreeIDList*	cur_list = pcsObject->m_listOcTreeID;
	OcTreeIDList*	del_list;
	while(cur_list)
	{
		del_list = cur_list;
		cur_list = cur_list->next;

		delete del_list;
	}

	return DestroyModuleData( pcsObject, APMOBJECT_DATA_OBJECT );
}

ApdObject * ApmObject::GetObject(INT32 lOID)
{
	ApdObject** ppcsObject = (ApdObject**)m_clObjects.GetObject(lOID);
	return ppcsObject ? *ppcsObject : NULL;
}

ApdObject *ApmObject::AddObject( INT32 lOID, INT32 lTID )
{
	ApdObject* pcsObject = CreateObject();
	if( !pcsObject )		return NULL;
	pcsObject->m_lID			= lOID;
	pcsObject->m_lTID			= lTID;
	pcsObject->m_pcsTemplate	= GetObjectTemplate( lTID );
	ASSERT( pcsObject->m_pcsTemplate );
	pcsObject->m_nBlockInfo		= pcsObject->m_pcsTemplate->m_nBlockInfo;
	pcsObject->m_nObjectType	= pcsObject->m_pcsTemplate->m_nObjectType;
	memcpy(	pcsObject->m_astBlockInfo, pcsObject->m_pcsTemplate->m_astBlockInfo, sizeof(AuBLOCKING) * APDOBJECT_MAX_BLOCK_INFO );

	if ( !AddObject( pcsObject ) )		//��ϵ� callback... ����..
	{
		DestroyObject( pcsObject );
		return NULL;		
	}

	return pcsObject;
}

ApdObject * ApmObject::AddObject( ApdObject* pcsObject )
{
	if( !m_clObjects.AddObject( &pcsObject, pcsObject->m_lID ) )
		return NULL;

	if ( m_pcsApmMap && m_pcsApmMap->AddObject( pcsObject->m_nDimension , pcsObject->m_stPosition, pcsObject->m_lID))
		pcsObject->m_bAddedToWorld = TRUE;

	// �߰��ȴ�..
	if ( !EnumCallback( APMOBJECT_CB_ON_ADD_OBJECT , pcsObject , NULL ) )
	{
		ASSERT( !"ApmObject::AddObject() EnumReadCallback Failure !!!" );
		MD_SetErrorMessage( "ApmObject::AddObject() EnumReadCallback Failure" );
		return pcsObject;
	}

	return pcsObject;
}

BOOL ApmObject::DeleteObject(AuBOX bbox, BOOL bForce, BOOL bImmediate)	// BBox �����ϰ� �׾ȿ� �ִ� �༮�� ����Ʈ..
{
	INT32 lIndex = 0;
	for( ApdObject* pcsObject = GetObjectSequence(&lIndex); pcsObject; pcsObject = GetObjectSequence(&lIndex) )
		if( AuIsInBBoxPlane( bbox , pcsObject->m_stPosition ) )
			DeleteObject(pcsObject, bForce, bImmediate);

	return TRUE;
}

BOOL ApmObject::DeleteObject(INT32 lOID, BOOL bForce, BOOL bImmediate)
{
	return DeleteObject( GetObject(lOID), bForce, bImmediate );
}

BOOL ApmObject::DeleteObject(ApWorldSector * pSector, BOOL bForce, BOOL bImmediate)
{
	// ���� �����ȿ� ������Ʈ�� �����.
	ASSERT( pSector );
	if( !pSector )		return FALSE;

	ApWorldSector::Dimension* pDimension = pSector->GetDimension( 0 );
	if( !pDimension )	return TRUE;

	ApAutoWriterLock	csLock( pDimension->lockObjects );

	ApWorldSector::IdPos * pObject = pDimension->pObjects;
	ApWorldSector::IdPos * pObjectNext;

	ApdObject** ppcsObject;
	
	while( pObject )
	{
		pObjectNext = pObject->pNext;

		ppcsObject = (ApdObject**) m_clObjects.GetObject(pObject->id);

		if( ppcsObject )
			DeleteObject( *ppcsObject, bForce, bImmediate );

		pObject = pObjectNext;
	}

	pDimension->pObjects = NULL;

	return TRUE;
}

BOOL ApmObject::DeleteObject(ApdObject *pcsObject, BOOL bForce, BOOL bImmediate)
{
	PROFILE("ApmObject::DeleteObject");

	if ( !pcsObject )
		return FALSE;

	EnumCallback( APMOBJECT_CB_ID_REMOVE, pcsObject, NULL );

	if( m_pcsApmMap && pcsObject->m_bAddedToWorld )
		m_pcsApmMap->DeleteObject( pcsObject->m_nDimension, pcsObject->m_stPosition, pcsObject->m_lID );

	m_clObjects.RemoveObject( pcsObject->m_lID );

	if( m_csAdminRemove.GetCount() > 0 && !bImmediate )
		return AddRemoveObject(pcsObject);
	else
		return DestroyObject(pcsObject);

	return TRUE;
}

BOOL ApmObject::DeleteAllObject( BOOL bForce, BOOL bImmediate  )
{
	INT32 lIndex = 0;
	for( ApdObject* pcsObject = GetObjectSequence(&lIndex); pcsObject; pcsObject = GetObjectSequence(&lIndex) )
		DeleteObject( pcsObject, bForce, bImmediate );

	return TRUE;
}

//---------------------------- ApaObjectTemplate Manager -------------------------
ApdObjectTemplate * ApmObject::GetObjectTemplateSequence(INT32 *plIndex)
{
	ApdObjectTemplate **pcsTemplate = (ApdObjectTemplate **) m_clObjectTemplates.GetObjectSequence(plIndex);
	return pcsTemplate ? *pcsTemplate : NULL;
}

INT16 ApmObject::AttachObjectTemplateData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor)
{
	return SetAttachedModuleData(pClass, APMOBJECT_DATA_OBJECT_TEMPLATE, nDataSize, pfConstructor, pfDestructor);
}

BOOL ApmObject::SetMaxObjectTemplate(INT32 lCount)
{
	return m_clObjectTemplates.SetCount(lCount);
}

ApdObjectTemplate * ApmObject::CreateTemplate()
{
	ApdObjectTemplate*	pcsTemplate = (ApdObjectTemplate*) CreateModuleData(APMOBJECT_DATA_OBJECT_TEMPLATE);
	if ( !pcsTemplate )		return NULL;

	pcsTemplate->Init();

	return pcsTemplate;
}

BOOL ApmObject::DestroyTemplate(ApdObjectTemplate *pcsTemplate)
{
	pcsTemplate->m_Mutex.Destroy();

	return DestroyModuleData( pcsTemplate, APMOBJECT_DATA_OBJECT_TEMPLATE );
}

BOOL ApmObject::DestroyAllTemplate()
{
	INT32 lIndex = 0;
	for( ApdObjectTemplate *pcsTemplate = GetObjectTemplateSequence(&lIndex); pcsTemplate; pcsTemplate = GetObjectTemplateSequence(&lIndex) )
		if( !DestroyTemplate( pcsTemplate ) )
			return FALSE;
		
	m_clObjectTemplates.RemoveObjectAll();

	return TRUE;
}

ApdObjectTemplate * ApmObject::GetObjectTemplate(INT32 lTID)
{
	ApdObjectTemplate **ppcsTemplate = (ApdObjectTemplate **) m_clObjectTemplates.GetObject(lTID);
	return ppcsTemplate ? *ppcsTemplate : NULL;
}

ApdObjectTemplate* ApmObject::AddObjectTemplate(INT32 lTID)
{
	ApdObjectTemplate* pcsTemplate = CreateTemplate();
	if( !pcsTemplate )		return NULL;

	pcsTemplate->m_lID = lTID;

	if( !m_clObjectTemplates.AddObject(&pcsTemplate, lTID) )
	{
		DestroyTemplate(pcsTemplate);
		return NULL;
	}

	return pcsTemplate;
}

BOOL ApmObject::DeleteObjectTemplate(INT32 lTID)
{
	ApdObjectTemplate** ppcsTemplate = (ApdObjectTemplate **) m_clObjectTemplates.GetObject(lTID);
	if ( !ppcsTemplate )		return FALSE;

	ApdObjectTemplate* pFindTemplate = *ppcsTemplate;
	
	// �ش� ���ø� ������Ʈ�� ��˶� ������.

	INT32 nCount	= 0	;
	INT32 lIndex = 0;
	for( ApdObject* pcsObject = GetObjectSequence(&lIndex); pcsObject; pcsObject = GetObjectSequence(&lIndex) )
	{
		if( pcsObject->m_pcsTemplate->m_lID == pFindTemplate->m_lID )
		{
			DeleteObject( pcsObject, TRUE, TRUE );
			++nCount;
		}
	}

	TRACE( "������Ʈ %d�� �������..\n" , nCount );

	m_clObjectTemplates.RemoveObject( lTID );
	DestroyTemplate( pFindTemplate );

	return TRUE;
}

BOOL ApmObject::StreamReadTemplate( const CHAR *szFile, ProgressCallback pfCallback , void * pData, CHAR *pszErrorMessage, BOOL bDecryption )
{
	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	ApdObjectTemplate	*pcsApdObjectTemplate;

	// szFile�� �д´�.
	csStream.Open(szFile, 0, bDecryption);

	nNumKeys = csStream.GetNumSections();

	ASSERT( nNumKeys <= m_clObjectTemplates.GetCount() );

	static	char	strMessage[] = "������Ʈ ���ø� �ε�";

	// �� Section�� ���ؼ�...
	for (i = 0; i < nNumKeys; ++i)
	{
		if( pfCallback ) pfCallback( strMessage , i + 1 , nNumKeys , pData );

		// Template�� �߰��Ѵ�.
		pcsApdObjectTemplate = AddObjectTemplate(atoi(csStream.ReadSectionName(i)));
		if (!pcsApdObjectTemplate)
		{
//			TRACEFILE(ALEF_ERROR_FILENAME, "StreamReadTemplate , AddObjectTemplate Fail" );
			ASSERT(!"ApmObject::StreamReadTemplate() AddObjectTemplate Failure");
			return FALSE;
		}

		// Stream Enumerate�� �Ѵ�. (ApModuleStream���� Ini File�� �����鼭 ������ Module�� Callback���ش�.
		if (!csStream.EnumReadCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, pcsApdObjectTemplate, this))
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s�� �߸��Ǿ����ϴ�!", pcsApdObjectTemplate->m_lID, pcsApdObjectTemplate->m_szName);

//			TRACEFILE(ALEF_ERROR_FILENAME, "StreamReadTemplate , EnumReadCallback Fail" );
			ASSERT(!"ApmObject::StreamReadTemplate() EnumReadCallback Failure !!!");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL ApmObject::StreamWriteTemplate( const CHAR *szFile, BOOL bEncryption )
{
	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szTID[32];
	ApdObjectTemplate	*pcsApdObjectTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if( bEncryption )	csStream.SetType(AUINIMANAGER_TYPE_KEYINDEX);

	FILE* pStream = fopen( "c:\\changeOTID.txt", "w+t" );

	// ��ϵ� ��� Object Template�� ���ؼ�...
	for (pcsApdObjectTemplate = GetObjectTemplateSequence(&lIndex); pcsApdObjectTemplate; pcsApdObjectTemplate = GetObjectTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsApdObjectTemplate->m_lID);

		// TID�� Section�� �����ϰ�
		csStream.SetSection(szTID);

		// Stream Enumerate �Ѵ�.
		if (!csStream.EnumWriteCallback(APMOBJECT_DATA_OBJECT_TEMPLATE, pcsApdObjectTemplate, this))
		{
			ASSERT(!"ApmObject::StreamWriteTemplate() EnumWriteCallback Failure !!!");
			return FALSE;
		}
	}

	fclose( pStream );

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL ApmObject::StreamRead( const CHAR *szFile, BOOL bDecryption , ApModuleDefaultCallBack pCheckCallback , BOOL bIndexUpdate)
{
	BOOL	bLogFile = FALSE;
	{
		// Full Processing Mode���� ������Ʈ �ε� ���п� ���� �α׸� �����.
		switch( g_eMiniDumpType )
		{
		case MiniDumpWithFullMemory:	bLogFile = TRUE;	break;
		case MiniDumpNormal:
		default:											break;
		}
	}

	// �������� ������ bIndexUpdate = TRUE �ؾ���..
	m_bUseObjectIndexUpdate = bIndexUpdate;

	ApModuleStream		csStream		;
	UINT16				nNumKeys		;
	INT32				i				;
	ApdObject			*pcsApdObject	;

	// szFile�� �д´�.
	if (!csStream.Open(szFile, 0, bDecryption)) {
		return TRUE;
	}

	nNumKeys = csStream.GetNumSections();

	if (nNumKeys == 0)
	{
		// debug message, kelovon, 20051005, kelovon
		printf("No objects in: %s\r\n", szFile);
		return TRUE;
	}
		
	printf("Loading %d objects from %s\r\n", nNumKeys, szFile);

	// �� Section�� ���ؼ�...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Object�� �����. 
		pcsApdObject = CreateObject();
		if (!pcsApdObject)
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : CreateObject Failed", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			m_bUseObjectIndexUpdate = FALSE;
			return NULL;
		}

		// OID�� �����ؼ�
		pcsApdObject->m_lID = atoi(csStream.ReadSectionName(i));

		// Stream Enumerate �Ѵ�.
		if (!csStream.EnumReadCallback(APMOBJECT_DATA_OBJECT, pcsApdObject, this))
		{
			//, �ش� �����Ʈ�� �ε����� ����. �α�ȭ�� Ȯ���غ�����!
//			ASSERT(!"ApmObject::StreamRead() EnumReadCallback Failure !!!");

			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : Enum Callback Failed", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			// ������Ʈ ��Ʈ���� �ع�����..
			// ��� ����..
			DestroyObject(pcsApdObject);
			continue;

			//return FALSE;
		}

		if (!pcsApdObject->m_pcsTemplate)
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : Template is NULL", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}
			DestroyObject(pcsApdObject);
			continue;
		}

		// ���� �˻��۾�..
		if( pCheckCallback )
		{
			if( pCheckCallback( ( void * ) pcsApdObject , ( void * ) szFile , NULL ) )
			{
				// do nothing..
			}
			else
			{
				if( bLogFile )
				{
					char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : pCheckCallback Failed", szFile , pcsApdObject->m_lID);
					AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
				}
				// ���� �����̸� �̰� �߰����� ����..
				DestroyObject(pcsApdObject);
				continue;
			}
		}

		UpdateInit(pcsApdObject);

		// ���������� Add

		if( !AddObject( pcsApdObject ) )
		{
			if( bLogFile )
			{
				switch( GetLastError() )
				{
				case	AEC_PARAMETER_ERROR			:	
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_PARAMETER_ERROR", szFile , pcsApdObject->m_lID);
						AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
						break;
					}
				case	AEC_KEY_ALEREADY_EXIST		:	
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_KEY_ALEREADY_EXIST", szFile , pcsApdObject->m_lID);
						AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
						break;
					}
				case	AEC_INSERT_FAILED			:	
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_INSERT_FAILED", szFile , pcsApdObject->m_lID);
						AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
						break;
					}
				case	AEC_OBJECT_COUNT_LIMITED	:	
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_OBJECT_COUNT_LIMITED", szFile , pcsApdObject->m_lID);
						AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
						break;
					}
				default:
				case	AEC_UNKNOWN					:	
					{
						char strCharBuff[256] = { 0, };
						sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_UNKNOWN", szFile , pcsApdObject->m_lID);
						AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
						break;
					}
				}
			}

			switch( GetLastError() )
			{
			case	AEC_PARAMETER_ERROR			:	MD_SetErrorMessage( "AEC_PARAMETER_ERROR"		) ;	break;
			case	AEC_KEY_ALEREADY_EXIST		:	MD_SetErrorMessage( "AEC_KEY_ALEREADY_EXIST"	) ;	break;
			case	AEC_INSERT_FAILED			:	MD_SetErrorMessage( "AEC_INSERT_FAILED"			) ;	break;
			case	AEC_OBJECT_COUNT_LIMITED	:	MD_SetErrorMessage( "AEC_OBJECT_COUNT_LIMITED"	) ;
				{
					DestroyObject(pcsApdObject);
					return FALSE;
				}
			default:
			case	AEC_UNKNOWN					:	MD_SetErrorMessage( "AEC_UNKNOWN"				) ;
				{
					DestroyObject(pcsApdObject);
					return FALSE;
				}
			}

			MD_SetErrorMessage( "�� �����߻����� ������Ʈ�ε��� ����� �̷������ �ʾҾ��!" );
		}
	}

	m_bUseObjectIndexUpdate = FALSE;

	return TRUE;
}

BOOL	ApmObject::StreamWrite( const CHAR *szFile	, BOOL bEncryption , ApModuleDefaultCallBack pCheckCallback )
{
	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szOID[32];
	ApdObject *			pcsApdObject;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// ��ϵ� ��� Object�� ���ؼ�...
	for (pcsApdObject = GetObjectSequence(&lIndex); pcsApdObject; pcsApdObject = GetObjectSequence(&lIndex))
	{
		if(
				( pCheckCallback && pCheckCallback( ( void * ) pcsApdObject , NULL , NULL )	)	||
				( NULL == pCheckCallback )
		)
		{
			sprintf(szOID, "%d", pcsApdObject->m_lID);

			// OID�� Section�� �����ϰ�
			csStream.SetSection(szOID);

			// Stream Enumerate �Ѵ�.
			if (!csStream.EnumWriteCallback(APMOBJECT_DATA_OBJECT, pcsApdObject, this))
			{
				ASSERT(!"ApmObject::StreamWrite() EnumWriteCallback Failure !!!");
				return FALSE;
			}
		}
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL ApmObject::StreamWrite(const CHAR *szFile, BOOL bStatic, BOOL bEncryption)
{
	ApModuleStream		csStream;
	INT32				lIndex = 0;
	CHAR				szOID[32];
	ApdObject *			pcsApdObject;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// ��ϵ� ��� Object�� ���ؼ�...
	for (pcsApdObject = GetObjectSequence(&lIndex); pcsApdObject; pcsApdObject = GetObjectSequence(&lIndex))
	{
		sprintf(szOID, "%d", pcsApdObject->m_lID);

		// OID�� Section�� �����ϰ�
		csStream.SetSection(szOID);

		// Stream Enumerate �Ѵ�.
		if (!csStream.EnumWriteCallback(APMOBJECT_DATA_OBJECT, pcsApdObject, this))
		{
			ASSERT(!"ApmObject::StreamWrite() EnumWriteCallback Failure !!!");
			return FALSE;
		}
	}

	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL ApmObject::ObjectTemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	const CHAR *szValueName;
	CHAR szValue[256];
//	UINT32 ulTemp1, ulTemp2;
	ApmObject *pThis = (ApmObject *) pClass;
	ApdObjectTemplate *pcsApdObjectTemplate = (ApdObjectTemplate *) pData;

	// ���� Value�� ���������� �� �׸� �´� ���� �д´�.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, APMOBJECT_INI_NAME_NAME))
		{
			pStream->GetValue(pcsApdObjectTemplate->m_szName, APDOBJECT_MAX_OBJECT_NAME);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_OBJECTTYPE, strlen(APMOBJECT_INI_NAME_OBJECTTYPE)))
		{
			pStream->GetValue( &pcsApdObjectTemplate->m_nObjectType );
		}		
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_BTYPE, strlen(APMOBJECT_INI_NAME_BTYPE)))
		{
			pStream->GetValue(szValue, 64);

			if (pcsApdObjectTemplate->m_nBlockInfo >= APDOBJECT_MAX_BLOCK_INFO)
				return FALSE;

			pcsApdObjectTemplate->m_nBlockInfo += 1;

			if (!strcmp(szValue, APMOBJECT_INI_NAME_BOX))
				pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_BOX;
			else if (!strcmp(szValue, APMOBJECT_INI_NAME_SPHERE))
				pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_SPHERE;
			else if (!strcmp(szValue, APMOBJECT_INI_NAME_CYLINDER))
				pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_CYLINDER;
//			else if ( !strcmp(szValue, APMOBJECT_INI_NAME_MINBOX))
//				pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_MINBOX;
			else
				pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type = AUBLOCKING_TYPE_NONE;
		}
		else if (	!strncmp(szValueName, APMOBJECT_INI_NAME_BOX_INF, strlen(APMOBJECT_INI_NAME_BOX_INF))
					&& (		(pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)	)	)
//							||	(pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_MINBOX)	)	)
		{
			pStream->GetValue(szValue, 64);

			pThis->ParsePOS(szValue, &pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.box.inf);
		}
		else if (	!strncmp(szValueName, APMOBJECT_INI_NAME_BOX_SUP, strlen(APMOBJECT_INI_NAME_BOX_SUP))
					&& (		(pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)	)	)
//							|| (pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_MINBOX)	)	)
		{
			pStream->GetValue(szValue, 64);

			pThis->ParsePOS(szValue, &pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.box.sup);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_SPHERE_CENTER, strlen(APMOBJECT_INI_NAME_SPHERE_CENTER)) && pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.sphere.center);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_SPHERE_RADIUS, strlen(APMOBJECT_INI_NAME_SPHERE_RADIUS)) && pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(&pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.sphere.radius);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_CENTER, strlen(APMOBJECT_INI_NAME_CYLINDER_CENTER)) && pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.cylinder.center);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_HEIGHT, strlen(APMOBJECT_INI_NAME_CYLINDER_HEIGHT)) && pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.cylinder.height);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_RADIUS, strlen(APMOBJECT_INI_NAME_CYLINDER_RADIUS)) && pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsApdObjectTemplate->m_astBlockInfo[pcsApdObjectTemplate->m_nBlockInfo - 1].data.cylinder.radius);
		}
	}

	return TRUE;
}


BOOL ApmObject::ObjectTemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	ApdObjectTemplate *	pcsApdObjectTemplate = (ApdObjectTemplate *) pData;

	// Object Template�� ������ Write�Ѵ�.
	if (!pStream->WriteValue(APMOBJECT_INI_NAME_NAME, pcsApdObjectTemplate->m_szName))
		return FALSE;

	if (!pStream->WriteValue(APMOBJECT_INI_NAME_OBJECTTYPE, pcsApdObjectTemplate->m_nObjectType ))
		return FALSE;

	return TRUE;
}

BOOL ApmObject::ObjectReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	const CHAR *szValueName;
	CHAR szValue[128];
	ApmObject *pThis = (ApmObject *) pClass;
	ApdObject *pcsApdObject = (ApdObject *) pData;

	// ���� Value�� ���������� �� �׸� �´� ���� �д´�.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strcmp(szValueName, APMOBJECT_INI_NAME_TID))
		{
			pStream->GetValue((INT32 *) &pcsApdObject->m_lTID);

			pcsApdObject->m_lTID		= pThis->GetTemplateReplaceIndex( pcsApdObject->m_lTID );
			pcsApdObject->m_pcsTemplate	= pThis->GetObjectTemplate(pcsApdObject->m_lTID);

			if ( !pcsApdObject->m_pcsTemplate )
				return FALSE;
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_OBJECTTYPE, strlen(APMOBJECT_INI_NAME_OBJECTTYPE)))
		{
			pStream->GetValue( &pcsApdObject->m_nObjectType );
		}		
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_BOX_INF, strlen(APMOBJECT_INI_NAME_BOX_INF)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.box.inf);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_BOX_SUP, strlen(APMOBJECT_INI_NAME_BOX_SUP)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_BOX)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.box.sup);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_SPHERE_CENTER, strlen(APMOBJECT_INI_NAME_SPHERE_CENTER)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.sphere.center);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_SPHERE_RADIUS, strlen(APMOBJECT_INI_NAME_SPHERE_RADIUS)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_SPHERE)
		{
			pStream->GetValue(&pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.sphere.radius);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_CENTER, strlen(APMOBJECT_INI_NAME_CYLINDER_CENTER)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.cylinder.center);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_HEIGHT, strlen(APMOBJECT_INI_NAME_CYLINDER_HEIGHT)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.cylinder.height);
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_CYLINDER_RADIUS, strlen(APMOBJECT_INI_NAME_CYLINDER_RADIUS)) && pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].type == AUBLOCKING_TYPE_CYLINDER)
		{
			pStream->GetValue(&pcsApdObject->m_astBlockInfo[pcsApdObject->m_nBlockInfo - 1].data.cylinder.radius);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_SCALE))
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_stScale);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_POSITION))
		{
			pStream->GetValue(szValue, 128);

			pThis->ParsePOS(szValue, &pcsApdObject->m_stPosition);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_DEGREE_X))		// ȸ�� �� �б�..
		{
			pStream->GetValue(&pcsApdObject->m_fDegreeX );
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_DEGREE_Y))
		{
			pStream->GetValue(&pcsApdObject->m_fDegreeY );
		}

		// ������ (2003-04-09 ���� 2:50:39) : �������� ȣ�ϼ��� ���� �ڵ�..
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_AXIS))
		{
			AuPOS	pos;

			// �׳� �о��ֱ⸸ ��...
			pStream->GetValue(szValue, 128);
			pThis->ParsePOS(szValue, &pos );

			// ���̰� ������..
			pcsApdObject->m_fDegreeX	= 0.0f;
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_DEGREE))
		{
			// �����ڵ�.. ������ ȸ�� ����.. Y�� ȸ�� ���� �ٷ��..

			pStream->GetValue(&pcsApdObject->m_fDegreeY);
		}
		else if (!strcmp(szValueName, APMOBJECT_INI_NAME_OCTREE_IDNUM))
		{
			pStream->GetValue(&pcsApdObject->m_nOcTreeID);
			pcsApdObject->m_listOcTreeID = NULL;
		}
		else if (!strncmp(szValueName, APMOBJECT_INI_NAME_OCTREE_IDDATA,strlen(APMOBJECT_INI_NAME_OCTREE_IDDATA)))
		{
			OcTreeIDList*	add_ID = new OcTreeIDList;
						
			pStream->GetValue(szValue, 128);
			sscanf(szValue, "%d,%d,%d", &add_ID->six,&add_ID->siz,&add_ID->ID);
	
			add_ID->next = pcsApdObject->m_listOcTreeID;
			pcsApdObject->m_listOcTreeID = add_ID;	
		}
	}

	AuMath::MatrixRotateX(&pcsApdObject->m_stMatrix, pcsApdObject->m_fDegreeX );
	AuMath::MatrixRotateY(&pcsApdObject->m_stMatrix, pcsApdObject->m_fDegreeY );

	if( pThis->IsUseIndexUpdate() )
	{
		INT32 nDivisionIndex	= GetDivisionIndexF( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z	);
		pcsApdObject->m_lID	= pThis->GetEmptyIndex( nDivisionIndex );
		if( pcsApdObject->m_lID == -1 )
		{
			MD_SetErrorMessage( "�ش����� ���ͳ��� ������Ʈ ���� �ʰ�����.. " );
			return FALSE;
		}
	}

	return TRUE;

}

BOOL ApmObject::ObjectWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	CHAR		szTemp[128];
	CHAR		szTemp2[128];
	ApdObject *	pcsApdObject = (ApdObject *) pData;
	INT32		nIndex;

	if (!pStream->WriteValue(APMOBJECT_INI_NAME_TID, (INT32) pcsApdObject->m_lTID))
		return FALSE;

	if (!pStream->WriteValue(APMOBJECT_INI_NAME_OBJECTTYPE, pcsApdObject->m_nObjectType ))
		return FALSE;

	INT32	lLength	= sprintf(szTemp, "%f,%f,%f", pcsApdObject->m_stScale.x, pcsApdObject->m_stScale.y, pcsApdObject->m_stScale.z);
	ASSERT(lLength < 128);
	if (!pStream->WriteValue(APMOBJECT_INI_NAME_SCALE, szTemp))
		return FALSE;

	lLength	= sprintf(szTemp, "%f,%f,%f", pcsApdObject->m_stPosition.x, pcsApdObject->m_stPosition.y, pcsApdObject->m_stPosition.z);
	ASSERT(lLength < 128);
	if (!pStream->WriteValue(APMOBJECT_INI_NAME_POSITION, szTemp))
		return FALSE;

	if (!pStream->WriteValue(APMOBJECT_INI_NAME_DEGREE_X, pcsApdObject->m_fDegreeX))
		return FALSE;
	if (!pStream->WriteValue(APMOBJECT_INI_NAME_DEGREE_Y, pcsApdObject->m_fDegreeY))
		return FALSE;

	// octree id list ����
	if (!pStream->WriteValue(APMOBJECT_INI_NAME_OCTREE_IDNUM, pcsApdObject->m_nOcTreeID))
		return FALSE;

	OcTreeIDList*	cur_ID = pcsApdObject->m_listOcTreeID;
	nIndex = 0;
	while(cur_ID)
	{
		lLength	= sprintf(szTemp2, "%s%d", APMOBJECT_INI_NAME_OCTREE_IDDATA,nIndex);
		ASSERT(lLength < 128);
		lLength	= sprintf(szTemp, "%d,%d,%d", cur_ID->six,cur_ID->siz,cur_ID->ID);
		ASSERT(lLength < 128);
		if (!pStream->WriteValue(szTemp2, szTemp))
			return FALSE;

		cur_ID = cur_ID->next;
		++nIndex;
	}

	return TRUE;
}

// ������ �߰�..
BOOL ApmObject::StreamWrite		( AuBOX bbox , INT32 lPartIndex , const CHAR *szFile , ApModuleDefaultCallBack pCheckCallback )	// BBox �����ϰ� �� �ȿ� �༮�鸸 �ͽ���Ʈ��.
{
	ApModuleStream		csStream	;
	INT32				lIndex = 0	;
	CHAR				szOID[32]	;
                                                                                          
	ApdObject *			pcsApdObject;
	//INT32				nIndexWidthDivision;


	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	csStream.SetType(APMODULE_STREAM_TYPE_PART_INDEX);

	// ��ϵ� ��� Object�� ���ؼ�...
	for (	pcsApdObject = GetObjectSequence(&lIndex)		;
			pcsApdObject									;
			pcsApdObject = GetObjectSequence(&lIndex)		)
	{
		INT32	lLength = sprintf(szOID, "%d", pcsApdObject->m_lID );	// �� �ε��� ����..
		ASSERT(lLength < 32);

		if( AuIsInBBoxPlane( bbox , pcsApdObject->m_stPosition )	&&
			(
				( pCheckCallback && pCheckCallback( ( void * ) pcsApdObject , NULL , NULL )	)	||
				( NULL == pCheckCallback )
			)
		)
		{
			// OID�� Section�� �����ϰ�
			csStream.SetSection(szOID);

			// Stream Enumerate �Ѵ�.
			if (!csStream.EnumWriteCallback(APMOBJECT_DATA_OBJECT, pcsApdObject, this))
			{
				ASSERT(!"ApmObject::StreamWrite() EnumWriteCallback Failure !!!");
				return FALSE;
			}
		}
	}

	csStream.Write(szFile, lPartIndex);
	
	return TRUE;
}

BOOL ApmObject::StreamRead		( AuBOX bbox , INT32 lPartIndex , const CHAR *szFile	)	// ���Ͽ��� ���� �ȿ� �༮�鸸 �鿩��.
{
	BOOL	bLogFile = FALSE;
	{
		// Full Processing Mode���� ������Ʈ �ε� ���п� ���� �α׸� �����.
		switch( g_eMiniDumpType )
		{
		case MiniDumpWithFullMemory:	bLogFile = TRUE;	break;
		case MiniDumpNormal:
		default:											break;
		}
	}

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	ApdObject			*pcsApdObject;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	csStream.SetType(APMODULE_STREAM_TYPE_PART_INDEX);

	// szFile�� �д´�.
	csStream.Open(szFile, lPartIndex);

	nNumKeys = csStream.GetNumSections();

	// �� Section�� ���ؼ�...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Object�� �����. 
		pcsApdObject = CreateObject();
		if (!pcsApdObject)
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : CreateObject Failed", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			EnumCallback(APMOBJECT_CB_ON_LOAD_ERROR, NULL, NULL);
			return NULL;
		}

		// OID�� �����ؼ�
		pcsApdObject->m_lID = atoi(csStream.ReadSectionName(i));

		// Stream Enumerate �Ѵ�.
		if (!csStream.EnumReadCallback(APMOBJECT_DATA_OBJECT, pcsApdObject, this))
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : Enum Callback Failed", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			EnumCallback(APMOBJECT_CB_ON_LOAD_ERROR, NULL, NULL);

			// ������Ʈ ��Ʈ���� �ع�����..
			// ��� ����..
			DestroyObject(pcsApdObject);

			continue;

		}
		
		if (!pcsApdObject->m_pcsTemplate)
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : Template is NULL", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			EnumCallback(APMOBJECT_CB_ON_LOAD_ERROR, NULL, NULL);
			DestroyObject(pcsApdObject);

			continue;
		}

		// ����üũ..
		if( AuIsInBBoxPlane( bbox , pcsApdObject->m_stPosition ) )
		{
			// ���������� Add
			if (!AddObject(pcsApdObject))
			{
				if( bLogFile )
				{
					switch( GetLastError() )
					{
					case	AEC_PARAMETER_ERROR			:	
						{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_PARAMETER_ERROR", szFile , pcsApdObject->m_lID);
							AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
							break;
						}
					case	AEC_KEY_ALEREADY_EXIST		:	
						{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_KEY_ALEREADY_EXIST", szFile , pcsApdObject->m_lID);
							AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
							break;
						}
					case	AEC_INSERT_FAILED			:	
						{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_INSERT_FAILED", szFile , pcsApdObject->m_lID);
							AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
							break;
						}
					case	AEC_OBJECT_COUNT_LIMITED	:	
						{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_OBJECT_COUNT_LIMITED", szFile , pcsApdObject->m_lID);
							AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
							break;
						}
					default:
					case	AEC_UNKNOWN					:	
						{
							char strCharBuff[256] = { 0, };
							sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d : AddObject AEC_UNKNOWN", szFile , pcsApdObject->m_lID);
							AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
							break;
						}
					}
				}

				BOOL bDeleteObjectNContinue = FALSE;
				EnumCallback(APMOBJECT_CB_ON_LOAD_ERROR, NULL, NULL);

				switch( GetLastError() )
				{
				case	AEC_PARAMETER_ERROR			:	MD_SetErrorMessage( "AEC_PARAMETER_ERROR"		) ;
														bDeleteObjectNContinue	= TRUE;
														break;
				case	AEC_KEY_ALEREADY_EXIST		:	// MD_SetErrorMessage( "AEC_KEY_ALEREADY_EXIST"	) ;
														{
															ApdObject			*pcsApdObject_Exist = 
																this->GetObject( pcsApdObject->m_lID );

															ASSERT( pcsApdObject_Exist );
															DestroyObject( pcsApdObject );
															pcsApdObject = pcsApdObject_Exist;
														}
														break;
				case	AEC_INSERT_FAILED			:	MD_SetErrorMessage( "AEC_INSERT_FAILED"			) ;
														bDeleteObjectNContinue	= TRUE;
														break;
				case	AEC_OBJECT_COUNT_LIMITED	:	MD_SetErrorMessage( "AEC_OBJECT_COUNT_LIMITED"	) ;
														#ifdef _DEBUG
														// ������ (2006-05-10 ���� 2:21:37) : 
														// �ø����� ����������..
														DebugBreak();
														#endif
														return FALSE;
				default:
				case	AEC_UNKNOWN					:	MD_SetErrorMessage( "AEC_UNKNOWN"				) ;	return FALSE;
				}

				if( bDeleteObjectNContinue )
				{
					DestroyObject( pcsApdObject );
					continue;
				}
			}
		}
		else
		{
			if( bLogFile )
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s OID=%d :Out of Range", szFile , pcsApdObject->m_lID);
				AuLogFile_s("LOG\\ObjectLoad.txt", strCharBuff);
			}

			EnumCallback(APMOBJECT_CB_ON_LOAD_ERROR, NULL, NULL);
			// ������ ���Ե��� �����Ƿ� �߰����� ����..
			DestroyObject( pcsApdObject );
			continue;
		}

		UpdateInit(pcsApdObject);

	}
	return TRUE;
}

BOOL	ApmObject::StreamRead	( ApWorldSector	* pSector 	)	// ���� �������� �ε���..
{
	ASSERT( NULL != pSector );
	if( NULL == pSector ) return FALSE;

	char	strFileName[ 256 ] , strFullPath[ 1024 ];
	wsprintf( strFileName , APMOBJECT_INI_FILE_NAME , 
		GetDivisionIndex( pSector->GetArrayIndexX() , pSector->GetArrayIndexZ() ) );
	wsprintf( strFullPath , "%s%s", m_strDir, strFileName );

	// ���� �̸� ���� ��..

	INT32	lPartIndex =	(pSector->GetArrayIndexZ() % MAP_DEFAULT_DEPTH) * MAP_DEFAULT_DEPTH +
							(pSector->GetArrayIndexX() % MAP_DEFAULT_DEPTH);

	AuBOX	bbox;
	bbox.inf.x	= pSector->GetXStart()	;
	bbox.inf.y	= 0.0f					;
	bbox.inf.z	= pSector->GetZStart()	;

	bbox.sup.x	= pSector->GetXEnd()	;
	bbox.sup.y	= 0.0f					;
	bbox.sup.z	= pSector->GetZEnd()	;

	if (!StreamRead( bbox , lPartIndex , strFullPath ))
		return FALSE;

	EnumCallback(APMOBJECT_CB_ON_LOAD_SECTOR, pSector, NULL);

	return TRUE;
}

BOOL ApmObject::UpdateStatus( INT32 lOID, INT32 lStatus )
{
	ApdObject* pcsObject = GetObject( lOID );
	if( !pcsObject )	return FALSE;

	AuAutoLock pLock(pcsObject->m_Mutex);

	INT32	lPrevStatus = pcsObject->m_lCurrentStatus;
	pcsObject->m_lCurrentStatus = lStatus;

	EnumCallback( APMOBJECT_CB_ID_UPDATE_STATUS, pcsObject, &lPrevStatus );

	return TRUE;
}

BOOL	ApmObject::StreamReadAllDivision( char * fmt , ApModuleDefaultCallBack pCheckCallback , BOOL bIndexUpdate )
{
	for( INT32 nZ = 0 ; nZ < MAP_WORLD_INDEX_HEIGHT / 16 ; ++ nZ )
	{
		for( INT32 nX = 0 ; nX < MAP_WORLD_INDEX_WIDTH / 16 ; ++ nX )
		{
			char	strFilename[ 256 ];
			wsprintf( strFilename, fmt,  ( nX * 100 + nZ )	);
			StreamRead( strFilename, FALSE, pCheckCallback , bIndexUpdate );
		}
	}

	return TRUE;
}

BOOL	ApmObject::StreamReadAllDivision()
{
	ASSERT( !"�̰� ������Ʈ�� ���� ����Ÿ�� �д� �ſ���~" );
	StreamReadAllDivision( APMOBJECT_LOCAL_INI_FILE_NAME_NORMAL );
	StreamReadAllDivision( APMOBJECT_LOCAL_INI_FILE_NAME_STATIC );

	return TRUE;
}

BOOL	ApmObject::StreamReadAllDivisionVersion2()
{
	StreamReadAllDivision( "Ini\\Object\\OBS\\" APMOBJECT_INI_SERVER_FILE_NAME );
	StreamReadAllDivision( APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN , NULL , TRUE );
	StreamReadAllDivision( APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN , NULL , TRUE );

	return TRUE;
}

BOOL	ApmObject::SetMaxObjectRemove(INT32 lCount)
{
	return m_csAdminRemove.SetCount(lCount);
}

BOOL	ApmObject::ProcessRemove(UINT32 ulClockCount)
{
	if( ulClockCount && m_ulPrevRemoveClockCount + AGPMOBJECT_PROCESS_REMOVE_INTERVAL > ulClockCount )
		return TRUE;

	INT32	lIndex	= 0;
	ApdObject** ppcsObject = (ApdObject **) m_csAdminRemove.GetObjectSequence(&lIndex);
	while( ppcsObject && *ppcsObject )
	{
		INT32	lRemovedOID	= 0;
		ApdObject* pcsObject = *ppcsObject;
		if( !ulClockCount || pcsObject->m_ulRemoveTimeMSec + AGPMOBJECT_PRESERVE_CHARACTER_DATA < ulClockCount )
		{
			lRemovedOID	= pcsObject->m_lID;	// ���� ��� ����Ÿ�� ������ ���� �Ǿ���.
			DestroyObject(pcsObject);
		}

		ppcsObject = (ApdObject **) m_csAdminRemove.GetObjectSequence(&lIndex);

		if( lRemovedOID != 0 )
			m_csAdminRemove.RemoveObject((INT_PTR) pcsObject);
	}

	m_ulPrevRemoveClockCount = ulClockCount;

	return TRUE;
}

BOOL	ApmObject::AddRemoveObject(ApdObject *pcsObject)
{
	if (!pcsObject)
		return FALSE;

	pcsObject->m_ulRemoveTimeMSec = GetClockCount();

	if( !m_csAdminRemove.AddObject(&pcsObject, (INT_PTR)pcsObject) )
	{
		ApdObject** ppcsObject = (ApdObject**) m_csAdminRemove.GetObject((INT_PTR) pcsObject);
		if( ppcsObject && *ppcsObject )
		{
			ASSERT(!"ApmObject::AddRemoveObject() Oh!!! NO");
			DestroyObject(*ppcsObject);
			m_csAdminRemove.RemoveObject((INT_PTR) (*ppcsObject));
		}

		return FALSE;
	}

	return TRUE;
}

BOOL ApmObject::UpdateInit( ApdObject *pcsObject )
{
	return EnumCallback(APMOBJECT_CB_ID_INIT, pcsObject, pcsObject->m_pcsTemplate);
}

BOOL ApmObject::SetCallbackInitObject( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(APMOBJECT_CB_ID_INIT, pfCallback, pClass);
}

BOOL ApmObject::SetCallbackRemoveObject( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(APMOBJECT_CB_ID_REMOVE, pfCallback, pClass);
}

BOOL ApmObject::SetCallbackUpdateStatus( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback(APMOBJECT_CB_ID_UPDATE_STATUS, pfCallback, pClass);
}

BOOL ApmObject::SetCallbackAddObject	( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback(APMOBJECT_CB_ON_ADD_OBJECT, pfCallback, pClass);
}

BOOL ApmObject::SetCallbackLoadSector	( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback(APMOBJECT_CB_ON_LOAD_SECTOR, pfCallback, pClass);
}

BOOL ApmObject::SetCallbackLoadError	( ApModuleDefaultCallBack pfCallback, PVOID pClass	)
{
	return SetCallback(APMOBJECT_CB_ON_LOAD_ERROR, pfCallback, pClass);
}


BOOL ApmObject::ParsePOS(const CHAR *szPoint, AuPOS *pstPos)
{
	sscanf(szPoint, "%f,%f,%f", &pstPos->x, &pstPos->y, &pstPos->z);
	return TRUE;
}

INT32	__AddDivisionIndex	( INT32 iid , INT32 nDivisionIndex )
{
	// 0xffffffff ���� 
	ASSERT( nDivisionIndex	< 0xefff );
	ASSERT( iid				< 0xffff );

	return ( nDivisionIndex << 16 ) | ( iid & 0xffff );
}

INT32	ApmObject::GetEmptyIndex		( INT32 nDivisionIndex )
{
	for( int i = 1 ; i < 0xffff ; i ++ )
	{
		INT32 nIndex = __AddDivisionIndex( i , nDivisionIndex );
		if( !GetObject( nIndex ) )	return nIndex;
	}

	MD_SetErrorMessage( "������Ʈ ��������!" );

	return -1;// ��������!..
}