#ifndef __AgcmEventTeleportDlg_h_
#define __AgcmEventTeleportDlg_h_

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmEventTeleport.h"

class AFX_EXT_CLASS AgcmEventTeleportDlg : public ApmEventDialog  
{
private:
	ApmEventManager *		m_pcsApmEventManager;
	ApmEventManagerDlg *	m_pcsApmEventManagerDlg;
	AgpmEventTeleport *		m_pcsAgpmEventTeleport;

public:
	AgcmEventTeleportDlg();
	virtual ~AgcmEventTeleportDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	Open(ApdEvent *pstEvent);
	void	Close();
};

#endif