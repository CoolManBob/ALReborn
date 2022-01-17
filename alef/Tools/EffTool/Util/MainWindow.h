// MainWindow.h: interface for the MainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
#define AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcmEff2.h"
#include "AcUIControlHeader.h"



class MainWindow : public AgcWindow  
{
public:	
	BOOL		m_bLBtnCamRot;

public:
	MainWindow();
	virtual ~MainWindow();

	BOOL OnInit			(					);	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseMove	( RsMouseStatus *ms	);
	BOOL OnLButtonUp	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	BOOL OnMButtonUp	( RsMouseStatus *ms	);
	BOOL OnMButtonDown	( RsMouseStatus *ms	);
	BOOL OnMouseWheel	( INT32	lDelta		);
	BOOL OnRButtonUp	( RsMouseStatus *ms	);
	void OnPreRender	( RwRaster *raster	);	// 렌더는 여기서;
	void OnRender		( RwRaster *raster	);	// 렌더는 여기서;
	void OnClose		(					);	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
	BOOL OnKeyDown		( RsKeyStatus *ks	);
	BOOL OnKeyUp		( RsKeyStatus *ks	);
	BOOL OnIdle			(UINT32 ulClockCount);
};

#endif // !defined(AFX_MAINWINDOW_H__B1E2FB1E_7FBD_474B_9099_05DA4F586D90__INCLUDED_)
