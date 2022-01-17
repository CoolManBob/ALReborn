#pragma once

#include "ApModule.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmAI2.h"

class AFX_EXT_CLASS AgcmAI2Dlg : public ApModule
{
public:
	AgcmAI2Dlg();
	virtual ~AgcmAI2Dlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	OpenTemplate();

	BOOL	OpenAddTemplate( char *pstrTemplateName );
	BOOL	OpenUpdateTemplate(AgpdAI2Template *pcsAI2Template);

	BOOL	InitData( AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmAI2 *pcsAgpmAI2 );

	void	Close();

public:
	AgpmCharacter*	m_pcsAgpmCharacter;
	AgpmItem*		m_pcsAgpmItem;
	AgpmSkill*		m_pcsAgpmSkill;
	AgpmAI2*		m_pcsAgpmAI2;
};
