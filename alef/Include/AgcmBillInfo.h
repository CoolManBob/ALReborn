#if !defined(__AGCMBILLINFO_H__)
#define	__AGCMBILLINFO_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmBillInfoD" )
#else
#pragma comment ( lib , "AgcmBillInfo" )
#endif
#endif

#include "AgpmBillInfo.h"

#include "AgcModule.h"

class AgcmUIControl;
class AgpmGrid;
class AgcmUIMain;
class AgpdGridItem;

class AgcmBillInfo : public AgcModule {
private:
	AgpmBillInfo	*m_pcsAgpmBillInfo;

public:
	AgcmBillInfo										( VOID );
	virtual ~AgcmBillInfo								( VOID );

	BOOL					OnAddModule					( VOID );
	BOOL					OnInit						( VOID );
	static BOOL				CBUpdateBillInfo			( PVOID pData, PVOID pClass, PVOID pCustData );

protected:
	VOID					_AddSystemGridHangameS		( VOID );
	VOID					_AddSystemGridHangameTPack	( VOID );

	AgpdGridItem*			m_pGridHangameS;
	AgpdGridItem*			m_pGridHangameTPack;

	AgcmUIControl*			m_pcsAgcmUIControl;
	AgpmGrid*				m_pcsAgpmGrid;
	AgcmUIMain*				m_pcsAgcmUIMain;

};

#endif	//__AGCMBILLINFO_H__