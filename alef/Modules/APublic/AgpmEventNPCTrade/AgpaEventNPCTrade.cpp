#include "AgpmEventNPCTrade.h"

ApAdmin *AgpmEventNPCTrade::GetTemplate()
{
	return &m_csNPCTradeTemplate;
}

AgpdEventNPCTradeTemplate *AgpmEventNPCTrade::GetNPCTradeTemplate( INT32 lTemplateTID )
{
	AgpdEventNPCTradeTemplate **ppcsTemplate;

	ppcsTemplate = (AgpdEventNPCTradeTemplate **)m_csNPCTradeTemplate.GetObject( lTemplateTID );

	if( ppcsTemplate != NULL )
	{
		return *ppcsTemplate;
	}
	else
	{
		return NULL;
	}
}

ApAdmin *AgpmEventNPCTrade::GetGroupItemTemplate()
{
	return &m_csNPCTradeItemGroup;
}
