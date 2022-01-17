#ifndef	__AGCMITEMCONVERT_H__
#define	__AGCMITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmItemConvertD" )
#else
#pragma comment ( lib , "AgcmItemConvert" )
#endif
#endif

#include "AgpmItemConvert.h"

#include "AgcmCharacter.h"
#include "AgcmItem.h"

class AgcmItemConvert : public AgcModule {
private:
	AgpmItemConvert		*m_pcsAgpmItemConvert;

public:
	AgcmItemConvert();
	virtual ~AgcmItemConvert();

	BOOL				OnAddModule();

	BOOL				SendRequestPhysicalConvert(INT32 lCID, INT32 lIID, INT32 lCatalystIID);
	BOOL				SendRequestSocketAdd(INT32 lCID, INT32 lIID);
	BOOL				SendRequestRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneID);
	BOOL				SendCheckCashRuneConvert(INT32 lCID, INT32 lIID, INT32 lRuneID);
	BOOL				SendSocketInitialize(INT32 lCID, INT32 lIID, INT32 lCatalystIID);

	AgpdItemConvertResult			IsPhysicalConvertable(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem);
	AgpdItemConvertSocketResult		IsSocketConvertable(AgpdItem *pcsItem);
	AgpdItemConvertRuneResult		IsRuneConvertable(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);

	AgpdItemConvertResult			PhysicalConvert(AgpdItem *pcsItem, AgpdItem *pcsCatalystItem);
	AgpdItemConvertSocketResult		SocketConvert(AgpdItem *pcsItem);
	AgpdItemConvertRuneResult		RuneConvert(AgpdItem *pcsItem, AgpdItem *pcsRuneItem);

	static BOOL			CBResultPhysicalConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBResultSocketConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBResultSpiritStoneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBResultRuneConvert(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGCMITEMCONVERT_H__