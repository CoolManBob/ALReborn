#pragma once

#include "ApModule.h"

#include "AgpmEventGacha.h"
#include "ApmEventManager.h"
#include "ApmEventManagerDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CAgcmEventGachaDlgApp
// See AgcmEventGachaDlg.cpp for the implementation of this class
//

class AFX_EXT_CLASS AgcmEventGachaDlg : public ApmEventDialog
{
public:
	AgpmEventGacha		*m_pcsAgpmEventGacha	;
	ApmEventManager		*m_pcsApmEventManager	;
	ApmEventManagerDlg	*m_pcsApmEventManagerDlg;

public:
	AgcmEventGachaDlg();
	virtual ~AgcmEventGachaDlg();

	// Virtual Function ต้
	BOOL	OnAddModule();

	BOOL	Open(ApdEvent *pcsEvent);
	BOOL	Close();

	static BOOL	CBAddGachaEvent(PVOID pData, PVOID pClass, PVOID pCustData);
};