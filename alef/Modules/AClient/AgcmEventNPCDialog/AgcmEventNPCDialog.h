#ifndef __AGCM_EVENT_NPCDIALOG_H__
#define __AGCM_EVENT_NPCDIALOG_H__



#include "AgcModule.h"
#include "CExNPC.h"



class AgcmEventNPCDialog : public AgcModule  
{
private:
	CExNPC											m_ExNPC;

public:
	AgcmEventNPCDialog( void );
	virtual ~AgcmEventNPCDialog( void );

public:
	BOOL			OnAddModule						( void );

public :
	BOOL			SetCallbackAriseNPCDialogEvent	( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	CExNPC*			GetExNPC						( void ) { return &m_ExNPC; }

public :
	static BOOL		CBRequestNPCDialog				( void* pData, void* pClass, void* pCustData );
	static BOOL		CBReceiveGrant					( void* pData, void* pClass, void* pCustData );
};

#endif