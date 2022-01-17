#include "AgcmExportResource.h"

#include "AuFileFind.h"
#include "RwUtil.h"

#include "AcuMathFunc.h"
USING_ACUMATH;
#include "AgcuEffUtil.h"
#include "AgcuEffPath.h"
#include "AuExcelBinaryLib.h"

RwInt32	dele_Eff_ini(const RwChar* fname, void* pNull=NULL)
{
	return DeleteFile( fname ) ? 0 : -1;
};

#define INI_RELATIVE_PATH						"INI"
#define INI_ADULT_RELATIVE_PATH					"INI\\Adult"

#define MODEL_DFF_RELATIVE_PATH					"CHARACTER"
#define MODEL_ANIMATION_RELATIVE_PATH			"CHARACTER\\ANIMATION"
#define MODEL_CUSTOMIZE_HAIR_RELATIVE_PATH		"CHARACTER\\DefaultHead\\Hair"
#define MODEL_CUSTOMIZE_FACE_RELATIVE_PATH		"CHARACTER\\DefaultHead\\Face"

#define	OBJECT_DFF_RELATIVE_PATH				"OBJECT"
#define	OBJECT_ANIMATION_RELATIVE_PATH			"OBJECT\\ANIMATION"

#define EFFECT_RELATIVE_PATH					"EFFECT"
#define EFFECT_INI_RELATIVE_PATH				"EFFECT\\INI"
#define EFFECT_INI_NEW_RELATIVE_PATH			"EFFECT\\INI\\NEW"
#define EFFECT_ANIMATION_RELATIVE_PATH			"EFFECT\\ANIMATION"
#define EFFECT_SPLINE_RELATIVE_PATH				"EFFECT\\ANIMATION"
#define EFFECT_OBJECT_RELATIVE_PATH				"EFFECT\\CLUMP"

#define SOUND_RELATIVE_PATH						"SOUND"
#define EFFECT_SOUND_RELATIVE_PATH				"SOUND\\EFFECT"
#define EFFECT_MONO_SOUND_RELATIVE_PATH			"SOUND\\EFFECT"

#define WORLD_DATA_RELATIVE_PATH				"WORLD"
#define WORLD_GRASS_RELATIVE_PATH				"WORLD\\GRASS"
#define WORLD_WATER_RELATIVE_PATH				"WORLD\\WATER"
#define WORLD_OCTREE_RELATIVE_PATH				"WORLD\\OCTREE"

#define TEXTURE_RELATIVE_PATH					"TEXTURE"
#define MODEL_TEXTURE_RELATIVE_PATH				"TEXTURE\\CHARACTER"
#define MODEL_TEXTURE_MQ_RELATIVE_PATH			"TEXTURE\\CHARACTER\\MEDIUM"
#define MODEL_TEXTURE_LQ_RELATIVE_PATH			"TEXTURE\\CHARACTER\\LOW"
#define ITEM_TEXTURE_RELATIVE_PATH				"TEXTURE\\ITEM"
#define OBJECT_TEXTURE_RELATIVE_PATH			"TEXTURE\\OBJECT"
#define OBJECT_TEXTURE_MQ_RELATIVE_PATH			"TEXTURE\\OBJECT\\MEDIUM"
#define OBJECT_TEXTURE_LQ_RELATIVE_PATH			"TEXTURE\\OBJECT\\LOW"
#define EFFECT_TEXTURE_RELATIVE_PATH			"TEXTURE\\EFFECT"
#define EFFECT_TEXTURE_MQ_RELATIVE_PATH			"TEXTURE\\EFFECT\\MEDIUM"
#define EFFECT_TEXTURE_LQ_RELATIVE_PATH			"TEXTURE\\EFFECT\\LOW"
#define WORLD_TEXTURE_RELATIVE_PATH				"TEXTURE\\WORLD"
#define WORLD_TEXTURE_MQ_RELATIVE_PATH			"TEXTURE\\WORLD\\MEDIUM"
#define WORLD_TEXTURE_LQ_RELATIVE_PATH			"TEXTURE\\WORLD\\LOW"
#define WORLDMAP_TYPE1_TEXTURE_RELATIVE_PATH	"TEXTURE\\WORLDMAP\\TYPE01"
#define WORLDMAP_TEXTURE_RELATIVE_PATH			"TEXTURE\\WORLDMAP"
#define MINIMAP_TEXTURE_RELATIVE_PATH			"TEXTURE\\MINIMAP"
#define SKILL_TEXTURE_RELATIVE_PATH				"TEXTURE\\SKILL"
#define ETC_TEXTURE_RELATIVE_PATH				"TEXTURE\\ETC"
#define UI_TEXTURE_RELATIVE_PATH				"TEXTURE\\UI"
#define UI_BASE_TEXTURE_RELATIVE_PATH			"TEXTURE\\UI\\BASE"
#define UI_TYPE1_TEXTURE_RELATIVE_PATH			"TEXTURE\\UI_Type1"
#define UI_TYPE1_BASE_TEXTURE_RELATIVE_PATH		"TEXTURE\\UI_Type1\\BASE"
#define NOT_PACKED_TEXTURE_RELATIVE_PATH		"TEXTURE\\NOTPACKED"
#define AMB_OCCL_TEXTURE_RELATIVE_PATH			"TEXTURE\\AmbOccl"

CHAR *	g_pszINI_UI_Resources[] =
{
	"UI_1024X768.INI",
	"UIMESSAGE.TXT",
	NULL
};

CHAR *	g_pszExcelResources[] =
{
	"CHARACTERDATATABLE.TXT",
	"SKILL_CONST.TXT",
	"ITEMDATATABLE.TXT",
	"RefineryItem2.txt",
	NULL
};

CHAR *	g_pszINIResources[] =
{
    "ALEFSKYSETTING.INI",
	"BASE.DAT",
	"CHARTYPE.INI",
	"EE_COMMON_CHAR.TXT",
	"EE_MOVE_SOUND.TXT",
	"EE_SS_CHANGE.TXT",
	"EE_SS_FX.TXT",
	"EE_SS_TARGET.TXT",
	"GRASSTEMPLATE.INI",
	"GROWUPFACTOR.TXT",
	"HWATERSTATUS.INI",
	"ITEMCONVERTPOINT.TXT",
	"ITEMCONVERTTABLE.TXT",
	"ITEMOPTIONTABLE.TXT",
	"ITEMRUNEATTRIBUTETABLE.TXT",
	"ITEMTOOLTIP.TXT",
	"ITEMTOOLTIPDESC.TXT",
	"ITEMTRANSFORMTABLE.TXT",
	"ITEMEVOLUTIONTABLE.TXT",
	"LENSFLARE.INI",
	"LEVELUPEXP.TXT",
	"LEVELUPMESSAGE.TXT",
	"LODDISTANCETYPE.TXT",
	"LOGINCHARPOS.INI",
	"MOBDIALOG.TXT",
	"NATUREEFFECT.INI",
	"NPCDIALOG.TXT",
	"NPCTRADEITEMGROUP.TXT",
	"NPCTRADEITEMLIST.TXT",
	"NPCTRADETEMPLATE.TXT",
	"PRODUCTCATEGORY.TXT",
	"PRODUCTCOMPOSE.TXT",
	"PRODUCTEXP.TXT",
	"PRODUCTGATHER.TXT",
	"PVPITEMDROPTABLE.TXT",
	"PVPSKULLDROPTABLE.TXT",
	"QUESTGROUP.INI",
	"QUESTTEMPLATE.INI",
	"REFINERY.TXT",
	"RefineryItem.txt",
	"REGIONTEMPLATE.INI",
	"SKILL_DEFAULTOWN.TXT",
	"SKILL_PRODUCT.TXT",
	"SKILL_SPEC.TXT",
	"SKILLMASTERY.TXT",
	"SKILLTOOLTIP.TXT",
	"SKYSET.INI",
	"TARGETING.INI",
	"TELEPORTGROUP.INI",
	"TELEPORTPOINT.INI",
	"TELEPORTFEE.TXT",
	"TEXTBOARD.INI",
	"UIDATALIST.INI",
	"WATERSTATUST1.INI",
	"WORLDMAP.INI",
	"charactercustomizelist.txt",
	"DriverInfo.ini",
	"OptionInit.ini",
	"productresultitem.txt",
	"refineryoptionstone.txt",
	"Tips.txt",
	"TipText.txt",
	"Help.txt",
	"BankSlotPrice.txt",
	"GuildMaxMember.txt",
	"EE_COMMON_CHAR.TXT",
	"GuildMark.txt",
	"SOCIAL_PC.txt",
	"SOCIAL_GM.txt",
	"Adult\\" "QUESTTEMPLATE.INI",
	"Adult\\" "BankSlotPrice.txt",
	"Adult\\" "CharacterCustomizeList.txt",
	"Adult\\" "CharacterDataTable.txt",
	"Adult\\" "GroupDropRate.txt",
	"Adult\\" "guildmark.txt",
	"Adult\\" "GuildMaxMember.txt",
	"Adult\\" "ItemConvertTable.txt",
	"Adult\\" "ITEMDATATABLE.TXT",
	"Adult\\" "ItemOptionTable.txt",
	"Adult\\" "ItemRuneAttributeTable.txt",
	"Adult\\" "ItemTooltipDesc.txt",
	"Adult\\" "NPCTradeItemList.txt",
	"Adult\\" "Refinery.txt",
	"Adult\\" "RefineryItem.txt",
	"Adult\\" "TeleportFee.txt",
	"Adult\\" "avatarset.ini",
	"SysStr.txt",
	"RegionGlossary.txt",
	"SiegeWar.txt",
	"SiegeWarObject.txt",
	"PolyMorphTable.txt",
	"ExcEffTable.txt",
	"SkillMastery_High.txt",
	"SocialAction.txt",
	"avatarset.ini",
	"SKILL_CONST2.TXT",
	NULL
};

AgcmExportResource::AgcmExportResource() : 
 m_eExportAreaSetting( AP_SERVICE_AREA_KOREA ),
 m_eExportTarget( SET_INTERNAL )
{
	memset( m_szSrcPath, 0, sizeof (CHAR) * 1024 );
	memset( m_szDestPath, 0, sizeof (CHAR) * 1024 );
}

AgcmExportResource::~AgcmExportResource()
{
}

BOOL AgcmExportResource::Initialize( CHAR* pszSrcPath, CHAR* pszDestPath, AgcmCharacter* pcsAgcmCharacter, AgcmItem* pcsAgcmItem, AgcmObject* pcsAgcmObject, LogFunc funcLog, LogFormatFunc funcLogFormat )
{
	ASSERT( funcLog && funcLogFormat );

	m_funcLog		= funcLog;
	m_funcLogFormat	= funcLogFormat;

	return SetSourcePath( pszSrcPath, pszDestPath, pcsAgcmCharacter, pcsAgcmItem, pcsAgcmObject );
}

BOOL AgcmExportResource::SetSourcePath(	CHAR* pszSrcPath, CHAR* pszDestPath, AgcmCharacter* pcsAgcmCharacter, AgcmItem* pcsAgcmItem, AgcmObject* pcsAgcmObject )
{
	ASSERT( pszSrcPath && pszSrcPath[0] );
	ASSERT( pszDestPath && pszDestPath[0] );

	strcpy( m_szSrcPath, pszSrcPath );
	strcpy( m_szDestPath, pszDestPath );

	CHAR	szPath[1024*2] = {'\0',};
	sprintf(szPath, "%s%s\\", pszSrcPath, MODEL_DFF_RELATIVE_PATH);
	if( pcsAgcmCharacter)
		pcsAgcmCharacter->SetClumpPath(szPath);
	if( pcsAgcmItem )
		pcsAgcmItem->SetClumpPath(szPath);
		
	sprintf(szPath, "%s%s\\", pszSrcPath, OBJECT_DFF_RELATIVE_PATH);
	if( pcsAgcmObject )
		pcsAgcmObject->SetClumpPath(szPath);

	sprintf(szPath, "%s%s\\", pszSrcPath, MODEL_ANIMATION_RELATIVE_PATH);
	if( pcsAgcmCharacter )
		pcsAgcmCharacter->SetAnimationPath(szPath);

	sprintf(szPath, "%s%s\\", pszSrcPath, OBJECT_ANIMATION_RELATIVE_PATH);
	if ( pcsAgcmObject )
		pcsAgcmObject->SetAnimationPath(szPath);

	strcpy( szPath , pszSrcPath );	strcat( szPath , MODEL_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , OBJECT_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , EFFECT_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , WORLD_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , MINIMAP_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , WORLDMAP_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , SKILL_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , ITEM_TEXTURE_RELATIVE_PATH			);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , ETC_TEXTURE_RELATIVE_PATH			);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , UI_TEXTURE_RELATIVE_PATH			);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , UI_BASE_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , NOT_PACKED_TEXTURE_RELATIVE_PATH	);	strcat( szPath , "\\;");
	strcat( szPath , pszSrcPath );	strcat( szPath , AMB_OCCL_TEXTURE_RELATIVE_PATH		);	strcat( szPath , "\\");

	RwImageSetPath(szPath);

	sprintf(szPath, "%s%s\\", pszSrcPath, EFFECT_RELATIVE_PATH);
	RpMaterialD3DFxSetSearchPath(szPath);

	SetCurrentDirectory(pszSrcPath);

	return TRUE;
}

BOOL AgcmExportResource::ExportCharacterTemplate( AgpmCharacter *pcsAgpmCharacter, AgcmCharacter *pcsAgcmCharacter, AgcmEventEffect *pcsAgcmEventEffect,
												CHAR *pszCharacterTemplatePublic, CHAR *pszCharacterTemplateClient, CHAR *pszCharacterTemplateAnimation, CHAR *pszCharacterTemplateCustomize,
												CHAR *pszCharacterTemplateSkill, CHAR *pszCharacterTemplateSkillSound, CHAR *pszCharacterTemplateEventEffect, 
												CHAR *pszSocialPC, CHAR *pszSocialGM, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszCharacterPreLOD,
												AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate )
{
	m_funcLog( "Export - character templates" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	m_csExportDFF.Init( "CharacterTexture.txt", m_funcLog );

	if (!ReadSkillTemplate(pcsAgpmSkill, pszSkillTemplate))
	{
		m_funcLog( "Failed - ReadSkillTemplate()" );
		return FALSE;
	}

	if( !ReadCharacterTemplate(	pcsAgpmCharacter, pcsAgcmCharacter, pcsAgcmEventEffect,
								pszCharacterTemplatePublic,	pszCharacterTemplateClient,
								pszCharacterTemplateAnimation, pszCharacterTemplateCustomize,
								pszCharacterTemplateSkill, pszCharacterTemplateSkillSound,
								pszCharacterTemplateEventEffect,
								pcsAgcmPreLODManager, pszCharacterPreLOD,
								pcsAgpmSkill, pcsAgcmSkill,	pszSkillTemplate ) )
	{
		m_funcLog( "Failed - ReadCharacterTemplate()" );
		return FALSE;
	}

	m_funcLog( "Social Animation을 읽고 있습니다." );

	CHAR	szSocialPC[128];
	sprintf(szSocialPC, "%s\\%s", INI_RELATIVE_PATH, pszSocialPC);
	CHAR	szSocialGM[128];
	sprintf(szSocialGM, "%s\\%s", INI_RELATIVE_PATH, pszSocialGM);
	pcsAgcmEventEffect->RemoveAllSocialAnimation();
	if (!pcsAgcmEventEffect->ReadSocialAnimationData(szSocialPC, szSocialGM, MODEL_ANIMATION_RELATIVE_PATH, FALSE))
	{
		m_funcLog( "Failed - ReadSocialAnimationData()" );
		return FALSE;
	}

	if (!CreateDirectoryForCharacterTemplate())
	{
		m_funcLog( "Failed - CreateDirectoryForCharacterTemplate()" );
		return FALSE;
	}

	m_funcLog( "ExportCharacterTemplate() 시작.." );

	if (!ExportCharacterTemplate(   pcsAgpmCharacter, 
									pcsAgcmCharacter, 
									pcsAgcmEventEffect, 
									pszCharacterTemplatePublic,
									pszCharacterTemplateClient,
									pszCharacterTemplateAnimation,
									pszCharacterTemplateCustomize,
									pszCharacterTemplateSkill,
									pszCharacterTemplateSkillSound,
									pszCharacterTemplateEventEffect,
									pcsAgcmSkill ) )
	{
		m_funcLog( "Failed - ExportCharacterTemplate() 시작.." );
		return FALSE;
	}

	m_funcLog( "Complete - character templates" );

	return TRUE;
}

BOOL AgcmExportResource::ExportItemTemplates( AgpmItem* ppmItem, AgcmItem* pcmItem, char* pTargetPath, char* pEntryFileName, AgcmPreLODManager* pcmPreLODManager, char* pItemPreLODFileName )
{
	m_funcLog( "Export - item templates" );

	RwTextureSetMipmapping( TRUE );
	RwTextureSetAutoMipmapping( TRUE );

	m_csExportDFF.Init( "ItemTexture.txt", m_funcLog );

	if( !ReadItemTemplate( ppmItem, pTargetPath, pcmPreLODManager, pItemPreLODFileName ) )
	{
		m_funcLog( "Failed - ReadItemTemplate()" );
		return FALSE;
	}

	if( !CreateDirectoryForItemTemplate() )
	{
		m_funcLog( "Failed - CreateDirectoryForItemTemplate()" );
		return FALSE;
	}

	m_funcLog( "ExportItemTemplates() 시작" );

	if( !ExportItemTemplates( ppmItem, pcmItem, pTargetPath, pEntryFileName ) )
	{
		m_funcLog( "Failed - ExportItemTemplates()" );
		return FALSE;
	}

	m_funcLog( "Complete - item templates" );

	return TRUE;
}


//BOOL AgcmExportResource::ExportItemTemplate( AgpmItem *pcsAgpmItem, AgcmItem *pcsAgcmItem, CHAR *pszItemTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD, vector<INT32>* pvectorWishlist )
//{
//	m_funcLog( "Export - item templates" );
//
//	RwTextureSetMipmapping(TRUE);
//	RwTextureSetAutoMipmapping(TRUE);
//
//	m_csExportDFF.Init( "ItemTexture.txt", m_funcLog );
//
//	if( !ReadItemTemplate( pcsAgpmItem, pszItemTemplate, pcsAgcmPreLODManager , pszItemPreLOD ) )
//	{
//		m_funcLog( "Failed - ReadItemTemplate()" );
//		return FALSE;
//	}
//
//	if( !CreateDirectoryForItemTemplate() )
//	{
//		m_funcLog( "Failed - CreateDirectoryForItemTemplate()" );
//		return FALSE;
//	}
//
//	m_funcLog( "ExportItemTemplate() 시작" );
//
//	if( !ExportItemTemplate( pcsAgpmItem, pcsAgcmItem, pszItemTemplate, pvectorWishlist ) )
//	{
//		m_funcLog( "Failed - ExportItemTemplate()" );
//		return FALSE;
//	}
//
//	m_funcLog( "Complete - item templates" );
//
//	return TRUE;
//}

BOOL AgcmExportResource::ExportObjectTemplate( ApmObject *pcsApmObject, AgcmObject *pcsAgcmObject, CHAR *pszObjectTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszObjectPreLOD )
{
	m_funcLog( "Export - Object templates" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	m_csExportDFF.Init( "ObjectTexture.txt", m_funcLog );

	if (!ReadObjectTemplate(pcsApmObject, pszObjectTemplate,pcsAgcmPreLODManager , pszObjectPreLOD ))
	{
		m_funcLog( "Failed - ReadObjectTemplate()" );
		return FALSE;
	}

	if (!CreateDirectoryForObjectTemplate())
	{
		m_funcLog( "Failed - CreateDirectoryForObjectTemplate()" );
		return FALSE;
	}

	if (!ExportObjectTemplate(pcsApmObject, pcsAgcmObject, pszObjectTemplate))
	{
		m_funcLog( "Failed - ExportObjectTemplate()" );
		return FALSE;
	}

	m_funcLog( "Complete - Object templates" );

	return TRUE;
}

BOOL AgcmExportResource::ExportSkillTemplate( AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate)
{
	m_funcLog( "Export - Skill template" );

	RwTextureSetMipmapping(FALSE);
	RwTextureSetAutoMipmapping(FALSE);

	if (!ReadSkillTemplate(pcsAgpmSkill, pszSkillTemplate))
		m_funcLog( "Failed - ReadSkillTemplate()" );

	if (!CreateDirectoryForSkillTemplate())
		m_funcLog( "Failed - CreateDirectoryForSkillTemplate()" );

	m_funcLog( "ExportSkillTemplate() 시작" );

	if ( !ExportSkillTemplate_( pcsAgpmSkill, pcsAgcmSkill, pszSkillTemplate ) )
		m_funcLog( "Failed - ExportSkillTemplate_()" );

	m_funcLog( "Complete - Skill template" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	return TRUE;
}

BOOL AgcmExportResource::ExportEffectResource(AgcmEff2 *pAgcmEff2 )
{
	m_funcLog( "Export - Effect files" );

	RwTextureSetMipmapping(FALSE);
	RwTextureSetAutoMipmapping(FALSE);

	m_csExportDFF.Init( "EffectTexture.txt", m_funcLog );

	if (!CreateDirectoryForEffectFile())
	{
		m_funcLog( "Failed - CreateDirectoryForEffectFile()" );
		return FALSE;
	}

	m_funcLog( "ExportEffectData() 시작" );

	if (!ExportEffectData(pAgcmEff2))
	{
		m_funcLog( "Failed - ExportEffectData()" );
		return FALSE;
	}

	if (!ExportFXData(EFFECT_RELATIVE_PATH))
	{
		m_funcLog( "Failed - ExportFXData()" );
		return FALSE;
	}

	m_funcLog( "Complete - Effect files" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	return TRUE;
}

BOOL AgcmExportResource::ExportUIResource( )
{
	m_funcLog( "Export - ui resource" );

	RwTextureSetMipmapping(FALSE);
	RwTextureSetAutoMipmapping(FALSE);
	
	m_csExportDFF.Init( "UITexture.txt", m_funcLog );

	if (!CreateDirectoryForUIData())
	{
		m_funcLog( "Failed - CreateDirectoryForUIData()" );
		return FALSE;
	}

	if( !ExportUIData() )
	{
		m_funcLog( "Failed - ExportUIData()" );
		return FALSE;
	}

	m_funcLog( "Complete - ui resource" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	return TRUE;
}

BOOL AgcmExportResource::ExportWorldResource()
{
	m_funcLog( "Export - world resource" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	m_csExportDFF.Init( "WorldTexture.txt", m_funcLog );

	if (!CreateDirectoryForWorldData())
	{
		m_funcLog( "Failed - CreateDirectoryForWorldData()" );
		return FALSE;
	}

	if (!ExportWorldTexture())
	{
		m_funcLog( "Failed - ExportWorldTexture()" );
		return FALSE;
	}

	m_funcLog( "::월드 데이타 카피" );

	if (!_CopyDirectory(WORLD_DATA_RELATIVE_PATH))
	{
		m_funcLog( "Failed - _CopyDirectory()" );
		return FALSE;
	}

	m_funcLog( "::풀 데이타 카피" );

	if (!_CopyDirectory(WORLD_GRASS_RELATIVE_PATH))
	{
		m_funcLog( "Failed - _CopyDirectory()" );
		return FALSE;
	}

	m_funcLog( "::물 데이타 카피" );

	if (!_CopyDirectory(WORLD_WATER_RELATIVE_PATH))
	{
		m_funcLog( "Failed - _CopyDirectory()" );
		return FALSE;
	}

	m_funcLog( "::옥트리 데이타 카피" );

	if (!_CopyDirectory(WORLD_OCTREE_RELATIVE_PATH))
	{
		m_funcLog( "Failed - _CopyDirectory()" );
		return FALSE;
	}

	m_funcLog( "Complete - world resource" );

	return TRUE;
}

BOOL AgcmExportResource::ExportEtcResource()
{
	m_funcLog( "Export - etc texture" );

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	m_csExportDFF.Init( "EtcTexture.txt", m_funcLog );

	if (!CreateDirectoryForEtcTexture())
	{
		m_funcLog( "Failed - CreateDirectoryForWorldData()" );
		return FALSE;
	}

	if (!ExportFXData(EFFECT_RELATIVE_PATH))
	{
		m_funcLog( "Failed - ExportFXData()" );
		return FALSE;
	}

	if (!ExportEtcTexture())
	{
		m_funcLog( "Failed - ExportEtcTexture()" );
		return FALSE;
	}

	m_funcLog( "Complete - etc resource" );

	return TRUE;
}

BOOL AgcmExportResource::ExportINIResource()
{
	m_funcLog( "Export - INI resource" );

	CreateDirectoryForINIResource();

	for (INT32 lIndex = 0; g_pszINIResources[lIndex]; ++lIndex)
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "Copy INI File : %s%s, Index = %d\n", INI_RELATIVE_PATH, g_pszINIResources[lIndex], lIndex );
		m_funcLog( strDebug );

		if (!_CopyFile(INI_RELATIVE_PATH, g_pszINIResources[lIndex], TRUE))
		{
			m_funcLogFormat( "Failed - %s", g_pszINIResources[lIndex] );
		}
	}

	#ifdef USE_BINARY_EXCEL_FILE
	// 엑셀파일 바이너리 작업..
	for (INT32 lIndex = 0; g_pszExcelResources[lIndex]; ++lIndex)
	{
		AuExcelTxtLib	excel;
		char	filename[ MAX_PATH ];
		sprintf( filename , "%s\\%s" , INI_RELATIVE_PATH , g_pszExcelResources[lIndex] );

		char	filenamebinary[ MAX_PATH ];
		sprintf( filenamebinary , "%s%s\\%s" , m_szDestPath , INI_RELATIVE_PATH , g_pszExcelResources[lIndex] );

		m_funcLogFormat( "Converting %s........", filename  );

		if( excel.OpenExcelFile( filename , TRUE ) )
		{
			using namespace AuExcel;

			AuExcelBinaryLib	exb( excel );

			m_funcLogFormat( "Writing %s.....", filenamebinary );

			if( !exb.WriteExcelFile( filenamebinary ) )
				m_funcLogFormat( "Failed - %s", g_pszExcelResources[lIndex] );
		}
		else
			m_funcLogFormat( "Open Excel File Faied '%s'", filename );
	}
	#else
	// 기존과 호환모드..
	for (INT32 lIndex = 0; g_pszExcelResources[lIndex]; ++lIndex)
	{
		char strDebug[ 256 ] = { 0, };
		sprintf( strDebug, "Copy Excel File : %s%s, Index = %d\n", INI_RELATIVE_PATH, g_pszExcelResources[lIndex], lIndex );
		m_funcLog( strDebug );

		if (!_CopyFile(INI_RELATIVE_PATH, g_pszExcelResources[lIndex], TRUE))
		{
			m_funcLogFormat( "Failed - %s", g_pszExcelResources[lIndex] );
		}
	}
	#endif

	CHAR			szFileName[256], szFindPath[512];
	AuFileFind		csFinder;

	sprintf(szFindPath, "%s\\OBJ*.INI", INI_RELATIVE_PATH);

	if (!csFinder.FindFile(szFindPath))
		return FALSE;
	else
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		if (szFileName[3] >= '0' && szFileName[3] <= '9')
			if (!_CopyFile(INI_RELATIVE_PATH, szFileName, FALSE))
				m_funcLogFormat( "Failed - %s", szFileName );
	}

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		if (szFileName[3] < '0' || szFileName[3] > '9')
			continue;

		if (!_CopyFile(INI_RELATIVE_PATH, szFileName, FALSE))
			m_funcLogFormat( "Failed - %s", szFileName );
	}

	m_funcLog( "Complete - INI resource" );

	return TRUE;
}

//---------------------------------- protected ---------------------------------
BOOL AgcmExportResource::ExportCharacterTemplate( AgpmCharacter *pcsAgpmCharacter, AgcmCharacter *pcsAgcmCharacter, AgcmEventEffect *pcsAgcmEventEffect,
													CHAR *pszCharacterTemplatePublic, CHAR *pszCharacterTemplateClient, CHAR *pszCharacterTemplateAnimation,
													CHAR *pszCharacterTemplateCustomize, CHAR *pszCharacterTemplateSkill, CHAR *pszCharacterTemplateSkillSound, CHAR *pszCharacterTemplateEventEffect,
													AgcmSkill *pcsAgcmSkill )
{
	CHAR	szOutput[256];
	memset(szOutput, 0, sizeof (CHAR) * 256);

	INT32 nSequenceIndex = 0;
	AgpdCharacterTemplate* pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetTemplateSequence( &nSequenceIndex );
	while( pcsAgpdCharacterTemplate )
	{
		AgcdCharacterTemplate *pcsAgcdCharacterTemplate = pcsAgcmCharacter->GetTemplateData(pcsAgpdCharacterTemplate);
		if (!pcsAgcdCharacterTemplate)
		{
			m_funcLogFormat( "-%s-", pcsAgpdCharacterTemplate->m_szTName );
			m_funcLog( "Failed - pcsAgcmCharacter->GetTemplateData()" );
			continue;
		}

		pcsAgcdCharacterTemplate->m_lRefCount = 1;
		pcsAgcmCharacter->LoadTemplateData( pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate );

		if (!ExportCharacterClump( pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate ) )
		{
			m_funcLogFormat( "-%s-", pcsAgpdCharacterTemplate->m_szTName );
			m_funcLog( "Failed - ExportCharacterClump()" );
		}

		AgcdSkillAttachTemplateData *pcsAgcdSkillData = pcsAgcmSkill->GetAttachTemplateData((ApBase *)(pcsAgpdCharacterTemplate));
		if (!ExportCharacterAnimation( pcsAgcmSkill, pcsAgcmCharacter, pcsAgpdCharacterTemplate, pcsAgcdCharacterTemplate, pcsAgcdSkillData ) )
		{
			m_funcLogFormat( "-%s-", pcsAgpdCharacterTemplate->m_szTName );
			m_funcLog( "Failed - ExportCharacterAnimation()" );
		}

		// 플래그에서 프리라이트를 끈다.
		pcsAgcdCharacterTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_PRE_LIGHT;
		pcsAgcmCharacter->RemovePreData(pcsAgcdCharacterTemplate);
		pcsAgcmCharacter->ReleaseTemplateData(pcsAgcdCharacterTemplate);

		pcsAgpdCharacterTemplate = pcsAgpmCharacter->GetTemplateSequence( &nSequenceIndex );
	}

	// 새로운 ini를 작성한다.
	CHAR szDestIniPath[256];
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplatePublic);
	if (!pcsAgpmCharacter->StreamWriteTemplate(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgpmCharacter->StreamWriteTemplate()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateClient);
	if (!pcsAgcmCharacter->StreamWriteTemplate(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmCharacter->StreamWriteTemplate()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateAnimation);
	if (!pcsAgcmCharacter->StreamWriteTemplateAnimation(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmCharacter->StreamWriteTemplateAnimation()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateCustomize);
	if (!pcsAgcmCharacter->StreamWriteTemplateCustomize(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmCharacter->StreamWriteTemplateCustomize()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateSkill);
	if (!pcsAgcmSkill->StreamWriteTemplateSkill(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmSkill->StreamWriteTemplateSkill()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateSkillSound);
	if (!pcsAgcmSkill->StreamWriteTemplateSkillSound(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmSkill->StreamWriteTemplateSkillSound()" );
		return FALSE;
	}
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszCharacterTemplateEventEffect);
	if (!pcsAgcmEventEffect->StreamWriteTemplate(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgcmEventEffect->StreamWriteTemplate()" );
		return FALSE;
	}

	m_funcLog( "Social Animation을 Export 합니다......" );

	if (!ExportCharacterSocialAnimation(pcsAgcmEventEffect))
	{
		m_funcLog( "Failed - ExportCharacterSocialAnimation()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportCharacterClump( AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate )
{
	if (!CheckLoadedClump(pcsAgcdCharacterTemplate))
	{
		m_funcLog( "Failed - CheckLoadedClump()" );
		return FALSE;
	}

	if (!pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName)
	{
		m_funcLog( "Invalid - pcsAgcdCharacterTemplate->m_szDFFName" );
		return FALSE;
	}

	UINT32	ulExportedID	= 0;
	CHAR	szTemp[256];

	strcpy(szTemp, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName);

	if (!ExportClump( szTemp, pcsAgcdCharacterTemplate->m_pClump, pcsAgpdCharacterTemplate->m_lID, AC_EXPORT_HD_CHAR_CHARACTER, AC_EXPORT_EXT_CLUMP,
					MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cCharacterClump ) ) 
	{
		m_funcLog( "Failed - ExportClump()" );
		return FALSE;
	}

	ulExportedID	= strtoul(szTemp + 1, NULL, 16);
	pcsAgcdCharacterTemplate->SetClumpID(ulExportedID);

	if (pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName)
	{
		strcpy(szTemp, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName);
		if( !ExportClump( szTemp, pcsAgcdCharacterTemplate->m_pDefaultArmourClump, pcsAgpdCharacterTemplate->m_lID, AC_EXPORT_HD_CHAR_DEF_ARMOUR, AC_EXPORT_EXT_CLUMP,
						MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cCharacterClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdCharacterTemplate->SetDefArmourID(ulExportedID);
	}

	if (pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName)
	{
		CHAR szPickClumpPathName[256];
		sprintf( szPickClumpPathName, "%s%s\\%s", m_szSrcPath, MODEL_DFF_RELATIVE_PATH, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName );

		RpClump *pstPickingClump = RwUtilCreateClump(szPickClumpPathName);
		if (!pstPickingClump)
			m_funcLog( "Failed - RwUtilCreateClump()" );

		strcpy(szTemp, pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName);
		if( !ExportClump( szTemp, pstPickingClump, pcsAgpdCharacterTemplate->m_lID, AC_EXPORT_HD_CHAR_CHAR_PICKING_DATA, AC_EXPORT_EXT_CLUMP,
						MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cCharacterClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
			RpClumpDestroy( pstPickingClump );
			return FALSE;
		}

		RpClumpDestroy(pstPickingClump);

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdCharacterTemplate->SetPickClumpID(ulExportedID);
	}

	if( pcsAgcdCharacterTemplate->m_pcsDefaultHeadData )
	{
		INT32	nHairNum = pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->GetHairNum();
		for( INT32 i = 0; i < nHairNum; i++ )
		{
			CHAR szCustomizeHairClumpPathName[256];
			sprintf( szCustomizeHairClumpPathName, "%s%s\\%s", m_szSrcPath, MODEL_CUSTOMIZE_HAIR_RELATIVE_PATH, pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[i].c_str() );

			RpClump *pstHairClump = RwUtilCreateClump( szCustomizeHairClumpPathName );
			if( !pstHairClump )
			{
				m_funcLog( "Failed - RwUtilCreateClump()" );
				return FALSE;
			}

			int nID = pcsAgpdCharacterTemplate->m_lID*0x100;
			strcpy( szTemp, pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[i].c_str() );
			if( !ExportClump( szTemp, pstHairClump, nID+i, AC_EXPORT_HD_CHAR_CUSTOMIZE, AC_EXPORT_EXT_CLUMP,
							MODEL_CUSTOMIZE_HAIR_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cCharacterClump ) )
			{
				m_funcLog( "Failed - ExportClump()" );
				RpClumpDestroy(pstHairClump);
				return FALSE;
			}

			RpClumpDestroy(pstHairClump);
		}

		int nFaceNum = pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->GetFaceNum();
		for( INT32 i = 0; i < nFaceNum; i++ )
		{
			CHAR szCustomizeFaceClumpPathName[256];
			sprintf( szCustomizeFaceClumpPathName, "%s%s\\%s", m_szSrcPath, MODEL_CUSTOMIZE_FACE_RELATIVE_PATH, pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[i].c_str() );

			RpClump *pstFaceClump = RwUtilCreateClump( szCustomizeFaceClumpPathName );
			if( !pstFaceClump )
			{
				m_funcLog( "Failed - RwUtilCreateClump()" );
				return FALSE;
			}

			int nID = pcsAgpdCharacterTemplate->m_lID*0x200;
			strcpy( szTemp, pcsAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[i].c_str() );
			if( !ExportClump( szTemp, pstFaceClump, nID+i, AC_EXPORT_HD_CHAR_CUSTOMIZE, AC_EXPORT_EXT_CLUMP,
							MODEL_CUSTOMIZE_FACE_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cCharacterClump ) )
			{
				m_funcLog( "Failed - ExportClump()" );
				RpClumpDestroy(pstFaceClump);
				return FALSE;
			}

			RpClumpDestroy(pstFaceClump);
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportCharacterAnimation( AgcmSkill *pcsAgcmSkill, AgcmCharacter *pcsAgcmCharacter, AgpdCharacterTemplate *pcsAgpdCharacterTemplate, AgcdCharacterTemplate *pcsAgcdCharacterTemplate, AgcdSkillAttachTemplateData *pcsAgcdSkillData )
{
	INT32	lType		= 0;
	INT32	lType2		= 0;
	INT32	lIndex		= 0;
	UINT32	ulID		= 0;
	INT32	lMaxType2	= pcsAgcmCharacter->GetAnimType2Num(pcsAgcdCharacterTemplate);

	AgcdAnimData2		*pcsCurrentAnimData	= NULL;
	ACA_AttachedData	*pcsAttachedData	= NULL;

	CHAR	szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	for (lType = AGCMCHAR_ANIM_TYPE_WAIT; lType < AGCMCHAR_MAX_ANIM_TYPE; ++lType)
	{
		for (lType2 = 0; lType2 < lMaxType2; ++lType2)
		{
			if (	(!pcsAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]) ||
					(!pcsAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation)	)
				continue;

			pcsCurrentAnimData	= pcsAgcdCharacterTemplate->m_pacsAnimationData[lType][lType2]->m_pcsAnimation->m_pcsHead;

			lIndex	= 0;
			while (pcsCurrentAnimData)
			{
				if (pcsCurrentAnimData->m_pszRtAnimName)
				{
					sprintf(szTemp, "%.*x%.*x%.*x%.*x", 4, pcsAgpdCharacterTemplate->m_lID, 1, lType, 1, lType2, 1, lIndex);
					ulID	= strtoul(szTemp, NULL, 16);

					strcpy(szTemp, pcsCurrentAnimData->m_pszRtAnimName);

					if (!ExportFile( szTemp, AC_EXPORT_HD_CHAR_ANIM_ANIMATION, AC_EXPORT_EXT_ANIMATION, MODEL_ANIMATION_RELATIVE_PATH, ulID, m_cCharacterAnimation ) )
					{
						m_funcLog( "Failed - ExportFile()" );
						return FALSE;
					}

					_StringCpy(&pcsCurrentAnimData->m_pszRtAnimName, szTemp, TRUE);

					pcsAttachedData = (ACA_AttachedData *)(pcsAgcmCharacter->GetAgcaAnimation2()->GetAttachedData( AGCD_CHARACTER_ANIMATION_ATTACHED_DATA_KEY, pcsCurrentAnimData ) );
					if (!pcsAttachedData)
					{
						m_funcLog( "Failed - GetAttachedData()" );
						return FALSE;
					}

					if( pcsAttachedData->m_pcsBlendingData && pcsAttachedData->m_pcsBlendingData->m_pszRtAnimName )
					{
						sprintf(szTemp, "%.*x%.*x%.*x%.*x", 4, pcsAgpdCharacterTemplate->m_lID, 1, lType, 1, lType2, 1, lIndex);
						ulID	= strtoul(szTemp, NULL, 16);

						strcpy(szTemp, pcsAttachedData->m_pcsBlendingData->m_pszRtAnimName);

						if (!ExportFile( szTemp, AC_EXPORT_HD_CHAR_ANIM_BLEND_ANIM, AC_EXPORT_EXT_ANIMATION, MODEL_ANIMATION_RELATIVE_PATH, ulID, m_cCharacterAnimation )	)
						{
							m_funcLog( "Failed - ExportFile()" );
							return FALSE;
						}

						_StringCpy(&pcsAttachedData->m_pcsBlendingData->m_pszRtAnimName, szTemp, TRUE);
					}

					if ( pcsAttachedData->m_pcsSubData && pcsAttachedData->m_pcsSubData->m_pszRtAnimName )
					{
						sprintf(szTemp, "%.*x%.*x%.*x%.*x", 4, pcsAgpdCharacterTemplate->m_lID, 1, lType, 1, lType2, 1, lIndex);
						ulID	= strtoul(szTemp, NULL, 16);

						strcpy(szTemp, pcsAttachedData->m_pcsSubData->m_pszRtAnimName);

						if (!ExportFile( szTemp, AC_EXPORT_HD_CHAR_ANIM_SUB_ANIM, AC_EXPORT_EXT_ANIMATION, MODEL_ANIMATION_RELATIVE_PATH, ulID, m_cCharacterAnimation )	)
						{
							m_funcLog( "Failed - ExportFile()" );
							return FALSE;
						}

						_StringCpy(&pcsAttachedData->m_pcsSubData->m_pszRtAnimName, szTemp, TRUE);
					}
				}

				++lIndex;
				pcsCurrentAnimData	= pcsCurrentAnimData->m_pcsNext;
			}
		}
	}

	INT32	lVisualInfo		= 0;
	INT32	lMaxVisualInfo	= pcsAgcmSkill->GetNumVisualInfo(pcsAgcdCharacterTemplate);
	if (pcsAgcdSkillData->m_pacsSkillVisualInfo)
	{
		for (lVisualInfo = 0; lVisualInfo < lMaxVisualInfo; ++lVisualInfo)
		{
			if (!pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo])
				continue;

			for (lType2 = 0; lType2 < lMaxType2; ++lType2)
			{
				if (!pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2])
					continue;

				if (	(pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2]->m_pcsAnimation) &&
						(pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2]->m_pcsAnimation->m_pcsHead) &&
						(pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
				{
					sprintf(szTemp, "%.*x%.*x%.*x", 5, pcsAgpdCharacterTemplate->m_lID, 1, lVisualInfo, 1, lType2);
					ulID	= strtoul(szTemp, NULL, 16);

					strcpy( szTemp, pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName );

					if (!ExportFile( szTemp, AC_EXPORT_HD_CHAR_ANIM_SKILL_ANIM, AC_EXPORT_EXT_ANIMATION, MODEL_ANIMATION_RELATIVE_PATH, ulID, m_cCharacterAnimation )	)
					{
						m_funcLog( "Failed - ExportFile()" );
						return FALSE;
					}

					_StringCpy( &pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_pacsAnimation[lType2]->m_pcsAnimation->m_pcsHead->m_pszRtAnimName, szTemp, TRUE );
				}

				DEF_SAFEDELETE( pcsAgcdSkillData->m_pacsSkillVisualInfo[lVisualInfo]->m_paszInfo[lType2] );	// Export하면 필요없으므로 delete한다.
			}
		}
	}

	return TRUE;
}

class functrExportSocailAnim
{
public:
	functrExportSocailAnim(AgcmExportResource* pAgcmExport, CHAR* path) : pAgcmExport_(pAgcmExport), path_(path)	{	};
	void operator () (AgcdAnimData2* panim)
	{
		pAgcmExport_->_CopyFile( path_, panim->m_pszRtAnimName, FALSE );
	}

private:
	AgcmExportResource* pAgcmExport_;
	AcCallbackData1		outputCB_;
	CHAR*				path_;
};

BOOL AgcmExportResource::ExportCharacterSocialAnimation( AgcmEventEffect *pcsAgcmEventEffect  )
{
	std::vector<AgcdAnimData2*>	container;
	pcsAgcmEventEffect->ForXport_GetAllSocialAnim(container);
	std::for_each( container.begin() , container.end() , functrExportSocailAnim( this, MODEL_ANIMATION_RELATIVE_PATH ) );

	return TRUE;
}

//BOOL AgcmExportResource::ExportItemTemplate( AgpmItem *pcsAgpmItem, AgcmItem *pcsAgcmItem, CHAR *pszItemTemplate, vector<INT32>* pvectorWishList )
//{
//	CHAR	szOutput[256];
//	memset(szOutput, 0, sizeof (CHAR) * 256);
//
//	for(AgpaItemTemplate::iterator it = pcsAgpmItem->csTemplateAdmin.begin(); it != pcsAgpmItem->csTemplateAdmin.end(); ++it)
//	{
//		AgpdItemTemplate* pcsAgpdItemTemplate = it->second;
//
//		if( pvectorWishList )
//		{
//			// 지정한 녀석만 익스포트하게
//			BOOL bFound = FALSE;
//			for( vector<INT32>::iterator iter = pvectorWishList->begin(); iter != pvectorWishList->end(); ++iter )
//			{
//				if( pcsAgpdItemTemplate->m_lID == *iter )
//				{
//					bFound = TRUE;
//					break;
//				}
//			}
//
//			if( !bFound )		continue;
//		}
//		
//		AgcdItemTemplate* pcsAgcdItemTemplate = pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);
//		if( !pcsAgcdItemTemplate )
//		{
//			m_funcLogFormat( "-%s-", pcsAgpdItemTemplate->m_szName );
//			m_funcLog( "Failed - pcsAgcmItem->GetTemplateData()" );
//			continue;
//		}
//
//		pcsAgcdItemTemplate->m_lRefCount = 1;
//		pcsAgcmItem->LoadTemplateData( pcsAgpdItemTemplate, pcsAgcdItemTemplate, FALSE );
//
//		if( !ExportItemClump( pcsAgpdItemTemplate, pcsAgcdItemTemplate ) )
//		{
//			m_funcLogFormat( "-%s-", pcsAgpdItemTemplate->m_szName );
//			m_funcLog( "Failed - ExportItemClump()" );
//		}
//
//		if( !ExportItemUI( pcsAgpdItemTemplate, pcsAgcdItemTemplate ) )
//		{
//			m_funcLogFormat( "-%s-", pcsAgpdItemTemplate->m_szName );
//			m_funcLog( "Failed - ExportItemUI()" );
//		}
//
//		// 플래그에서 프리라이트를 끈타.
//		pcsAgcdItemTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_PRE_LIGHT;
//
//		pcsAgcmItem->ReleaseTemplateData( pcsAgpdItemTemplate, pcsAgcdItemTemplate, FALSE );
//		pcsAgcmItem->DeleteTemplatePreData( pcsAgcdItemTemplate );
//	}
//
//	// 새로운 ini를 작성한다.
//	if (!pcsAgpmItem->StreamWriteTemplate( "INI\\ItemTemplateAll.ini", TRUE ) )
//	{
//		m_funcLog( "Failed - pcsAgpmItem->StreamWriteTemplate()" );
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL AgcmExportResource::ExportItemTemplates( AgpmItem* ppmItem, AgcmItem* pcmItem, char* pTargetPath, char* pEntryFileName )
{
	AgpaItemTemplate::iterator Iter;
	Iter = ppmItem->csTemplateAdmin.begin();

	while( Iter != ppmItem->csTemplateAdmin.end() )
	{
		AgpdItemTemplate* ppdItemTemplate = Iter->second;
		AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItemTemplate );
		if( !pcdItemTemplate )
		{
			m_funcLogFormat( "-%s-", ppdItemTemplate->m_szName );
			m_funcLog( "Failed - pcsAgcmItem->GetTemplateData()" );
			continue;
		}

		pcdItemTemplate->m_lRefCount = 1;
		pcmItem->LoadTemplateData( ppdItemTemplate, pcdItemTemplate, FALSE );

		if( !ExportItemClump( ppdItemTemplate, pcdItemTemplate ) )
		{
			m_funcLogFormat( "-%s-", ppdItemTemplate->m_szName );
			m_funcLog( "Failed - ExportItemClump()" );
		}

		if( !ExportItemUI( ppdItemTemplate, pcdItemTemplate ) )
		{
			m_funcLogFormat( "-%s-", ppdItemTemplate->m_szName );
			m_funcLog( "Failed - ExportItemUI()" );
		}

		// 플래그에서 프리라이트를 끈타.
		pcdItemTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_PRE_LIGHT;
		pcmItem->ReleaseTemplateData( ppdItemTemplate, pcdItemTemplate, FALSE );
		pcmItem->DeleteTemplatePreData( pcdItemTemplate );

		Iter++;
	}

	// 새로운 ini를 작성한다.
	CHAR szDestIniPath[ 256 ] = { 0, };
	sprintf_s( szDestIniPath, 256, "%s%s", m_szDestPath, pTargetPath );

	CHAR szDestEntryFile[ 256 ] = { 0, };
	sprintf_s( szDestEntryFile, 256, "%s%s", m_szDestPath, pEntryFileName );
	if( !ppmItem->StreamWriteTemplates( szDestIniPath, szDestEntryFile, TRUE ) )
	{
		m_funcLog( "Failed - ppmItem->StreamWriteTemplates()" );
		return FALSE;
	}

	// 통파일 ini를 작성한다.
	char strAllFileName[ 256 ] = { 0, };
	sprintf_s( strAllFileName, sizeof( char ) * 256, "%s\\INI\\ItemTemplateAll.ini", m_szDestPath );
	if (!ppmItem->StreamWriteTemplate( strAllFileName, TRUE ) )
	{
		m_funcLog( "Failed - pcsAgpmItem->StreamWriteTemplate()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportItemClump( AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate  )
{
	if (!CheckLoadedClump(pcsAgcdItemTemplate))
	{
		m_funcLog( "Failed - CheckLoadedClump()" );
		return FALSE;
	}

	if (!pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName)
	{
		m_funcLog( "Invalid - pcsAgcdItemTemplate->m_szBaseDFFName" );
		return FALSE;
	}

	UINT32	ulExportedID	= 0;
	CHAR	szTemp[256];

	strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName);

	if (!ExportClump( szTemp, pcsAgcdItemTemplate->m_pstBaseClump, pcsAgpdItemTemplate->m_lID, AC_EXPORT_HD_CHAR_BASE_ITEM, AC_EXPORT_EXT_CLUMP,
					MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cItemClump ) )
	{
		m_funcLog( "Failed - ExportClump()" );
		return FALSE;
	}

	ulExportedID	= strtoul(szTemp + 1, NULL, 16);
	pcsAgcdItemTemplate->SetBaseDFFID(ulExportedID);

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName)
	{
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName);
		if (!ExportClump( szTemp, pcsAgcdItemTemplate->m_pstSecondClump, pcsAgpdItemTemplate->m_lID, AC_EXPORT_HD_CHAR_SECOND_ITEM, AC_EXPORT_EXT_CLUMP,
						MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cItemClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetSecondDFFID(ulExportedID);
	}

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName)
	{
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName);
		if (!ExportClump( szTemp, pcsAgcdItemTemplate->m_pstFieldClump, pcsAgpdItemTemplate->m_lID, AC_EXPORT_HD_CHAR_FILED_ITEM, AC_EXPORT_EXT_CLUMP,
						MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cItemClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetFieldDFFID(ulExportedID);
	}

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName)
	{
		CHAR szPickClumpPathName[256];
		sprintf( szPickClumpPathName, "%s%s\\%s", m_szSrcPath, MODEL_DFF_RELATIVE_PATH, pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName	);

		RpClump *pstPickingClump = RwUtilCreateClump(szPickClumpPathName);
		if (!pstPickingClump)
		{
			m_funcLog( "Failed - RwUtilCreateClump()" );
			return FALSE;
		}

		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName);

		if (!ExportClump( szTemp, pstPickingClump, pcsAgpdItemTemplate->m_lID, AC_EXPORT_HD_CHAR_ITEM_PICKING_DATA, AC_EXPORT_EXT_CLUMP,
						MODEL_DFF_RELATIVE_PATH, MODEL_TEXTURE_RELATIVE_PATH, MODEL_TEXTURE_MQ_RELATIVE_PATH, MODEL_TEXTURE_LQ_RELATIVE_PATH, m_cItemClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
			RpClumpDestroy(pstPickingClump);
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetPickDFFID(ulExportedID);

		RpClumpDestroy(pstPickingClump);
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportItemUI(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pcsAgcdItemTemplate )
{
	RwTextureSetMipmapping(FALSE);
	RwTextureSetAutoMipmapping(FALSE);

	if (!pcsAgcdItemTemplate->m_pcsPreData)
	{
		m_funcLog( "Invalid - pcsAgcdItemTemplate->m_pcsPreData" );
		return FALSE;
	}

	UINT32			ulExportedID = 0;
	AcuTextureType	eTextureType = ACUTEXTURE_TYPE_NONE;
	CHAR			szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName)
	{	
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszTextureName);

		if (!ExportTextureDictionary( szTemp, ITEM_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_ITEM_ITEM, pcsAgpdItemTemplate->m_lID, m_cTextureItemUI ) )
		{
			m_funcLog( "Failed - ExportTextureDictionary()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetTextureID(ulExportedID);
	}

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName)
	{
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszSmallTextureName);

		if (!ExportTextureDictionary( szTemp, ITEM_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_ITEM_SMALL, pcsAgpdItemTemplate->m_lID, m_cTextureItemUI ) )
		{
			m_funcLog( "Failed - ExportTextureDictionary()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetSmallTextureID(ulExportedID);
	}

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName)
	{
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName);

		if (!ExportTextureDictionary( szTemp, ITEM_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_ITEM_DUR_5_UNDER, pcsAgpdItemTemplate->m_lID, m_cTextureItemUI ) )
		{
			m_funcLog( "Failed - ExportTextureDictionary()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetDur5UnderTextureID(ulExportedID);
	}

	if (pcsAgcdItemTemplate->m_pcsPreData->m_pszDurabilityZeroTextureName)
	{
		strcpy(szTemp, pcsAgcdItemTemplate->m_pcsPreData->m_pszDurability5UnderZeroTextureName);

		if (!ExportTextureDictionary( szTemp, ITEM_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_ITEM_DUR_ZERO, pcsAgpdItemTemplate->m_lID, m_cTextureItemUI ) )
		{
			m_funcLog( "Failed - ExportTextureDictionary()" );
			return FALSE;
		}

		ulExportedID	= strtoul(szTemp + 1, NULL, 16);
		pcsAgcdItemTemplate->SetDurZeroTextureID(ulExportedID);
	}

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	return TRUE;
}

BOOL AgcmExportResource::ExportObjectTemplate( ApmObject *pcsApmObject, AgcmObject *pcsAgcmObject, CHAR *pszObjectTemplate )
{
	CHAR	szOutput[256];
	memset(szOutput, 0, sizeof (CHAR) * 256);

	INT32	nTotalTemplate = pcsApmObject->m_clObjectTemplates.GetObjectCount();
	INT32	nCurrentTemplate = 0;

	INT32	lTID = 0;
	for (	ApdObjectTemplate *pcsApdObjectTemplate = pcsApmObject->GetObjectTemplateSequence(&lTID);
			pcsApdObjectTemplate;
			pcsApdObjectTemplate = pcsApmObject->GetObjectTemplateSequence(&lTID) ,
			nCurrentTemplate ++)
	{
		AgcdObjectTemplate *pcsAgcdObjectTemplate = pcsAgcmObject->GetTemplateData(pcsApdObjectTemplate);
		if (!pcsAgcdObjectTemplate)
		{
			m_funcLogFormat( "-%s-", pcsApdObjectTemplate->m_szName );
			m_funcLogFormat( "Failed - pcsAgcmObject->GetTemplateData() TID(%d) AgcdObjectTemplate를 가져올 수 없다!", pcsApdObjectTemplate->m_lID );
			continue;
		}

		memset(szOutput, 0, sizeof (CHAR) * 256);

		pcsAgcdObjectTemplate->m_lRefCount = 1;
		if (!pcsAgcmObject->LoadTemplateData(pcsAgcdObjectTemplate, FALSE, szOutput))
		{
			m_funcLogFormat( "-%s-", pcsApdObjectTemplate->m_szName );
			m_funcLogFormat( "%s TID(%d) Object Template 읽기 실패!", szOutput, pcsApdObjectTemplate->m_lID );
			continue;
		}

		if (!ExportObjectData( pcsApdObjectTemplate, pcsAgcdObjectTemplate ))
		{
			m_funcLogFormat( "-%s-", pcsApdObjectTemplate->m_szName );
			m_funcLog( "Failed - ExportObjectData()" );
		}

		pcsAgcmObject->ReleaseObjectTemplateData(pcsAgcdObjectTemplate);
	}

	CHAR szDestIniPath[256];
	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszObjectTemplate);
	if (!pcsApmObject->StreamWriteTemplate(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsApmObject->StreamWriteTemplate()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportObjectData( ApdObjectTemplate *pcsApdObjectTemplate, AgcdObjectTemplate *pcsAgcdObjectTemplate  )
{
	UINT32	ulID	= 0;
	CHAR	szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	AgcdObjectTemplateGroupList	*pstList = pcsAgcdObjectTemplate->m_stGroup.m_pstList;
	while (pstList)
	{
		if (!CheckLoadedClump(pcsAgcdObjectTemplate, pstList))
		{
			m_funcLog( "Failed - CheckLoadedClump()" );
			pstList = pstList->m_pstNext;
			continue;
		}
		
		sprintf(szTemp, "%.*x%.*x", 5, pcsApdObjectTemplate->m_lID, 2, pstList->m_csData.m_lIndex);
		ulID	= strtoul(szTemp, NULL, 16);

		if (!ExportClump( pstList->m_csData.m_pszDFFName, pstList->m_csData.m_pstClump, ulID, AC_EXPORT_HD_OBJ_OBJECT, AC_EXPORT_EXT_CLUMP,
						OBJECT_DFF_RELATIVE_PATH, OBJECT_TEXTURE_RELATIVE_PATH, OBJECT_TEXTURE_MQ_RELATIVE_PATH, OBJECT_TEXTURE_LQ_RELATIVE_PATH, m_cObjectClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
		}

		if (	(pstList->m_csData.m_pcsAnimation) &&
				(pstList->m_csData.m_pcsAnimation->m_pcsHead) &&
				(pstList->m_csData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName)	)
		{
			sprintf(szTemp, "%.*x%.*x", 6, pcsApdObjectTemplate->m_lID, 1, pstList->m_csData.m_lIndex);
			ulID	= strtoul(szTemp, NULL, 16);

			strcpy(szTemp, pstList->m_csData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName);

			if (!ExportFile( szTemp, AC_EXPORT_HD_OBJ_ANIM_ANIMATION, AC_EXPORT_EXT_ANIMATION, OBJECT_ANIMATION_RELATIVE_PATH, ulID, m_cObjectAnimation ) )
			{
				m_funcLog( "Failed - ExportFile()" );
			}
			else
			{
				_StringCpy( &pstList->m_csData.m_pcsAnimation->m_pcsHead->m_pszRtAnimName, szTemp, TRUE );
			}
		}

		pstList = pstList->m_pstNext;
	}

	if (pcsAgcdObjectTemplate->m_szCollisionDFFName[0])
	{
		CHAR szCollisionPathName[256];
		sprintf(szCollisionPathName, "%s%s\\%s", m_szSrcPath, OBJECT_DFF_RELATIVE_PATH, pcsAgcdObjectTemplate->m_szCollisionDFFName);
		RpClump *pstCollisionClump = RwUtilCreateClump(szCollisionPathName);
		if (!pstCollisionClump)
		{
			m_funcLog( "Failed - RwUtilCreateClump()" );
			return FALSE;
		}

		if (!ExportClump( pcsAgcdObjectTemplate->m_szCollisionDFFName, pstCollisionClump, pcsApdObjectTemplate->m_lID, AC_EXPORT_HD_OBJ_COLLISION_DATA, AC_EXPORT_EXT_CLUMP,
						OBJECT_DFF_RELATIVE_PATH, OBJECT_TEXTURE_RELATIVE_PATH, OBJECT_TEXTURE_MQ_RELATIVE_PATH, OBJECT_TEXTURE_LQ_RELATIVE_PATH, m_cObjectClump )	)
		{
			m_funcLog( "Failed - ExportClump()" );
		}

		RpClumpDestroy(pstCollisionClump);
	}

	if (pcsAgcdObjectTemplate->m_szPickDFFName[0])
	{
		CHAR szPickClumpPathName[256];
		sprintf(szPickClumpPathName, "%s%s\\%s", m_szSrcPath, OBJECT_DFF_RELATIVE_PATH, pcsAgcdObjectTemplate->m_szPickDFFName);
		RpClump *pstPickingClump = RwUtilCreateClump(szPickClumpPathName);
		if (!pstPickingClump)
		{
			m_funcLogFormat( "Failed - RwUtilCreateClump() TID(%d) %s가 없다!", pcsApdObjectTemplate->m_lID, pcsAgcdObjectTemplate->m_szPickDFFName );
			return FALSE;
		}

		if (!ExportClump( pcsAgcdObjectTemplate->m_szPickDFFName, pstPickingClump, pcsApdObjectTemplate->m_lID, AC_EXPORT_HD_OBJ_PICKING_DATA, AC_EXPORT_EXT_CLUMP,
						OBJECT_DFF_RELATIVE_PATH, OBJECT_TEXTURE_RELATIVE_PATH, OBJECT_TEXTURE_MQ_RELATIVE_PATH, OBJECT_TEXTURE_LQ_RELATIVE_PATH, m_cObjectClump ) )
		{
			m_funcLog( "Failed - ExportClump()" );
		}

		RpClumpDestroy(pstPickingClump);
	}

	// pre-light를 off한다.
	if (pcsAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
		pcsAgcdObjectTemplate->m_lObjectType &= ~ACUOBJECT_TYPE_USE_PRE_LIGHT;

	return TRUE;
}

BOOL AgcmExportResource::ExportSkillTemplate_( AgpmSkill *pcsAgpmSkill, AgcmSkill *pcsAgcmSkill, CHAR *pszSkillTemplate )
{
	INT32	lIndex = 0;
	for( AgpdSkillTemplate *pstAgpdSkillTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex); pstAgpdSkillTemplate; pstAgpdSkillTemplate = pcsAgpmSkill->GetSkillTemplateSequence(&lIndex) )
	{
		AgcdSkillTemplate *pstAgcdSkillTemplate = pcsAgcmSkill->GetADSkillTemplate((PVOID)(pstAgpdSkillTemplate));
		if (!pstAgcdSkillTemplate)
		{
			m_funcLogFormat( "-%s", pstAgpdSkillTemplate->m_szName );
			m_funcLog( "Failed - pcsAgcmSkill->GetADSkillTemplate()" );
			continue;
		}

		if (!ExportSkillTexture(pstAgpdSkillTemplate, pstAgcdSkillTemplate))
		{
			m_funcLogFormat( "-%s", pstAgpdSkillTemplate->m_szName );
			m_funcLog( "Failed - pcsAgcmSkill->ExportSkillTexture()" );
		}
	}
	
	CHAR szDestIniPath[256];

	sprintf(szDestIniPath, "%s%s\\%s", m_szDestPath, INI_RELATIVE_PATH, pszSkillTemplate);
	if (!pcsAgpmSkill->StreamWriteTemplate(szDestIniPath, TRUE))
	{
		m_funcLog( "Failed - pcsAgpmSkill->StreamWriteTemplate" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportSkillTexture( AgpdSkillTemplate *pstAgpdSkillTemplate, AgcdSkillTemplate *pstAgcdSkillTemplate )
{
	if (pstAgcdSkillTemplate->m_pszTextureName)
	{
		if (!ExportTextureDictionary( pstAgcdSkillTemplate->m_pszTextureName, SKILL_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_SKILL_SKILL, pstAgpdSkillTemplate->m_lID, m_cTextureSkill ) )
		{
			m_funcLog( "Failed - ExportTexture()" );
			return FALSE;
		}
	}

	if (pstAgcdSkillTemplate->m_pszSmallTextureName)
	{
		if (!ExportTextureDictionary( pstAgcdSkillTemplate->m_pszSmallTextureName, SKILL_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_SKILL_SMALL, pstAgpdSkillTemplate->m_lID, m_cTextureSkill ) )
		{
			m_funcLog( "Failed - ExportTexture()" );
			return FALSE;
		}
	}

	if (pstAgcdSkillTemplate->m_pszUnableTextureName)
	{
		if (!ExportTextureDictionary( pstAgcdSkillTemplate->m_pszUnableTextureName, SKILL_TEXTURE_RELATIVE_PATH, AC_EXPORT_HD_TEX_SKILL_UNABLE, pstAgpdSkillTemplate->m_lID, m_cTextureSkill ) )
		{
			m_funcLog( "Failed - ExportTexture()" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportEffectData(AgcmEff2 *pAgcmEff2 )
{
	AgcuEffIniExporter	cuEffIniExporter;
	UINT32	ulEID		= 0;
	UINT32	ulTemp		= 0;
	CHAR*	pszName	= NULL;
	CHAR	szTemp[256];
	memset(szTemp, 0, sizeof (CHAR) * 256);

	for( LPEffectSetMapCItr	Itr = pAgcmEff2->ForTool_GetEffSetMng().begin() ; Itr != pAgcmEff2->ForTool_GetEffSetMng().end(); ++Itr )
	{
		LPEFFSET pEffSet = (*Itr).second;
		if( !pEffSet )
		{
			m_funcLog( "Invalid - pEffSet" );
			continue;
		}

		ulEID	= pEffSet->bGetID();
		if( -1 == cuEffIniExporter.bSetEffSet( ulEID ) )
		{
			m_funcLogFormat( "EID(%d)", ulEID );
			m_funcLog( "Load할수 없습니다." );
			continue;
		}

		//tex
		for( int i=0; i<cuEffIniExporter.bGetNumTex(); ++i )
		{
			pszName = cuEffIniExporter.bGetSzTex(i);
			if( !pszName )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - cuEffIniExporter.bGetSzTex()" );
				return FALSE;
			}
			if( !pszName[0] )		continue;
				

			sprintf(szTemp, "%.*x%.*x", 5, ulEID, 2, i);
			ulTemp	= strtoul(szTemp, NULL, 16);

			if (!ExportDDSTexture( pszName, AC_EXPORT_HD_TEX_EFFECT_EFFECT, EFFECT_TEXTURE_RELATIVE_PATH, EFFECT_TEXTURE_MQ_RELATIVE_PATH, EFFECT_TEXTURE_LQ_RELATIVE_PATH, ulTemp, m_cTextureEffect ) )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - ExportTexture()" );
			}
		}

		//mask
		for( int i=0; i<cuEffIniExporter.bGetNumMask(); ++i )
		{
			pszName = cuEffIniExporter.bGetSzMask(i);
			if (!pszName)
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - cuEffIniExporter.bGetSzMask()" );
				return FALSE;
			}
			if( !pszName[0] )		continue;

			sprintf(szTemp, "%.*x%.*x", 5, ulEID, 2, i);
			ulTemp	= strtoul(szTemp, NULL, 16);

			if (!ExportDDSTexture( pszName, AC_EXPORT_HD_TEX_EFFECT_EFFECT, EFFECT_TEXTURE_RELATIVE_PATH, EFFECT_TEXTURE_MQ_RELATIVE_PATH, EFFECT_TEXTURE_LQ_RELATIVE_PATH, ulTemp, m_cTextureEffect ) )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - ExportDDSTexture()" );
			}
	
		}

		//clump
		for( int  i=0; i<cuEffIniExporter.bGetNumClump(); ++i )
		{
			pszName = cuEffIniExporter.bGetSzClump(i);
			if( !pszName )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - cuEffIniExporter.bGetSzClump()" );
				return FALSE;
			}
			if( !pszName[0] )		continue;

			sprintf(szTemp, "%s%s\\%s", m_szSrcPath, EFFECT_OBJECT_RELATIVE_PATH, pszName);

			RpClump *pstClump = RwUtilCreateClump(szTemp);
			if(!pstClump)
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - RwUtilCreateClump()" );
			}

			sprintf(szTemp, "%.*x%.*x", 5, ulEID, 2, i);
			ulTemp	= strtoul(szTemp, NULL, 16);
			if (!ExportClump( pszName, pstClump, ulTemp, AC_EXPORT_HD_EFF_CLUMP_BASE, AC_EXPORT_EXT_CLUMP, 
							EFFECT_OBJECT_RELATIVE_PATH, EFFECT_TEXTURE_RELATIVE_PATH, EFFECT_TEXTURE_MQ_RELATIVE_PATH, EFFECT_TEXTURE_LQ_RELATIVE_PATH, m_cEffectClump ) )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - ExportClump()" );
			}

			RpClumpDestroy(pstClump);
		}

		//RtAnim
		for( int i=0; i<cuEffIniExporter.bGetNumAnim(); ++i )
		{
			pszName = cuEffIniExporter.bGetSzAnim(i);
			if (!pszName)
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - cuEffIniExporter.bGetSzAnim()" );
				return FALSE;
			}
			if( !pszName[0] )		continue;

			sprintf(szTemp, "%.*x%.*x", 5, ulEID, 2, i);
			ulTemp	= strtoul(szTemp, NULL, 16);

			if (!ExportFile( pszName, AC_EXPORT_HD_EFF_ANIM_ANIMATION, AC_EXPORT_EXT_ANIMATION, EFFECT_ANIMATION_RELATIVE_PATH, ulTemp, m_cEffectAnimation ) )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - ExportFile()" );
			}
		}
		//RpSpline
		for( int i=0; i<cuEffIniExporter.bGetNumSpline(); ++i )
		{
			pszName = cuEffIniExporter.bGetSzSpline(i);
			if (!pszName)
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - cuEffIniExporter.bGetSzSpline()" );
				return FALSE;
			}
			if( !pszName[0] )		continue;

			sprintf(szTemp, "%.*x%.*x", 5, ulEID, 2, i);
			ulTemp	= strtoul(szTemp, NULL, 16);

			if (!ExportFile( pszName, AC_EXPORT_HD_EFF_ANIM_SPLINE, AC_EXPORT_EXT_SPLINE, EFFECT_ANIMATION_RELATIVE_PATH, ulTemp, m_cEffectAnimation ) )
			{
				m_funcLogFormat( "EID(%d)", ulEID );
				m_funcLog( "Failed - ExportFile()" );
			}
		}

		sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_INI_NEW_RELATIVE_PATH);
		cuEffIniExporter.bSaveFile( szTemp );
	}

	m_funcLog( "effect file packing" );

	if( T_ISMINUS4( pAgcmEff2->ForTool_MakeEffFile( m_szDestPath, "Effect\\INI\\New\\" ) ) )
	{
		m_funcLog( "Failed - pAgcmEff2->bForTool_MakeEffFile" );

		sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_INI_NEW_RELATIVE_PATH);
		Eff2Ut_ForAllFile( szTemp, ".ini", dele_Eff_ini, NULL );
		return FALSE;
	}
	m_funcLog( "delete all x_eff.ini" );

	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_INI_NEW_RELATIVE_PATH);
	if ( T_ISMINUS4( Eff2Ut_ForAllFile( szTemp, ".ini", dele_Eff_ini, NULL ) ) )
	{
		m_funcLog( "Failed - delete all x_eff.ini" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportFXData(CHAR *pszPath )
{
	CHAR	szFindPath[512], szFileName[256];
	sprintf(szFindPath, "%s%s\\*.%s", m_szSrcPath, pszPath, AER_FX_EXT);

	AuFileFind	csFinder;
	if (!csFinder.FindFile(szFindPath))
	{
		m_funcLog( "Failed - csFinder.FindFile()" );
		return FALSE;
	}

	memset(szFileName, 0, sizeof (CHAR) * 256);
	csFinder.GetFileName(szFileName);

	if (!ExportFile(pszPath, pszPath, szFileName, szFileName))
		m_funcLog( "Failed - ExportFile()" );

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		if (!ExportFile(pszPath, pszPath, szFileName, szFileName))
			m_funcLog( "Failed - ExportFile()" );
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportUIData()
{
	// UI
	m_funcLog( "::UI Data Export중.." );
	ExportFolder( UI_TEXTURE_RELATIVE_PATH );
	m_funcLog( "::UI Data Base Export중.." );
	ExportFolder( UI_BASE_TEXTURE_RELATIVE_PATH );
	m_funcLog( "::미니맵 텍스쳐 익스포트중.." );
	ExportFolder( MINIMAP_TEXTURE_RELATIVE_PATH );
	m_funcLog( "::월드맵 텍스쳐 익스포트중.." );
	ExportFolder( WORLDMAP_TEXTURE_RELATIVE_PATH );

	CreateDirectoryForINIResource();

	for( INT32 lIndex = 0; g_pszINI_UI_Resources[lIndex]; ++lIndex )
		if (!_CopyFile(INI_RELATIVE_PATH, g_pszINI_UI_Resources[lIndex], TRUE))
			m_funcLogFormat( "Failed - %s", g_pszINI_UI_Resources[lIndex] );

	return TRUE;
}

BOOL AgcmExportResource::ExportWorldTexture()
{
	m_funcLog( "::월드 텍스쳐 익스포트중.." );

	if (!ExportTextureDDSFolder(WORLD_TEXTURE_RELATIVE_PATH, WORLD_TEXTURE_RELATIVE_PATH, -2, 1, ACUTEXTURE_COPY_LOAD_TRIAGLE))
	{
		m_funcLog( "Failed - ExportTextureDDSFolder() 1" );
		return FALSE;
	}

	m_funcLog( "::월드 텍스쳐(미디엄) 익스포트중.." );

	if (!ExportTextureDDSFolder(WORLD_TEXTURE_MQ_RELATIVE_PATH, WORLD_TEXTURE_RELATIVE_PATH, -2, 2, ACUTEXTURE_COPY_LOAD_TRIAGLE))
	{
		m_funcLog( "Failed - ExportTextureDDSFolder() 2" );
		return FALSE;
	}

	m_funcLog( "::월드 텍스쳐(로우) 익스포트중.." );

	if (!ExportTextureDDSFolder(WORLD_TEXTURE_LQ_RELATIVE_PATH, WORLD_TEXTURE_RELATIVE_PATH, -2, 3, ACUTEXTURE_COPY_LOAD_TRIAGLE))
	{
		m_funcLog( "Failed - ExportTextureDDSFolder() 3" );
		return FALSE;
	}

	m_funcLog( "::월드 텍스쳐(알파) 익스포트중.." );

	if (!ExportFile(WORLD_TEXTURE_RELATIVE_PATH, WORLD_TEXTURE_RELATIVE_PATH, D_SKIP_FILE_A0001C0T_TIF, D_SKIP_FILE_A0001C0T_TIF))
	{
		m_funcLog( "Failed - ExportFile() 1" );
		return FALSE;
	}

	if (!ExportFile(WORLD_TEXTURE_RELATIVE_PATH, WORLD_TEXTURE_MQ_RELATIVE_PATH, D_SKIP_FILE_A0001C0T_TIF, D_SKIP_FILE_A0001C0T_TIF))
	{
		m_funcLog( "Failed - ExportFile() 2" );
		return FALSE;
	}

	if (!ExportFile(WORLD_TEXTURE_RELATIVE_PATH, WORLD_TEXTURE_LQ_RELATIVE_PATH, D_SKIP_FILE_A0001C0T_TIF, D_SKIP_FILE_A0001C0T_TIF))
	{
		m_funcLog( "Failed - ExportFile() 3" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportEtcTexture()
{
	if (!_CopyDirectory(ETC_TEXTURE_RELATIVE_PATH))
	{
		m_funcLog( "Failed - ExportTextureFolder()" );
		return FALSE;
	}

	if (!_CopyDirectory(NOT_PACKED_TEXTURE_RELATIVE_PATH))
	{
		m_funcLog( "Failed - _CopyDirectory()" );
		return FALSE;
	}

	if (!_CopyDirectory(AMB_OCCL_TEXTURE_RELATIVE_PATH))
		m_funcLog( "Texture\\AmbOccl cannot be copied." );

	return TRUE;
}

BOOL AgcmExportResource::ExportDDSPath( AgcmResourceLoader *pcsAgcmResourceLoader 	)
{
	if (!ExportDDSPath(MODEL_TEXTURE_RELATIVE_PATH, pcsAgcmResourceLoader))		return FALSE;
	if (!ExportDDSPath(MODEL_TEXTURE_MQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(MODEL_TEXTURE_LQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(OBJECT_TEXTURE_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(OBJECT_TEXTURE_MQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(OBJECT_TEXTURE_LQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(EFFECT_TEXTURE_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(EFFECT_TEXTURE_MQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(EFFECT_TEXTURE_LQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(WORLD_TEXTURE_RELATIVE_PATH, pcsAgcmResourceLoader))		return FALSE;
	if (!ExportDDSPath(WORLD_TEXTURE_MQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;
	if (!ExportDDSPath(WORLD_TEXTURE_LQ_RELATIVE_PATH, pcsAgcmResourceLoader))	return FALSE;

	return TRUE;
}

BOOL AgcmExportResource::ExportDDSPath( CHAR *pszPath, AgcmResourceLoader *pcsAgcmResourceLoader  )
{
	pcsAgcmResourceLoader->SetTexturePath(pszPath);
	return ExportTexDictFolder( pszPath, TRUE ) ? FALSE : TRUE;
}

BOOL AgcmExportResource::ReadCharacterTemplate(	 AgpmCharacter *pcsAgpmCharacter,
												 AgcmCharacter *pcsAgcmCharacter,
												 AgcmEventEffect *pcsAgcmEventEffect,
												 CHAR *pszCharacterTemplatePublic,
												 CHAR *pszCharacterTemplateClient,
												 CHAR *pszCharacterTemplateAnimation,
												 CHAR *pszCharacterTemplateCustomize,
												 CHAR *pszCharacterTemplateSkill,
												 CHAR *pszCharacterTemplateSkillSound,
												 CHAR *pszCharacterTemplateEventEffect,
												 AgcmPreLODManager *pcsAgcmPreLODManager,
												 CHAR *pszCharacterPreLOD,
												 AgpmSkill *pcsAgpmSkill,
												 AgcmSkill *pcsAgcmSkill,
												 CHAR *pszSkillTemplate )
{
	m_funcLog( "캐릭터 템플릿을 읽고 있습니다." );

	if (!pcsAgpmCharacter->RemoveAllCharacterTemplate())
	{
		m_funcLog( "Failed - pcsAgpmCharacter->RemoveAllCharacterTemplate()" );
		//return FALSE;
	}

	CHAR	szTemp[1024] = { 0, };
	sprintf(szTemp, "%s%s\\CharacterTemplate", m_szSrcPath, INI_RELATIVE_PATH );
	
	m_funcLog(  "Read Previous Version of Character Templates" );

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplatePublic);

	CHAR	szError[256];
	memset(szError, 0, sizeof (CHAR) * 256);
	if(!pcsAgpmCharacter->StreamReadTemplate( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateClient);
	if(!pcsAgcmCharacter->StreamReadTemplate( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateAnimation);
	if(!pcsAgcmCharacter->StreamReadTemplateAnimation( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateCustomize);
	if(!pcsAgcmCharacter->StreamReadTemplateCustomize( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateSkill);
	if(!pcsAgcmSkill->StreamReadTemplateSkill( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateSkillSound);
	if(!pcsAgcmSkill->StreamReadTemplateSkillSound( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterTemplateEventEffect);
	if(!pcsAgcmEventEffect->StreamReadTemplate( szTemp, szError ) )
	{
		m_funcLog( szError );
		return FALSE;
	}

	// Character Pre LOD도 여기서 읽어들이자
	if (!ReadCharacterPreLOD( pcsAgcmPreLODManager, pszCharacterPreLOD ) )
	{
		m_funcLog( "Failed - ReadCharacterPreLOD()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ReadCharacterPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszCharacterPreLOD )
{
	m_funcLog( "캐릭터 LOD를 읽고 있습니다." );

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszCharacterPreLOD);
	if( !pcsAgcmPreLODManager->CharPreLODStreamRead(szTemp) )
	{
		m_funcLog(  "Failed - pcsAgcmPreLODManager->CharPreLODStreamRead()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ReadItemTemplate(AgpmItem *pcsAgpmItem, CHAR *pszItemTemplate,  AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD )
{
	m_funcLog( "아이템 템플릿을 읽고 있습니다." );

	if (!pcsAgpmItem->DestroyAllTemplate())
	{
		m_funcLog( "Failed - pcsAgpmItem->DestroyAllTemplate()" );
		return FALSE;
	}

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\ItemTemplate", m_szSrcPath, INI_RELATIVE_PATH);
	
	TSO::ReadError	eError = TSO::LE_NO_SEPARATED_TEMPLATE;//TSO::LoadItemTemplateSeperated( *g_pEngine , szTemp , CompatibleStringLogger( pfOutputCallback ) );
	if( eError == TSO::LE_NO_SEPARATED_TEMPLATE )
	{
		m_funcLog( "Read Previous Version of Item Templates" );

		CHAR	szError[256];
		CHAR	szTemp[1024];

		memset(szError, 0, sizeof (CHAR) * 256);
		memset(szTemp, 0, sizeof (CHAR) * 1024);

		sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszItemTemplate);

		if (!pcsAgpmItem->StreamReadTemplate("INI\\ItemTemplateAll.ini", szError, FALSE ))
		{
			if (!pcsAgpmItem->StreamReadTemplates("INI\\ItemTemplate", "INI\\ItemTemplateEntry.ini", szError))
			{
				m_funcLog( szError );
				return FALSE;
			}
		}

		if (!ReadItemPreLOD(pcsAgcmPreLODManager, pszItemPreLOD ))
		{
			m_funcLog( "Failed - ReadItemPreLOD()" );
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		return eError == TSO::LE_NOERROR ? TRUE : FALSE;
	}
}

BOOL AgcmExportResource::ReadItemPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszItemPreLOD )
{
	m_funcLog( "아이템 LOD를 읽고 있습니다." );

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszItemPreLOD);
	if (!pcsAgcmPreLODManager->ItemPreLODStreamRead(szTemp))
	{
		m_funcLog( "Failed - pcsAgcmPreLODManager->CharPreLODStreamRead()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ReadObjectTemplate(ApmObject *pcsApmObject, CHAR *pszObjectTemplate, AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszObjectPreLOD )
{
	m_funcLog( "오브젝트 템플릿을 읽고 있습니다." );

	if (!pcsApmObject->DestroyAllTemplate())
	{
		m_funcLog( "Failed - pcsApmObject->DestroyAllTemplate()" );
		return FALSE;
	}

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\ObjectTemplate", m_szSrcPath, INI_RELATIVE_PATH);
	
	TSO::ReadError	eError = TSO::LE_NO_SEPARATED_TEMPLATE ;// = TSO::LoadObjectTemplateSeperated( *g_pEngine , szTemp , CompatibleStringLogger( pfOutputCallback ) );
	if( eError == TSO::LE_NO_SEPARATED_TEMPLATE )
	{
		m_funcLog( "Read Previous Version of Object Templates" );

		CHAR	szTemp[1024];
		memset(szTemp, 0, sizeof (CHAR) * 1024);
		sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszObjectTemplate);

		CHAR	szError[256];
		memset(szError, 0, sizeof (CHAR) * 256);
		if (!pcsApmObject->StreamReadTemplate(szTemp, NULL, NULL, szError))
		{
			m_funcLog( szError );
			return FALSE;
		}

		if (!ReadObjectPreLOD(pcsAgcmPreLODManager, pszObjectPreLOD))
		{
			m_funcLog( "Failed - ReadObjectPreLOD()" );
			return FALSE;
		}

		return TRUE;
	}
	else
		return eError== TSO::LE_NOERROR ? TRUE : FALSE;
}

BOOL AgcmExportResource::ReadObjectPreLOD(AgcmPreLODManager *pcsAgcmPreLODManager, CHAR *pszObjectPreLOD )
{
	m_funcLog( "오브젝트 LOD를 읽고 있습니다." );

	CHAR	szTemp[1024];
	memset(szTemp, 0, sizeof (CHAR) * 1024);
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszObjectPreLOD);

	if (!pcsAgcmPreLODManager->ObjectPreLODStreamRead(szTemp))
	{
		m_funcLog( "Failed - pcsAgcmPreLODManager->CharPreLODStreamRead()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ReadSkillTemplate(AgpmSkill *pcsAgpmSkill, CHAR *pszSkillTemplate )
{
	m_funcLog( "스킬 템플릿을 읽고 있습니다." );

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszSkillTemplate);
	if (!pcsAgpmSkill->RemoveAllSkillTemplate())
	{
		m_funcLog(  "Failed - pcsAgpmSkill->RemoveAllSkillTemplate()" );
		return FALSE;
	}

	CHAR	szError[256];
	memset( szError, 0, sizeof (CHAR) * 256 );
	if (!pcsAgpmSkill->StreamReadTemplate(szTemp, szError))
	{
		m_funcLog( szError );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ReadUIINI(AgcmUIManager2 *pcsAgcmUIManager2, CHAR *pszUIMessage, CHAR *pszUIINI )
{
	m_funcLog( "UI 파일을 읽고 있습니다..." );

	CHAR	szTemp[512];
	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszUIMessage);
	if (!pcsAgcmUIManager2->StreamReadUIMessage(szTemp, FALSE))
	{
		m_funcLog( "Failed - pcsAgcmUIManager2->StreamReadUIMessage()" );
		return FALSE;
	}

	sprintf(szTemp, "%s%s\\%s", m_szSrcPath, INI_RELATIVE_PATH, pszUIINI);
	if (!pcsAgcmUIManager2->StreamRead(szTemp, FALSE))
	{
		m_funcLog( "Failed - pcsAgcmUIManager2->StreamRead()" );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportClump( CHAR *pszDffName, RpClump *pstClump, UINT32 ulID, CHAR *pszHeader, CHAR *pszExtension, CHAR *pszDffPath,
									 CHAR *pszHTexturePath, CHAR *pszMTexturePath, CHAR *pszLTexturePath, CStringFinder& cStringFinder )
{
	char* szFind = cStringFinder.GetData( pszDffName );
	if( szFind )
	{
		strcpy( pszDffName, szFind );
		return TRUE;
	}

	m_csExportDFF.InitNameRule( pszHeader, ulID, pszExtension, 1, 7, 3 );
	m_csExportDFF.SetPath( m_szSrcPath, m_szDestPath, pszDffPath, pszHTexturePath, pszMTexturePath, pszLTexturePath );

	CHAR	szNewDffName[256];
	if( !m_csExportDFF.ExportDFF( pstClump, szNewDffName ) )
	{
		m_funcLogFormat( "Failed - m_csExportDFF.ExportDFF() %s", pszDffName );
		return FALSE;
	}
	cStringFinder.Add( pszDffName, szNewDffName );
	strcpy( pszDffName, szNewDffName );
	return TRUE;
}

BOOL AgcmExportResource::ExportFile( CHAR *pszSrcPath, CHAR *pszDestPath, CHAR *pszSrcFileName, CHAR *pszDestFileName )
{
	CHAR szSrcFullPath[256];
	CHAR szDestFullPath[256];

	sprintf(szSrcFullPath, "%s%s\\%s", m_szSrcPath, pszSrcPath, pszSrcFileName);
	sprintf(szDestFullPath, "%s%s\\%s", m_szDestPath, pszDestPath, pszDestFileName);

	if( !CopyFile( szSrcFullPath, szDestFullPath, FALSE ) )
	{
		m_funcLogFormat( "CopyFile failed - szSrcFullPath : %s, szDestFullPath : %s", szSrcFullPath, szDestFullPath );
		return FALSE;
	}

	if (FILE_ATTRIBUTE_READONLY == (GetFileAttributes(szDestFullPath) & FILE_ATTRIBUTE_READONLY))
		SetFileAttributes(szDestFullPath, (GetFileAttributes(szDestFullPath) & ~FILE_ATTRIBUTE_READONLY));

	return TRUE;
}

BOOL AgcmExportResource::ExportFile( CHAR *pszFileName, CHAR *pszHeader, CHAR *pszExtension, CHAR *pszPath, UINT32 ulID, CStringFinder& cStringFinder )
{
	CHAR* pFind = cStringFinder.GetData( pszFileName );
	if( pFind )
		strcpy( pszFileName, pFind );
	else
	{
		CHAR	szNewFileName[256];
		sprintf( szNewFileName, "%s%.*x.%s", pszHeader, AC_EXPORT_ID_LENGTH, ulID, pszExtension );

		if( !ExportFile( pszPath, pszPath, pszFileName, szNewFileName ) )
		{
			m_funcLogFormat( "Failed - ExportFile() %s", pszFileName );
		}

		cStringFinder.Add( pszFileName, szNewFileName );
		strcpy( pszFileName, szNewFileName );
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportDDSTexture(CHAR *pszTextureName, CHAR *pszPath, CHAR *pszHeader, UINT32 ulID )
{
	CHAR	szExportTexture[512];
	sprintf( szExportTexture, "%s%s\\%s%.*x.%s", m_szDestPath, pszPath, pszHeader, AC_EXPORT_ID_LENGTH, ulID, AC_EXPORT_EXT_TEXTURE_DDS );
	if( !AcuTexture::RwD3D9DDSTextureWrite( pszTextureName, szExportTexture, -1, 1, ACUTEXTURE_COPY_LOAD_LINEAR ) )
	{
		m_funcLogFormat( "Failed - AcuTexture::RwD3D9DDSTextureWrite() %s", pszTextureName );
		return FALSE;
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportDDSTexture( CHAR *pszTextureName, CHAR *pszHeader, CHAR *pszHTexturePath, CHAR *pszMTexturePath, CHAR *pszLTexturePath, UINT32 ulID, CStringFinder& cStringFinder )
{
	char* pFind = cStringFinder.GetData( pszTextureName );
	if( pFind )
		strcpy( pszTextureName, pFind );
	else
	{
		ExportDDSTexture( pszTextureName, pszHTexturePath, pszHeader, ulID );
		ExportDDSTexture( pszTextureName, pszMTexturePath, pszHeader, ulID );
		ExportDDSTexture( pszTextureName, pszLTexturePath, pszHeader, ulID );

		CHAR	szExportedTexture[256];
		sprintf( szExportedTexture, "%s%.*x", pszHeader, AC_EXPORT_ID_LENGTH, ulID );

		cStringFinder.Add( pszTextureName, szExportedTexture );
		strcpy( pszTextureName, szExportedTexture );
	}
	
	return TRUE;
}

BOOL AgcmExportResource::ExportTextureDictionary( CHAR *pszTextureName, CHAR *pszPath, CHAR *pszHeader, UINT32 ulID, CStringFinder& cStringFinder )
{
	CHAR* pFind = cStringFinder.GetData( pszTextureName );
	if( pFind )
		strcpy( pszTextureName, pFind );
	else
	{
		CHAR	szExportTexture[512];
		if( pszHeader )
			sprintf( szExportTexture, "%s%s\\%s%.*x", m_szDestPath, pszPath, pszHeader, AC_EXPORT_ID_LENGTH, ulID );
		else
		{
			CHAR	szFileName[256];
			_splitpath( pszTextureName, NULL, NULL, szFileName, NULL );
			sprintf( szExportTexture, "%s%s\\%s", m_szDestPath, pszPath, szFileName );
		}

		AcuTextureType	eTextureType = _GetTextureType(pszTextureName);
		if( !AcuTexture::RwTextureWriteWithTexDict( pszTextureName, szExportTexture, eTextureType, TRUE, FALSE ) )
		{
			m_funcLogFormat( "Failed - AcuTexture::RwTextureWriteWithTexDict() %s", pszTextureName );
			return FALSE;
		}

		if( pszHeader )
			sprintf( szExportTexture, "%s%.*x", pszHeader, AC_EXPORT_ID_LENGTH, ulID );
		else
		{
			CHAR	szFileName[256];
			_splitpath( pszTextureName, NULL, NULL, szFileName, NULL );
			strcpy( szExportTexture, szFileName );
		}

		cStringFinder.Add( pszTextureName, szExportTexture );
		strcpy( pszTextureName, szExportTexture );
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportTexDictFolder(CHAR *pszPath, BOOL bUseMipmap )
{
	CHAR	szFileName[256], szName[256], szExtension[256], szExportFileName[512], szSourceFullPath[512], szFindPath[512];
	sprintf(szFindPath, "%s%s\\", m_szSrcPath, pszPath);
	
	CHAR	szOldPath[1024*2] = {'\0',};
	strcpy( szOldPath, RwImageGetPath () );
	RwImageSetPath(szFindPath);

	strcat( szFindPath, "*.*" );

	AuFileFind	csFinder;
	if (!csFinder.FindFile(szFindPath))
		return FALSE;

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		_splitpath(szFileName, NULL, NULL, szName, szExtension);
		sprintf(szExportFileName, "%s%s\\%s.dds", m_szDestPath, pszPath, szName);
		if ( !AcuTexture::RwTextureWriteWithTexDict( szFileName, szExportFileName, ACUTEXTURE_TYPE_DDS, TRUE, bUseMipmap ) )
		{
			sprintf(szSourceFullPath, "%s%s\\%s", m_szSrcPath, pszPath, szFileName);
			sprintf(szExportFileName, "%s%s\\%s%s", m_szDestPath, pszPath, szName, szExtension);

			if (CopyFile(szSourceFullPath, szExportFileName, FALSE))
				m_funcLogFormat( "Failed - AcuTexture::RwTextureWriteWithTexDict() %s", szFileName );
		}
	}

	RwImageSetPath(szOldPath);

	return TRUE;
}

BOOL AgcmExportResource::ExportTextureDictFolder(CHAR *pszPath, BOOL bUseMipmap , BOOL bUseDDS, BOOL bMipmap)
{
	CHAR	szFileName[256], szName[256], szExtension[256], szExportFileName[512], szSourceFullPath[512], szFindPath[512];
	sprintf(szFindPath, "%s%s\\", m_szSrcPath, pszPath);

	CHAR	szOldPath[1024*2] = {'\0',};
	strcpy( szOldPath, RwImageGetPath () );
	RwImageSetPath(szFindPath);

	strcat( szFindPath, "*.*" );

	AuFileFind	csFinder;
	if (!csFinder.FindFile(szFindPath))
		return FALSE;

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		_splitpath(szFileName, NULL, NULL, szName, szExtension);

		AcuTextureType eTextureType	= _GetTextureType(szExtension);
		if (ACUTEXTURE_TYPE_NONE != eTextureType)
		{
			INT32 nFormat = -1;
			if( !bMipmap )
			{
				if( AcuTexture::DetermineDetailAlphaData( szFileName ) )		// 밉맵 안쓰고 알파텍스쳐 많이 쓸경우 DXT5로 변경..
					nFormat = D3DFMT_DXT5;
			}

			sprintf(szExportFileName, "%s%s\\%s.dds", m_szDestPath, pszPath, szName);
			if ( !AcuTexture::RwD3D9DDSTextureWrite( szFileName, szExportFileName, nFormat, 1, ACUTEXTURE_COPY_LOAD_LINEAR, NULL, !bUseDDS, bMipmap ) )
			{
				sprintf(szSourceFullPath, "%s%s\\%s", m_szSrcPath, pszPath, szFileName);
				sprintf(szExportFileName, "%s%s\\%s%s", m_szDestPath, pszPath, szName, szExtension);

				if (CopyFile(szSourceFullPath, szExportFileName, FALSE))
					m_funcLogFormat( "Failed - AcuTexture::RwTextureWriteWithTexDict() %s", szFileName );

			}
		}
	}

	RwImageSetPath(szOldPath);

	return TRUE;
}

BOOL AgcmExportResource::ExportTextureDDSFolder( CHAR *pszPath, CHAR *pszFindPath, INT32 lFormat, INT32 lReduceRatio, AcuTextureCopyType eCopyType, BOOL bMipmap )
{
	CHAR	szFileName[256], szName[256], szExtension[256], szExportFileName[512], szFindPath[512];
	sprintf(szFindPath, "%s%s\\*.*", m_szSrcPath, pszFindPath);

	AuFileFind	csFinder;
	if (!csFinder.FindFile(szFindPath))
		return FALSE;

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		_splitpath(szFileName, NULL, NULL, szName, szExtension);

		AcuTextureType eTextureType	= _GetTextureType(szExtension);
		if (ACUTEXTURE_TYPE_NONE != eTextureType)
		{
			if (_IsSkipFile(szFileName))
				continue;

			sprintf(szExportFileName, "%s%s\\%s.%s", m_szDestPath, pszPath, szName, AC_EXPORT_EXT_TEXTURE_DDS);
			if( !AcuTexture::RwD3D9DDSTextureWrite( szFileName, szExportFileName, lFormat, lReduceRatio, eCopyType, NULL, TRUE, bMipmap ) )
			{
				m_funcLogFormat( "Failed - AcuTexture::RwD3D9DDSTextureWrite() %s", szFileName );
			}
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::ExportFolder( CHAR* szRelativePath )
{
	CHAR szFindPath[512];
	sprintf( szFindPath, "%s%s\\*.*", m_szSrcPath, szRelativePath );

	AuFileFind	csFinder;
	if( !csFinder.FindFile( szFindPath ) )	return FALSE;

	for( ; csFinder.FindNextFile(); )
	{
		CHAR szFileName[256];
		memset( szFileName, 0, sizeof (CHAR) * 256 );
		csFinder.GetFileName( szFileName );

		if( !ExportFile( szRelativePath, szRelativePath, szFileName, szFileName ) )
			m_funcLogFormat( "Failed - ExportFolder [%s]", szRelativePath );
	}

	return TRUE;
}

BOOL AgcmExportResource::CheckLoadedClump(AgcdCharacterTemplate *pcsAgcdCharacterTemplate )
{
	if( !pcsAgcdCharacterTemplate->m_pcsPreData )
	{
		m_funcLog( "pcsAgcdCharacterTemplate->m_pcsPreData == NULL" );
		return FALSE;
	}

	if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName && !pcsAgcdCharacterTemplate->m_pClump )
	{
		m_funcLogFormat( "m_pszDFFName = %s but m_pClump == NULL", pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDFFName );
		return FALSE;
	}

	if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName && !pcsAgcdCharacterTemplate->m_pDefaultArmourClump )
	{
		m_funcLogFormat( "m_pszDADFFName = %s but m_pDefaultArmourClump == NULL", pcsAgcdCharacterTemplate->m_pcsPreData->m_pszDADFFName );
		return FALSE;
	}

	if( pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName && !pcsAgcdCharacterTemplate->m_pPickingAtomic )
	{
		m_funcLogFormat( "m_pszPickDFFName = %s but m_pPickingAtomic == NULL", pcsAgcdCharacterTemplate->m_pcsPreData->m_pszPickDFFName );
		return FALSE;
	}

	if( pcsAgcdCharacterTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT )
	{
		AcuObject::SetClumpPreLitLim( pcsAgcdCharacterTemplate->m_pClump, &pcsAgcdCharacterTemplate->m_pcsPreData->m_stPreLight );
		AcuObject::SetClumpPreLitLim( pcsAgcdCharacterTemplate->m_pDefaultArmourClump, &pcsAgcdCharacterTemplate->m_pcsPreData->m_stPreLight );
	}

	return TRUE;
}

BOOL AgcmExportResource::CheckLoadedClump(AgcdItemTemplate *pcsAgcdItemTemplate )
{
	if( !pcsAgcdItemTemplate->m_pcsPreData )
	{
		m_funcLog( "pcsAgcdItemTemplate->m_pcsPreData == NULL" );
		return FALSE;
	}

	if( pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName && !pcsAgcdItemTemplate->m_pstBaseClump )
	{
		m_funcLogFormat( "m_pszBaseDFFName = %s but m_pstBaseClump == NULL", pcsAgcdItemTemplate->m_pcsPreData->m_pszBaseDFFName );
		return FALSE;
	}

	if( pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName && !pcsAgcdItemTemplate->m_pstSecondClump )
	{
		m_funcLogFormat( "m_pszSecondDFFName = %s but m_pstSecondClump == NULL", pcsAgcdItemTemplate->m_pcsPreData->m_pszSecondDFFName );
		return FALSE;
	}

	if( pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName && !pcsAgcdItemTemplate->m_pstFieldClump )
	{
		m_funcLogFormat( "m_pszFieldDFFName = %s but m_pstFieldClump == NULL", pcsAgcdItemTemplate->m_pcsPreData->m_pszFieldDFFName );
		return FALSE;
	}

	if( pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName && !pcsAgcdItemTemplate->m_pstPickingAtomic )
	{
		m_funcLogFormat( "m_pszPickDFFName = %s but m_pstPickingAtomic == NULL", pcsAgcdItemTemplate->m_pcsPreData->m_pszPickDFFName );
		return FALSE;
	}

	if( pcsAgcdItemTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT )
	{
		AcuObject::SetClumpPreLitLim(pcsAgcdItemTemplate->m_pstBaseClump, &pcsAgcdItemTemplate->m_pcsPreData->m_stPreLight);
		AcuObject::SetClumpPreLitLim(pcsAgcdItemTemplate->m_pstSecondClump, &pcsAgcdItemTemplate->m_pcsPreData->m_stPreLight);
		AcuObject::SetClumpPreLitLim(pcsAgcdItemTemplate->m_pstFieldClump, &pcsAgcdItemTemplate->m_pcsPreData->m_stPreLight);
	}

	return TRUE;
}

BOOL AgcmExportResource::CheckLoadedClump(AgcdObjectTemplate *pcsAgcdObjectTemplate, AgcdObjectTemplateGroupList *pstList )
{
	if( pstList->m_csData.m_pszDFFName && !pstList->m_csData.m_pstClump )
	{
		m_funcLogFormat( "m_pszDFFName = %s but m_pstClump == NULL", pstList->m_csData.m_pszDFFName );
		return FALSE;
	}

	AcuObject::SetClumpMaterialAlphaGeometryFlagOff(pstList->m_csData.m_pstClump);

	if (pcsAgcdObjectTemplate->m_lObjectType & ACUOBJECT_TYPE_USE_PRE_LIGHT)
		AcuObject::SetClumpPreLitLim(pstList->m_csData.m_pstClump, &pcsAgcdObjectTemplate->m_stPreLight);

	pstList->m_csData.m_csClumpRenderType.m_lSetCount = 0;

	if (!RwUtilClumpHasSkin(pstList->m_csData.m_pstClump))
	{
		if (!AcuObject::InstanceClump(pstList->m_csData.m_pstClump))
		{
			m_funcLog( "Failed - AcuObject::InstanceClump" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForCharacterTemplate()
{
	m_funcLog( "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset( szDir, 0, sizeof(CHAR) * 1024 );
	GetCurrentDirectory( 1024, szDir );

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);					_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_DFF_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_ANIMATION_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_MQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_LQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, "CHARACTER\\DefaultHead");			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_CUSTOMIZE_HAIR_RELATIVE_PATH);_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_CUSTOMIZE_FACE_RELATIVE_PATH);_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForItemTemplate()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);					_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, "INI\\ItemTemplate" );				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_DFF_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_MQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MODEL_TEXTURE_LQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, ITEM_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	
	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForObjectTemplate()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);					_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, OBJECT_DFF_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, OBJECT_ANIMATION_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, OBJECT_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, OBJECT_TEXTURE_MQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, OBJECT_TEXTURE_LQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForSkillTemplate()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, SKILL_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForEffectFile()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);					_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_INI_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_INI_NEW_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_ANIMATION_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_OBJECT_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_TEXTURE_MQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_TEXTURE_LQ_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForUIData()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, UI_TEXTURE_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, UI_BASE_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, MINIMAP_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLDMAP_TEXTURE_RELATIVE_PATH);_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForWorldData()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_DATA_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_TEXTURE_MQ_RELATIVE_PATH);_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_TEXTURE_LQ_RELATIVE_PATH);_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_GRASS_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_WATER_RELATIVE_PATH);		_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, WORLD_OCTREE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForEtcTexture()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset(szDir, 0, sizeof (CHAR) * 1024);
	GetCurrentDirectory(1024, szDir);

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, EFFECT_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, TEXTURE_RELATIVE_PATH);				_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, ETC_TEXTURE_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, NOT_PACKED_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, AMB_OCCL_TEXTURE_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

BOOL AgcmExportResource::CreateDirectoryForINIResource()
{
	m_funcLog(  "익스포트에 필요한 디렉토리를 만들고 있습니다." );

	CHAR	szDir[1024];
	memset( szDir, 0, sizeof(CHAR) * 1024 );
	GetCurrentDirectory( 1024, szDir );

	CHAR	szTemp[1024];
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_RELATIVE_PATH);			_CreateDirectory(szDir, szTemp);
	sprintf(szTemp, "%s%s\\", m_szDestPath, INI_ADULT_RELATIVE_PATH);	_CreateDirectory(szDir, szTemp);

	return TRUE;
}

//----------------------------------------------------------------------------
AcuTextureType AgcmExportResource::_GetTextureType(CHAR *pszFile)
{
	CHAR	szTemp[256];
	_splitpath(pszFile, NULL, NULL, NULL, szTemp);

	CHAR	szExtension[256];
	strcpy( szExtension, szTemp + 1 );

	if( _StringCmp( szExtension, D_IMAGE_EXTENSION_PNG ) )		return ACUTEXTURE_TYPE_PNG;
	if( _StringCmp( szExtension, D_IMAGE_EXTENSION_TIF ) )		return ACUTEXTURE_TYPE_TIF;
	if( _StringCmp( szExtension, D_IMAGE_EXTENSION_BMP ) )		return ACUTEXTURE_TYPE_BMP;

	return ACUTEXTURE_TYPE_NONE;
}

VOID AgcmExportResource::_ToUpper(CHAR *szDest, CHAR *szSrc)
{
	INT32	lMax	= strlen(szSrc);
	for (INT32 lCount = 0; lCount < lMax; szDest[lCount] = toupper(szSrc[lCount]), ++lCount);
}

VOID AgcmExportResource::_StringCpy(CHAR **ppszDest, CHAR *pszSrc, BOOL bSkipExt)
{
	if (*(ppszDest))
		delete [] *(ppszDest);

	CHAR	szTemp[256];
	if (bSkipExt)
	{
		_splitpath(pszSrc, NULL, NULL, szTemp, NULL);
	}
	else
	{
		strcpy(szTemp, pszSrc);
	}

	*(ppszDest)	= new CHAR [strlen(szTemp) + 1];
	strcpy(*(ppszDest), szTemp);
}


BOOL AgcmExportResource::_StringCmp(CHAR *pszSrcString, CHAR *pszCmpString)
{
	CHAR	szTemp1[256], szTemp2[256];

	memset(szTemp1, 0,sizeof (CHAR) * 256);
	memset(szTemp2, 0,sizeof (CHAR) * 256);

	_ToUpper(szTemp1, pszSrcString);
	_ToUpper(szTemp2, pszCmpString);

	return !strcmp(szTemp1, szTemp2) ? TRUE : FALSE;
}

BOOL AgcmExportResource::_CopyFile(CHAR *pszPath, CHAR *pszName, BOOL bEncryption )
{
	if (!ExportFile(pszPath, pszPath, pszName, pszName))
	{
		m_funcLog( "Failed - ExportFile()" );
		return FALSE;
	}

	if (bEncryption)
	{
		CHAR	szDestFilePathName[512];
		sprintf(szDestFilePathName, "%s%s\\%s", m_szDestPath, pszPath, pszName);
		if (!m_csExcelTxtLib.EncryptSave(szDestFilePathName))
		{
			m_funcLog( "Failed - m_csExcelTxtLib.EncryptSave()" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::_CreateDirectory(CHAR *pszCurrentDirectory, CHAR *pszNewDirectory)
{
	if( SetCurrentDirectory(pszNewDirectory) )
		return SetCurrentDirectory(pszCurrentDirectory);

	return CreateDirectory( pszNewDirectory, NULL );
}

BOOL AgcmExportResource::_CopyDirectory(CHAR *pszPath )
{
	CHAR	szFileName[256], szFindPath[512];
	sprintf( szFindPath, "%s%s\\*.*", m_szSrcPath, pszPath );

	AuFileFind	csFinder;
	if (!csFinder.FindFile(szFindPath))
		return FALSE;

	for (; csFinder.FindNextFile(); )
	{
		memset(szFileName, 0, sizeof (CHAR) * 256);
		csFinder.GetFileName(szFileName);

		if (!ExportFile(pszPath, pszPath, szFileName, szFileName))
		{
			m_funcLog( "Failed - ExportFile()" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcmExportResource::_IsSkipFile(CHAR *pszFile)
{
	if( _StringCmp( pszFile, D_SKIP_FILE_A0001C0T_TIF ) )	return TRUE;
	if( _StringCmp( pszFile, D_SKIP_FILE_A010380T_PNG ) )	return TRUE;
	if( _StringCmp (pszFile, D_SKIP_FILE_A010280T_PNG ) )	return TRUE;
	if( _StringCmp (pszFile, D_SKIP_FILE_A010180T_PNG ) )	return TRUE;

	return FALSE;
}
