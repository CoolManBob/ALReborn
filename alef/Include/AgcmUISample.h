// AgcmUISample.h: interface for the AgcmUISample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMUISAMPLE_H__9C51313F_2898_427B_A143_E709F20874C8__INCLUDED_)
#define AFX_AGCMUISAMPLE_H__9C51313F_2898_427B_A143_E709F20874C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"
#include "AgcmUIManager2.h"
#include "AgpmGrid.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUISampleD" )
#else
#pragma comment ( lib , "AgcmUISample" )
#endif
#endif

typedef enum
{
	UIS_DID_NAME	= 0,
	UIS_DID_HP,
	UIS_DID_MP,
	UIS_DID_SP,
} AgcdUISampleDID;

class AgcmUISample : public AgcModule  
{
private:
	AgcmUIManager2 *	m_pcsAgcmUIManager2;
	AgpmGrid *			m_pcsAgpmGrid;
	INT32				m_alValue[20];
	INT32				m_alDepth[20];
	INT32				m_lMaxValue;

	INT32				m_lDisplayID;
	INT32				m_lEventID;

	AgpdGrid			m_astSkill[3];
	AgpdGrid			m_astGrid[2];
	AgcdUIUserData *	m_pstUserData;
	AgcdUIUserData *	m_pstUserData2;
	AgcdUIUserData *	m_pstGrid;
	AgcdUIUserData *	m_pstSkill;
	AgcdUIUserData *	m_pstDepth;

	AcUIBase *			m_pcsCustomBase;

public:
	AgcmUISample();
	virtual ~AgcmUISample();

	BOOL			OnAddModule();
	BOOL			OnInit();

	BOOL			OnIdle(UINT32 ulClockCount);

	BOOL			OnDestroy();

	static BOOL		CBTest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBDisplayCharacterInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue);
	static BOOL		CBReturnTRUE(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
	static BOOL		CBReturnFALSE(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);
};

#endif // !defined(AFX_AGCMUISAMPLE_H__9C51313F_2898_427B_A143_E709F20874C8__INCLUDED_)
