#pragma once
#pragma warning(disable: 4786)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//Codejock
//#define _XTP_STATICKLINK
#include <XTToolkitPro.h>

//Windows
#include <Windows.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <stdio.h>
#include <mmsystem.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

//DirectX
#include <d3dx9core.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include <d3dx9tex.h>
#include <dxerr9.h>

//RenderWare
#include <skeleton.h>
#include <menu.h>
#include <camera.h>
#include <win.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rpcollis.h>
#include <rpusrdat.h>
#include <rpspline.h>
#include <rplodatm.h>
#include <rpmatfx.h>
#include <rphanim.h> 
#include <rpskin.h>
#include <rpmorph.h>
#include <rpnormmap.h>
#include <rtcharse.h>
#include <rtanim.h>
#include <rtfsyst.h>
#include <rtbmp.h>
#include <rtpng.h>
#include <rtquat.h>

//ApModule
#include "AcuMathFunc.h"
#include "AcuRpMtexture.h"

//AgcModule
#include <AgcEngine.h>
#include "AgcmFont.h"
#include "AgcuEffUtil.h"
#include "AgcuEffPath.h"
#include "AgcdEffSet.h"
#include "AgcdEffAnim.h"
#include "AgcdEffBase.h"
#include "AgcdEffObj.h"
#include "AgcdEffPublicStructs.h"
#include "AgcdEffGlobal.h"
#include "AgcmEff2.h"

//±×³É EffectTool Use Header File
#include "Util/Singleton.hpp"
#include "Util/BitmapLoader.h"
#include "Util/Axis.h"
#include "Util/Camera.h"
#include "Util/Arrow.h"
#include "Util/PickUser.h"
#include "Util/RenderWare.h"
#include "Util/EffUt_Vtx.h"
#include "Util/EffUt_Geo.h"
#include "Util/EffUt_HFMap.h"
#include "Util/ShowFrm.h"
#include "Util/Grid.h"
#include "Util/MainWindow.h"
#include "Util/MyEngine.h"


using namespace __EffUtil__;

void	ToWnd( LPCTSTR szInfo );
void	ErrToWnd( LPCTSTR szMsg, LPCTSTR szFunc=_T("unknown"), LPCTSTR szFile = __FILE__, INT nLine = __LINE__ );