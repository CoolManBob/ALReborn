#include "AgcmCharacter.h"
#include "ApModuleStream.h"

#define AGCMCHAR_INI_NAME_LABEL						"LABEL"
#define AGCMCHAR_INI_NAME_DFF						"DFF"
#define AGCMCHAR_INI_NAME_DEF_ARM_DFF				"DEFAULT_ARMOUR_DFF"

#define AGCMCHAR_INI_NAME_ANIMATION_NAME			"ANIMATION_NAME"
#define AGCMCHAR_INI_NAME_BLENDING_ANIMATION_NAME	"B_ANIMATION_NAME"
#define AGCMCHAR_INI_NAME_ANIMATION_FLAGS			"ANIMATION_FLAGS"
#define AGCMCHAR_INI_NAME_ANIMATION_ACTIVE_RATE		"ANIMATION_ACTIVE_RATE"
#define AGCMCHAR_INI_NAME_ANIMATION_CUST_TYPE		"ANIMATION_CUST_TYPE"
#define AGCMCHAR_INI_NAME_ANIMATION_POINT			"ANIMATION_POINT"
#define AGCMCHAR_INI_NAME_ANIMATION_CUST_FLAGS		"ANIMATION_CUST_FLAGS"

#define AGCMCHAR_INI_NAME_SUB_ANIMATION_NAME		"SUB_ANIMATION_NAME"
/*#define AGCMCHAR_INI_NAME_SUB_ANIMATION_FLAGS		"SUB_ANIMATION_FLAGS"
#define AGCMCHAR_INI_NAME_SUB_ANIMATION_ACTIVE_RATE	"SUB_ANIMATION_ACTIVE_RATE"
#define AGCMCHAR_INI_NAME_SUB_ANIMATION_CUST_TYPE	"SUB_ANIMATION_CUST_TYPE"
#define AGCMCHAR_INI_NAME_SUB_ANIMATION_POINT		"SUB_ANIMATION_POINT"*/

#define AGCMCHAR_INI_NAME_ANIM_TYPE2				"ANIMATION_TYPE2"

#define AGCMCHAR_INI_NAME_LOD_LEVEL					"LOD_LEVEL"
#define AGCMCHAR_INI_NAME_LOD_DISTANCE				"LOD_DISTANCE"
#define AGCMCHAR_INI_NAME_LOD_HAS_BILLBOARD_NUM		"LOD_HAS_BILLBOARD"
#define AGCMCHAR_INI_NAME_LOD_BILLBOARD_INFO		"LOD_BILLBOARD_INFO"
#define AGCMCHAR_INI_NAME_HEIGHT					"HEIGHT"
#define AGCMCHAR_INI_NAME_DEPTH						"DEPTH"	//. 2005. 10. 07. Nonstopdj stream에 출력/입력할 depth value name추가
#define AGCMCHAR_INI_NAME_RIDER_HEIGHT				"RIDER_HEIGHT"
#define AGCMCHAR_INI_NAME_PICK_DFF					"PICK_DFF"
#define AGCMCHAR_INI_NAME_PRE_LIGHT					"PRE_LIGHT"
#define AGCMCHAR_INI_NAME_OBJECT_TYPE				"OBJECT_TYPE"
#define AGCMCHAR_INI_NAME_BOUNDING_SPHERE			"BOUNDING_SPHERE"
#define AGCMCHAR_INI_NAME_SCALE						"SCALE"
#define AGCMCHAR_INI_NAME_PICKNODE					"PICKNODE"

//@{ 2006/02/21 burumal 
// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP 에 정의된 값과 수를 일치시킬것
#define AGCMCHAR_INI_NAME_PICKNODE_2				"PICKNODE_2"
#define AGCMCHAR_INI_NAME_PICKNODE_3				"PICKNODE_3"
#define AGCMCHAR_INI_NAME_PICKNODE_4				"PICKNODE_4"
#define AGCMCHAR_INI_NAME_PICKNODE_5				"PICKNODE_5"
#define AGCMCHAR_INI_NAME_PICKNODE_6				"PICKNODE_6"
//@}

//@{ 2006/03/24 burumal
#define AGCMCHAR_INI_NAME_TAGGING					"TAGGING"
//@}

//@{ 2006/05/03 burumal
#define AGCMCHAR_INI_NAME_DNF_1						"DID_NOT_FINISH_KOREA"
#define AGCMCHAR_INI_NAME_DNF_2						"DID_NOT_FINISH_CHINA"
#define AGCMCHAR_INI_NAME_DNF_3						"DID_NOT_FINISH_WESTERN"
#define AGCMCHAR_INI_NAME_DNF_4						"DID_NOT_FINISH_JAPAN"
//@}

//@{ 2006/08/22 burumal
#define AGCMCHAR_INI_NAME_LOOKAT_NODE				"LOOK_AT_NODE"
//@}

//@{ 2006/08/28 burumal
#define AGCMCHAR_INI_NAME_USE_BENDING				"USE_BENDING"
#define AGCMCHAR_INI_NAME_BENDING_FACTOR			"BENDING_FACTOR"
//@}

//@{ 2006/12/15 burumal
#define AGCMCHAR_INI_NON_PICKING_TYPE				"NON_PICKING"
//@}

#define AGCMCHAR_INI_NAME_FACEATOMIC				"FACEATOMIC"

#define AGCMCHAR_INI_NAME_OCTREE_DATA				"OCTREE_DATA"

#define AGCMCHAR_INI_NAME_DEFAULT_FACE_NUM			"DEFAULT_FACE_NUM"
#define AGCMCHAR_INI_NAME_DEFAULT_FACE				"DEFAULT_FACE"

#define AGCMCHAR_INI_NAME_DEFAULT_HAIR_NUM			"DEFAULT_HAIR_NUM"
#define AGCMCHAR_INI_NAME_DEFAULT_HAIR				"DEFAULT_HAIR"

#define AGCMCHAR_INI_NAME_DEFAULT_FACE_RENDER_TYPE	"DEFAULT_RENDER_TYPE_FACE"
#define AGCMCHAR_INI_NAME_DEFAULT_HAIR_RENDER_TYPE	"DEFAULT_RENDER_TYPE_HAIR"

#define AGCMCHAR_INI_NAME_CUSTOMIZE_REVIEW			"CUSTOMIZE_REVIEW"

//@{ Jaewon 20050525
// A dirty hack for printing when exporting 
typedef void (*OutputExportLog)(const char *);
extern OutputExportLog _outputExportLog;
//@} Jaewon

BOOL AgcmCharacter::StreamReadTemplate( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
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
			TRACE( "AgcmCharacter::StreamReadTemplate 템플릿을 찾을 수 없습니다.\n" );
			TRACE( "pcsAgpdCharacterTemplate == NULL.\n" );

			if( pszErrorMessage ) {
				sprintf(pszErrorMessage, "ERROR:[TID : %d]가 잘못되었습니다!", lTID );
			}

			continue;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMCHAR_DATA_TYPE_TEMPLATE, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if(pszErrorMessage)
				sprintf(pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName);

			TRACE( "AgcmCharacter::StreamReadTemplate Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::StreamWriteTemplate(CHAR *szFile, BOOL bEncryption)
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
	for (pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex);
		pcsAgpdCharacterTemplate;
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_TEMPLATE, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgcmCharacter::StreamReadTemplateAnimation( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
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

		if( pcsAgpdCharacterTemplate == NULL )
		{
			if( pszErrorMessage ) {
				sprintf(pszErrorMessage, "ERROR:[TID : %d]가 잘못되었습니다!", lTID );
			}

			TRACE( "AgcmCharacter::StreamReadTemplate 템플릿을 찾을 수 없습니다.\n" );

			continue;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMCHAR_DATA_TYPE_ANIMATION, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if( pszErrorMessage )
				sprintf( pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName );

			TRACE( "AgcmCharacter::StreamReadTemplate Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::StreamWriteTemplateAnimation(CHAR *szFile, BOOL bEncryption)
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
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex) )
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_ANIMATION, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgcmCharacter::StreamReadTemplateCustomize( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
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
		if( pcsAgpdCharacterTemplate == NULL )
		{
			if( pszErrorMessage ) {
				sprintf(pszErrorMessage, "ERROR:[TID : %d]가 잘못되었습니다!", lTID );
			}

			TRACE( "AgcmCharacter::StreamReadTemplate 템플릿을 찾을 수 없습니다.\n" );

			continue;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if( csStream.EnumReadCallback( AGCMCHAR_DATA_TYPE_CUSTOMIZE, pcsAgpdCharacterTemplate, this ) == NULL )
		{
			if( pszErrorMessage )
				sprintf( pszErrorMessage, "ERROR:[TID : %d]%s가 잘못되었습니다!", pcsAgpdCharacterTemplate->m_lID, pcsAgpdCharacterTemplate->m_szTName );

			TRACE( "AgcmCharacter::StreamReadTemplate Callback returns FALSE!.\n" );
			return FALSE;
		}
	}

	return TRUE;
}

//. 2006. 6. 21. Nonstopdj
BOOL AgcmCharacter::StreamReadPolyMorphTable( CHAR *szFile, CHAR *pszErrorMessage, BOOL bDecryption )
{
	if (!szFile)
		return FALSE;

	AuExcelTxtLib	csExcelTxtLib;

	if(!csExcelTxtLib.OpenExcelFile(szFile, TRUE, bDecryption))
	{
		OutputDebugString("AgcmCharacter::StreamReadPolyMorphTable() Error (1) !!!\n");
		csExcelTxtLib.CloseFile();
		return FALSE;
	}

	const INT16				nKeyRow						= 0;
	const INT16				nTNameColumn				= 0;	
	char					*pszData					= NULL;

	for(INT16 nRow = 1; nRow < csExcelTxtLib.GetRow(); ++nRow)
	{
		pszData = csExcelTxtLib.GetData(nTNameColumn, nRow);
		if(!pszData)
		{
			continue;
		}

		int	iBaseTID = atoi(pszData);
		AgcdCharacterPolyMorphWidget* pPolyMorphData = new AgcdCharacterPolyMorphWidget;

		for(INT16 nCol = 1; nCol < csExcelTxtLib.GetColumn(); ++nCol)
		{
			pszData = csExcelTxtLib.GetData(nCol, nKeyRow);
			if(!pszData)
				continue;

			if(!strcmp(pszData, "Condition"))
			{
				pszData = csExcelTxtLib.GetData(nCol, nRow);
				pPolyMorphData->m_eConditionType = (AgcmCharacterPolymorphCondition)(atoi(pszData));
			}
			else if(!strcmp(pszData, "FirstTID") ||
				 !strcmp(pszData, "SecondTID") ||
				 !strcmp(pszData, "ThirdTID"))
			{
				pszData = csExcelTxtLib.GetData(nCol, nRow);
				
				if(pszData != NULL)
					pPolyMorphData->m_vecTID.push_back(atoi(pszData));
			}
		}

		//. 변형가능한 template type 지정.
		m_pcsAgpmCharacter->SetTypePolyMorph( m_pcsAgpmCharacter->GetCharacterTemplate(iBaseTID));
		m_PolyMorphTableMap.insert(std::pair<UINT32, AgcdCharacterPolyMorphWidget*>(iBaseTID, pPolyMorphData));

	}

	return TRUE;
}

BOOL AgcmCharacter::StreamWriteTemplateCustomize(CHAR *szFile, BOOL bEncryption)
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
	for (pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex); 
		pcsAgpdCharacterTemplate; 
		pcsAgpdCharacterTemplate = m_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
	{
		sprintf(szTID, "%d", pcsAgpdCharacterTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGCMCHAR_DATA_TYPE_CUSTOMIZE, pcsAgpdCharacterTemplate, this))
			return FALSE;
	}
	csStream.Write(szFile, 0, bEncryption);

	return TRUE;
}

BOOL AgcmCharacter::TemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);
	CHAR					szValue[256];
	CHAR					szTemp[128];

	INT32					lRt;
	UINT32					ulTemp1, ulTemp2, ulTemp3, ulTemp4;
	FLOAT					fTemp1, fTemp2, fTemp3, fTemp4;
	//	AgcdCharacterAnimationAttachedData	*pstCharAnimAttachedData;

	//@{ 2006/05/04 burumal
	pstAgcdCharacterTemplate->m_nDNF = 0;
	//@}

	//@{ 2006/12/15 burumal
	pstAgcdCharacterTemplate->m_bNonPickingType = FALSE;
	//@}

	while( pcsStream->ReadNextValue() )
	{
		if( pThis->m_pcsAgcmLODManager )
		{
			lRt = pThis->m_pcsAgcmLODManager->StreamRead(pcsStream, &pstAgcdCharacterTemplate->m_stLOD);
			if (lRt == E_AGCD_LOD_STREAM_READ_RESULT_READ)
				continue;
			else if (lRt == E_AGCD_LOD_STREAM_READ_RESULT_ERROR)
			{
				ASSERT(!"AgcmCharacter::TemplateStreamReadCB() LOD Stream Read !!!");
				return FALSE;
			}

			//>@ 캐릭터의 바운더리 일단 하드 코딩.. 2005.3.28 gemani
			AgcdLODList*	cur_LOD = pstAgcdCharacterTemplate->m_stLOD.m_pstList;
			while(cur_LOD)
			{
				cur_LOD->m_csData.m_ulBoundary = 5000;
				cur_LOD = cur_LOD->m_pstNext;
			}
			//<@
		}

		lRt = pThis->m_pcsAgcmRender->StreamReadClumpRenderType(pcsStream, &pstAgcdCharacterTemplate->m_csClumpRenderType);
		if (lRt == E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_READ)
			continue;
		else if (lRt == E_AGCM_CLUMP_RENDER_TYPE_STREAM_READ_RESULT_ERROR)
		{
			ASSERT(!"AgcmCharacter::TemplateStreamReadCB() RenderType Stream Read !!!");
			return FALSE;
		}

		if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_LABEL))
		{
			if (pstAgcdCharacterTemplate->m_pcsPreData)
			{
				pcsStream->GetValue(szTemp, 128);

				if (strcmp(szTemp, ""))
				{
					if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel)
					{
						ASSERT(!"pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel이 재정의 되었습니다.");

						free(pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel);
					}

					pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel	= new CHAR [strlen(szTemp) + 1];
					strcpy(pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel, szTemp);
				}
			}
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DFF))
		{
			//pcsStream->GetValue(pstAgcdCharacterTemplate->m_szDFFName, AGCDCHAR_DFF_NAME_LENGTH);
			pcsStream->GetValue(szTemp, 128);

			if (strcmp(szTemp, ""))
			{
				if (pstAgcdCharacterTemplate->m_pcsPreData)
				{
					if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName)
					{
						ASSERT(!"pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName 재정의 되었습니다.");

						free(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName);
					}

					pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName	= new CHAR [strlen(szTemp) + 1];
					strcpy(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName, szTemp);
				}
				else
				{
					ulTemp1	= atoi(szTemp);
					pstAgcdCharacterTemplate->SetClumpID(ulTemp1);
				}
			}
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEF_ARM_DFF))
		{
			//pcsStream->GetValue(pstAgcdCharacterTemplate->m_szDADFFName, AGCDCHAR_DFF_NAME_LENGTH);
			pcsStream->GetValue(szTemp, 128);

			if (strcmp(szTemp, ""))
			{
				if (pstAgcdCharacterTemplate->m_pcsPreData)
				{
					if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName)
					{
						ASSERT(!"pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName 재정의 되었습니다.");

						free(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName);
					}

					pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName	= new CHAR [strlen(szTemp) + 1];
					strcpy(pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName, szTemp);
				}
				else
				{
					ulTemp1	= atoi(szTemp);
					pstAgcdCharacterTemplate->SetDefArmourID(ulTemp1);
				}
			}
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_HEIGHT))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d", &pstAgcdCharacterTemplate->m_lHeight);
		}
		//. 2005. 10. 07. Nonstopdj
		//. DEPTH 읽어들이기
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEPTH))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%f", &pstAgcdCharacterTemplate->m_fDepth);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_RIDER_HEIGHT))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d", &pstAgcdCharacterTemplate->m_lRiderHeight);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICK_DFF))
		{
			pcsStream->GetValue(szTemp, 128);

			if (strcmp(szTemp, ""))
			{
				if (pstAgcdCharacterTemplate->m_pcsPreData)
				{
					if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName)
					{
						ASSERT(!"pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName 재정의 되었습니다.");

						free(pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName);
					}

					pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName	= new CHAR [strlen(szTemp) + 1];
					strcpy(pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName, szTemp);
				}
				else
				{
					ulTemp1	= atoi(szTemp);
					pstAgcdCharacterTemplate->SetPickClumpID(ulTemp1);
				}
			}

			//pcsStream->GetValue(pstAgcdCharacterTemplate->m_szPickDFFName, AGCDCHAR_DFF_NAME_LENGTH);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_OBJECT_TYPE))
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lObjectType);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PRE_LIGHT))
		{
			if (pstAgcdCharacterTemplate->m_pcsPreData)
			{
				pcsStream->GetValue(szValue, 256);

				sscanf(szValue, "%d %d %d %d", &ulTemp1, &ulTemp2, &ulTemp3, &ulTemp4);

				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.red	= ulTemp1;
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.green	= ulTemp2;
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.blue	= ulTemp3;
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.alpha	= ulTemp4;
				/*pstAgcdCharacterTemplate->m_stPreLight.red		= ulTemp1;
				pstAgcdCharacterTemplate->m_stPreLight.green	= ulTemp2;
				pstAgcdCharacterTemplate->m_stPreLight.blue		= ulTemp3;
				pstAgcdCharacterTemplate->m_stPreLight.alpha	= ulTemp4;*/
			}
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_BOUNDING_SPHERE))
		{
			pcsStream->GetValue(szValue, 256);

			sscanf(szValue, "%f:%f:%f:%f", &fTemp1, &fTemp2, &fTemp3, &fTemp4);

			pstAgcdCharacterTemplate->m_stBSphere.center.x	= fTemp1;
			pstAgcdCharacterTemplate->m_stBSphere.center.y	= fTemp2;
			pstAgcdCharacterTemplate->m_stBSphere.center.z	= fTemp3;
			pstAgcdCharacterTemplate->m_stBSphere.radius	= fTemp4;
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_SCALE))
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_fScale);
		}
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE))
		{			
			//@{ 2006/02/20 burumal
			//pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex);			
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[0]);
			//@}
		}
		//@{ 2006/02/21 burumal
		// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP 에 정의된 값과 수를 일치시킬것
		else 
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE_2) )
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[1]);
		}
		else 
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE_3) )
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[2]);
		}
		else 
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE_4) )
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[3]);
		}
		else 
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE_5) )
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[4]);
		}
		else 
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_PICKNODE_6) )
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lPickingNodeIndex[5]);
		}
		//@}
		//@{ 2006/03/24 burumal
		else
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_TAGGING) )
		{
			INT32 nTaggingValue = FALSE;
			pcsStream->GetValue(&nTaggingValue);
			pstAgcdCharacterTemplate->m_bTagging = (nTaggingValue > 0) ? TRUE : FALSE;
		}
		//@}
		//@{ 2006/08/22 burumal
		else
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_LOOKAT_NODE) )
		{
			INT32 lLookAtNode = 1;
			pcsStream->GetValue(&lLookAtNode);
			if ( lLookAtNode < 0 )
				lLookAtNode = 1;

			pstAgcdCharacterTemplate->m_lLookAtNode = lLookAtNode;
		}
		//@}
		//@{ 2006/08/28 burumal
		else
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_USE_BENDING) )
		{
			INT32 nBending = FALSE;
			pcsStream->GetValue(&nBending);
			pstAgcdCharacterTemplate->m_bUseBending = (nBending > 0) ? TRUE : FALSE;
		}
		else
		if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_BENDING_FACTOR) )
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%f", &pstAgcdCharacterTemplate->m_fBendingFactor);
		}
		//@}		
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_FACEATOMIC))
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lFaceAtomicIndex);
		}
		/* 마고자 삭제 , 쓰지 않는 값 2008/04/24
		else if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_OCTREE_DATA))		// add by gemani
		{
			pcsStream->GetValue(szValue, 256);

			FLOAT	afTemp[12];
			sscanf(szValue,
				"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
				afTemp + 0,
				afTemp + 1,
				afTemp + 2,
				afTemp + 3,
				afTemp + 4,
				afTemp + 5,
				afTemp + 6,
				afTemp + 7,
				afTemp + 8,
				afTemp + 9,
				afTemp + 10,
				afTemp + 11);

			// 위에 임시로 set 아직 파일에 값이 제대로 들어가 있지 않다..
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].x	= afTemp[0];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].y	= afTemp[1];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].z	= afTemp[2];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].x	= afTemp[3];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].y	= afTemp[4];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].z	= afTemp[5];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].x	= afTemp[6];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].y	= afTemp[7];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].z	= afTemp[8];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].x	= afTemp[9];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].y	= afTemp[10];
			//pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].z	= afTemp[11];
		}
		*/
		//@{ 2006/05/03 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DNF_1) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 1;

			if ( nDNF > 0 )
				pstAgcdCharacterTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DNF_2) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 2;

			if ( nDNF > 0 )
				pstAgcdCharacterTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DNF_3) )
		{			
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 3;

			if ( nDNF > 0 )
				pstAgcdCharacterTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		else if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DNF_4) )
		{
			INT32 nDNF = 0;
			pcsStream->GetValue(&nDNF);

			if ( nDNF != 0 )
				nDNF = 4;

			if ( nDNF > 0 )
				pstAgcdCharacterTemplate->m_nDNF |= (1 << (nDNF-1));
		}
		//@}		
		//@{ 2006/8/17 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGPMCHAR_INI_NAME_SWCO_BOX) )
		{
			pcsStream->GetValue(szValue, 256);

			sscanf(szValue, "%f:%f", &fTemp1, &fTemp2);

			if ( fTemp1 > 0.0f )
				pstAgpdCharacterTemplate->m_fSiegeWarCollBoxWidth = fTemp1;

			if ( fTemp2 > 0.0f )
				pstAgpdCharacterTemplate->m_fSiegeWarCollBoxHeight = fTemp2;
		}		
		else if ( !strcmp(pcsStream->GetValueName(), AGPMCHAR_INI_NAME_SWCO_SPHERE) )
		{
			FLOAT fValue;
			pcsStream->GetValue(&fValue);
			if ( fValue > 0.0f )
				pstAgpdCharacterTemplate->m_fSiegeWarCollSphereRadius = fValue;
		}
		//@}
		//@{ 2006/09/08 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGPMCHAR_INI_NAME_SELF_DESTRUCTION_ATTACK_TYPE) )
		{
			INT32 nSelfDest = FALSE;
			pcsStream->GetValue(&nSelfDest);
			pstAgpdCharacterTemplate->m_bSelfDestructionAttackType = (nSelfDest > 0) ? TRUE : FALSE;
		}
		//@}
		//@{ 2006/12/15 burumal
		else if ( !strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NON_PICKING_TYPE) )
		{
			INT32 nNonPickingType = FALSE;
			pcsStream->GetValue(&nNonPickingType);
			pstAgcdCharacterTemplate->m_bNonPickingType = (nNonPickingType > 0) ? TRUE : FALSE;
			if ( pstAgcdCharacterTemplate->m_bNonPickingType )
				pThis->SetNonPickingTID(pstAgpdCharacterTemplate->m_lID);
		}
		//@}
	}

	return TRUE;
}

BOOL AgcmCharacter::TemplatePreDataStreamWrite(ApModuleStream *pcsStream, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	CHAR				szValue[128];

	UINT32				ulIndex					= 0;

	if (pstAgcdCharacterTemplate->m_pcsPreData)
	{
		if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_LABEL, pstAgcdCharacterTemplate->m_pcsPreData->m_pszLabel);

		if (pstAgcdCharacterTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
		{
			sprintf(szValue, "%d %d %d %d",	pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.red,
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.green,
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.blue,
				pstAgcdCharacterTemplate->m_pcsPreData->m_stPreLight.alpha	);

			pcsStream->WriteValue(AGCMCHAR_INI_NAME_PRE_LIGHT, szValue);
		}

		if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_DFF, pstAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName);

		if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_DEF_ARM_DFF, pstAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName);

		if (pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICK_DFF, pstAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName);
	}
	else
	{
		ulIndex		= pstAgcdCharacterTemplate->GetClumpID();
		if (ulIndex > 0)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_DFF, (INT32)(ulIndex));

		ulIndex		= pstAgcdCharacterTemplate->GetDefArmourID();
		if (ulIndex > 0)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_DEF_ARM_DFF, (INT32)(ulIndex));

		ulIndex		= pstAgcdCharacterTemplate->GetPickClumpID();
		if (ulIndex > 0)
			pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICK_DFF, (INT32)(ulIndex));
	}

	return TRUE;
}

BOOL AgcmCharacter::TemplateAnimationAttachedDataStreamWrite(	ApModuleStream *pcsStream,
															 AgcdAnimData2 *pcsCurAnimData,
															 INT32 lAnimType,
															 INT32 lAnimEquip,
															 INT32 lAnimIndex				)
{
	CHAR				szName[128];
	CHAR				szValue[128];

	ACA_AttachedData	*pcsAnimAttachedData	=
		(ACA_AttachedData *)(m_csAnimation2.GetAttachedData(
		AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
		pcsCurAnimData										));
	if (!pcsAnimAttachedData)
		return TRUE; // skip!

	// 블랜딩 애니메이션.
	if (pcsAnimAttachedData->m_pcsBlendingData)
	{
		if (pcsAnimAttachedData->m_pcsBlendingData->m_pszRtAnimName)
		{
			sprintf(
				szValue,
				"%d:%d:%d:%s",
				lAnimType,
				lAnimEquip,
				lAnimIndex,
				pcsAnimAttachedData->m_pcsBlendingData->m_pszRtAnimName	);

			sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_BLENDING_ANIMATION_NAME, lAnimType, lAnimEquip, lAnimIndex);

			pcsStream->WriteValue(szName, szValue);
		}
	}

	// 서브 애니메이션.
	if (pcsAnimAttachedData->m_pcsSubData)
	{
		if (pcsAnimAttachedData->m_pcsSubData->m_pszRtAnimName)
		{
			sprintf(
				szValue,
				"%d:%d:%d:%d:%s",
				lAnimType,
				lAnimEquip,
				0,
				lAnimIndex,
				pcsAnimAttachedData->m_pcsSubData->m_pszRtAnimName	);

			sprintf(szName, "%s%d%d%d%d", AGCMCHAR_INI_NAME_SUB_ANIMATION_NAME, lAnimType, lAnimEquip, 0, lAnimIndex);

			pcsStream->WriteValue(szName, szValue);
		}
	}

	if (pcsAnimAttachedData->m_unActiveRate)
	{
		sprintf(
			szValue,
			"%d:%d:%d:%d",
			lAnimType,
			lAnimEquip,
			lAnimIndex,
			pcsAnimAttachedData->m_unActiveRate	);

		sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_ANIMATION_ACTIVE_RATE, lAnimType, lAnimEquip, lAnimIndex);

		pcsStream->WriteValue(szName, szValue);
	}

	if (pcsAnimAttachedData->m_nCustType)
	{
		sprintf(
			szValue,
			"%d:%d:%d:%d",
			lAnimType,
			lAnimEquip,
			lAnimIndex,
			pcsAnimAttachedData->m_nCustType	);

		sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_ANIMATION_CUST_TYPE, lAnimType, lAnimEquip, lAnimIndex);

		pcsStream->WriteValue(szName, szValue);
	}

	if (pcsAnimAttachedData->m_pszPoint)
	{
		sprintf(
			szValue,
			"%d:%d:%d:%s",
			lAnimType,
			lAnimEquip,
			lAnimIndex,
			pcsAnimAttachedData->m_pszPoint	);

		sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_ANIMATION_POINT, lAnimType, lAnimEquip, lAnimIndex);

		pcsStream->WriteValue(szName, szValue);
	}

	if (pcsAnimAttachedData->m_unCustFlags)
	{		
		sprintf(
			szValue,
			//@{ 2006/09/08 burumal
			//"%d:%d:%d:%d",
			"%d:%d:%d:%d:%d",			
			//@}
			lAnimType,
			lAnimEquip,
			lAnimIndex,
			pcsAnimAttachedData->m_unCustFlags,
			//@{ 2006/09/08 burumal
			pcsAnimAttachedData->m_uClumpFadeOutTime
			//@}
			);

		sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_ANIMATION_CUST_FLAGS, lAnimType, lAnimEquip, lAnimIndex);
		
		pcsStream->WriteValue(szName, szValue);
	}

	return TRUE;
}

BOOL AgcmCharacter::TemplateAnimationStreamWrite(ApModuleStream *pcsStream, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
{
	CHAR				szName[128];
	CHAR				szValue[128];

	INT32				lIndex					= 0;

	AgcdAnimData2		*pcsCurAnimData			= NULL;

	INT32				lNumAnimType2			= GetAnimType2Num(pstAgcdCharacterTemplate);

	pcsStream->WriteValue(AGCMCHAR_INI_NAME_ANIM_TYPE2, pstAgcdCharacterTemplate->m_lAnimType2);

	for (INT32 lAnimType = AGCMCHAR_ANIM_TYPE_WAIT; lAnimType < AGCMCHAR_MAX_ANIM_TYPE; ++lAnimType)
	{
		for (INT32 lAnimTyp2 = 0; lAnimTyp2 < lNumAnimType2; ++lAnimTyp2)
		{
			//if (!pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[lAnimType][lAnimEquip])
			if (	(!pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]->m_pcsAnimation)	)
				continue;

			lIndex			= 0;
			//pcsCurAnimData	= pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[lAnimType][lAnimEquip]->m_pcsHead;
			pcsCurAnimData	= pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]->m_pcsAnimation->m_pcsHead;
			while (pcsCurAnimData)
			{
				// 기본 애니메이션
				if (pcsCurAnimData->m_pszRtAnimName)
				{
					sprintf(
						szValue,
						"%d:%d:%d:%s",
						lAnimType,
						lAnimTyp2,
						lIndex,
						pcsCurAnimData->m_pszRtAnimName	);

					sprintf(szName, "%s%d%d%d", AGCMCHAR_INI_NAME_ANIMATION_NAME, lAnimType, lAnimTyp2, lIndex);

					pcsStream->WriteValue(szName, szValue);

					TemplateAnimationAttachedDataStreamWrite(pcsStream, pcsCurAnimData, lAnimType, lAnimTyp2, lIndex);
				}

				++lIndex;
				pcsCurAnimData	= pcsCurAnimData->m_pcsNext;
			}

			//if (pstAgcdCharacterTemplate->m_csAnimationData.m_ppstAnimFlags[lAnimType][lAnimEquip])
			if (pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]->m_pstAnimFlags)
			{
				sprintf(szValue,
					"%d:%d:%d:%d",
					lAnimType,
					lAnimTyp2,
					//pstAgcdCharacterTemplate->m_csAnimationData.m_ppstAnimFlags[lAnimType][lAnimEquip]->m_unAnimFlag,
					pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]->m_pstAnimFlags->m_unAnimFlag,
					//pstAgcdCharacterTemplate->m_csAnimationData.m_ppstAnimFlags[lAnimType][lAnimEquip]->m_unPreference	);
					pstAgcdCharacterTemplate->m_pacsAnimationData[lAnimType][lAnimTyp2]->m_pstAnimFlags->m_unPreference	);

				sprintf(szName, "%s%d%d", AGCMCHAR_INI_NAME_ANIMATION_FLAGS, lAnimType, lAnimTyp2);

				pcsStream->WriteValue(szName, szValue);
			}
		}
	}

	return TRUE;
}

//@{ 2006/08/21 burumal
//BOOL AgcmCharacter::TemplateEtcDataStreamWrite(ApModuleStream *pcsStream, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
BOOL AgcmCharacter::TemplateEtcDataStreamWrite(ApModuleStream *pcsStream, PVOID pData, AgcdCharacterTemplate *pstAgcdCharacterTemplate)
//@}
{
	CHAR					szName[128];
	CHAR					szValue[128];

	// Object type
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_OBJECT_TYPE, pstAgcdCharacterTemplate->m_lObjectType);

	// Height
	sprintf(szName,		"%s", AGCMCHAR_INI_NAME_HEIGHT);
	sprintf(szValue,	"%d", pstAgcdCharacterTemplate->m_lHeight);
	pcsStream->WriteValue(szName, szValue);

	//. 2005. 10. 07. Nonstopdj
	//. Depth 
	sprintf(szName,		"%s", AGCMCHAR_INI_NAME_DEPTH);
	print_compact_format(szValue,	"%f", pstAgcdCharacterTemplate->m_fDepth);
	pcsStream->WriteValue(szName, szValue);


	// Rider Height
	sprintf(szName,		"%s", AGCMCHAR_INI_NAME_RIDER_HEIGHT);
	sprintf(szValue,	"%d", pstAgcdCharacterTemplate->m_lRiderHeight);
	pcsStream->WriteValue(szName, szValue);

	// Bounding sphere
	print_compact_format(
		szValue,
		"%f:%f:%f:%f",
		pstAgcdCharacterTemplate->m_stBSphere.center.x,	
		pstAgcdCharacterTemplate->m_stBSphere.center.y,	
		pstAgcdCharacterTemplate->m_stBSphere.center.z,	
		pstAgcdCharacterTemplate->m_stBSphere.radius	);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_BOUNDING_SPHERE, szValue);

	// Scale
	if (pstAgcdCharacterTemplate->m_fScale != 1.0f)
	{
		print_compact_format(szValue, "%f", pstAgcdCharacterTemplate->m_fScale);
		pcsStream->WriteValue(AGCMCHAR_INI_NAME_SCALE, szValue);
	}
	
	//@{ 2006/02/21 burumal
	// D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP 에 정의된 값과 수를 일치시킬것
	
	//pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE, pstAgcdCharacterTemplate->m_lPickingNodeIndex);
	
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE, pstAgcdCharacterTemplate->m_lPickingNodeIndex[0]);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE_2, pstAgcdCharacterTemplate->m_lPickingNodeIndex[1]);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE_3, pstAgcdCharacterTemplate->m_lPickingNodeIndex[2]);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE_4, pstAgcdCharacterTemplate->m_lPickingNodeIndex[3]);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE_5, pstAgcdCharacterTemplate->m_lPickingNodeIndex[4]);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_PICKNODE_6, pstAgcdCharacterTemplate->m_lPickingNodeIndex[5]);
	//@}

	//@{ 2006/03/24 burumal
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_TAGGING, (INT32) pstAgcdCharacterTemplate->m_bTagging);
	//@}

	if (pstAgcdCharacterTemplate->m_lFaceAtomicIndex >= 0)
		pcsStream->WriteValue(AGCMCHAR_INI_NAME_FACEATOMIC, pstAgcdCharacterTemplate->m_lFaceAtomicIndex);

	//@{ 2006/05/03 burumal	
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_DNF_1, (pstAgcdCharacterTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_KOREA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_DNF_2, (pstAgcdCharacterTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_CHINA		)) ? 1 : 0);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_DNF_3, (pstAgcdCharacterTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_WESTERN	)) ? 1 : 0);
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_DNF_4, (pstAgcdCharacterTemplate->m_nDNF & GETSERVICEAREAFLAG(AP_SERVICE_AREA_JAPAN		)) ? 1 : 0);
	//@}	

	//@{ 2006/08/22 burumal
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_LOOKAT_NODE, (INT32) pstAgcdCharacterTemplate->m_lLookAtNode);
	//@}

	//@{ 2006/08/28 burumal
	pcsStream->WriteValue(AGCMCHAR_INI_NAME_USE_BENDING, (INT32) pstAgcdCharacterTemplate->m_bUseBending);

	sprintf(szName, "%s", AGCMCHAR_INI_NAME_BENDING_FACTOR);
	print_compact_format(szValue, "%f", pstAgcdCharacterTemplate->m_fBendingFactor);
	pcsStream->WriteValue(szName, szValue);
	//@}

	//@{ 2006/12/15 burumal
	pcsStream->WriteValue(AGCMCHAR_INI_NON_PICKING_TYPE, (INT32) pstAgcdCharacterTemplate->m_bNonPickingType);
	//@}

	return TRUE;
}

BOOL AgcmCharacter::TemplateFaceStreamWrite( ApModuleStream *pcsStream, AgcdCharacterTemplate *pstAgcdCharacterTemplate	)
{
	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
		return TRUE;

	CHAR szDefaultFace[256], szValue[256];
	int	i;
	pcsStream->WriteValue( AGCMCHAR_INI_NAME_DEFAULT_FACE_NUM, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetFaceNum() );

	INT32 nFaceNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetFaceNum();
	for( i = 0; i < nFaceNum; i++ )
	{
		sprintf( szDefaultFace, "%s%04d", AGCMCHAR_INI_NAME_DEFAULT_FACE, i );
		pcsStream->WriteValue( szDefaultFace, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[i].c_str() );
	}

	for( i = 0; i < nFaceNum; i++ )
	{
		sprintf( szDefaultFace, "%s%04d", AGCMCHAR_INI_NAME_DEFAULT_FACE_RENDER_TYPE, i );
		sprintf(szValue, "%d:%d", pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[i], pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData[i] );
		pcsStream->WriteValue( szDefaultFace, szValue );
	}
	return TRUE;
}

BOOL AgcmCharacter::TemplateHairStreamWrite( ApModuleStream *pcsStream,	AgcdCharacterTemplate *pstAgcdCharacterTemplate	)
{
	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
		return TRUE;

	CHAR szDefaultHair[256], szValue[256];
	pcsStream->WriteValue( AGCMCHAR_INI_NAME_DEFAULT_HAIR_NUM, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetHairNum() );

	INT32 nHairNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetHairNum();
	for( int i = 0; i < nHairNum; i++ )
	{
		sprintf( szDefaultHair, "%s%04d", AGCMCHAR_INI_NAME_DEFAULT_HAIR, i );
		pcsStream->WriteValue( szDefaultHair, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[i].c_str() );
	}

	for( int i = 0; i < nHairNum; i++ )
	{
		sprintf( szDefaultHair, "%s%04d", AGCMCHAR_INI_NAME_DEFAULT_HAIR_RENDER_TYPE, i );
		sprintf(szValue, "%d:%d", pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[i], pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData[i] );
		pcsStream->WriteValue( szDefaultHair, szValue );
	}
	return TRUE;
}

BOOL AgcmCharacter::TemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate;
	//CHAR					szValue[256];

	pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	// Predata 관련.
	pThis->TemplatePreDataStreamWrite(pcsStream, pstAgcdCharacterTemplate);

	// LOD
	if (pThis->m_pcsAgcmLODManager)
		pThis->m_pcsAgcmLODManager->StreamWrite(pcsStream, &pstAgcdCharacterTemplate->m_stLOD);

	// render type
	pThis->m_pcsAgcmRender->StreamWriteClumpRenderType(pcsStream, &pstAgcdCharacterTemplate->m_csClumpRenderType);

	// Etc.
	//@{ 2006/08/21 burumal
	//pThis->TemplateEtcDataStreamWrite(pcsStream, pstAgcdCharacterTemplate);
	pThis->TemplateEtcDataStreamWrite(pcsStream, pData, pstAgcdCharacterTemplate);
	//@}

	/*
	// 익스포트시에 사용하지 않는다 ..
	// 의미도 없는거 같아서 삭제. by 마고자 2008/04/24
	// octree data
	{
		print_compact_format(szValue,
			"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f",
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].x,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].y,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[0].z,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].x,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].y,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[1].z,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].x,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].y,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[2].z,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].x,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].y,
			pstAgcdCharacterTemplate->m_stOcTreeData.topVerts_MAX[3].z	);

		if (!pcsStream->WriteValue(AGCMCHAR_INI_NAME_OCTREE_DATA, szValue))
			return FALSE;
	}
	*/

	return TRUE;
}

BOOL AgcmCharacter::AnimationStreamReadCB( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream	)
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	CHAR					szValue[256];
	CHAR					szTemp[128];

	INT32					lTemp1, lTemp2, lTemp3, lTemp4;
	UINT32					ulTemp4;

	while( pcsStream->ReadNextValue() )
	{
		if (!strcmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIM_TYPE2))
		{
			pcsStream->GetValue(&pstAgcdCharacterTemplate->m_lAnimType2);

			pThis->AllocateAnimationData(pstAgcdCharacterTemplate);
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_NAME, strlen(AGCMCHAR_INI_NAME_ANIMATION_NAME)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%s", &lTemp1, &lTemp2, &lTemp3, szTemp);
			
			if (!pstAgcdCharacterTemplate->m_pacsAnimationData)	return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2])
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]	= new AgcdCharacterAnimation();

			pThis->m_csAnimation2.AddAnimation(
				//&pstAgcdCharacterTemplate->m_csAnimationData.m_ppcsAnimation[lTemp1][lTemp2],
				&pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				szTemp																			);

			// WALK와 RUN의 경우 강제로 LOOP를 세팅한다.
			if (lTemp1 == AGCMCHAR_ANIM_TYPE_WALK || lTemp1 == AGCMCHAR_ANIM_TYPE_RUN)
			{
				if (!pThis->m_csAnimation2.AddFlags(&pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags))
				{
					ASSERT(!"!pThis->m_csAnimation2.AddFlags()");
					return FALSE;
				}

				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags->m_unAnimFlag |= AGCD_ANIMATION_FLAG_LOOP;
			}
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_BLENDING_ANIMATION_NAME, strlen(AGCMCHAR_INI_NAME_BLENDING_ANIMATION_NAME)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%s", &lTemp1, &lTemp2, &lTemp3, szTemp);

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData) return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if( (!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation) )
			{
				ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2].m_pcsAnimation");
				return FALSE;
			}

			AgcdAnimData2	*pcsAnimData	=
				pThis->m_csAnimation2.GetAnimData(
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				lTemp3																			);
			if (!pcsAnimData)
			{
				ASSERT(!"!pcsAnimData");
				return FALSE;
			}

			ACA_AttachedData	*pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsAnimData												));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			if (pcsAttachedData->m_pcsBlendingData)
			{
				ASSERT(!"pcsAttachedData->m_pcsBlendingData");
				return FALSE;
			}

			pcsAttachedData->m_pcsBlendingData		= pThis->m_csAnimation2.AddAnimData(szTemp, FALSE);
			if (!pcsAttachedData->m_pcsBlendingData)
			{
				ASSERT(!"!pcsAttachedData->m_pcsBlendingData");
				return FALSE;
			}
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_FLAGS, strlen(AGCMCHAR_INI_NAME_ANIMATION_FLAGS)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%d", &lTemp1, &lTemp2, &lTemp3, &lTemp4);

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData) return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if ((!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]))
			{
				ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]");
				return FALSE;
			}

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags)
			{
				if (!pThis->m_csAnimation2.AddFlags(&pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags))
				{
					ASSERT(!"!pThis->m_csAnimation2.AddFlags()");
					return FALSE;
				}
			}

			// WALK와 RUN의 경우 강제로 LOOP를 세팅한다.
			if (lTemp1 == AGCMCHAR_ANIM_TYPE_WALK || lTemp1 == AGCMCHAR_ANIM_TYPE_RUN)
				lTemp3 |= AGCD_ANIMATION_FLAG_LOOP;

			pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags->m_unAnimFlag		= lTemp3;
			pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pstAnimFlags->m_unPreference	= lTemp4;
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_ACTIVE_RATE, strlen(AGCMCHAR_INI_NAME_ANIMATION_ACTIVE_RATE)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%d", &lTemp1, &lTemp2, &lTemp3, &lTemp4);

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData)	return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if ( (!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation)	)
			{
				ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2].m_pcsAnimation");
				return FALSE;
			}

			AgcdAnimData2	*pcsAnimData	=
				pThis->m_csAnimation2.GetAnimData(
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				lTemp3																			);
			if (!pcsAnimData)
			{
				ASSERT(!"!pcsAnimData");
				return FALSE;
			}

			ACA_AttachedData	*pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsAnimData												));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			pcsAttachedData->m_unActiveRate			= lTemp4;
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_CUST_TYPE, strlen(AGCMCHAR_INI_NAME_ANIMATION_CUST_TYPE)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%d", &lTemp1, &lTemp2, &lTemp3, &lTemp4);

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData)
				return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if ( (!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation)	)
			{
				ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2].m_pcsAnimation");
				return FALSE;
			}

			AgcdAnimData2	*pcsAnimData	=
				pThis->m_csAnimation2.GetAnimData(
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				lTemp3																			);
			if (!pcsAnimData)
			{
				ASSERT(!"!pcsAnimData");
				return FALSE;
			}

			ACA_AttachedData	*pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsAnimData												));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			pcsAttachedData->m_nCustType			= lTemp4;
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_POINT, strlen(AGCMCHAR_INI_NAME_ANIMATION_POINT)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%s", &lTemp1, &lTemp2, &lTemp3, szTemp);

			if (strcmp(szTemp, ""))
			{
				if (!pstAgcdCharacterTemplate->m_pacsAnimationData)
					return FALSE;

				lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
				if (lTemp2 < 0)
					continue;

				//@{ 2006/09/21 burumal
				if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
					continue;
				//@}

				if (	(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
					(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation)	)
				{
					ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2].m_pcsAnimation");
					return FALSE;
				}

				AgcdAnimData2	*pcsAnimData	=
					pThis->m_csAnimation2.GetAnimData(
					pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
					lTemp3																			);
				if (!pcsAnimData)
				{
					ASSERT(!"!pcsAnimData");
					return FALSE;
				}

				ACA_AttachedData	*pcsAttachedData	=
					(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
					AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
					pcsAnimData												));
				if (!pcsAttachedData)
				{
					ASSERT(!"!pcsAttachedData");
					return FALSE;
				}

				if (pcsAttachedData->m_pszPoint)
				{
					ASSERT(!"pcsAttachedData->m_pszPoint");
					return FALSE;
				}

				pcsAttachedData->m_pszPoint	= new CHAR [strlen(szTemp) + 1];
				strcpy(pcsAttachedData->m_pszPoint, szTemp);
			}
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_ANIMATION_CUST_FLAGS, strlen(AGCMCHAR_INI_NAME_ANIMATION_CUST_FLAGS)))
		{
			pcsStream->GetValue(szValue, 256);

			//@{ 2006/09/08 burumal
			//sscanf(szValue, "%d:%d:%d:%D", &lTemp1, &lTemp2, &lTemp3, &ulTemp4);

			UINT32 uClumpHideFadeOutTime = 0;
			int nColNum = 0;
			for ( int nIdx = 0 ; nIdx < (int) strlen(szValue); nIdx++ )
				if ( szValue[nIdx] == ':' ) nColNum++;
			if ( nColNum > 3 )
				sscanf(szValue, "%d:%d:%d:%D:%d", &lTemp1, &lTemp2, &lTemp3, &ulTemp4, &uClumpHideFadeOutTime);
			else
				sscanf(szValue, "%d:%d:%d:%D", &lTemp1, &lTemp2, &lTemp3, &ulTemp4);
			//@}

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData) return FALSE;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if (	(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation)	)
			{
				ASSERT(!"!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2].m_pcsAnimation");
				return FALSE;
			}

			AgcdAnimData2	*pcsAnimData	=
				pThis->m_csAnimation2.GetAnimData(
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				lTemp3																			);
			if (!pcsAnimData)
			{
				ASSERT(!"!pcsAnimData");
				return FALSE;
			}

			ACA_AttachedData	*pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsAnimData												));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			pcsAttachedData->m_unCustFlags			= ulTemp4;

			//@{ 2006/09/08 burumal
			pcsAttachedData->m_uClumpFadeOutTime = uClumpHideFadeOutTime;
			//@}
		}
		else if (!strncmp(pcsStream->GetValueName(), AGCMCHAR_INI_NAME_SUB_ANIMATION_NAME, strlen(AGCMCHAR_INI_NAME_SUB_ANIMATION_NAME)))
		{
			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%d:%d:%d:%d:%s", &lTemp1, &lTemp2, &lTemp3, &lTemp4, szTemp);

			if (!pstAgcdCharacterTemplate->m_pacsAnimationData) return FALSE;
			if ( (lTemp1 < 0) || (lTemp1 >= AGCMCHAR_MAX_ANIM_TYPE)	) continue;

			lTemp2	= pThis->ConvertAnimType2(pstAgcdCharacterTemplate->m_lAnimType2, lTemp2);
			if (lTemp2 < 0)
				continue;

			//@{ 2006/09/21 burumal
			if ( pThis->GetAnimType2Num(pstAgcdCharacterTemplate) <= lTemp2 )
				continue;
			//@}

			if (	(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]) ||
				(!pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation)	) continue;

			AgcdAnimData2	*pcsAnimData	=
				pThis->m_csAnimation2.GetAnimData(
				pstAgcdCharacterTemplate->m_pacsAnimationData[lTemp1][lTemp2]->m_pcsAnimation,
				lTemp4																			);
			if (!pcsAnimData) continue;

			ACA_AttachedData	*pcsAttachedData	=
				(ACA_AttachedData *)(pThis->m_csAnimation2.GetAttachedData(
				AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY,
				pcsAnimData												));
			if (!pcsAttachedData)
			{
				ASSERT(!"!pcsAttachedData");
				return FALSE;
			}

			if (pcsAttachedData->m_pcsSubData)
			{
				ASSERT(!"pcsAttachedData->m_pcsSubData");
				return FALSE;
			}

			pcsAttachedData->m_pcsSubData			= pThis->m_csAnimation2.AddAnimData(szTemp, FALSE);
			if (!pcsAttachedData->m_pcsSubData)
			{
				ASSERT(!"!pcsAttachedData->m_pcsSubData");
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL AgcmCharacter::AnimationStreamWriteCB( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream )
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	// Animation
	pThis->TemplateAnimationStreamWrite(pcsStream, pstAgcdCharacterTemplate);

	return TRUE;
}

BOOL AgcmCharacter::CustomizeStreamReadCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream )
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	CHAR					szValue[256];

	while( pcsStream->ReadNextValue() )
	{
		if( !strcmp( pcsStream->GetValueName(),AGCMCHAR_INI_NAME_DEFAULT_FACE_NUM ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL ) {
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData = new AgcdCharacterDefaultHeadTemplate;
			}

			INT32 nFaceNum;
			pcsStream->GetValue( &nFaceNum );

			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo.resize( nFaceNum );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.Alloc( nFaceNum );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.MemsetRenderType( 0 );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.MemsetCustData( 0 );
		}
		else if( !strcmp( pcsStream->GetValueName(),AGCMCHAR_INI_NAME_DEFAULT_HAIR_NUM ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL ) {
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData = new AgcdCharacterDefaultHeadTemplate;
			}

			INT32 nHairNum;
			pcsStream->GetValue( &nHairNum );

			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo.resize( nHairNum );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.Alloc( nHairNum );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.MemsetRenderType( 0 );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.MemsetCustData( 0 );
		}
		else if( !strncmp( pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEFAULT_FACE, strlen(AGCMCHAR_INI_NAME_DEFAULT_FACE) ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
				continue;

			const char* pValueName = pcsStream->GetValueName();
			int nIdx = atoi( pValueName + strlen(AGCMCHAR_INI_NAME_DEFAULT_FACE) );

			pcsStream->GetValue(szValue, 256);
			CHAR szFileName[256];
			sscanf( szValue, "%s", szFileName );
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[nIdx]= szFileName;
		}
		else if( !strncmp( pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEFAULT_HAIR, strlen(AGCMCHAR_INI_NAME_DEFAULT_HAIR) ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
				continue;

			const char* pValueName = pcsStream->GetValueName();
			int nIdx = atoi( pValueName + strlen(AGCMCHAR_INI_NAME_DEFAULT_HAIR) );

			pcsStream->GetValue(szValue, 256);

			CHAR szFileName[65535] = {0, };
			sscanf( szValue, "%s", szFileName );
			
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[nIdx] = szFileName;
		}
		//add by dobal
		//face render type
		else if( !strncmp( pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEFAULT_FACE_RENDER_TYPE, strlen(AGCMCHAR_INI_NAME_DEFAULT_FACE_RENDER_TYPE) ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
				continue;

			INT32 lTemp1, lTemp2;
			int nIdx = atoi( pcsStream->GetValueName() + strlen(AGCMCHAR_INI_NAME_DEFAULT_FACE_RENDER_TYPE) );

			pcsStream->GetValue(szValue, 256);
			if( sscanf(szValue, "%d:%d", &lTemp1, &lTemp2) < 2 )
				lTemp2 = 0;

			if( nIdx < ( INT32 ) pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType.size() )
			{
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType	[nIdx] = lTemp1;
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData		[nIdx] = lTemp2;
			}
			#ifdef USE_MFC
			else
			{
				::MessageBox( NULL , "CustomizeStreamReadCB 케릭터 커스터마이즈 Face 인덱스 초과" , "XportTool" , MB_OK );

			}
			#endif
		}
		else if( !strncmp( pcsStream->GetValueName(), AGCMCHAR_INI_NAME_DEFAULT_HAIR_RENDER_TYPE, strlen(AGCMCHAR_INI_NAME_DEFAULT_HAIR_RENDER_TYPE) ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
				continue;

			INT32 lTemp1, lTemp2;
			int nIdx = atoi( pcsStream->GetValueName() + strlen(AGCMCHAR_INI_NAME_DEFAULT_HAIR_RENDER_TYPE) );

			pcsStream->GetValue(szValue, 256);
			if( sscanf(szValue, "%d:%d", &lTemp1, &lTemp2) < 2 )
				lTemp2 = 0;

			if( nIdx < ( INT32 ) pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType.size() )
			{
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[nIdx]	= lTemp1;
				pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData	[nIdx]	= lTemp2;
			}
			#ifdef USE_MFC
			else
			{
				::MessageBox( NULL , "CustomizeStreamReadCB 케릭터 커스터마이즈 Hair 인덱스 초과" , "XportTool" , MB_OK );

			}
			#endif
		}
		else if( !strncmp( pcsStream->GetValueName(), AGCMCHAR_INI_NAME_CUSTOMIZE_REVIEW, strlen(AGCMCHAR_INI_NAME_CUSTOMIZE_REVIEW) ) )
		{
			if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
				continue;

			float fTemp1, fTemp2, fTemp3, fTemp4, fTemp5, fTemp6;
			int nIdx = atoi( pcsStream->GetValueName() + strlen(AGCMCHAR_INI_NAME_CUSTOMIZE_REVIEW) );

			pcsStream->GetValue(szValue, 256);
			sscanf(szValue, "%f:%f:%f:%f:%f:%f", &fTemp1, &fTemp2, &fTemp3, &fTemp4, &fTemp5, &fTemp6 );

			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.x = fTemp1;
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.y = fTemp2;
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.z = fTemp3;

			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.x = fTemp4;
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.y = fTemp5;
			pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.z = fTemp6;
		}
	}
	return TRUE;
}

BOOL AgcmCharacter::CustomizeStreamWriteCB( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream )
{
	AgcmCharacter			*pThis = (AgcmCharacter *) pClass;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate = (AgpdCharacterTemplate *) pData;
	AgcdCharacterTemplate	*pstAgcdCharacterTemplate = pThis->GetTemplateData(pstAgpdCharacterTemplate);

	// Face
	pThis->TemplateFaceStreamWrite(pcsStream, pstAgcdCharacterTemplate);

	// Hair
	pThis->TemplateHairStreamWrite(pcsStream, pstAgcdCharacterTemplate);

	// customize preview
	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData ) {
		char szValue[256];
		float fTemp1, fTemp2, fTemp3, fTemp4, fTemp5, fTemp6;
		fTemp1 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.x;
		fTemp2 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.y;
		fTemp3 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera.z;
		fTemp4 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.x;
		fTemp5 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.y;
		fTemp6 = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera.z;

		print_compact_format( szValue, "%f:%f:%f:%f:%f:%f", fTemp1, fTemp2, fTemp3, fTemp4, fTemp5, fTemp6 );
		pcsStream->WriteValue( AGCMCHAR_INI_NAME_CUSTOMIZE_REVIEW, szValue );
	}
	return TRUE;
}

BOOL AgcmCharacter::CharacterImportDataReadCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCharacter			*pThis						= (AgcmCharacter *)			pClass;
	AgpdCharacterTemplate	*pcsAgpdCharacterTemplate	= (AgpdCharacterTemplate *)	pData;
	PVOID					*ppvBuffer					= (PVOID *)					pCustData;

	AuExcelLib				*pcsExcelTxtLib				= (AuExcelLib *)			ppvBuffer[0];
	INT32					nRow						= (INT32)					ppvBuffer[1];
	INT32					nCol						= (INT32)					ppvBuffer[2];

	if (!pcsExcelTxtLib)
		return FALSE;

	AgcdCharacterTemplate	*pcsAgcdCharacterTemplate	= pThis->GetTemplateData(pcsAgpdCharacterTemplate);

	// key row is zero
	//////////////////////////////////////////////////////////

	CHAR					*pszKey				= pcsExcelTxtLib->GetData(nCol, 0);
	if(!pszKey)
		return FALSE;

	if (!strcmp(pszKey, AGPMITEM_IMPORT_CHAR_RUN_CORRECT))
	{
		CHAR	*pszBuffer = pcsExcelTxtLib->GetData(nCol, nRow);

		if (pszBuffer)
			pcsAgcdCharacterTemplate->m_afDefaultAnimSpeedRate[AGCMCHAR_ANIM_TYPE_RUN]	= (FLOAT) atof(pszBuffer);
	}
	else if (!strcmp(pszKey, AGPMITEM_IMPORT_CHAR_ATTACK_CORRECT))
	{
		CHAR	*pszBuffer = pcsExcelTxtLib->GetData(nCol, nRow);

		if (pszBuffer)
			pcsAgcdCharacterTemplate->m_afDefaultAnimSpeedRate[AGCMCHAR_ANIM_TYPE_ATTACK]	= (FLOAT) atof(pszBuffer);
	}

	return TRUE;
}

