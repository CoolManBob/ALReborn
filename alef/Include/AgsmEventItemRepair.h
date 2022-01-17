//	AgsmEventItemRepair	module header file
//		- item repair event module (server side)
//			(클라이언트의 수리 요청을 실제 처리한다.)
///////////////////////////////////////////////////////////

#ifndef	__AGSMEVENTITEMREPAIR_H__
#define	__AGSMEVENTITEMREPAIR_H__


#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmLog.h"

#include "AgpmEventItemRepair.h"

#include "AgsmCharacter.h"
#include "AgsmItem.h"

class AgsmEventItemRepair : public AgsModule {
private:
	AgpmFactors*				m_pcsAgpmFactors;
	AgpmGrid*					m_pcsAgpmGrid;
	AgpmCharacter*				m_pcsAgpmCharacter;
	AgpmItem*					m_pcsAgpmItem;
	AgpmLog*					m_pcsAgpmLog;

	AgpmEventItemRepair*		m_pcsAgpmEventItemRepair;

	AgsmCharacter*				m_pcsAgsmCharacter;
	AgsmItem*					m_pcsAgsmItem;

public:
	AgsmEventItemRepair();
	~AgsmEventItemRepair();

	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnDestroy();

	static BOOL	CBItemRepairEventReq(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRepairItemReq(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBUpdateItemDurability(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL ItemRepairItemReq(AgpdCharacter *pcsCharacter, AgpdItemRepair *pcsRepairInfo);

	BOOL SendPacketItemRepair(EnumAgpmEventItemRepairOperation eOperation, INT32 lCID, EnumAgpmEventItemRepairResultCode eResult);
};

#endif	//__AGSMEVENTITEMREPAIR_H__