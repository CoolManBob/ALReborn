#include "AgcdEffCtrlMng.h"
#include "AgcdEffGlobal.h"
#include "ApMemoryTracker.h"
#include "cslog.h"

AgcdEffCtrlMng::AgcdEffCtrlMng()
: m_dwInsID( 0 )
{
}

AgcdEffCtrlMng::~AgcdEffCtrlMng()
{
	ClearCtrlSet();
}

void AgcdEffCtrlMng::ClearCtrlSet(void)
{
	AgcmEff2*		pAgcmEff	=	AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2();

	mapCtrlSetIter		Iter		=	m_mapCtrlSet.begin();
	for( ; Iter != m_mapCtrlSet.end() ; ++Iter )
	{
		stEffectCtrlSetEntry*	pEntry	=	&Iter->second;
		if( pEntry )
		{
			pAgcmEff->EnumCallback( AGCMEFF2_CB_ID_REMOVEEFFECT , pEntry->m_pcdEffCtrl_Set , pEntry->m_pcdEffCtrl_Set->GetPtrCBInfo()->m_pBase );
			DEF_SAFEDELETE( pEntry->m_pcdEffCtrl_Set );
		}
	}

	mapPreSetIter		PreSetIter		=	m_mapPreSet.begin();
	for( ; PreSetIter != m_mapPreSet.end() ; ++PreSetIter )
	{
		stEffectPreSetEntry*	pEntry	=	&PreSetIter->second;
		if( pEntry )
			DEF_SAFEDELETE( pEntry->m_pcdPreSet );
	}

	m_mapPreSet.clear();
	m_mapCtrlSet.clear();

	m_dwInsID = 0;
}

void	AgcdEffCtrlMng::InsertCtrlSet( LPEFFCTRL_SET pEffCtrl_Set )
{
	stEffectCtrlSetEntry NewEntry;

	NewEntry.m_pcdEffCtrl_Set	=	pEffCtrl_Set;
	NewEntry.m_nEffectID		=	pEffCtrl_Set->GetPtrEffSet()->bGetID();
	NewEntry.m_nCreateID		=	++m_dwInsID;

	NewEntry.m_pcdEffCtrl_Set->SetID( NewEntry.m_nCreateID );
	m_mapCtrlSet.insert( make_pair( NewEntry.m_nCreateID , NewEntry ) );
}

BOOL	AgcdEffCtrlMng::RemoveCtrl( LPEFFCTRL_SET pEffCtrlSet )
{
	if( !pEffCtrlSet )
		return FALSE;

	for( mapCtrlSetIter	Iter = m_mapCtrlSet.begin(); Iter != m_mapCtrlSet.end(); ++Iter )
	{
		stEffectCtrlSetEntry &	entry	=	Iter->second;

		if( entry.m_pcdEffCtrl_Set	==	pEffCtrlSet )
		{
			DEF_SAFEDELETE( pEffCtrlSet );
			m_mapCtrlSet.erase( Iter );
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL	AgcdEffCtrlMng::RemoveEffSet( LPEFFCTRL_SET pEffCtrlSet , BOOL bCallCB/* =TRUE */ )
{

	if( !pEffCtrlSet )	
		return FALSE;

	const AgcdEffSet* pEffSet = pEffCtrlSet->GetPtrEffSet();
	if( !pEffSet )
	{
		pEffCtrlSet->SetLife( AGCMEFFCTRL_IMMEDIATE_FINISH_LIFE_VALUE );
		return TRUE;
	}

	if( !pEffCtrlSet->GetPtrClumpParent() )
		return TRUE;

	CCSLog	stLog( 1 );
	static DebugValueUINT32	_sEffectIndex( 0 , "LastRemovedEffect = %u" );
	static DebugValueUINT32	_sFlag( 0 , "EffectFlage = %X" );
	_sEffectIndex	= pEffSet->bGetID();
	_sFlag			= pEffSet->bGetFlag();

	pEffCtrlSet->RemFromRenderOrOctree();
	pEffCtrlSet->End(bCallCB);

	return TRUE;
}

void		AgcdEffCtrlMng::InsertPreEff( LPEFFSET pEffSet, stEffUseInfo* Info, AgcdEffCtrl_Set* pNewEffCtrl_Set)
{
	stEffectPreSetEntry NewEntry;

	NewEntry.m_pcdPreSet = new AgcdPreEffSet;
	NewEntry.m_pcdPreSet->m_pstEffUseInfo.CloneEffUseInfo( Info );
	NewEntry.m_pcdPreSet->m_pEffCtrl_Set = pNewEffCtrl_Set;
	//NewEntry.m_nCreateID = m_mapPreSet.size();
	NewEntry.m_nCreateID = m_mapPreSet.empty() ? 0 : m_mapPreSet.rbegin()->second.m_nCreateID + 1;

	m_mapPreSet.insert( make_pair( NewEntry.m_nCreateID , NewEntry ) );
}

bool	AgcdEffCtrlMng::ValidParentClump(stEffUseInfo* pInfo)
{
	if( !pInfo )			return false;
	if( !pInfo->m_pBase )	return true;		//. 반드시 있어야하는 것은 아니므로.
		
	switch( pInfo->m_pBase->m_eType )
	{
	case APBASE_TYPE_OBJECT:
		{
			ApdObject	*pdObject			= (ApdObject *)(pInfo->m_pBase);
			if(pdObject)
			{
				AgcdObject	*pcObject	= AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->m_pcsAgcmObject->GetObjectData(pdObject);
				if(pcObject && 
				   pcObject->m_stGroup.m_pstList && 
				   pcObject->m_stGroup.m_pstList->m_csData.m_pstClump)
					return true;
			}
		}
		return false;

	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter* pdChracter		= (AgpdCharacter*)(pInfo->m_pBase);
			if(pdChracter)
			{
				AgcdCharacter* pcdCharacter	= AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->m_pcsAgcmCharacter->GetCharacterData(pdChracter);
				if(pcdCharacter && pcdCharacter->m_pClump)
					return true;
			}
		}
		return false;

	case APBASE_TYPE_ITEM:
		{
			AgpdItem	*pdItem			= (AgpdItem *)(pInfo->m_pBase);
			if(pdItem)
			{
				AgcdItem* pcdItem = AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect()->m_pcsAgcmItem->GetItemData(pdItem);
				if(pcdItem && pcdItem->m_pstClump)
					return true;
			}
		}
		return false;
	}
	
	return false;
}


void	AgcdEffCtrlMng::UpdateLoadedEffCtrlList()
{
	AgcmEff2 * pcmEffect = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2();

	for( mapPreSetIter iter = m_mapPreSet.begin(); iter != m_mapPreSet.end(); ++iter )
	{
		AgcdPreEffSet* preset = iter->second.m_pcdPreSet;

		if( preset )
		{
			if( ValidParentClump( &preset->m_pstEffUseInfo ) )
			{
				//if( pEntry->m_pcdPreSet->m_pEffset->m_enumLoadStatus == AGCDEFFSETRESOURCELOADSTATUS_LOADED )
				{
					pcmEffect->LockFrame();
					AgcdEffCtrl_Set* pEffectNode = pcmEffect->CreateEffSet( preset->m_pEffset, &preset->m_pstEffUseInfo, preset->m_pEffCtrl_Set );
					pcmEffect->UnlockFrame();

					if( pEffectNode )
					{
						AgcmEventEffect* pcmEventEffect = AgcdEffGlobal::bGetInst().bGetPtrAgcmEventEffect();
						if( pcmEventEffect )
						{
							AgcmEventEffectList* pstEffectList = pcmEventEffect->GetEventEffectList( preset->m_pstEffUseInfo.m_pBase);
							if( pstEffectList )
							{
								pstEffectList->AddUseEffectList( pEffectNode );
							}
						}

						DEF_SAFEDELETE( preset );
					}
				}
			}
			else
			{
				DEF_SAFEDELETE( preset );
			}
		}
	}

	m_mapPreSet.clear();
}

void	AgcdEffCtrlMng::TimeUpdate(RwUInt32 dwDifTick)
{
	EffCtrlMapEraser< mapCtrlSet > eraser( m_mapCtrlSet );

	mapCtrlSetIter iter = m_mapCtrlSet.begin();

	while( eraser.next( iter ) )
	{
		AgcdEffCtrl_Set * effCtrlSet = iter->second.m_pcdEffCtrl_Set;
		if( effCtrlSet )
		{
			if( effCtrlSet->TimeUpdate( dwDifTick ) )
			{
				RemoveEffSet( effCtrlSet, FALSE );
				DEF_SAFEDELETE( effCtrlSet );
				eraser.reserve();
			}
		}
		else
		{
			eraser.reserve();
		}
	}
}

void AgcdEffCtrlMng::ToneDown()
{
	mapCtrlSetIter		Iter		=	m_mapCtrlSet.begin();
	for( ; Iter != m_mapCtrlSet.end() ; ++Iter )
	{
		stEffectCtrlSetEntry* pEntry	=	&Iter->second;
		if( pEntry && IsVaildCtrlSet( pEntry->m_pcdEffCtrl_Set ) )
		{
			pEntry->m_pcdEffCtrl_Set->ToneDown();
		}
	}
}

void AgcdEffCtrlMng::ToneRestore()
{

	mapCtrlSetIter		Iter		=	m_mapCtrlSet.begin();
	for( ; Iter != m_mapCtrlSet.end() ; ++Iter )
	{
		stEffectCtrlSetEntry*	pEntry	=	&Iter->second;
		if( pEntry && IsVaildCtrlSet( pEntry->m_pcdEffCtrl_Set ) )
		{
			pEntry->m_pcdEffCtrl_Set->ToneRestore();
		}
	}

}

void AgcdEffCtrlMng::RemoveAllEffectByClump( RpClump * pClump )
{
	if( !pClump )		return;

	EffCtrlMapEraser< mapCtrlSet > eraser( m_mapCtrlSet );
	
	mapCtrlSetIter	iter  = m_mapCtrlSet.begin();

	while( eraser.next(iter) )
	{
		AgcdEffCtrl_Set * effCtrlSet = iter->second.m_pcdEffCtrl_Set;
		if( effCtrlSet )
		{	
			if( effCtrlSet->GetPtrClumpEmiter() == pClump || effCtrlSet->GetPtrClumpParent() == pClump )
			{
				RemoveEffSet( effCtrlSet, FALSE );
				DEF_SAFEDELETE( effCtrlSet );
				eraser.reserve();
			}
		}
	}
}

RwInt32	AgcdEffCtrlMng::ForTool_DeleteEff(RwUInt32 dwEffSetID)
{
	RwInt32	nDeleteCount = 0;

	EffCtrlMapEraser< mapCtrlSet > eraser( m_mapCtrlSet );

	mapCtrlSetIter iter = m_mapCtrlSet.begin();
	
	while( eraser.next( iter ) )
	{
		AgcdEffCtrl_Set * effCtrlSet = iter->second.m_pcdEffCtrl_Set;
		if( effCtrlSet && IsVaildCtrlSet( effCtrlSet ) )
		{
			if( effCtrlSet->GetPtrEffSet()->bGetID() == dwEffSetID )
			{
				effCtrlSet->End( FALSE );
				DEF_SAFEDELETE( effCtrlSet );
				eraser.reserve();
				++nDeleteCount;
			}
		}
	}

	return nDeleteCount;
}

BOOL AgcdEffCtrlMng::FindEffectSet( AgcdEffCtrl_Set* pcdEffCtrl_Set )
{
	if( !IsVaildCtrlSet( pcdEffCtrl_Set ) ) return FALSE;

	if( m_mapCtrlSet.end() != m_mapCtrlSet.find( pcdEffCtrl_Set->GetID() ) )
		return TRUE;

	return FALSE;
}

BOOL AgcdEffCtrlMng::IsVaildCtrlSet( AgcdEffCtrl_Set* pcdEffCtrl_Set )
{
	if( pcdEffCtrl_Set->GetPtrEffSet()->m_enumLoadStatus == AGCDEFFSETRESOURCELOADSTATUS_LOADING)	return FALSE;

	return TRUE;
}

void AgcdEffCtrlMng::ClearPreSet( void )
{
	for( mapPreSetIter iter = m_mapPreSet.begin(); iter != m_mapPreSet.end(); ++iter )
		DEF_SAFEDELETE( iter->second.m_pcdPreSet );

	m_mapPreSet.clear();

}
/*
void AgcdEffCtrlMng::DeletePreSet( AgcdEffCtrl_Set* pcdEffCtrl_Set )
{
	for( mapPreSetIter Iter = m_mapPreSet.begin(); Iter != m_mapPreSet.end(); ++Iter )
	{
		stEffectPreSetEntry*	pEntry	=	&Iter->second;
		if( pEntry )
		{
			if( pEntry->m_pcdPreSet->m_pEffCtrl_Set == pcdEffCtrl_Set )
			{
				pEntry->m_pcdPreSet->CleanUp();
				DEF_SAFEDELETE( pEntry->m_pcdPreSet );

				m_mapPreSet.erase( Iter );
				break;
			}
		}
	}

}*/
