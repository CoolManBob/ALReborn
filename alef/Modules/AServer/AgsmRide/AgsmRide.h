#pragma once

#include "AgsEngine.h"
#include "ApmMap.h"
#include "AgpmRide.h"
#include "AgpmItem.h"
#include "AgpmFactors.h"
#include "AgsmSkill.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmSummons.h"
#include "AgsmAOIFilter.h"

struct AgsdADRideData
{
	UINT32 m_lStartTime;
	UINT32 m_lPrevTime;
	INT32 m_lStep;
};

class AgsmRide : public AgsModule
{
private:
	ApmMap			*m_pcsApmMap;
	AgpmRide		*m_pcsAgpmRide;
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgsmSkill		*m_pcsAgsmSkill;
	AgsmCharacter	*m_pcsAgsmCharacter;
	AgsmItem		*m_pcsAgsmItem;
	AgsmSummons		*m_pcsAgsmSummons;
	AgsmAOIFilter	*m_pcsAgsmAOIFilter;

	INT32			m_lIndexAttachData;

public:
	AgsmRide(void);
	~AgsmRide(void);

	virtual BOOL OnAddModule();

	virtual BOOL OnInit();
	virtual BOOL OnDestroy();
	virtual BOOL OnIdle(UINT32 ulClockCount);

	static BOOL CBRideReq(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDismountReq(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReCalcFactors(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharItemEquip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEnterGameWorld(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRideReqBySkill(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdADRideData* GetAttachRideData(AgpdCharacter *pcsCharacter);
	BOOL UpdateRideDurability(AgpdCharacter *pcsCharacter, INT32 lDurability, UINT32 ulClockCount, INT32 lItemID = 0);
	BOOL IsFreeDurationItem(AgpdCharacter *pcsCharacter);

	BOOL AutoRide(AgpdCharacter* pcsAgpdCharacter);
	BOOL RideReq(AgpdCharacter* pcsAgpdCharacter, INT32 ItemID);
	BOOL Ride(AgpdCharacter* pcsAgpdCharacter, INT32 lItemID, INT32 lRideTID = 0);
	BOOL DismountReq(AgpdCharacter* pcsAgpdCharacter, INT32 ItemID, BOOL bBySkill = FALSE);
	BOOL RegionChange(AgpdCharacter* pcsAgpdCharacter, INT16 nPrevRegionIndex);

	BOOL CheckRideItem(AgpdCharacter* pcsAgpdCharacter, INT32 lItemID);
	BOOL CheckRideRegion(AgpdCharacter* pcsAgpdCharacter);

	BOOL MakeAndSendPacket(INT8 cOperation, AgpdCharacter *pcsCharacter, INT32 lRideItemID, INT32 lRemainTime);
};
