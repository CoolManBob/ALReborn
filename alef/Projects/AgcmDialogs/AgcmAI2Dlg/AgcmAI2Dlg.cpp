#include "stdafx.h"
#include "../resource.h"

#include "AgcmAI2Dlg.h"

#include "AgcmAI2Dialog.h"

AgcmAI2Dialog			g_csAI2Dialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AgcmAI2Dlg::AgcmAI2Dlg()
{
	SetModuleName("AgcmAI2Dlg");

	ZeroMemory( &m_pcsAgpmCharacter	, sizeof( m_pcsAgpmCharacter	) );
	ZeroMemory( &m_pcsAgpmItem		, sizeof( m_pcsAgpmItem			) );
	ZeroMemory( &m_pcsAgpmSkill		, sizeof( m_pcsAgpmSkill		) );
	ZeroMemory( &m_pcsAgpmAI2		, sizeof( m_pcsAgpmAI2			) );
}

AgcmAI2Dlg::~AgcmAI2Dlg()
{
}

BOOL	AgcmAI2Dlg::OnAddModule()
{
	//아래 코드는 실제로 쓰이지 않는다. Spawn에서 멤머로 AgcmAI2Dlg를 가지고 있기때문이다.
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmSkill = (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmAI2 = (AgpmAI2 *) GetModule("AgpmAI2");

	if( !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsAgpmSkill || !m_pcsAgpmAI2 )
		return FALSE;

	return TRUE;
}

BOOL AgcmAI2Dlg::InitData( AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2 )
{
	m_pcsAgpmCharacter = pcsAgpmCharacter;
	m_pcsAgpmItem = pcsAgpmItem;
	m_pcsAgpmSkill = pcsAgpmSkill;
	m_pcsAgpmAI2 = pcsAgpmAI2;

	return TRUE;
}

BOOL AgcmAI2Dlg::OpenAddTemplate( char *pstrTemplateName )
{
	//이런저런 작업을 해주고~
	AgpdAI2Template* pcsAI2Template = new AgpdAI2Template;
	pcsAI2Template->m_lTID = m_pcsAgpmAI2->GetEmptyTemplateID();

	if( !m_pcsAgpmAI2->m_aAI2Template.AddAITemplate( pcsAI2Template ) )
		return FALSE;

	strcat( pcsAI2Template->m_strTemplateName, pstrTemplateName );
	g_csAI2Dialog.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, pcsAI2Template );
	g_csAI2Dialog.DoModal();

	return TRUE;
}

BOOL	AgcmAI2Dlg::OpenUpdateTemplate(AgpdAI2Template *pcsAI2Template)
{
	if( !pcsAI2Template )		return FALSE;

	g_csAI2Dialog.InitData( m_pcsAgpmItem, m_pcsAgpmSkill, m_pcsAgpmAI2, pcsAI2Template );
	g_csAI2Dialog.DoModal();
	return TRUE;
}

void	AgcmAI2Dlg::Close()
{
}