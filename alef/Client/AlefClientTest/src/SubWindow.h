// SubWindow.h: interface for the SubWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_)
#define AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcEngine.h"

class SubWindow : public AgcWindow  
{
public:
	RwRaster	*m_pRaster	;
	
	BOOL OnInit			(					);	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
	BOOL OnLButtonDown	( RsMouseStatus *ms	);
	BOOL OnRButtonDown	( RsMouseStatus *ms	);
	void OnRender		( RwRaster *raster	);	// 렌더는 여기서;
	void OnClose		(					);	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.

	SubWindow();
	virtual ~SubWindow();

};

#endif // !defined(AFX_SUBWINDOW_H__470281DC_D592_4149_AE2D_BEFF04C93603__INCLUDED_)
