#include "AgcmUVAnimation.h"
#include "AgcmRender.h"
#include "AcuRpMatD3DFx.h"
#include <rpuvanim.h>
#include "rpmatfx.h"


AgcmUVAnimation*	AgcmUVAnimation::m_pThis	=	NULL;
AgcmUVAnimation::AgcmUVAnimation( void )
{
	SetModuleName( "AgcmUVAnimation" );
	EnableIdle( TRUE );

	m_nPrevTickCount = 0;
}


AgcmUVAnimation::~AgcmUVAnimation( void )
{
	m_listUVAnimation.clear();
}

BOOL AgcmUVAnimation::OnAddModule( void )
{
	AgcmRender* pcmRender = ( AgcmRender* )GetModule( "AgcmRender" );
	if( pcmRender )
	{
		if( !pcmRender->SetCallbackAddAtomic( CallBack_AddAtomic, this ) ) 
			return FALSE;

		if( !pcmRender->SetCallbackAddClump( CallBack_AddClump, this ) ) 
			return FALSE;

		//if( !pcmRender->SetCallbackRemoveAtomic( CallBack_DeleteAtomic, this ) ) 
		//	return FALSE;

		//if( !pcmRender->SetCallbackRemoveClump( CallBack_DeleteClump, this ) ) 
		//	return FALSE;
	}

	MatDestrucTorCallbackUV	=	Callback_UVDestructor;

	m_pThis	=	this;
	

	return TRUE;
}

BOOL AgcmUVAnimation::OnIdle( unsigned int nCurrTickCount )
{
	m_nPrevTickCount = m_nPrevTickCount ? m_nPrevTickCount : nCurrTickCount;

	float fSPF = ( float )( nCurrTickCount - m_nPrevTickCount ) * 0.001f;
	m_nPrevTickCount = nCurrTickCount;

	return UpdateUVAnimation( fSPF );
}

BOOL AgcmUVAnimation::OnDestroy( void )
{
	m_listUVAnimation.clear();
	return TRUE;
}

BOOL AgcmUVAnimation::UpdateUVAnimation( float fSPF )
{

	stUVAnimationEntry*	pEntry	=	NULL;
	ListUVAnimationIter	Iter	=	m_listUVAnimation.begin();
	while( Iter != m_listUVAnimation.end() )
	{
		pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_pMaterial )
		{
			if( IsVaildUVAnimation( pEntry->m_pMaterial ) )
			{
				RpMaterialUVAnimAddAnimTime( pEntry->m_pMaterial , fSPF );
				RpMaterialUVAnimApplyUpdate( pEntry->m_pMaterial );
				
				pEntry->m_fCurrentTime += fSPF;
			}
			else
			{
				m_listUVAnimation.erase( Iter++ );
				continue;
			}
		}

		++Iter;
	}

	return TRUE;

}

BOOL AgcmUVAnimation::AddUVAnimation( RpMaterial* pMaterial )
{
	if( !pMaterial ) return FALSE;
	if( !pMaterial->texture ) return FALSE;
	if( FindUVAnimation( pMaterial ) ) return FALSE;

	stUVAnimationEntry NewEntry;

	NewEntry.m_pMaterial = pMaterial;
	NewEntry.m_fCurrentTime = 0.0f;

	m_listUVAnimation.push_back( NewEntry );
	return TRUE;
}

BOOL AgcmUVAnimation::RemoveUVAnimation( RpMaterial* pMaterial )
{
	if( !pMaterial ) 
		return FALSE;

	stUVAnimationEntry*	pEntry	=	NULL;
	ListUVAnimationIter	Iter	=	m_listUVAnimation.begin();
	for( ; Iter != m_listUVAnimation.end() ; ++Iter )
	{
		pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_pMaterial == pMaterial )
		{
			m_listUVAnimation.erase( Iter );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmUVAnimation::FindUVAnimation( RpMaterial* pMaterial )
{
	if( !pMaterial ) return FALSE;

	stUVAnimationEntry*	pEntry	=	NULL;
	ListUVAnimationIter	Iter	=	m_listUVAnimation.begin();
	for( ; Iter != m_listUVAnimation.end() ; ++Iter )
	{
		pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_pMaterial == pMaterial )
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmUVAnimation::IsVaildUVAnimation( RpMaterial* pMaterial )
{
	if( !pMaterial || pMaterial->refCount <= 0 ) 
		return FALSE;

	RpMatFXMaterialFlags	pFlag	=	RpMatFXMaterialGetEffects( pMaterial );
	if( pFlag == 0 )
		return FALSE;

	//if( RPUVANIMMATERIALGETCONSTDATA(material) )
	//	return FALSE;

	if( !pMaterial->texture )	return FALSE;
	return TRUE;
}

BOOL AgcmUVAnimation::CallBack_AddAtomic( void* pData, void* pClass, void* pCustData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pClass;
	if( !pcmUVAnimation ) return FALSE;

	RpAtomic* pAtomic = ( RpAtomic* )pData;
	if( !pAtomic ) return FALSE;
	
	RpGeometry* pGeometry = RpAtomicGetGeometry( pAtomic );
	if( !pGeometry ) return FALSE;

	AgcmUVAnimation::CallBack_FindGeometry( pGeometry, pcmUVAnimation );
	RpLODAtomicForAllLODGeometries( pAtomic, CallBack_FindGeometry, pcmUVAnimation );
	return TRUE;
}

BOOL AgcmUVAnimation::CallBack_AddClump( void* pData, void* pClass, void* pCustData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pClass;
	if( !pcmUVAnimation ) return FALSE;

	RpClump* pClump = ( RpClump* )pData;
	if( !pClump ) return FALSE;

	RpClumpForAllAtomics( pClump, CallBack_FindAtomic, pcmUVAnimation );
	return TRUE;
}

BOOL AgcmUVAnimation::CallBack_DeleteAtomic( void* pData, void* pClass, void* pCustData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pClass;
	if( !pcmUVAnimation ) return FALSE;

	RpAtomic* pAtomic = ( RpAtomic* )pData;
	if( !pAtomic ) return FALSE;
	
	RpGeometry* pGeometry = RpAtomicGetGeometry( pAtomic );
	if( !pGeometry ) return FALSE;

	AgcmUVAnimation::CallBack_RemoveGeometry( pGeometry, pcmUVAnimation );
	RpLODAtomicForAllLODGeometries( pAtomic, CallBack_RemoveGeometry, pcmUVAnimation );
	return TRUE;
}

BOOL AgcmUVAnimation::CallBack_DeleteClump( void* pData, void* pClass, void* pCustData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pClass;
	if( !pcmUVAnimation ) return FALSE;

	RpClump* pClump = ( RpClump* )pData;
	if( !pClump ) return FALSE;

	RpClumpForAllAtomics( pClump, CallBack_RemoveAtomic, pcmUVAnimation );
	return TRUE;
}

RpAtomic* AgcmUVAnimation::CallBack_FindAtomic( RpAtomic* pAtomic, void* pData )
{
	AgcmUVAnimation::CallBack_AddAtomic( pAtomic, pData, NULL );	
	return pAtomic;
}

RpGeometry* AgcmUVAnimation::CallBack_FindGeometry( RpGeometry* pGeometry, void* pData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pData;
	if( !pcmUVAnimation ) return pGeometry;

	RpGeometryForAllMaterials( pGeometry, CallBack_FindMaterial, pcmUVAnimation );
	return pGeometry;
}

RpMaterial*	AgcmUVAnimation::CallBack_FindMaterial( RpMaterial* pMaterial, void* pData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pData;
	if( !pcmUVAnimation ) return pMaterial;

	if( RpMaterialUVAnimExists( pMaterial ) )
	{
		pcmUVAnimation->AddUVAnimation( pMaterial );
	}
	
	return pMaterial;
}

RpAtomic* AgcmUVAnimation::CallBack_RemoveAtomic( RpAtomic* pAtomic, void* pData )
{
	AgcmUVAnimation::CallBack_DeleteAtomic( pAtomic, pData, NULL );	
	return pAtomic;
}

RpGeometry* AgcmUVAnimation::CallBack_RemoveGeometry( RpGeometry* pGeometry, void* pData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pData;
	if( !pcmUVAnimation ) return pGeometry;

	RpGeometryForAllMaterials( pGeometry, CallBack_RemoveMaterial, pcmUVAnimation );
	return pGeometry;
}

RpMaterial*	AgcmUVAnimation::CallBack_RemoveMaterial( RpMaterial* pMaterial, void* pData )
{
	AgcmUVAnimation* pcmUVAnimation = ( AgcmUVAnimation* )pData;
	if( !pcmUVAnimation ) return pMaterial;

	if( RpMaterialUVAnimExists( pMaterial ) )
	{
		pcmUVAnimation->RemoveUVAnimation( pMaterial );
	}
	
	return pMaterial;
}

VOID	AgcmUVAnimation::Callback_UVConstructor( void* pData , void* pClass , void* pCustData )
{
	CallBack_AddAtomic( pData , m_pThis , 0 );
}

VOID	AgcmUVAnimation::Callback_UVDestructor( void* pData , void* pClass , void* pCustData )
{
	CallBack_RemoveMaterial( (RpMaterial*)pData , m_pThis );
}