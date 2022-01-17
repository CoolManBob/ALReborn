// AgcmRWThread.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AgcmRWThread.h"
#include "skeleton.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
AGCMRWTHREAD_API int nAgcmRWThread=0;

// This is an example of an exported function.
AGCMRWTHREAD_API int fnAgcmRWThread(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see AgcmRWThread.h for the class definition
AgcmRWThread::AgcmRWThread()
{
	SetModuleName("AgcmRWThread");

	return; 
}

VOID		AgcmRWThread::SetInstance(HINSTANCE hInstance)
{
	m_hWnd = (HWND) hInstance;
}

BOOL		AgcmRWThread::OnAddModule()
{
	g_pEngine = (AgcEngine *) GetModuleManager();

	return TRUE;
}

BOOL		AgcmRWThread::OnInit()
{
    RwEngineOpenParams  openParams;

	/*
    m_hWnd = CreateWindow("a", "a",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        1, 1,
                        (HWND)NULL, (HMENU)NULL, (HINSTANCE)m_hInstance, NULL);
	*/

    if (!RwEngineInit(NULL, 0, 20 * 1024))
    {
        return (FALSE);
    }

    if (!RpWorldPluginAttach())
    {
        return FALSE;
    }

	if (!RpMatFXPluginAttach())
	{
		return FALSE;
	}

	if (!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpSkinPluginAttach())
	{
		return FALSE;
	}

    if (!RpCollisionPluginAttach() )
    {
        return FALSE;
    }

	if (!RpSplinePluginAttach() )
	{
		return FALSE;
	}

	if(!RpMorphPluginAttach())
	{
		return FALSE;
	}

	if( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}

	if( !RpLODAtomicPluginAttach() )
	{
		return FALSE;
	}

	if (!RtAnimInitialize())
	{
		return FALSE;
	}

    openParams.displayID = m_hWnd;

    if (!RwEngineOpen(&openParams))
    {
        RwEngineTerm();
        return (FALSE);
    }

	RwD3D9EngineSetMultiThreadSafe(TRUE);
	RwEngineSetSubSystem(0);
	RwEngineSetVideoMode(0);

    if (!RwEngineStart())
    {
        RwEngineClose();
        RwEngineTerm();
        return (FALSE);
    }

	RwBBox	bbox = {-10000000, -10000000, 10000000, 10000000};

	m_pWorld = RpWorldCreate(&bbox);
	if (!m_pWorld)
		return FALSE;

	AcuCamera	csCamera;
	m_pCamera = csCamera.CameraCreate(1024,768,0);
	if (!m_pCamera)
		return FALSE;

	RpWorldAddCamera(m_pWorld, m_pCamera);

	AcuObject::SetCamera(m_pCamera);

	return TRUE;
}

BOOL		AgcmRWThread::OnDestroy()
{
	AcuCamera	csCamera;

	csCamera.CameraDestroy(m_pCamera);

	RpWorldDestroy(m_pWorld);

    RwEngineStop();
    RwEngineClose();
    RwEngineTerm();

	return TRUE;
}

BOOL			AgcmRWThread::InstanceAtomic(RpAtomic *pstAtomic)
{
	// Camera가 없으면, 당근 안된다.
	if (!m_pCamera)
		return FALSE;

	if (!RpAtomicGetGeometry(pstAtomic))
		return TRUE;

	/*
	if (!RpSkinGeometryGetSkin(RpAtomicGetGeometry(pstAtomic)))
		return TRUE;
	*/

	TRACE("Instancing Start.....\n");
	// Camera Begin Update가 안되도 안된다.
//	if (!RwCameraBeginUpdate(m_pCamera))
//		return FALSE;

	// Instancing

	if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(pstAtomic)))
	{
		/*
		RxPipeline *				pPipeline;
		RxPipelineNodeInstance *	pNode;
		
		RpAtomicGetPipeline(pstAtomic, &pPipeline);
		pNode = (RxPipelineNodeInstance *) RxPipelineFindNodeByIndex(pPipeline, 0);

		_rxD3D9SkinInstanceNodeData *	pNodeData = (_rxD3D9SkinInstanceNodeData *) pNode->privateData;
		RxD3D9AllInOneRenderCallBack	pRenderCB = pNodeData->renderCallback;
		RxD3D9AllInOneLightingCallBack	pLightingCB = pNodeData->lightingCallback;

		pNodeData->renderCallback = NULL;
		pNodeData->lightingCallback = NULL;
		*/

		RpAtomicInstance(pstAtomic);

		/*
		pNodeData->renderCallback = pRenderCB;
		pNodeData->lightingCallback = pLightingCB;
		*/
	}

//	RwCameraEndUpdate(m_pCamera);
	TRACE("Instancing End.....\n");

	return TRUE;
}

RpAtomic *		AgcmRWThread::CBInstanceAtomic(RpAtomic *pstAtomic, PVOID pvData)
{
	INT32			lIndex;
	BOOL			bIsSkin;
	BOOL			bLODInstance;
	AgcmRWThread *	pThis = (AgcmRWThread *) pvData;

	if (!pThis->InstanceAtomic(pstAtomic))
		return NULL;

	/*
	for (lIndex = 1; lIndex < RPLODATOMICMAXLOD; ++lIndex)
	{
		if (!RpLODAtomicSetCurrentLOD(pstAtomic, lIndex))
			continue;

		if((bIsSkin) && (!AcuObject_RwUtilAtomicHasSkin(pstAtomic)))
			bLODInstance = FALSE;
		else
			bLODInstance = TRUE;

		if(bLODInstance)
		{
			if (!InstanceAtomic(pstAtomic))
				return NULL;
		}
	}
	*/

	return pstAtomic;
}

BOOL			AgcmRWThread::InstanceClump(RpClump *pstClump)
{
	if (!RpClumpForAllAtomics(pstClump, CBInstanceAtomic, this))
		return FALSE;

	return TRUE;
}
/*
BOOL		AgcmRWThread::InstanceClump(RpClump *pstClump)
{
	return AcuObject::InstanceClump(pstClump);
}
*/


