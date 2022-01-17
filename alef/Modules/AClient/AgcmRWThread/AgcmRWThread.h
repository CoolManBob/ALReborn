#ifndef __AGCMRWTHREAD
#define __AGCMRWTHREAD


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AGCMRWTHREAD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AGCMRWTHREAD_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AGCMRWTHREAD_EXPORTS
#define AGCMRWTHREAD_API __declspec(dllexport)
#else
#define AGCMRWTHREAD_API __declspec(dllimport)
#endif

#include "ApBase.h"
#include "AgcEngine.h"

#include "AcuObject.h"

#include "rwplcore.h"
#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"
#include "rphanim.h"
#include "rpskin.h"
#include "rtanim.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "rpmorph.h"

// This class is exported from the AgcmRWThread.dll


typedef struct __rxD3D9SkinInstanceNodeData _rxD3D9SkinInstanceNodeData;
struct __rxD3D9SkinInstanceNodeData
{
    RxD3D9AllInOneRenderCallBack    renderCallback;
    RxD3D9AllInOneLightingCallBack  lightingCallback;
};

#pragma warning(disable:4275)

class AGCMRWTHREAD_API AgcmRWThread : public AgcModule
{
private:
	HWND		m_hWnd;
	HINSTANCE	m_hInstance;

	RpWorld *	m_pWorld;
	RwCamera *	m_pCamera;

public:
	AgcmRWThread(void);

	void		SetInstance(HINSTANCE hInstance);

	BOOL		OnAddModule();
	BOOL		OnInit();
	BOOL		OnDestroy();

	BOOL		InstanceClump(RpClump *pstClump);

	BOOL		InstanceAtomic(RpAtomic *pstAtomic);
	static RpAtomic *	CBInstanceAtomic(RpAtomic *pstAtomic, PVOID pvData);
};

extern AGCMRWTHREAD_API int nAgcmRWThread;

AGCMRWTHREAD_API int fnAgcmRWThread(void);
AGCMRWTHREAD_API void AgcmRWThread::SetInstance(HINSTANCE hInstance);


#endif // __AGCMRWTHREAD