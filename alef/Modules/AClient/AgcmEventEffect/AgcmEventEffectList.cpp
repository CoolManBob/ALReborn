#include "AgcmEventEffectList.h"
#include "ApMemoryTracker.h"
#include "cslog.h"

BOOL AgcmEventEffectList::CreateList( VOID )
{
	if( m_pListUseEffect )
		return FALSE;

	m_pListUseEffect	=	new ListUseEffect;

	return TRUE;
}

BOOL AgcmEventEffectList::DestroyUseEffectList()
{
	m_pListUseEffect->clear();

	return TRUE;
}

BOOL AgcmEventEffectList::AddUseEffectList( EFFCTRL_SET* pNode )
{
	if ( CheckUseEffect(pNode) ) return FALSE;

	m_pListUseEffect->push_back( pNode );

	return TRUE;
}

BOOL AgcmEventEffectList::CheckUseEffect( EFFCTRL_SET* pNode )
{
	ListUseEffectIter		Iter		=	m_pListUseEffect->begin();
	EFFCTRL_SET*			pCtrlSet	=	NULL;

	for( ; Iter != m_pListUseEffect->end() ; ++Iter )
	{
		pCtrlSet		=	(*Iter);
		if( pNode == pCtrlSet )
			return TRUE;
	}

	return FALSE;
}

BOOL	AgcmEventEffectList::RemoveUseEffectList( EFFCTRL_SET* pNode )
{
	ListUseEffectIter		Iter		=	m_pListUseEffect->begin();
	EFFCTRL_SET*			pCtrlSet	=	NULL;

	for( ; Iter != m_pListUseEffect->end() ; ++Iter )
	{
		pCtrlSet		=	(*Iter);
		if( pCtrlSet	==	pNode )
		{
			m_pListUseEffect->erase( Iter );
			return TRUE;
		}
	}

	return FALSE;
}