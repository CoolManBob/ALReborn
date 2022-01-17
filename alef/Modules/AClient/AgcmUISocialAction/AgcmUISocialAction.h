#pragma once

#include "AgcmSocialAction.h"
#include "AgcmCharacter.h"
#include "AgcmUIMain.h"

class AgcmUISocialAction : public AgcModule, public AgcmSocialActon
{
//Managerµé...
public:
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgcmUIMain*			m_pcsAgcmUIMain;

public:
	AgcmUISocialAction();
	~AgcmUISocialAction();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	//Function
	static BOOL	CBSocialActionOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSocialActionCloseUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSocialActionToggleUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionDragDrop(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionDragEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionDLClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionRClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBSocialActionNotFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL ActionStart( INT32 nItem );

private:
	BOOL AddEvent()		{	return TRUE;	}
	BOOL AddFunction()	{	return TRUE;	}
	BOOL AddUserData()	{	return TRUE;	}
	BOOL AddDisplay()	{	return TRUE;	}

private:
	BOOL			m_bIsOpen;
	INT32			m_nEventSocialOpen;
	INT32			m_nEventSocialClose;
	AgcdUIUserData*	m_pcsUserDataGrid;
};