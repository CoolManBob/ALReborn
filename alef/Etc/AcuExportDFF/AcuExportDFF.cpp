#include "AcuExportDFF.h"
#include "AcuTexture.h"
#include "AcuRpUVAnimData.h"

//--------------------------- CFileName --------------------------- 
void	CFileNameRule::InitNameRule( char* szHeader, DWORD dwID, char* szExtension, WORD wHeaderLen, WORD wIDLen, WORD wExtenceLen )
{
	strncpy( m_szHeader, szHeader, wHeaderLen );
	m_szHeader[ wHeaderLen ] = NULL;

	m_dwID	= dwID;

	strncpy( m_szExtension, szExtension, wExtenceLen );
	m_szExtension[ wExtenceLen ] = NULL;

	m_wHeaderLen	= wHeaderLen;
	m_wIDLen		= wIDLen;
	m_wExtenceLen	= wExtenceLen;

	sprintf( m_szName, "%s%.*x.%s", m_szHeader, m_wIDLen, m_dwID, m_szExtension );
}

char*	CFileNameRule::GetName( char* szName )
{
	if( szName )
		strcpy( szName, m_szName );
		
	return m_szName;
}

void	CFileNameRule::GetFileName( char* szName, int nCount )
{
	sprintf( szName, "%s%.*x%c%c",	m_szHeader, 5, m_dwID, 'A'+(CHAR)(nCount/23), 'A'+(nCount%23) );
}

//--------------------------- AcuExportDFF --------------------------- 
AcuExportDFF::AcuExportDFF() : 
 m_funcLog(NULL)
{
	memset( m_szTexturePathHQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	memset( m_szTexturePathMQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	memset( m_szTexturePathLQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );

	m_strLogFile.clear();
	m_vecStringPair.clear();
}

AcuExportDFF::~AcuExportDFF()
{
	m_strLogFile.clear();
	m_vecStringPair.clear();
}

void	AcuExportDFF::Init( CHAR* szLog, LogFunc funcLog )
{
	m_strLogFile	= szLog;
	m_funcLog		= funcLog;
	m_vecStringPair.clear();
}

void	AcuExportDFF::SetPath( CHAR *szSrcPath, CHAR *szDestPath, CHAR *szDffPath, CHAR *szTexturePathHQ, CHAR *szTexturePathMQ, CHAR *szTexturePathLQ )
{
	memset( m_szSrcPath, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy( m_szSrcPath, szSrcPath, ACUEXPORTDFF_BUFFER_LENGTH - 1 );

	memset( m_szDestPath, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy( m_szDestPath, szDestPath, ACUEXPORTDFF_BUFFER_LENGTH - 1 );

	memset( m_szDffPath, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy (m_szDffPath, szDffPath, ACUEXPORTDFF_BUFFER_LENGTH - 1 );

	memset( m_szTexturePathHQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy (m_szTexturePathHQ, szTexturePathHQ, ACUEXPORTDFF_BUFFER_LENGTH - 1 );

	memset( m_szTexturePathMQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy( m_szTexturePathMQ, szTexturePathMQ, ACUEXPORTDFF_BUFFER_LENGTH - 1 );

	memset( m_szTexturePathLQ, 0, ACUEXPORTDFF_BUFFER_LENGTH );
	strncpy( m_szTexturePathLQ, szTexturePathLQ, ACUEXPORTDFF_BUFFER_LENGTH - 1 );
}

BOOL	AcuExportDFF::ExportDFF( RpClump* pstClump, CHAR* szDestDffName )
{
	m_nMaterialCount = 0;

	if( !pstClump )			return FALSE;
	if( pstClump != RpClumpForAllAtomics( pstClump, ForAllAtomics, this ) )		return FALSE;
	
	char szFile[ACUEXPORTDFF_BUFFER_LENGTH];
	sprintf( szFile, "%s%s\\%s", m_szDestPath, m_szDffPath, GetName() );

	RwStream* pstStream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, szFile );
	if( !pstStream )		return FALSE;

	RtDict*	pstDict = RpUVAnimDataClumpGetDict( pstClump );
	if( pstDict && pstDict != RtDictStreamWrite( pstDict, pstStream ) )		goto FAIL;
	if( pstClump != RpClumpStreamWrite( pstClump, pstStream ) )				goto FAIL;

	RwStreamClose( pstStream, NULL );

	GetName( szDestDffName );

	return TRUE;

FAIL:
	RwStreamClose( pstStream, NULL );
	return FALSE;
}

void			AcuExportDFF::ProcessTexture(RwTexture *pstTexture)
{
	if( !pstTexture )		return;

	RwTextureSetFilterMode( pstTexture, rwFILTERLINEARMIPLINEAR );

	CHAR szTextureName[128];
	strcpy( szTextureName, RwTextureGetName(pstTexture) );

	CHAR* szDstTableName = FindDstName( szTextureName );
	if( szDstTableName )		return;

	szDstTableName = FindSrcName( szTextureName );
	if( szDstTableName )
	{
		RwTextureSetName( pstTexture, szDstTableName );
		return;
	}

	CHAR	szDstFileName[ACUEXPORTDFF_BUFFER_LENGTH];
	do
	{
		GetFileName( szDstFileName, m_nMaterialCount );
		++m_nMaterialCount;
	} while( FindDstName( szDstFileName ) );

	TextureWrite( pstTexture, m_szTexturePathHQ, szTextureName, szDstFileName, 1 );
	TextureWrite( pstTexture, m_szTexturePathMQ, szTextureName, szDstFileName, 2 );
	TextureWrite( pstTexture, m_szTexturePathLQ, szTextureName, szDstFileName, 3 );

	RegisterName( szTextureName, szDstFileName );

	if( !m_strLogFile.empty() )
	{
		static char* strDummy = "*no texture*";

		CHAR* pTex = RwTextureGetName( pstTexture );
		CHAR* pMask = RwTextureGetMaskName( pstTexture );

		if( !pTex || !pTex[0] )		pTex = strDummy;
		if( !pMask || !pMask[0] )	pMask = strDummy;
	}

	RwTextureSetName( pstTexture, szDstFileName );
}

void	AcuExportDFF::TextureWrite( RwTexture* pstTexture, CHAR* szTexturePath, CHAR* szSrcName, CHAR* szDstName, INT32 lFormat )
{
	if( !pstTexture )		return;
	if( !szTexturePath )	return;
	if( !szSrcName )		return;
	if( !szDstName )		return;

	CHAR	szFile[ACUEXPORTDFF_BUFFER_LENGTH];
	sprintf( szFile, "%s%s\\%s.dds", m_szDestPath, szTexturePath, szDstName );
	if( !AcuTexture::RwD3D9DDSTextureWrite( pstTexture, szFile, -1, lFormat, ACUTEXTURE_COPY_LOAD_LINEAR, (AcCallbackData1)m_funcLog ) )
	{
		if( m_funcLog )
		{
			CHAR szMessageBox[256];
			sprintf( szMessageBox, "%s[%s]\n의 size가 이상하거나\nRwD3D9DDSTextureWrite()실패!", szSrcName, szDstName );
			m_funcLog( szMessageBox );
		}
	}
}

CHAR*	AcuExportDFF::FindSrcName( CHAR* szSrc )
{
	if( !szSrc )	return NULL;

	for( StringPairVecItr Itr = m_vecStringPair.begin(); Itr != m_vecStringPair.end(); ++Itr )
		if( (*Itr).first == szSrc )
			return (CHAR*)(*Itr).second.c_str();

	return NULL;
}

CHAR*	AcuExportDFF::FindDstName( CHAR* szSrc )
{
	if( !szSrc )	return NULL;

	for( StringPairVecItr Itr = m_vecStringPair.begin(); Itr != m_vecStringPair.end(); ++Itr )
		if( (*Itr).second == szSrc )
			return (CHAR*)(*Itr).first.c_str();

	return NULL;
}

//------------------------------ static internal use ------------------------------
struct MatFXEnvMapData
{
	RwFrame   *frame;
	RwTexture *texture;
	RwReal    coef;
	RwBool    useFrameBufferAlpha;
};

struct MatFXDualData
{
	RwTexture          *texture;
	RwBlendFunction     srcBlendMode;
	RwBlendFunction     dstBlendMode;
};

struct MatFXBumpMapData
{
	RwFrame   *frame;
	RwTexture *texture;
	RwTexture *bumpTexture;
	RwReal    coef;
	RwReal    invBumpWidth;
};

struct MatFXUVAnimData
{
	RwMatrix *baseTransform;
	RwMatrix *dualTransform;
};

#define MATFXENVMAPGETCONSTDATA(material)		((const MatFXEnvMapData *)MatFXGetConstData(material, rpMATFXEFFECTENVMAP))

union MatFXEffectUnion
{
	MatFXBumpMapData  bumpMap;
	MatFXEnvMapData   envMap;
	MatFXDualData     dual;
	MatFXUVAnimData   uvAnim;
};

struct MatFXEffectData
{
	MatFXEffectUnion     data;
	RpMatFXMaterialFlags flag;
};

enum MatFXPass
{
	rpSECONDPASS = 0,
	rpTHIRDPASS  = 1,
	rpMAXPASS    = 2
};

struct rpMatFXMaterialData
{
	MatFXEffectData      data[rpMAXPASS];
	RpMatFXMaterialFlags flags;
};

#define MATFXMATERIALGETCONSTDATA(material)     ((const rpMatFXMaterialData* const*)((const RwUInt8 *)material) + MatFXMaterialDataOffset)

extern "C"
{
	extern RwInt32 MatFXMaterialDataOffset;
}

static const MatFXEffectUnion* MatFXGetConstData( const RpMaterial * material, RpMatFXMaterialFlags flags )
{
	const rpMatFXMaterialData* materialData = (const rpMatFXMaterialData *) *MATFXMATERIALGETCONSTDATA(material);
	if( !materialData )		return NULL;
	if( IsBadReadPtr( materialData, sizeof(rpMatFXMaterialData) ) )		return NULL;

	for( RwUInt8 pass = 0; pass < rpMAXPASS; ++pass )
		if( materialData->data[pass].flag == flags )
			return &(materialData->data[pass].data);

	return NULL;
}

RpAtomic *	AcuExportDFF::ForAllAtomics(RpAtomic *pstAtomic, PVOID pvData)
{
	ForAllGeometries( RpAtomicGetGeometry(pstAtomic), pvData );
	RpLODAtomicForAllLODGeometries( pstAtomic, ForAllGeometries, pvData );
	return pstAtomic;
}

RpGeometry *	AcuExportDFF::ForAllGeometries(RpGeometry *pstGeometry, PVOID pvData)
{
	return RpGeometryForAllMaterials( pstGeometry, ForAllMaterials, pvData );
}

RpMaterial*	AcuExportDFF::ForAllMaterials(RpMaterial *pstMaterial, PVOID pvData)
{
	AcuExportDFF *	pThis = (AcuExportDFF *) pvData;
	pThis->ProcessTexture( RpMaterialGetTexture( pstMaterial ) );

	if( (RpMatFXMaterialGetEffects(pstMaterial) & rpMATFXEFFECTENVMAP) )
	{
		const MatFXEnvMapData* envMapData = MATFXENVMAPGETCONSTDATA( pstMaterial );
		if( !envMapData )
		{
			if( pThis->m_funcLog )
			{
				CHAR buf[256];
				sprintf( buf, "envMapData was NULL in a rpMATFXEFFECTENVMAP-set material! OR Fx Not Data" );
				pThis->m_funcLog( buf );
			}
		}
		else
		{
			const MatFXEnvMapData* pEnvMapData = MATFXENVMAPGETCONSTDATA( pstMaterial );
			if( pEnvMapData )
			{
				pThis->ProcessTexture( pEnvMapData->texture );
			}
		}
	}

	DxEffect* pstDxEffect= RpMaterialD3DFxGetEffect(pstMaterial);
	if( pstDxEffect )
		RpMaterialD3DFxForAllTweakableParameters( pstMaterial, ForAllParameters, pThis );

	return pstMaterial;
}

PVOID			AcuExportDFF::ForAllParameters(DxEffect* effect, RwUInt32 type, RwChar *name, void *data, void *callbackData)
{
	if( type == EFFECT_TWEAKABLE_PARAM_TEXTURE )
		((AcuExportDFF *) callbackData)->ProcessTexture(*(RwTexture **)data);

	return effect;
}