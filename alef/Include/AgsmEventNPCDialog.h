#ifndef	__AGSMEVENTNPCDIALOG_H__
#define	__AGSMEVENTNPCDIALOG_H__

#include "AgpmEventNPCDialog.h"
#include "AgsmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventNPCDialogD" )
#else
#pragma comment ( lib , "AgsmEventNPCDialog" )
#endif
#endif

class AgsmNpcManager;

class AgsmEventNPCDialog : public AgsModule {
private:
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmEventNPCDialog		*m_pcsAgpmEventNPCDialog;
	AgsmCharacter			*m_pcsAgsmCharacter;
	AgsmNpcManager*			m_pcsAgsmNpcManager;

public:
	AgsmEventNPCDialog();
	virtual ~AgsmEventNPCDialog();

	BOOL					OnAddModule();
	BOOL					OnInit();

	static BOOL				CBNPCDialogEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	
};


#endif	//__AGSMEVENTNPCDIALOG_H__