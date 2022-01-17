#include <tchar.h>

#include <cassert>
#include <algorithm>
#include <fstream>
#include <strstream>
using namespace std;

#include <d3d9.h>
#include <d3dx9.h>

#include <rpcollis.h>
#include <rtSkinSp.h>
#include <rtfsyst.h>

#include "skel\win.h"
#include "skel\skeleton.h"
#include "skel\camera.h"

#include "AtlasCreation\TATypes.h"
#include "AtlasCreation\CmdLineOptions.h"
#include "AtlasCreation\TextureObject.h"
#include "AtlasCreation\AtlasContainer.h"

#include "MemoryManager\mmgr.h"

#include "AcuRpMatD3DFx.h"

#include "Scene.h"
#include "Control.h"

extern int doFileDlg(bool bOpenFileDlg, 
					 LPCTSTR lpszDefExt,
					 LPTSTR lpszFileName,
					 DWORD dwFlags,
					 LPCTSTR lpszFilter,
					 HWND hParentWnd,
					 LPTSTR lpszFileTitle,
					 LPCTSTR lpszTitle,
					 LPTSTR lpszInitialDir);

TCHAR _modelLoadPath[MAX_PATH];
TCHAR _modelSavePath[MAX_PATH];
TCHAR _animationPath[MAX_PATH];
TCHAR _texturePath[MAX_PATH];

struct psGlobalType
{
    HWND        window;
    HINSTANCE   instance;
    RwBool      fullScreen;
    RwV2d       lastMousePos;
};

extern psGlobalType PsGlobal;

#define INITHFOV (90.0f)

#define DegToRad(x) ( (x) * ( rwPI / 180.0f ) )

#define NEARMINCLIP (0.1f)
#define MINNEARTOFARCLIP (1.0f)
#define FARMAXCLIP (9999.0f)

static Scene* _pThis = NULL;

Scene::Scene() : m_pClump(NULL), m_pAnim(NULL), m_pBackTex(NULL)
{
	assert( !_pThis );
	_pThis = this;

	memset(m_acBackTexPath, 0, sizeof(m_acBackTexPath));

	RwBBox bb;
	bb.inf.x = bb.inf.y = bb.inf.z = -10000.0f;
	bb.sup.x = bb.sup.y = bb.sup.z = 10000.0f;

	m_pWorld = RpWorldCreate(&bb);
	if( !m_pWorld )
	{
		RsErrorMessage( RWSTRING( "Cannot create world." ) );
		assert(0);
	}

	m_pCamera = CameraCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, TRUE);
	if( !m_pCamera )
	{
		RsErrorMessage( RWSTRING( "Cannot create camera." ) );
		assert(0);
	}

	RpWorldAddCamera(m_pWorld, m_pCamera);

	m_fNearClip				= NEARMINCLIP;
	m_fFarClip				= FARMAXCLIP;
	m_fFOV					= INITHFOV;
	m_fCurrentViewWindow	= 0.3f;
	
	m_pLightAmbient = RpLightCreate( rpLIGHTAMBIENT );
	if( !m_pLightAmbient )
	{
		RsErrorMessage( RWSTRING( "Cannot create a ambient light." ) );
		assert(0);
	}

	RwRGBAReal color = {1.0f, 1.0f, 1.0f, 1.0f};
	RpLightSetColor( m_pLightAmbient, &color );
	RpWorldAddLight( m_pWorld, m_pLightAmbient );

	// create a main directional light.
	m_pMainLight = RpLightCreate(rpLIGHTDIRECTIONAL);
	if( !m_pMainLight )
	{
		RsErrorMessage( RWSTRING( "Cannot create a main light." ) );
		assert(0);
	}

	RwFrame* f = RwFrameCreate();
	RpLightSetFrame( m_pMainLight, f );

	RwV3d v = { 0.0f , 10.0f , 0.0f };
	RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

	RwFrameTranslate( f , &v , rwCOMBINEREPLACE );
	RwFrameRotate( f , &xaxis , 135.0f , rwCOMBINEREPLACE );

	RpLightSetColor( m_pMainLight, &color );
	RpWorldAddLight( m_pWorld, m_pMainLight );

    RwImageSetPath( ".\\Texture\\Effect\\;.\\Texture\\UI\\;.\\Texture\\Character\\;.\\Texture\\Object\\;.\\Texture\\World\\;.\\Texture\\UI\\Base\\;.\\Texture\\SKILL\\;.\\Texture\\Item\\;.\\Texture\\Etc\\;.\\Texture\\Minimap\\;.\\Texture\\AmbOccl\\;.\\Texture\\NotPacked\\" );

	RpMaterialD3DFxSetSearchPath(".\\Effect\\");		// set fx paths.
	RwTexDictionarySetCurrent( NULL );		// disable texture caching for the dynamic update.
	RwTextureSetAutoMipmapping(TRUE);		// enable auto mipmapping.
	RwTextureSetMipmapping(TRUE);

	// read the scene config.
	ifstream config("scene.cfg");
	if(config.is_open())
	{
		RwRGBAReal colAmbient, colMain;
		config >> colAmbient.red >> colAmbient.green >> colAmbient.blue >> colMain.red >> colMain.green >> colMain.blue;
		colAmbient.alpha = colMain.alpha = 1.0f;
		RpLightSetColor( m_pLightAmbient, &colAmbient );
		RpLightSetColor( m_pMainLight, &colMain );

		char buf[MAX_PATH];
		buf[0] = '\0';
		config >> buf;
		mbstowcs(_modelLoadPath, buf, MAX_PATH-1);
		// model save
		buf[0] = '\0';
		config >> buf;
		mbstowcs(_modelSavePath, buf, MAX_PATH-1);
		// animation
		buf[0] = '\0';
		config >> buf;
		mbstowcs(_animationPath, buf, MAX_PATH-1);
		// texture
		buf[0] = '\0';
		config >> buf;
		mbstowcs(_texturePath, buf, MAX_PATH-1);

		int x, y, w, h;
		config >> x >> y >> w >> h;
		MoveWindow(PsGlobal.window, x, y, w, h, TRUE);

		buf[0] = '\0';
		config >> buf;
		if(strlen(buf) > 0)
			loadBackTex(buf);

		config.close();
	}
	else
	{
		MoveWindow(PsGlobal.window, 0, 0, 640, 480, TRUE);
	}
}
	
Scene::~Scene()
{
	//@{ Jaewon 20040924
	// save the scene config.
	ofstream config("scene.cfg", ios::out|ios::trunc);
	if(config.is_open())
	{
		const RwRGBAReal *pColAmbient = RpLightGetColor(m_pLightAmbient);
		const RwRGBAReal *pColMain = RpLightGetColor(m_pMainLight);

		config << pColAmbient->red <<" "<<  pColAmbient->green <<" "<< pColAmbient->blue << endl
			   << pColMain->red <<" "<< pColMain->green <<" "<< pColMain->blue << endl;

		//@{ Jaewon 20041103
		char buf[MAX_PATH];
		// save file paths
		// model load
		wcstombs(buf, _modelLoadPath, MAX_PATH-1);
		config << buf << endl;
		// model save
		wcstombs(buf, _modelSavePath, MAX_PATH-1);
		config << buf << endl;
		// animation
		wcstombs(buf, _animationPath, MAX_PATH-1);
		config << buf << endl;
		// texture
		wcstombs(buf, _texturePath, MAX_PATH-1);
		config << buf << endl;
		//@} Jaewon

		RECT rect;
		GetWindowRect(PsGlobal.window, &rect);
		config << rect.left << " " << rect.top << " " << rect.right-rect.left << " " << rect.bottom-rect.top << endl;
		config << m_acBackTexPath << endl;

		config.close();
	}
	else
	{
		RsErrorMessage(RWSTRING("Cannot write a config file."));
		assert(0);
	}
	config.close();
	//@} Jaewon

	if( m_pBackTex )
	{
		RwTextureDestroy( m_pBackTex );
		m_pBackTex = NULL;
	}

	if( m_pAnim )
	{
		RtAnimAnimationDestroy(m_pAnim);
		m_pAnim = NULL;
	}

    if( m_pClump )
    {
        RpWorldRemoveClump( m_pWorld, m_pClump );

        RpClumpDestroy( m_pClump );
		m_pClump = NULL;
    }

	if( m_pMainLight )
	{
		RpWorldRemoveLight( m_pWorld , m_pMainLight );

		RwFrame* frame = RpLightGetFrame( m_pMainLight );
		RwFrameDestroy( frame );
		RpLightSetFrame( m_pMainLight, NULL );
		RpLightDestroy( m_pMainLight );
		m_pMainLight = NULL;
	}

	if( m_pLightAmbient )
	{
		RpWorldRemoveLight( m_pWorld, m_pLightAmbient );

		RpLightDestroy( m_pLightAmbient );
		m_pLightAmbient = NULL;
	}

	if( m_pCamera )
	{
		RpWorldRemoveCamera( m_pWorld, m_pCamera );

		CameraDestroy( m_pCamera );
		m_pCamera = NULL;
	}

	if( m_pWorld )
	{
		RpWorldDestroy( m_pWorld );
		m_pWorld = NULL;
	}

	_pThis = NULL;
}

void Scene::updateCamera()
{
	static RwReal fPreviousFOV = -1;

	/*
	 * Update field of view if necessary
	 */
	if(m_fFOV != fPreviousFOV)
	{
		m_fCurrentViewWindow = (RwReal)RwTan(DegToRad(m_fFOV * 0.5f));
		RsEventHandler(rsCAMERASIZE, NULL);
		fPreviousFOV = m_fFOV;
	}

	/*
	 * Update near clipping plane if necessary
	 */
	if(m_fNearClip != RwCameraGetNearClipPlane(m_pCamera))
	{
		RwCameraSetNearClipPlane(m_pCamera, m_fNearClip);

		if(m_fFarClip < m_fNearClip + MINNEARTOFARCLIP )
		{
			m_fFarClip = m_fNearClip + MINNEARTOFARCLIP;
		}
	}

	/*
	 * Update far clipping plane if necessary
	 */
	if(m_fFarClip != RwCameraGetFarClipPlane(m_pCamera))
	{
		RwCameraSetFarClipPlane(m_pCamera, m_fFarClip);

		if(m_fNearClip > m_fFarClip - MINNEARTOFARCLIP)
		{
			m_fNearClip = m_fFarClip - MINNEARTOFARCLIP;
		}
	}
}

void Scene::updateAnimation(float fElapsedTime)
{
	if( !m_pAnim )			return;
	if( !m_pClump )			return;

	RpHAnimHierarchy* pHierarchy = getHierarchy( m_pClump );
	if( !pHierarchy )		return;
		
	RpHAnimHierarchyAddAnimTime( pHierarchy, fElapsedTime );
    RpHAnimUpdateHierarchyMatrices( pHierarchy );
}

void Scene::resetCamera()
{
	/*
	 * Set the camera's view window so that it gives
	 * a *horizontal* field of view of INITHFOV degrees...
	 */
	m_fFOV = INITHFOV;
	m_fCurrentViewWindow = (RwReal)RwTan(DegToRad(m_fFOV * 0.5f));
	RsEventHandler(rsCAMERASIZE, NULL);

	/*
	 * Set far and near clipping planes
	 */
	m_fFarClip = FARMAXCLIP;

	m_fNearClip = NEARMINCLIP;
	RwCameraSetNearClipPlane( m_pCamera, m_fNearClip );
	RwCameraSetFarClipPlane( m_pCamera, m_fFarClip );

	/*
	 * Initialise the camera position
	 */
	RwV3d Yaxis = { 0.0f, 1.0f, 0.0f };

	RwFrame* cameraFrame = RwCameraGetFrame( m_pCamera );
	RwFrameSetIdentity( cameraFrame );
	RwFrameRotate( cameraFrame, &Yaxis, 180.0f, rwCOMBINEREPLACE);
	RwFrameTranslate( cameraFrame, &m_clumpBoundingSphere.center, rwCOMBINEPOSTCONCAT );

	/*
	 * Pull the camera backwards...
	 */
	RwReal cameraDistance = 2.0f * m_clumpBoundingSphere.radius / m_fCurrentViewWindow;
	if( cameraDistance )
		m_fFarClip = 5.0f * cameraDistance;

	RwMatrix* matrix = RwFrameGetLTM( cameraFrame );
	RwV3d temp;
	RwV3dScale( &temp, RwMatrixGetAt(matrix), -cameraDistance );
	RwFrameTranslate( cameraFrame, &temp, rwCOMBINEPOSTCONCAT );
}

static RpAtomic *addAtomicBSphereCenterCB(RpAtomic *pAtomic, void *pData)
{
	RpGeometry* geometry = RpAtomicGetGeometry(pAtomic);
	if( !geometry )	return pAtomic;

	RwV3d atomicCentre = { 0.0f, 0.0f, 0.0f };
	RwInt32 numMorphTargets = RpGeometryGetNumMorphTargets( geometry );
	for( RwInt32 i = 0; i < numMorphTargets; ++i )
	{
		RpMorphTarget* morphTarget = RpGeometryGetMorphTarget( geometry, i );
		RwV3d& center = RpMorphTargetGetBoundingSphere(morphTarget)->center;
		RwV3dAdd(&atomicCentre, &atomicCentre, &center);
	}

	RwV3dScale( &atomicCentre, &atomicCentre, 1.0f / numMorphTargets );

	//Tranform the average centre of the atomic to world space
	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(pAtomic));
	RwV3dTransformPoints(&atomicCentre, &atomicCentre, 1, LTM);

	//Add the average centre of the atomic up in order to calculate the centre of the clump
	RwSphere* clumpSphere = (RwSphere *)pData;
	RwV3dAdd(&clumpSphere->center, &clumpSphere->center, &atomicCentre);

	return pAtomic;
}

static RpAtomic *compareAtomicBSphereRadiusCB(RpAtomic *pAtomic, void *pData)
{
	RpGeometry* geometry = RpAtomicGetGeometry(pAtomic);
	if( !geometry )		return pAtomic;

	RwSphere* sphere = (RwSphere *)pData;
	RwMatrix* LTM = RwFrameGetLTM( RpAtomicGetFrame( pAtomic ) );

	RwInt32 numMorphTargets = RpGeometryGetNumMorphTargets(geometry);
	for(RwInt32 i = 0; i < numMorphTargets; ++i)
	{
		RpMorphTarget* morphTarget = RpGeometryGetMorphTarget(geometry, i);
		RwSphere morphTargetSphere = *RpMorphTargetGetBoundingSphere(morphTarget);

		RwV3dTransformPoints(&morphTargetSphere.center, &morphTargetSphere.center, 1, LTM);

		RwV3d tempVec;
		RwV3dSub(&tempVec, &morphTargetSphere.center, &sphere->center);
		RwReal dist = RwV3dLength(&tempVec) + morphTargetSphere.radius;
		sphere->radius = max( dist, sphere->radius );
	}

	return pAtomic;
}

void Scene::computeClumpBoundingSphere()
{
	/*
	 * First find the mean of all the atomics' bounding sphere centers.
	 * All morph targets of all atomics are taken into account.
	 * The result is the clump's bounding sphere center...
	 */

	RwV3d	zero = { 0.0f, 0.0f, 0.0f };
	m_clumpBoundingSphere.center = zero;

	/*
	 * average over morph targets and atomics
	 */
	RpClumpForAllAtomics( m_pClump, addAtomicBSphereCenterCB, &m_clumpBoundingSphere );
	RwV3dScale( &m_clumpBoundingSphere.center, &m_clumpBoundingSphere.center, 1.0f / RpClumpGetNumAtomics( m_pClump) );

	/*
	 * Now, given the clump's bounding sphere center, determine the radius
	 * by finding the greatest distance from the center that encloses all
	 * the atomics' bounding spheres.  All morph targets, atomics are
	 * taken into account
	 */
	m_clumpBoundingSphere.radius = -RwRealMAXVAL;

	RpClumpForAllAtomics(m_pClump, compareAtomicBSphereRadiusCB, &m_clumpBoundingSphere);
}

static RpAtomic *getFirstAtomicCB(RpAtomic *pAtomic, PVOID pvData)
{
	*(RpAtomic **) pvData = pAtomic;

	return NULL;
}

static RpAtomic *initAtomicCB(RpAtomic *pAtomic, void *data)
{
	int	i, j;
	RpHAnimHierarchy *pHierarchy = (RpHAnimHierarchy *)data;
    RpGeometry* geom = RpAtomicGetGeometry(pAtomic);

	RwUInt32 nMatsBeforeSplit = RpGeometryGetNumMaterials(geom);

	RpSkin *pSkin = RpSkinGeometryGetSkin( geom );
	if(pSkin)
	{
		//@{ Jaewon 20050407
		// m_notWantAnim condition added.
		if( !_pThis->m_notWantAnim == false && !_pThis->getAnimation() )
		//@} Jaewon
		{
			TCHAR pathname[MAX_PATH];
			_tcscpy(pathname, L"*.rws");

			TCHAR filetitle[MAX_PATH];
			if(doFileDlg(TRUE, L"rws", pathname, NULL, 
						 L"RenderWare Stream Files (*.rws)\0.rws\0All Files (*.*)\0*.*\0\0",
						 psWindowGetHandle(),
						 filetitle,
						 L"Open a matching animation",
						 _animationPath))
			{
				char openFilePath[MAX_PATH];
				wcstombs(openFilePath, pathname, wcslen(pathname)+1);
				_pThis->loadAnimRWS(openFilePath);
			}

			if( !_pThis->getAnimation() )
				_pThis->m_notWantAnim = true;
		}

		// Only set the hierarchy if an animation is given.
		if(_pThis->m_notWantAnim == false)
			RpSkinAtomicSetHAnimHierarchy(pAtomic, pHierarchy);

		// do a skin split.
		RtSkinSplitAtomicSplitGeometry( pAtomic, (((D3DCAPS9 *)RwD3D9GetCaps())->MaxVertexShaderConst - 30) / 3 );
	}

	// build collision data.
    if( !RpCollisionGeometryQueryData( geom ) )
    {
        RsWarningMessage( "Building collision data for clump, this may take some time..." );
        RpCollisionGeometryBuildData( geom, NULL );
        RsWarningMessage( "Collision data built." );
    }

	//@{ Jaewon 20041102
	// initialize the atomic stats.
	assert(_pThis->m_stats.m_nAtomics < 100);
	Scene::AtomicStats &atomicStats = _pThis->m_stats.m_atomic[_pThis->m_stats.m_nAtomics];
	atomicStats.m_nTris = RpGeometryGetNumTriangles(geom);
	atomicStats.m_nMats = RpGeometryGetNumMaterials(geom);
	//@{ Jaewon 20041103
	atomicStats.m_nMatsBeforeSplit = nMatsBeforeSplit;
	//@} Jaewon
	assert(atomicStats.m_nMats <= 100);
	atomicStats.m_nBones = pSkin?RpSkinGetNumBones(pSkin):0;
	// update total polygon counts.
	_pThis->m_stats.m_nTris += atomicStats.m_nTris;
	// materials
	for(i=0; i<atomicStats.m_nMats; ++i)
	{
		RwTexture *pTexture = RpMaterialGetTexture(RpGeometryGetMaterial(geom, i));
		if(pTexture)
		{
			RwChar *name = RwTextureGetName(pTexture);
			// check if there is an already-loaded one.
			for(j=0; j<_pThis->m_stats.m_nTexs; ++j)
			{
				if(strcmp(name, _pThis->m_stats.m_texture[j].m_name) == 0)
				{
					atomicStats.m_material[i].m_pTexStats = &(_pThis->m_stats.m_texture[j]);
					break;
				}
			}
			if(j == _pThis->m_stats.m_nTexs)
			// if not found,
			{
				// add a new texture.
				assert(_pThis->m_stats.m_nTexs < 30);
				Scene::TextureStats &textureStats = _pThis->m_stats.m_texture[_pThis->m_stats.m_nTexs];
				strcpy(textureStats.m_name, name);
				RwRaster *pRaster = RwTextureGetRaster(pTexture);
				textureStats.m_texWidth = RwRasterGetWidth(pRaster);
				textureStats.m_texHeight = RwRasterGetHeight(pRaster);
				textureStats.m_texBPP = RwRasterGetDepth(pRaster);
				++(_pThis->m_stats.m_nTexs);

				atomicStats.m_material[i].m_pTexStats = &textureStats;
			}
		}
		else
			atomicStats.m_material[i].m_pTexStats = NULL;
	}
	++(_pThis->m_stats.m_nAtomics);
	//@} Jaewon
	
    return pAtomic;
}

RwFrame *getChildFrameHierarchyCB(RwFrame *pFrame, PVOID pvData)
{
	RpHAnimHierarchy* hierarchy = RpHAnimFrameGetHierarchy( pFrame );
	if( !hierarchy )	return NULL;

	RpHAnimHierarchy** ppHierarchy = (RpHAnimHierarchy**)pvData;
	*ppHierarchy = hierarchy;
	RwFrameForAllChildren( pFrame, getChildFrameHierarchyCB, pvData );

	return pFrame;
}

RpHAnimHierarchy *Scene::getHierarchy(RpClump *pClump) const
{
	RpHAnimHierarchy* pHierarchy = RpHAnimGetHierarchy( RpClumpGetFrame( pClump ) );
	if( pHierarchy )	return pHierarchy;

	RwFrameForAllChildren( RpClumpGetFrame( pClump ), getChildFrameHierarchyCB, (void *)&pHierarchy );
	return pHierarchy;
}

void Scene::initClump(RpClump *pClump)
{
	assert(pClump);

	if( m_pClump )
	{
		RpWorldRemoveClump( m_pWorld, m_pClump );
		RpClumpDestroy( m_pClump );
		m_pClump = NULL;
	}

	m_pClump = pClump;

	/*
	 * Move the clump to the world's origin...
	 */
	RwV3d Zero = {0.0f, 0.0f, 0.0f};
	RwFrame* clumpFrame = RpClumpGetFrame(m_pClump);
	m_matBackup = *RwFrameGetLTM( clumpFrame );

	RwFrameTranslate( clumpFrame, &Zero, rwCOMBINEREPLACE );

	RpWorldAddClump( m_pWorld, m_pClump );

	/*
	 * Determine the clump's overall bounding sphere
	 * (for all morph targets and for all animations)...
	 */
	computeClumpBoundingSphere();

	RpHAnimHierarchy *pHierarchy = getHierarchy(pClump);

	//@{ Jaewon 20041102
	// initialize the model stats.
	m_stats.m_nAtomics = 0;
	m_stats.m_nTris = 0;
	m_stats.m_nTexs = 0;
	//@} Jaewon
	
	//@{ Jaewon 20050407
	m_notWantAnim = false;
	//@} Jaewon

	// set a skin hierarchy(if any) and create collision data for picking.
	RpClumpForAllAtomics( m_pClump, initAtomicCB, (void *)pHierarchy );

	if( pHierarchy && m_pAnim )
	{
		int flag = (int)RpHAnimHierarchyGetFlags( pHierarchy );
		flag |= (int)rpHANIMHIERARCHYUPDATELTMS;
		flag |= (int)rpHANIMHIERARCHYUPDATEMODELLINGMATRICES;
		
		RpHAnimHierarchySetFlags( pHierarchy, (RpHAnimHierarchyFlag)flag );
		RpHAnimHierarchySetCurrentAnim( pHierarchy, m_pAnim );
		RpHAnimHierarchyAttach( pHierarchy );        
		RpHAnimUpdateHierarchyMatrices( pHierarchy );
	}
}

bool Scene::loadRWS(const char *pathname)
{
	assert( rwstrstr( pathname, RWSTRING (".rws") ) || rwstrstr( pathname, RWSTRING (".RWS") ) );
		   
	RwChar* path = RsPathnameCreate( pathname );
	RwStream* rwsStream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, path );
	if( !rwsStream )
	{
		RsPathnameDestroy(path);
		return false;
	}

	RwStreamClose( rwsStream, NULL );
	rwsStream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, path );

	bool bSuccess = false;
    while( RwStreamFindChunk( rwsStream, rwID_CLUMP,  (RwUInt32 *)NULL, (RwUInt32 *)NULL ) )
	{
		RpClump* pClump = RpClumpStreamRead( rwsStream );     
		if( !pClump )	continue;

		// check its validity.
		RpAtomic *pAtomic = NULL;
		RpClumpForAllAtomics( pClump, getFirstAtomicCB, &pAtomic );
		initClump( pClump );

		if( pAtomic )
		// if it is valid,
		{	
			bSuccess = true;
			break;
		}
	}
	
	RwStreamClose( rwsStream, NULL );
	RsPathnameDestroy( path );

	if( bSuccess )
	{
		if( m_pAnim )
		{
			RtAnimAnimationDestroy( m_pAnim );
			m_pAnim = NULL;
		}

		resetCamera();
		resetClumpControl();

		sprintf( m_acPath, "%s", pathname );
	}

	return bSuccess;
}

bool Scene::loadAnimRWS(const char *pathname)
{
	bool bSuccess = false;

	RwChar* path = RsPathnameCreate(pathname);
	RwStream* rwsStream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, path );
	RsPathnameDestroy(path);

	if( !rwsStream )		return false;

	RwStreamClose( rwsStream, NULL );
	rwsStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);

    while( RwStreamFindChunk(rwsStream, rwID_HANIMANIMATION, (RwUInt32 *)NULL, (RwUInt32 *)NULL) )
	{
		m_pAnim = RtAnimAnimationStreamRead(rwsStream);     
		bSuccess = true;
		
		assert( rpHANIMSTDKEYFRAMETYPEID == RtAnimAnimationGetTypeID( m_pAnim ) );
		break;
	}
	
	RwStreamClose(rwsStream, NULL);
	

    return bSuccess;
}

bool Scene::saveRWS(const char *pathname) const
{
	bool bSuccess = true;

	RwFrame* clumpFrame = RpClumpGetFrame(m_pClump);
	RwMatrix mat = *RwFrameGetLTM( clumpFrame );
	RwFrameTransform( clumpFrame , &m_matBackup , rwCOMBINEREPLACE );

	RwChar* path = RsPathnameCreate( pathname );
	RwStream* stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, path );
	RsPathnameDestroy( path );

	if(stream)
	{
		if(!RpClumpStreamWrite(m_pClump, stream))
		{   
			RsErrorMessage(RWSTRING("Cannot write RWS file."));
			bSuccess = false;
		}

		RwStreamClose(stream, NULL);
	}
	else
	{
		RsErrorMessage(RWSTRING("Cannot open stream to write RWS file."));
		bSuccess =  false;
	}

	RwFrameTransform( clumpFrame , &mat , rwCOMBINEREPLACE );

	return bSuccess;
}

const char *Scene::getStatsString()
{
	static char buf[10240];

	ostrstream stream( buf, 10240 );

	stream << "Total # of Tris : " << m_stats.m_nTris << " / ";
	stream << "# of Atomics : " << m_stats.m_nAtomics << " / ";
	stream << "# of Textures : " << m_stats.m_nTexs << endl;

	// textures
	for(int	i=0; i<m_stats.m_nTexs; ++i)
	{
		Scene::TextureStats &textureStats = m_stats.m_texture[i];
		stream << "  " << textureStats.m_name << " " 
			   << textureStats.m_texWidth << "x" << textureStats.m_texHeight << " " 
			   << textureStats.m_texBPP << endl;
	}

	// atomics
	for(int	i=0; i<m_stats.m_nAtomics; ++i)
	{
		Scene::AtomicStats &atomicsStats = m_stats.m_atomic[i];
		stream << "  ---------------------" << endl;
		stream << "  # of Tris : " << atomicsStats.m_nTris << " / ";
		stream << "  # of Bones : " << atomicsStats.m_nBones << " / ";
		stream << "  # of Materials : " << atomicsStats.m_nMatsBeforeSplit << "->" << atomicsStats.m_nMats << endl;

		// materials
		for(int j=0; j<atomicsStats.m_nMats; ++j)
		{
			Scene::MaterialStats &materialStats = atomicsStats.m_material[j];
			stream << "    Material : ";
			stream << materialStats.m_pTexStats ? materialStats.m_pTexStats->m_name : "NULL";
			stream << endl;
		}
	}

	stream << ends;

	return buf;
}

static bool getFullTexturePath(const char *texName, char* fullPath)
{
	const char *ext = RwImageFindFileType( texName );
	if( !ext )		return false;

	const char* pathElement = RwImageGetPath();
	while( pathElement && pathElement[0] != '\0')
	{
		int pathElementLength;

		const char *nextPathElement = rwstrchr(pathElement, ';');
		if(nextPathElement)
		{
			/* Length of pathstring (in bytes) */
			pathElementLength = (RwUInt8 *)nextPathElement - (RwUInt8 *)pathElement;
			++nextPathElement; /* Skip the separator */
		}
		else
		{
			pathElementLength = rwstrlen(pathElement);
		}

		/* Just need to assemble a path with the current search path element */
		memcpy(fullPath, pathElement, pathElementLength);
		rwstrcpy((RwChar *)(((RwUInt8 *) fullPath) + pathElementLength), texName);

		RwChar* extPos = fullPath + rwstrlen(fullPath);
		rwstrcpy( extPos, ext );
		if( RwFexist( fullPath ) )			/* Got it, cache the pointer and early out */
			return true;

		pathElement = nextPathElement;
	}

	// it should have been returned already.
	assert(0);

	return false;
}

static RpAtomic *geometryMergeTexturesCB(RpAtomic *pAtomic, void *pData)
{
	int	i;
	RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);

	if(pGeometry && RpGeometryGetNumMaterials(pGeometry)>1)
	{
	int argc = 1;
	char *argv[100];
	for(i=0; i<100; ++i)
		argv[i] = new char[MAX_PATH];

	// make up a command line options.
	strncpy(argv[0], "MatD3DFxAtlasTool", MAX_PATH-1);
	strncpy(argv[1], "-o", MAX_PATH-1);
	// get the name of clump.
	char acTmp[MAX_PATH];
	int len = strlen(_pThis->m_acPath);
	while(--len > 0)
	{
		if(_pThis->m_acPath[len] == '\\')
			break;
	}
	strcpy(acTmp, _pThis->m_acPath+len+1);
	len = strlen(acTmp);
	while(--len > 0)
	{
		if(acTmp[len] == '.')
		{
			acTmp[len] = '\0';
			break;
		}
	}
	sprintf(argv[2], "%s%d", acTmp, *(int*)pData);
	strncpy(argv[3], "-width", MAX_PATH-1);
	sprintf(argv[4], "%d", 512);
	strncpy(argv[5], "-height", MAX_PATH-1);
	sprintf(argv[6], "%d", 512);
	argc = 7;

	// get texture paths
	const char *imagePaths = RwImageGetPath();
	for(i=0; i<RpGeometryGetNumMaterials(pGeometry); ++i)
	{
		RpMaterial *pMaterial = RpGeometryGetMaterial(pGeometry, i);
		RwTexture *pTexture = RpMaterialGetTexture(pMaterial);
		if(pTexture == NULL)
		{
			RsErrorMessage(RWSTRING("A material does not have a texture."));
			continue;
		}
		char acFullPath[MAX_PATH];
		const char *texName = RwTextureGetName(pTexture);
		if(getFullTexturePath(texName, acFullPath) == false)
		{
			assert(0);
			RsErrorMessage(RWSTRING("A texture cannot be found."));
			continue;
		}
		strncpy(argv[argc], acFullPath, MAX_PATH-1);
		++argc;
		if(argc >= 100)
		{
			assert(0);
			RsErrorMessage(RWSTRING("Too many(>100) materials."));
			break;
		}
	}

	// set the output path to the path of the first input texture.
	strcpy(acTmp, argv[7]);
	len = strlen(acTmp);
	while(--len > 0)
	{
		if(acTmp[len] == '\\')
		{
			acTmp[len+1] = '\0';
			break;
		}
	}
	strcat(acTmp, argv[2]);
	strcpy(argv[2], acTmp);

	CmdLineOptionCollection options(argc, (char**)argv);
    assert(options.IsValid());

	// merge to atlases.
    // Create an array of texture objects and load the filenames/textures into that array.
	int const kNumTextures = options.GetNumFilenames();
	char const *pFilename = NULL;

	Texture2D *pSourceTex = new Texture2D[kNumTextures];

	for(i = 0; i < kNumTextures; ++i)
	{
		options.GetFilename(i, &pFilename);
		pSourceTex[i].Init(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()), pFilename);
		if(FAILED(pSourceTex[i].LoadTexture(options)))
		{
			assert(0);
			RsErrorMessage(RWSTRING("Texture load failure!"));
			delete [] pSourceTex;
			return pAtomic;
		}
	}

	// Bin these textures into format groups (maps of vectors)
	TNewFormatMap formatMap;
	for(i = 0; i < kNumTextures; ++i)
		formatMap[pSourceTex[i].GetFormat()].push_back(&(pSourceTex[i]));

	// We do not do format conversions, so all these different formats 
	// require their own atlases.  Each format may have multiple atlases, e.g., 
	// there is not enough space in a single atlas for all textures of the 
	// same format.  An atlas container contains all these concepts.
	AtlasContainer atlas(options, formatMap.size());

	// For each format-vector of textures, Sort textures by size (width*height, then height
	TNewFormatMap::iterator fmSort;
	for(fmSort = formatMap.begin(); fmSort != formatMap.end(); ++fmSort)
		std::sort((*fmSort).second.begin(), (*fmSort).second.end(), Texture2DGreater());

	// For each format-vector of textures, insert them into their respective 
	// atlas vector: 
	TNewFormatMap::const_iterator fmIter;
	for(i = 0, fmIter = formatMap.begin(); fmIter != formatMap.end(); ++fmIter, ++i)
		atlas.Insert(i, fmIter->second);

	// Done inserting data: shrink all atlases to minimum size
	atlas.Shrink();

	// save merged textures.
    // Write all atlases to disk w/ the filenames they have stored
    atlas.WriteToDisk();
	
	// update the geometry according to a tai file.
	// create atlas materials
	int nMaterials = 0;
	RpMaterial *pNewMaterial[10];
	TAtlasVector::const_iterator atlasObj;
	for(i = 0; i < atlas.mNumFormats; ++i)
	{
		for(atlasObj = atlas.mpAtlasVectorArray[i].begin(); atlasObj != atlas.mpAtlasVectorArray[i].end(); ++atlasObj)
		{
			strncpy(acTmp, (*atlasObj)->GetFilename(), MAX_PATH-1);
			// discard the file extension.
			len = strlen(acTmp);
			while(--len > 0)
			{
				if(acTmp[len] == '.')
				{
					acTmp[len] = '\0';
					break;
				}
			}
			// discard the file path.
			len = strlen(acTmp);
			while(--len > 0)
			{
				if(acTmp[len] == '\\')
					break;
			}
			RwTexture *pTexture = RwTextureRead(acTmp+len+1, NULL);
			pNewMaterial[nMaterials] = RpMaterialCreate();
			RpMaterialSetTexture(pNewMaterial[nMaterials], pTexture);
			// decrement the reference count.
			RwTextureDestroy(pTexture);
			++nMaterials;
		}
	}

	*(int*)pData += nMaterials;

	// atlas materials replace existing materials.
	for(i=0; i<RpGeometryGetNumMaterials(pGeometry); ++i)
	{
		RpMaterial *pMaterial = RpGeometryGetMaterial(pGeometry, i);
		RwTexture *pTexture = RpMaterialGetTexture(pMaterial);
		if(pTexture == NULL)
			continue;

		RpMaterial *pAtlasMat = NULL;
		float uOffset, vOffset, uWidth, vHeight;
		// search the matching atlas.
		TNewFormatMap::const_iterator fmIter;
		TTexture2DPtrVector::const_iterator texIter;
		for(fmIter = formatMap.begin(); fmIter != formatMap.end(); ++fmIter)
		{
			for(texIter = (fmIter->second).begin(); texIter != (fmIter->second).end(); ++texIter) 
			{
				sprintf(acTmp, "\\%s.", RwTextureGetName(pTexture));
				if(strstr((*texIter)->GetFilename(), acTmp))
				{
					const char *atlasName = (*texIter)->GetTAI(options, uOffset, vOffset, uWidth, vHeight);
					if(atlasName)
					{
						for(int j=0; j<nMaterials; ++j)
						{
							sprintf(acTmp, "\\%s.", RwTextureGetName(RpMaterialGetTexture(pNewMaterial[j])));
							if(strstr(atlasName, acTmp))
							{
								pAtlasMat = pNewMaterial[j];
								break;
							}
						}
					}
					goto BREAK;
				}
			}
		}
		
BREAK:
		if(pAtlasMat)
		{
		RpGeometryLock(pGeometry, rpGEOMETRYLOCKPOLYGONS|rpGEOMETRYLOCKTEXCOORDS);
		RpTriangle *pTris;
		pTris = RpGeometryGetTriangles(pGeometry);
		for(int j=0; j<RpGeometryGetNumTriangles(pGeometry); ++j)
		{
			if(RpGeometryTriangleGetMaterial(pGeometry, &pTris[j]) == pMaterial)
			{
				// change the material.
				RpGeometry *result = RpGeometryTriangleSetMaterial(pGeometry, &pTris[j], pAtlasMat);
				assert(result);

				// update texture coordinates.
				if(RpGeometryGetNumTexCoordSets(pGeometry) > 1)
				{
					assert(0);
					RsErrorMessage(RWSTRING("More than one texture coordinates set found."));
				}
				RwTexCoords *pTexCoords = RpGeometryGetVertexTexCoords(pGeometry, rwTEXTURECOORDINATEINDEX0);
				for(int k=0; k<3; ++k)
				{
					pTexCoords[pTris[j].vertIndex[k]].u = uOffset + uWidth * (pTexCoords[pTris[j].vertIndex[k]].u);
					pTexCoords[pTris[j].vertIndex[k]].v = vOffset + vHeight * (pTexCoords[pTris[j].vertIndex[k]].v);
				}
			}
		}
		RpGeometryUnlock(pGeometry);
		}
	}

	// free all memory: clear all vectors of everything
	delete [] pSourceTex;

	for(i=0; i<100; ++i)
		delete [] argv[i];
	}

	return pAtomic;
}

unsigned int Scene::mergeTextures(unsigned int width, unsigned int height)
{
	if( !m_pClump )		return 0;
		
	int nAtlases = 0;
	RpClumpForAllAtomics( m_pClump, geometryMergeTexturesCB, &nAtlases );

	return nAtlases;
}
//@} Jaewon

//@{ Jaewon 20041214
// for testing ambient occlusion map modules
static RpMaterial *setupMaterialsForAmbOcclMapCB(RpMaterial *pMaterial, void *pData)
{
	RwUInt32 flag = RtAmbOcclMapMaterialGetFlags( pMaterial );
	RtAmbOcclMapMaterialSetFlags( pMaterial, flag | rtAMBOCCLMAPMATERIALAMBOCCLMAP );
	return pMaterial;
}
static RpAtomic *setupAtomicsForAmbOcclMapCB(RpAtomic *pAtomic, void *pData)
{
	RpGeometry *pGeometry = RpAtomicGetGeometry(pAtomic);

	RpGeometryForAllMaterials(pGeometry, setupMaterialsForAmbOcclMapCB, NULL);

	RwUInt32 flag =  RtAmbOcclMapAtomicGetFlags(pAtomic);
	RtAmbOcclMapAtomicSetFlags(pAtomic, ( flag | rtAMBOCCLMAPOBJECTAMBOCCLMAP) & ~rtAMBOCCLMAPOBJECTVERTEXLIGHT);

	//@{ Jaewon 20041217
	// adjust its ambient occlusion map size according to the size of the atomic.
	const RwSphere *pBoundingSphere = RpMorphTargetGetBoundingSphere(RpGeometryGetMorphTarget(pGeometry,0));
	RwUInt32 size = 128;
	if(2 * pBoundingSphere->radius * 0.02f > 192)
		size *= 2;
	else if(2 * pBoundingSphere->radius * 0.02f < 64)
		size /= 2;

	//@{ Jaewon 20050112
	// adjust the size by a given scale.
	Scene::AmbOcclMapSizeScale scale = *(Scene::AmbOcclMapSizeScale*)pData;
	switch(scale)
	{
	case Scene::AOMSS_QUATER:
		size /= 4;
		break;
	case Scene::AOMSS_HALF:
		size /= 2;
		break;
	case Scene::AOMSS_ONE:
		break;
	case Scene::AOMSS_DOUBLE:
		size *= 2;
		break;
	case Scene::AOMSS_QUADRUPLE:
		size *= 4;
		break;
	default:
		assert(0);
		break;
	}
	//@} Jaewon

	RtAmbOcclMapAtomicSetAmbOcclMapSize(pAtomic, size);
	//@} Jaewon

	return pAtomic;
}

bool Scene::createAmbOcclMaps(bool uvOnly, AmbOcclMapSizeScale scale)
{
	if( !m_pClump )		return false;

	RtAmbOcclMapLightingSessionInitialize( &m_lightingSession, m_pWorld );
	m_lightingSession.camera = NULL;

	RtAmbOcclMapAmbOcclMapSetDefaultSize(128);
	RtAmbOcclMapSetRayCount(64);
	RpClumpForAllAtomics(m_pClump, setupAtomicsForAmbOcclMapCB, &scale);

	if(uvOnly)
		RtAmbOcclMapAmbOcclMapsCreateUVsOnly(&m_lightingSession, 0.02f);
	else
		RtAmbOcclMapAmbOcclMapsCreate(&m_lightingSession, 0.02f, NULL);

	return true;
}

bool Scene::clearAmbOcclMaps()
{
	if( !RpAmbOcclMapWorldAmbOcclMapsQuery(m_lightingSession.world) )
		return false;

	RtAmbOcclMapAmbOcclMapsClear( &m_lightingSession, NULL );

	return true;
}

bool Scene::destroyAmbOcclMaps()
{
	RtAmbOcclMapAmbOcclMapsDestroy( &m_lightingSession );

	return true;
}

bool Scene::computeAmbOcclMaps(RtAmbOcclMapIlluminateProgressCallBack progressCB)
{
	if(m_lightingSession.totalObj > 0 && m_lightingSession.startObj >= m_lightingSession.totalObj)
	// illumination complete
		return true;

	if( !RpAmbOcclMapWorldAmbOcclMapsQuery(m_lightingSession.world) )	return false;

	m_lightingSession.progressCallBack = progressCB;

	if(m_lightingSession.totalObj == 0)
		m_lightingSession.startObj = 0;

	m_lightingSession.numObj = 1;

	int numObjs = RtAmbOcclMapIlluminate(&m_lightingSession, 2);
	if( numObjs == -1 )		return false;
		
	m_lightingSession.startObj += numObjs;
	return true;
}

static RwTexture *saveAmbOcclImageCB(RwTexture *pTexture, void *pData)
{
	const char *pathname = (const char*)pData;

	RwTextureAddRef(pTexture);

	char fullpath[MAX_PATH];
	strncpy(fullpath, pathname, MAX_PATH-1);
	strncat(fullpath, pTexture->name, MAX_PATH-1);
	strncat(fullpath, ".png", MAX_PATH-1);

	RwRaster *pRaster = RwTextureGetRaster(pTexture);
	RwImage *pAmbOcclMap = RwImageCreate(RwRasterGetWidth(pRaster), RwRasterGetHeight(pRaster), 32);
	assert(pAmbOcclMap);

	pAmbOcclMap = RwImageAllocatePixels(pAmbOcclMap);
	assert(pAmbOcclMap);

	pAmbOcclMap = RwImageSetFromRaster(pAmbOcclMap, pRaster);
	assert(pAmbOcclMap);

	RwImageWrite(pAmbOcclMap, fullpath);
	RwImageDestroy(pAmbOcclMap);

	return pTexture;
}

bool Scene::saveAmbOcclMaps(const char *pathname)
{
	if( !pathname )		return false;
	if( !RpAmbOcclMapWorldAmbOcclMapsQuery(m_lightingSession.world) )	return false;

	RwTexDictionary* pTexDic = RtAmbOcclMapTexDictionaryCreate( &m_lightingSession );
	if( !pTexDic )		return false;
		
	RwTexDictionaryForAllTextures( pTexDic, saveAmbOcclImageCB, (void*)pathname );
	RwTexDictionaryDestroy( pTexDic );

	return true;
}

bool Scene::loadBackTex(const char *pathname)
{
	if( m_pBackTex )
	{
		RwTextureDestroy(m_pBackTex);
		m_pBackTex = NULL;
	}

	m_pBackTex = RwTextureRead(pathname, NULL);
	if( !m_pBackTex )	return false;

	strncpy(m_acBackTexPath, pathname, MAX_PATH-1);
	return true;
}
