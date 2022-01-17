#ifndef _AGCMEVENTSPAWNDLG_H_
#define _AGCMEVENTSPAWNDLG_H_

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmEventSpawn.h"
#include "AgpmDropItem.h"
#include "AgpmSkill.h"
#include "AgpmAI2.h"
#include "AgcmFactorsDlg.h"

class AFX_EXT_CLASS AgcmEventSpawnDlg : public ApmEventDialog
{
public:
	ApmEventManager *		m_pcsApmEventManager;
	ApmEventManagerDlg *	m_pcsApmEventManagerDlg;
	AgpmEventSpawn *		m_pcsAgpmEventSpawn;
	AgpmCharacter *			m_pcsAgpmCharacter;
	AgpmItem *				m_pcsAgpmItem;
	AgpmSkill *				m_pcsAgpmSkill;
	AgpmDropItem *			m_pcsAgpmDropItem;
	AgcmFactorsDlg *		m_pcsAgcmFactorsDlg;
	AgpmAI2 *				m_pcsAgpmAI2;

	AgcmEventSpawnDlg();
	virtual ~AgcmEventSpawnDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	Open(ApdEvent *pstEvent);

	void	Close();
};

#endif