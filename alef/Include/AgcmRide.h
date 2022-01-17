#ifndef _AGCMRIDE_H_
#define _AGCMRIDE_H_

#include "ApModule.h"
#include "AgpmRide.h"

class AgpmItem;
class AgcmItem;
class AgpmCharacter;
class AgcmCharacter;

class AgcmRide : public ApModule
{
public:
	AgcmRide();
	~AgcmRide();

	virtual BOOL		OnAddModule();
	virtual BOOL		OnInit();
	virtual BOOL		OnDestroy();

	// Callback functions
	static BOOL			CBRideAck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideDismountAck(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideTimeOut(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBRideTID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBInitCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	static AgcmRide*	m_pThis;

	AgpmItem*			m_pcsAgpmItem;
	AgcmItem*			m_pcsAgcmItem;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgpmRide*			m_pcsAgpmRide;
};

#endif
