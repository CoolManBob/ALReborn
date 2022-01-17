// SubWindow.cpp: implementation of the SubWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rtcharse.h"

#include <skeleton.h>
#include <menu.h>
//#include "events.h"
#include <camera.h>
#include <AgcEngine.h>
#include <MagDebug.h>
#include "MainWindow.h"
#include "SubWindow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SubWindow::SubWindow()
{
	m_pRaster	= NULL;
}

SubWindow::~SubWindow()
{

}

BOOL SubWindow::OnInit			(					)	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
{
	m_pRaster = RsRasterReadFromBitmap( "86.bmp" );
	return TRUE;
}

BOOL SubWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

BOOL SubWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	return FALSE;
}

void SubWindow::OnRender		( RwRaster *raster	)	// 렌더는 여기서;
{
 	if( m_pRaster )
	{
		RwRasterPushContext	( raster );
		RwRasterRender		( m_pRaster , this->x , this->y );
		RwRasterPopContext	();
	}
}
void SubWindow::OnClose		(					)	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
{
	if( m_pRaster )
	{
		RwRasterDestroy( m_pRaster );
	}
}
