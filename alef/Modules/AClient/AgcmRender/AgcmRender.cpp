#include "AgcmRender.h"
#include "d3d9.h"
#include <skeleton.h>

#include "fadeD3D9.h"
#include "rpworld.h"
#include "rwcore.h"
#include "rpskin.h"

#include "AcuRpMTexture.h"
#include "AcuRpMatFxEx.h"

#include "AgcmPostFX.h"
#include "AcuRpMatD3DFx.h"
#include "AgcmMap.h"
#include "AcuRtAmbOcclMap.h"

#ifdef USE_MFC
#include "AcuObjecWire.h"
#endif //USE_MFC

#include "ApMemoryTracker.h"

#include "AgcmSkill.h"	//for debugging skill
#include "AgcmObject.h"	//for show object collision data

#include "AgcmUIConsole.h"

#include "AgcdEffGlobal.h"

#include "AgcmResourceLoader.h"
#include "AgcmCharacter.h"

#include <vector>
#include "cslog.h"

//-------------------------- Lua -------------------------- 
LuaGlue	LG_SetOcLineCheck( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nUse		= ( INT32 ) pLua->GetNumberArgument( 1 , TRUE );

	if( AgcmRender::m_pThisAgcmRender )
	{
		AgcmRender::m_pThisAgcmRender->SetOcLineCheck( nUse );
	}
	return 0;
}

LuaGlue	LG_GetOcLineCheck( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nUse = TRUE;

	if( AgcmRender::m_pThisAgcmRender ) nUse = AgcmRender::m_pThisAgcmRender->m_bOcLineCheck;

	pLua->PushNumber( ( double ) nUse );
	return 1;
}

LuaGlue	LG_SetCharacterLightEffect( lua_State *L )
{
	// nCID , nPart , Type, Light1_R , Light1_G , Light1_B , Light2_R ,Light2_G , Light2_B , uGap );
	AuLua * pLua = AuLua::GetPointer();

	INT32	nCID		= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nPart		= ( INT32 ) pLua->GetNumberArgument( 2 , 0 );
	INT32	nType		= ( INT32 ) pLua->GetNumberArgument( 3 , 0 );

	UINT32	uGap		= ( UINT32 ) pLua->GetNumberArgument( 10 , 1000 );

	AgpdCharacter * pCharacter;
	static AgpmCharacter * pcsAgpmCharacter = ( AgpmCharacter * ) g_pEngine->GetModule( "AgpmCharacter" );
	pCharacter = pcsAgpmCharacter->GetCharacter( nCID );

	if( NULL == pCharacter )
	{
		return 0;
	}

	AgcdCharacter * pcsAgcdCharacter		= AgcmCharacter::m_pThisAgcmCharacter->GetCharacterData( pCharacter );
	AuCharacterLightInfo * pLightInfo = pcsAgcdCharacter->GetLightInfo( nPart );

	pLightInfo->eType		= ( AuCharacterLightInfo::TYPE )nType	;
	pLightInfo->r1 = ( FLOAT ) pLua->GetNumberArgument( 4 , 0.0f );
	pLightInfo->g1 = ( FLOAT ) pLua->GetNumberArgument( 5 , 0.0f );
	pLightInfo->b1 = ( FLOAT ) pLua->GetNumberArgument( 6 , 0.0f );
	pLightInfo->r2 = ( FLOAT ) pLua->GetNumberArgument( 7 , 1.0f );
	pLightInfo->g2 = ( FLOAT ) pLua->GetNumberArgument( 8 , 1.0f );
	pLightInfo->b2 = ( FLOAT ) pLua->GetNumberArgument( 9 , 1.0f );
	pLightInfo->uChangeGap	= uGap	;

	return 0;
}

static luaDef MVLuaGlue[] = 
{
	{"SetOcLineCheck"		,	LG_SetOcLineCheck		},
	{"GetOcLineCheck"		,	LG_GetOcLineCheck		},
	{"SetCharacterLightEffect" , LG_SetCharacterLightEffect },
	{NULL					,	NULL					},
};



void stBlendAtomicInfo::Add( stBlendAtomic & BlendAtomic )
{
	atomics_.insert( make_pair( BlendAtomic.m_fSortFactor , BlendAtomic ) );
}

void stBlendAtomicInfo::_SortAtomicsBySortFactor( void )
{
	//stBlendAtomic TempAtomic;

	//INT32 nAtomicCount = m_vecAtomics.GetSize();
	//for( INT32 nCount = 0 ; nCount < nAtomicCount - 1 ; nCount++ )
	//{
	//	stBlendAtomic* pAtomic1 = m_vecAtomics.Get( nCount );
	//	if( pAtomic1 )
	//	{
	//		for( INT32 nCount2 = nCount + 1 ; nCount2 < nAtomicCount ; nCount2++ )
	//		{
	//			stBlendAtomic* pAtomic2 = m_vecAtomics.Get( nCount2 );
	//			if( pAtomic2 )
	//			{
	//				if( pAtomic1->m_fSortFactor < pAtomic2->m_fSortFactor )
	//				{
	//					TempAtomic.Copy( pAtomic1 );
	//					pAtomic1->Copy( pAtomic2 );
	//					pAtomic2->Copy( &TempAtomic );
	//				}
	//			}
	//		}
	//	}
	//}
}

void stBlendAtomicEntry::Clear( void )
{
	for( int i=0; i<m_mapBlendAtomics.GetSize(); ++i ) 
	{
		stBlendAtomicInfo ** info = m_mapBlendAtomics.GetByIndex(i);

		if( info && *info )
		{
			delete (*info);
		}
	}

	m_mapBlendAtomics.Clear();
}

void stBlendAtomicEntry::Add( enumRenderBMode eRenderMode, stBlendAtomic BlendAtomic )
{
	stBlendAtomicInfo** pBlendAtomics = m_mapBlendAtomics.Get( eRenderMode );
	if( pBlendAtomics && *pBlendAtomics )
	{
		(*pBlendAtomics)->Add( BlendAtomic );
	}
	else
	{
		stBlendAtomicInfo * NewBlendType = new stBlendAtomicInfo();

		NewBlendType->m_eBlendMode = eRenderMode;
		NewBlendType->Add( BlendAtomic );

		m_mapBlendAtomics.Add( eRenderMode, NewBlendType );
	}
}



//---------------- CRenderFadeInOut ------------------
struct CBFuncAtomicParam
{
	int		nLevel;		// lod-level
	UINT32	uEndTick;	// end-tick
};

void	CRenderFadeInOut::StartFadeInOut( RpAtomic* atomic, int level1, int level2, UINT32 endtick )
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderFade )		return;
		
	specialRenderingFlag |= eSpecialRenderFade;

	atomic->stRenderInfo.shaderUseType	= VS_FADEINOUT_A;
	atomic->stRenderInfo.countStart		= 0;
	atomic->stRenderInfo.countEnd		= endtick;
	atomic->stRenderInfo.fadeinLevel	= level1;
	atomic->stRenderInfo.fadeoutLevel	= level2;

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, AgcmRender::m_pThisAgcmRender->RenderCallbackForSpecialRendering);
	RpLODAtomicHookRender(atomic);
}

void	CRenderFadeInOut::StartFadeIn( RpAtomic* atomic, int level, UINT32 endtick )
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderFade )		return;

	specialRenderingFlag |= eSpecialRenderFade;

	atomic->stRenderInfo.shaderUseType	= VS_FADEIN_A;
	atomic->stRenderInfo.countStart		= 0;
	atomic->stRenderInfo.countEnd		= endtick;
	atomic->stRenderInfo.fadeinLevel	= level;

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, AgcmRender::m_pThisAgcmRender->RenderCallbackForSpecialRendering);		
	RpLODAtomicHookRender(atomic);
}

void	CRenderFadeInOut::StartFadeOut( RpAtomic* atomic, int level, UINT32 endtick )
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderFade )		return;

	specialRenderingFlag |= eSpecialRenderFade;

	atomic->stRenderInfo.shaderUseType	= VS_FADEOUT_A;
	atomic->stRenderInfo.countStart		= 0;
	atomic->stRenderInfo.countEnd		= endtick;
	atomic->stRenderInfo.fadeoutLevel	= level;

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, AgcmRender::m_pThisAgcmRender->RenderCallbackForSpecialRendering);	
	RpLODAtomicHookRender(atomic);
}

void CRenderFadeInOut::StartFadeOutClump(RpClump*	pClump, int level, UINT32 endtick)
{
	CBFuncAtomicParam stParam;
	stParam.nLevel		= level;
	stParam.uEndTick	= max( 2000, endtick );

	if(pClump) RpClumpForAllAtomics( pClump, CallBackSetFadeOut, (void*) &stParam );
}

void		CRenderFadeInOut::StartFadeInClump(RpClump*	pClump,int level,UINT32	endtick)
{
	CBFuncAtomicParam stParam;
	stParam.nLevel		= level;
	stParam.uEndTick	= max( 2000, endtick );

	if( pClump )	RpClumpForAllAtomics( pClump, CallBackSetFadeIn, (void*) &stParam );
}

void		CRenderFadeInOut::StartTransparentClump(RpClump*	pClump,int alpha)
{
	if( pClump )	RpClumpForAllAtomics( pClump, CallBackSetStartTransparent, &alpha );
}

void		CRenderFadeInOut::EndTransparentClump(RpClump*	pClump)
{
	if( pClump )	RpClumpForAllAtomics( pClump, CallBackSetEndTransparent, NULL );
}

void		CRenderFadeInOut::ChangeTransparentValue(RpClump*	pClump,int	alpha)
{
	if(pClump)	RpClumpForAllAtomics(pClump,CallBackChangeTransparent,&alpha);
}

void CRenderFadeInOut::InvisibleFxBegin(RpClump *pClump, RwUInt8 alpha, RwUInt16 fadeOutTiming)
{
	std::pair<RwUInt8, RwUInt16> data(alpha, fadeOutTiming);
	if( pClump )	RpClumpForAllAtomics( pClump, CallBackInvisibleFxBegin, &data );
}

void CRenderFadeInOut::InvisibleFxEnd(RpClump *pClump)
{
	if(pClump)		RpClumpForAllAtomics( pClump, CallBackInvisibleFxEnd, NULL );
}

RpAtomic*	CRenderFadeInOut::CallBackSetFadeIn( RpAtomic* atomic, void* data )
{
	CBFuncAtomicParam* pParam = (CBFuncAtomicParam*) data;
	if ( pParam )
	{
		int nLevel = pParam->nLevel;
		if ( nLevel == -1 )
			nLevel = RpLODAtomicGetCurrentLOD(atomic);

		AgcmRender::m_pThisAgcmRender->StartFadeIn( atomic, nLevel, pParam->uEndTick );
	}

	return atomic;
}

RpAtomic*	CRenderFadeInOut::CallBackSetFadeOut( RpAtomic* atomic, void* data )
{
	CBFuncAtomicParam* pParam = (CBFuncAtomicParam*) data;
	if ( pParam )
		AgcmRender::m_pThisAgcmRender->StartFadeOut( atomic, pParam->nLevel, pParam->uEndTick );
	
	return atomic;
}

RpAtomic*	CRenderFadeInOut::CallBackSetStartTransparent(RpAtomic*	atomic,void*	data)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderFade )		return atomic;

	specialRenderingFlag |= eSpecialRenderFade;

	int	alpha = *((int*)data);
	atomic->stRenderInfo.shaderUseType	= VS_JUST_TRANSPARENT;
	atomic->stRenderInfo.countStart		= max( 0, min( alpha, 255 ) );
	atomic->stRenderInfo.countEnd		= 255;

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, AgcmRender::m_pThisAgcmRender->RenderCallbackForSpecialRendering);	
	RpLODAtomicHookRender(atomic);

	return atomic;
}

RpAtomic*	CRenderFadeInOut::CallBackSetEndTransparent(RpAtomic*	atomic,void*	data)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	ASSERT(specialRenderingFlag & eSpecialRenderFade);
	if( specialRenderingFlag & eSpecialRenderInvisible )		return atomic;

	specialRenderingFlag &= ~eSpecialRenderFade;

	atomic->stRenderInfo.shaderUseType = VS_NONE;

	if( !specialRenderingFlag )
	{
		RpLODAtomicUnHookRender(atomic);
		RpAtomicCallBackRender pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB2;
		RpAtomicSetRenderCallBack(atomic, pFunc);
		RpLODAtomicHookRender(atomic);
	}

	return atomic;
}

RpAtomic*	CRenderFadeInOut::CallBackChangeTransparent(RpAtomic*	atomic,void*	data)
{
	RwUInt32 specialRenderingFlag = (RwUInt32)(atomic->stRenderInfo.backupCB3);
	ASSERT(specialRenderingFlag & eSpecialRenderFade);
	if( specialRenderingFlag & eSpecialRenderInvisible )		return atomic;
		
	int	alpha = *((int*)data);
	atomic->stRenderInfo.countStart = max( 0, min( alpha, 255 ) );
	
	return atomic;
}

RpAtomic*	CRenderFadeInOut::CallBackInvisibleFxBegin(RpAtomic*	atomic,void*	data)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderInvisible )		return atomic;

	specialRenderingFlag |= eSpecialRenderInvisible;
	specialRenderingFlag |= eSpecialRenderFade;

	std::pair<RwUInt8, RwUInt16> pairData = *((std::pair<RwUInt8, RwUInt16>*)data);
	atomic->stRenderInfo.shaderUseType	= VS_JUST_TRANSPARENT;
	atomic->stRenderInfo.beforeLODLevel	= 0;
	atomic->stRenderInfo.countStart		= 255;
	atomic->stRenderInfo.countEnd		= pairData.second;
	atomic->stRenderInfo.fadeinLevel	= *((RwInt8*)&(pairData.first));

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, AgcmRender::m_pThisAgcmRender->RenderCallbackForSpecialRendering);	
	RpLODAtomicHookRender(atomic);

	return atomic;
}


RpAtomic*	CRenderFadeInOut::CallBackInvisibleFxEnd(RpAtomic*	atomic,void*	data)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)(atomic->stRenderInfo.backupCB3);
	ASSERT(specialRenderingFlag & eSpecialRenderInvisible);
	ASSERT(specialRenderingFlag & eSpecialRenderFade);

	specialRenderingFlag &= ~eSpecialRenderInvisible;
	specialRenderingFlag &= ~eSpecialRenderFade;

	atomic->stRenderInfo.shaderUseType = VS_NONE;

	if( !specialRenderingFlag )
	{
		RpLODAtomicUnHookRender(atomic);
		RpAtomicCallBackRender pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB2;
		RpAtomicSetRenderCallBack(atomic, pFunc);
		RpLODAtomicHookRender(atomic);
	}

	return atomic;
}

//---------------- CUsableRwFrameTable ------------------
RwFrame* CUsableRwFrameTable::EnumRemoveRwFrameCallback(RwFrame* frame, void* pData)
{
	if( !pData || !frame )		return NULL;

	CUsableRwFrameTable* pUsableTable = (CUsableRwFrameTable*)pData;
	if( pUsableTable->RemoveFromRwFrameMapTable(frame) == FALSE )
	{
		ASSERT(FALSE);
		return NULL;
	}

	return frame;
}

//---------------- CD3DFxMng ------------------
CD3DFxMng::CD3DFxMng() : 
 m_bDisableMatD3DFx(FALSE),
 m_pcApmMap(NULL),
 m_pcAgcmMap(NULL)
{
}

BOOL	CD3DFxMng::OnInit( ApmMap* pcApmMap, ApmOcTree* pcApmOcTree, AgcmOcTree* pcAgcmOcTree )
{
	m_pcApmMap		= pcApmMap;
	m_pcAgcmMap		= (AgcmMap*)AgcmRender::m_pThisAgcmRender->GetModule("AgcmMap");
	m_pApmOcTree	= pcApmOcTree;
	m_pAgcmOcTree	= pcAgcmOcTree;

	return TRUE;
}

RpMaterial*		CD3DFxMng::FxCheckMaterialCallBack(RpMaterial*		pMat,void*	data)
{
	if( RpMaterialD3DFxGetEffect(pMat) )
	{
		*((bool*)data) = true;
		return NULL;
	}
	
	return pMat;
}

RpGeometry*		CD3DFxMng::FxCheckGeometryCallBack(RpGeometry*	pGeom,void*		data)
{
	RpGeometryForAllMaterials (pGeom,FxCheckMaterialCallBack,data);
	return pGeom;
}

void	CD3DFxMng::AtomicFxOn(RpAtomic*	pAtomic)
{
	//. vertex shader version이 낮으면 check하지 않는다.
	static DWORD _VertexShaderVersion = (((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff);
	if( _VertexShaderVersion < 0x0101 && !m_bDisableMatD3DFx )
		m_bDisableMatD3DFx = TRUE;

	

	
	if( !m_bDisableMatD3DFx )
	{
		bool		bIsEffect = false;

		RpGeometry* pGeom = NULL;
		
		pGeom	=	RpLODAtomicGetGeometry( pAtomic , 0 );
		if( !pGeom )
			pGeom = RpAtomicGetGeometry(pAtomic);

		if( pGeom )
			FxCheckGeometryCallBack( pGeom, &bIsEffect );

		if( bIsEffect)
			RpAtomicFxEnable(pAtomic);
	}
}

void	CD3DFxMng::AtomicFxOff(RpAtomic*	pAtomic)
{
	RpGeometry*		pGeom = RpAtomicGetGeometry(pAtomic);
	if( !pGeom )		return;

	for(int i=0; i<RpGeometryGetNumMaterials(pGeom); ++i)
	{
		DxEffect *pEffect = RpMaterialD3DFxGetEffect(RpGeometryGetMaterial(pGeom, i));
		if( pEffect && !stricmp( pEffect->pSharedD3dXEffect->name, "uvAnimAddAlpha2.fx" ) )	//진영환 파트장님과 협의하에 '시그너스'에 적용되는 fx는 제거하도록함
			return;
	}

	RpAtomicFxDisable(pAtomic);

	if( RpSkinGeometryGetSkin( pGeom ) )	RpSkinAtomicSetType(pAtomic, rpSKINTYPEGENERIC);
	else									RpAtomicSetPipeline( pAtomic, NULL );
}

void	CD3DFxMng::AtomicFxCheck( RpAtomic*	pAtomic)
{
	if( !pAtomic )			return;

	if( m_bDisableMatD3DFx && RpAtomicFxIsEnabled(pAtomic) )
	{
		AtomicFxOff(pAtomic);
	}
	else if( !m_bDisableMatD3DFx && !RpAtomicFxIsEnabled(pAtomic) )
	{
		AtomicFxOn(pAtomic);
	}
}

RpClump*		CD3DFxMng::FxAllCheckClumpCallBack( RpClump* pClump, void* data )
{
	RpClumpForAllAtomics( pClump, FxAllCheckAtomicCallBack, data );
	return pClump;
}

RpAtomic*		CD3DFxMng::FxAllCheckAtomicCallBack( RpAtomic* pAtomic, void* data )
{
	AgcmRender::m_pThisAgcmRender->AtomicFxCheck(pAtomic);
	return pAtomic;
}

void	CD3DFxMng::AllAtomicFxCheck()
{
	static INT32 nCorruptCheckArray[65536];

	ApWorldSector** ppSectors = m_pcApmMap->GetCurrentLoadedSectors();
	UINT32 iSectorCount = m_pcApmMap->GetCurrentLoadedSectorCount();

	for( int i=0; i<(int)iSectorCount; ++i )
	{
		// object들 변경..
		if( m_pApmOcTree->m_bOcTreeEnable )
		{
			int six = ppSectors[i]->GetArrayIndexX();
			int siz = ppSectors[i]->GetArrayIndexZ();

			m_pAgcmOcTree->OcTreeForAllClumps( six, siz, FxAllCheckClumpCallBack, NULL, nCorruptCheckArray );
		}
		else
		{
			SectorRenderList* pList = AgcmRender::m_pThisAgcmRender->GetSectorData(ppSectors[i]);
			if(!pList)
			{
				ASSERT(!"sector attach data 없음");
				continue;
			}

			for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
				AtomicFxCheck( (*Itr).atomic );

			for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
				AtomicFxCheck( (*Itr).atomic );

			for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
				AtomicFxCheck( (*Itr) );

			for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
				AtomicFxCheck( (*Itr) );
		}

		RpAtomic*		cur_atomic;
		WorldAddClumpGroup*	cur_wc = AgcmRender::m_pThisAgcmRender->m_listWorldClumps;
		while(cur_wc)
		{
			if(cur_wc->pClump)
			{
				ASSERT(cur_wc->Atomiclist == NULL);
				if(cur_wc->pClump->atomicList)
				{
					cur_atomic = cur_wc->pClump->atomicList;
					do
					{
						AtomicFxCheck(cur_atomic);
						cur_atomic = cur_atomic->next;
					}
					while(cur_atomic != cur_wc->pClump->atomicList);
				}
			}
			else
			{
				ASSERT(cur_wc->Atomiclist != NULL);
				cur_atomic = cur_wc->Atomiclist;

				while(cur_atomic)
				{
					AtomicFxCheck(cur_atomic);
					cur_atomic = cur_atomic->next;
				}
			}

			cur_wc = cur_wc->next;
		}

		RpDWSector* pDWSector = m_pcAgcmMap->RpDWSectorGetDetail(ppSectors[i], SECTOR_HIGHDETAIL);
		if( pDWSector &&
			(ppSectors[i]->GetFlag() & ApWorldSector::OP_GEOMETRYEFFECTENABLE) && 
			(RtAmbOcclMapAtomicGetFlags(pDWSector->atomic) & rtAMBOCCLMAPOBJECTAMBOCCLMAP))
		{
			AtomicFxCheck(pDWSector->atomic);
		}
	}
}

//---------------- CEdgeRender ------------------
RpToonInk* CEdgeRender::m_pSilhouetteInk = NULL;

CEdgeRender::CEdgeRender() : 
 m_fSilThickness(8.5f)
{
	m_EdgeColor.red		= 255;
	m_EdgeColor.green	= 0;
	m_EdgeColor.blue	= 0;
	m_EdgeColor.alpha	= 128;
}

CEdgeRender::~CEdgeRender()
{
}

BOOL	CEdgeRender::OnInit()
{
#ifndef USE_MFC
	m_pSilhouetteInk = RpToonInkCreate();
    RpToonInkSetOverallThickness( m_pSilhouetteInk, m_fSilThickness );
    RpToonInkSetColor( m_pSilhouetteInk, m_EdgeColor );
    RpToonInkSetName( m_pSilhouetteInk, "silhouette" );
#endif

	return TRUE;
}

void	CEdgeRender::SetToonInkColor( int red, int green, int blue )
{
	m_EdgeColor.red		= red;
	m_EdgeColor.green	= green;
	m_EdgeColor.blue	= blue;
	m_EdgeColor.alpha	= 128;
	RpToonInkSetColor( m_pSilhouetteInk, m_EdgeColor );
}

RpAtomic* CEdgeRender::CreateToonGeometry( RpAtomic *atomic, void * data __RWUNUSED__ )
{
	RpToonGeo* pToonGeometry = RpToonGeometryGetToonGeo( RpAtomicGetGeometry(atomic) );
	if( pToonGeometry )			return atomic;

	pToonGeometry = RtToonGeometryCreateToonGeo( RpAtomicGetGeometry(atomic), FALSE );
	AgcmRender::m_pThisAgcmRender->Toonify( pToonGeometry );

	RpAtomicSetRenderCallBack( atomic, RpAtomicGetRenderCallBack( atomic ) );

    return atomic;
}

RpAtomic* CEdgeRender::SilhouetteRender( RpAtomic *atomic, void * data __RWUNUSED__ )
{
	if( !atomic )		return NULL;

	RpGeometry* pGeometry = RpAtomicGetGeometry(atomic);
	if( !pGeometry )	return atomic;

	RpAtomicCallBackRender BackupCallback = RpAtomicGetRenderCallBack(atomic);
	RpToonGeo* pToonGeometry = RpToonGeometryGetToonGeo( pGeometry );
	if( !pToonGeometry )
	{
		pToonGeometry = RtToonGeometryCreateToonGeo(pGeometry, FALSE);
		AgcmRender::m_pThisAgcmRender->Toonify( pToonGeometry );
	}

	RpSkinType SkinType = rpSKINTYPEGENERIC;
	if(atomic->pipeline)
	{
		SkinType = RpSkinAtomicGetType(atomic);
		RpSkinAtomicSetType(atomic, rpSKINTYPETOON);
	}

	RpToonAtomicEnable(atomic);
	RpAtomicRender(atomic);		//. Silhouette rendering by ToonAtomicRender.

	if( atomic->pipeline )
	{
		AgcmRender::m_pThisAgcmRender->RpSkinResEntryRelease(atomic);
		RpSkinAtomicSetType(atomic, SkinType);
		RpAtomicSetRenderCallBack(atomic, BackupCallback);
	}

	return atomic;
}

RpAtomic* CEdgeRender::RpSkinResEntryRelease( RpAtomic *atomic )
{
    RpGeometry* geometry = RpAtomicGetGeometry( atomic );
	if( !geometry )		return NULL;

	RwResEntry* resEntry = geometry->numMorphTargets != 1 ? atomic->repEntry : geometry->repEntry;
    if( resEntry )
    {
        RwResourcesFreeResEntry( resEntry );
        resEntry = NULL;
    }

	return atomic;
}

void CEdgeRender::Toonify( RpToonGeo *pToonGeometry )
{
    if( !RpToonGeoGetSilhouetteInk(pToonGeometry) )		//If there was no match, set some by hand 
        RpToonGeoSetSilhouetteInk( pToonGeometry, m_pSilhouetteInk );
}

void CEdgeRender::SetSilThickness( float fSilThickness )
{
	fSilThickness = max( 1.f, min( fSilThickness, 2.5f ) );
	RpToonInkSetOverallThickness( m_pSilhouetteInk, fSilThickness );
}

//-------------------------- Global -------------------------- 
AgcmRender *				AgcmRender::m_pThisAgcmRender = NULL;
RpAtomicCallBackRender		AgcmRender::OriginalDefaultAtomicRenderCallback = NULL;
UINT32						g_uRemoveUpdateInfoFromClump2Error = 0;

extern "C"
{
	D3DMATERIAL9	LastMaterial;
	RpAtomic*		AtomicDefaultRenderCallBack(RpAtomic * atomic);

	extern void _rwD3D9ForceRenderState(RwUInt32 state, RwUInt32 value);
}

const FLOAT	DO_NOT_CULL_MBBOX_VALUE	= 0.0f;

// 마고자 (2004-08-11 오후 1:03:00) : 사정상 여기다가 옮겨 놓습니다.
// 셰이더 콘스탄트 설정이 딴데서하면 꼬이는 증상이 있다.
FLOAT	__fFadeOutFactor[ 4 ] = { 0.00100000f , 100.000f , 0.0f , 0.0f };

static CHAR *g_aszRenderTypeName[R_RENDER_TYPE_NUM] = 
{
	"None",
	"NonAlpha",
	"Alpha",
	"BlendSort",
	"BlendNSort",
	"Ambient",
	"AlphaFunc",
	"Skin",
	"LastZWrite"
};

static CHAR *g_aszRenderBlendModeName[R_BELND_MODE_NUM] =
{
	"BLENDSRCAINVA_ADD",
	"BLENDSRCAINVA_REVSUB",
	"BLENDSRCAINVA_SUB",
	"TEXTURE_STAGE_ADD",
	"TEXTURE_STAGE_REVSUB",
	"TEXTURE_STAGE_SUB",
	"BLENDSRCCINVC_ADD",
	"BLENDSRCCINVC_REVSUB",
	"BLENDSRCCINVC_SUB",
	"BLEND11_ADD",
	"BLEND11_REVSUB",
	"BLEND11_SUB",
	"BLENDSRCA1_ADD",
	"BLENDSRCA1_REVSUB",
	"BLENDSRCA1_SUB"
};

//---------------- AgcmRender ------------------
void		AgcmRender::SetDrawCollisionTerrain(BOOL	bVal)
{ 
#ifdef	USE_MFC
	m_bDrawCollision_Terrain = bVal;
#endif
}

void		AgcmRender::SetDrawCollisionObject(BOOL		bVal)
{
#ifdef	USE_MFC
	m_bDrawCollision_Object = bVal;
#endif
}

BOOL		AgcmRender::IsMaptoolBuild()
{
	#ifdef	USE_MFC
	return TRUE;
	#else
	return FALSE;
	#endif
}

#ifdef	USE_MFC
	// 마고자 (2004-06-01 오전 11:33:31) : 콜백정보 저장..
static BOOL ( *	g_pfPreRenderAtomicCallback ) ( RpAtomic * , void * pData );
static void *	g_pPreRenderAtomicData;
#endif

BOOL		AgcmRender::SetPreRenderAtomicCallback		( BOOL ( *pCallback ) ( RpAtomic * , void * pData ) , void * pData )
{
#ifdef	USE_MFC
	g_pfPreRenderAtomicCallback	= pCallback	;
	g_pPreRenderAtomicData		= pData		;
	return TRUE;
#else
	return FALSE;
#endif
}

RwInt32	drawSphere(const RwSphere& sphere, const RwUInt32 col, RwMatrix* pLTM)
{
	static const RwInt32	NumAngle	= 13;
	static const RwInt32	NumVtx		= NumAngle*(NumAngle+NumAngle)+1;

	static RwIm3DVertex		avtx[NumVtx];
	static RwBool			bFirst(TRUE);

	int	nc, i, j, k;

	for( nc = 0; nc<NumVtx; ++nc )
		avtx[nc].color = col;

	if( bFirst )
	{
		bFirst	= FALSE;

		RwReal fStep	= DEF_2PI/static_cast<RwReal>(NumAngle+NumAngle);
		RwReal fCurr	= DEF_D2R(90.f);

		RwIm3DVertex	temp	= { {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, col, 0.f, 0.f, };

		RwIm3DVertex atempvtx[NumAngle+NumAngle];
		memset( atempvtx, 0, sizeof( atempvtx ) );

		for( i = 0; i<NumAngle+NumAngle; ++i )
		{
			atempvtx[i].objVertex.x	= static_cast<RwReal>(cos(fCurr));
			atempvtx[i].objVertex.y	= static_cast<RwReal>(sin(fCurr));

			fCurr	+= fStep;
		}

		RwMatrix	matR;
		RwV3d		axis	= { 0.f, 1.f, 0.f };

		fStep	= 360.f/static_cast<RwReal>(NumAngle);
		fCurr	= fStep;
		i=0;
		for( j=0; j<NumAngle; ++j, fCurr += fStep ){
			RwMatrixRotate( &matR, &axis, fCurr, rwCOMBINEREPLACE );

			for( k=0; k<NumAngle+NumAngle; ++k, ++i ){
				RwV3dTransformPoint( &temp.objVertex, &atempvtx[k].objVertex, &matR );
				avtx[i]	= temp;
			}
		}
		temp.objVertex = atempvtx[0].objVertex;
		avtx[i]	= temp;
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

	RwV3d		vScale	= { sphere.radius, sphere.radius, sphere.radius };
	RwMatrix	mat;
	RwMatrixScale( &mat, &vScale, rwCOMBINEREPLACE );
	if( pLTM )
	{
		RwV3d	vPos	= { sphere.center.x + RwMatrixGetPos( pLTM )->x,
			sphere.center.y + RwMatrixGetPos( pLTM )->y,
			sphere.center.z + RwMatrixGetPos( pLTM )->z};
		RwMatrixTranslate( &mat, &vPos, rwCOMBINEPOSTCONCAT );
	}
	else
		RwMatrixTranslate( &mat, &sphere.center, rwCOMBINEPOSTCONCAT );

	if( RwIm3DTransform(avtx, NumVtx, &mat, rwIM3D_ALLOPAQUE) )
	{
		RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);
	return 0;
}

//----------------------------------- AgcmRender -----------------------------
AgcmRender::AgcmRender()
{
	SetModuleName("AgcmRender");
	EnableIdle(TRUE);
	
	AgcmRender::m_pThisAgcmRender =	this;

	m_pcApmMap			= NULL;
	m_pApmOcTree		= NULL;
	m_pAgcmOcTree		= NULL;
	m_pcsAgcmPostFX		= NULL;

	m_fRotAngle = 0.0f;
	m_fPanAngle = 0.0f;

	m_iShadow2Range = -1;
	m_bDrawShadow2 = TRUE;

	m_bVertexShaderEnable = FALSE;
	m_bActiveClass = TRUE;

	for( INT32 i=0; i<R_HASH_SIZE; ++i )
	{
		m_listNonAlpha.hash_table[i]			= NULL;
		m_listAmbient.hash_table[i]				= NULL;							
		m_listAlphaFunc.hash_table[i]			= NULL;
		m_listSkinning.hash_table[i]			= NULL;
		m_listSkinningAlphaFunc.hash_table[i]	= NULL;
	}

	m_BlendAtomics.Clear();

	for( INT32 i = 0; i<R_NOT_SORT_BLEND_MODE; ++i )
	{
		for( INT32 j=0;j<3;++j )
			m_listBlendNSort.hash_table[i][j] = NULL;

		m_iNumBlendNSort[i] = 0;
	}

	for( INT32 i=0; i<R_BELND_MODE_NUM; ++i )
		m_listSkinningBlend[i] = NULL;

	m_listSkinningAlpha = NULL;
	
	m_listUVAnim = NULL;
	m_listUVAnimSkin = NULL;

	m_listShadow2Terrain = NULL;
	m_pShadow2Terrain_Own = NULL;
	m_listTerrainNoMTexture = NULL;

	m_listCustomRender = NULL;

	m_listWorldClumps = NULL;

	m_listLocalLight = NULL;

	m_listLastZWrite = NULL;

	m_ulLastTick = 0;
	m_ulCurTick = 0;
	m_ulCurTickDiff = 0;

	m_pViewMatrixAt = NULL;
	m_pCameraPos = NULL;

	m_pWorld = NULL;
	m_pCamera =	NULL;
	m_pAmbientLight = NULL;
	m_pDirectLight = NULL;

	m_bWBufferEnable = FALSE;

	m_pFrame = NULL;

	m_vYAxis.x = 0.0f;
	m_vYAxis.y = 1.0f;
	m_vYAxis.z = 0.0f;

	m_iCameraRasterWidth = 0;
	m_iCameraRasterHeight = 0;

	memset(&m_csRenderType,0,sizeof(AgcmRenderType));

	m_iSectorDataIndex = 0;
	m_bUseCullMode = false;
	m_bUseLODBoundary = true;

	m_bLogin = TRUE;
	m_fLightColorParam = 0.5f;
	m_iLightColorStatus = 1;

	m_iLoadRangeX1 = m_iLoadRangeX2 = m_iLoadRangeZ1 = m_iLoadRangeZ2 = 0;
	
	m_fFogConstant.x = m_fFogConstant.y = m_fFogConstant.z = m_fFogConstant.w = 0.0f;
	m_fFogEnd = 10000.0f;

	m_fDirectionalLightAdjust	= 1.0f;
	m_fAmbientLightAdjust		= 1.0f;

	m_stAdjustedDirectionalLight.red = m_stAdjustedDirectionalLight.green = m_stAdjustedDirectionalLight.blue = m_stAdjustedDirectionalLight.alpha = 1.0f;
	m_stAdjustedAmbientLight.red = m_stAdjustedAmbientLight.green = m_stAdjustedAmbientLight.blue = m_stAdjustedAmbientLight.alpha = 1.0f;

	// OcTree관련 초기화는 OnInit에서..

#ifdef	USE_MFC
	// 마고자 (2004-06-01 오전 11:34:40) : 맵툴 정보~
	m_bDrawCollision_Terrain	= FALSE	;
	m_bDrawCollision_Object		= FALSE	;
	g_pfPreRenderAtomicCallback	= NULL	;
	g_pPreRenderAtomicData		= NULL	;
#endif

	// 마고자 (2004-05-17 오전 11:15:39) : 
	// 지형 라이트 어저스트 값 설정.
	
	SetDirectionalLightAdjust	( 1.0f );
	SetAmbientLightAdjust		( 0.5f );

	m_iDrawRange = 0;

	for(int i=0;i<10;++i)
	{
		m_pTableLight[i] = NULL;
		m_iTableLightNum[i] = 0;
	}

	m_bDrawOcLineDebug = FALSE;

	m_bDisableMatD3DFx = FALSE;

	m_bOcLineCheck  = TRUE;
	m_iRenderCount	= 0;

	OriginalDefaultAtomicRenderCallback = AtomicDefaultRenderCallBack;

	m_colorOriginalAmbient.red = m_colorOriginalAmbient.green = m_colorOriginalAmbient.blue = 0.0f;
	m_colorOriginalAmbient.alpha = 1.0f;
	m_colorOriginalDirect.red = m_colorOriginalDirect.green = m_colorOriginalDirect.blue = 0.0f;
	m_colorOriginalDirect.alpha = 1.0f;
	m_colorBlack.red = m_colorBlack.blue = m_colorBlack.green = 0.0f;
	m_colorBlack.alpha = 1.0f;

	for(int i=0;i<_ocLineMax;++i)
	{
		m_pStartOcLine[i] = NULL;
		m_iMaxHeightOcLine[i] = 0;
	}

	m_listOccluderLine = NULL;
	m_bDrawCollision_Terrain = FALSE;
	m_bDrawCollision_Object = FALSE;

	m_bUseAtomicCustomRender	= FALSE;

	for(RwUInt32 i=0; i<5; ++i)
		m_pInvisibleMaterial[i] = NULL;

	m_lighterColorBySpecialStatus[0].red	= 0.15f;
	m_lighterColorBySpecialStatus[0].green	= 0.15f;
	m_lighterColorBySpecialStatus[0].blue	= 0.15f;
	m_lighterColorBySpecialStatus[0].alpha	= 1.0f;

	m_lighterColorBySpecialStatus[1].red	= 1.0f;
	m_lighterColorBySpecialStatus[1].green	= 1.0f;
	m_lighterColorBySpecialStatus[1].blue	= 1.0f;
	m_lighterColorBySpecialStatus[1].alpha	= 1.0f;

	m_lighterColorBySpecialStatus[2].red	= 1.0f;
	m_lighterColorBySpecialStatus[2].green	= 0.5f;
	m_lighterColorBySpecialStatus[2].blue	= 1.0f;
	m_lighterColorBySpecialStatus[2].alpha	= 1.0f;

#ifndef USE_MFC
	m_pSilhouetteInk	= NULL;
	m_fSilThickness		= 8.5f;
	m_EdgeColor.red		= 255;
	m_EdgeColor.green	= 0;
	m_EdgeColor.blue	= 0;
	m_EdgeColor.alpha	= 128;
#endif
}

AgcmRender::~AgcmRender()
{
}

BOOL	AgcmRender::OnAddModule()
{
	m_pcApmMap		= (ApmMap*)GetModule("ApmMap");
	m_pApmOcTree	= (ApmOcTree*)GetModule("ApmOcTree");
	m_pAgcmOcTree	= (AgcmOcTree*)GetModule("AgcmOcTree");
	m_pcsAgcmPostFX	= (AgcmPostFX*)GetModule("AgcmPostFX");

	if( !m_pcApmMap ||
		!m_pApmOcTree ||
		!m_pAgcmOcTree ||
		!m_pcsAgcmPostFX )
		return FALSE;

	if( m_pApmOcTree->m_bOcTreeEnable )
	{
		m_iSectorDataIndex = m_pcApmMap->AttachSectorData( this, sizeof(SectorData), CBInitSectorData, CBRemoveSectorData);
	}
	else
	{
		int nSize = sizeof(SectorRenderList);
		m_iSectorDataIndex = m_pcApmMap->AttachSectorData( this, sizeof(SectorRenderList), CBInitSector, CBRemoveSector );
		m_bUseCullMode = true;
	}

	m_pAgcmOcTree->SetWorldIntersectionCallBack( CBWorldIntersection );

	return TRUE;
}

BOOL	AgcmRender::OnInit()
{
	RwD3D9SetRenderState( D3DRS_ZFUNC , D3DCMP_LESSEQUAL  );							// zfighting 해결 
	RwRenderStateSet( rwRENDERSTATECULLMODE , (void *)rwCULLMODECULLNONE ); 
	
	const D3DCAPS9* d3dCaps = (const D3DCAPS9 *)RwD3D9GetCaps();

	// 셰이더 지원하지 않으면 라이트 옵셋 없엠.
	SetDirectionalLightAdjust( 1.0f );
    if ( (d3dCaps->VertexShaderVersion & 0xffff) >= 0x0101)
	{
        m_bVertexShaderEnable = true;
		SetAmbientLightAdjust( 0.5f );
	}
    else 
	{
		m_bVertexShaderEnable = false;
		SetAmbientLightAdjust( 1.0f );
	}
	
	if( d3dCaps->RasterCaps & D3DPRASTERCAPS_WBUFFER )
	{
		m_bWBufferEnable = TRUE;
		RwD3D9SetRenderState( D3DRS_ZENABLE, D3DZB_USEW );					// zfighting 해결(매번 해줘야 할지도..)
	}
	else
	{
		m_bWBufferEnable = FALSE;
	}

	m_pCurD3D9Device = (IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice ();

	LightValueUpdate();

	m_pInvisibleMaterial[0] = RpMaterialCreate();
	RpMaterialD3DFxSetEffectNoWeight(m_pInvisibleMaterial[0], "invisible.fx");
	m_pInvisibleMaterial[1] = RpMaterialCreate();
	RpMaterialD3DFxSetEffectWeight1(m_pInvisibleMaterial[1], "invisible.fx");
	m_pInvisibleMaterial[2] = RpMaterialCreate();
	RpMaterialD3DFxSetEffectWeight2(m_pInvisibleMaterial[2], "invisible.fx");
	m_pInvisibleMaterial[3] = RpMaterialCreate();
	RpMaterialD3DFxSetEffectWeight3(m_pInvisibleMaterial[3], "invisible.fx");
	m_pInvisibleMaterial[4] = RpMaterialCreate();
	RpMaterialD3DFxSetEffectWeight4(m_pInvisibleMaterial[4], "invisible.fx");

	
	RpMTextureEnableGlossMap( TRUE );		// Enable the terrain gloss map by default.

	// Max number of local lights : 2 -> 8
#ifdef	USE_MFC
	//@{ Jaewon 20050416
	//RpWorldSetGet2AtomicLocalLightsCallBack(MyWorldGet2AtomicLocalLightsCallBack_MapTool);
	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsCallBack);
	//@} Jaeown
#else
	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsCallBack);
#endif

	AS_REGISTER_TYPE_BEGIN( AgcmRender, AgcmRender );

#ifndef USE_MFC
	//AS_REGISTER_METHOD3(void, CEdgeRender::SetToonInkColor, int, int, int);	//빌드가 안되서 일단 막아둔다
	AS_REGISTER_VARIABLE(int, m_bDrawOcLineDebug );
#endif

	AS_REGISTER_TYPE_END;

	CD3DFxMng::OnInit( m_pcApmMap, m_pApmOcTree, m_pAgcmOcTree );
	CUsableRwFrameTable::OnInit();
	CEdgeRender::OnInit();

	return TRUE;
}

BOOL	AgcmRender::OnDestroy()
{
	for( RwUInt32 i=0; i<5; ++i )
	{
		if( m_pInvisibleMaterial[i] )
			RpMaterialDestroy( m_pInvisibleMaterial[i] );

		m_pInvisibleMaterial[i] = NULL;
	}

	WorldAddClumpGroup *cur_clump, *remove_clump;
	
	cur_clump = m_listWorldClumps;

	while(cur_clump)
	{
		remove_clump = cur_clump;
		cur_clump = cur_clump->next;

		delete remove_clump;
		remove_clump = NULL;
	}
	
	CustomRenderList*	cur_list = m_listCustomRender;
	CustomRenderList*	remove_list;

	while(cur_list)
	{
		remove_list = cur_list;
		cur_list = cur_list->next;

		delete remove_list;	
		remove_list = NULL;
 	}

	LocalLight*			cur_lt = m_listLocalLight;
	LocalLight*			remove_lt;
	while(cur_lt)
	{
		remove_lt = cur_lt;
		cur_lt = cur_lt->next;

		RemoveLightFromRenderWorld(remove_lt->pLight,remove_lt->bMove);
		delete remove_lt;
		remove_lt = NULL;
	}

	return TRUE;
}

BOOL	AgcmRender::OnIdle							(UINT32 ulClockCount)
{
	m_ulCurTickDiff = ulClockCount - m_ulCurTick;
	m_ulLastTick = m_ulCurTick;
	m_ulCurTick = ulClockCount;
	
	if( m_pCamera )
	{
		RwMatrix* pViewMatrix	= RwCameraGetViewMatrix( m_pCamera );
		ASSERT( pViewMatrix );

		m_pViewMatrixAt	= RwMatrixGetAt( pViewMatrix );
	}

	// Billboard 용 ambient color update
	if ( m_pAmbientLight && m_pDirectLight )
	{
		m_colorOriginalAmbient = m_pAmbientLight->color;
		m_colorOriginalDirect = m_pDirectLight->color;
	}

	if( RpMTextureIsEnableLinearFog() )
	{
		// Linear Fog 사용시
		FLOAT	f1,f2;
		RwD3D9GetRenderState( D3DRS_FOGSTART, &f1 );
		RwD3D9GetRenderState( D3DRS_FOGEND, &f2 );
		// a too big constant can be a problem in GeforceFX series.
		// m_fFogConstant.x = f2; -> m_fFogConstant.x = f2 / (f2 - f1);
		m_fFogConstant.x = f2 / (f2 - f1);
		m_fFogConstant.y = 1.0f / (f2 - f1);
	}
	else
	{
		//exp이므로 x에 density * log2(e)(1.44270)을 전달한다
		float		val;
		RwRenderStateGet( rwRENDERSTATEFOGDENSITY,(void*)&val );
		m_fFogConstant.x = min( val * 1.44270f, 1.f );
	}

	m_iRenderCount = 0;

	return TRUE;
}

void	AgcmRender::OnCameraStateChange(CAMERASTATECHANGETYPE	ctype)
{
	if( !m_pCamera )		return;

	if( CSC_RESIZE == ctype || CSC_INIT == ctype )
	{
		RwRaster* pRas = RwCameraGetRaster(m_pCamera);
		ASSERT( pRas );
		if( pRas )
		{
			m_iCameraRasterWidth	= RwRasterGetWidth(pRas);
			m_iCameraRasterHeight	= RwRasterGetHeight(pRas);
		}
	}
}

void	AgcmRender::OnLuaInitialize( AuLua * pLua )
{
	for(int i=0; MVLuaGlue[i].name; i++)
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
}

void		AgcmRender::SetMainFrame(RwFrame* pFrame)
{
	m_pFrame = pFrame;
}

// ######################################################################################## Render Funcs
void AgcmRender::Render()
{
	if( ++m_iRenderCount >= 2 )		return;
	if( !m_bActiveClass )			return;

	PROFILE("AgcmRender::Render");

	ApAutoWriterLock	csLock( lockRender );
	static AgcmResourceLoader * _spcsAgcmResourceLoader = ( AgcmResourceLoader * ) GetModule( "AgcmResourceLoader" );
	AuAutoLock Lock( _spcsAgcmResourceLoader->m_csMutexRemoveResource );

	m_pCameraPos	= RwMatrixGetPos(RwFrameGetLTM(RwCameraGetFrame(m_pCamera)));

	//RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLBACK	);
	
	// Fog Set
	RwD3D9SetRenderState( D3DRS_FOGEND,		*((DWORD*)(&m_fFogEnd)));
	RwD3D9SetRenderState( D3DRS_BLENDOP,	D3DBLENDOP_ADD );

	// Render World 
	RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
	RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA );
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, (void*)FALSE );

	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);

	//RwD3D9SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME  );
	
	{
	PROFILE("AgcmRender::PRERENDER");
	EnumCallback( AGCMRENDER_CB_ID_PRERENDER, NULL, NULL );
	}

	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)true );
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );

	{
	PROFILE("AgcmRender::RpWorldRender");
	if (m_pWorld)
		RpWorldRender( m_pWorld );
	}	
	
	// world에 추가된 것들 검사
	if( m_pApmOcTree->m_bOcTreeEnable )
	{
		{
		PROFILE("AgcmRender::OCTREERENDER()");

		OcTreeModeRender( m_pCamera, DefaultTerrainRender, TRUE );

		if(m_bOcLineCheck)		UpdateOBufferAndDrawTest(DefaultAtomicListTestRender);
		else					UpdateOBufferAndDrawNotTest(DefaultAtomicListTestRender);
		}

		if( m_bDrawOcLineDebug )
		{
			// occluder line 그려보자
			RwV2d	p1,p2;
			OccluderLineList* cur_line = m_listOccluderLine;
			while(cur_line)
			{
				p1.x = ( float ) cur_line->startx;
				p1.y = ( float ) cur_line->y;

				p2.x = ( float ) cur_line->endx;
				p2.y = ( float ) cur_line->y;

				AcuIMDraw::Draw2DLine(&p1,&p2,0xff00ffff);

				cur_line = cur_line->next;
			}
		}
	}
	else
	{
		RenderWorldAtomics(m_pCamera,DefaultAtomicListTestRender);
		RenderCustomObjects();

		if(m_bUseCullMode)
		{
		PROFILE("AgcmRender::CullModeWorldRender()");
		CullModeWorldRender(m_pCamera,DefaultAtomicRender,DefaultTerrainRender);
		}
		else
		{
		}
	}
	
	int		fogTableMode;
	int		fogVertexMode;

	if( RpMTextureIsEnableLinearFog() && m_bVertexShaderEnable )
	{
		RwD3D9GetRenderState(D3DRS_FOGTABLEMODE , &fogTableMode);
		RwD3D9GetRenderState(D3DRS_FOGVERTEXMODE , &fogVertexMode);

		RwD3D9SetRenderState(D3DRS_FOGTABLEMODE , D3DFOG_NONE);
		RwD3D9SetRenderState(D3DRS_FOGVERTEXMODE , D3DFOG_NONE);
	}
			
	{
	
	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsDummyCallBack);		// No light binding

	PROFILE("Render - Shadow2 path");

	for( INT32 i=0;i<6;++i )
		RwD3D9SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	for( INT32 i=1; i<=5; i+=2 )				// 알파 텍스쳐 Filtering Point지정
	{
		RwD3D9SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		RwD3D9SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	}

	// 지형shadow2 path 그린다.
	RpAtomicCallBackRender	pFunc;
		
	RpLightSetColor(m_pDirectLight, &m_stAdjustedDirectionalLight);
	RpLightSetColor(m_pAmbientLight, &m_stAdjustedAmbientLight);
	
	// 마고자 (2005-10-24 오후 3:57:54) :
	// Shadow2 범위에 들어가는 것들에 대한 Shadow2 렌더 처리 하는 부분..
	// bShadow 플래그는 DefaultTerrainRender 에서 거리체크하여 설정함.

	RpMTextureDrawStart();

	if(m_bVertexShaderEnable)	TerrainDataSet();

	Shadow2TerrainList*		cur_s2 = m_listShadow2Terrain;
	Shadow2TerrainList*		bef_s2 = cur_s2;
	RpAtomic*				terrain_atomic;

	while(cur_s2)
	{
		terrain_atomic = cur_s2->atomic;

		#ifdef _DEBUG
		RpGeometry*		pGeom = RpAtomicGetGeometry(terrain_atomic);
		if(pGeom->repEntry )
		{
			RxD3D9ResEntryHeader* rsh = (RxD3D9ResEntryHeader *) (pGeom->repEntry + 1);
			LPDIRECT3DVERTEXDECLARATION9	decl = (LPDIRECT3DVERTEXDECLARATION9)rsh->vertexDeclaration;
			D3DVERTEXELEMENT9	elem[256];
			UINT				num;
			decl->GetDeclaration(elem,&num);
			if( (*((*(cur_s2->atomic)).geometry)).numVertices != 289 ) // 러프맵인지
			if(elem[0].Usage != 0 || elem[1].Usage != 3 || elem[2].Usage != 10 ||
				elem[3].Usage != 5 || elem[4].Usage != 5 ||elem[5].Usage != 5 ||
				elem[6].Usage != 5 ||elem[7].Usage != 5 ||elem[8].Usage != 5 ||
				elem[9].Type != 17)
			{
				TRACE("지형 포맷 이상! 재욱씨나 태형씨 호출 바람 !!!\n" );
			}
		}
		#endif

		pFunc = (RpAtomicCallBackRender)terrain_atomic->stRenderInfo.backupCB;

		terrain_atomic->stRenderInfo.shaderUseType = 0;
		terrain_atomic->stRenderInfo.beforeLODLevel = cur_s2->sectordist;

		pFunc(terrain_atomic);
					
		if(!cur_s2->bShadow)					// 그림자가 없다면 다음 list순회를 줄이기 위해 삭제 ^^
		{
			if(cur_s2 == m_listShadow2Terrain)
			{
				m_listShadow2Terrain = cur_s2->next;
			}
			else 
			{
				bef_s2->next = cur_s2->next;
			}
			
			cur_s2 = cur_s2->next;
		}
		else
		{
			bef_s2 = cur_s2;									
			cur_s2 = cur_s2->next;
		}
	}

	EnumCallback( AGCMRENDER_CB_MTEXTURE_RENDER, NULL, NULL );

	RpMTextureDrawEnd(TRUE);

	//  셰도우 렌더.
	if(m_bDrawShadow2)
	{
		cur_s2 = m_listShadow2Terrain;				// Object Shadow 그리자 
		while(cur_s2)
		{
			terrain_atomic = cur_s2->atomic;
			pFunc = (RpAtomicCallBackRender)terrain_atomic->stRenderInfo.backupCB;

			terrain_atomic->stRenderInfo.shaderUseType = 1;

			if( terrain_atomic->stType && terrain_atomic->stType->eType & ACUOBJECT_TYPE_SECTOR_ROUGHMAP )
			{
				// do nothing
			}
			else
			{
                pFunc(terrain_atomic);
			}
			
			cur_s2 = cur_s2->next;
		}
	}

	if(m_pShadow2Terrain_Own)
	{
		RpMTextureDrawStart();

		terrain_atomic = m_pShadow2Terrain_Own->atomic;

		#ifdef _DEBUG
		RpGeometry*		pGeom = RpAtomicGetGeometry(terrain_atomic);
		if(pGeom->repEntry )
		{
			RxD3D9ResEntryHeader* rsh = (RxD3D9ResEntryHeader *) (pGeom->repEntry + 1);
			LPDIRECT3DVERTEXDECLARATION9	decl = (LPDIRECT3DVERTEXDECLARATION9)rsh->vertexDeclaration;
			D3DVERTEXELEMENT9	elem[256];
			UINT				num;
			decl->GetDeclaration(elem,&num);
			if( (*((*(m_pShadow2Terrain_Own->atomic)).geometry)).numVertices != 289 ) // 러프맵인지
			if(elem[0].Usage != 0 || elem[1].Usage != 3 || elem[2].Usage != 10 ||
				elem[3].Usage != 5 || elem[4].Usage != 5 ||elem[5].Usage != 5 ||
				elem[6].Usage != 5 ||elem[7].Usage != 5 ||elem[8].Usage != 5 ||
				elem[9].Type != 17)
			{
				TRACE("지형 포맷 이상! 재욱씨나 태형씨 호출 바람 !!!\n" );
			}
		}
		#endif

		pFunc = (RpAtomicCallBackRender)terrain_atomic->stRenderInfo.backupCB;

		terrain_atomic->stRenderInfo.shaderUseType = 0;
		terrain_atomic->stRenderInfo.beforeLODLevel = m_pShadow2Terrain_Own->sectordist;
		
		pFunc(terrain_atomic);

		RpMTextureDrawEnd(TRUE);
			
		if(m_bDrawShadow2)
		{
			terrain_atomic->stRenderInfo.shaderUseType = 1;
			pFunc(terrain_atomic);
		}
	}
	
	RpLightSetColor(m_pAmbientLight, &m_colorOriginalAmbient);
	RpLightSetColor(m_pDirectLight, &m_colorOriginalDirect);

	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsCallBack);		// Restore the original callback.
	}

	ComputeLightColors();

	if( RpMTextureIsEnableLinearFog() && m_bVertexShaderEnable )
	{
		RwD3D9SetRenderState( D3DRS_FOGTABLEMODE , fogTableMode );
		RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE , fogVertexMode );
	}

	{
	PROFILE("AgcmRender::TerrainNoRpMTexture");

	// Force to turn off the alpha-blending.
	// Because of 'alpha' channel in gloss tile maps, RenderWare enables the alpha-blending by default.
	_rwD3D9ForceRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pCurD3D9Device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	Shadow2TerrainList*	terrain_node = m_listTerrainNoMTexture;	
	while(terrain_node)
	{
		RenderAtomic(terrain_node->atomic,FALSE);
		terrain_node = terrain_node->next;
	}

	// Resync the alpha-blending setting.
	_rwD3D9ForceRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pCurD3D9Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	}
					
	{
	PROFILE("AgcmRender::NonAlpha");
	
	RenderNonAlphaAtomics();		// Render Non Alpha Atomic 

	AgcmSkill* pAgcmSkill = static_cast<AgcmSkill*>(GetModule("AgcmSkill"));
	if(pAgcmSkill)
		pAgcmSkill->DebugSkill_Render();

	}

	{
	// For the batch rendering of "AMBIENTOCCLUSIONMVL.FX"-applied atomics
	PROFILE("AgcmRender::AmbOcclMVL");
	RenderAmbOcclMVLAtomics();
	for( RwUInt32 i=0; i <FX_LIGHTING_VARIATION; ++i)
		m_ambOcclMVLAtomics[i].clear();
	}

	{
	PROFILE("AgcmRender::UVAnim");
	RenderUVAnimAtomics(m_listUVAnim);
	}
	
	//RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE , TRUE );
	{
	PROFILE("AgcmRender::AlphaAmbient");
	RenderAmbientAtomics();
	}

	{
	PROFILE("AgcmRender::AlphaFunc");
	RenderAlphaFuncAtomics();
	}

	{
	PROFILE("AgcmRender::Skin");
	RenderSkinPipeAtomics();
	}

	{
	PROFILE("AgcmRender::SkinAlphaFunc");
	RenderSkinPipeAlphaFuncAtomics();
	}

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE , (void*)TRUE  );

	{
	PROFILE("AgcmRender::UVAnimSkin");
	RenderUVAnimAtomics(m_listUVAnimSkin);
	}

	{
	PROFILE("AgcmRender::Alpha");
	RenderAlphaAtomics();
	}

	{
	PROFILE("AgcmRender::SkinAlpha");
	RenderSkinPipeAlphaAtomics();
	}

	{
	PROFILE("AgcmRender::RenderLastZWriteAtomics");
	RenderLastZWriteAtomics();
	}

	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)false );

	D3DMATERIAL9		lMaterial;
	memcpy(&lMaterial,&LastMaterial,sizeof(D3DMATERIAL9));

	lMaterial.Emissive.r = 0.0f;
    lMaterial.Emissive.g = 0.0f;
    lMaterial.Emissive.b = 0.0f;
    lMaterial.Emissive.a = 1.0f;

	RwD3D9SetMaterial( &lMaterial );

	{
	PROFILE("AgcmRender::SkinBlend");
	RenderSkinPipeBlendAtomics();
	}	

	{
	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsDummyCallBack);	// No light binding
	
	PROFILE("AgcmRender::PostRender");
	EnumCallback( AGCMRENDER_CB_ID_POSTRENDER, NULL, NULL );
	RpWorldSetGetAtomicLocalLightsCallBack(MyWorldGetAtomicLocalLightsCallBack);		// Restore the original callback.
	}

	{
	PROFILE("AgcmRender::PostRenderOC");
	EnumCallback( AGCMRENDER_CB_ID_POSTRENDER_OCTREE_CLEAR, NULL, NULL );		// 새로운 camera로 지평선 체크 수행하기 위해.. renderlist는 두고 지평선과 사전 검사 list 지워도 될 경우..
	}

	// 지형 반사 그릴때 ambient가 변경된다..
	RwD3D9SetRenderState( D3DRS_AMBIENT , 0xffffffff );

	RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, ( void * )rwTEXTUREADDRESSCLAMP  );

	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// Render Sorted Alpha Atomic
	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );
	{
	PROFILE("AgcmRender::BlendSort");
	RenderSortedBlendAtomics();			// 나중에 위로 올림(ZBUFFER에 적는다)
	}

	{
	PROFILE("AgcmRender::BlendNotSort");
	RenderNotSortedBlendAtomics();
	}

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );
	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);

	{
	PROFILE("AgcmRender::PostRender2");
	EnumCallback( AGCMRENDER_CB_ID_POSTRENDER2, NULL, NULL );
	}

	//@{ Jaewon 20050915
	{
		PROFILE("AgcmRender::RenderInvisibleAtomics");
		RenderInvisibleAtomics();
	}
	//@} Jaewon

	EnumCallback( AGCMRENDER_CB_CUSTOM_RENDER, NULL, NULL );

	ClearLists();
	
	{
	PROFILE("PostFX::Render");
	if( m_pcsAgcmPostFX )
		m_pcsAgcmPostFX->render();
	}
}


void	AgcmRender::SetWorldCamera( RpWorld* pWorld, RwCamera* pCamera )
{
	m_pWorld = pWorld;
	m_pCamera = pCamera;

	RwRaster* pRas = RwCameraGetRaster(pCamera);

	m_iCameraRasterWidth = RwRasterGetWidth(pRas);
	m_iCameraRasterHeight = RwRasterGetHeight(pRas);
}

void	AgcmRender::SetLight( RpLight* pAmbient, RpLight* pDirect )
{
	m_pAmbientLight	= pAmbient;
	m_pDirectLight	= pDirect;

	LightValueUpdate();
}

// Add ########################################################################################################
BOOL	AgcmRender::MyWorldAddAtomic(RpAtomic*	pAtomic)
{
	if( m_pApmOcTree->m_bOcTreeEnable )
	{
		AgcdType*	lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
		INT32	lIndex = lpType->eType;

		switch( AcuObject::GetType(lIndex) )
		{
		case ACUOBJECT_TYPE_WORLDSECTOR:
			{
				RwSphere*	sphere = RpAtomicGetBoundingSphere (pAtomic);
				OcTreeRoot* pRoot = m_pApmOcTree->GetRootByWorldPos(sphere->center.x, sphere->center.z, TRUE);
				if(!pRoot)			return FALSE;

				OcRootData*	pRootData = m_pAgcmOcTree->GetRootData(pRoot);
				if(!pRootData)		return FALSE;

				pRootData->terrain = pAtomic;
			}
			break;
		case ACUOBJECT_TYPE_CHARACTER:
		case ACUOBJECT_TYPE_ITEM:
			{
				if ( !AddAtomicToRenderWorld(pAtomic,RpAtomicGetClump(pAtomic)) )
					return FALSE;
			}
			break;
		default:
			{
				INT32	flag = AcuObject::GetProperty(lIndex);
				if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
					if ( !AddAtomicToRenderWorld(pAtomic,RpAtomicGetClump(pAtomic)) )
						return FALSE;
			}
			break;
		}
	}
	else
	{
		if( m_bUseCullMode )		// Cull
		{
			INT32	lIndex = AcuObject::GetAtomicType( pAtomic, NULL );
			INT32	type = AcuObject::GetType( lIndex );

			switch( AcuObject::GetType(lIndex) )
			{
			case ACUOBJECT_TYPE_WORLDSECTOR:
				AddTerrainToSector(pAtomic);
				break;
			case ACUOBJECT_TYPE_CHARACTER:
			case ACUOBJECT_TYPE_ITEM:
				if( !AddAtomicToRenderWorld( pAtomic,RpAtomicGetClump(pAtomic) ) )
					return FALSE;
				break;
			default:
				{
					INT32	flag = AcuObject::GetProperty(lIndex);
					if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
					{
						if( !AddAtomicToRenderWorld( pAtomic, RpAtomicGetClump(pAtomic) ) )
							return FALSE;
					}
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) && DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
						AddAtomicToSector( pAtomic, eAddSectorNoInSectorMove );
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
						AddAtomicToSector( pAtomic, eAddSectorNoInSectorStatic );
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) )
						AddAtomicToSector( pAtomic, eAddSectorInSectorMove );
					else
						AddAtomicToSector( pAtomic, eAddSectorInSectorStatic );
				}
				break;
			}
		}
		else
		{
			#ifdef _DPVS
			if ( eOccOption == AGCMRENDER_OP_OCCLUDER)
			{
				if ( !RpDPVSWorldAddOccludingAtomic( m_pWorld, pAtomic ) )
					return FALSE;
			}
			else
			{
				if ( !RpDPVSWorldAddNonOccludingAtomic( m_pWorld, pAtomic, rpDPVS_USE_BOUNDING_BOX ) )
					return FALSE;
			}
			#else
			if (!AddAtomicToRenderWorld(pAtomic,RpAtomicGetClump(pAtomic))) return FALSE;
			#endif //_DPVS
		}
	}

	EnumCallback( AGCMRENDER_CB_ID_ADDATOMIC, pAtomic, NULL );

	return TRUE;
}

BOOL	AgcmRender::MyWorldAddClump(RpClump*	pClump)
{
	if( m_pApmOcTree->m_bOcTreeEnable )
	{
		INT32	lIndex = pClump->stType.eType;
		INT32	type = AcuObject::GetType(lIndex);
		switch( AcuObject::GetType(lIndex) )
		{
			case ACUOBJECT_TYPE_WORLDSECTOR:
				return FALSE;
			case ACUOBJECT_TYPE_CHARACTER:
			case ACUOBJECT_TYPE_ITEM:
				{
					if( !(((AgpdCharacter*)((*pClump).stType).pObject)->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_POLYMORPH) )
						StartFadeInClump( pClump, 0 );

					if( !AddClumpToRenderWorld(pClump) )
						return FALSE;
				}
				break;
			case ACUOBJECT_TYPE_OBJECT:
				m_pAgcmOcTree->AddClumpToOcTree(pClump);
				break;
			default:
				{
					INT32	flag = AcuObject::GetProperty(lIndex);
					if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
						if( !AddClumpToRenderWorld( pClump ) )
							return FALSE;
				}
				break;
		}
	}
	else
	{
		if( m_bUseCullMode )		// Cull
		{
			INT32	lIndex = pClump->stType.eType;
			INT32	type = AcuObject::GetType( lIndex );

			switch( AcuObject::GetType( lIndex ) )
			{
			case ACUOBJECT_TYPE_CHARACTER:
			case ACUOBJECT_TYPE_ITEM:
				if( !AddClumpToRenderWorld( pClump ) )
					return FALSE;
				break;
			default:
				{
					INT32	flag = AcuObject::GetProperty( lIndex );
					if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
					{
						if( !AddClumpToRenderWorld( pClump ) )
							return FALSE;
					}
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) && DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
						AddClumpToSector( pClump, eAddSectorNoInSectorMove );
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
						AddClumpToSector( pClump, eAddSectorNoInSectorStatic );
					else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) )
						AddClumpToSector( pClump, eAddSectorInSectorMove );
					else
						AddClumpToSector( pClump, eAddSectorInSectorStatic );
				}
				break;
			}
		}
		else
		{
			if( !AddClumpToRenderWorld( pClump ) ) 
				return FALSE;
		}
	}

	EnumCallback( AGCMRENDER_CB_ID_ADDCLUMP, pClump, NULL );

	AddToRwFrameMapTable( RpClumpGetFrame( pClump ) );	

	return TRUE;
}

BOOL	AgcmRender::MyWorldRemoveAtomic(RpAtomic*	pAtomic)
{
	if( !pAtomic ) return FALSE;

	AgcdType* lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
	if( !lpType ) return FALSE;
	if( !m_pApmOcTree )
	{
		m_pApmOcTree = (ApmOcTree*)GetModule("ApmOcTree");
		if( !m_pApmOcTree )
		{
			return FALSE;
		}
	}

	if( m_pApmOcTree->m_bOcTreeEnable)
	{
		INT32	lIndex = lpType->eType;
		INT32	type = AcuObject::GetType( lIndex );
		
		if( ACUOBJECT_TYPE_WORLDSECTOR == type )
		{
			RwSphere* sphere = RpAtomicGetBoundingSphere( pAtomic );
			OcTreeRoot* pRoot = m_pApmOcTree->GetRootByWorldPos( sphere->center.x, sphere->center.z );
			if( !pRoot )		return FALSE;

			OcRootData*	pRootData = m_pAgcmOcTree->GetRootData(pRoot);
			if( !pRootData )	return FALSE;

			if( pRootData->terrain == pAtomic )
				pRootData->terrain = NULL;
		}
		else	// world 에 추가(atomic은 object로 들어오는거 생각 않는다.)
		{
			if( !RemoveAtomicFromRenderWorld( pAtomic, RpAtomicGetClump( pAtomic ) ) )
				return FALSE;
		}
	}
	else
	{
		if( m_bUseCullMode )		// Cull
		{
			INT32	lIndex = lpType->eType;
			INT32	type = AcuObject::GetType(lIndex);

			if( ACUOBJECT_TYPE_CHARACTER == type || ACUOBJECT_TYPE_ITEM == type )		// object list에 추가
			{
				if( !RemoveAtomicFromRenderWorld( pAtomic, RpAtomicGetClump( pAtomic ) ) )
					return FALSE;
			}
			else
			{
				INT32 flag = AcuObject::GetProperty(lIndex);
				if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
				{
					if( !RemoveAtomicFromRenderWorld( pAtomic, RpAtomicGetClump( pAtomic ) ) )
						return FALSE;
				}
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) && DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
					RemoveAtomicFromSector( pAtomic, eAddSectorNoInSectorMove );
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
					RemoveAtomicFromSector( pAtomic, eAddSectorNoInSectorStatic );
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) )
					RemoveAtomicFromSector( pAtomic, eAddSectorInSectorMove );
				else
					RemoveAtomicFromSector( pAtomic, eAddSectorInSectorStatic );
			}
		}
		else
		{
			if( !RemoveAtomicFromRenderWorld( pAtomic, RpAtomicGetClump( pAtomic ) ) )
				return FALSE;
		}
	}

	EnumCallback( AGCMRENDER_CB_ID_REMOVEATOMIC, pAtomic, NULL );

	return	TRUE;
}

BOOL	AgcmRender::MyWorldRemoveClump(RpClump*	pClump)
{
	if( m_pApmOcTree->m_bOcTreeEnable)
	{
		INT32	lIndex = pClump->stType.eType;
		INT32	type = AcuObject::GetType(lIndex);
		INT32	flag = AcuObject::GetProperty(lIndex);

		switch( AcuObject::GetType(lIndex) )
		{
		case ACUOBJECT_TYPE_WORLDSECTOR:
			return FALSE;
		case ACUOBJECT_TYPE_CHARACTER:
		case ACUOBJECT_TYPE_ITEM:
			if( !RemoveClumpFromRenderWorld(pClump) )
				return FALSE;
			break;
		case ACUOBJECT_TYPE_OBJECT:
			m_pAgcmOcTree->RemoveClumpFromOcTree( pClump );
			break;
		default:
			if( !RemoveClumpFromRenderWorld( pClump ) )
				return FALSE;
			break;
		}
	}
	else
	{
		if( m_bUseCullMode )		// Cull
		{
			INT32	lIndex = pClump->stType.eType;
			INT32	type = AcuObject::GetType(lIndex);
			
			if( ACUOBJECT_TYPE_CHARACTER == type || ACUOBJECT_TYPE_ITEM == type )		// object list에 추가
			{
				if( !RemoveClumpFromRenderWorld( pClump ) )
					return FALSE;
			}
			else
			{
				INT32	flag = AcuObject::GetProperty( lIndex );
				if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_WORLDADD ) )
				{
					if( !RemoveClumpFromRenderWorld( pClump ) )
						return FALSE;
				}
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) && DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
					RemoveClumpFromSector( pClump, eAddSectorNoInSectorMove );
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_NO_INTERSECTION ) )
					RemoveClumpFromSector( pClump, eAddSectorNoInSectorStatic );
				else if( DEF_FLAG_CHK( flag, ACUOBJECT_TYPE_MOVE_INSECTOR ) )
					RemoveClumpFromSector( pClump, eAddSectorInSectorMove );
				else
					RemoveClumpFromSector( pClump, eAddSectorInSectorStatic );
			}
		}
		else
		{
			if( !RemoveClumpFromRenderWorld( pClump ) )
				return FALSE;
		}
	}

	EnumCallback( AGCMRENDER_CB_ID_REMOVECLUMP, pClump, NULL );

	return TRUE;
}

bool	AgcmRender::AddClumpToRenderWorld( RpClump*	pClump )
{
	if( !pClump )							return false;
	if( pClump->stType.pRenderAddedNode )	return false;
		
	WorldAddClumpGroup*	nw_data = new WorldAddClumpGroup;
	nw_data->pClump			= pClump;
	nw_data->BS				= pClump->stType.boundingSphere;
	nw_data->WorldBS.radius = pClump->stType.boundingSphere.radius;

	OcTreeRenderData2*	pData = (OcTreeRenderData2*)pClump->stType.pOcTreeData;
	if( pData )
		memcpy( nw_data->MaxBBox, pData->topVerts_MAX, sizeof(RwV3d) * 4 );
	else
	{
		nw_data->MaxBBox[0].x = nw_data->BS.center.x - nw_data->BS.radius;
		nw_data->MaxBBox[0].y = nw_data->BS.center.y + nw_data->BS.radius;
		nw_data->MaxBBox[0].z = nw_data->BS.center.z - nw_data->BS.radius;

		nw_data->MaxBBox[1].x = nw_data->BS.center.x + nw_data->BS.radius;
		nw_data->MaxBBox[1].y = nw_data->BS.center.y + nw_data->BS.radius;
		nw_data->MaxBBox[1].z = nw_data->BS.center.z - nw_data->BS.radius;

		nw_data->MaxBBox[2].x = nw_data->BS.center.x + nw_data->BS.radius;
		nw_data->MaxBBox[2].y = nw_data->BS.center.y + nw_data->BS.radius;
		nw_data->MaxBBox[2].z = nw_data->BS.center.z + nw_data->BS.radius;

		nw_data->MaxBBox[3].x = nw_data->BS.center.x - nw_data->BS.radius;
		nw_data->MaxBBox[3].y = nw_data->BS.center.y + nw_data->BS.radius;
		nw_data->MaxBBox[3].z = nw_data->BS.center.z + nw_data->BS.radius;
	}

	nw_data->Atomiclist	= NULL;
	RpClumpForAllAtomics( pClump, SetAtomicListCB, (PVOID)nw_data );
	nw_data->prev		= NULL;
	nw_data->next		= m_listWorldClumps;

	if( m_listWorldClumps )
		m_listWorldClumps->prev = nw_data;
	m_listWorldClumps	= nw_data;

	pClump->stType.pRenderAddedNode = nw_data;

	AddToRwFrameMapTable( RpClumpGetFrame( pClump ) );
	
	return true;
}

bool	AgcmRender::RemoveClumpFromRenderWorld( RpClump* pClump )
{
	PROFILE("Render - RemoveClumpFromRenderWorld");

	if( !pClump->stType.pRenderAddedNode )		return false;

	WorldAddClumpGroup* cur_clump = (WorldAddClumpGroup*) pClump->stType.pRenderAddedNode;
	ASSERT( !cur_clump->Atomiclist );
	if(cur_clump->pClump->atomicList)
	{
		RpAtomic* cur_atomic = cur_clump->pClump->atomicList;
		do
		{
			cur_atomic->ulFlag	= cur_atomic->ulFlag & ~(RWFLAG_RENDER_ADD);
			cur_atomic			= cur_atomic->next;
		}
		while(cur_atomic != cur_clump->pClump->atomicList);
	}

	if( cur_clump == m_listWorldClumps )
	{
		m_listWorldClumps = cur_clump->next;
		if( m_listWorldClumps )
			m_listWorldClumps->prev = NULL;
	}
	else 
	{
		cur_clump->prev->next = cur_clump->next;
		if( cur_clump->next )
			cur_clump->next->prev = cur_clump->prev;
	}

	delete cur_clump;
	cur_clump = NULL;

	pClump->stType.pRenderAddedNode = NULL;

	return true;
}

// 기존에 ClumpGroup이 있으면 그곳에 추가, 없으면 ClumpGroup을 만든다. 
bool	AgcmRender::AddAtomicToRenderWorld( RpAtomic* pAtomic, RpClump* pClump )
{
	if( pClump )
	{
		ASSERT( pClump->stType.eType & ACUOBJECT_TYPE_ITEM );
		return true;
	}

	WorldAddClumpGroup* nw_data = new WorldAddClumpGroup;
	nw_data->pClump = NULL;

	RwSphere* BS = RpAtomicGetBoundingSphere(pAtomic);
	nw_data->BS	= *BS;

	nw_data->WorldBS.radius = BS->radius;

	nw_data->MaxBBox[0].x = BS->center.x - BS->radius;
	nw_data->MaxBBox[0].y = BS->center.y + BS->radius;
	nw_data->MaxBBox[0].z = BS->center.z - BS->radius;

	nw_data->MaxBBox[1].x = BS->center.x + BS->radius;
	nw_data->MaxBBox[1].y = BS->center.y + BS->radius;
	nw_data->MaxBBox[1].z = BS->center.z - BS->radius;

	nw_data->MaxBBox[2].x = BS->center.x + BS->radius;
	nw_data->MaxBBox[2].y = BS->center.y + BS->radius;
	nw_data->MaxBBox[2].z = BS->center.z + BS->radius;

	nw_data->MaxBBox[3].x = BS->center.x - BS->radius;
	nw_data->MaxBBox[3].y = BS->center.y + BS->radius;
	nw_data->MaxBBox[3].z = BS->center.z + BS->radius;

	nw_data->Atomiclist = pAtomic;
	ASSERT( !pAtomic->next );

	nw_data->prev = NULL;

	nw_data->next = m_listWorldClumps;
	if( m_listWorldClumps )
		m_listWorldClumps->prev = nw_data;
	m_listWorldClumps = nw_data;

	ASSERT(pAtomic->stType);
	if( pAtomic->stType )
		pAtomic->stType->pRenderAddedNode = nw_data;

	AddToRwFrameMapTable( RpAtomicGetFrame( pAtomic ) );

	return true;
}

bool	AgcmRender::RemoveAtomicFromRenderWorld( RpAtomic*	pAtomic, RpClump*	pClump )
{
	if(pClump)
	{
		ASSERT(pClump->stType.eType & ACUOBJECT_TYPE_ITEM);
		return true;
	}

	if( !pAtomic->stType )
	{
		ASSERT(!"RenderWorld Atomic Remove 실패");
		return false;
	}

	WorldAddClumpGroup* cur_clump = (WorldAddClumpGroup*) pAtomic->stType->pRenderAddedNode;
	if( !cur_clump )
		return false;

	if( cur_clump->pClump )
	{
		ASSERT(!"atomic 삭제 실패");
		return false;
	}

	RpAtomic* cur_atomic = cur_clump->Atomiclist;
	ASSERT( !cur_atomic->next && cur_atomic == pAtomic );

	while( cur_atomic )
	{
		if(cur_atomic == pAtomic)
		{
			// clump 삭제
			if(cur_clump == m_listWorldClumps)
			{
				m_listWorldClumps = cur_clump->next;
				if( m_listWorldClumps )
					m_listWorldClumps->prev = NULL;
			}
			else 
			{
				cur_clump->prev->next = cur_clump->next;
				if( cur_clump->next )
					cur_clump->next->prev = cur_clump->prev;
			}

			delete cur_clump;
			cur_clump = NULL;

			pAtomic->stType->pRenderAddedNode = NULL;

			return true;
		}

		cur_atomic = cur_atomic->next;
	}

	ASSERT(!"atomic 삭제 실패!");
	return false;
}

void	AgcmRender::UpdateBoundingSphere(RpClump*	pClump)
{
	ASSERT( pClump );
	if( !pClump )		return;

	WorldAddClumpGroup* cur_clump = (WorldAddClumpGroup*) pClump->stType.pRenderAddedNode;
	if( !cur_clump )	return;
	ASSERT( cur_clump->pClump == pClump );

	cur_clump->BS = pClump->stType.boundingSphere;
}

bool	AgcmRender::AddLightToRenderWorld( RpLight* pLight, BOOL bMove )
{
	RpLightType ltType = RpLightGetType( pLight );
	if( rpLIGHTDIRECTIONAL == ltType || rpLIGHTAMBIENT == ltType )		return false;
	
	LocalLight* nw_light = new LocalLight;
	nw_light->bMove		= bMove;
	nw_light->pLight	= pLight;
	
	RwMatrix* ltm = RwFrameGetLTM( RpLightGetFrame( pLight ) );
	
	if(ltType == rpLIGHTPOINT)
	{
		nw_light->BS.radius = RpLightGetRadius(pLight);
		nw_light->BS.center.x = nw_light->BS.center.y = nw_light->BS.center.z = 0.0f;

		nw_light->WorldBS.radius = nw_light->BS.radius;
		nw_light->WorldBS.center = ltm->pos;
	}
	else if(ltType == rpLIGHTSPOT || ltType == rpLIGHTSPOTSOFT)
	{
		// cone의 중앙에 BS center 위치, radius 계산
		nw_light->BS.radius = RpLightGetRadius(pLight);
		nw_light->BS.center.x = nw_light->BS.center.y = 0.0f;
		nw_light->BS.center.z = nw_light->BS.radius * 0.5f;

		nw_light->WorldBS.radius = nw_light->BS.radius * 0.6f;

		RwV3dTransformPoint( &nw_light->WorldBS.center, &nw_light->BS.center, ltm );
	}

	nw_light->next = m_listLocalLight;
	m_listLocalLight = nw_light;
	
	if( !bMove && m_pApmOcTree->m_bOcTreeEnable )		// octree 에 추가
	{
		OcCustomDataList	stSetParam;
		stSetParam.pData2			= NULL;
		stSetParam.iAppearanceDistance	= 3;
		stSetParam.pClass			= this;
		stSetParam.pRenderCB		= NULL;
		stSetParam.pUpdateCB		= CB_LIGHT_UPDATE;
		stSetParam.pDistCorrectCB	= NULL;
		stSetParam.pData1			= nw_light;
		stSetParam.BS.center		= nw_light->WorldBS.center;
		stSetParam.BS.radius		= nw_light->WorldBS.radius;
		stSetParam.piCameraZIndex	= &nw_light->iCameraIndex; 

		stSetParam.TopVerts[0].x = nw_light->WorldBS.center.x - nw_light->WorldBS.radius;
		stSetParam.TopVerts[0].y = nw_light->WorldBS.center.y + nw_light->WorldBS.radius;
		stSetParam.TopVerts[0].z = nw_light->WorldBS.center.z - nw_light->WorldBS.radius;

		stSetParam.TopVerts[1].x = nw_light->WorldBS.center.x + nw_light->WorldBS.radius;
		stSetParam.TopVerts[1].y = nw_light->WorldBS.center.y + nw_light->WorldBS.radius;
		stSetParam.TopVerts[1].z = nw_light->WorldBS.center.z - nw_light->WorldBS.radius;

		stSetParam.TopVerts[2].x = nw_light->WorldBS.center.x + nw_light->WorldBS.radius;
		stSetParam.TopVerts[2].y = nw_light->WorldBS.center.y + nw_light->WorldBS.radius;
		stSetParam.TopVerts[2].z = nw_light->WorldBS.center.z + nw_light->WorldBS.radius;

		stSetParam.TopVerts[3].x = nw_light->WorldBS.center.x - nw_light->WorldBS.radius;
		stSetParam.TopVerts[3].y = nw_light->WorldBS.center.y + nw_light->WorldBS.radius;
		stSetParam.TopVerts[3].z = nw_light->WorldBS.center.z + nw_light->WorldBS.radius;

		m_pAgcmOcTree->AddCustomRenderDataToOcTree( nw_light->WorldBS.center.x, nw_light->WorldBS.center.y, nw_light->WorldBS.center.z, &stSetParam );
	}
	else	
	{
		// Show the bounding sphere of a light in Maptool.
#ifdef USE_MFC
		AddCustomRenderObject( this, &nw_light->WorldBS, 0.f, CB_LIGHT_UPDATE, CB_LIGHT_RENDER, (PVOID)nw_light, NULL);
#else
		AddCustomRenderObject( this, &nw_light->WorldBS, 0.f, CB_LIGHT_UPDATE, NULL, (PVOID)nw_light, NULL );
#endif
	}

	ApWorldSector *pSectors[25];
	AuPOS pos;
	pos.x = nw_light->WorldBS.center.x;
	pos.y = nw_light->WorldBS.center.y;
	pos.z = nw_light->WorldBS.center.z;
	RwInt32 count = m_pcApmMap->GetSectorList(pos, nw_light->WorldBS.radius, pSectors, 25);

	for(RwInt32 i=0; i<count; ++i)
	{
#ifdef	USE_MFC
		SectorRenderList *pSectorData = GetSectorData(pSectors[i]);
#else
		SectorData *pSectorData = GetClientSectorData(pSectors[i]);
#endif
		pSectorData->listLocalLight->push_back( nw_light );
	}

	return true;
}

bool	AgcmRender::RemoveLightFromRenderWorld( RpLight* pLight , BOOL bMove )
{
	LocalLight*		cur_light = m_listLocalLight;
	LocalLight		*remove_light,*before_light;
	BOOL				bFind = FALSE;

	while(cur_light)
	{
		if(cur_light->pLight == pLight)
		{
			bFind = TRUE;
			remove_light = cur_light;

			if(cur_light == m_listLocalLight)
			{
				m_listLocalLight = cur_light->next;
			}
			else
			{
				before_light->next = cur_light->next;
			}
			break;
		}
		else
		{
			before_light = cur_light;
		}

		cur_light = cur_light->next;
	}

	if( !bFind )		return false;

	if( !bMove && m_pApmOcTree->m_bOcTreeEnable )
	{
		m_pAgcmOcTree->RemoveCustomRenderDataFromOcTree(remove_light->WorldBS.center.x, remove_light->WorldBS.center.y, remove_light->WorldBS.center.z, this, (PVOID)remove_light, NULL );
	}
	else
	{
		RemoveCustomRenderObject(this,(PVOID)remove_light,NULL);
	}

	ApWorldSector *pSectors[25];
	AuPOS pos;
	pos.x = remove_light->WorldBS.center.x;
	pos.y = remove_light->WorldBS.center.y;
	pos.z = remove_light->WorldBS.center.z;
	RwInt32 count = m_pcApmMap->GetSectorList(pos, remove_light->WorldBS.radius, pSectors, 25);

	for( RwInt32 i = 0; i < count; ++i )
	{
#ifdef	USE_MFC
		SectorRenderList *pSectorData = GetSectorData(pSectors[i]);
#else
		SectorData *pSectorData = GetClientSectorData(pSectors[i]);
#endif
		LocalLightList* listLocalLight = pSectorData->listLocalLight;
		LocalLightListItr Itr = find( listLocalLight->begin(), listLocalLight->end(), remove_light );
		if( Itr != listLocalLight->end() )
			listLocalLight->erase( Itr );
	}

	delete remove_light;
	remove_light = NULL;
	return true;
}

// Consider local light lists of sectors which the atomic touches.
// Use the light's intensity at the center of the atomic as a sorting criterion.
const	int			SearchOrder[15] = { 0, -1, 1, -2, 2, -3, 3, -4, 4, -5, 5, -6, 6, -7, 7 };
const	RwUInt32	MAX_NUMBER_OF_LOCAL_LIGHTS = 7;
INT32 AgcmRender::GetLightByAtomic(RpAtomic *pAtomic, RpLight *pLights[])
{
	PROFILE("GetLightByAtomic");

	RwInt32 numFound = 0;
	RwReal intensityList[MAX_NUMBER_OF_LOCAL_LIGHTS];

	const RwSphere *pSphere = RpAtomicGetWorldBoundingSphere(pAtomic);

	ApWorldSector* pSectors[27];
	AuPOS pos;
	pos.x = pSphere->center.x;
	pos.y = pSphere->center.y;
	pos.z = pSphere->center.z;
	RwInt32 count = m_pcApmMap->GetSectorList( pos, min( MAP_SECTOR_WIDTH - 1.0f, pSphere->radius ), pSectors, 27 );
	ASSERT(count<=27);

	for( RwInt32 i=0; i<count; ++i )
	{
#ifdef	USE_MFC
		SectorRenderList *pSectorData = GetSectorData(pSectors[i]);
#else
		SectorData *pSectorData = GetClientSectorData(pSectors[i]);
#endif
		std::list<LocalLight*>::const_iterator iter = pSectorData->listLocalLight->begin();
		std::list<LocalLight*>::const_iterator end = pSectorData->listLocalLight->end();
		for( ; iter != end; ++iter )
		{
			RwV3d D;
			RwV3dSub( &D, &((*iter)->WorldBS.center), &(pSphere->center) );
			RwReal d = RwV3dLength(&D);
			if( d > (*iter)->WorldBS.radius + pSphere->radius )
				continue;

			RwReal intensity = 1.0f - min(1.0f, d / ((*iter)->WorldBS.radius+0.001f));

			// Sort.
			bool alreadyInList = false;
			RwInt32 j;
			for( j=0; j<numFound; ++j)
			{
				if( intensityList[j] < intensity )
					break;

				else if( (*iter)->pLight == pLights[j] )
				{
					alreadyInList = true;
					break;
				}
			}
			if( alreadyInList )		continue;
				
			RwUInt32 size = numFound - j;
			if(size > 0 && numFound == MAX_NUMBER_OF_LOCAL_LIGHTS)
				size -= 1;
			if(size > 0)
			{
				memmove(pLights+j+1, pLights+j, size*sizeof(RpLight*));
				memmove(intensityList+j+1, intensityList+j, size*sizeof(RwReal));
			}
			if(j<MAX_NUMBER_OF_LOCAL_LIGHTS)
			{
				pLights[j] = (*iter)->pLight;
				intensityList[j] = intensity;
				if(numFound < MAX_NUMBER_OF_LOCAL_LIGHTS)
					++numFound;
			}
		}
	}

	return numFound;
}

//@{ Jaewon 20050418
BOOL	AgcmRender::CB_LIGHT_UPDATE ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmRender*		pThis = (AgcmRender*) pClass;

	LocalLight*		pLocalLight = (LocalLight*) pData;
	if( !pLocalLight->bMove )		return TRUE;

	RwFrame* pFrame = RpLightGetFrame( pLocalLight->pLight );
	if( !pFrame )		return FALSE;

	RwMatrix* ltm = RwFrameGetLTM(pFrame);

	AuPOS pos;
	pos.x = pLocalLight->WorldBS.center.x;
	pos.y = pLocalLight->WorldBS.center.y;
	pos.z = pLocalLight->WorldBS.center.z;
	
	pLocalLight->WorldBS.center.x = ltm->pos.x + pLocalLight->BS.center.x;
	pLocalLight->WorldBS.center.y = ltm->pos.y + pLocalLight->BS.center.y;
	pLocalLight->WorldBS.center.z = ltm->pos.z + pLocalLight->BS.center.z;

	// Unregister a local light from sectors in the light's previous range.
	ApWorldSector *pSectors[25];
	RwInt32 count = pThis->m_pcApmMap->GetSectorList(pos, pLocalLight->WorldBS.radius, pSectors, 25);

	for(RwInt32 i=0; i<count; ++i)
	{
#ifdef	USE_MFC
		SectorRenderList *pSectorData = pThis->GetSectorData(pSectors[i]);
#else
		SectorData *pSectorData = pThis->GetClientSectorData(pSectors[i]);
#endif
		std::list<LocalLight*>::iterator iter = pSectorData->listLocalLight->begin();
		std::list<LocalLight*>::iterator end = pSectorData->listLocalLight->end();
		for(; iter!=end; ++iter)
		{
			if(pLocalLight == *iter)
			{
				pSectorData->listLocalLight->erase(iter);
				break;
			}
		}
	}

	// Register a local light to sectors in the new light's range.
	pos.x = pLocalLight->WorldBS.center.x;
	pos.y = pLocalLight->WorldBS.center.y;
	pos.z = pLocalLight->WorldBS.center.z;
	count = pThis->m_pcApmMap->GetSectorList(pos, pLocalLight->WorldBS.radius, pSectors, 25);

	for(RwInt32 i=0; i<count; ++i)
	{
#ifdef	USE_MFC
		SectorRenderList *pSectorData = pThis->GetSectorData(pSectors[i]);
#else
		SectorData *pSectorData = pThis->GetClientSectorData(pSectors[i]);
#endif
		pSectorData->listLocalLight->push_back(pLocalLight);
	}

	return TRUE;
}
//@} Jaewon

BOOL AgcmRender::CB_LIGHT_RENDER(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmRender *pThis = (AgcmRender*)pClass;
	LocalLight *pLocalLight = (LocalLight*)pData;

	return TRUE;
}

RwUInt32 AgcmRender::MyWorldGetAtomicLocalLightsCallBack(RpAtomic *pAtomic, RpLight *pLights[])
{
	return m_pThisAgcmRender->GetLightByAtomic( pAtomic, pLights );
}	

RwUInt32 AgcmRender::MyWorldGetAtomicLocalLightsDummyCallBack(RpAtomic *pAtomic, RpLight *pLights[])
{
	return 0;
}	

RpAtomic*	AgcmRender::SetAtomicListCB( RpAtomic*	atomic, void*	data)
{
	WorldAddClumpGroup*	pData = (WorldAddClumpGroup*)data;
	atomic->ulFlag = atomic->ulFlag | RWFLAG_RENDER_ADD;

	return atomic;
}

void	AgcmRender::InitAtomicSetRenderCallBack( RpAtomic* atomic, RpAtomicCallBackRender pCB, enumRenderType eRenderType, enumRenderBMode eRenderBMode, BOOL bBill )
{
	RpLODAtomicUnHookRender(atomic);

	// If the current render callback has been already set to 'RenderCallbackForSpecialRendering',
	// use the 'pCB' parameter instead of it to initialize 'atomic->stRenderInfo.backupCB'.
	RpAtomicCallBackRender renderCallBack = RpAtomicGetRenderCallBack(atomic);
	AcuObject::SetAtomicRenderUDA( atomic, eRenderType, renderCallBack == RenderCallbackForSpecialRendering ? pCB:renderCallBack, eRenderBMode, bBill, 0 );
								
	atomic->stRenderInfo.backupCB2 = (void*)pCB;
	RpAtomicSetRenderCallBack( atomic, pCB );
	RpLODAtomicHookRender( atomic );
}

bool	AgcmRender::AddClumpToWorld	( RpClump* pClump,enumAddOption AddOption)
{
	PROFILE("AddClumpToWorld");

	if( pClump->ulFlag & RWFLAG_RENDER_ADD)		return true;		// 중복 Add이다..

	AgcdLODData*	pAgcdLODData = (AgcdLODData*)pClump->stType.pCustData;
	BOOL			bBill	= FALSE;

	INT32			eType = pClump->stType.eType;

	enumAlphaOption	 eAlphaOption = eType & ACUOBJECT_TYPE_USE_ALPHA ?  ONLY_ALPHA :
											(eType & ACUOBJECT_TYPE_USE_AMBIENT ? ONLY_AMBIENT  : 
											(eType & ACUOBJECT_TYPE_USE_ALPHAFUNC ? ONLY_ALPHAFUNC : ONLY_NONALPHA));
	
	if( pAgcdLODData && pAgcdLODData->m_lHasBillNum > 0 )
		bBill = TRUE;

	if( AcuObject::GetProperty(eType) & ACUOBJECT_TYPE_RENDER_UDA )
	{
		RpClumpForAllAtomics( pClump, SetRenderCBFromUDACB, (void*)&bBill );

		SetBoundingSphereFromUDA( pClump );
		MyWorldAddClump( pClump );
		SetSectorDistanceC(pClump);
	}
	else
	{
		//Alpha Check
		CheckAlphaAtomicFromClump( pClump, eAlphaOption , bBill );

		if( AddOption & AGCMRENDER_ADD_FADEIN )
		{
			int tmp = -1;
			RpClumpForAllAtomics( pClump, CallBackSetFadeIn, &tmp );
		}
	
		SetBoundingSphereFromUDA( pClump );
		MyWorldAddClump( pClump );
		SetSectorDistanceC( pClump );

		InitAtomicDatas( pClump );
	}

	pClump->ulFlag = pClump->ulFlag | RWFLAG_RENDER_ADD;

	return TRUE;
}

bool	AgcmRender::AddAlphaBModeClumpToWorld( RpClump* pClump, enumRenderBMode eBMode,BOOL bBill, enumAddOption AddOption)
{
	if( pClump->ulFlag & RWFLAG_RENDER_ADD ) return true;		// 중복 Add이다..	

	int	data[2] = { eBMode, bBill };
    
	RpClumpForAllAtomics(pClump, AtomicSetInfoBlendAlpha, (void*)data ); 

	if( AddOption & AGCMRENDER_ADD_FADEIN)
	{
		int	tmp = -1;
		RpClumpForAllAtomics( pClump, CallBackSetFadeIn, &tmp );
	}

	SetBoundingSphereFromUDA( pClump );
	MyWorldAddClump( pClump );
	SetSectorDistanceC(pClump);

	InitAtomicDatas(pClump);

	pClump->ulFlag = pClump->ulFlag | RWFLAG_RENDER_ADD;

	return true;
}

bool	AgcmRender::AddBillAtomicToWorld( RpAtomic* pAtomic, enumAlphaOption eAlphaOption, enumAddOption AddOption)
{
	if( pAtomic->ulFlag & RWFLAG_RENDER_ADD ) return true;		// 중복 Add이다..

	CheckAlphaAtomic( pAtomic, eAlphaOption , TRUE);

	SetBoundingSphereFromUDA( pAtomic );
	MyWorldAddAtomic(pAtomic);
	SetSectorDistanceA(pAtomic);

	InitAtomicDatas(pAtomic);

	pAtomic->ulFlag = pAtomic->ulFlag | RWFLAG_RENDER_ADD;
	
	return TRUE;
}

bool	AgcmRender::AddAtomicToWorld( RpAtomic* pAtomic, enumAlphaOption eAlphaOption, enumAddOption AddOption ,bool bCheck)
{
	PROFILE("AgcmRender::AddAtomicToWorld");

	if(pAtomic->ulFlag & RWFLAG_RENDER_ADD) return true;		// 중복 Add이다..

	AgcdType* lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
	
	if(bCheck && AcuObject::GetProperty(lpType->eType) & ACUOBJECT_TYPE_RENDER_UDA)
	{
		BOOL		bBill = FALSE;
		SetRenderCBFromUDACB( pAtomic,(void*)&bBill );
		SetBoundingSphereFromUDA( pAtomic );
		MyWorldAddAtomic( pAtomic );
		SetSectorDistanceA( pAtomic );
	}
	else
	{
		//Alpha Check
		CheckAlphaAtomic( pAtomic, eAlphaOption , FALSE);

		SetBoundingSphereFromUDA( pAtomic );
		MyWorldAddAtomic(pAtomic);
		SetSectorDistanceA(pAtomic);

		InitAtomicDatas(pAtomic);
	}

	pAtomic->ulFlag = pAtomic->ulFlag | RWFLAG_RENDER_ADD;

	return TRUE;
}

bool	AgcmRender::AddAlphaBModeAtomicToWorld(RpAtomic* pAtomic,enumRenderBMode eBMode,BOOL bBill, enumAddOption AddOption)
{
	if( pAtomic->ulFlag & RWFLAG_RENDER_ADD )		return true;		// 중복 Add이다..

	if( IsSkinedPipe( pAtomic ) )
	{
		InitAtomicSetRenderCallBack(pAtomic,RenderCallbackForSkinPipeBlend,R_SKIN,eBMode,FALSE);
	}
	else
	{
		if( eBMode == R_NONE )
			CheckAlphaAtomic( pAtomic, CHECK_ALPHA , FALSE );
		else if(eBMode < R_BLEND11_ADD)
		{
			if( m_pApmOcTree->m_bOcTreeEnable )	InitAtomicSetRenderCallBack( pAtomic, bBill ? RenderCallbackForBlendAtomicBill2 : RenderCallbackForBlendAtomic2, R_BLEND_SORT, eBMode, bBill );
			else								InitAtomicSetRenderCallBack( pAtomic, bBill ? RenderCallbackForBlendAtomicBill : RenderCallbackForBlendAtomic, R_BLEND_SORT, eBMode, bBill );
		}
		else
			InitAtomicSetRenderCallBack( pAtomic, bBill ? RenderCallbackForBlendAtomicNotSortBill : RenderCallbackForBlendAtomicNotSort, R_BLEND_NSORT, eBMode, bBill );
	}

	SetBoundingSphereFromUDA( pAtomic );
	MyWorldAddAtomic(pAtomic);
	SetSectorDistanceA(pAtomic);

	InitAtomicDatas(pAtomic);

	pAtomic->ulFlag = pAtomic->ulFlag | RWFLAG_RENDER_ADD;
	
	return true;
}

RpAtomic*	AgcmRender::AtomicSetInfoBlendAlpha			( RpAtomic *atomic, void *data )
{
	int*		val = (int*)data;

	if( m_pThisAgcmRender->IsSkinedPipe( atomic ) )
	{
		m_pThisAgcmRender->InitAtomicSetRenderCallBack(atomic,RenderCallbackForSkinPipeBlend,R_SKIN,(enumRenderBMode)val[0],FALSE);
	}
	else
	{
		if(val[0] == R_NONE)
		{
			m_pThisAgcmRender->CheckAlphaAtomic( atomic, CHECK_ALPHA, FALSE );
		}
		else if(val[0] < R_BLEND11_ADD)
		{
			if( m_pThisAgcmRender->m_pApmOcTree->m_bOcTreeEnable)
				m_pThisAgcmRender->InitAtomicSetRenderCallBack(atomic, val[1] ? RenderCallbackForBlendAtomicBill2 : RenderCallbackForBlendAtomic2, R_BLEND_SORT, (enumRenderBMode)val[0], (BOOL)val[1] );
			else
				m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, val[1] ? RenderCallbackForBlendAtomicBill : RenderCallbackForBlendAtomic, R_BLEND_SORT, (enumRenderBMode)val[0], (BOOL)val[1] );
		}
		else
			m_pThisAgcmRender->InitAtomicSetRenderCallBack(atomic, val[1] ? RenderCallbackForBlendAtomicNotSortBill : RenderCallbackForBlendAtomicNotSort,R_BLEND_NSORT,(enumRenderBMode)val[0],(BOOL)val[1]);
	}

	return atomic;
}

void	AgcmRender::InitAtomicDatas( RpClump* clump )
{
	RpClumpForAllAtomics( clump, InitAtomicDatasCB, NULL );
}

struct myD3D9InstanceNodeData
{
	RxD3D9AllInOneInstanceCallBack		instanceCallback;
	RxD3D9AllInOneReinstanceCallBack	reinstanceCallback;
	RxD3D9AllInOneRenderCallBack		renderCallback;
};

void	AgcmRender::InitAtomicDatas(RpAtomic*	atomic)
{
	PROFILE("InitAtomicDatas");
	AtomicFxCheck(atomic);

	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	if( geometry && atomic->stRenderInfo.renderType == R_NONALPHA )
	{
		if( RtAmbOcclMapAtomicGetFlags(atomic) & rtAMBOCCLMAPOBJECTAMBOCCLMAP )
			atomic->stRenderInfo.backupCB = AtomicAmbOcclMVLRenderCallBack;
	}
}

RpAtomic*	AgcmRender::InitAtomicDatasCB( RpAtomic *atomic, void *data )
{
	m_pThisAgcmRender->InitAtomicDatas(atomic);
	return atomic;
}

bool	AgcmRender::AddClumpFromUDA(RpClump*	pClump)
{
	AgcdLODData*	pAgcdLODData = (AgcdLODData*)pClump->stType.pCustData;

	BOOL bBill = pAgcdLODData && pAgcdLODData->m_lHasBillNum > 0 ? TRUE : FALSE;
	RpClumpForAllAtomics( pClump, SetRenderCBFromUDACB, (void*)&bBill );

	return true;
}

// InitAtomicDatas까지 해주자(최적화를 위해서)
RpAtomic*	AgcmRender::SetRenderCBFromUDACB(RpAtomic	*atomic, void*	data)
{
	PROFILE("SetRenderCBFromUDACB");

	BOOL	bBill = *((BOOL*)data);

	switch( atomic->stRenderInfo.renderType )
	{
	case R_NONALPHA:
		m_pThisAgcmRender->AtomicSetNonAlpha( atomic, data );
		break;
	case R_ALPHA:
		m_pThisAgcmRender->AtomicSetAlpha( atomic, data );
		break;
	case R_BLEND_NSORT:
	case R_BLEND_SORT:
		{
			enumRenderBMode	eBMode = (enumRenderBMode)atomic->stRenderInfo.blendMode;
			if( m_pThisAgcmRender->IsSkinedPipe( atomic ) )
			{
				m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, RenderCallbackForSkinPipeBlend, R_SKIN, eBMode, FALSE );
			}
			else
			{
				if( eBMode == R_NONE )
					m_pThisAgcmRender->AtomicSetAlpha( atomic, data );
				else if(eBMode < R_BLEND11_ADD)
				{
					if( m_pThisAgcmRender->m_pApmOcTree->m_bOcTreeEnable )
						m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForBlendAtomicBill2 : RenderCallbackForBlendAtomic2, R_BLEND_SORT, eBMode, bBill );
					else
						m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForBlendAtomicBill : RenderCallbackForBlendAtomic, R_BLEND_SORT, eBMode, bBill );
				}
				else
					m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForBlendAtomicNotSortBill : RenderCallbackForBlendAtomicNotSort, R_BLEND_NSORT, eBMode, bBill );
			}
		}
		break;
	case R_AMBIENT:
		m_pThisAgcmRender->AtomicSetAmbient( atomic, data );
		break;
	case R_ALPHAFUNC:
		m_pThisAgcmRender->AtomicSetAlphaFunc( atomic, data );
		break;
	default:
		m_pThisAgcmRender->AtomicSetNonAlpha( atomic, data );
		break;
	}

	m_pThisAgcmRender->InitAtomicDatas(atomic);

	return atomic;
}

// #Remove ########################################################################################################
bool	AgcmRender::RemoveClumpFromWorld( RpClump* pClump )
{
	if( !pClump )		
		return FALSE;

	PROFILE("Render - RemoveClumpFromWorld");

	// 2005.2.15 gemani 맵툴에서 오브젝트 이동시 이거 안해주니 rendercallback 문제 생긴다 .. 풀지 말자 gemani
	RpClumpForAllAtomics(pClump,ReturnDefOriginalRenderCB,(void*)NULL);

	if( pClump->stType.pUpdateList )
	{
		ListUpdateCallback*			pUpdateCallbackList		=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);
		ListUpdateCallbackIter		Iter					=	pUpdateCallbackList->begin();
		for( ; Iter != pUpdateCallbackList->end() ; )
		{
			UpdateCallbackList*		pUpdateCB		=	(*Iter);

			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );

			DEF_SAFEDELETE( pUpdateCB );
			pUpdateCallbackList->erase( Iter++ );
		}

		DEF_SAFEDELETE( pClump->stType.pUpdateList );
	}

	MyWorldRemoveClump(pClump);
	pClump->ulFlag = pClump->ulFlag & (~RWFLAG_RENDER_ADD);

	return TRUE;
}

bool	AgcmRender::RemoveAtomicFromWorld( RpAtomic* pAtomic )
{
	if( !pAtomic )		return FALSE;
	
	PROFILE("RemoveAtomicFromWorld");

	ReturnDefOriginalRenderCB(pAtomic,NULL);
	
	if ( !RpAtomicGetClump( pAtomic ) && pAtomic->stType && pAtomic->stType->pUpdateList )
	{
		ListUpdateCallback*		pUpdateCallbackList		=	static_cast< ListUpdateCallback* >(pAtomic->stType->pUpdateList);
		ListUpdateCallbackIter	Iter					=	pUpdateCallbackList->begin();

		for( ; Iter != pUpdateCallbackList->end() ; )
		{
			UpdateCallbackList*		pUpdateCB		=	(*Iter);

			if(pUpdateCB->DestructCB)
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );

			DEF_SAFEDELETE( pUpdateCB );
			pUpdateCallbackList->erase( Iter++ );
		}

		DEF_SAFEDELETE( pAtomic->stType->pUpdateList );
	}
	
	MyWorldRemoveAtomic(pAtomic);
	pAtomic->ulFlag = pAtomic->ulFlag & (~RWFLAG_RENDER_ADD);

	return TRUE;
}

RpAtomic*	AgcmRender::ReturnDefOriginalRenderCB(RpAtomic *atomic, void *data )
{
	RpLODAtomicUnHookRender(atomic);
	RpAtomicCallBackRender	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;
	RpAtomicSetRenderCallBack(atomic, pFunc);
	RpLODAtomicHookRender(atomic);
	
	return atomic;
}

void AgcmRender::CheckAlphaAtomicFromClump( RpClump *clump, enumAlphaOption eAlphaOption , BOOL bBillboard )
{
	//Cache a list of atomics that all have alpha components...
	switch( eAlphaOption )
	{
	case CHECK_ALPHA:		RpClumpForAllAtomics( clump, AtomicSetAlpha, (void*)&bBillboard );		break;
	case ONLY_ALPHA:		RpClumpForAllAtomics( clump, AtomicSetAlpha, (void*)&bBillboard );		break;
	case ONLY_NONALPHA:		RpClumpForAllAtomics( clump, AtomicSetNonAlpha, (void*)&bBillboard );	break;
	case ONLY_AMBIENT:		RpClumpForAllAtomics( clump, AtomicSetAmbient, (void*)&bBillboard );	break;
	case ONLY_ALPHAFUNC:	RpClumpForAllAtomics( clump, AtomicSetAlphaFunc, (void*)&bBillboard );	break;
	case ONLY_LASTZWRITE:	RpClumpForAllAtomics( clump, AtomicSetLastZWrite, (void*)&bBillboard );	break;
	}
}

void AgcmRender::CheckAlphaAtomic( RpAtomic *atomic, enumAlphaOption eAlphaOption , BOOL bBillboard)
{
	switch( eAlphaOption )
	{
	case CHECK_ALPHA:		AtomicSetAlpha( atomic, (void*)&bBillboard );		break;
	case ONLY_ALPHA:		AtomicSetAlpha( atomic, (void*)&bBillboard );		break;
	case ONLY_NONALPHA:		AtomicSetNonAlpha( atomic, (void*)&bBillboard );	break;
	case ONLY_AMBIENT:		AtomicSetAmbient( atomic, (void*)&bBillboard );		break;
	case ONLY_ALPHAFUNC:	AtomicSetAlphaFunc( atomic, (void*)&bBillboard );	break;
	case ONLY_LASTZWRITE:	AtomicSetLastZWrite( atomic, (void*)&bBillboard );	break;
	}
}

// ATOMIC SET
RpAtomic*	AgcmRender::AtomicSetAlpha( RpAtomic *atomic, void *data )
{
	if( m_pThisAgcmRender->IsSkinedPipe( atomic ) )
	{
		m_pThisAgcmRender->InitAtomicSetRenderCallBack(atomic,RenderCallbackForSkinPipeAlpha,R_SKIN,R_BLENDSRCAINVA_ADD,FALSE);
	}
	else
	{
		//Defer and sort the rendering of transparent atomics...
		BOOL bBill = *((BOOL*)data);
		if( m_pThisAgcmRender->m_pApmOcTree->m_bOcTreeEnable )
			m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForAlphaAtomicBill2 : RenderCallbackForAlphaAtomic2, R_ALPHA, R_BLENDSRCAINVA_ADD, bBill );
		else
			m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForAlphaAtomicBill : RenderCallbackForAlphaAtomic, R_ALPHA, R_BLENDSRCAINVA_ADD, bBill );
	}

	return atomic;
}
	
RpAtomic*	AgcmRender::AtomicSetNonAlpha( RpAtomic *atomic, void *data )	
{
	if( m_pThisAgcmRender->IsSkinedPipe( atomic ))
	{
		m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, RenderCallbackForSkinPipe, R_SKIN, R_BLENDSRCAINVA_ADD, FALSE );
	}
	else
	{
		BOOL bBill = *((BOOL*)data);
		m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForNonAlphaAtomicBill : RenderCallbackForNonAlphaAtomic, R_NONALPHA, R_BLENDSRCAINVA_ADD, bBill );
	}
	
	return atomic;
}

RpAtomic*	AgcmRender::AtomicSetAmbient( RpAtomic *atomic, void *data )
{
	if( m_pThisAgcmRender->IsSkinedPipe( atomic ) )
	{
		m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, RenderCallbackForSkinPipe, R_SKIN, R_BLENDSRCAINVA_ADD, FALSE );
	}
	else
	{
		BOOL bBill = *((BOOL*)data);
		m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForAmbientAtomicBill : RenderCallbackForAmbientAtomic, R_AMBIENT, R_BLENDSRCAINVA_ADD, bBill );
	}
		
	return atomic;
}

RpAtomic*	AgcmRender::AtomicSetAlphaFunc( RpAtomic *atomic, void *data )
{
	if( m_pThisAgcmRender->IsSkinedPipe( atomic ) )
	{
		m_pThisAgcmRender->InitAtomicSetRenderCallBack(atomic,RenderCallbackForSkinPipeAlphaFunc,R_SKIN,R_BLENDSRCAINVA_ADD,FALSE);
	}
	else
	{
		BOOL bBill = *((BOOL*)data);
		m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, bBill ? RenderCallbackForAlphaFuncAtomicBill : RenderCallbackForAlphaFuncAtomic, R_ALPHAFUNC, R_BLENDSRCAINVA_ADD, bBill );
	}
		
	return atomic;
}

RpAtomic*	AgcmRender::AtomicSetLastZWrite( RpAtomic*	atomic, void*	data)
{
	m_pThisAgcmRender->InitAtomicSetRenderCallBack( atomic, RenderCallbackForLastZWrite, R_LASTZWRITE, R_BLENDSRCAINVA_ADD, FALSE );
	return atomic;
}

RwReal AgcmRender::GetCameraDistance(RpAtomic *atomic)
{
    //Atomic's bounding-sphere world-space position...
    RwSphere* atomicBSphere = RpAtomicGetBoundingSphere(atomic);
	
	RwV3d atomicPos;
    RwV3dTransformPoints( &atomicPos, &atomicBSphere->center, 1, RwFrameGetLTM(RpAtomicGetFrame(atomic)));

    //...camera position...
    RwFrame* frame = RwCameraGetFrame( m_pCamera );
    RwV3d* camPos = RwMatrixGetPos(RwFrameGetLTM(frame));

    //...vector from camera to atomic...
	RwV3d	temp;
    RwV3dSub(&temp, &atomicPos, camPos);

    return RwV3dDotProduct( &temp, &temp );
}

void AgcmRender::ClearLists()
{
	m_listInvisible.clear();

	for( int i=0; i<R_HASH_SIZE; ++i )
	{
		m_listNonAlpha.hash_table[i]	= NULL;
		m_listAmbient.hash_table[i]		= NULL;	
		m_listAlphaFunc.hash_table[i]	= NULL;
		m_listSkinning.hash_table[i]	= NULL;
		m_listSkinningAlphaFunc.hash_table[i] = NULL;
	}
	
	m_BlendAtomics.Clear();
	m_mapAlpha.clear();

	for( int i=0; i<R_NOT_SORT_BLEND_MODE; ++i )
		for( int j=0; j<3; ++j )
			m_listBlendNSort.hash_table[i][j] = NULL;

	for(int i=0; i<R_BELND_MODE_NUM; ++i )
		m_listSkinningBlend[i] = NULL;

	m_listSkinningAlpha		= NULL;
	
	m_listUVAnim			= NULL;
	m_listUVAnimSkin		= NULL;

	m_listShadow2Terrain	= NULL;
	m_pShadow2Terrain_Own	= NULL;
	m_listTerrainNoMTexture	= NULL;
	m_listLastZWrite		= NULL;

	if( m_pApmOcTree->m_bOcTreeEnable )
	{
		while( !m_priorityQueuePreRender.empty() )
			m_priorityQueuePreRender.pop();
	}

	for(int i=0; i<10; ++i)
	{
		m_pTableLight[i]	= NULL;
		m_iTableLightNum[i]	= 0;
	}

	if ( AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect() )
		AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->ClearExclusiveEffectInfoPerFrame();

}

void	AgcmRender::DefaultAtomicListTestRender(RpAtomic*	pList)
{
	PROFILE("DefaultAtomicListTestRender");
	RpAtomic*	cur_atomic = pList;

	if(cur_atomic)
	{
		do
		{
			if( cur_atomic->geometry )
				RpAtomicRender(cur_atomic);

			cur_atomic = cur_atomic->next;
		}
		while(cur_atomic && (cur_atomic != pList));
	}
}

void	AgcmRender::ImmediateAtomicListTestRender(RpAtomic*	pList)
{
	RpAtomic*	cur_atomic = pList;

	if(cur_atomic)
	{
		do
		{
			AgcmRender::OriginalDefaultAtomicRenderCallback(cur_atomic);
			cur_atomic = cur_atomic->next;
		}
		while(cur_atomic && (cur_atomic != pList));
	}
}

void	AgcmRender::DefaultAtomicRender(RpAtomic*		atomic)
{
	RpAtomicRender(atomic);
}

void	AgcmRender::ImmediateAtomicRender(RpAtomic*		atomic)
{
	RpGeometry*	pGeom = RpAtomicGetGeometry(atomic);
	if( pGeom )
		AgcmRender::OriginalDefaultAtomicRenderCallback(atomic);
}

void	AgcmRender::DefaultTerrainRender(RpAtomic*	atomic,INT32 SectorDist)
{
	Shadow2TerrainList*		nw_node = (Shadow2TerrainList*)AcuFrameMemory::AllocFrameMemory(sizeof(Shadow2TerrainList));

	ASSERT( nw_node && "FrameMemory Allocation Error!!!");
	if (nw_node)
	{
		nw_node->atomic = atomic;

		// 2005.4.11 gemani .. 지형이 오브젝트로 Add된 경우
		if(atomic->stType && atomic->stType->eType & ACUOBJECT_TYPE_NO_MTEXTURE)
		{
			nw_node->next = m_pThisAgcmRender->m_listTerrainNoMTexture;
			m_pThisAgcmRender->m_listTerrainNoMTexture = nw_node;
		}
		else
		{
			if(SectorDist == 0)
			{
				m_pThisAgcmRender->m_pShadow2Terrain_Own = nw_node;
			}
			else
			{
				nw_node->next = m_pThisAgcmRender->m_listShadow2Terrain;
				m_pThisAgcmRender->m_listShadow2Terrain = nw_node;
			}
		
			nw_node->bShadow = FALSE;

			if(m_pThisAgcmRender->m_bVertexShaderEnable)
			{
				INT32	range = m_pThisAgcmRender->m_iShadow2Range;
				if(m_pThisAgcmRender->m_bDrawShadow2 && SectorDist < range)
				{
					nw_node->bShadow = TRUE;
				}

				nw_node->sectordist = SectorDist;
			}
		}
	}
}

// shadow2는 안그린다.
void	AgcmRender::ImmediateTerrainRender(RpAtomic*	atomic,INT32 SectorDist)
{
	RpAtomicCallBackRender	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;
	atomic->stRenderInfo.shaderUseType	= 0;
	atomic->stRenderInfo.beforeLODLevel	= SectorDist;
	
	RpMTextureDrawStart();
	if( m_pThisAgcmRender->m_bVertexShaderEnable )
		m_pThisAgcmRender->TerrainDataSet();
	pFunc(atomic);
	RpMTextureDrawEnd(TRUE);
}

void AgcmRender::RenderWorldAtomics(RwCamera*	pCheckCamera,AgcmRenderTestRenderAtomicList		pRenderFunc)
{
	// RenderWorld List 검사
	WorldAddClumpGroup*		cur_wc;
	RwFrame*				frame;
	RwMatrix*				matrix;
	RpAtomic*				test_atomic;
	RwFrustumTestResult		res;

	cur_wc = m_listWorldClumps;
	while(cur_wc)
	{
		if(cur_wc->pClump)
		{
			ASSERT( !cur_wc->Atomiclist );

			frame  = RpClumpGetFrame(cur_wc->pClump);
			matrix = RwFrameGetLTM(frame);
			
			RwV3dTransformPoint( &cur_wc->WorldBS.center, &cur_wc->BS.center, matrix );
			res = RwCameraFrustumTestSphere( pCheckCamera, &cur_wc->WorldBS );
			if( res != rwSPHEREOUTSIDE )
				pRenderFunc(cur_wc->pClump->atomicList); 
		}
		else
		{
			ASSERT( cur_wc->Atomiclist );

			test_atomic = cur_wc->Atomiclist;

			frame  = RpAtomicGetFrame(test_atomic);
			if ( frame )
			{				
				if ( FindFromRwFrameMapTable(frame) )
				{
					matrix = RwFrameGetLTM(frame);

					RwV3dTransformPoint(&cur_wc->WorldBS.center,&cur_wc->BS.center,matrix);
					res = RwCameraFrustumTestSphere( pCheckCamera, &cur_wc->WorldBS );
					if( res != rwSPHEREOUTSIDE )
						pRenderFunc(cur_wc->Atomiclist);
				}
			}
		}

		cur_wc = cur_wc->next;
	}
}

// 임의의 camera로 지평선 체크후 그린다..
void	AgcmRender::RenderMyCamera( RwCamera* pCheckCamera, BOOL bDrawWorldAdded, AgcmRenderTestRenderTerrain TF, AgcmRenderTestRenderAtomicList ALF, AgcmRenderTestRenderAtomic AF )
{
	if( m_pApmOcTree->m_bOcTreeEnable)
	{
		if(m_pApmOcTree && m_pApmOcTree->m_bOcTreeEnable)
		{
			while(!m_priorityQueuePreRender.empty())
				m_priorityQueuePreRender.pop();
		}

		{
		PROFILE("OcTreeModeRender");
		OcTreeModeRender(pCheckCamera,TF,bDrawWorldAdded);
		}
		
		{
		PROFILE("UpdateOBufferAndTest");
		UpdateOBufferAndDrawTest(ALF,FALSE);		// 지평선 체크후 그리자 ^^
		}

	}
	else
	{
		if( bDrawWorldAdded )
			RenderWorldAtomics( pCheckCamera, ALF );

		if( m_bUseCullMode )
			CullModeWorldRender( pCheckCamera, AF, TF );
	}
}

//RENDER FUNCTIONS #$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$#$
void AgcmRender::ProcessBillboardDraw(RpAtomic*	atomic,int billtype)
{
	PROFILE("ProcessBillboardDraw");

	RpAtomicCallBackRender	pFunc;
	
	RwV3d BoardPos;
	RwFrame*	pBoardFrame;
	RwMatrix* pBoardMatrix;

	if(billtype == 1)	// 빌보드
	{
		RwFrame* CameraFrame = RwCameraGetFrame( m_pCamera );
		
		pBoardFrame = RpAtomicGetFrame(atomic);
		pBoardMatrix = RwFrameGetMatrix( pBoardFrame );
		BoardPos = pBoardMatrix->pos;

		RwMatrix* CameraMatrix = RwFrameGetLTM( CameraFrame );
		RwFrameTransform( pBoardFrame, CameraMatrix,  rwCOMBINEPOSTCONCAT);

		pBoardMatrix->pos = BoardPos;
	}
	else if(billtype == 2)		// Y축 빌보드
	{
		pBoardFrame = RpAtomicGetFrame(atomic);
		RwFrame* pParentFrame = RwFrameGetParent( pBoardFrame );
		pBoardMatrix = RwFrameGetMatrix( pBoardFrame );
		
		BoardPos = pBoardMatrix->pos;
	
		if ( pParentFrame )
		{
			RwMatrix* pParentMatrix = RwFrameGetMatrix( pParentFrame );
			RwMatrix matInvParent;
			RwMatrixInvert( &matInvParent, pParentMatrix );

			RwV3d   vScale;
			vScale.y = 1.0f / matInvParent.up.y;
			vScale.x = vScale.y;
			vScale.z = vScale.y;

			RwMatrixScale( pBoardMatrix, &vScale, rwCOMBINEREPLACE);
			RwMatrixRotate( pBoardMatrix , &m_vYAxis , m_fRotAngle , rwCOMBINEPOSTCONCAT); 
			RwMatrixTransform( pBoardMatrix, &matInvParent, rwCOMBINEPOSTCONCAT );
			RwFrameUpdateObjects( pBoardFrame );
		}
		else 
		{
			RwFrameRotate( pBoardFrame , &m_vYAxis , m_fRotAngle , rwCOMBINEPRECONCAT);
		}

		pBoardMatrix->pos = BoardPos;
	}

	RwV3d		vert2Eye;
	
	RwMatrix*	bLTM = RwFrameGetLTM(pBoardFrame);

	RwV3d* bPos = RwMatrixGetPos(bLTM);
	RwV3dSub( &vert2Eye, m_pCameraPos, bPos );
	RwV3dNormalize( &vert2Eye, &vert2Eye );

	RwV3d* pLightAt = RwMatrixGetAt( RwFrameGetLTM( RpLightGetFrame( m_pDirectLight ) ) );
	RwReal	fDis = RwV3dDotProduct( &vert2Eye, pLightAt );
	if( 0.0f >= -fDis )	fDis = 0.0f;
	else				fDis = -fDis;

	RwRGBAReal final;
	final.red	= min( m_colorOriginalDirect.red * fDis * 1.1f + m_colorOriginalAmbient.red, 1.f );
	final.green	= min( m_colorOriginalDirect.green * fDis * 1.1f + m_colorOriginalAmbient.green, 1.f );
	final.blue	= min( m_colorOriginalDirect.blue * fDis * 1.1f + m_colorOriginalAmbient.blue, 1.f );
	final.alpha	= 1.0f;

	//Ambient Light 바꾸기 
	if ( m_pAmbientLight && m_pDirectLight )
	{
		RpLightSetColor( m_pDirectLight, &m_colorBlack );
		RpLightSetColor( m_pAmbientLight, &final );
	}

	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;
	pFunc( atomic );

	if ( m_pAmbientLight && m_pDirectLight)
	{
		RpLightSetColor( m_pAmbientLight, &m_colorOriginalAmbient );
		RpLightSetColor( m_pDirectLight, &m_colorOriginalDirect );
	}
}

#include "AcuObjecWire.h"
void AgcmRender::RenderAtomic(RpAtomic*	atomic,BOOL	 bBill)
{
	RpAtomicCallBackRender	pFunc;

	RpGeometry*	pGeom = RpAtomicGetGeometry(atomic);
	if( !pGeom )								return;
	if( RpGeometryGetNumTriangles(pGeom) < 2 )	return;

	//@{ Jaewon 20050913
	// Adjust the lighting according to the lighting type of the clump
	// (no more render callbacks & render lists --;).
	RpClump* pClump = RpAtomicGetClump(atomic);

	BOOL	bNeedLightRestore = FALSE;
	RwRGBAReal ambientLightColor0;
	RwRGBAReal directLightColor0;

	if( m_pAmbientLight && m_pDirectLight && pClump )
	{
		ambientLightColor0 = m_pAmbientLight->color;
		directLightColor0 = m_pDirectLight->color;

		RwRGBAReal* pAmbientColor		= &m_pAmbientLight->color;
		RwRGBAReal* pDirectionalColor	= &m_pDirectLight->color;

		switch( pClump->stUserData.customLightStatus )
		{
		case LIGHT_LOGIN_SELECT:
			pDirectionalColor = &m_lightColorAtLoginSelect[1];
			bNeedLightRestore = TRUE;
			break;
		case LIGHT_QUEST_OBJECT:
			pAmbientColor = &m_lightColorOfQuestObject[0];
			bNeedLightRestore = TRUE;
			break;
		case LIGHT_IN_SHADOW:
			pDirectionalColor	= &m_lightColorInShadow[1];
			pAmbientColor		= &m_lightColorInShadow[0];
			bNeedLightRestore = TRUE;
			break;
		case LIGHT_STATUS_STUN:
			pAmbientColor		= &m_lighterColorBySpecialStatus[0];
			bNeedLightRestore = TRUE;
			break;
		case LIGHT_QUEST_FREEZE:
			pAmbientColor		= &m_lighterColorBySpecialStatus[1];
			bNeedLightRestore = TRUE;
			break;
		case LIGHT_QUEST_POISON:
			pAmbientColor		= &m_lighterColorBySpecialStatus[2];
			bNeedLightRestore = TRUE;
			break;
		}

		RwRGBAReal abientResult;

		AgpdCharacter * pcsAgpdCharacter	= ( AgpdCharacter * ) pClump->stType.pObject;
		if( pClump && pClump->object.type == 2 && pClump->stType.pObject && pcsAgpdCharacter->m_eType == APBASE_TYPE_CHARACTER )
		{
			AuCharacterLightInfo * pLightInfo;

			#ifdef USE_MFC
			if( pcsAgpdCharacter->m_eType == APBASE_TYPE_ITEM )
			{
				AgpdItem * pcsAgpdItem = ( AgpdItem * ) pcsAgpdCharacter;
				pLightInfo = &( ( AgpdItemTemplateEquip * )pcsAgpdItem->m_pcsItemTemplate )->m_lightInfo;
			}
			else
			#endif
			{
				AgcdCharacter * pcsAgcdCharacter	= AgcmCharacter::m_pThisAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
				pLightInfo = pcsAgcdCharacter->GetLightInfo( atomic->iPartID );
			}

			if( pLightInfo )
			{
				switch( pLightInfo->GetType() )
				{
				case AuCharacterLightInfo::NO_VARIATION		:
					break;
				case AuCharacterLightInfo::AMBIENT_PLUS		:
					{
						bNeedLightRestore = TRUE;

						UINT32	uUpdateGap	= pLightInfo->uChangeGap;
						UINT32	uRate = g_pEngine->GetTickCount() % ( uUpdateGap * 2 );
						float	fRate;

						if( uRate < uUpdateGap )
						{
							fRate = ( float ) uRate / ( float ) uUpdateGap;
						}
						else
						{
							fRate = ( float ) ( uUpdateGap * 2 - uRate ) / ( float ) uUpdateGap;
						}

						float	fRate_inv = 1.0f - fRate;

						abientResult.red	= pAmbientColor->red	+ (pLightInfo->r1	* fRate + pLightInfo->r2	* fRate_inv ) * 0.3f;
						abientResult.green	= pAmbientColor->green	+ (pLightInfo->g1	* fRate + pLightInfo->g2	* fRate_inv ) * 0.3f;
						abientResult.blue	= pAmbientColor->blue	+ (pLightInfo->b1	* fRate + pLightInfo->b2	* fRate_inv ) * 0.3f;
						abientResult.alpha	= 1.0f;

						pAmbientColor = &abientResult;
					}
					break;
				case AuCharacterLightInfo::AMBIENT_MINUS		:
					{
						bNeedLightRestore = TRUE;

						UINT32	uUpdateGap	= pLightInfo->uChangeGap ? pLightInfo->uChangeGap : 1000;
						UINT32	uRate = g_pEngine->GetTickCount() % ( uUpdateGap * 2 );
						float	fRate;

						if( uRate < uUpdateGap )
						{
							fRate = ( float ) uRate / ( float ) uUpdateGap;
						}
						else
						{
							fRate = ( float ) ( uUpdateGap * 2 - uRate ) / ( float ) uUpdateGap;
						}

						//float	fRate = ( float ) ( g_pEngine->GetTickCount() % uUpdateGap ) / ( float ) uUpdateGap;
						float	fRate_inv = 1.0f - fRate;

						abientResult.red	= pAmbientColor->red	- (pLightInfo->r1	* fRate + pLightInfo->r2	* fRate_inv ) * 0.3f;
						abientResult.green	= pAmbientColor->green	- (pLightInfo->g1	* fRate + pLightInfo->g2	* fRate_inv ) * 0.3f;
						abientResult.blue	= pAmbientColor->blue	- (pLightInfo->b1	* fRate + pLightInfo->b2	* fRate_inv ) * 0.3f;
						abientResult.alpha	= 1.0f;

						pAmbientColor = &abientResult;
					}
					break;
				case AuCharacterLightInfo::DIRECTIONA_VARIATION	:
					break;
				}
			}
		}

		if(	pAmbientColor != &m_pAmbientLight->color )
			RpLightSetColor(m_pAmbientLight, pAmbientColor);
		if(	pDirectionalColor != &m_pDirectLight->color )
			RpLightSetColor(m_pDirectLight, pDirectionalColor);
	}

	if (bBill)
	{
		ProcessBillboardDraw(atomic,bBill);
	}
	else
	{
		pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;
		if ( pFunc && atomic->geometry )
			pFunc(atomic);
	}

	if( bNeedLightRestore )
	{
		RpLightSetColor(m_pAmbientLight, &ambientLightColor0);
		RpLightSetColor(m_pDirectLight, &directLightColor0);
	}

#ifdef USE_MFC
	// 마고자 (2005-05-16 오후 5:53:38) : 
	// 퍼포먼스때문에 따로 옵션처리.
	if( m_bUseAtomicCustomRender )
	{
		AgcdType*	lpType = AcuObject::GetAtomicTypeStruct(atomic);
		if( lpType )
		{
			if( ACUOBJECT_TYPE_OBJECT == AcuObject::GetType( lpType->eType ) )
			{
				AcuObjecWire::bGetInst().bOnOffPicking(true);
				RpClump* clump = RpAtomicGetClump( atomic );
				clump ? AcuObjecWire::bGetInst().bRenderClump( clump ) : AcuObjecWire::bGetInst().bRenderAtomic( atomic );
			}
		}
	}
#endif //USE_MFC
	
	AgcmObject*	pAgcmObject = static_cast<AgcmObject*>(GetModule( "AgcmObject" ));
	if(pAgcmObject && pAgcmObject->IsShowPickingInfo())
	{
		//AcuObjecWire::bGetInst().bOnOffPicking( true );
		AgcdType* lpType = AcuObject::GetAtomicTypeStruct(atomic);
		if( lpType )
		{
			if( ACUOBJECT_TYPE_OBJECT == AcuObject::GetType( lpType->eType ) )
			{
				RpClump* clump = RpAtomicGetClump( atomic );
				clump ? AcuObjecWire::bGetInst().bRenderClumpOnce( clump ) : AcuObjecWire::bGetInst().bRenderAtomic( atomic );
			}
		}
	}
}

void AgcmRender::RenderSortedBlendAtomics()
{
	bool bD3D9Set = _RenderBlendAtomic_BillBoard();
	_RenderBlendAtomic_LastBlended();
	bD3D9Set = _RenderBlendAtomic();

	if( bD3D9Set )
	{
		_RestoreRenderStateForBlendAtomic();
	}
}

void AgcmRender::RenderNotSortedBlendAtomics()
{
	bool	bD3D9Set = false;

	TextureSortAtomic*		cur_node;
	TextureSortAtomic*		cur_siblings;
	int j;

	if(m_iNumBlendNSort[0] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );
		bD3D9Set = true;
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[0][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}

				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	if(m_iNumBlendNSort[1] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
		bD3D9Set = true;

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[1][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}
				
				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	if(m_iNumBlendNSort[2] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
		bD3D9Set = true;

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[2][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}
			
				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	if(m_iNumBlendNSort[3] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );
		bD3D9Set = true;

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[3][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}
								
				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	if(m_iNumBlendNSort[4] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
		bD3D9Set = true;

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[4][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}

				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	if(m_iNumBlendNSort[5] > 0)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
		bD3D9Set = true;

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		for(j=0;j<3;++j)
		{
			cur_node = m_listBlendNSort.hash_table[5][j];
			while(cur_node)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
					cur_siblings = cur_siblings->sibling;
				}
				
				RenderAtomic(cur_node->atomic,cur_node->billboard);
				cur_node = cur_node->next;
			}
		}
	}

	for(int i=0;i<R_NOT_SORT_BLEND_MODE;++i)
	{
		m_iNumBlendNSort[i] = 0;
	}

	if(bD3D9Set == true)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );	// default복귀
	}
}

void	AgcmRender::RenderNonAlphaAtomics()
{
	TextureSortAtomic*		cur_node;
	TextureSortAtomic*		cur_siblings;
	
	for(INT32 i = 0; i<R_HASH_SIZE ; ++i)
	{
		cur_node = m_listNonAlpha.hash_table[i];
		while(cur_node)
		{
			cur_siblings = cur_node->sibling;
			while(cur_siblings)
			{
				RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
				cur_siblings = cur_siblings->sibling;
			}

			RenderAtomic(cur_node->atomic,cur_node->billboard);
			cur_node = cur_node->next;
		}
	}
}

void	AgcmRender::RenderAlphaAtomics()
{
	mmapAlphaAtomicIter	Iter	=	m_mapAlpha.begin();
	for( ; Iter != m_mapAlpha.end() ; ++Iter )
	{
		AlphaAtomic*	pAlphaAtomic	=	&Iter->second;
		if( pAlphaAtomic )
		{
			RenderAtomic( pAlphaAtomic->atomic , pAlphaAtomic->billboard );
		}
	}
}

void	AgcmRender::RenderAmbientAtomics()
{
	// Ambient의 용도가 ambient light 안먹는걸로 변경 (2004.06.14)
	RpLightSetColor(m_pAmbientLight, &m_colorBlack);

	TextureSortAtomic*		cur_node;
	TextureSortAtomic*		cur_siblings;
	
	for(INT32 i = 0; i<R_HASH_SIZE ; ++i)
	{
		cur_node = m_listAmbient.hash_table[i];
		while(cur_node)
		{
			cur_siblings = cur_node->sibling;
			while(cur_siblings)
			{
				RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
				cur_siblings = cur_siblings->sibling;
			}

			RenderAtomic(cur_node->atomic,cur_node->billboard);
			cur_node = cur_node->next;
		}
	}
	
	if ( m_pAmbientLight)
		RpLightSetColor( m_pAmbientLight, &m_colorOriginalAmbient );
	
	return;
}

void	AgcmRender::RenderAlphaFuncAtomics()
{
	TextureSortAtomic*		cur_node;
	TextureSortAtomic*		cur_siblings;
	
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 220  );
	
	for(INT32 i = 0; i<R_HASH_SIZE ; ++i)
	{
		cur_node = m_listAlphaFunc.hash_table[i];
		while(cur_node)
		{
			cur_siblings = cur_node->sibling;
			while(cur_siblings)
			{
				RenderAtomic(cur_siblings->atomic,cur_siblings->billboard);
				cur_siblings = cur_siblings->sibling;
			}
			
			RenderAtomic(cur_node->atomic,cur_node->billboard);
			cur_node = cur_node->next;
		}
	}

	RwRenderStateSet( rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0 );
		
	return;
}

// Effect도 skin을 가질수 있으므로 renderstate를 지원한다. ^^ (2004.07.01)
void	AgcmRender::RenderSkinPipeBlendAtomics()
{
	SkinningAtomic*			cur_node = m_listSkinningBlend[R_BLENDSRCAINVA_ADD];

	bool	bD3D9Set = false;
	
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCAINVA_REVSUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCAINVA_SUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	for(int i=3;i<6;++i)
	{
		cur_node = m_listSkinningBlend[i];

		if(cur_node)
		{
			bD3D9Set = true;
			RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD      );

			while(cur_node)
			{
				RenderAtomic(cur_node->atomic,FALSE);
				cur_node = cur_node->next;
			}
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCCINVC_ADD];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );

		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCCINVC_REVSUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCCINVC_SUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR   );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLEND11_ADD];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLEND11_REVSUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLEND11_SUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE   );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCA1_ADD];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCA1_REVSUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	cur_node = m_listSkinningBlend[R_BLENDSRCA1_SUB];
	if(cur_node)
	{
		bD3D9Set = true;
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
				
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA    );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE    );

		while(cur_node)
		{
			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	if(bD3D9Set == true)
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );	// default복귀
		
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		
		RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}
}

void	AgcmRender::RenderSkinPipeAtomics()
{
	SkinningTSortAtomic*		cur_node;
	SkinningTSortAtomic*		cur_siblings;
	
	for(INT32 i = 0; i<R_HASH_SIZE ; ++i)
	{
		cur_node = m_listSkinning.hash_table[i];
		while(cur_node)
		{
			if(cur_node->sibling)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,FALSE);
					cur_siblings = cur_siblings->sibling;
				}
			}

			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}
}

void	AgcmRender::RenderSkinPipeAlphaAtomics()
{
	SkinningAtomic*		cur_node;
	cur_node = m_listSkinningAlpha;
	while(cur_node)
	{
		RenderAtomic(cur_node->atomic,FALSE);
		cur_node = cur_node->next;
	}
}

void	AgcmRender::RenderSkinPipeAlphaFuncAtomics()
{
	SkinningTSortAtomic*		cur_node;
	SkinningTSortAtomic*		cur_siblings;

	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 220  );

	for(INT32 i = 0; i<R_HASH_SIZE ; ++i)
	{
		cur_node = m_listSkinningAlphaFunc.hash_table[i];
		while(cur_node)
		{
			if(cur_node->sibling)
			{
				cur_siblings = cur_node->sibling;
				while(cur_siblings)
				{
					RenderAtomic(cur_siblings->atomic,FALSE);
					cur_siblings = cur_siblings->sibling;
				}
			}

			RenderAtomic(cur_node->atomic,FALSE);
			cur_node = cur_node->next;
		}
	}

	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF   , (void*) 0  );
}

//@{ Jaewon 20050201
// RpSkin definitions for 'vertexMaps' access
typedef void * SkinUnaligned;

typedef struct SkinPlatformData         SkinPlatformData;
typedef struct SkinAtomicPlatformData   SkinAtomicPlatformData;
typedef struct SkinGlobalPlatform       SkinGlobalPlatform;

struct SkinPlatformData
{
    RwUInt32 maxNumBones;
    RwUInt32 useVertexShader;
};

typedef struct SkinSplitData SkinSplitData;
struct SkinSplitData
{
    RwUInt32            boneLimit;
    RwUInt32            numMeshes;
    RwUInt32            numRLE;
    RwUInt8             *matrixRemapIndices;
    RwUInt8             *meshRLECount;
    RwUInt8             *meshRLE;
};

typedef struct SkinBoneData SkinBoneData;
struct SkinBoneData
{
    RwUInt32            numBones;
    RwUInt32            numUsedBones;
    RwUInt8             *usedBoneList;
    RwMatrix            *invBoneToSkinMat;
};

typedef struct SkinVertexMaps SkinVertexMaps;
struct SkinVertexMaps
{
    RwUInt32            maxWeights;
    RwUInt32            *matrixIndices;
    RwMatrixWeights     *matrixWeights;
};

struct RpSkin
{
    SkinBoneData        boneData;
    SkinVertexMaps      vertexMaps;
    SkinPlatformData    platformData;
    SkinSplitData       skinSplitData;
    SkinUnaligned       *unaligned;
};
//@} Jaewon

void	AgcmRender::RenderUVAnimAtomics(UVAnimAtomic*	start_node)
{
	UVAnimAtomic* cur_node = start_node;
	while(cur_node)
	{
		RpGeometry* pGeometry = RpAtomicGetGeometry( cur_node->atomic );
		if( !pGeometry )
		{
			cur_node = cur_node->next;
			continue;
		}
		
		RpMaterial* pMaterial = RpGeometryGetMaterial(pGeometry,0);
		
		bool setok = false;
		RpMaterial** pMaterials = (RpMaterial**) cur_node->atomic->stRenderInfo.pData1;
		DxEffect* pFXBackups[10];
		int	iEnd = min( RpGeometryGetNumMaterials(pGeometry), 10 );

		if( pMaterials )
		{
			if( !m_bDisableMatD3DFx )
			{
				RpSkin *pSkin = RpSkinGeometryGetSkin(pGeometry);
				ASSERT(pSkin);
				ASSERT(pSkin->vertexMaps.maxWeights <= 4);

				if( pSkin )
				{
					for(int i=0; i<iEnd; ++i)
					{
						RpMaterialD3DFxExt *matData = MATERIAL_GET_FX_DATA(RpGeometryGetMaterial(pGeometry,i));
						pFXBackups[i] = matData->effect;
						
						if( pFXBackups[i] )
						{
							matData->effect = MATERIAL_GET_FX_DATA(pMaterials[pSkin->vertexMaps.maxWeights-1])->effect;
							setok = true;
						}
					}
				}
			}
		}

		RenderAtomic( cur_node->atomic, FALSE );
		
		if( setok )
		{
			for(int i=0; i<iEnd; ++i)
				MATERIAL_GET_FX_DATA( pGeometry->matList.materials[i] )->effect = pFXBackups[i];
		}

		cur_node = cur_node->next;
	}
}

void	AgcmRender::RenderLastZWriteAtomics()
{
	SimpleAtomic*		cur_node = m_listLastZWrite;
	while(cur_node)
	{
		RenderAtomic(cur_node->atomic,FALSE);
		cur_node = cur_node->next;
	}
}

// ############################################################################################### Render Funcs End

void	AgcmRender::TerrainDataSet()
{
	D3DXVECTOR4	ambient_color(m_pAmbientLight->color.red,m_pAmbientLight->color.green, m_pAmbientLight->color.blue,m_pAmbientLight->color.alpha);
	D3DXVECTOR4	direct_color( m_pDirectLight->color.red,m_pDirectLight->color.green, m_pDirectLight->color.blue,m_pDirectLight->color.alpha);
										
	D3DMATRIX	composedMatrix;

	RwV3d* pAt = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(m_pDirectLight)));
	D3DXVECTOR4 direct_dir( pAt->x, pAt->y, pAt->z, 0.f );

	RwMatrix identity;
	RwMatrixSetIdentity(&identity);
	
	_rwD3D9VSSetActiveWorldMatrix(&identity);
	_rwD3D9VSGetComposedTransformMatrix((void *)&composedMatrix);

	RwD3D9SetVertexShaderConstant(0, (void *)&composedMatrix, 4);

	RwD3D9SetVertexShaderConstant(4, (void *)&direct_dir, 1);

	RwD3D9SetVertexShaderConstant(5, (void *)&direct_color, 1);
	RwD3D9SetVertexShaderConstant(6, (void *)&ambient_color, 1);

	RwD3D9SetVertexShaderConstant(7, (void *)&m_fFogConstant, 1);
	
	D3DMATRIX				WorldTransposeMatrix;
	
	WorldTransposeMatrix.m[0][0] = identity.right.x;
	WorldTransposeMatrix.m[0][1] = identity.up.x;
	WorldTransposeMatrix.m[0][2] = identity.at.x;
	WorldTransposeMatrix.m[0][3] = identity.pos.x;

	WorldTransposeMatrix.m[1][0] = identity.right.y;
	WorldTransposeMatrix.m[1][1] = identity.up.y;
	WorldTransposeMatrix.m[1][2] = identity.at.y;
	WorldTransposeMatrix.m[1][3] = identity.pos.y;

	WorldTransposeMatrix.m[2][0] = identity.right.z;
	WorldTransposeMatrix.m[2][1] = identity.up.z;
	WorldTransposeMatrix.m[2][2] = identity.at.z;
	WorldTransposeMatrix.m[2][3] = identity.pos.z;

	WorldTransposeMatrix.m[3][0] = 0.0f;
	WorldTransposeMatrix.m[3][1] = 0.0f;
	WorldTransposeMatrix.m[3][2] = 0.0f;
	WorldTransposeMatrix.m[3][3] = 1.0f;

	RwD3D9SetVertexShaderConstant(8, (void *)&WorldTransposeMatrix, 4);

	if( RpMTextureIsEnableLinearFog() )
	{
		// 리니어 포그 계산용..
		D3DMATRIX				worldviewMatrix;
		_rwD3D9VSGetWorldViewTransposedMatrix((void *)&worldviewMatrix);
		RwD3D9SetVertexShaderConstant(15, (void *)&worldviewMatrix.m[2], 1);
	}

	// 마고자 (2004-08-11 오후 1:03:00) : 사정상 여기다가 옮겨 놓습니다.
	// 셰이더 콘스탄트 설정이 딴데서하면 꼬이는 증상이 있다.
	// 그래서 부득이 여기다가 옮겨둠.
	D3DMATRIX				world_view_matrix;
	_rwD3D9VSGetWorldViewMatrix( ( void * ) & world_view_matrix );
	RwD3D9SetVertexShaderConstant( 16,(void *)__fFadeOutFactor,1);
	RwD3D9SetVertexShaderConstant( 17,(void *) &world_view_matrix ,4);

	//@{ Jaewon 20050512
	// Terrain gloss map support(a view position for the half-angle vector calculation)
	RwV3d *pViewPos	= RwMatrixGetPos(RwFrameGetLTM(RwCameraGetFrame(m_pCamera)));
	D3DXVECTOR4	viewPos(pViewPos->x, pViewPos->y, pViewPos->z, 1.0f);
	RwD3D9SetVertexShaderConstant(21, (void*)&viewPos, 1);
	//@} Jaewon
}

RpAtomic*	AgcmRender::RenderCallbackForAmbientAtomic	( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry == NULL) return atomic;
	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	//AmbientList에 추가
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->sibling = NULL;
	new_node->billboard = 0;
	new_node->next = NULL;

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listAmbient.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node; 

			return NULL;
		}
		
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listAmbient.hash_table[key];
	m_pThisAgcmRender->m_listAmbient.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForAlphaFuncAtomic	( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry == NULL) return atomic;
	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	//AmbientList에 추가
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->sibling = NULL;
	new_node->billboard = 0;
	new_node->next = NULL;

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listAlphaFunc.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node; 

			return NULL;
		}
		
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listAlphaFunc.hash_table[key];
	m_pThisAgcmRender->m_listAlphaFunc.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomicNotSort		(RpAtomic *atomic)
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(!pGeometry) return NULL;
	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	//NotSortBlendList에 추가 
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->sibling = NULL;
	new_node->billboard = 0;
	new_node->next = NULL;

	INT32			key = new_node->texaddr%3;
	INT32			bmode = atomic->stRenderInfo.blendMode - R_BLEND11_ADD;
	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node;

			++m_pThisAgcmRender->m_iNumBlendNSort[bmode];

			return NULL;
		}
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key];
	m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key] = new_node;

	++m_pThisAgcmRender->m_iNumBlendNSort[bmode];

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForAlphaAtomic		( RpAtomic *atomic )
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	//AlphaList에 추가 
	AlphaAtomic	new_AlphaAtomic;
	ZeroMemory( &new_AlphaAtomic , sizeof(AlphaAtomic) );


	new_AlphaAtomic.atomic		=	atomic;
	new_AlphaAtomic.billboard	=	0;
	new_AlphaAtomic.SortFactor	=	m_pThisAgcmRender->_GetSortFactor( atomic );

	m_pThisAgcmRender->m_mapAlpha.insert( make_pair(new_AlphaAtomic.SortFactor , new_AlphaAtomic) );
	
	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomic( RpAtomic* atomic) // sort처리
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic = atomic;
	NewBlendAtomic.m_nBillBoardType = 0;
			
	if( atomic->stRenderInfo.blendMode == R_BLENDSRCAINVA_ADD ) // sort
	{		
		RwV3d atomicpos = RwFrameGetLTM(RpAtomicGetFrame (atomic))->pos;

		//@{ 2006/11/21 burumal
		RwV3dAdd(&atomicpos, &atomicpos, &atomic->boundingSphere.center);
		//@}

		RwV3d screenpos;
		m_pThisAgcmRender->GetWorldPosToScreenPos(&atomicpos,&screenpos);		

		NewBlendAtomic.m_fSortFactor = screenpos.z;
		m_pThisAgcmRender->m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );
	}
	else
	{
		NewBlendAtomic.m_fSortFactor = 0.0f;
		int nBlendMode = atomic->stRenderInfo.blendMode;

		m_pThisAgcmRender->m_BlendAtomics.Add( ( enumRenderBMode )nBlendMode, NewBlendAtomic );
	}
	
	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForNonAlphaAtomic			( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(!pGeometry) return NULL;
	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	// NonAlphaList에 추가
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->next = NULL;
	new_node->sibling = NULL;
	new_node->billboard = 0;

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listNonAlpha.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node;

			return NULL;
		}
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listNonAlpha.hash_table[key];
	m_pThisAgcmRender->m_listNonAlpha.hash_table[key] = new_node;
	
	return atomic;
}


RpAtomic*	AgcmRender::RenderCallbackForSkinPipe(RpAtomic*	atomic)
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry == NULL) return atomic;
	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	SkinningTSortAtomic*	new_node = NULL;

//#ifndef _DEBUG
//	new_node = AllocExtraFrameMemory<SkinningTSortAtomic>();
//#else
//	//Skin List에 추가
//	new_node = (SkinningTSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(SkinningTSortAtomic));
//#endif

	//Skin List에 추가
	new_node = (SkinningTSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(SkinningTSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic	= atomic;
	new_node->texaddr	= (DWORD)pTexture;
	new_node->sibling	= NULL;
	new_node->next		= NULL;

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	SkinningTSortAtomic*		cur_node = m_pThisAgcmRender->m_listSkinning.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node; 

			return NULL;
		}
		
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listSkinning.hash_table[key];
	m_pThisAgcmRender->m_listSkinning.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForSkinPipeBlend( RpAtomic* atomic )
{	
	return m_pThisAgcmRender->RenderCallbackForBlendAtomic(atomic);
}

RpAtomic*	AgcmRender::RenderCallbackForSkinPipeAlpha( RpAtomic* atomic )
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	SkinningAtomic*	new_node = (SkinningAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(SkinningAtomic));
	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic	= atomic;
	new_node->next		= m_pThisAgcmRender->m_listSkinningAlpha;
	m_pThisAgcmRender->m_listSkinningAlpha = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForSkinPipeAlphaFunc(RpAtomic*	atomic)
{
	RpGeometry*	pGeometry = RpAtomicGetGeometry(atomic);
	if( !pGeometry )	return atomic;
	RpMaterial* pMaterial = RpGeometryGetMaterial( pGeometry, 0 );
	RwTexture* pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	//Skin AlphaFunc에 추가
	SkinningTSortAtomic* new_node = (SkinningTSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(SkinningTSortAtomic));
	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic	= atomic;
	new_node->texaddr	= (DWORD)pTexture;
	new_node->sibling	= NULL;
	new_node->next		= NULL;

	UINT32 i = 0;

	INT32 key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);
	SkinningTSortAtomic* cur_node = m_pThisAgcmRender->m_listSkinningAlphaFunc.hash_table[key];
	while(cur_node)
	{
		if( IsBadReadPtr( cur_node, sizeof(SkinningTSortAtomic ) ) )
		{
			MD_SetErrorMessage("(IsBadReadPtr(cur_node)) : %d, %d, 0x%08x\n", key, i, new_node);
			return (RpAtomic*)NULL;
		}

		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node; 

			return NULL;
		}

		cur_node = cur_node->next;
		++i;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listSkinningAlphaFunc.hash_table[key];
	m_pThisAgcmRender->m_listSkinningAlphaFunc.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForUVAnim(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	//uvanim list에 추가
	UVAnimAtomic*	new_node = (UVAnimAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(UVAnimAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic	= atomic;
	new_node->next		= m_pThisAgcmRender->m_listUVAnim;
	m_pThisAgcmRender->m_listUVAnim = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForUVAnimSkin(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	//uvanim list에 추가
	UVAnimAtomic*	new_node = (UVAnimAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(UVAnimAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic	= atomic;
	new_node->next		= m_pThisAgcmRender->m_listUVAnimSkin;

	m_pThisAgcmRender->m_listUVAnimSkin = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForLastZWrite(RpAtomic*	atomic)
{
	//m_pThisAgcmRender->AtomicPreRender(atomic);
	SimpleAtomic*	new_node = (SimpleAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(SimpleAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic = atomic;

	new_node->next = m_pThisAgcmRender->m_listLastZWrite;
	m_pThisAgcmRender->m_listLastZWrite = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForNone(RpAtomic*	atomic)
{
	return atomic;
}

//###################################################################################################
// BillBoard 검사 & Render

RpAtomic*	AgcmRender::RenderCallbackForAmbientAtomicBill	( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry == NULL) return atomic;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->next = NULL;
	new_node->sibling = NULL;
	new_node->billboard = AcuObject::GetAtomicIsBillboard(atomic);

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listAmbient.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node;

			return NULL;
		}
		
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listAmbient.hash_table[key];
	m_pThisAgcmRender->m_listAmbient.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForAlphaFuncAtomicBill	( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(pGeometry == NULL) return atomic;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->next = NULL;
	new_node->sibling = NULL;
	new_node->billboard = AcuObject::GetAtomicIsBillboard(atomic);

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listAlphaFunc.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node;

			return NULL;
		}
		
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listAlphaFunc.hash_table[key];
	m_pThisAgcmRender->m_listAlphaFunc.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForAlphaAtomicBill		( RpAtomic *atomic )
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	AlphaAtomic	new_AlphaAtomic;
	ZeroMemory( &new_AlphaAtomic , sizeof(AlphaAtomic) );

	new_AlphaAtomic.atomic			=	atomic;
	new_AlphaAtomic.billboard		=	AcuObject::GetAtomicIsBillboard(atomic);
	new_AlphaAtomic.SortFactor		=	m_pThisAgcmRender->_GetSortFactor( atomic );

	m_pThisAgcmRender->m_mapAlpha.insert( make_pair(new_AlphaAtomic.SortFactor , new_AlphaAtomic) );

	if(!m_pThisAgcmRender->m_pApmOcTree->m_bOcTreeEnable)
		m_pThisAgcmRender->SetDistance(atomic);

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomicNotSortBill		(RpAtomic *atomic)
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(!pGeometry) return NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	//AlphaNorsortList에 추가 
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->next = NULL;
	new_node->sibling = NULL;
	new_node->billboard = AcuObject::GetAtomicIsBillboard(atomic);

	INT32			key = new_node->texaddr%3;
	int				bmode = atomic->stRenderInfo.blendMode - R_BLEND11_ADD;

	ASSERT( bmode >= 0 && bmode < R_NOT_SORT_BLEND_MODE && "Blend mode Error!!!");
	if (bmode < 0 || bmode >= R_NOT_SORT_BLEND_MODE)
		return atomic;

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node;

			return NULL;
		}
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key];
	m_pThisAgcmRender->m_listBlendNSort.hash_table[bmode][key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForNonAlphaAtomicBill		( RpAtomic *atomic )
{
	RpGeometry*		pGeometry = RpAtomicGetGeometry(atomic);
	if(!pGeometry) return NULL;

	m_pThisAgcmRender->AtomicPreRender(atomic);

	RpMaterial*		pMaterial = RpGeometryGetMaterial(pGeometry,0);
	RwTexture*		pTexture = pMaterial ? RpMaterialGetTexture(pMaterial) : NULL;

	// NonAlphaList에 추가
	TextureSortAtomic*	new_node = (TextureSortAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(TextureSortAtomic));

	ASSERT( new_node && "FrameMemory Allocation Error!!!");
	if (!new_node)
		return atomic;

	new_node->atomic = atomic;
	new_node->texaddr = (DWORD)pTexture;
	new_node->next = NULL;
	new_node->sibling = NULL;
	new_node->billboard = AcuObject::GetAtomicIsBillboard(atomic);

	INT32			key = m_pThisAgcmRender->GetHashKey(new_node->texaddr);

	TextureSortAtomic*		cur_node = m_pThisAgcmRender->m_listNonAlpha.hash_table[key];
	while(cur_node)
	{
		if(cur_node->texaddr == new_node->texaddr)
		{
			new_node->sibling = cur_node->sibling;
			cur_node->sibling = new_node; 

			return NULL;
		}
		cur_node = cur_node->next;
	}

	// 없었으므로 추가
	new_node->next = m_pThisAgcmRender->m_listNonAlpha.hash_table[key];
	m_pThisAgcmRender->m_listNonAlpha.hash_table[key] = new_node;

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomicBill		( RpAtomic *atomic )
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic = atomic;
	NewBlendAtomic.m_nBillBoardType = AcuObject::GetAtomicIsBillboard( atomic );

	if(atomic->stRenderInfo.blendMode == R_BLENDSRCAINVA_ADD) // sort
	{
		RwV3d atomicpos = RwFrameGetLTM(RpAtomicGetFrame (atomic))->pos;

		RwV3d screenpos;
		m_pThisAgcmRender->GetWorldPosToScreenPos(&atomicpos,&screenpos);

		NewBlendAtomic.m_fSortFactor = screenpos.z;
		m_pThisAgcmRender->m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );
	}
	else
	{
		NewBlendAtomic.m_fSortFactor = 0.0f;
		int bmode = atomic->stRenderInfo.blendMode;

		m_pThisAgcmRender->m_BlendAtomics.Add( ( enumRenderBMode )bmode, NewBlendAtomic );
	}
	
	return atomic;
}

BOOL AgcmRender::SetCallbackCustomRender(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_CUSTOM_RENDER, pfCallback, pClass);
}
BOOL AgcmRender::SetCallbackPreRender(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_PRERENDER, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackPostRender(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_POSTRENDER, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackPostRender2(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_POSTRENDER2, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackPostRenderOcTreeClear(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_POSTRENDER_OCTREE_CLEAR, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackRemoveAtomic(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_REMOVEATOMIC, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackRemoveClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_REMOVECLUMP, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackOctreeUDASet(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_OCTREE_UDA_SET, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackOctreeIDSet(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_OCTREE_ID_SET, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackAddAtomic(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_ADDATOMIC, pfCallback, pClass);
}

BOOL	AgcmRender::SetCallbackAddClump(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMRENDER_CB_ID_ADDCLUMP, pfCallback, pClass);
}

void	AgcmRender::RenderWorldForAllIntersections(RpIntersection*    intersection,   RpIntersectionCallBackAtomic    callBack, void *    data )
{
	RwLine		line = intersection->t.line;
	RwReal		dist;

	RpAtomic*				cur_atomic;
	
	WorldAddClumpGroup*	cur_group = m_listWorldClumps;
	while( cur_group )
	{
		if( RtLineSphereIntersectionTest( &line, &cur_group->WorldBS, &dist ) )
		{
			if( cur_group->pClump )
			{
				ASSERT(cur_group->Atomiclist == NULL);
				if(cur_group->pClump->atomicList)
				{
					cur_atomic = cur_group->pClump->atomicList;
					do
					{
						if ( cur_atomic->geometry )
							callBack (intersection, NULL , cur_atomic , dist , data);

						cur_atomic = cur_atomic->next;
					}
					while(cur_atomic != cur_group->pClump->atomicList);
				}
			}
			else
			{
				ASSERT(cur_group->Atomiclist != NULL);
				cur_atomic = cur_group->Atomiclist;

				while(cur_atomic)
				{
					if ( cur_atomic->geometry )
						callBack (intersection, NULL , cur_atomic , dist , data);

					cur_atomic = cur_atomic->next;
				}
			}
		}

		cur_group = cur_group->next;
	}
}

void	AgcmRender::CBWorldIntersection(RpIntersection*    intersection,   RpIntersectionCallBackAtomic    callBack, void *    data )
{
	m_pThisAgcmRender->RenderWorldForAllIntersections(intersection,callBack,data);
}

RpAtomic*	AgcmRender::GeometryMaterialFlagOff( RpAtomic *atomic, void *data )
{
	RpGeometry* pGeometry = RpAtomicGetGeometry( atomic );
	if ( !pGeometry )
	{
		ASSERT( "Geometry가 NULL" );
		return NULL;
	}

	RwUInt32 lGeometryFlags = RpGeometryGetFlags( pGeometry );
	
	if( rpGEOMETRYMODULATEMATERIALCOLOR == ( lGeometryFlags & rpGEOMETRYMODULATEMATERIALCOLOR ) )
		RpGeometrySetFlags( pGeometry, ( lGeometryFlags & ~rpGEOMETRYMODULATEMATERIALCOLOR ) );

	return atomic;
}

RpAtomic*	AgcmRender::GeometryMaterialFlagOn( RpAtomic *atomic, void *data )
{
	RpGeometry* pGeometry = RpAtomicGetGeometry( atomic );
	if ( !pGeometry )
	{
		ASSERT( "Geometry가 NULL" );
		return NULL;
	}

	RwUInt32 lGeometryFlags = RpGeometryGetFlags( pGeometry );
	
	if( rpGEOMETRYMODULATEMATERIALCOLOR != ( lGeometryFlags & rpGEOMETRYMODULATEMATERIALCOLOR ) )
		RpGeometrySetFlags( pGeometry, ( lGeometryFlags | rpGEOMETRYMODULATEMATERIALCOLOR ) );

	return atomic;
}


void		AgcmRender::OriginalAtomicRender( RpAtomic* pAtomic )
{
	if ( RpAtomicGetGeometry(pAtomic) && RpGeometryGetNumTriangles(RpAtomicGetGeometry(pAtomic)) > 1 )
		AgcmRender::OriginalDefaultAtomicRenderCallback( pAtomic );
}

void		AgcmRender::OriginalClumpRender ( RpClump* pClump )
{
	RpClumpForAllAtomics( pClump, AgcmRender::RenderClumpOriginalDefaultRender, NULL );
}

void		AgcmRender::SetUVAnimRenderCB(RpAtomic*	atomic)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)atomic->stRenderInfo.backupCB3;
	if( specialRenderingFlag & eSpecialRenderUVAni )		return;

	specialRenderingFlag |= eSpecialRenderUVAni;

	RpLODAtomicUnHookRender(atomic);
	RpAtomicSetRenderCallBack(atomic, RenderCallbackForSpecialRendering);
	RpLODAtomicHookRender(atomic);
}

void		AgcmRender::ReturnUVAnimRenderCB(RpAtomic*	atomic)
{
	RwUInt32& specialRenderingFlag = (RwUInt32&)atomic->stRenderInfo.backupCB3;
	if(!(specialRenderingFlag & eSpecialRenderUVAni))
		return;

	specialRenderingFlag &= ~eSpecialRenderUVAni;

	if( !specialRenderingFlag)
	{
		RpLODAtomicUnHookRender(atomic);
		RpAtomicCallBackRender pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB2;
		RpAtomicSetRenderCallBack(atomic, pFunc);
		RpLODAtomicHookRender(atomic);
	}
}

RpAtomic*	AgcmRender::RenderClumpOriginalDefaultRender( RpAtomic *atomic, void *data )
{
	AgcmRender::m_pThisAgcmRender->OriginalAtomicRender( atomic );
	return atomic;
}

void	AgcmRender::UpdateDistanceForTool()
{
	// RenderWorld List 순회하며 distance갱신
	WorldAddClumpGroup*	cur_wc = m_listWorldClumps;
	while(cur_wc)
	{
		if( cur_wc->pClump )
		{
			ASSERT( !cur_wc->Atomiclist );
			if( cur_wc->pClump->atomicList )
				SetDistance( cur_wc->pClump->atomicList );
		}
		else
		{
			ASSERT( cur_wc->Atomiclist );
			SetDistance( cur_wc->Atomiclist );
		}

		cur_wc = cur_wc->next;
	}
}

void	AgcmRender::SetDistance(RpAtomic* atomic,INT32 CameraZindex) // clump가 있으면 clump의 userdata에 거리와 tick을 set하고 없음 atomic에 set
{
	RpClump *pClump	= RpAtomicGetClump(atomic);

	if( pClump && !IsBadWritePtr( pClump, sizeof(RpClump ) ) )
	{
		if( pClump->stUserData.calcDistanceTick != m_ulCurTick && ( pClump->stUserData.maxLODLevel > 0 || pClump->stUserData.characterShadowLevel > 0 ))		// 거리값 set
		{
			ASSERT( m_pFrame );

			RwV3d	clumpPos, mainPos, temp;
			RwReal	distance;

			clumpPos = RwFrameGetLTM(RpClumpGetFrame (pClump))->pos;
			mainPos	 = RwFrameGetLTM(m_pFrame)->pos;

			RwV3dSub(&temp, &clumpPos, &mainPos);
			distance = RwV3dLength(&temp);

			pClump->stUserData.calcDistance = distance;
			pClump->stUserData.calcDistanceTick = m_ulCurTick;

			pClump->stUserData.octreeModeCamZIndex = CameraZindex;
		}
		else														// TextBoard같은 것들 검사위해 setting해주자
			pClump->stUserData.calcDistanceTick = m_ulCurTick;
	}
}

void	AgcmRender::AddCustomToBlendRenderList( PVOID pClass, ApModuleDefaultCallBack pCustRenderCB, PVOID pData1, PVOID pData2 ,  RwV3d*	pWorldPos )
{
	if( !pCustRenderCB )		return;

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic		=	( RpAtomic* )pData1;
	NewBlendAtomic.m_pData2			=	pData2;
	NewBlendAtomic.m_pClass			=	pClass;
	NewBlendAtomic.m_nBillBoardType =	BS_CUSTOM;	
	NewBlendAtomic.m_pData1			=	( PVOID )pCustRenderCB;

	if( pWorldPos )
	{
		RwV3d screenpos;
		GetWorldPosToScreenPos( pWorldPos , &screenpos );		
		NewBlendAtomic.m_fSortFactor	=	screenpos.z;
	}

	m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );
}

void AgcmRender::ResetLighting(RpClump *pClump) const
{
	pClump->stUserData.customLightStatus = 0;
}

void AgcmRender::CustomizeLighting(RpClump *pClump, enumLightingType lightingType) const
{
	if( m_bLogin && lightingType != LIGHT_LOGIN_SELECT )	return; //login모드일 경우 shadow등으 lighting조작X

	pClump->stUserData.customLightStatus = (RwInt8)lightingType;
}
//@} Jaewon

//◆◆◆◆◆◆◆◆◆◆◆◆ Sector관련 처리 함수 ◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
void	AgcmRender::SetSectorDistanceC(RpClump*		pClump)
{
	AgcdLODData* pstAgcdLODData = (AgcdLODData*) pClump->stType.pCustData;

	UINT32	setVal = pstAgcdLODData && m_bUseLODBoundary ? (pstAgcdLODData->m_ulBoundary ? ( pstAgcdLODData->m_ulBoundary / (UINT32)MAP_SECTOR_WIDTH + 2 ) : R_MAX_SECTOR_DISTANCE) : R_MAX_SECTOR_DISTANCE;

	pClump->stType.viewSectorDistance = setVal;
}

void	AgcmRender::SetSectorDistanceA( RpAtomic* pAtomic )
{
	AgcdType*	lpType =  AcuObject::GetAtomicTypeStruct(pAtomic); 
	AgcdLODData* pstAgcdLODData = (AgcdLODData*) lpType->pCustData;

	UINT32 setVal = pstAgcdLODData && m_bUseLODBoundary ? ( pstAgcdLODData->m_ulBoundary ? (pstAgcdLODData->m_ulBoundary / (UINT32)MAP_SECTOR_WIDTH + 2) : R_MAX_SECTOR_DISTANCE ) : R_MAX_SECTOR_DISTANCE;
	lpType->viewSectorDistance = setVal;
}

RpAtomic * AgcmRender::CalcBoundingSphere(RpAtomic * atomic, void *data)
{
    RwSphere*  sphere = (RwSphere *) data;
	RwSphere bs = *RpAtomicGetBoundingSphere(atomic);

	FLOAT			distx = sphere->center.x - bs.center.x;
	FLOAT			disty = sphere->center.y - bs.center.y;
	FLOAT			distz = sphere->center.z - bs.center.z;

	FLOAT			dist = (FLOAT)sqrt((FLOAT)(distx*distx + disty*disty + distz*distz));

	if(bs.radius > dist)
	{
		sphere->radius = bs.radius;
	}
	else if(dist + bs.radius > sphere->radius)
	{
		sphere->radius = dist + bs.radius;
	}

	return atomic;
}

void	AgcmRender::AddTerrainToSector(RpAtomic*	atomic)
{
	RwSphere*		sphere = RpAtomicGetBoundingSphere (atomic);
	
	ApWorldSector* pSector = m_pcApmMap->GetSector(sphere->center.x, sphere->center.z);
	if(pSector)
	{
		SectorRenderList*	pList = GetSectorData(pSector);

		pList->terrain = atomic;
	}
}

void	AgcmRender::AddAtomicToSector(RpAtomic*	atomic,INT32 addtype)
{
	RpClump* pClump = RpAtomicGetClump(atomic);
	RwFrame* frame = pClump ? RpClumpGetFrame(pClump) : RpAtomicGetFrame(atomic);
	if( !frame )	return;

 	RwMatrix*		mtm = RwFrameGetMatrix(frame);
	RwV3d*			pos = RwMatrixGetPos(mtm);

	RwSphere*		sphere = RpAtomicGetBoundingSphere (atomic);
	float			radius = (float)((int)sphere->radius);

	AuPOS	pos2;
	pos2.x = (float)((int)pos->x);
	pos2.y = pos->y;
	pos2.z = (float)((int)pos->z);

	float* sector_adddata = AcuObject::MakeAtomicSectorModeBackup(atomic);
	if ( !sector_adddata )		return;

	sector_adddata[0] = pos->x;
	sector_adddata[1] = pos->z;
	sector_adddata[2] = radius;

	ApWorldSector*	vectorSector[48];
	int num = m_pcApmMap->GetSectorList( pos2, radius, vectorSector, 48 );
	for( int i=0; i<num; ++i )
	{
		SectorRenderList* pList = GetSectorData( vectorSector[i] );
		pList->Add( (eAddSectorMode)addtype, atomic );
	}
}

void	AgcmRender::AddClumpToSector( RpClump* clump, INT32 type )
{
	RpClumpForAllAtomics( clump, CallBackForEnumAtomicAddSector, (PVOID)type );
}

RpAtomic*	AgcmRender::CallBackForEnumAtomicAddSector( RpAtomic* atomic, void* data )
{
	INT32	type = (INT32)data;

	m_pThisAgcmRender->AddAtomicToSector( atomic, type );
	
	return	atomic;
}

void	AgcmRender::RemoveAtomicFromSector( RpAtomic* atomic, INT32 deltype )
{
	if ( atomic->object.object.type != 1 )
	{
		ASSERT(!"AgcmRender::RemoveAtomicFromSector() not atomic!!!\n");
		return;
	}

	ApWorldSector*	pSector;
	int		lIndex = AcuObject::GetAtomicType(atomic, (INT32 *) &pSector);
	int		type = AcuObject::GetType(lIndex);

	if( ACUOBJECT_TYPE_WORLDSECTOR == type )
	{
		if( pSector )
		{
			SectorRenderList* pList = GetSectorData( pSector );
			if( atomic == pList->terrain )
				pList->terrain = NULL;
		}
		else
			ASSERT( !"AgcmRender::RemoveAtomicFromSector() Error Sector Atomic Type!!!\n" );

		return;
	}

	float*	pSectorAddData = AcuObject::GetAtomicSectorModeBackup( atomic );
	if( !pSectorAddData )	return;

	AuPOS cPosSector = { (float)pSectorAddData[0], 0.f, (float)pSectorAddData[1] };
	float fRadius = (float)pSectorAddData[2];

	ApWorldSector*	listSector[48];
	int nSector = m_pcApmMap->GetSectorList( cPosSector, fRadius, listSector, 48 );
	for( int i=0; i<nSector; ++i )
	{
		SectorRenderList* pList = GetSectorData( listSector[i] );
		if( pList )
			pList->Delete( (eAddSectorMode)deltype, atomic );
	}
}

void	AgcmRender::RemoveClumpFromSector( RpClump* clump, INT32 type )
{
	RpClumpForAllAtomics( clump, CallBackForEnumAtomicRemoveSector, (PVOID)type );
}

void	AgcmRender::AddCustomRenderToSector( PVOID pClass, RwSphere* pSphere, ApModuleDefaultCallBack pCustUpdateCB, ApModuleDefaultCallBack pCustRenderCB, PVOID pData1, PVOID pData2, INT32 iSectorDistance )
{
	ApWorldSector*	pSector = m_pcApmMap->GetSector( pSphere->center.x, pSphere->center.z );
	if( !pSector )			return;

	SectorRenderList* pList = GetSectorData( pSector );
	pList->listCustomRenderSector->push_back( CustomRenderSector( pCustUpdateCB, pCustRenderCB, pSphere, pClass, pData1, pData2, iSectorDistance ) );
}

void	AgcmRender::RemoveCustomRenderFromSector( PVOID pClass, RwSphere* pSphere, PVOID pData1, PVOID pData2)
{
	ApWorldSector*	pSector = m_pcApmMap->GetSector( pSphere->center.x,pSphere->center.z );
	if( !pSector )		return;

	SectorRenderList*	pList = GetSectorData(pSector);

	for( CustomRenderSectorListItr Itr = pList->listCustomRenderSector->begin(); Itr != pList->listCustomRenderSector->end(); ++Itr )
	{
		if( (*Itr).pClass == pClass && (*Itr).data1 == pData1 && (*Itr).data2 == pData2 )
		{
			pList->listCustomRenderSector->erase( Itr );
			break;
		}
	}
}

RpAtomic*	AgcmRender::CallBackForEnumAtomicRemoveSector(RpAtomic*	atomic,void*	data)
{
	INT32	type = (INT32)data;
	m_pThisAgcmRender->RemoveAtomicFromSector(atomic,type);
	
	return	atomic;
}

void	AgcmRender::RenderCustomObjects()
{
	// Custom Render List검사(maincamera만 지원)
	CustomRenderList* cur_cr = m_listCustomRender;
	while(cur_cr)
	{
		RwFrustumTestResult eResult = RwCameraFrustumTestSphere( m_pCamera, cur_cr->BS );
		if( eResult != rwSPHEREOUTSIDE )
		{
			cur_cr->CustUpdateCB( cur_cr->data1, cur_cr->pClass, cur_cr->data2 );  
			AddCustomToBlendRenderList( cur_cr->pClass, cur_cr->CustRenderCB, cur_cr->data1, cur_cr->data2 , &cur_cr->BS->center );
		}

		cur_cr = cur_cr->next;
	}
}

BOOL AgcmRender::CBInitSector( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmRender*	pThis = (AgcmRender*)pClass;
	SectorRenderList* pList = pThis->GetSectorData( (ApWorldSector*)pData );
	pList->Init();

	return TRUE;
}

BOOL	AgcmRender::CBRemoveSector ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmRender* pThis  = (AgcmRender*) pClass;
	SectorRenderList* pList = pThis->GetSectorData( (ApWorldSector*)pData );
	pList->Clear();
	pList = NULL;

  	return TRUE;
}

BOOL AgcmRender::CBInitSectorData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmRender* pThis = (AgcmRender*)pClass;
	SectorData* pSectorData = pThis->GetClientSectorData( (ApWorldSector*)pData );
	pSectorData->listLocalLight = new LocalLightList;
	pSectorData->listLocalLight->clear();

	return TRUE;
}
BOOL AgcmRender::CBRemoveSectorData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmRender* pThis = (AgcmRender*)pClass;
	SectorData* pSectorData = pThis->GetClientSectorData( (ApWorldSector*)pData );
	pSectorData->listLocalLight->clear();
	delete pSectorData->listLocalLight;
	pSectorData->listLocalLight = NULL;

	return TRUE;
}

void	AgcmRender::RenderGeometryOnly	(int type)		// 0 - 지형 , 1 - 나머지
{
	m_pCameraPos	= RwMatrixGetPos(RwFrameGetLTM(RwCameraGetFrame(m_pCamera)));

	if( m_bActiveClass )
	{
		// Render World 
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)true );

		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );

		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			
		if(type == 1)
		{
			if (m_pWorld)
			{
				#ifdef _DPVS
					RpDPVSWorldRender( m_pWorld );
				#else
					RpWorldRender( m_pWorld );
				#endif //_DPVS
			}
		}

		if(m_bUseCullMode)
		{
			RenderWorldAtomics(m_pCamera,DefaultAtomicListTestRender);
			RenderCustomObjects();

			CullModeWorldRenderGeometryOnly(type);
		}
		
		int		fogTableMode;
		int		fogVertexMode;

		if( RpMTextureIsEnableLinearFog() && m_bVertexShaderEnable )
		{
			RwD3D9GetRenderState( D3DRS_FOGTABLEMODE , &fogTableMode );
			RwD3D9GetRenderState( D3DRS_FOGVERTEXMODE , &fogVertexMode );

			RwD3D9SetRenderState( D3DRS_FOGTABLEMODE , D3DFOG_NONE );
			RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE , D3DFOG_NONE );
		}

		// 지형shadow2 path 그린다.
		RpAtomicCallBackRender	pFunc;
		if(m_bVertexShaderEnable)
		{
			RpLightSetColor(m_pDirectLight, &m_stAdjustedDirectionalLight);
			RpLightSetColor(m_pAmbientLight, &m_stAdjustedAmbientLight);
		}
		else 
		{
			RwRGBAReal	direct = { 0.08f, 0.08f, 0.08f, 1.0f };

			RpLightSetColor( m_pDirectLight, &direct );
			RpLightSetColor( m_pAmbientLight, &m_colorBlack );
		}

		RpMTextureDrawStart();

		if( m_bVertexShaderEnable )
			TerrainDataSet();

		Shadow2TerrainList*		cur_s2 = m_listShadow2Terrain;
		Shadow2TerrainList*		bef_s2 = cur_s2;
		while(cur_s2)
		{
			pFunc = (RpAtomicCallBackRender)cur_s2->atomic->stRenderInfo.backupCB;

			cur_s2->atomic->stRenderInfo.shaderUseType = 0;
			cur_s2->atomic->stRenderInfo.beforeLODLevel = cur_s2->sectordist;

			pFunc(cur_s2->atomic);
						
			if(!cur_s2->bShadow)					// 그림자가 없다면 다음 list순회를 줄이기 위해 삭제 ^^
			{
				if(cur_s2 == m_listShadow2Terrain)
				{
					m_listShadow2Terrain = cur_s2->next;
				}
				else 
				{
					bef_s2->next = cur_s2->next;
				}
				
				cur_s2 = cur_s2->next;
			}
			else
			{
				bef_s2 = cur_s2;									
				cur_s2 = cur_s2->next;
			}
		}

		RpMTextureDrawEnd(TRUE);

		cur_s2 = m_listShadow2Terrain;				// Object Shadow 그리자 
		while(cur_s2)
		{
			pFunc = (RpAtomicCallBackRender)cur_s2->atomic->stRenderInfo.backupCB;
			cur_s2->atomic->stRenderInfo.shaderUseType = 1;
			pFunc(cur_s2->atomic);

			cur_s2 = cur_s2->next;
		}

		if(m_pShadow2Terrain_Own)
		{
			RpMTextureDrawStart();

			pFunc = (RpAtomicCallBackRender)m_pShadow2Terrain_Own->atomic->stRenderInfo.backupCB;

			m_pShadow2Terrain_Own->atomic->stRenderInfo.shaderUseType = 0;
			m_pShadow2Terrain_Own->atomic->stRenderInfo.beforeLODLevel = m_pShadow2Terrain_Own->sectordist;

			pFunc(m_pShadow2Terrain_Own->atomic);

			RpMTextureDrawEnd(TRUE);
			
			if(m_bDrawShadow2)
			{
				m_pShadow2Terrain_Own->atomic->stRenderInfo.shaderUseType = 1;
				pFunc(m_pShadow2Terrain_Own->atomic);
			}
		}
		
		RpLightSetColor(m_pAmbientLight, &m_colorOriginalAmbient);
		RpLightSetColor(m_pDirectLight, &m_colorOriginalDirect);

		if(RpMTextureIsEnableLinearFog() && m_bVertexShaderEnable)
		{
			RwD3D9SetRenderState(D3DRS_FOGTABLEMODE , fogTableMode);
			RwD3D9SetRenderState(D3DRS_FOGVERTEXMODE , fogVertexMode);
		}

		ComputeLightColors();

		// 2005.4.11 gemani
		cur_s2 = m_listTerrainNoMTexture;
		while(cur_s2)
		{
			RenderAtomic(cur_s2->atomic,FALSE);
			cur_s2 = cur_s2->next;
		}

		RenderNonAlphaAtomics();
		
		RenderUVAnimAtomics(m_listUVAnim);
		
		RenderAlphaAtomics();
		RenderAmbientAtomics();

		RenderAlphaFuncAtomics();
		RenderSkinPipeAtomics();
		RenderSkinPipeAlphaFuncAtomics();
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE , (void*)TRUE  );

		RenderUVAnimAtomics(m_listUVAnimSkin);
		RenderLastZWriteAtomics();
		
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void*)false );

		D3DMATERIAL9		lMaterial;
		memcpy(&lMaterial,&LastMaterial,sizeof(D3DMATERIAL9));
	
		lMaterial.Emissive.r = 0.0f;
        lMaterial.Emissive.g = 0.0f;
        lMaterial.Emissive.b = 0.0f;
        lMaterial.Emissive.a = 1.0f;

		RwD3D9SetMaterial( &lMaterial );

		RenderSkinPipeBlendAtomics();
		EnumCallback( AGCMRENDER_CB_ID_POSTRENDER, NULL, NULL );
		EnumCallback( AGCMRENDER_CB_ID_POSTRENDER_OCTREE_CLEAR, NULL, NULL );
		
		RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, ( void * )rwTEXTUREADDRESSCLAMP  );

		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);

		RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

		// Render Sorted Alpha Atomic
		RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );
		RenderSortedBlendAtomics();			// 나중에 위로 올림(ZBUFFER에 적는다)
		RenderNotSortedBlendAtomics();
		
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA  );
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA   );

		EnumCallback( AGCMRENDER_CB_ID_POSTRENDER2, NULL, NULL );
				
		RenderInvisibleAtomics();

		// ProcessAtomic처리 안함
		ClearLists();
	}
}

void	AgcmRender::WorldToAtomicRender( RpAtomic* pAtomic, INT32 nDistSector, BOOL bUpdateFrustum )
{
	#ifdef	USE_MFC
	if( g_pfPreRenderAtomicCallback && !g_pfPreRenderAtomicCallback( pAtomic, g_pPreRenderAtomicData ) )
		return;
	#endif

	AgcdType* pType = AcuObject::GetAtomicTypeStruct( pAtomic );
	if( nDistSector < pType->viewSectorDistance )
	{
		if( bUpdateFrustum )
		{
			pType->frustumTestTick		= m_ulCurTick;
			pType->frustumTestResult	= rwSPHEREBOUNDARY;
		}

		if( pAtomic->stRenderInfo.curTick != (INT32)m_ulCurTick )		// 한번만 추가~~
		{
			pAtomic->stRenderInfo.curTick = m_ulCurTick;
			RpAtomicRender( pAtomic );
		}
	}
}

void	AgcmRender::WorldToAtomicFrustumRender( RpAtomic* pAtomic, RwSphere& cSphere, INT32 nDistSector, RwCamera* pCheckCamera, AgcmRenderTestRenderAtomic pRenderFunc )
{
	#ifdef	USE_MFC
	if( g_pfPreRenderAtomicCallback && !g_pfPreRenderAtomicCallback( pAtomic, g_pPreRenderAtomicData ) )
		return;
	#endif

	AgcdType* pType = AcuObject::GetAtomicTypeStruct( pAtomic );
	if( nDistSector >= pType->viewSectorDistance )				return;
	if( pAtomic->stRenderInfo.curTick == (INT32)m_ulCurTick )	return;

	pAtomic->stRenderInfo.curTick = m_ulCurTick;

	if( pType->frustumTestTick == m_ulCurTick && pType->frustumTestResult == rwSPHEREBOUNDARY )
	{
		pRenderFunc( pAtomic );
	}
	else if( pType->frustumTestTick == m_ulCurTick && pType->frustumTestResult == rwSPHEREOUTSIDE )
	{
		// nothing
	}
	else
	{
		pType->frustumTestTick = m_ulCurTick;

		RwFrustumTestResult res = RwCameraFrustumTestSphere( pCheckCamera, &cSphere );
		if(res != rwSPHEREOUTSIDE)		// 카메라 안에 들어왔다! render callback 호출
		{
			pType->frustumTestResult = rwSPHEREBOUNDARY;
			pRenderFunc( pAtomic );
		}
		else
		{
			pType->frustumTestResult = rwSPHEREOUTSIDE;
		}
	}
}

// custom render는 maincamera만 지원^^
void	AgcmRender::CullModeWorldRender( RwCamera* pCheckCamera, AgcmRenderTestRenderAtomic pRenderFunc, AgcmRenderTestRenderTerrain pTerrainFunc )
{
	RwV3d* vPos = RwMatrixGetPos( RwFrameGetMatrix(m_pFrame) );

	ApWorldSector* pSector = m_pcApmMap->GetSector( vPos->x, vPos->z );
	if( !pSector )		return;

	INT32	ix = pSector->GetArrayIndexX();
	INT32	iz = pSector->GetArrayIndexZ();

	INT32	sx = ix - 10;
	INT32	ex = ix + 10;
	INT32	sz = iz - 10;
	INT32	ez = iz + 10;
	
	RwSphere	bs2;		//  movable용
	RwFrustumTestResult		res;
		
	for( int i=sx; i<ex; ++i )
	{
		if( i < 0 )					continue;

		for( int j=sz; j<ez; ++j )
		{
			if( j < 0 )				continue;

			pSector = m_pcApmMap->GetSectorByArrayIndex( i, j );
			if( !pSector )			continue;

			SectorRenderList* pList = GetSectorData( pSector );
			if( !pList->terrain )	continue;

			INT32 nDistSector = (INT32)sqrt((FLOAT)((ix-i)*(ix-i) + (iz-j)*(iz-j)));
			RwSphere* sphere = RpAtomicGetBoundingSphere( pList->terrain );

			res = RwCameraFrustumTestSphere( pCheckCamera, sphere );

			if( res == rwSPHEREINSIDE )
			{
				pTerrainFunc( pList->terrain, nDistSector );		// 지형 그리기
				
				for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
				{
					SetDistance( (*Itr).atomic );
					WorldToAtomicRender( (*Itr).atomic, nDistSector, TRUE );
				}

				for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
				{
					SetDistance( (*Itr).atomic );
					WorldToAtomicRender( (*Itr).atomic, nDistSector, TRUE );
				}

				for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
				{
					SetDistance( (*Itr) );
					WorldToAtomicRender( (*Itr), nDistSector, TRUE );
				}

				for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
				{
					SetDistance( (*Itr) );
					WorldToAtomicRender( (*Itr), nDistSector, TRUE );
				}

				for( CustomRenderSectorListItr Itr = pList->listCustomRenderSector->begin(); Itr != pList->listCustomRenderSector->end(); ++Itr )
				{
					if( nDistSector <= (*Itr).iSectorDistance )
					{
						res = RwCameraFrustumTestSphere( m_pCamera, (*Itr).BS );
						if( res != rwSPHEREOUTSIDE )
						{
							(*Itr).CustUpdateCB( (*Itr).data1, (*Itr).pClass, (*Itr).data2 );  
							AddCustomToBlendRenderList( (*Itr).pClass, (*Itr).CustRenderCB, (*Itr).data1, (*Itr).data2 , &(*Itr).BS->center );
							
						}
					}
				}
			}
			else if( res == rwSPHEREBOUNDARY )
			{
				pTerrainFunc( pList->terrain, nDistSector );		// 지형 그리기

				for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
				{
					SetDistance( (*Itr).atomic );
					WorldToAtomicFrustumRender( (*Itr).atomic, (*Itr).cSphere, nDistSector, pCheckCamera, pRenderFunc );
				}

				for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
				{
					SetDistance( (*Itr).atomic );
					WorldToAtomicFrustumRender( (*Itr).atomic, (*Itr).cSphere, nDistSector, pCheckCamera, pRenderFunc );
				}

				for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
				{
					SetDistance( (*Itr) );
					CalcWorldBoundingSphere( &bs2, (*Itr) );
					WorldToAtomicFrustumRender( (*Itr), bs2, nDistSector, pCheckCamera, pRenderFunc );
				}

				for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
				{
					SetDistance( (*Itr) );
					CalcWorldBoundingSphere( &bs2, (*Itr) );
					WorldToAtomicFrustumRender( (*Itr), bs2, nDistSector, pCheckCamera, pRenderFunc );
				}

				for( CustomRenderSectorListItr Itr = pList->listCustomRenderSector->begin(); Itr != pList->listCustomRenderSector->end(); ++Itr )
				{
					if( nDistSector <= (*Itr).iSectorDistance )
					{
						res = RwCameraFrustumTestSphere( m_pCamera, (*Itr).BS );
						if( res != rwSPHEREOUTSIDE )
						{
							(*Itr).CustUpdateCB( (*Itr).data1, (*Itr).pClass, (*Itr).data2 );  
							AddCustomToBlendRenderList( (*Itr).pClass, (*Itr).CustRenderCB, (*Itr).data1, (*Itr).data2 , &(*Itr).BS->center  );
						}
					}
				}
			}
		}
	}
}

void	AgcmRender::CullModeWorldRenderGeometryOnly( int type )		// 0 - 지형 1- 그외(지형포함)
{
	RwV3d* vPos = RwMatrixGetPos( RwFrameGetMatrix( RwCameraGetFrame(m_pCamera) ));

	ApWorldSector*	pSector = m_pcApmMap->GetSector( vPos->x, vPos->z );
	if( !pSector )		return;

	INT32	ix = pSector->GetArrayIndexX();
	INT32	iz = pSector->GetArrayIndexZ();

	INT32	sx = ix-10;
	INT32	ex = ix+10;
	INT32	sz = iz-10;
	INT32	ez = iz+10;

	Shadow2TerrainList* nw_node = NULL;
	
	for(int i=sx;i<ex;++i)
	{
		if( i < 0 )					continue;

		for(int j=sz;j<ez;++j)
		{
			if( j < 0 )				continue;

			 pSector = m_pcApmMap->GetSectorByArrayIndex( i, j );
			if( !pSector )			continue;

			SectorRenderList* pList = GetSectorData(pSector);
			if( !pList->terrain )	continue;

			// 지형 그리기
			if(type <= 1)
			{
				nw_node = (Shadow2TerrainList*)AcuFrameMemory::AllocFrameMemory( SHADOW2_TERRAIN_LIST_SIZE );
				ASSERT( nw_node && "FrameMemory Allocation Error!!!");
				if ( !nw_node )		return;
			}

			INT32 nDistSector = (INT32)sqrt((FLOAT)((ix-i)*(ix-i) + (iz-j)*(iz-j)));

			nw_node->atomic		= pList->terrain;
			nw_node->sectordist	= nDistSector;

			// 2005.4.11 gemani .. 지형이 오브젝트로 Add된 경우
			if(nw_node->atomic->stType && nw_node->atomic->stType->eType & ACUOBJECT_TYPE_NO_MTEXTURE)
			{
				nw_node->next = m_listTerrainNoMTexture;
				m_listTerrainNoMTexture = nw_node;
			}
			else
			{
				if(i == ix && j == iz)
				{
					m_pShadow2Terrain_Own = nw_node;
				}
				else
				{
					nw_node->next = m_listShadow2Terrain;
					m_listShadow2Terrain = nw_node;
				}
				
				nw_node->bShadow = FALSE;

				if(m_bVertexShaderEnable)
				{
					if(m_bDrawShadow2 && ix-i < m_iShadow2Range && i-ix < m_iShadow2Range &&
						iz-j < m_iShadow2Range && j-iz < m_iShadow2Range)
					{
						nw_node->bShadow = TRUE;
					}
				}
			}

			if( type == 1 )
			{
				for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
					WorldToAtomicRender( (*Itr).atomic, nDistSector );

				for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
					WorldToAtomicRender( (*Itr).atomic, nDistSector );

				for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
					WorldToAtomicRender( (*Itr), nDistSector );

				for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
					WorldToAtomicRender( (*Itr), nDistSector );
			}
		}
	}
}

void	AgcmRender::CalcWorldBoundingSphere( RwSphere* sphere, RpAtomic* atomic )
{
	RwV3dTransformPoint( &sphere->center, &atomic->boundingSphere.center, RwFrameGetLTM( RpAtomicGetFrame(atomic) ) );
	sphere->radius = atomic->boundingSphere.radius;
}

// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
// UDA에서 BoundingSphere Set
void	AgcmRender::SetBoundingSphereFromUDA(RpClump*	clump)
{
	PROFILE("SetBoundingSphereFromUDA");

	float radius = clump->stType.boundingSphere.radius;;
	if(radius == 0.0f) return;

	int	lIndex = clump->stType.eType;
	int type = AcuObject::GetType(lIndex);
	
	// UDA에서 bounding sphere set
	if(type == ACUOBJECT_TYPE_OBJECT || type == ACUOBJECT_TYPE_CHARACTER || type == ACUOBJECT_TYPE_ITEM)
	{
		RwSphere	sphere;
		sphere.center.x = clump->stType.boundingSphere.center.x;
		sphere.center.y = clump->stType.boundingSphere.center.y;
		sphere.center.z = clump->stType.boundingSphere.center.z;
		sphere.radius = radius;

		RpClumpForAllAtomics(clump,CallBackForEnumAtomicforSetBSFromUDA,(PVOID)&sphere);
	}
}

void	AgcmRender::SetBoundingSphereFromUDA(RpAtomic*	atomic)
{
	AgcdType*	lpType = AcuObject::GetAtomicTypeStruct(atomic);
	
	float radius = lpType->boundingSphere.radius;
	if(radius == 0.0f) return;

	int	lIndex = lpType->eType;
	int type = AcuObject::GetType(lIndex);

	// UDA에서 bounding sphere set
	if(type == ACUOBJECT_TYPE_OBJECT || type == ACUOBJECT_TYPE_CHARACTER || type == ACUOBJECT_TYPE_ITEM)
	{
		RwFrame*	frame	= RpAtomicGetFrame(atomic);
		RwV3d*		pos		= RwMatrixGetPos(RwFrameGetMatrix(frame));
		RwSphere*	sphere	= RpAtomicGetBoundingSphere(atomic);

		//@{ 2006/11/21 burumal
		// UDA 정보를 이용해 atomic의 bs를 설정한다고는 하지만
		// clump의 bs값을 atomic에 설정하는것을 이해할수가 없다.
		// atomic sort의 부정확성 문제를 해결하기 위해 여기서 설정하던
		// clump의 bs정보 대신 atomic 자신의 bs값을 그대로 유지시키도록 변경했다
		// 이로인한 다른 문제가 발생할시 코드를 원상복구 시킬 예정이다
		/*
		sphere->center.x = lpType->boundingSphere.center.x - pos->x;
		sphere->center.y = lpType->boundingSphere.center.y - pos->y;
		sphere->center.z = lpType->boundingSphere.center.z - pos->z;
		sphere->radius = radius;
		*/
		sphere->center.x -= pos->x;
		sphere->center.y -= pos->y;
		sphere->center.z -= pos->z;
		//@}
	}
}

RpAtomic*	AgcmRender::CallBackForEnumAtomicforSetBSFromUDA(RpAtomic* atomic,void* data)
{
	RwSphere*	sphere_src = (RwSphere*) data;

	RwSphere*	sphere_dest = RpAtomicGetBoundingSphere(atomic);
	RwFrame*	frame = RpAtomicGetFrame(atomic);
	RwV3d*		pos = RwMatrixGetPos(RwFrameGetMatrix(frame));

	sphere_dest->center.x = sphere_src->center.x - pos->x;
	sphere_dest->center.y = sphere_src->center.y - pos->y;
	sphere_dest->center.z = sphere_src->center.z - pos->z;

	sphere_dest->radius = sphere_src->radius;

	return atomic;
}

// ■■■■■■■■■■■■■ UpdateList 관련 함수 ■■■■■■■■■■■■■■■■■■■■■■■■■■
bool	AgcmRender::AddUpdateInfotoClump(RpClump*	pClump,PVOID		pThisClass,ApModuleDefaultCallBack	pUpdate,
							ApModuleDefaultCallBack	pDestruct,PVOID	pData,PVOID	pCustData,ApModuleDefaultCallBack	pRender)
{
	PROFILE("AddUpdateInfotoClump");

	if( !pClump )
		return false;

	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);

	
	if( !pList )		
	{
		pList		=	new ListUpdateCallback;

		UpdateCallbackList*		newCallbackList = new UpdateCallbackList;

		newCallbackList->pClass			=	pThisClass;
		newCallbackList->data1			=	pData;
		newCallbackList->data2			=	pCustData;
		newCallbackList->UpdateCB		=	pUpdate;
		newCallbackList->DestructCB		=	pDestruct;
		newCallbackList->pRenderCB		=	pRender;
		newCallbackList->pSphere		=	&pClump->stType.boundingSphere;

		pClump->stType.pUpdateList = pList;
		pList->push_back( newCallbackList );

	}
	else
	{
		ListUpdateCallbackIter		Iter		=	pList->begin();
		for( ; Iter != pList->end() ; ++Iter )
		{
			UpdateCallbackList*	pUpdateCB	=	(*Iter);
			if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct && pUpdateCB->data1 == pData )
				return false;
		}

		UpdateCallbackList*		newCallbackList = new UpdateCallbackList;

		newCallbackList->pClass			=	pThisClass;
		newCallbackList->data1			=	pData;
		newCallbackList->data2			=	pCustData;
		newCallbackList->UpdateCB		=	pUpdate;
		newCallbackList->DestructCB		=	pDestruct;
		newCallbackList->pRenderCB		=	pRender;
		newCallbackList->pSphere		=	&pClump->stType.boundingSphere;

		pList->push_back( newCallbackList );
	}

	return	true;
}

bool	AgcmRender::AddUpdateInfotoAtomic(RpAtomic*	pAtomic,PVOID		pThisClass,ApModuleDefaultCallBack	pUpdate,
							ApModuleDefaultCallBack	pDestruct,PVOID	pData,PVOID	pCustData,ApModuleDefaultCallBack	pRender)
{
	PROFILE("AddUpdateInfotoAtomic");

	AgcdType*				lpType	=	AcuObject::GetAtomicTypeStruct(pAtomic);
	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(lpType->pUpdateList);
	
	if( !pList )		
	{
		UpdateCallbackList*		newCallbackList = new UpdateCallbackList;
		pList		=	new ListUpdateCallback;

		newCallbackList->pClass		= pThisClass;
		newCallbackList->data1		= pData;
		newCallbackList->data2		= pCustData;
		newCallbackList->UpdateCB	= pUpdate;
		newCallbackList->DestructCB = pDestruct;
		newCallbackList->pRenderCB	= pRender;

		lpType->pUpdateList	=	pList;
		pList->push_back( newCallbackList );
	}
	else
	{
		ListUpdateCallbackIter		Iter	=	pList->begin();
		for( ; Iter != pList->end() ; ++Iter )
		{
			UpdateCallbackList*	pUpdateCB	=	(*Iter);
			if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct && pUpdateCB->data1 == pData )
				return false;
		}

		UpdateCallbackList*		newCallbackList = new UpdateCallbackList;

		newCallbackList->pClass		= pThisClass;
		newCallbackList->data1		= pData;
		newCallbackList->data2		= pCustData;
		newCallbackList->UpdateCB	= pUpdate;
		newCallbackList->DestructCB = pDestruct;
		newCallbackList->pRenderCB	= pRender;

		pList->push_back( newCallbackList );
	}

	return	true;
}

void	AgcmRender::AtomicPreRender(RpAtomic*	atomic)				// atomic의 rendering되기 전에 처리해주는 함수
{
	PROFILE("AtomicPreRender");

	AgcdType*		lpType = AcuObject::GetAtomicTypeStruct(atomic);		// Type에 붙어 있으므로 clump나 atomic단위로 처리
	RwV3d			vPos;
	
	
	if(lpType->updateTick != m_ulCurTick && !atomic->stRenderInfo.isNowBillboard && lpType->pUpdateList )
	{
		lpType->updateTick = m_ulCurTick;

		ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(lpType->pUpdateList);
		ListUpdateCallbackIter	Iter	=	pList->begin();

		for( ; Iter != pList->end() ; )
		{
			UpdateCallbackList*	pUpdateCB		=	(*Iter);
			if( pUpdateCB->UpdateCB )
			{

				if( !pUpdateCB->UpdateCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 ) )
				{
					DEF_SAFEDELETE( (*Iter) );
					pList->erase( Iter++ );
					continue;
				}
			}

			/*
			vPos	=	atomic->worldBoundingSphere.center;

			if( pUpdateCB->pRenderCB )
				AddCustomToBlendRenderList( pUpdateCB->pClass , pUpdateCB->pRenderCB , pUpdateCB->data1 , pUpdateCB->data2 , &vPos );
				*/

			RwFrame*	pFrame	=	RpAtomicGetFrame( atomic );
			RwMatrix*	pMatrix	=	NULL;
			if( pFrame )
			{
				pMatrix		=	RwFrameGetLTM( pFrame );
				if( pMatrix )
				{
					vPos	=	pMatrix->pos;
					vPos.x	+= (*Iter)->pSphere->center.x;
					vPos.y	+= (*Iter)->pSphere->center.y;
					vPos.z	+= (*Iter)->pSphere->center.z;

					if( pUpdateCB->pRenderCB )
						AddCustomToBlendRenderList( pUpdateCB->pClass , pUpdateCB->pRenderCB , pUpdateCB->data1 , pUpdateCB->data2 , &vPos );
				}
			}

			++Iter;
		}

	}

#ifdef	USE_MFC
	int		lIndex = lpType->eType;
	int		type = AcuObject::GetType(lIndex);

	if(type == ACUOBJECT_TYPE_WORLDSECTOR && m_bDrawCollision_Terrain)
	{
		RpAtomic*	pickAtomic = (RpAtomic*)lpType->pPickAtomic;
		if( pickAtomic )	OriginalAtomicRender(pickAtomic);
	}
	else if(type == ACUOBJECT_TYPE_OBJECT && m_bDrawCollision_Object)
	{
		RpAtomic*	pickAtomic = (RpAtomic*)lpType->pPickAtomic;
		if( pickAtomic )	OriginalAtomicRender(pickAtomic);
	}
#endif
}

bool	AgcmRender::RemoveUpdateInfoFromClump( RpClump* pClump, PVOID pThisClass,ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData )
{	
	if ( !pClump || !pClump->stType.pUpdateList )	
		return false;

	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);
	ListUpdateCallbackIter	Iter	=	pList->begin();	

	for( ; Iter != pList->end() ; )
	{
		UpdateCallbackList*		pUpdateCB		=	(*Iter);
		if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct && pUpdateCB->data1 == pData )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );
			
			DEF_SAFEDELETE( pUpdateCB );
			pList->erase( Iter++ );
			continue;

		}
		++Iter;
	}

	return true;
}

bool	AgcmRender::RemoveUpdateInfoFromAtomic( RpAtomic* pAtomic, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct, PVOID pData )
{
	AgcdType* lpType = AcuObject::GetAtomicTypeStruct(pAtomic);

	if( !lpType || !lpType->pUpdateList )		
		return false;

	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(lpType->pUpdateList);
	ListUpdateCallbackIter	Iter	=	pList->begin();

	for( ; Iter != pList->end() ; )
	{
		UpdateCallbackList*	pUpdateCB	=	(*Iter);
		if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct && pUpdateCB->data1 == pData )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );

			DEF_SAFEDELETE( pUpdateCB );
			pList->erase( Iter++ );
			continue;
		}

		++Iter;
	}
	
	return true;
}

bool	AgcmRender::RemoveUpdateInfoFromClump2( RpClump* pClump, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct )
{
	CCSLog	stLog( 3004 );
	if ( !pClump || !pClump->stType.pUpdateList )		
		return false;
		
	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);
	ListUpdateCallbackIter	Iter	=	pList->begin();

	for( ; Iter != pList->end() ; )
	{
		UpdateCallbackList*		pUpdateCB		=	(*Iter);

		if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );
			
			DEF_SAFEDELETE( pUpdateCB );
			pList->erase( Iter++ );
			continue;
			
		}
		++Iter;
	}

	return true;
}

bool	AgcmRender::RemoveUpdateInfoFromAtomic2( RpAtomic* pAtomic, PVOID pThisClass, ApModuleDefaultCallBack pUpdate, ApModuleDefaultCallBack pDestruct )
{
	AgcdType* lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
	if( !lpType || !lpType->pUpdateList )		
		return false;

	ListUpdateCallback*		pUpdateCallbackList		=	static_cast< ListUpdateCallback* >(lpType->pUpdateList);
	ListUpdateCallbackIter	Iter					=	pUpdateCallbackList->begin();
	for( ; Iter != pUpdateCallbackList->end() ; )
	{
		UpdateCallbackList*		pUpdateCB	=	(*Iter);
		if( pUpdateCB->pClass == pThisClass && pUpdateCB->UpdateCB == pUpdate && pUpdateCB->DestructCB == pDestruct )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );
			
			DEF_SAFEDELETE( pUpdateCB );
			pUpdateCallbackList->erase( Iter++ );
			continue;
		}

		++Iter;
	}

	return true;
}

bool	AgcmRender::RemoveUpdateInfoFromClump3( RpClump* pClump, PVOID pThisClass )
{
	CCSLog	stLog( 3004 );
	if ( !pClump || !pClump->stType.pUpdateList )		
		return false;

	ListUpdateCallback*		pList	=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);
	ListUpdateCallbackIter	Iter	=	pList->begin();

	for( ; Iter != pList->end() ; )
	{
		UpdateCallbackList*		pUpdateCB		=	(*Iter);

		if( pUpdateCB->pClass == pThisClass )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );

			DEF_SAFEDELETE( pUpdateCB );
			pList->erase( Iter++ );
			continue;

		}
		++Iter;
	}

	return true;
}

bool	AgcmRender::RemoveUpdateinfoFromAtomic3( RpAtomic* pAtomic, PVOID pThisClass )
{
	AgcdType* lpType = AcuObject::GetAtomicTypeStruct(pAtomic);
	if( !lpType || !lpType->pUpdateList )		
		return false;

	ListUpdateCallback*		pUpdateCallbackList		=	static_cast< ListUpdateCallback* >(lpType->pUpdateList);
	ListUpdateCallbackIter	Iter					=	pUpdateCallbackList->begin();
	for( ; Iter != pUpdateCallbackList->end() ; )
	{
		UpdateCallbackList*		pUpdateCB	=	(*Iter);
		if( pUpdateCB->pClass == pThisClass )
		{
			if( pUpdateCB->DestructCB )
				pUpdateCB->DestructCB( pUpdateCB->data1 , pUpdateCB->pClass , pUpdateCB->data2 );

			DEF_SAFEDELETE( pUpdateCB );
			pUpdateCallbackList->erase( Iter++ );
			continue;
		}

		++Iter;
	}

	return true;
}

INT32 AgcmRender::StreamReadClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType)
{
	return m_csRenderType.StreamReadClumpRenderType(pcsStream, pcsType);
}

VOID AgcmRender::StreamWriteClumpRenderType(ApModuleStream *pcsStream, AgcdClumpRenderType *pcsType)
{
	m_csRenderType.StreamWriteClumpRenderType(pcsStream, pcsType);
}

CHAR *AgcmRender::GetRenderTypeName(INT32 lIndex)
{
	if((lIndex < 0) || (lIndex >= R_RENDER_TYPE_NUM))
		return NULL;

	return g_aszRenderTypeName[lIndex];
}

CHAR *AgcmRender::GetRenderBlendModeName(INT32 lIndex)
{
	if ((lIndex < 0) || (lIndex >= R_BELND_MODE_NUM))
		return NULL;

	return g_aszRenderBlendModeName[lIndex];
}

RpAtomic* AgcmRender::SetRenderTypeCB(RpAtomic *atomic, void *data)
{
	AgcdClumpRenderType *pcsType	= (AgcdClumpRenderType *)(data);
	if ( pcsType->m_lCBCount < 0 )		return NULL;

	INT32 lTemp	= pcsType->m_csRenderType.m_vecRenderType[pcsType->m_lCBCount];
	if ( !lTemp )
		lTemp = atomic->stRenderInfo.renderType ? atomic->stRenderInfo.renderType : R_NONALPHA;

	INT32 lRenderType	= lTemp & 0x0000ffff;

	INT32 lBlendMode = R_BLENDSRCAINVA_ADD;
	if ( lRenderType == R_BLEND_SORT || lRenderType == R_BLEND_NSORT )
		lBlendMode = (pcsType->m_csRenderType.m_vecRenderType[pcsType->m_lCBCount] & 0xffff0000) >> 16;

	AcuObject::SetAtomicRenderUDA(atomic, lRenderType, 0, lBlendMode, 0, 0);

	--pcsType->m_lCBCount;

	return atomic;
}

VOID AgcmRender::ClumpSetRenderType(RpClump *pstClump, AgcdClumpRenderType *pcsType)
{
	pcsType->m_lCBCount = pcsType->m_lSetCount - 1;
	RpClumpForAllAtomics(pstClump, SetRenderTypeCB, (void *)(pcsType));
}

RpAtomic* AgcmRender::SetRenderTypeCheckCustDataCB(RpAtomic *atomic, void *data)
{
	AgcdClumpRenderType *pcsType	= (AgcdClumpRenderType *)(data);

	INT32				lTemp;
	INT32				lRenderType	= 0;
	INT32				lIndex;

	for (lIndex = 0; lIndex < pcsType->m_lSetCount; ++lIndex)
	{
		if (pcsType->m_csRenderType.m_vecCustData[lIndex] == pcsType->m_plCustData[pcsType->m_lCBCount])
		{
			lTemp					= pcsType->m_csRenderType.m_vecRenderType[lIndex];
			lRenderType				= lTemp & 0x0000ffff;
			break;
		}
	}

	if ( !lRenderType )
	{
		lTemp		= atomic->stRenderInfo.renderType ? atomic->stRenderInfo.renderType : R_NONALPHA;
		lRenderType	= lTemp & 0x0000ffff;
	}

	INT32 lBlendMode = R_BLENDSRCAINVA_ADD;
	if ( lRenderType == R_BLEND_SORT || lRenderType == R_BLEND_NSORT )
		lBlendMode = (pcsType->m_csRenderType.m_vecRenderType[lIndex] & 0xffff0000) >> 16;

	AcuObject::SetAtomicRenderUDA(atomic, lRenderType, 0, lBlendMode, 0, 0);

	++pcsType->m_lCBCount;

	return atomic;
}

VOID AgcmRender::ClumpSetRenderTypeCheckCustData(RpClump *pstClump, AgcdClumpRenderType *pcsType, INT32 *plCustData)
{
	pcsType->m_lCBCount		= 0;
	pcsType->m_plCustData	= plCustData;
	RpClumpForAllAtomics(pstClump, SetRenderTypeCheckCustDataCB, (void *)(pcsType));
}

// OCTREE MODE FUNCTIONS
// 저장을 위해 userdata를 만든다.(OCTREEID 란 이름으로.. 맵툴에서만 쓰임, 클라이언트 X)
BOOL AgcmRender::MakeOctrees()
{
	INT32 iSectorLoadedCount = m_pcApmMap->GetCurrentLoadedSectorCount();
	ApWorldSector** pSectorLoadedArray = m_pcApmMap->GetCurrentLoadedSectors();

	ApWorldSector* pSector;
	SectorRenderList* pList;

	for( int i = 0 ; i < iSectorLoadedCount ; i ++ )
	{
		pSector = pSectorLoadedArray[ i ];
		if( !pSector )			continue;

		pList = GetSectorData(pSector);
		if( !pList->terrain )	continue;

		m_pAgcmOcTree->CreateRootByTerrain( pList->terrain , pSector );
	}

	// 초기화
	// 여러번 저장시 다시 계산 하도록 변경.
	for( int i = 0 ; i < iSectorLoadedCount ; i ++ )
	{
		pSector = pSectorLoadedArray[ i ];
		if( !pSector )		continue;

		pList = GetSectorData( pSector );
		if( !pList )		continue;

		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
			AcuObject::GetOcTreeID( RpAtomicGetClump( (*Itr).atomic ) )[0] = 0;

		for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
			AcuObject::GetOcTreeID( RpAtomicGetClump( (*Itr) ) )[0] = 0;

		for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
			AcuObject::GetOcTreeID( RpAtomicGetClump( (*Itr).atomic ) )[0] = 0;

		for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
			AcuObject::GetOcTreeID( RpAtomicGetClump( (*Itr) ) )[0] = 0;
	}

	for( int i = 0 ; i < iSectorLoadedCount ; i ++ )
	{
		pSector = pSectorLoadedArray[ i ];
		if( !pSector )	continue;

		pList = GetSectorData(pSector);
		if( !pList )	continue;

		for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
			if( !m_pAgcmOcTree->TestClumpOctrees( RpAtomicGetClump( (*Itr).atomic ) ) )
				return FALSE;

		for( RpAtomicListItr Itr = pList->listInMove->begin(); Itr != pList->listInMove->end(); ++Itr )
			if( !m_pAgcmOcTree->TestClumpOctrees( RpAtomicGetClump( (*Itr) ) ) )
				return FALSE;

		for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
			if( !m_pAgcmOcTree->TestClumpOctrees( RpAtomicGetClump( (*Itr).atomic ) ) )
				return FALSE;

		for( RpAtomicListItr Itr = pList->listNotInMove->begin(); Itr != pList->listNotInMove->end(); ++Itr )
			if( !m_pAgcmOcTree->TestClumpOctrees( RpAtomicGetClump( (*Itr) ) ) )
				return FALSE;
	}

	// 미리 쪼개지 않고 없으면 쪼갠다. 그러므로 최적화 불필요..

	return TRUE;
}

BOOL	AgcmRender::SaveAllOctrees(char*	szDirectory)
{
	if( !MakeOctrees() ) return FALSE;

	// 풀 디렉토리 지정 저장 관련..
	EnumCallback( AGCMRENDER_CB_OCTREE_ID_SET, (void*)szDirectory, NULL );		// octree ID 검사(custom render쓰는 풀등..)

	if( !m_pApmOcTree->SaveToFiles( szDirectory, m_iLoadRangeX1, m_iLoadRangeX2, m_iLoadRangeZ1, m_iLoadRangeZ2 ) )
		return FALSE;

	EnumCallback( AGCMRENDER_CB_OCTREE_UDA_SET, NULL, NULL );				// octree ID를 apdobject에 set해주기 위해

	return TRUE;
}

// 정적이지 않은 것들이 여기로 추가된다.. 정적인 것들은 AgcmOcTree에 추가.
void	AgcmRender::AddCustomRenderObject( PVOID pClass, RwSphere* BS, FLOAT height, ApModuleDefaultCallBack pCustUpdateCB, ApModuleDefaultCallBack pCustRenderCB, PVOID pData1, PVOID pData2, BOOL bCheck )
{
	CustomRenderList*	nw_list = new CustomRenderList;

	nw_list->BS = BS;
	nw_list->CustUpdateCB = pCustUpdateCB;
	nw_list->CustRenderCB = pCustRenderCB;
	nw_list->data1 = pData1;
	nw_list->data2 = pData2;
	nw_list->pClass = pClass;

	nw_list->height = height;
	nw_list->bCheck = bCheck;

	nw_list->next = m_listCustomRender;
	m_listCustomRender = nw_list;
}

void	AgcmRender::RemoveCustomRenderObject(PVOID pClass,PVOID pData1,PVOID pData2)
{
	CCSLog	stLog( 3005 );

	CustomRenderList*	cur_list = m_listCustomRender;
	CustomRenderList*	bef_list;
	CustomRenderList*	remove_list;

	while(cur_list)
	{
		if(cur_list->pClass == pClass && cur_list->data1 == pData1 && cur_list->data2 == pData2)
		{
			// remove
			remove_list = cur_list;

			if(cur_list == m_listCustomRender)
			{
				m_listCustomRender = cur_list->next;
			}
			else 
			{
				bef_list->next = cur_list->next;
			}

			delete remove_list;	
			remove_list = NULL;
			return;
		}

		bef_list = cur_list;
		cur_list = cur_list->next;
 	}
}

// sort할 필요 없으므로 기존 rendercallback에서 sort한다면 새로 만든다.
RpAtomic*	AgcmRender::RenderCallbackForAlphaAtomic2	(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	AlphaAtomic new_AlphaAtomic;
	ZeroMemory( &new_AlphaAtomic , sizeof(AlphaAtomic) );

	new_AlphaAtomic.atomic			=	atomic;
	new_AlphaAtomic.billboard		=	0;
	new_AlphaAtomic.SortFactor		=	m_pThisAgcmRender->_GetSortFactor( atomic );

	m_pThisAgcmRender->m_mapAlpha.insert( make_pair( new_AlphaAtomic.SortFactor , new_AlphaAtomic ) );
	
	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomic2	(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);
	int nBlendMode = atomic->stRenderInfo.blendMode;

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic = atomic;
	NewBlendAtomic.m_nBillBoardType = 0;
	NewBlendAtomic.m_fSortFactor	=	m_pThisAgcmRender->_GetSortFactor( atomic );

	m_pThisAgcmRender->m_BlendAtomics.Add( ( enumRenderBMode )nBlendMode, NewBlendAtomic );
	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForLastBlendAtomic( RpAtomic* atomic )
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	stBlendAtomic NewBlendAtomic;
	
	NewBlendAtomic.m_pAtomic = atomic;
	NewBlendAtomic.m_nBillBoardType = 0;

	if(atomic->stRenderInfo.blendMode < R_BLEND11_ADD) // sort
	{		
		RwV3d atomicpos = RwFrameGetLTM(RpAtomicGetFrame (atomic))->pos;
		RwV3dAdd(&atomicpos, &atomicpos, &atomic->boundingSphere.center);

		RwV3d screenpos;
		m_pThisAgcmRender->GetWorldPosToScreenPos(&atomicpos,&screenpos);		

		NewBlendAtomic.m_fSortFactor = screenpos.z;
		m_pThisAgcmRender->m_BlendAtomics.AddLast( NewBlendAtomic );
	}
	else
	{
		NewBlendAtomic.m_fSortFactor = 0.0f;
		m_pThisAgcmRender->m_BlendAtomics.AddLast( NewBlendAtomic );
	}

	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForAlphaAtomicBill2	(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);
	
	//AlphaList에 추가 
	AlphaAtomic	new_AlphaAtomic;
	ZeroMemory( &new_AlphaAtomic , sizeof(AlphaAtomic) );

	new_AlphaAtomic.atomic			=	atomic;
	new_AlphaAtomic.billboard		=	AcuObject::GetAtomicIsBillboard(atomic);
	new_AlphaAtomic.SortFactor		=	m_pThisAgcmRender->_GetSortFactor( atomic );

	m_pThisAgcmRender->m_mapAlpha.insert( make_pair( new_AlphaAtomic.SortFactor , new_AlphaAtomic) );
	
	return atomic;
}

RpAtomic*	AgcmRender::RenderCallbackForBlendAtomicBill2	(RpAtomic*	atomic)
{
	m_pThisAgcmRender->AtomicPreRender(atomic);

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic			= atomic;
	NewBlendAtomic.m_nBillBoardType		= AcuObject::GetAtomicIsBillboard( atomic );
	NewBlendAtomic.m_fSortFactor		= m_pThisAgcmRender->_GetSortFactor( atomic );
	
	int bmode = atomic->stRenderInfo.blendMode;
	m_pThisAgcmRender->m_BlendAtomics.Add( ( enumRenderBMode )bmode, NewBlendAtomic );
	
	return atomic;
}

FLOAT	AgcmRender::_GetSortFactor( RpAtomic* pAtomic )
{
	if( !pAtomic )
		return 0;

	RwFrame*	pFrame	=	RpAtomicGetFrame( pAtomic );
	RwMatrix*	pMatrix	=	NULL;
	if( !pFrame )
		return 0;

	pMatrix		=	RwFrameGetLTM( pFrame );
	if( !pMatrix )
		return 0;

	RwV3d atomicpos = pMatrix->pos;
	RwV3d screenpos;

	//RwV3dAdd(&atomicpos, &atomicpos, &pAtomic->boundingSphere.center);
	GetWorldPosToScreenPos(&atomicpos,&screenpos);		

	return screenpos.z;
}

void	AgcmRender::InsertPreListOcTree(RpAtomic* atomiclist,RpClump*	pClumpArg)
{
	INT32	i;

	PROFILE("InsertPreListOcTree");

	PreRender*	add_node = (PreRender*) AcuFrameMemory::AllocFrameMemory(sizeof(PreRender));

	ZeroMemory( add_node , sizeof(PreRender) );
	if (!add_node)
		return;

	add_node->TestType = PRE_TEST_NORMAL;
	add_node->atomicList = atomiclist;

	OcTreeRenderData*	pData = (OcTreeRenderData*)pClumpArg->stType.pOcTreeData;
	RwV3d	screen_pos;

	if(!pData) return;

	// A temporary expedient...--
	if(IsBadReadPtr(pData, sizeof(OcTreeRenderData)))
		return;

	if( pClumpArg->ulFlag & RWFLAG_DONOT_CULL )
	{
		add_node->maxBBox[0] = DO_NOT_CULL_MBBOX_VALUE;
		add_node->maxBBox[1] = DO_NOT_CULL_MBBOX_VALUE;
		add_node->maxBBox[2] = DO_NOT_CULL_MBBOX_VALUE;
		add_node->maxBBox[3] = DO_NOT_CULL_MBBOX_VALUE;
		add_node->isOccluder = FALSE;
		add_node->nOccluder = 0;
		add_node->arrayOccluder = NULL;
	}
	else
	if(pData->bMove)
	{
		RpClump*	pClump = RpAtomicGetClump(atomiclist);

		RwV3d		calcVerts[4];
		RwMatrix*	matrix = NULL;
		if(pClump)
		{
			RwFrame*	frame = RpClumpGetFrame(pClump);

			matrix = RwFrameGetLTM(frame);

			RwV3dTransformPoints(calcVerts,pData->topVerts_MAX, 4,matrix);
		}
		else
		{
			memcpy(calcVerts, pData->topVerts_MAX, sizeof(calcVerts));
		}

		GetWorldPosToScreenPos(&calcVerts[0],&screen_pos);

		add_node->maxBBox[0] = screen_pos.x;
		add_node->maxBBox[1] = screen_pos.x;
		add_node->maxBBox[2] = screen_pos.y;
		add_node->maxBBox[3] = screen_pos.z;
		add_node->dOnXZ = GetCameraDistanceOnXZ(calcVerts[0]);

		for(i=1;i<4;++i)
		{
			GetWorldPosToScreenPos(&calcVerts[i],&screen_pos);

			if(screen_pos.x < add_node->maxBBox[0])
			{
				add_node->maxBBox[0] = screen_pos.x;
			}

			if(screen_pos.x > add_node->maxBBox[1])
			{
				add_node->maxBBox[1] = screen_pos.x;
			}

			if(screen_pos.y < add_node->maxBBox[2])
			{
				add_node->maxBBox[2] = screen_pos.y;
			}

			if(screen_pos.z < add_node->maxBBox[3])
			{
				add_node->maxBBox[3] = screen_pos.z;
			}

			RwReal dOnXZ = GetCameraDistanceOnXZ(calcVerts[i]);
			if(dOnXZ > add_node->dOnXZ)
				add_node->dOnXZ = dOnXZ;
		}

		if(pData->isOccluder && m_fPanAngle < 26.0f && m_fPanAngle > -20.0f) // 카메라가 높은 곳에서 아래를 쳐다볼경우 성벽 같은데서 지평선 구성시 오류가 생길수 있다
		{
			add_node->isOccluder = TRUE;
			add_node->arrayOccluder = (FLOAT*) AcuFrameMemory::AllocFrameMemory(12*pData->nOccluderBox); 
			if( add_node->arrayOccluder )
			{
				BOOL	bNear = FALSE;
				int		oc_index = 0;
				int		oc_num = 0;
				FLOAT	miny,maxy;

				for(i=0;i<pData->nOccluderBox;++i)
				{
					if(pClump)
					{
						RwV3dTransformPoints(calcVerts,&pData->pTopVerts[i*4], 4,matrix);
					}
					else
					{
						memcpy(calcVerts, &pData->pTopVerts[i*4], sizeof(calcVerts));
					}

					if(!GetWorldPosToScreenPos(&calcVerts[0],&screen_pos))
					{
						continue;
					}

					add_node->arrayOccluder[oc_index+0] = screen_pos.x;
					add_node->arrayOccluder[oc_index+1] = screen_pos.x;
					miny = maxy = screen_pos.y;
					
					for(int j=1;j<4;++j)
					{
						if(!GetWorldPosToScreenPos(&calcVerts[i],&screen_pos))
						{
							bNear = TRUE;
							break;
						}

						if(screen_pos.x < add_node->arrayOccluder[oc_index+0])
						{
							add_node->arrayOccluder[oc_index+0] = screen_pos.x;
						}

						if(screen_pos.x > add_node->arrayOccluder[oc_index+1])
						{
							add_node->arrayOccluder[oc_index+1] = screen_pos.x;
						}

						if(screen_pos.y < miny)
						{
							miny = screen_pos.y;
						}
						else if(screen_pos.y > maxy)
						{
							maxy = screen_pos.y;
						}
					}

					if(!bNear)
					{
						add_node->arrayOccluder[oc_index+2] = (miny + maxy) * 0.5f;

						++oc_num;
						oc_index += 3;
					}
				}
				add_node->nOccluder = oc_num;
			}
		}
		else
		{
			add_node->isOccluder = FALSE;
			add_node->nOccluder = 0;
			add_node->arrayOccluder = NULL;
		}
	}
	else
	{
		GetWorldPosToScreenPos(&pData->topVerts_MAX[0],&screen_pos);

		add_node->maxBBox[0] = screen_pos.x;
		add_node->maxBBox[1] = screen_pos.x;
		add_node->maxBBox[2] = screen_pos.y;
		add_node->maxBBox[3] = screen_pos.z;
		add_node->dOnXZ = GetCameraDistanceOnXZ(pData->topVerts_MAX[0]);

		int i = 1;
		for(i;i<4;++i)
		{
			GetWorldPosToScreenPos(&pData->topVerts_MAX[i],&screen_pos);

			if(screen_pos.x < add_node->maxBBox[0])
			{
				add_node->maxBBox[0] = screen_pos.x;
			}

			if(screen_pos.x > add_node->maxBBox[1])
			{
				add_node->maxBBox[1] = screen_pos.x;
			}

			if(screen_pos.y < add_node->maxBBox[2])
			{
				add_node->maxBBox[2] = screen_pos.y;
			}

			if(screen_pos.z < add_node->maxBBox[3])
			{
				add_node->maxBBox[3] = screen_pos.z;
			}

			RwReal dOnXZ = GetCameraDistanceOnXZ(pData->topVerts_MAX[i]);
			if(dOnXZ > add_node->dOnXZ)
				add_node->dOnXZ = dOnXZ;
		}

		if(pData->isOccluder && m_fPanAngle < 26.0f && m_fPanAngle > -20.0f && add_node->maxBBox[3] > 0.0f
			&& pData->pTopVerts) // 카메라가 높은 곳에서 아래를 쳐다볼경우 성벽 같은데서 지평선 구성시 오류가 생길수 있다)
		{
			add_node->isOccluder = TRUE;
			add_node->arrayOccluder = (FLOAT*) AcuFrameMemory::AllocFrameMemory(12*pData->nOccluderBox); 

			if( add_node->arrayOccluder )
			{
				BOOL	bNear = FALSE;
				int		oc_index = 0;
				int		oc_num = 0;
				FLOAT	miny,maxy;

				RwV3d*	pTopVerts = pData->pTopVerts;
				
				for(i=0;i<pData->nOccluderBox;++i)
				{
					int		iTopVertsIndex = i*4;
					if(!GetWorldPosToScreenPos(&pTopVerts[iTopVertsIndex],&screen_pos))
					{
						continue;
					}

					add_node->arrayOccluder[oc_index+0] = screen_pos.x;
					add_node->arrayOccluder[oc_index+1] = screen_pos.x;
					miny = maxy = screen_pos.y;

					if(screen_pos.z > add_node->maxBBox[3])
					{
						add_node->maxBBox[3] = screen_pos.z;
					}

					RwReal dOnXZ = GetCameraDistanceOnXZ(pTopVerts[iTopVertsIndex]);
					if(dOnXZ > add_node->dOnXZ)
						add_node->dOnXZ = dOnXZ;
					++iTopVertsIndex;

					for(int j=1;j<4;++j,++iTopVertsIndex)
					{
						if(!GetWorldPosToScreenPos(&pTopVerts[iTopVertsIndex],&screen_pos))
						{
							bNear = TRUE;
							break;
						}

						if(screen_pos.x < add_node->arrayOccluder[oc_index+0])
						{
							add_node->arrayOccluder[oc_index+0] = screen_pos.x;
						}

						if(screen_pos.x > add_node->arrayOccluder[oc_index+1])
						{
							add_node->arrayOccluder[oc_index+1] = screen_pos.x;
						}

						if(screen_pos.y < miny)
						{
							miny = screen_pos.y;
						}
						else if(screen_pos.y > maxy)
						{
							maxy = screen_pos.y;
						}

						if(screen_pos.z > add_node->maxBBox[3])
						{
							add_node->maxBBox[3] = screen_pos.z;
						}

						dOnXZ = GetCameraDistanceOnXZ(pTopVerts[iTopVertsIndex]);
						if(dOnXZ > add_node->dOnXZ)
							add_node->dOnXZ = dOnXZ;
					}

					if(!bNear)
					{
						add_node->arrayOccluder[oc_index+2] = (miny + maxy) * 0.5f;

						++oc_num;
						oc_index += 3;
					}
				}
				add_node->nOccluder = oc_num;
			}
		}
		else
		{
			add_node->isOccluder = FALSE;
			add_node->nOccluder = 0;
			add_node->arrayOccluder = NULL;
		}
	}

	m_priorityQueuePreRender.push(add_node);
	INT32 zindex = (INT32)(add_node->maxBBox[3] * OCTREE_Z_SCALE);
	if(zindex >= 10) zindex = 9;
	else if(zindex < 0)	 zindex = 0;

	if(atomiclist)
		SetDistance(atomiclist,zindex);
}

void	AgcmRender::InsertPreListWorld(WorldAddClumpGroup*		pClumpGroup)
{
	PROFILE("InsertPreListWorld");
	PreRender*	add_node = (PreRender*) AcuFrameMemory::AllocFrameMemory(sizeof(PreRender));
	//@} Jaewon

	ZeroMemory( add_node , sizeof(PreRender) );

	ASSERT( add_node && "FrameMemory Allocation Error!!!");
	if (!add_node)
		return;

	add_node->TestType = PRE_TEST_NORMAL;
	if(pClumpGroup->pClump)
	{	
		ASSERT(pClumpGroup->Atomiclist == NULL);
		add_node->atomicList = pClumpGroup->pClump->atomicList;
	}
	else
	{
		ASSERT(pClumpGroup->Atomiclist != NULL);
		add_node->atomicList = pClumpGroup->Atomiclist;
	}
	//@} Jaewon

	RwV3d	screen_pos;

	RwV3d		calcVerts[4];
	RwFrame*	frame;

	if(pClumpGroup->pClump)
		frame = RpClumpGetFrame(pClumpGroup->pClump);
	else 
		frame = RpAtomicGetFrame(pClumpGroup->Atomiclist);

	RwMatrix*	matrix = RwFrameGetLTM(frame);

	// 일단 atomic의 matrix로 최대 bbox변환 하자
	RwV3dTransformPoints(calcVerts, pClumpGroup->MaxBBox, 4,matrix);

	GetWorldPosToScreenPos(&calcVerts[0],&screen_pos);

	add_node->maxBBox[0] = screen_pos.x;
	add_node->maxBBox[1] = screen_pos.x;
	add_node->maxBBox[2] = screen_pos.y;
	add_node->maxBBox[3] = screen_pos.z;
	//@{ Jaewon 20050714
	// ;)
	add_node->dOnXZ = GetCameraDistanceOnXZ(calcVerts[0]);
	//@} Jaewon

	for(int i=1;i<4;++i)
	{
		GetWorldPosToScreenPos(&calcVerts[i],&screen_pos);

		if(screen_pos.x < add_node->maxBBox[0])
		{
			add_node->maxBBox[0] = screen_pos.x;
		}

		if(screen_pos.x > add_node->maxBBox[1])
		{
			add_node->maxBBox[1] = screen_pos.x;
		}

		if(screen_pos.y < add_node->maxBBox[2])
		{
			add_node->maxBBox[2] = screen_pos.y;
		}

		// World objects cannot be an occluder,
		// so it should take a near z conservatively.
		if(screen_pos.z < add_node->maxBBox[3])
		{
			add_node->maxBBox[3] = screen_pos.z;
		}

		RwReal dOnXZ = GetCameraDistanceOnXZ(calcVerts[i]);
		if(dOnXZ < add_node->dOnXZ)
			add_node->dOnXZ = dOnXZ;
	}

	add_node->isOccluder = FALSE;
	add_node->nOccluder = 0;
	add_node->arrayOccluder = NULL;

	m_priorityQueuePreRender.push(add_node);
	INT32 zindex = (INT32)(add_node->maxBBox[3] * OCTREE_Z_SCALE);
	if(zindex >= 10) zindex = 9;
	else if(zindex < 0)	 zindex = 0;

	if(pClumpGroup->pClump && pClumpGroup->pClump->atomicList)
		SetDistance(pClumpGroup->pClump->atomicList,zindex);
}

void	AgcmRender::InsertPreListCustom( OcCustomDataList*	customdata )
{
	PROFILE("InsertPreListCustom");
	RwV3d				screen_pos;
	int					i,zindex;
		
	// PreRenderList -> std::priority_queue<PreRender>
	PreRender*	add_pr = (PreRender*) AcuFrameMemory::AllocFrameMemory(sizeof(PreRender));

	ZeroMemory( add_pr , sizeof(PreRender) );

	ASSERT( add_pr && "FrameMemory Allocation Error!!!");
	if (!add_pr)
		return;

	add_pr->TestType = PRE_TEST_CUSTOM;
	
	// OcClumpAtomicList -> RpAtomic
	add_pr->atomicList = (RpAtomic*)customdata->pClass;

	add_pr->isOccluder = (BOOL)customdata->pUpdateCB;
	add_pr->nOccluder = (INT32)customdata->pData1;
	add_pr->arrayOccluder = (FLOAT*)customdata->pData2;
	add_pr->reserve1 = (PVOID)customdata->pRenderCB;	
	add_pr->Sphere	=	customdata->BS;

	GetWorldPosToScreenPos(&customdata->TopVerts[0],&screen_pos);

	add_pr->maxBBox[0] = screen_pos.x;
	add_pr->maxBBox[1] = screen_pos.x;
	add_pr->maxBBox[2] = screen_pos.y;
	add_pr->maxBBox[3] = screen_pos.z;
	add_pr->dOnXZ = GetCameraDistanceOnXZ(customdata->TopVerts[0]);

	if ( customdata->pDistCorrectCB )
		customdata->pDistCorrectCB((PVOID)&add_pr->dOnXZ, NULL, NULL);

	for(i=1;i<4;++i)
	{
		GetWorldPosToScreenPos(&customdata->TopVerts[i],&screen_pos);

		if(screen_pos.x < add_pr->maxBBox[0])
		{
			add_pr->maxBBox[0] = screen_pos.x;
		}

		if(screen_pos.x > add_pr->maxBBox[1])
		{
			add_pr->maxBBox[1] = screen_pos.x;
		}

		if(screen_pos.y < add_pr->maxBBox[2])
		{
			add_pr->maxBBox[2] = screen_pos.y;
		}

		// Custom render objects cannot be an occluder,
		// so it should take a near z conservatively.
		if(screen_pos.z < add_pr->maxBBox[3])
		{
			add_pr->maxBBox[3] = screen_pos.z;
		}

		FLOAT dOnXZ = GetCameraDistanceOnXZ(customdata->TopVerts[i]);
		
		if ( customdata->pDistCorrectCB )
			customdata->pDistCorrectCB((PVOID)&dOnXZ, NULL, NULL);

		if(dOnXZ < add_pr->dOnXZ)
			add_pr->dOnXZ = dOnXZ;
	}

	m_priorityQueuePreRender.push(add_pr);
	zindex = (INT32)(add_pr->maxBBox[3] * OCTREE_Z_SCALE);
	if(zindex >= 10) zindex = 9;
	else if(zindex < 0)	 zindex = 0;

	if(customdata->piCameraZIndex)
		*customdata->piCameraZIndex = zindex;
}

BOOL	AgcmRender::GetWorldPosToScreenPos		( RwV3d * pWorldPos ,RwV3d* pScreenPos)
{
	//#ifdef _DEBUG
	if( NULL == pWorldPos )
	{
		return FALSE;
	}
	//#endif //_DEBUG

	//@{ 2006/11/30 burumal
	if ( NULL == m_pCamera )
		return FALSE;
	//@}

	RwMatrix	*viewMatrix = RwCameraGetViewMatrix(m_pCamera);
	RwV3d		CameraPos;
	RwV3dTransformPoint(&CameraPos, pWorldPos, viewMatrix);

	float recipZ = 1.0f/CameraPos.z;

	pScreenPos->x = CameraPos.x * recipZ * m_iCameraRasterWidth + m_pCamera->viewOffset.x;
	pScreenPos->y = CameraPos.y * recipZ * m_iCameraRasterHeight + m_pCamera->viewOffset.y;
	pScreenPos->z = (CameraPos.z - m_pCamera->nearPlane)/m_pCamera->farPlane;

	if(CameraPos.z <= m_pCamera->nearPlane)
	{
		return FALSE;
	}
	
	return TRUE;
}

void	AgcmRender::UpdateOBufferAndDrawTest(AgcmRenderTestRenderAtomicList		pRenderFunc,BOOL bCustProcess)
{
	PROFILE("UpdateOBufferAndDrawTest");
	int i,ocindex;

	// Occluder Line List Init
	INT32	iCameraMaxHeight = m_iCameraRasterHeight << 1;
	m_listOccluderLine = NULL;
	for(i=0;i<_ocLineMax;++i)
	{
		m_pStartOcLine[i] = NULL;
		m_iMaxHeightOcLine[i] = iCameraMaxHeight;			// 카메라 위로 올려도 아래것이 나오도록 설정..
	}

	OccluderLineList*	add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));

	ASSERT( add_Onode && "FrameMemory Allocation Error!!!");
	if (!add_Onode)
		return;

	add_Onode->startx = 0;
	add_Onode->endx = m_iCameraRasterWidth;
	add_Onode->y = iCameraMaxHeight;
	add_Onode->next = NULL;
	m_listOccluderLine = add_Onode;
	m_pStartOcLine[0] = add_Onode;

	PreRender *cur_node;
	
	while(!m_priorityQueuePreRender.empty())
	{
		cur_node = m_priorityQueuePreRender.top();
		m_priorityQueuePreRender.pop();


		if(cur_node->TestType == PRE_TEST_NORMAL)
		{
			// occluder 영역에 가려지는지 체크
			if(cur_node->maxBBox[3] < 0 || cur_node->maxBBox[0] == DO_NOT_CULL_MBBOX_VALUE )
			{
				pRenderFunc(cur_node->atomicList);
			}
			else if(!IsExcept(cur_node->maxBBox[0],cur_node->maxBBox[1],cur_node->maxBBox[2]))
			{
				pRenderFunc(cur_node->atomicList);
				// Occluder 영역 update
				if(cur_node->isOccluder)
				{
					ocindex = 0;
					for(i=0;i<cur_node->nOccluder;++i)
					{
						SetOccluderLine(cur_node->arrayOccluder[ocindex],cur_node->arrayOccluder[ocindex+1],
							cur_node->arrayOccluder[ocindex+2]);	

						ocindex += 3;
					}
				}
			}

		}
		else if(cur_node->TestType == PRE_TEST_NOT_TEST)		// 지형이 이리로 들어온다.
		{
			// Occluder 영역 update
			if(cur_node->isOccluder)
			{
				ocindex = 0;
				for(i=0;i<cur_node->nOccluder;++i)
				{
					SetOccluderLine(cur_node->arrayOccluder[ocindex],cur_node->arrayOccluder[ocindex+1],
						cur_node->arrayOccluder[ocindex+2]);

					ocindex += 3;
				}
			}
		}
		else if(bCustProcess && cur_node->TestType == PRE_TEST_CUSTOM)
		{
			if(!IsExcept(cur_node->maxBBox[0],cur_node->maxBBox[1],cur_node->maxBBox[2]))
			{
				// 밑의 인자들은 custom 모드시엔 용도가 다 틀림..(형변환되는 것을 보고 짐작하기 ^^)
				if(cur_node->isOccluder)
				{
					((ApModuleDefaultCallBack)cur_node->isOccluder)((PVOID)cur_node->nOccluder,(PVOID)cur_node->atomicList,
						(PVOID)cur_node->arrayOccluder);
				}

				AddCustomToBlendRenderList(
					(PVOID)cur_node->atomicList,
					(ApModuleDefaultCallBack)cur_node->reserve1,
					(PVOID)cur_node->nOccluder,
					(PVOID)cur_node->arrayOccluder,
					&cur_node->Sphere.center);

			}

		}
		else if(bCustProcess && cur_node->TestType == PRE_TEST_CUSTOM_NOT_TEST)
		{
			// 밑의 인자들은 custom 모드시엔 용도가 다 틀림..(형변환되는 것을 보고 짐작하기 ^^)
			if(cur_node->isOccluder)
			{
				((ApModuleDefaultCallBack)cur_node->isOccluder)((PVOID)cur_node->nOccluder,(PVOID)cur_node->atomicList,
					(PVOID)cur_node->arrayOccluder);
			}

			AddCustomToBlendRenderList(
				(PVOID)cur_node->atomicList,
				(ApModuleDefaultCallBack)cur_node->reserve1,
				(PVOID)cur_node->nOccluder,
				(PVOID)cur_node->arrayOccluder,
				&cur_node->Sphere.center);
		}
	}
	//@} Jaewon
}

void	AgcmRender::UpdateOBufferAndDrawNotTest(AgcmRenderTestRenderAtomicList		pRenderFunc,BOOL bCustProcess)
{
	int i,ocindex;

	// Occluder Line List Init
	m_listOccluderLine = NULL;
	for(i=0;i<_ocLineMax;++i)
	{
		m_pStartOcLine[i] = NULL;
		m_iMaxHeightOcLine[i] = m_iCameraRasterHeight;
	}

	//@{ Jaewon 20050714
	// PreRenderList -> std::priority_queue<PreRender>
	PreRender *cur_node = NULL;
	while(!m_priorityQueuePreRender.empty())
	{
		cur_node = m_priorityQueuePreRender.top();
		m_priorityQueuePreRender.pop();

		if(cur_node->TestType == PRE_TEST_NORMAL)
		{
			pRenderFunc(cur_node->atomicList);
		}
		else if(cur_node->TestType == PRE_TEST_NOT_TEST)		// 지형이 이리로 들어온다.
		{
			// Occluder 영역 update
			if(cur_node->isOccluder)
			{
				ocindex = 0;
				for(i=0;i<cur_node->nOccluder;++i)
				{
					SetOccluderLine(cur_node->arrayOccluder[ocindex],cur_node->arrayOccluder[ocindex+1],
						cur_node->arrayOccluder[ocindex+2]);

					ocindex += 3;
				}
			}
		}
		else if(bCustProcess && cur_node->TestType == PRE_TEST_CUSTOM)
		{
			// 밑의 인자들은 custom 모드시엔 용도가 다 틀림..(형변환되는 것을 보고 짐작하기 ^^)
			if(cur_node->isOccluder)
			{
				((ApModuleDefaultCallBack)cur_node->isOccluder)((PVOID)cur_node->nOccluder,(PVOID)cur_node->atomicList,
					(PVOID)cur_node->arrayOccluder);
			}

			AddCustomToBlendRenderList(
				(PVOID)cur_node->atomicList,
				(ApModuleDefaultCallBack)cur_node->reserve1,
				(PVOID)cur_node->nOccluder,
				(PVOID)cur_node->arrayOccluder,
				&cur_node->Sphere.center);
		}
		else if(bCustProcess && cur_node->TestType == PRE_TEST_CUSTOM_NOT_TEST)
		{
			// 밑의 인자들은 custom 모드시엔 용도가 다 틀림..(형변환되는 것을 보고 짐작하기 ^^)
			if(cur_node->isOccluder)
			{
				((ApModuleDefaultCallBack)cur_node->isOccluder)((PVOID)cur_node->nOccluder,(PVOID)cur_node->atomicList,
					(PVOID)cur_node->arrayOccluder);
			}

			AddCustomToBlendRenderList(
				(PVOID)cur_node->atomicList,
				(ApModuleDefaultCallBack)cur_node->reserve1,
				(PVOID)cur_node->nOccluder,
				(PVOID)cur_node->arrayOccluder,
				&cur_node->Sphere.center);
		}
	}
	//@} Jaewon
}

BOOL	AgcmRender::IsExcept(INT32 sx,INT32 ex,INT32 y)
{
	PROFILE("IsExcept");
	if(y > (m_iCameraRasterHeight << 1)) return TRUE;
		
	if(ex < 0) 
	{
		sx = 0;
		ex = 1;
	}
	else if(sx >= m_iCameraRasterWidth)
	{
		sx = m_iCameraRasterWidth - 1;
		ex = m_iCameraRasterWidth;
	}
	else
	{
		if(sx < 0) sx = 0;
		if(ex > m_iCameraRasterWidth) ex = m_iCameraRasterWidth;
		if(sx == ex) ex += 1;
	}
		
	INT32		sindex = sx/100;
	INT32		eindex = ex/100;
	INT32		i;

	// 옥트리 라인체크 변수가 하드 코딩 돼어있다.
	if( sindex > _ocLineMax ) sindex = _ocLineMax;
	if( eindex > _ocLineMax ) eindex = _ocLineMax;

	// Max Height Table로 미리 검사
	for(i=sindex;i<=eindex;++i)
	{
		if(m_iMaxHeightOcLine[i] > y)
			return FALSE;
	}

	OccluderLineList*	check_node;
	for(i=sindex;i>=0;--i)
	{
		if(m_pStartOcLine[i] && m_pStartOcLine[i]->startx <= sx) 
		{
			check_node = m_pStartOcLine[i];
			break;
		}
	}

	while(check_node)
	{
		if(check_node->endx > sx && check_node->startx < ex)
		{
			if(check_node->y > y)
			{
				return FALSE;
			}
		}
		else if(check_node->startx >= ex)
		{
			break;
		}

		check_node = check_node->next;
	}

	return TRUE;
}

void	AgcmRender::OcTreeModeRender(RwCamera*	pCheckCamera,AgcmRenderTestRenderTerrain	pTerrainFunc,BOOL bWorldAdded)
{
	RwMatrix*	mtm = RwFrameGetLTM(m_pFrame);
	RwV3d*		pos = RwMatrixGetPos(mtm);

	ApWorldSector*	pSector = m_pcApmMap->GetSector(pos->x,pos->z);
	if(pSector == NULL) return;

	INT32	ix = pSector->GetArrayIndexX();
	INT32	iz = pSector->GetArrayIndexZ();

	INT32	sx = ix-m_iDrawRange;
	INT32	ex = ix+m_iDrawRange;
	INT32	sz = iz-m_iDrawRange;
	INT32	ez = iz+m_iDrawRange;	

	int		i,j,k;

	OcTreeRoot*			root;
	OcTreeRootList*		root_list;
	OcRootData*			root_data;
	RwSphere*			sphere;
	RwFrustumTestResult  res;

	//@{ Jaewon 20050714
	// PreRenderList -> std::priority_queue<PreRender>
	PreRender*			add_terrain;

	INT32				nDistSector;

	RwV3d				world_pos;
	RwV3d				screen_pos;

	RwV3d				terrain_at;	
	INT32				terrain_OBoxindex;

	//OcClumpAtomicList*		nw_atomiclist;
	//INT32*				uda_type;

	//Shadow2TerrainList* nw_node;

	RwV3d				at = {0.0f,0.0f,1.0f};
	RwMatrix			mat;
	RwMatrixRotate  (&mat,&m_vYAxis,m_fRotAngle,rwCOMBINEREPLACE);
	RwV3dTransformPoint(&at,&at,&mat);

	RwV3dScale(&terrain_at,&at,6400.0f);

	AuAutoLock	lock(m_pApmOcTree->m_csCSection);
	if (!lock.Result()) return;

	for(i=sx;i<ex;++i)
	{
		if(i<0) continue;
		for(j=sz;j<ez;++j)
		{
			if(j<0) continue;

			root = m_pApmOcTree->GetRoot(i,j);
			if(!root) continue;

			nDistSector = (INT32)sqrt((FLOAT)((ix-i)*(ix-i) + (iz-j)*(iz-j)));

			root_data = m_pAgcmOcTree->GetRootData(root);
			
			// 지형 add검사
			if(!root_data->terrain) continue;
			sphere = RpAtomicGetBoundingSphere( root_data->terrain );
					
			res = RwCameraFrustumTestSphere( pCheckCamera, sphere );

			if(res != rwSPHEREOUTSIDE )
			{
				pTerrainFunc(root_data->terrain, nDistSector );

				//@{ Jaewon 20050714
				// PreRenderList -> std::priority_queue<PreRender>
				add_terrain = (PreRender*) AcuFrameMemory::AllocFrameMemory(sizeof(PreRender));
				ZeroMemory( add_terrain , sizeof(PreRender) );
				if( add_terrain )
				{
					//@} Jaewon

					//@{ Jaewon 20050608
					// OcClumpAtomicList -> RpAtomic
					//nw_atomiclist = (OcClumpAtomicList*) AcuFrameMemory::AllocFrameMemory(OC_CLUMP_ATOMIC_LIST_SIZE);
					//nw_atomiclist->atomic = root_data->terrain;
					////@{ Jaewon 20050602
					//// Just use atomic->renderCallBack.
					////nw_atomiclist->renderCB = RpAtomicGetRenderCallBack(root_data->terrain);
					////@} Jaewon
					//nw_atomiclist->next = NULL;

					add_terrain->atomicList = root_data->terrain;
					ASSERT(root_data->terrain->next == NULL);
					//@} Jaewon
					add_terrain->TestType = PRE_TEST_NOT_TEST;

					if(m_fPanAngle < 70.0f && m_fPanAngle > -25.0f && m_bOcLineCheck && nDistSector < 4)
					{
						add_terrain->isOccluder = TRUE;
						
						add_terrain->arrayOccluder = (FLOAT*) AcuFrameMemory::AllocFrameMemory(768); // 12 * quad tree node수(64) = 1024
						if( add_terrain->arrayOccluder )
						{
							terrain_OBoxindex = 0;
							// Occluder data set
							for(k=0;k<4;++k)
							{
								SetTerrainOccluderBoxs(root_data->child[k],add_terrain->arrayOccluder,&terrain_OBoxindex);
							}

							add_terrain->nOccluder = terrain_OBoxindex/3;
						}
					}
					else
					{
						add_terrain->isOccluder = FALSE;
					}

					RwV3dAdd(&world_pos,&sphere->center,&terrain_at);

					GetWorldPosToScreenPos(&world_pos,&screen_pos);
					add_terrain->maxBBox[3] = screen_pos.z;

					//@{ Jaewon 20050714
					// PreRenderList -> std::priority_queue<PreRender>
					add_terrain->dOnXZ = GetCameraDistanceOnXZ(world_pos);
					m_priorityQueuePreRender.push(add_terrain);
					//@} Jaewon					
				}
			}

			root_list = root->roots;
			while(root_list)
			{
				// objects 검사
				OcTreeChildRender(root_list->node,TRUE,nDistSector,pCheckCamera);
				root_list = root_list->next;
			}
		}
	}

	// RenderWorld List 검사
	if(bWorldAdded)
	{
		WorldAddClumpGroup*		cur_wc;
		RwFrame*				frame;
		RwMatrix*				matrix;
		RpAtomic*				test_atomic;

		cur_wc = m_listWorldClumps;
		while(cur_wc)
		{
			if(cur_wc->pClump)
			{
				ASSERT(cur_wc->Atomiclist == NULL);

				frame  = RpClumpGetFrame(cur_wc->pClump);
			}
			else
			{
				ASSERT(cur_wc->Atomiclist != NULL);

				// cur_wc->A^tomiclist->atomic -> cur_wc->Atomiclist
				test_atomic = cur_wc->Atomiclist;

				frame  = RpAtomicGetFrame(test_atomic);
				if(!frame)
				{
					cur_wc = cur_wc->next;
					continue;
				}

				else
				{					
					if ( FindFromRwFrameMapTable(frame) == FALSE )
					{
						cur_wc = cur_wc->next;
						continue;
					}
				}
			}

			matrix = RwFrameGetLTM(frame);

			RwV3dTransformPoint(&cur_wc->WorldBS.center,&cur_wc->BS.center,matrix);
			res = RwCameraFrustumTestSphere( pCheckCamera, &cur_wc->WorldBS );
			
			if(res != rwSPHEREOUTSIDE || (cur_wc->pClump && cur_wc->pClump->ulFlag & RWFLAG_DONOT_CULL))
			{
				InsertPreListWorld(cur_wc);
			}

			cur_wc = cur_wc->next;
		}
	}

	// Custom Render List검사 (maincamera만 지원.. 즉시 그려야 하기 때문에..현재로서는..)
	CustomRenderList*	cur_cr = m_listCustomRender;
	//@{ Jaewon 20050714
	// PreRenderList -> std::priority_queue<PreRender>
	PreRender *add_pr;
	//@} Jaewon
	
	RwV3d		calcVerts[4];
	float		cvy;

	while(cur_cr)
	{
		res = RwCameraFrustumTestSphere( m_pCamera, cur_cr->BS );

		if(res != rwSPHEREOUTSIDE)
		{
			//@{ Jaewon 20050714
			// PreRenderList -> std::priority_queue<PreRender>
			add_pr = (PreRender*) AcuFrameMemory::AllocFrameMemory(sizeof(PreRender));
			ZeroMemory( add_pr , sizeof(PreRender) );
			if( add_pr )
			{
				//@} Jaewon

				if( cur_cr->bCheck)	add_pr->TestType = PRE_TEST_CUSTOM;
				else add_pr->TestType = PRE_TEST_CUSTOM_NOT_TEST;

				//@{ Jaewon 20050608
				// OcClumpAtomicList -> RpAtomic
				add_pr->atomicList = (RpAtomic*)cur_cr->pClass;
				//@} Jaewon

				add_pr->isOccluder = (BOOL)cur_cr->CustUpdateCB;
				add_pr->nOccluder = (INT32)cur_cr->data1;
				add_pr->arrayOccluder = (FLOAT*)cur_cr->data2;
				add_pr->reserve1 = (PVOID)cur_cr->CustRenderCB;
				add_pr->Sphere	=	*(cur_cr->BS);

				cvy = cur_cr->BS->center.y + cur_cr->height;

				calcVerts[0].x = cur_cr->BS->center.x - cur_cr->BS->radius;
				calcVerts[0].y = cvy;
				calcVerts[0].z = cur_cr->BS->center.z - cur_cr->BS->radius;

				calcVerts[1].x = cur_cr->BS->center.x + cur_cr->BS->radius;
				calcVerts[1].y = cvy;
				calcVerts[1].z = cur_cr->BS->center.z - cur_cr->BS->radius;

				calcVerts[2].x = cur_cr->BS->center.x + cur_cr->BS->radius;
				calcVerts[2].y = cvy;
				calcVerts[2].z = cur_cr->BS->center.z + cur_cr->BS->radius;
			
				calcVerts[3].x = cur_cr->BS->center.x - cur_cr->BS->radius;
				calcVerts[3].y = cvy;
				calcVerts[3].z = cur_cr->BS->center.z + cur_cr->BS->radius;
			
				GetWorldPosToScreenPos(&calcVerts[0],&screen_pos);

				add_pr->maxBBox[0] = screen_pos.x;
				add_pr->maxBBox[1] = screen_pos.x;
				add_pr->maxBBox[2] = screen_pos.y;
				add_pr->maxBBox[3] = screen_pos.z;
				//@{ Jaewon 20050714
				// ;)
				add_pr->dOnXZ = GetCameraDistanceOnXZ(calcVerts[0]);
				//@} Jaewon

				for(i=1;i<4;++i)
				{
					GetWorldPosToScreenPos(&calcVerts[i],&screen_pos);

					if(screen_pos.x < add_pr->maxBBox[0])
					{
						add_pr->maxBBox[0] = screen_pos.x;
					}

					if(screen_pos.x > add_pr->maxBBox[1])
					{
						add_pr->maxBBox[1] = screen_pos.x;
					}

					if(screen_pos.y < add_pr->maxBBox[2])
					{
						add_pr->maxBBox[2] = screen_pos.y;
					}

					//@{ Jaewon 20050714
					// > -> <
					// Custom render objects cannot be an occluder,
					// so it should take a near z conservatively.
					if(screen_pos.z < add_pr->maxBBox[3])
					//@} Jaewon
					{
						add_pr->maxBBox[3] = screen_pos.z;
					}

					//@{ Jaewon 20050714
					// ;)
					RwReal dOnXZ = GetCameraDistanceOnXZ(calcVerts[i]);
					if(dOnXZ < add_pr->dOnXZ)
						add_pr->dOnXZ = dOnXZ;
					//@} Jaewon
				}

				//@{ Jaewon 20050714
				// PreRenderList -> std::priority_queue<PreRender>
				m_priorityQueuePreRender.push(add_pr);
				//@} Jaewon
			}
		}
		
		cur_cr = cur_cr->next;
	}
}

void	AgcmRender::SetTerrainOccluderBoxs(OcRootQuadTreeNode*	node,FLOAT*	fArray,INT32* index)
{
	PROFILE("SetTerrainOccluderBoxs");
	if(node->isleaf)
	{
		RwV3d	screen_pos;

		//AcuIMDraw::DrawLine(&node->topVerts[0],&node->topVerts[1]);
		//AcuIMDraw::DrawLine(&node->topVerts[1],&node->topVerts[3]);
		//AcuIMDraw::DrawLine(&node->topVerts[3],&node->topVerts[2]);
		//AcuIMDraw::DrawLine(&node->topVerts[2],&node->topVerts[0]);

		if(!GetWorldPosToScreenPos(&node->topVerts[0],&screen_pos))
		{
			return;
		}

		fArray[(*index)] = screen_pos.x;
		fArray[(*index)+1] = screen_pos.x;
		fArray[(*index)+2] = screen_pos.y;

		for(int i=1;i<4;++i)
		{
			if(!GetWorldPosToScreenPos(&node->topVerts[i],&screen_pos))
			{
				return;
			}

			if(screen_pos.x < fArray[(*index)+0])
			{
				fArray[(*index)+0] = screen_pos.x;
			}

			if(screen_pos.x > fArray[(*index)+1])
			{
				fArray[(*index)+1] = screen_pos.x;
			}

			if(screen_pos.y > fArray[(*index)+2])
			{
				fArray[(*index)+2] = screen_pos.y;
			}
		}

		(*index) += 3;
	}
	else
	{
		for(int i=0;i<4;++i)
		{
			SetTerrainOccluderBoxs(node->child[i] ,fArray,index);
		}
	}
}

void	AgcmRender::OcTreeChildRender(OcTreeNode*	node,BOOL	bCheck,INT32 nDistSector,RwCamera*	pCheckCamera)
{
	PROFILE("OcTreeChildRender");

	if ( !node )		return;

	RwFrustumTestResult		res;

	OcNodeData*				pNodeData = m_pAgcmOcTree->GetNodeData(node);
	OcClumpList*			cur_node = pNodeData->clump_list;
	OcCustomDataList*		cur_cd = pNodeData->custom_data_list;
	
	if( bCheck )
	{
		res = RwCameraFrustumTestSphere( pCheckCamera, (RwSphere*)&node->BS );
		if( res == rwSPHEREOUTSIDE  )
			return;
		else if(res == rwSPHEREINSIDE)
		{
			if(node->objectnum>0)
			{
				// 현재 node의 clump를 검사없이 그려준다.
				while(cur_node)
				{
					if(nDistSector < cur_node->clump->stType.viewSectorDistance &&
						cur_node->clump->stType.frustumTestTick != m_ulCurTick)		// octree에선 clumprendertick개념이다.
					{
						cur_node->clump->stType.frustumTestTick = m_ulCurTick;

						InsertPreListOcTree( cur_node->clump->atomicList, cur_node->clump );					
					}

					cur_node = cur_node->next;
				}

				while(cur_cd)
				{
					if(nDistSector < cur_cd->iAppearanceDistance && *(cur_cd->iRenderTick) != (INT32)m_ulCurTick)
					{
						*(cur_cd->iRenderTick) = (INT32)m_ulCurTick;

						InsertPreListCustom(cur_cd);
					}
					
					cur_cd = cur_cd->next;
				}
			}

			if(node->bHasChild)
			{
				for(int i=0;i<8;++i)
				{
					OcTreeChildRender(node->child[i],FALSE,nDistSector,pCheckCamera);
				}
			}
		}
		else if(res == rwSPHEREBOUNDARY)
		{
			if(node->objectnum > 0)
			{
				// 현재 node의 clump를 검사하여 그려준다.
				while(cur_node)
				{
					if(nDistSector < cur_node->clump->stType.viewSectorDistance
						&& cur_node->clump->stType.frustumTestTick != m_ulCurTick)		// octree에선 clumprendertick개념이다.
					{
						cur_node->clump->stType.frustumTestTick = m_ulCurTick;

						res = RwCameraFrustumTestSphere( pCheckCamera, &cur_node->BS );
						if(res != rwSPHEREOUTSIDE)
						{
							//@{ Jaewon 20050608
							// Just use clump->atomicList.
							InsertPreListOcTree(cur_node->clump->atomicList,cur_node->clump);					
							//@} Jaewon
						}
					}

					cur_node = cur_node->next;
				}

				while(cur_cd)
				{
					if(nDistSector < cur_cd->iAppearanceDistance && *(cur_cd->iRenderTick) != (INT32)m_ulCurTick)
					{
						*(cur_cd->iRenderTick) = (INT32)m_ulCurTick;

						res = RwCameraFrustumTestSphere( pCheckCamera, &cur_cd->BS );
						if(res != rwSPHEREOUTSIDE)
						{
							InsertPreListCustom(cur_cd);
						}
					}
					
					cur_cd = cur_cd->next;
				}
			}

			if(node->bHasChild)
			{
				for(int i=0;i<8;++i)
				{
					OcTreeChildRender(node->child[i],TRUE,nDistSector,pCheckCamera);
				}
			}
		}
	}
	else		// 무조건 통과(parent 가 inside일 경우)
	{
		// 현재 node의 clump를 검사없이 그려준다.
		while(cur_node)
		{
			if(nDistSector < cur_node->clump->stType.viewSectorDistance
				&& cur_node->clump->stType.frustumTestTick != m_ulCurTick)		// octree에선 clumprendertick개념이다.
			{
				cur_node->clump->stType.frustumTestTick = m_ulCurTick;
				//@{ Jaewon 20050608
				// Just use clump->atomicList.
				InsertPreListOcTree(cur_node->clump->atomicList,cur_node->clump);					
				//@} Jaewon
			}

			cur_node = cur_node->next;
		}

		while(cur_cd)
		{
			if(nDistSector < cur_cd->iAppearanceDistance && *(cur_cd->iRenderTick) != (INT32)m_ulCurTick)
			{
				*(cur_cd->iRenderTick) = (INT32)m_ulCurTick;

				InsertPreListCustom(cur_cd);
			}
			
			cur_cd = cur_cd->next;
		}

		if(node->bHasChild)
		{
			for(int i=0;i<8;++i)
			{
				OcTreeChildRender(node->child[i],FALSE,nDistSector,pCheckCamera);
			}
		}
	}
}

void	AgcmRender::SetMaxHeightTable(OccluderLineList*		node)
{
	PROFILE("SetMaxHeightTable");
	int sx = node->startx/100;
	int ex = node->endx/100;

	for(;sx <= ex;++sx)
	{
		if(m_iMaxHeightOcLine[sx] > node->y)
			m_iMaxHeightOcLine[sx] = node->y;
	}
}

void	AgcmRender::SetOccluderLine(INT32 sx,INT32 ex,INT32 y)
{
	PROFILE("SetOccluderLine");
	if(y > m_iCameraRasterHeight) return;
	if(ex < 0 || sx >= m_iCameraRasterWidth) return;

	if(sx == ex) ex += 1;
	if(sx < 0) sx = 0;
	if(ex > m_iCameraRasterWidth) ex = m_iCameraRasterWidth;

	// 삽입 지점 탐색
	OccluderLineList*	cur_node = NULL;
	OccluderLineList*	cur_find;
	OccluderLineList*	add_Onode;
	OccluderLineList*	recycle_node;

	INT32		sindex = sx/100;
	INT32		eindex = ex/100;
	INT32		temp;
	INT32		i;

	if(m_pStartOcLine[sindex] && m_pStartOcLine[sindex]->startx <= sx)
	{
		for(i=sindex;i>=0;--i)
		{
			if(m_pStartOcLine[i]) 
			{
				cur_node = m_pStartOcLine[i];
				break;
			}
		}
	}
	else
	{
		if(sindex > 0)
		{
			for(i=sindex-1;i>=0;--i)
			{
				if(m_pStartOcLine[i]) 
				{
					cur_node = m_pStartOcLine[i];
					break;
				}
			}
		}
		else
		{
			cur_node = m_listOccluderLine;
		}
	}

	while(sx<ex && cur_node)
	{
		if(cur_node->endx <= sx)
		{
			cur_node = cur_node->next;
			continue;
		}
		else if(cur_node->startx == sx)
		{
			if(cur_node->endx == ex)					// 현재 노드와 같을때
			{
				// 바로 재활용
				if(cur_node->y > y)						// y값이 더 높을때만 갱신
				{
					cur_node->y = y;
					SetMaxHeightTable(cur_node);
				}

				break;
			}
			else if(cur_node->endx > ex)				// 현재 노드와 시작 지점이 같고 넓이가 더 짧은경우
			{
				if(cur_node->y > y)
				{
					add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
					if( add_Onode )
					{
						add_Onode->startx = ex;
						add_Onode->endx = cur_node->endx;
						add_Onode->y = cur_node->y;

						add_Onode->next = cur_node->next;
						cur_node->next = add_Onode;

						cur_node->y = y;
						cur_node->endx = ex;

						temp = add_Onode->startx / 100;
						if(!m_pStartOcLine[temp])
						{
							m_pStartOcLine[temp] = add_Onode;
						}
						else if(m_pStartOcLine[temp]->startx >= add_Onode->startx)
						{
							m_pStartOcLine[temp] = add_Onode;
						}

						SetMaxHeightTable(cur_node);
					}
				}

				break;
			}
			else
			{
				if(cur_node->y > y)						// 자신보다 큰 y가 나올때까지 갱신
				{
					recycle_node = cur_node;
					recycle_node->y = y;
					
					cur_find = cur_node->next;
					while(cur_find)
					{
						if(cur_find->y >= y)
						{
							temp = cur_find->startx/100;
							if(m_pStartOcLine[temp] == cur_find)
							{
								m_pStartOcLine[temp] = NULL;
							}

							if(cur_find->endx > ex)
							{
								recycle_node->endx = ex;
								cur_find->startx = ex;

								temp = cur_find->startx / 100;
								if(!m_pStartOcLine[temp])
								{
									m_pStartOcLine[temp] = cur_find;
								}
								else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
								{
									m_pStartOcLine[temp] = cur_find;
								}

								SetMaxHeightTable(recycle_node);
								return;
							}

							recycle_node->endx = cur_find->endx;
						}
						else
						{
							sx = cur_find->endx;
							cur_node = cur_find->next;
						
							break;
						}

						sx = cur_find->endx;
						recycle_node->next = cur_find->next;
						
						cur_find = cur_find->next;
						cur_node = cur_find;
					}

					if(cur_find)
					{
						temp = cur_find->startx / 100;
						if(!m_pStartOcLine[temp])
						{
							m_pStartOcLine[temp] = cur_find;
						}
						else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
						{
							m_pStartOcLine[temp] = cur_find;
						}
					}

					SetMaxHeightTable(recycle_node);
				}
				else 
				{	
					sx = cur_node->endx;
					cur_node = cur_node->next;
				}
							
				continue;
			}
		}
		else if(cur_node->startx < sx)
		{
			if(cur_node->endx == ex)
			{
				if(cur_node->y > y)
				{
					cur_node->endx = sx;

					add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
					if( add_Onode )
					{
						add_Onode->startx = sx;
						add_Onode->endx = ex;
						add_Onode->y = y;

						add_Onode->next = cur_node->next;
						cur_node->next = add_Onode;

						temp = add_Onode->startx / 100;
						if(!m_pStartOcLine[temp])
						{
							m_pStartOcLine[temp] = add_Onode;
						}
						else if(m_pStartOcLine[temp]->startx >= add_Onode->startx)
						{
							m_pStartOcLine[temp] = add_Onode;
						}

						SetMaxHeightTable(add_Onode);
					}
				}
				
				break;
			}
			else if(cur_node->endx > ex)
			{
				if(cur_node->y > y)
				{
					OccluderLineList*	add_Onode2;

					add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
					if( add_Onode )
					{
						add_Onode->startx = sx;
						add_Onode->endx = ex;
						add_Onode->y = y;

						add_Onode2 = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
						if( add_Onode2  )
						{
							add_Onode2->startx = ex;
							add_Onode2->endx = cur_node->endx;
							add_Onode2->y = cur_node->y;

							temp = add_Onode2->startx/100;
							if(!m_pStartOcLine[temp])
							{
								m_pStartOcLine[temp] = add_Onode2;
							}
							else if(m_pStartOcLine[temp]->startx >= add_Onode2->startx)
							{
								m_pStartOcLine[temp] = add_Onode2;
							}

							add_Onode->next = add_Onode2;
							add_Onode2->next = cur_node->next;
							cur_node->next = add_Onode;

							cur_node->endx = sx;

							temp = add_Onode->startx / 100;
							if(!m_pStartOcLine[temp])
							{
								m_pStartOcLine[temp] = add_Onode;
							}
							else if(m_pStartOcLine[temp]->startx >= add_Onode->startx)
							{
								m_pStartOcLine[temp] = add_Onode;
							}

							SetMaxHeightTable(add_Onode);
						}
					}
				}

				break;
			}
			else
			{
				if(cur_node->y > y)
				{
					// cur_node의 next가 null이 아니라 가정
					if(cur_node->next->y > y)
					{
						cur_find = cur_node->next;

						if(cur_find->endx == ex)			// cur_node -> cur_find
						{
							cur_node->endx = sx;

							temp = cur_find->startx/100;
							if(m_pStartOcLine[temp] == cur_find)
							{
								m_pStartOcLine[temp] = NULL;
							}

							cur_find->startx = sx;
							cur_find->y = y;

							temp = cur_find->startx / 100;
							if(!m_pStartOcLine[temp])
							{
								m_pStartOcLine[temp] = cur_find;
							}
							else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
							{
								m_pStartOcLine[temp] = cur_find;
							}

							SetMaxHeightTable(cur_find);
							
							break;
						}
						else if(cur_find->endx > ex)		// cur_node -> 생성 -> cur_find
						{
							cur_node->endx = sx;

							add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
							if( add_Onode )
							{
								add_Onode->startx = sx;
								add_Onode->endx = ex;
								add_Onode->y = y;

								add_Onode->next = cur_find;
								cur_node->next = add_Onode;

								temp = cur_find->startx/100;
								if(m_pStartOcLine[temp] == cur_find)
								{
									m_pStartOcLine[temp] = NULL;
								}

								cur_find->startx = ex;

								temp = cur_find->startx / 100;
								if(!m_pStartOcLine[temp])
								{
									m_pStartOcLine[temp] = cur_find;
								}
								else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
								{
									m_pStartOcLine[temp] = cur_find;
								}

								temp = add_Onode->startx / 100;
								if(!m_pStartOcLine[temp])
								{
									m_pStartOcLine[temp] = add_Onode;
								}
								else if(m_pStartOcLine[temp]->startx >= add_Onode->startx)
								{
									m_pStartOcLine[temp] = add_Onode;
								}

								SetMaxHeightTable(add_Onode);
							}
							break;
						}
						else					// cur_node -> 재활용(cur_node 다음)-> ex 보다 작고 y작은 노드
						{
							cur_node->endx = sx;

							recycle_node = cur_find;
							
							temp = recycle_node->startx/100;
							if(m_pStartOcLine[temp] == recycle_node)
							{
								m_pStartOcLine[temp] = NULL;
							}

							recycle_node->startx = sx;
							recycle_node->y = y;

							temp = recycle_node->startx / 100;
							if(!m_pStartOcLine[temp])
							{
								m_pStartOcLine[temp] = recycle_node;
							}
							else if(m_pStartOcLine[temp]->startx >= recycle_node->startx)
							{
								m_pStartOcLine[temp] = recycle_node;
							}
				
							while(cur_find)
							{
								if(cur_find->y >= y)
								{
									INT32	temp = cur_find->startx/100;
									if(m_pStartOcLine[temp] == cur_find)
									{
										m_pStartOcLine[temp] = NULL;
									}
									
									if(cur_find->endx > ex)
									{
										recycle_node->endx = ex;
										cur_find->startx = ex;

										temp = cur_find->startx / 100;
										if(!m_pStartOcLine[temp])
										{
											m_pStartOcLine[temp] = cur_find;
										}
										else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
										{
											m_pStartOcLine[temp] = cur_find;
										}

										SetMaxHeightTable(recycle_node);
										return;
									}
									
									recycle_node->endx = cur_find->endx;
								}
								else
								{
									sx = cur_find->endx;
									cur_node = cur_find->next;
								
									break;
								}

								sx = cur_find->endx;
								recycle_node->next = cur_find->next;

								cur_find = cur_find->next;
								cur_node = cur_find;
							}

							if(cur_find)
							{
								temp = cur_find->startx / 100;
								if(!m_pStartOcLine[temp])
								{
									m_pStartOcLine[temp] = cur_find;
								}
								else if(m_pStartOcLine[temp]->startx >= cur_find->startx)
								{
									m_pStartOcLine[temp] = cur_find;
								}
							}

							SetMaxHeightTable(recycle_node);
						}
					}
					else
					{
						recycle_node = cur_node;

						add_Onode = (OccluderLineList*) AcuFrameMemory::AllocFrameMemory(sizeof(OccluderLineList));
						if( add_Onode )
						{
							add_Onode->startx = sx;
							add_Onode->endx = recycle_node->endx;
							add_Onode->y = y;

							add_Onode->next = recycle_node->next;
							
							recycle_node->endx = sx;

							sx = cur_node->next->endx;
							cur_node = cur_node->next->next;

							recycle_node->next = add_Onode;

							temp = add_Onode->startx / 100;
							if(!m_pStartOcLine[temp])
							{
								m_pStartOcLine[temp] = add_Onode;
							}
							else if(m_pStartOcLine[temp]->startx > add_Onode->startx)
							{
								m_pStartOcLine[temp] = add_Onode;
							}

							SetMaxHeightTable(add_Onode);
						}
					}
				}
				else 
				{
					sx = cur_node->endx;
					cur_node = cur_node->next;
				}

				continue;
			}
		}
	}
}

// FADE IN/OUT	FUNCTIONS
RpAtomic*	AgcmRender::RenderCallbackForVSFadeAtomic( RpAtomic* pAtomic )
{
	m_pThisAgcmRender->AtomicPreRender(pAtomic);

	// 매프레임마다 동적추가
	VShaderFadeAtomic*	new_fade = (VShaderFadeAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(VShaderFadeAtomic));
	ASSERT( new_fade && "FrameMemory Allocation Error!!!");
	if( !new_fade )		return pAtomic;

	stBlendAtomic NewBlendAtomic;

	NewBlendAtomic.m_pAtomic = pAtomic;
	NewBlendAtomic.m_nBillBoardType	= BS_FADEINOUT;			// FADEIN/OUT

	NewBlendAtomic.m_fSortFactor	=	m_pThisAgcmRender->_GetSortFactor( pAtomic );

	new_fade->bHook			= FALSE;

	pAtomic->stRenderInfo.countStart = max( 0, pAtomic->stRenderInfo.countStart );

	int	shader_type	= pAtomic->stRenderInfo.shaderUseType;
	int	cur_count	= pAtomic->stRenderInfo.countStart;
	int	end_count	= shader_type == VS_JUST_TRANSPARENT ? 255 : pAtomic->stRenderInfo.countEnd;
		
	if( shader_type != VS_JUST_TRANSPARENT )
		pAtomic->stRenderInfo.countStart += m_pThisAgcmRender->m_ulCurTickDiff;
	
	float per = (float)cur_count / (float)end_count ;
	per = max( 0.f, min( per, 1.f ) );

	if( shader_type == VS_FADEINOUT_A )
	{
		if(per<0.5f)
		{
			new_fade->shader_type = VS_FADEIN_A;
			new_fade->alpha = per;

			NewBlendAtomic.m_pData1 = (PVOID)new_fade;
			m_pThisAgcmRender->m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );

			new_fade = (VShaderFadeAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(VShaderFadeAtomic));
			if( new_fade  )
			{
				new_fade->shader_type = VS_FADEOUT_A;
				new_fade->alpha = 1.0f - per;
				new_fade->bHook = FALSE;

				NewBlendAtomic.m_pAtomic = pAtomic;
				NewBlendAtomic.m_nBillBoardType = BS_FADEINOUT;			// FADEIN/OUT
			}
		}
		else
		{
			new_fade->shader_type = VS_FADEOUT_A;
			new_fade->alpha = 1.0f - per;

			NewBlendAtomic.m_pData1 = (PVOID)new_fade;
			m_pThisAgcmRender->m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );

			new_fade = (VShaderFadeAtomic*)AcuFrameMemory::AllocFrameMemory(sizeof(VShaderFadeAtomic));
			if( new_fade )
			{
				new_fade->shader_type = VS_FADEIN_A;
				new_fade->alpha = per;
				new_fade->bHook = FALSE;
			}

			NewBlendAtomic.m_pAtomic = pAtomic;
			NewBlendAtomic.m_nBillBoardType = BS_FADEINOUT;			// FADEIN/OUT
		}
	}
	else if(shader_type == VS_FADEIN_A)
	{
		new_fade->shader_type = VS_FADEIN_A;
		new_fade->alpha = per;
	}
	else if(shader_type == VS_FADEOUT_A)
	{
		new_fade->shader_type = VS_FADEOUT_A;
		new_fade->alpha = 1.0f - per;
	}
	else if(shader_type == VS_JUST_TRANSPARENT)
	{
		new_fade->shader_type = VS_JUST_TRANSPARENT;
		new_fade->alpha = per;
	}

	NewBlendAtomic.m_pData1 = (PVOID)new_fade;
	m_pThisAgcmRender->m_BlendAtomics.Add( R_BLENDSRCAINVA_ADD, NewBlendAtomic );

	if(cur_count > end_count) // fade처리 end
	{
		new_fade->bHook = TRUE;
		pAtomic->stRenderInfo.shaderUseType = VS_NONE;
	}

	return pAtomic;
}

//@{ Jaewon 20050914
// Universal render callback for various temporary special 
// renderings(a.k.a. fade-in/out, uv animation, invisible renderings).
RpAtomic* AgcmRender::RenderCallbackForSpecialRendering(RpAtomic *atomic)
{
	RwUInt32 specialRenderingFlag = (RwUInt32)(atomic->stRenderInfo.backupCB3);
	if( specialRenderingFlag & eSpecialRenderInvisible )
	{
		if(atomic->stRenderInfo.countEnd < 2000)
			atomic->stRenderInfo.countEnd += m_pThisAgcmRender->m_ulCurTickDiff;
		if(atomic->stRenderInfo.countEnd > 2000)
			atomic->stRenderInfo.countEnd = 2000;
		RwUInt8 targetAlpha = *(RwUInt8*)&(atomic->stRenderInfo.fadeinLevel);
		atomic->stRenderInfo.countStart = 255 + (targetAlpha - 255) * (atomic->stRenderInfo.countEnd) / 2000;

		if( m_pThisAgcmRender->m_pcsAgcmPostFX->isReady() &&
			m_pThisAgcmRender->m_bDisableMatD3DFx == FALSE &&
			(((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) >= 0x0101 )
		{
			m_pThisAgcmRender->AtomicPreRender(atomic);
			m_pThisAgcmRender->m_listInvisible.push_back(atomic);
		}
		else
			RenderCallbackForVSFadeAtomic(atomic);
	}
	else if( specialRenderingFlag & eSpecialRenderFade )
	{
		RenderCallbackForVSFadeAtomic(atomic);
	}
	else if( specialRenderingFlag & eSpecialRenderUVAni )
	{
		if(m_pThisAgcmRender->IsSkinedPipe(atomic))
			RenderCallbackForUVAnimSkin(atomic);
		else
			RenderCallbackForUVAnim(atomic);
	}
	else
	{
		ASSERT(!"Invalid special rendering flag in AgcmRender::RenderCallbackForSpecialRendering()!");
	}

	return atomic;
}

void	AgcmRender::ProcessFadeAtomic( RpAtomic* atomic, VShaderFadeAtomic*	pData )
{
	if ( atomic && IsBadReadPtr(atomic, sizeof(RpAtomic)) )
		return;

	INT32					iBill = 0;
	RpGeometry*				pGeom;
	RpClump*				pClump = RpAtomicGetClump(atomic);

	RwUInt32& specialRenderingFlag = (RwUInt32&)atomic->stRenderInfo.backupCB3;

	if(pData->shader_type == VS_FADEIN_A)
	{
		//FadeIn처리
		RpLODAtomicSetCurrentLOD (atomic,atomic->stRenderInfo.fadeinLevel);

		iBill = AcuObject::GetAtomicIsBillboard(atomic);
		RpLODAtomicHookRender(atomic);
	}
	else if(pData->shader_type == VS_FADEOUT_A)
	{
		//FadeOut처리
		RpLODAtomicSetCurrentLOD (atomic,atomic->stRenderInfo.fadeoutLevel);

		iBill = AcuObject::GetAtomicIsBillboard(atomic);
		RpLODAtomicHookRender(atomic);
	}

	pGeom = RpAtomicGetGeometry(atomic);
	if(!pGeom) return;
	
	if(RpGeometryGetNumTriangles(pGeom) <= 1)
	{
		if(pData->bHook)
		{
			specialRenderingFlag &= ~eSpecialRenderFade;

			if(0 == specialRenderingFlag)
			{
				RpLODAtomicUnHookRender  (atomic);
				//// render callback 복귀
				RpAtomicCallBackRender	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB2;
				ASSERT(pFunc);
				RpAtomicSetRenderCallBack(atomic,pFunc);
				RpLODAtomicHookRender  (atomic);
			}
		}

		return;
	}

	int		mat_num = RpGeometryGetNumMaterials(pGeom);
	RpMaterial*		pMat;
	int	mem_alloced = 0;
	int	temp;
	int	i;

	mem_alloced = temp = sizeof(RwRGBA) * mat_num;

	RwRGBA*		matColorSrcArray = (RwRGBA*)AcuFrameMemory::AllocFrameMemory(temp);
	RwRGBA			matColorModify;
	const	RwRGBA*	pmatColorSrc;

	if( NULL == matColorSrcArray ) return;

	for(i=0;i<mat_num;++i)
	{
		pMat = RpGeometryGetMaterial (pGeom,i);
		pmatColorSrc = RpMaterialGetColor(pMat);
		memcpy(&matColorSrcArray[i],pmatColorSrc,sizeof(RwRGBA));
		matColorModify.red = matColorSrcArray[i].red;
		matColorModify.green = matColorSrcArray[i].green;
		matColorModify.blue = matColorSrcArray[i].blue;
		matColorModify.alpha = (RwUInt8 ) ( pData->alpha * 255 );

		RpMaterialSetColor(pMat,&matColorModify);
	}

	RwUInt32 lGeometryFlags = RpGeometryGetFlags( pGeom );
	
	//rpGEOMETRYMODULATEMATERIALCOLOR Set
	RpGeometrySetFlags( pGeom, ( lGeometryFlags | rpGEOMETRYMODULATEMATERIALCOLOR ) );

	int		iTypeProp = AcuObject::GetAtomicType(atomic,NULL);
	int		iType = AcuObject::GetType(iTypeProp);
	RpAtomicCallBackRender pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB;

	if( ACUOBJECT_TYPE_CHARACTER == iType || ACUOBJECT_TYPE_ITEM == iType )
	{
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, ( void * ) TRUE	);
		
		D3DXVECTOR4*		orgColor;
		D3DXVECTOR4			tmpColor;
		if( RpAtomicFxIsEnabled( atomic ) )
		{
			temp = sizeof(D3DXVECTOR4) * mat_num;
			orgColor = (D3DXVECTOR4*)AcuFrameMemory::AllocFrameMemory(temp);
			if( orgColor )
			{
				mem_alloced += temp;

				for(i =0;i<mat_num;++i)
				{
					pMat = RpGeometryGetMaterial (pGeom,i);

					ASSERT(RpMaterialD3DFxGetEffect(pMat));
					if(!RpMaterialD3DFxGetEffect(pMat))	break;
					
					RpMaterialD3DFxGetVector(pMat,"materialDiffuse", &orgColor[i]);
					tmpColor.x = orgColor[i].x;
					tmpColor.y = orgColor[i].y;
					tmpColor.z = orgColor[i].z;
					tmpColor.w = pData->alpha;
					RpMaterialD3DFxSetVector(pMat,"materialDiffuse", &tmpColor);
				}
			}
		}
		
		if( pFunc )		RenderAtomic( atomic, FALSE );
		else			OriginalDefaultAtomicRenderCallback( atomic );
			
		RwRenderStateSet( rwRENDERSTATEZWRITEENABLE	, ( void * ) FALSE	);

		if(RpAtomicFxIsEnabled(atomic))
		{
			for(i =0;i<mat_num;++i)
			{
				pMat = RpGeometryGetMaterial (pGeom,i);

				ASSERT(RpMaterialD3DFxGetEffect(pMat));
				if(!RpMaterialD3DFxGetEffect(pMat))	break;

				RpMaterialD3DFxSetVector(pMat,"materialDiffuse", &orgColor[i]);
			}
		}
	}
	else
	{
		if( pFunc )		RenderAtomic( atomic, iBill );
		else			OriginalDefaultAtomicRenderCallback( atomic );
	}

	RpGeometrySetFlags( pGeom, lGeometryFlags );

	for(i=0;i<mat_num;++i)
	{
		pMat = RpGeometryGetMaterial( pGeom, i );
		RpMaterialSetColor( pMat, &matColorSrcArray[i] );
	}

	if(pData->bHook)
	{
		specialRenderingFlag &= ~eSpecialRenderFade;

		if(0 == specialRenderingFlag)
		{
			RpLODAtomicUnHookRender  (atomic);
			// render callback 복귀
			RpAtomicCallBackRender	pFunc = (RpAtomicCallBackRender)atomic->stRenderInfo.backupCB2;
			ASSERT(pFunc);
			RpAtomicSetRenderCallBack(atomic,pFunc);
			RpLODAtomicHookRender  (atomic);
		}
	}

	AcuFrameMemory::DeallocFrameMemory(mem_alloced);
}

FLOAT	AgcmRender::SetDirectionalLightAdjust(FLOAT fval)
{
	m_fDirectionalLightAdjust = max( 0.f, fval );
	if( m_pDirectLight )
	{
		m_stAdjustedDirectionalLight.red	= m_pDirectLight->color.red * m_fDirectionalLightAdjust;
		m_stAdjustedDirectionalLight.green	= m_pDirectLight->color.green * m_fDirectionalLightAdjust;
		m_stAdjustedDirectionalLight.blue	= m_pDirectLight->color.blue * m_fDirectionalLightAdjust;
	}
	
	return m_fDirectionalLightAdjust;
}

FLOAT	AgcmRender::SetAmbientLightAdjust( FLOAT	fval )
{
	m_fAmbientLightAdjust = max( 0.f, fval );
	if( m_pAmbientLight )
	{
		m_stAdjustedAmbientLight.red	= m_pAmbientLight->color.red * m_fAmbientLightAdjust;
		m_stAdjustedAmbientLight.green	= m_pAmbientLight->color.green * m_fAmbientLightAdjust;
		m_stAdjustedAmbientLight.blue	= m_pAmbientLight->color.blue * m_fAmbientLightAdjust;
	}

	return m_fAmbientLightAdjust;
}

void	AgcmRender::LightValueUpdate()		// 라이트 변경 시에 콜해줘야함.
{
	if( m_pDirectLight )
	{
		m_stAdjustedDirectionalLight.red	= min( m_pDirectLight->color.red * m_fDirectionalLightAdjust, 1.f );
		m_stAdjustedDirectionalLight.green	= min( m_pDirectLight->color.green * m_fDirectionalLightAdjust, 1.f );
		m_stAdjustedDirectionalLight.blue	= min( m_pDirectLight->color.blue * m_fDirectionalLightAdjust, 1.f );

		m_colorOriginalDirect = m_pDirectLight->color;
	}

	if( m_pAmbientLight )
	{
		m_stAdjustedAmbientLight.red	= min( m_pAmbientLight->color.red * m_fAmbientLightAdjust, 1.f );
		m_stAdjustedAmbientLight.green	= min( m_pAmbientLight->color.green * m_fAmbientLightAdjust, 1.f );
		m_stAdjustedAmbientLight.blue	= min( m_pAmbientLight->color.blue * m_fAmbientLightAdjust, 1.f );

		m_colorOriginalAmbient = m_pAmbientLight->color;
	}
}

RwReal AgcmRender::GetCameraDistanceOnXZ(const RwV3d& pointInWorld) const
{
	RwV3d vViewAt = *m_pViewMatrixAt;
	vViewAt.y = 0;

	RwV3d vSub;
	RwV3dSub( &vSub, &pointInWorld, m_pCameraPos );

	return RwV3dDotProduct( &vViewAt, &vSub );
}

extern "C"
{
	extern RpWorldGetAtomicLocalLightsCallBack getAtomicLocalLightsCB;
}

RpAtomic *AgcmRender::AtomicAmbOcclMVLRenderCallBack(RpAtomic *atomic)
{
	if(m_pThisAgcmRender->m_bDisableMatD3DFx || !( RtAmbOcclMapAtomicGetFlags(atomic) & rtAMBOCCLMAPOBJECTINDOOR ))
		OriginalDefaultAtomicRenderCallback( atomic );
	else
	{
		ASSERT( RpMaterialD3DFxGetEffect(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0)) );
		ASSERT( !strnicmp( RpMaterialD3DFxGetEffect(RpGeometryGetMaterial(RpAtomicGetGeometry(atomic), 0))->pSharedD3dXEffect->name, "ambientOcclusionMVL.fx", FX_FILE_NAME_LENGTH ) );
						   
		AmbOcclMVLAtomic ambOcclMVLAtomic;
		RwUInt32 nLights = min( getAtomicLocalLightsCB ? getAtomicLocalLightsCB(atomic, ambOcclMVLAtomic.pLight_) : 0, 2 );


		ambOcclMVLAtomic.pAtomic_ = atomic;
		m_pThisAgcmRender->m_ambOcclMVLAtomics[nLights].push_back(ambOcclMVLAtomic);
	}

	return atomic;
}

extern "C"
{
	extern LPDIRECT3DBASETEXTURE9 D3DTextureFromRwTexture(RwTexture *texture);
	extern RwRaster* _rwD3D9RWGetRasterStage(RwUInt32 stage);
	extern RwBool _rwD3D9RWSetRasterStage(RwRaster *raster, RwUInt32 stage);

	extern D3DMATRIX _RwD3D9D3D9ViewTransform;
}

// Batch-render gathered "AMBIENTOCCLUSIONMVL.FX" atomics.
void AgcmRender::RenderAmbOcclMVLAtomics()
{
	ASSERT((m_ambOcclMVLAtomics[0].empty() && m_ambOcclMVLAtomics[1].empty() && m_ambOcclMVLAtomics[2].empty()) || !m_bDisableMatD3DFx);

	// First pass
	for( RwUInt32 k=0; k<FX_LIGHTING_VARIATION; ++k )
	{
		for( RwUInt32 i=0; i<m_ambOcclMVLAtomics[k].size(); ++i )
		{
			RxD3D9AllInOneRenderCallBack backup = ((myD3D9InstanceNodeData*)(m_ambOcclMVLAtomics[k][i].pAtomic_->pipeline->nodes[0].privateData))->renderCallback;
			((myD3D9InstanceNodeData*)(m_ambOcclMVLAtomics[k][i].pAtomic_->pipeline->nodes[0].privateData))->renderCallback = NULL;
			OriginalDefaultAtomicRenderCallback(m_ambOcclMVLAtomics[k][i].pAtomic_);
			((myD3D9InstanceNodeData*)(m_ambOcclMVLAtomics[k][i].pAtomic_->pipeline->nodes[0].privateData))->renderCallback = backup;
		}
	}

	// Second pass - final batch render
	for(RwUInt32 k=0; k<FX_LIGHTING_VARIATION; ++k)
	{
		if( m_ambOcclMVLAtomics[k].empty() )		continue;

		// keep sync with RW's texture caches.
		// back up rasters.
		RwRaster *backup[8];
		for(RwUInt32 i=0; i<8; ++i)
			backup[i] = _rwD3D9RWGetRasterStage(i);

		DxEffect *effect = RpMaterialD3DFxGetEffect(RpGeometryGetMaterial(RpAtomicGetGeometry(m_ambOcclMVLAtomics[k][0].pAtomic_), 0));
		LPD3DXEFFECT d3dxEffect = effect->pSharedD3dXEffect->d3dxEffect[k];
		if(effect->pSharedD3dXEffect->technicSet == FALSE)
		{
			for(RwUInt32 m=0; m < FX_LIGHTING_VARIATION; ++m)
			{
				D3DXHANDLE handle;
				effect->pSharedD3dXEffect->d3dxEffect[m]->FindNextValidTechnique(NULL, &handle);
				effect->pSharedD3dXEffect->d3dxEffect[m]->SetTechnique(handle);
			}

			effect->pSharedD3dXEffect->technicSet = TRUE;
		}

		// EffectUploadRenderWareShaderConstants
		ASSERT(effect->rw[ID_LIGHT_AMBIENT].handle[k]);
		d3dxEffect->SetVector( effect->rw[ID_LIGHT_AMBIENT].handle[k], (const D3DXVECTOR4*)&m_colorOriginalAmbient );
			
		ASSERT(effect->rw[ID_FOG].handle[k]);
		const RwCamera *camera = RwCameraGetCurrentCamera();
		const RwReal fogPlane  = camera->fogPlane;
		const RwReal farPlane  = camera->farPlane;
		RwV4d fogData;
		/* Fog Constant: x = -1/(fog_end-fog_start), y = fog_end/(fog_end-fog_start) */
		fogData.x = -1.f / (farPlane - fogPlane);
		fogData.y = farPlane / (farPlane - fogPlane);
		fogData.z = 0.0f;
		fogData.w = 1.0f;
		d3dxEffect->SetVector( effect->rw[ID_FOG].handle[k], (const D3DXVECTOR4 *)&fogData );
			

		RwUInt32 numPasses;
		HRESULT hr = d3dxEffect->Begin(&numPasses, 0);
		ASSERT(SUCCEEDED(hr));
		hr = d3dxEffect->BeginPass(0);
		ASSERT(SUCCEEDED(hr));

		for(RwUInt32 i=0; i<m_ambOcclMVLAtomics[k].size(); ++i)
		{
			RpAtomic *atomic = m_ambOcclMVLAtomics[k][i].pAtomic_;
			RpLight **pLight = m_ambOcclMVLAtomics[k][i].pLight_;
			RpGeometry *geometry = RpAtomicGetGeometry(atomic);
			if( !geometry->repEntry )		continue;
				
			// If it is a geometry-effect terrain, disable alpha-blending & alpha-testing.
			if(atomic->stType && (atomic->stType->eType & ACUOBJECT_TYPE_NO_MTEXTURE))
			{
				((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}

			_rwD3D9EnableClippingIfNeeded( atomic, 1 );
			RxD3D9ResEntryHeader *resEntryHeader = (RxD3D9ResEntryHeader *)(geometry->repEntry + 1);

			if( resEntryHeader->indexBuffer )
				RwD3D9SetIndices(resEntryHeader->indexBuffer);

			_rwD3D9SetStreams(resEntryHeader->vertexStream, resEntryHeader->useOffsets);
			RwD3D9SetVertexDeclaration(resEntryHeader->vertexDeclaration);
			_rwD3D9VSSetActiveWorldMatrix(RwFrameGetLTM(RpAtomicGetFrame(atomic)));
				
			// EffectUploadRenderWareShaderConstants
			D3DXMATRIX D3DMatrix;
			ASSERT(effect->rw[ID_WORLD_VIEW_PROJ_MATRIX].handle[k]);
			_rwD3D9VSGetComposedTransformMatrix(&D3DMatrix);
			d3dxEffect->SetMatrixTranspose(effect->rw[ID_WORLD_VIEW_PROJ_MATRIX].handle[k], &D3DMatrix);
			ASSERT(effect->rw[ID_WORLD_MATRIX].handle[k]);
			_rwD3D9VSGetWorldNormalizedTransposeMatrix(&D3DMatrix);
			d3dxEffect->SetMatrixTranspose(effect->rw[ID_WORLD_MATRIX].handle[k], &D3DMatrix);
			ASSERT(effect->rw[ID_BASE_TEXTURE].handle[k]);
			// Set the ambient occlusion texture.
			ASSERT(effect->constUploadCallBack);
			effect->constUploadCallBack(atomic, NULL, effect, 0, k);

			for(RwUInt32 m=0; m<5; ++m)
			{
				if(pLight[m] == NULL)
				{
					if(effect->rw[ID_LIGHT_COLOR0+m].handle[k])
					{
						// just set its color to 'black'.
						RwRGBAReal black = { 0.0f, 0.0f, 0.0f, 1.0f };
						d3dxEffect->SetVector( effect->rw[ID_LIGHT_COLOR0+m].handle[k], (const D3DXVECTOR4*)&black );
					}
				}
				else
				{
					// color
					if(effect->rw[ID_LIGHT_COLOR0+m].handle[k])
						d3dxEffect->SetVector( effect->rw[ID_LIGHT_COLOR0+m].handle[k], (const D3DXVECTOR4*)RpLightGetColor(pLight[m]) );

					// parameter
					if(effect->rw[ID_LIGHT_PARAMETERS0+m].handle[k])
					{
						// {1/r, 1/(1-cos), -cos/(1-cos), 0}
						D3DXVECTOR4 param;
						float r, c;
						r = RpLightGetRadius(pLight[m]);
						param.x = r != 0.f ? 1.f/r : 0.f;
						if(RpLightGetType(pLight[m]) == rpLIGHTPOINT)
						{
							param.y = 0.0f;
							param.z = 1.0f;
						}
						else
						{
							c = (float)RwCos(RpLightGetConeAngle(pLight[m]));
							param.y = c>0.999f ? 1/(0.001f) : 1/(1-c);
							param.z = -c * param.y;
						}
						param.w = 0.0f;
						d3dxEffect->SetVector( effect->rw[ID_LIGHT_PARAMETERS0+m].handle[k], &param );
					}

					if( effect->rw[ID_LIGHT_POSITION0 + m].handle[k])
					{
						RwV3d out;
						RwV3d *pos = RwMatrixGetPos(RwFrameGetLTM(RpLightGetFrame(pLight[m])));

						SpaceAnnotation space = effect->localLightPositionSpace[m];
						if(ID_LOCAL_SPACE == space)
							_rwD3D9VSGetPointInLocalSpace(pos, &out);
						else if(ID_VIEW_SPACE == space)
							D3DXVec3TransformCoord( (D3DXVECTOR3 *)&out, (D3DXVECTOR3 *)pos, (const D3DXMATRIX *)&_RwD3D9D3D9ViewTransform );
						else if(ID_WORLD_SPACE == space)
							RwV3dAssign(&out, pos);
						else
							ASSERT(0);

						D3DXVECTOR4 d3dVector( out.x, out.y, out.z, 0.f );
						d3dxEffect->SetVector(effect->rw[ID_LIGHT_POSITION0+m].handle[k], &d3dVector);
					}

					if( effect->rw[ID_LIGHT_DIRECTION0 + m].handle[k] )
					{
						RwV3d out;
						RwV3d* at = RwMatrixGetAt(RwFrameGetLTM(RpLightGetFrame(pLight[m])));

						SpaceAnnotation space = effect->localLightDirectionSpace[m];
						if( ID_LOCAL_SPACE == space )
							_rwD3D9VSGetNormalInLocalSpace( at, &out );
						else if(ID_VIEW_SPACE == space)
							D3DXVec3TransformNormal( (D3DXVECTOR3 *)&out, (D3DXVECTOR3 *)at, (const D3DXMATRIX *)&_RwD3D9D3D9ViewTransform );
						else if( ID_WORLD_SPACE == space )
							RwV3dAssign(&out, at);
						else
							ASSERT(0);

						D3DXVECTOR4 d3dVector( out.x, out.y, out.z, 0.f );
						d3dxEffect->SetVector(effect->rw[ID_LIGHT_DIRECTION0+m].handle[k], &d3dVector);
					}
				}
			}

			RxD3D9InstanceData *instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);
			RwUInt32 numMeshes = resEntryHeader->numMeshes;
			while(numMeshes--)
			{
				RpMaterial *material = instancedData->material;
				RpMaterialD3DFxExt *matData = MATERIAL_GET_FX_DATA(material);
				DxEffect *effect = matData->effect;
				if( !effect )
				{
					++instancedData;
					continue;
				}

				ASSERT(effect->pSharedD3dXEffect->d3dxEffect[k] == d3dxEffect);

				RwTexture* texture = RpMaterialGetTexture(material);
				if( texture )
					d3dxEffect->SetTexture(effect->rw[ID_BASE_TEXTURE].handle[k], D3DTextureFromRwTexture(texture));

				d3dxEffect->CommitChanges();

				if( resEntryHeader->indexBuffer )
				{
					RwD3D9DrawIndexedPrimitive( (D3DPRIMITIVETYPE)resEntryHeader->primType,
												instancedData->baseIndex, 0, instancedData->numVertices,
												instancedData->startIndex, instancedData->numPrimitives );
				}
				else
					RwD3D9DrawPrimitive( (D3DPRIMITIVETYPE)resEntryHeader->primType, instancedData->baseIndex, instancedData->numPrimitives);

				++instancedData;
			}

			// Restore alpha-blending & alpha-testing.
			if(atomic->stType && (atomic->stType->eType & ACUOBJECT_TYPE_NO_MTEXTURE))
			{
				((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			}
		}

		d3dxEffect->EndPass();
		d3dxEffect->End();

		// restore rasters.
		for( RwUInt32 i=0; i<8; ++i )
		{
			_rwD3D9RWSetRasterStage( NULL, i );
			_rwD3D9RWSetRasterStage( backup[i], i );
		}
	}
}

void AgcmRender::ComputeLightColors()		// Compute light colors in shadow, at login selection, of quest objects.
{
	if( !m_pAmbientLight || !m_pDirectLight)		return;

	const RwRGBAReal Rainbow[7] = { { 1.0f, 0.03f, 0.03f, 1.0f },
									{ 0.96f, 0.46f, 0.24f, 1.0f },
									{ 1.0f, 0.9f, 0.1f, 1.0f },
									{ 0.1f, 1.0f, 0.21f, 1.0f },
									{ 0.22f, 0.3f, 0.96f, 1.0f },
									{ 0.12f, 0.27f, 0.69f, 1.0f },
									{ 0.9f, 0.15f, 0.9f, 1.0f } };

	m_lightColorInShadow[0].red		= m_pAmbientLight->color.red * 0.6f;
	m_lightColorInShadow[0].green	= m_pAmbientLight->color.green * 0.6f;
	m_lightColorInShadow[0].blue	= m_pAmbientLight->color.blue * 0.6f;

	m_lightColorInShadow[1].red		= m_pDirectLight->color.red * 0.6f;
	m_lightColorInShadow[1].green	= m_pDirectLight->color.green * 0.6f;
	m_lightColorInShadow[1].blue	= m_pDirectLight->color.blue * 0.6f;

	if( m_bLogin )
	{
		if( m_iLightColorStatus == 0 )
		{
			m_fLightColorParam += (0.0003f * m_ulCurTickDiff);

			if( m_fLightColorParam > 0.8f)
			{ 
				m_fLightColorParam	= 0.8f;
				m_iLightColorStatus	= 1;
			}
		}
		else if( m_iLightColorStatus == 1 )
		{
			m_fLightColorParam -= (0.0003f * m_ulCurTickDiff);

			if(m_fLightColorParam < 0.2f)
			{
				m_fLightColorParam	= 0.2f;
				m_iLightColorStatus	= 0;
			}
		}

		m_lightColorAtLoginSelect[1].red = m_lightColorAtLoginSelect[1].green = m_lightColorAtLoginSelect[1].blue = m_fLightColorParam;
		m_lightColorAtLoginSelect[1].alpha = 1.0f;
	}
	else			// 일단 지금은 login외에 라이팅 처리가 하나므로 else처리
	{
		m_fLightColorParam += (0.0004f * m_ulCurTickDiff);

		if(m_fLightColorParam >= 7.0f)
		{
			while(m_fLightColorParam > 7.0f)
				m_fLightColorParam -= 7.0f;
		}

		int		light_index = (int)m_fLightColorParam;
		int		color_index1,color_index2;
		float	per = m_fLightColorParam - light_index;
		float	per_inv = 1.0f - per;

		if(light_index == 6)
		{
			color_index1 = 6;
			color_index2 = 0;
		}
		else
		{
			color_index1 = light_index;
			color_index2 = color_index1 + 1;
		}

		m_lightColorOfQuestObject[0].red	= m_pAmbientLight->color.red + (Rainbow[color_index2].red * per + Rainbow[color_index1].red * per_inv) * 0.3f;
		m_lightColorOfQuestObject[0].green	= m_pAmbientLight->color.green + (Rainbow[color_index2].green * per + Rainbow[color_index1].green * per_inv) * 0.3f;
		m_lightColorOfQuestObject[0].blue	= m_pAmbientLight->color.blue + (Rainbow[color_index2].blue * per + Rainbow[color_index1].blue * per_inv) * 0.3f;
		m_lightColorOfQuestObject[0].alpha	= 1.0f;
	}
}

void AgcmRender::RenderInvisibleAtomics()
{
	if( m_listInvisible.empty() )		return;
		
	ASSERT( m_pcsAgcmPostFX && m_pcsAgcmPostFX->isReady() && !m_bDisableMatD3DFx &&
			(((const D3DCAPS9*)RwD3D9GetCaps())->VertexShaderVersion & 0xffff) >= 0x0101 );

	// Capture the current scene.
	RwCamera* pMainCam = GetCamera();
	RwCameraEndUpdate( pMainCam );
	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	pd3dDevice->EndScene();

	m_pcsAgcmPostFX->saveImmediateFrame();

	pd3dDevice->BeginScene();
	LockFrame();
	RwCameraBeginUpdate(pMainCam);
	RwD3D9SetRenderState(D3DRS_FOGEND, *((DWORD*)&m_fFogEnd)); 
	UnlockFrame();

	// Set the 'sceneTexture' in "invisible.fx" to the captured frame.
	RwTexture *pSceneTexture = RwTextureCreate(m_pcsAgcmPostFX->getSavedFrame());
	for(RwUInt32 i=0; i<5; ++i)
		RpMaterialD3DFxSetTexture(m_pInvisibleMaterial[i], "sceneTexture", pSceneTexture);

	// Apply the "invisible.fx" & Render.
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);

	for(RwUInt32 i=0; i<m_listInvisible.size(); ++i)
	{
		RpAtomic* pAtomic = m_listInvisible[i];
		RpGeometry* pGeometry = RpAtomicGetGeometry(pAtomic);
		if( !pGeometry )		continue;
			
		DxEffect *pFXBackups[10];
		RwUInt32 nMats = RpGeometryGetNumMaterials(pGeometry);
		if(nMats >= 10) 
			nMats = 10;

		RpSkin *pSkin = RpSkinGeometryGetSkin(pGeometry);
		for(RwUInt32 j=0; j<nMats; ++j)
		{
			RpMaterialD3DFxExt *matData = MATERIAL_GET_FX_DATA(RpGeometryGetMaterial(pGeometry, j));
			pFXBackups[j] = matData->effect;
			// The original material should be a fx material.
			ASSERT(pFXBackups[j]);
			if(pFXBackups[j])
				matData->effect = MATERIAL_GET_FX_DATA(m_pInvisibleMaterial[pSkin?pSkin->vertexMaps.maxWeights:0])->effect;

			// Adjust its alpha.
			RpMaterialD3DFxSetFloat(RpGeometryGetMaterial(pGeometry, j), "invisibleAlpha", RwReal(pAtomic->stRenderInfo.countStart)/255.0f);
		}

		RenderAtomic(pAtomic, FALSE);

		for(RwUInt32 j=0; j<nMats; ++j)
		{
			RpMaterialD3DFxExt *matData = MATERIAL_GET_FX_DATA(RpGeometryGetMaterial(pGeometry, j));
			matData->effect = pFXBackups[j];
		}
	}

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);

	// Reset the 'sceneTexture' in "invisible.fx" to NULL and destroy the temporary texture.
	for( RwUInt32 i=0; i<5; ++i )
		RpMaterialD3DFxSetTexture( m_pInvisibleMaterial[i], "sceneTexture", NULL );

	RwTextureSetRaster( pSceneTexture, NULL );
	RwTextureDestroy( pSceneTexture );
	pSceneTexture = NULL;
}

bool AgcmRender::_RenderBlendAtomic_BillBoard( void )
{
	stBlendAtomicInfo* pBlendAtomic = m_BlendAtomics.GetBlendAtomicByRenderMode( R_BLENDSRCAINVA_ADD );
	if( !pBlendAtomic ) return false;

	/*
	INT32 nBlendAtomicCount = pBlendAtomic->GetCount();
	for( INT32 nCount = 0 ; nCount < nBlendAtomicCount ; nCount++ )
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );

		stBlendAtomic* pAtomic = pBlendAtomic->GetAtomicByIndex( nCount );
		if( pAtomic )
		{
			if( pAtomic->m_nBillBoardType < 10 )
			{
				RenderAtomic( pAtomic->m_pAtomic, pAtomic->m_nBillBoardType );
			}
			else if( pAtomic->m_nBillBoardType == BS_FADEINOUT )
			{
				if( pAtomic->m_pData1 )
				{
					ProcessFadeAtomic( pAtomic->m_pAtomic, ( VShaderFadeAtomic* )pAtomic->m_pData1 );
				}
			}
			else if( pAtomic->m_nBillBoardType == BS_CUSTOM )
			{
				ApModuleDefaultCallBack fnCallBack = ( ApModuleDefaultCallBack )pAtomic->m_pData1;
				if( fnCallBack )
				{
					fnCallBack( pAtomic->m_pAtomic, pAtomic->m_pClass, pAtomic->m_pData2 );
				}
			}
		}
	}
	*/

	for( stBlendAtomicInfo::Atomics::iterator iter = pBlendAtomic->atomics_.begin() ; iter != pBlendAtomic->atomics_.end() ; ++iter )
	{
		RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );

		RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );

		stBlendAtomic* pAtomic = &iter->second;
		if( pAtomic )
		{
			if( pAtomic->m_nBillBoardType < 10 )
			{
				RenderAtomic( pAtomic->m_pAtomic, pAtomic->m_nBillBoardType );
			}
			else if( pAtomic->m_nBillBoardType == BS_FADEINOUT )
			{
				if( pAtomic->m_pData1 )
				{
					ProcessFadeAtomic( pAtomic->m_pAtomic, ( VShaderFadeAtomic* )pAtomic->m_pData1 );
				}
			}
			else if( pAtomic->m_nBillBoardType == BS_CUSTOM )
			{
				ApModuleDefaultCallBack fnCallBack = ( ApModuleDefaultCallBack )pAtomic->m_pData1;
				if( fnCallBack )
				{
					fnCallBack( pAtomic->m_pAtomic, pAtomic->m_pClass, pAtomic->m_pData2 );
				}
			}
		}
	}

	return true;
}

void AgcmRender::_RenderBlendAtomic_LastBlended( void )
{
	/*
	INT32 nBlendAtomicCount = m_BlendAtomics.m_LastBlendAtomic.GetCount();
	for( INT32 nCount = 0 ; nCount < nBlendAtomicCount ; nCount++ )
	{
		stBlendAtomic* pAtomic = m_BlendAtomics.m_LastBlendAtomic.GetAtomicByIndex( nCount );
		if( pAtomic )
		{
			RenderAtomic( pAtomic->m_pAtomic, pAtomic->m_nBillBoardType );
		}
	}
	*/

	stBlendAtomicInfo::Atomics & atomics = m_BlendAtomics.m_LastBlendAtomic.atomics_;

	INT32 nBlendAtomicCount = m_BlendAtomics.m_LastBlendAtomic.GetCount();
	for( stBlendAtomicInfo::Atomics::iterator iter = atomics.begin(); iter != atomics.end() ; ++iter )
	{
		stBlendAtomic & atomic = iter->second;

		RenderAtomic( atomic.m_pAtomic, atomic.m_nBillBoardType );
	}
}

bool AgcmRender::_RenderBlendAtomic( void )
{
	INT32 nBlendAtomicCount = m_BlendAtomics.GetCount();
	bool bD3D9Set = false;

	/*
	// 0번째 아토믹들은 이미 먼저 렌더링되었으니 1번째 부터 그린다.
	for( INT32 nCount = 1 ; nCount < nBlendAtomicCount ; nCount++ )
	{
		stBlendAtomicInfo* pBlendAtomic = m_BlendAtomics.GetBlendAtomicByRenderMode( ( enumRenderBMode )nCount );
		if( pBlendAtomic )
		{
			_ChangeRenderStateForBlendAtomic( ( enumRenderBMode )nCount );
			
			INT32 nAtomicCount = pBlendAtomic->GetCount();
			for( INT32 nSubCount = 0 ; nSubCount < nAtomicCount ; nSubCount++ )
			{
				stBlendAtomic* pAtomic = pBlendAtomic->GetAtomicByIndex( nSubCount );
				if( pAtomic )
				{
					RenderAtomic( pAtomic->m_pAtomic, pAtomic->m_nBillBoardType );
				}
			}

			bD3D9Set = true;
		}
	}
	*/

	// 0번째 아토믹들은 이미 먼저 렌더링되었으니 1번째 부터 그린다.
	for( INT32 nCount = 1 ; nCount < nBlendAtomicCount ; nCount++ )
	{
		stBlendAtomicInfo* pBlendAtomic = m_BlendAtomics.GetBlendAtomicByRenderMode( ( enumRenderBMode )nCount );
		if( pBlendAtomic )
		{
			_ChangeRenderStateForBlendAtomic( ( enumRenderBMode )nCount );

			typedef stBlendAtomicInfo::Atomics Atomics;

			Atomics & atomics = pBlendAtomic->atomics_;

			for( Atomics::iterator iter = atomics.begin(); iter != atomics.end(); ++iter )
			{
				stBlendAtomic & pAtomic = iter->second;

				RenderAtomic( pAtomic.m_pAtomic, pAtomic.m_nBillBoardType );
			}

			bD3D9Set = true;
		}
	}

	return bD3D9Set;
}

void AgcmRender::_ChangeRenderStateForBlendAtomic( enumRenderBMode eRenderMode )
{
	switch( eRenderMode )
	{
	case R_BLENDSRCAINVA_REVSUB :
		{
			RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
			RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCALPHA );
			RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );
		}
		break;

	case R_BLENDSRCAINVA_SUB :
		{
			RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
			RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCALPHA );
			RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );
		}
		break;

	case R_TEXTURE_STAGE_ADD :
	case R_TEXTURE_STAGE_REVSUB :
	case R_TEXTURE_STAGE_SUB :
		{
			RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
			RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
		}
		break;

	case R_BLENDSRCCINVC_ADD :
		{
			RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );
			RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCCOLOR );
			RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCCOLOR );
		}
		break;

	case R_BLENDSRCCINVC_REVSUB :
		{
			RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT );
			RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCCOLOR );
			RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCCOLOR );
		}
		break;

	case R_BLENDSRCCINVC_SUB :
		{
			RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_SUBTRACT );
			RwRenderStateSet( rwRENDERSTATESRCBLEND, ( void* )rwBLENDSRCCOLOR );
			RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCCOLOR );
		}
		break;
	}
}

void AgcmRender::_RestoreRenderStateForBlendAtomic( void )
{
	RwD3D9SetRenderState( D3DRS_BLENDOP , D3DBLENDOP_ADD );	// default복귀
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
}


FLOAT	stBlendAtomicInfo::_AtomicGetCameraDistance( RpAtomic *pAtomic )
{
	RwFrame *frame;
	RwV3d *camPos, atomicPos, temp;
	RwSphere *atomicBSphere;
	RwReal distance2;

	atomicBSphere = RpAtomicGetBoundingSphere(pAtomic);
	if( !atomicBSphere )
		return 0;

	RwV3dTransformPoint(&atomicPos, &atomicBSphere->center, 
		RwFrameGetLTM(RpAtomicGetFrame(pAtomic)));

	frame = RwCameraGetFrame(RwCameraGetCurrentCamera());
	camPos = RwMatrixGetPos(RwFrameGetLTM(frame));

	RwV3dSub(&temp, &atomicPos, camPos);

	distance2 = RwV3dDotProduct(&temp, &temp);

	return distance2;
}
