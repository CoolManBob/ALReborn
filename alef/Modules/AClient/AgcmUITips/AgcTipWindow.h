// AgcTipWIndow.h: interface for the AgcTipWIndow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCTIPWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_)
#define AFX_AGCTIPWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_

#include "AgcEngine.h"
#include "AcUIBase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AgcmUITips;

class AgcTipWindow : public AcUIBase  
{
private:
	AgcmUITips *	m_pcsAgcmUITips;

	RwTexture *		m_pstTipTexture;
	UINT32			m_ulPrevClockCount;

public:
	AgcTipWindow(AgcmUITips *pcsAgcmUITips = NULL);
	virtual ~AgcTipWindow();

	BOOL	OnInit();
	VOID	OnClose();

	VOID	OnWindowRender();
};

#endif // !defined(AFX_AGCTIPWINDOW_H__D2C88F8F_36DA_4689_B70B_10504B38CFE4__INCLUDED_)
