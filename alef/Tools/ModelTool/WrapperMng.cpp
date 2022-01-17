#include "stdafx.h"
#include "WrapperMng.h"

WrapperMng::WrapperMng( AgcEngineChild& cEngineChild ) : m_cEngineChild( cEngineChild )
{
}


WrapperMng::~WrapperMng()
{
}

BOOL	WrapperMng::GetCharacterTemplate( INT32 lID, AgpdCharacterTemplate* pPublic, AgcdCharacterTemplate* pClient )
{
	pPublic = m_cEngineChild.GetAgpmCharacterModule()->GetCharacterTemplate( lID );
	pClient = m_cEngineChild.GetAgcmCharacterModule()->GetTemplateData( pPublic );

	return pPublic && pClient ? TRUE : FALSE;
}

BOOL	WrapperMng::GetItemTemplate( INT32 lID, AgpdItemTemplate* pPublic, AgcdItemTemplate* pClient )
{
	pPublic = m_cEngineChild.GetAgpmItemModule()->GetItemTemplate( lID );
	pClient = m_cEngineChild.GetAgcmItemModule()->GetTemplateData( pPublic );

	return pPublic && pClient ? TRUE : FALSE;
}

BOOL	WrapperMng::GetObjectTemplate( INT32 lID, ApdObjectTemplate* pPublic, AgcdObjectTemplate* pClient )
{
	pPublic = m_cEngineChild.GetApmObjectModule()->GetObjectTemplate( lID );
	pClient = m_cEngineChild.GetAgcmObjectModule()->GetTemplateData( pPublic );

	return pPublic && pClient ? TRUE : FALSE;
}