// AgcUIWindow.h: interface for the AgcUIWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUIWINDOW_H__22A9ABEB_B5BE_4731_B84D_BABE4BF79E3F__INCLUDED_)
#define AFX_AGCUIWINDOW_H__22A9ABEB_B5BE_4731_B84D_BABE4BF79E3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcmUIControl.h"

class AgcmUIManager2;
class AgcdUI;

class AgcUIWindow : public AcUIBase
{
private:
	AgcmUIManager2*										m_pcsAgcmUIManager2;
	AgcdUI*												m_pcsUI;
	BOOL												m_bRButtonDown;

public:
	AgcUIWindow( AgcmUIManager2* pcsAgcmUIManager2, AgcdUI *pcsUI );
	virtual ~AgcUIWindow( void ) {	}

	BOOL 					OnInit						( void ) { return TRUE; }
	void 					OnClose						( void ) {	}
	void					OnMoveWindow				( void );
	BOOL					OnLButtonDown				( RsMouseStatus *ms	);
	BOOL					OnRButtonDown				( RsMouseStatus *ms	);
	BOOL					OnRButtonUp					( RsMouseStatus *ms	);
	BOOL					OnLButtonDblClk				( RsMouseStatus *ms	);	
	BOOL					OnRButtonDblClk				( RsMouseStatus *ms	);	
	BOOL					OnMouseMove					( RsMouseStatus *ms );
	BOOL					OnKeyDown					( RsKeyStatus *ks );
	BOOL					OnKeyUp						( RsKeyStatus *ks );
	BOOL					OnCommand					( INT32	nID, PVOID pParam );

	BOOL					PreTranslateInputMessage	( RsEvent event, PVOID param );
	void					UpdateChildWindow			( void ) { WindowListUpdate(); }

	//@{ 2006/09/14 burumal
	INT32					GetUIDepth					( AgcdUI* pUI );

protected :
	void					GetUIDepthRecurse			( AgcdUI* pCurUI, INT32* pDepth );
	//@}
};

#endif // !defined(AFX_AGCUIWINDOW_H__22A9ABEB_B5BE_4731_B84D_BABE4BF79E3F__INCLUDED_)
