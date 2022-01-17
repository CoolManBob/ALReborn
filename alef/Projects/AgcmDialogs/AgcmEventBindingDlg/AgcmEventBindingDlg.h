#ifndef _AGCMEVENTBINDINGDLG_H_
#define _AGCMEVENTBINDINGDLG_H_

#include "ApModule.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"
#include "AgpmEventBinding.h"
#include "AgpmFactors.h"
#include "ApmMap.h"

class AFX_EXT_CLASS AgcmEventBindingDlg : public ApmEventDialog  
{
private:
	ApmEventManager			*m_pcsApmEventManager;
	ApmEventManagerDlg		*m_pcsApmEventManagerDlg;
	AgpmEventBinding		*m_pcsAgpmEventBinding;
	AgpmFactors				*m_pcsAgpmFactors;
	ApmMap					*m_pcsApmMap;

public:
	AgcmEventBindingDlg();
	virtual ~AgcmEventBindingDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();
	BOOL	Open(ApdEvent *pstEvent);
	void	Close();
};

#endif