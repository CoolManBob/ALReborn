#include "AgcmLensFlare.h"

#include <d3dx9.h>

//@{ Jaewon 20050120
#include <ctime>
//@} Jaewon

#include "ApMemoryTracker.h"
#include "AgcmEventNature.h"
#include "AgcmUIConsole.h"
#include "ApModuleStream.h"

//@{ Jaewon 20050120
// d3d device lost & reset
static rwD3D9DeviceRestoreCallBack _oldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack _oldD3D9ReleaseDeviceCB = NULL;
static void newD3D9ReleaseDeviceCB()
{
	((AgcmLensFlare*)g_pEngine->GetModule("AgcmLensFlare"))->onLostDevice();

	if(_oldD3D9ReleaseDeviceCB)
		_oldD3D9ReleaseDeviceCB();
}
static void newD3D9RestoreDeviceCB()
{
	if(_oldD3D9RestoreDeviceCB)
		_oldD3D9RestoreDeviceCB();

	((AgcmLensFlare*)g_pEngine->GetModule("AgcmLensFlare"))->onResetDevice();
}
//@} Jaewon

AgcmLensFlare::AgcmLensFlare()
{
	SetModuleName("AgcmLensFlare");

	EnableIdle(TRUE);

	m_pWorldCamera	= NULL;
	m_pWorld		= NULL;

	m_pcRender		= NULL;
	m_pcEventNature	= NULL;
	m_pcAgcmMap		= NULL;
	m_pcAgpmTimer	= NULL;
	m_pcsApmMap		= NULL;

	m_fIntensityBorder	= 0.0f;
	
	m_bUpdateThisFrame = TRUE;

	m_iScreenWidth		= 0;
	m_iScreenHeight		= 0;
	m_iCenterOfScreenX	= 0;
	m_iCenterOfScreenY	= 0;
	m_fRecipZ			= 0.0f;

	ZeroMemory(m_vTriFan4, sizeof(My2DVertex) * 4);

	m_ptrLightScreenPos.x = 0;
	m_ptrLightScreenPos.y = 0;

	m_bObscured	= FALSE	;
	m_bInScene	= FALSE	;
	m_bActive	= TRUE	;
	m_bInRoom	= FALSE	;

	//@{ Jaewon 20050119
	m_pOcclusionQuery = NULL;
	m_fIntensity = 0.0f;
	m_fCurrentIntensity = 0.0f;
	//@} Jaewon
}

void	AgcmLensFlare::ResetContent()
{
	// 마고자 (2005-11-09 오후 6:59:03) : 
	// 메모리 잡힌 부분 제거.

	m_stLFSet.Empty();
}

void	AgcmLensFlare::ChangeLensFlareTemplate( const std::string& strFilename )
{
	print_ui_console( _T("템플릿을 새로 로딩합니다.( ini 파일 이름만 치세요 )") );

	char	str[256];
	sprintf( str , "./ini/%s.ini" , ( CHAR * ) strFilename.c_str() );

	if( StreamReadTemplate( str , TRUE ) )
	{
		print_ui_console( _T("변경 성공~") );
	}
	else
	{
		print_ui_console( _T("템플릿 읽기에 실패했어요 다시 원래거 로딩함.") );
		StreamReadTemplate( _T("./Ini/LensFlare.ini") , TRUE );
	}

	CameraStatusChange();
}

AgcmLensFlare::~AgcmLensFlare()
{

}

BOOL	AgcmLensFlare::OnInit()
{
	//StreamReadTemplate("./Ini/LensFlare.ini");

	m_fIntensityBorder = 300.0f;

	//@{ Jaewon 20050120
	// Create a occlusion query.
	HRESULT hr = ((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pOcclusionQuery);
	if(D3D_OK != hr)
		m_pOcclusionQuery = NULL;
	//@} Jaewon

	AS_REGISTER_TYPE_BEGIN(AgcmLensFlare, AgcmLensFlare);
		AS_REGISTER_METHOD1(void, ChangeLensFlareTemplate , string &in );
	AS_REGISTER_TYPE_END;

	return TRUE;
}

BOOL	AgcmLensFlare::OnAddModule()
{
	SetModuleName("AgcmLensFlare");

	m_pcRender		= (AgcmRender *)GetModule("AgcmRender");
	m_pcEventNature	= (AgcmEventNature *)GetModule("AgcmEventNature");
	m_pcAgcmMap		= (AgcmMap*)GetModule("AgcmMap");
	m_pcAgpmTimer	= (AgpmTimer*)GetModule("AgpmTimer");
	m_pcsApmMap		= (ApmMap*)GetModule("ApmMap");

	AgcmEventNature * pcsEventNature = ( AgcmEventNature * ) GetModule( "AgcmEventNature" );

	if (m_pcRender && !m_pcRender->SetCallbackPreRender( CB_PRE_RENDER, this ) ) 
		return FALSE;
	if (m_pcRender && !m_pcRender->SetCallbackPostRender2( CB_POST_RENDER, this ) ) 
		return FALSE;

	if (pcsEventNature && !pcsEventNature->SetCallbackSkyChange( CBSkyChange, this ))
		return FALSE;

	SetMyWorldCamera(GetCamera());
	SetMyWorld(GetWorld());

	//@{ Jaewon 20050120
	// save original device lost & reset callbacks.
    _oldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	_oldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	// set new callbacks.
	_rwD3D9DeviceSetReleaseCallback(newD3D9ReleaseDeviceCB);
    _rwD3D9DeviceSetRestoreCallback(newD3D9RestoreDeviceCB);
	//@} Jaewon

	return TRUE;
}

BOOL	AgcmLensFlare::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmLensFlare::OnIdle");

	if(	m_bActive			&&
		m_bUpdateThisFrame	&&
		!m_bInRoom			) 
	{
		//m_bUpdateThisFrame = false;		TUNER 지금은 비활성화이므로.. 
		RwV3d	lightpos;
		if(m_pcEventNature)
			m_pcEventNature->GetSunPosition	( &lightpos );

		// Light Source 의 Screen Pos 구해논다.
		RwReal f = GetWorldPosToScreenPos(&lightpos,&m_ptrLightScreenPos);

		if(f >= 0.0f)	m_bInScene = TRUE	;
		else			m_bInScene = FALSE	;

		/*if(m_bInScene)
		{
			// Ray Tracing한다
			RpIntersection	intersection	;
			RwLine			pixelRay		;

			RwFrame*	frame = RwCameraGetFrame(m_pWorldCamera);
			RwMatrix*	ltm = RwFrameGetMatrix(frame);
		
			//RwCameraCalcPixelRay( m_pWorldCamera , &pixelRay , &m_ptrLightScreenPos );
			pixelRay.start = lightpos;
			pixelRay.end.x = ltm->pos.x;
			pixelRay.end.y = ltm->pos.y + 300.0f;
			pixelRay.end.z = ltm->pos.z;
		
			intersection.type	=	rpINTERSECTLINE	;
			intersection.t.line	=	pixelRay		;

			m_bObscured = FALSE;

			{
				PROFILE("AgcmLensFlare-intersection");
				if(!m_pcsApmOcTree->m_bOcTreeEnable)
				{
					if(m_pcAgcmMap->m_bUseCullMode)
					{
						m_pcAgcmMap->LoadingSectorsForAllAtomicsIntersection2(1,
						&intersection, AgcmLensFlare::CBLightRayIntersect, this );
					}
					else
					{
						RpWorldForAllAtomicIntersections( m_pWorld, &intersection, CBLightRayIntersect, this );
					}
				}
				else
				{
					m_pcAgcmMap->m_pcsAgcmOcTree->OcTreesForAllAtomicsIntersection2(1,
					&intersection, AgcmLensFlare::CBLightRayIntersect, this );
				}
			}
		}*/
	}

	if( !m_bActive ||
		m_bInRoom	)
	{
		m_fCurrentIntensity = 0.0f;
	}

	//@{ Jaewon 20050120
	// Feedback loop for the flare intensity control
	static clock_t prev = clock();
	clock_t now = clock();
	float dt = float(now - prev)/CLOCKS_PER_SEC;
	m_fCurrentIntensity +=  5.0f * (m_fIntensity - m_fCurrentIntensity) * dt;
	if(m_fCurrentIntensity <= 0)
	{
		m_fCurrentIntensity = 0;
		m_bObscured = true;
	}
	else 
	{
		m_bObscured = false;
		if(m_fCurrentIntensity > 1.0f)
			m_fCurrentIntensity = 1.0f;
	}
	prev = now;
	//@} Jaewon

	// Idle 처리에서 결정 돼는것..
	// m_bInScene			화면안에 해가 있냐 없냐
	// m_ptrLightScreenPos	화면에서 해의 위치
	// m_fCurrentIntensity	화면내에 있을때의 농도.
	// m_bObscured			장애물에 걸리는지.

	// 달처리..
	{
		LFSet *	pLFSet = GetTemplate();
		if( pLFSet->nType == FT_MOON )
		{
			m_bObscured = false;
			m_fCurrentIntensity = 1.0f;
		}
	}

	return TRUE;
}

RpAtomic*
AgcmLensFlare::CBLightRayIntersect	(RpIntersection * intersection, RpWorldSector * sector, RpAtomic * atomic, RwReal distance, void *data) 
{
	AgcmLensFlare* pThis = (AgcmLensFlare*)data;

	if ( NULL == atomic ) return atomic;
	RwFrame*	frame = RpAtomicGetFrame(atomic);

	pThis->m_bObscured = TRUE;
		
	//return atomic;	
	return NULL;
}

BOOL	AgcmLensFlare::OnDestroy()
{
	ResetContent();

	//@{ Jaewon 20050120
	// Destroy the occlusion query.
	if(m_pOcclusionQuery)
	{
		m_pOcclusionQuery->Release();
		m_pOcclusionQuery = NULL;
	}
	//@} Jaewon

	return TRUE;
}

void	AgcmLensFlare::OnCameraStateChange(CAMERASTATECHANGETYPE	ctype)
{
	CameraStatusChange();
}

BOOL	AgcmLensFlare::CB_PRE_RENDER( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmLensFlare::CB_PRE_RENDER");

	AgcmLensFlare* pThis = (AgcmLensFlare*)pClass;
	LFSet *	pLFSet = pThis->GetTemplate();

	if( pLFSet->nType == FT_MOON ) pThis->RenderLensFlare();

	return TRUE;
}

BOOL	AgcmLensFlare::CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmLensFlare::CB_POST_RENDER");

	AgcmLensFlare* pThis = (AgcmLensFlare*)pClass;
	LFSet *	pLFSet = pThis->GetTemplate();

	if( pLFSet->nType == FT_SUN ) pThis->RenderLensFlare();

	return TRUE;
}

void	AgcmLensFlare::RenderLensFlare()
{
	LFSet *	pLFSet = GetTemplate();

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE , ( void * ) FALSE );

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER ,(void*)rwFILTERLINEAR );

	// 낮이거나 광원과의 사이에 장애물이 없을 경우에만 그린다.
	if(	m_bActive	&&
		m_bInScene	&&
		!m_bObscured	)
	{
		BOOL	bExcept = FALSE;
		if(m_pcRender->m_pApmOcTree->m_bOcTreeEnable && m_pcRender->m_bOcLineCheck)
		{
			bExcept = m_pcRender->IsExcept(
				( INT32 ) m_ptrLightScreenPos.x - 1	,
				( INT32 ) m_ptrLightScreenPos.x + 1	,
				( INT32 ) m_ptrLightScreenPos.y		);
		}

		if(!bExcept)
		{
			int	sw,sh,csx,csy;
			sw = m_iScreenWidth;
			sh = m_iScreenHeight;
			csx = m_iCenterOfScreenX;
			csy = m_iCenterOfScreenY;

			float fRealIntensity;
			int iAwayX = ( int ) ( (m_ptrLightScreenPos.x < 0) ? -m_ptrLightScreenPos.x : 
			(m_ptrLightScreenPos.x > sw) ? m_ptrLightScreenPos.x-sw : 0 );
			int iAwayY = ( int ) ( (m_ptrLightScreenPos.y < 0) ? -m_ptrLightScreenPos.y : 
			(m_ptrLightScreenPos.y > sh) ? m_ptrLightScreenPos.y-sh : 0 );

			float fAway = ( float ) ( (iAwayX > iAwayY) ? iAwayX : iAwayY );
			if (fAway > m_fIntensityBorder) fAway = m_fIntensityBorder;
			fRealIntensity = 1.0f - (fAway / m_fIntensityBorder);
			fRealIntensity = FLARE_INTENSITY_MIN + fRealIntensity * (FLARE_INTENSITY_MAX - FLARE_INTENSITY_MIN);

			//@{ Jaewon 20050120
			if(m_pOcclusionQuery)
			// use the query result as an intensity.
				fRealIntensity = m_fCurrentIntensity;
			//@} Jaewon

			int iDistanceX = csx - ( int ) m_ptrLightScreenPos.x;
			int iDistanceY = csy - ( int ) m_ptrLightScreenPos.y;

			int		before_texid = -1;
					
			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDONE);

			int iSpotCenterPosX;
			int iSpotCenterPosY;
			int iSizeDiv2;

			int		myalpha;
			DWORD	mycolor;

			for(int i=0;i< pLFSet->iFlareNum ; ++i )
			{
				iSpotCenterPosX = csx - (int)((float)iDistanceX * pLFSet->stInfo[i].fLinePos);
				iSpotCenterPosY = csy - (int)((float)iDistanceY * pLFSet->stInfo[i].fLinePos);
				iSizeDiv2 = (int)((float)m_iScreenWidth * pLFSet->stInfo[i].fSize/2.0f * (0.6f + fRealIntensity*0.4f));

				myalpha = (int) ( pLFSet->stInfo[i].alpha * fRealIntensity );
				if (myalpha > 255) myalpha = 255;
				if (myalpha < 0) myalpha = 0;
				
				mycolor = ARGB32_TO_DWORD(myalpha,pLFSet->stInfo[i].red,pLFSet->stInfo[i].green,pLFSet->stInfo[i].blue);
				
				m_vTriFan4[0].x = ( float ) ( iSpotCenterPosX-iSizeDiv2 );
				m_vTriFan4[0].y = ( float ) ( iSpotCenterPosY-iSizeDiv2 );

				m_vTriFan4[1].x = ( float ) ( iSpotCenterPosX+iSizeDiv2 );
				m_vTriFan4[1].y = ( float ) ( iSpotCenterPosY-iSizeDiv2 );

				m_vTriFan4[2].x = ( float ) ( iSpotCenterPosX+iSizeDiv2 );
				m_vTriFan4[2].y = ( float ) ( iSpotCenterPosY+iSizeDiv2 );

				m_vTriFan4[3].x = ( float ) ( iSpotCenterPosX-iSizeDiv2 );
				m_vTriFan4[3].y = ( float ) ( iSpotCenterPosY+iSizeDiv2 );

				for(int j=0;j<4;++j)
				{
					m_vTriFan4[j].color = mycolor;
				}
		
				if(before_texid != pLFSet->stInfo[i].tex_id)		// texture set
				{
					//@{ 2006/12/30 burumal
					/*
					before_texid = pLFSet->stInfo[i].tex_id;
					RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
						(void *)RwTextureGetRaster(pLFSet->pFlares[before_texid]));
					*/
					if ( pLFSet->stInfo[i].tex_id < FLARE_TEX_NUM )
					{
						before_texid = pLFSet->stInfo[i].tex_id;
						RwRenderStateSet(rwRENDERSTATETEXTURERASTER, 
							(void *)RwTextureGetRaster(pLFSet->pFlares[before_texid]));
					}
					//@}
				}

				RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
				RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

				RwD3D9SetFVF(MY2D_VERTEX_FLAG);
				RwD3D9DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , m_vTriFan4 , SIZE_MY2D_VERTEX );
			}

			RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *)rwBLENDSRCALPHA);
			RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *)rwBLENDINVSRCALPHA);
		}
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);

	//@{ Jaewon 20050120
	checkAndUpdateOcclusionQuery();
	//@} Jaewon
}

void	AgcmLensFlare::SetMyWorldCamera(RwCamera*	pCam)
{
	m_pWorldCamera	= pCam;
	CameraStatusChange();
}

void	AgcmLensFlare::CameraStatusChange()
{
	RwCamera * pCamera = GetCamera();
	if (!m_pWorldCamera || !pCamera || m_pWorldCamera != pCamera )
		return;	

	RwRaster*		camera_raster;
	camera_raster = RwCameraGetRaster(m_pWorldCamera);
	
	m_iScreenWidth = RwRasterGetWidth(camera_raster);
	m_iScreenHeight = RwRasterGetHeight(camera_raster);
		
	m_iCenterOfScreenX = m_iScreenWidth>>1;
	m_iCenterOfScreenY = m_iScreenHeight>>1;

	for(int i=0;i<4;++i)
	{
		m_vTriFan4[i].rhw = 1.0f;
		m_vTriFan4[i].z = 0.0f;
	}

	m_vTriFan4[0].u = 0.0f;
	m_vTriFan4[0].v = 0.0f;

	m_vTriFan4[1].u = 1.0f;
	m_vTriFan4[1].v = 0.0f;

	m_vTriFan4[2].u = 1.0f;
	m_vTriFan4[2].v = 1.0f;

	m_vTriFan4[3].u = 0.0f;
	m_vTriFan4[3].v = 1.0f;
}

float	AgcmLensFlare::GetWorldPosToScreenPos		( RwV3d * pWorldPos , RwV2d * pPoint )
{
	RwMatrix	* pViewMatrix	= RwCameraGetViewMatrix( m_pWorldCamera );
	ASSERT( NULL != pViewMatrix );

    RwCamera            *camera;
    D3DMATRIX           viewMatrix;
    D3DMATRIX           projMatrix;
	D3DMATRIX			projviewMatrix;
	D3DXMatrixIdentity((D3DXMATRIX *)&projMatrix);

    //
    // Save the camera pointer
    //
    camera = m_pWorldCamera;
      
	RwV3d	* vec;
	vec		= RwMatrixGetRight	( pViewMatrix );
	viewMatrix.m[0][0] = vec->x;
	viewMatrix.m[0][1] = vec->y;
	viewMatrix.m[0][2] = vec->z;
	vec		= RwMatrixGetUp		( pViewMatrix );
	viewMatrix.m[1][0] = vec->x;
	viewMatrix.m[1][1] = vec->y;
	viewMatrix.m[1][2] = vec->z;
	vec		= RwMatrixGetAt		( pViewMatrix );
	viewMatrix.m[2][0] = vec->x;
	viewMatrix.m[2][1] = vec->y;
	viewMatrix.m[2][2] = vec->z;
	vec		= RwMatrixGetPos	( pViewMatrix );
	viewMatrix.m[3][0] = vec->x;
	viewMatrix.m[3][1] = vec->y;
	viewMatrix.m[3][2] = vec->z;
	
	viewMatrix.m[0][3] = 0.0f;
    viewMatrix.m[1][3] = 0.0f;
	viewMatrix.m[2][3] = 0.0f;
    viewMatrix.m[3][3] = 1.0f;

    //
    // Projection matrix
    //
    projMatrix.m[0][0] = camera->recipViewWindow.x;
    projMatrix.m[1][1] = camera->recipViewWindow.y;

    // Shear X, Y by view offset with Z invariant 
    projMatrix.m[2][0] =
        camera->recipViewWindow.x * camera->viewOffset.x;
    projMatrix.m[2][1] =
        camera->recipViewWindow.y * camera->viewOffset.y;

    // Translate to shear origin 
    projMatrix.m[3][0] =
        -camera->recipViewWindow.x * camera->viewOffset.x;

    projMatrix.m[3][1] =
        -camera->recipViewWindow.y * camera->viewOffset.y;

    // Projection type 
    if (camera->projectionType == rwPARALLEL)
    {
        projMatrix.m[2][2] =
            1.0f / (camera->farPlane - camera->nearPlane);
        projMatrix.m[2][3] = 0.0f;
        projMatrix.m[3][3] = 1.0f;
    }
    else
    {
        projMatrix.m[2][2] =
            camera->farPlane / (camera->farPlane - camera->nearPlane);
        projMatrix.m[2][3] = 1.0f;
        projMatrix.m[3][3] = 0.0f;
    }

    projMatrix.m[3][2] = -projMatrix.m[2][2] * camera->nearPlane;

	D3DXMatrixMultiply((D3DXMATRIX *)&projviewMatrix,(const D3DXMATRIX *)&viewMatrix,(const D3DXMATRIX *)&projMatrix);

	D3DXVECTOR3 InV(pWorldPos->x,pWorldPos->y,pWorldPos->z);
	D3DXVECTOR4	OutV4;

	D3DXVec3Transform(&OutV4,(const D3DXVECTOR3*)&InV,(const D3DXMATRIX *)&projviewMatrix);
	
	pPoint->x = OutV4.x/OutV4.w * RwRasterGetWidth	( RwCameraGetRaster( camera ) )/camera->recipViewWindow.x;
	pPoint->y = OutV4.y/OutV4.w * RwRasterGetHeight	( RwCameraGetRaster( camera ) )/camera->recipViewWindow.y;

	return OutV4.w;
}

BOOL	AgcmLensFlare::StreamReadTemplate(char* szFile, BOOL bDecryption)
{
	if (!szFile)
		return FALSE;

	// 데이타 정리..
	ResetContent();
	
	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];
	CHAR			szFileName[256];
	INT32			lIndex;
	INT32			lFlareNum;
	float			fSize;
	float			fLinePos;
	UINT32			red,green,blue,alpha;
	INT32			texid;

	LFSet *	pLFSet = GetTemplate();

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	if( !csStream.Open(szFile, 0, bDecryption) )
	{
		// 파일찾을 수 없음..
		return FALSE;
	}

	{
		csStream.ReadSectionName(0);
		csStream.SetValueID(-1);

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, FLARE_INI_NAME_FILE_NAME))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d:%s", &lIndex, szFileName);
				ASSERT( lIndex < FLARE_TEX_NUM );

				if( lIndex < FLARE_TEX_NUM )
				{
					pLFSet->pFlares[lIndex] = RwTextureRead(szFileName, NULL);
					if( pLFSet->pFlares[lIndex] == NULL ){ 
						ASSERT(!"LensFlare image가 없습니다.");
						return FALSE;  
					}
				}
			}
		}
	}

	{
		csStream.ReadSectionName(1);
		csStream.SetValueID(-1);
		INT32	index = 0;

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, FLARE_INI_FLARE_NUMBERS))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%d", &lFlareNum);
				pLFSet->iFlareNum = lFlareNum;
			}
			else if(!strcmp(szValueName, FLARE_INI_FLARE_SETTINGS))
			{
				csStream.GetValue(szValue, 256);
				sscanf(szValue, "%f,%f,%d,%d,%d,%d,%d", &fSize,&fLinePos,&red,&green,&blue,&alpha,&texid);

				pLFSet->stInfo[index].fSize		= fSize		;
				pLFSet->stInfo[index].fLinePos	= fLinePos	;
				pLFSet->stInfo[index].red		= red		;
				pLFSet->stInfo[index].green		= green		;
				pLFSet->stInfo[index].blue		= blue		;
				pLFSet->stInfo[index].alpha		= alpha		;
				pLFSet->stInfo[index].tex_id	= texid		;
								
				++index;
			}
		}
	}

	{
		// 추가정보 읽기.
		csStream.ReadSectionName(2);
		csStream.SetValueID(-1);
		INT32	index = 0;

		while(csStream.ReadNextValue())
		{
			szValueName = csStream.GetValueName();

			if(!strcmp(szValueName, FLARE_INI_FLARE_OPTION))
			{
				csStream.GetValue(szValue, 256);

				INT32	lMoon;
				sscanf(szValue, "%d", &lMoon);
				pLFSet->nType = lMoon;
			}
		}
	}

	return TRUE;
}

BOOL	AgcmLensFlare::StreamWriteTemplate(CHAR* szFile, BOOL bEncryption)
{
	if (!szFile)
		return FALSE;

	LFSet *	pLFSet = GetTemplate();

	INT32			i;
	ApModuleStream	csStream;
	CHAR			szValue[256];

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	
	csStream.SetSection("[Textures]");

	for(i=0;i<FLARE_TEX_NUM;++i)
	{
		sprintf(szValue, "%d:%s", i, RwTextureGetName(pLFSet->pFlares[i]));
		if(!csStream.WriteValue("FN", szValue))
			return FALSE;
	}

	csStream.SetSection("[Setting]");

	sprintf(szValue,"Size(0.0f~1.0f),LinePos(1.0-Sun,0.0-Screen center,-1.0 - oposite at sun: -1.0~1.0),red,green,blue,alpha,TextureID");
	csStream.WriteValue("JustCommentForModify",szValue);

	sprintf(szValue,"%d",pLFSet->iFlareNum);
	if(!csStream.WriteValue(FLARE_INI_FLARE_NUMBERS, szValue))
		return FALSE;

	for(i=0;i<pLFSet->iFlareNum;++i)
	{
		sprintf(szValue,"%f,%f,%d,%d,%d,%d,%d", pLFSet->stInfo[i].fSize,pLFSet->stInfo[i].fLinePos,pLFSet->stInfo[i].red
			,pLFSet->stInfo[i].green,pLFSet->stInfo[i].blue,pLFSet->stInfo[i].alpha,
			pLFSet->stInfo[i].tex_id);

		if(!csStream.WriteValue(FLARE_INI_FLARE_SETTINGS, szValue))
			return FALSE;
	}

	csStream.Write(szFile, 0 , bEncryption);

	return TRUE;
}

BOOL	AgcmLensFlare::CBSkyChange ( PVOID	pData, PVOID pClass, PVOID pCustData )
{
	AgcmLensFlare	* pThis		= (AgcmLensFlare	*) pClass	;
	ASkySetting		* pSkySet	= (ASkySetting		*) pData	;

	ASSERT( NULL != pThis	);
	ASSERT( NULL != pSkySet	);

	// 스카이설정으로 렌즈 플레어 온오프
	if( pSkySet->nWeatherEffect1 & ASkySetting::USE_LENS_FLARE )
	{
		pThis->m_bInRoom = FALSE;
	}
	else
	{
		pThis->m_bInRoom = TRUE	;
	}

	/*
	AgpdCharacter*	pdCharacter = (AgpdCharacter*)	pData;

	// 사운드 room type 처리
	ApmMap::RegionTemplate*	pTemplate = pThis->m_pcsApmMap->GetTemplate( pdCharacter->m_nBindingRegionIndex );

	// 던젼인지 확인
	//@{ kday 20050201
	//validation check
	ASSERT(pTemplate);
	if( pTemplate )
	{
		if(pTemplate->ti.stType.uFieldType == ApmMap::FT_PVP)
		{
			pThis->m_bInRoom = TRUE;
		}
		else
		{
			pThis->m_bInRoom = FALSE;
		}
	}
	//@} kday

	*/

	return	TRUE;
}

//@{ Jaewon 20050120
static UINT32 cSunPixelSize = 2;
static UINT32 cSunPixelArea = cSunPixelSize*cSunPixelSize;
void AgcmLensFlare::checkAndUpdateOcclusionQuery()
{
	if(NULL == m_pOcclusionQuery)
		return;

	DWORD numberOfPixelsDrawn;

	// Check last query result, if any.
	HRESULT hr = m_pOcclusionQuery->GetData(&numberOfPixelsDrawn, sizeof(DWORD), 0);
	
	if(S_OK == hr)
	// The result signaled
	{
		// Update the visibility & intensity.
		m_fIntensity = float(numberOfPixelsDrawn)/float(cSunPixelArea);

		//TCHAR buf[256];
		//_stprintf(buf, _T("numberOfPixelsDrawn = %d\n"), numberOfPixelsDrawn);
		//OutputDebugString(buf);
	}
	else if(S_FALSE == hr)
	// Not ready
	{
		return;
	}
	else
	{
		ASSERT(!"m_pOcclusionQuery->GetData returned an error!");
	}

	// Issue a new query.
	hr = m_pOcclusionQuery->Issue(D3DISSUE_BEGIN);
	ASSERT(D3D_OK == hr);

		// Get the screen position of sun.
		RwV3d worldPos, cameraPos, screenPos;
		float rhw;
		if(m_pcEventNature)
			m_pcEventNature->GetSunPosition	(&worldPos);
		else
			ASSERT(!"AgcmLensFlare needs AgcmEventNature!");
		RwV3dTransformPoint(&cameraPos, &worldPos, RwCameraGetViewMatrix(m_pWorldCamera));
		rhw = 1.0f/cameraPos.z;
		screenPos.x = cameraPos.x * rhw * m_iScreenWidth + m_pWorldCamera->viewOffset.x;
		screenPos.y = cameraPos.y * rhw * m_iScreenHeight + m_pWorldCamera->viewOffset.y;
		screenPos.z = m_pWorldCamera->zScale * rhw + m_pWorldCamera->zShift;

		// Set render states.
		RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
		((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

		// Draw Sun.
		RwIm2DVertex rectVertices[4];
		memset(rectVertices, 0, sizeof(rectVertices));

		for(int i=0; i<4; ++i)
		{
			// Red
			RwIm2DVertexSetIntRGBA(&rectVertices[i], 255, 0, 0, 255);

			// Screen z
			RwIm2DVertexSetScreenZ(&rectVertices[i], screenPos.z);

			// RHW
			RwIm2DVertexSetRecipCameraZ(&rectVertices[i], rhw);
		}
		
		RwIm2DVertexSetScreenX(&rectVertices[0], screenPos.x-cSunPixelSize/2);
		RwIm2DVertexSetScreenY(&rectVertices[0], screenPos.y-cSunPixelSize/2);

		RwIm2DVertexSetScreenX(&rectVertices[1], screenPos.x-cSunPixelSize/2);
		RwIm2DVertexSetScreenY(&rectVertices[1], screenPos.y+cSunPixelSize/2);

		RwIm2DVertexSetScreenX(&rectVertices[2], screenPos.x+cSunPixelSize/2);
		RwIm2DVertexSetScreenY(&rectVertices[2], screenPos.y-cSunPixelSize/2);

		RwIm2DVertexSetScreenX(&rectVertices[3], screenPos.x+cSunPixelSize/2);
		RwIm2DVertexSetScreenY(&rectVertices[3], screenPos.y+cSunPixelSize/2);

		RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, rectVertices, 4);

		((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	hr = m_pOcclusionQuery->Issue(D3DISSUE_END);
	ASSERT(D3D_OK == hr);
}
void AgcmLensFlare::onLostDevice()
{
	// Destroy the occlusion query.
	if(m_pOcclusionQuery)
	{
		m_pOcclusionQuery->Release();
		m_pOcclusionQuery = NULL;
	}
}

void AgcmLensFlare::onResetDevice()
{
	// Create a occlusion query.
	HRESULT hr = ((IDirect3DDevice9*)RwD3D9GetCurrentD3DDevice())->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_pOcclusionQuery);
	if(D3D_OK != hr)
		m_pOcclusionQuery = NULL;
}
//@} Jaewon

	/*m_pFlares[0] = RwTextureRead(RWSTRING("flare1.png"), NULL);
    if( m_pFlares[0] == NULL ){  return FALSE;    }

	m_pFlares[1] = RwTextureRead(RWSTRING("flare2.png"), NULL);
    if( m_pFlares[1] == NULL ){  return FALSE;    }

	m_pFlares[2] = RwTextureRead(RWSTRING("flare3.png"), NULL);
    if( m_pFlares[2] == NULL ){  return FALSE;    }
	*/
/*	m_pFlares[0] = RwTextureRead(RWSTRING("LF_Glow.tif"), NULL);
    if( m_pFlares[0] == NULL ){  return FALSE;    }

	m_pFlares[1] = RwTextureRead(RWSTRING("LF_OuterGlow.tif"), NULL);
    if( m_pFlares[1] == NULL ){  return FALSE;    }

	m_pFlares[2] = RwTextureRead(RWSTRING("LF_Ray.tif"), NULL);
    if( m_pFlares[2] == NULL ){  return FALSE;    }

	m_pFlares[3] = RwTextureRead(RWSTRING("LF_Star.tif"), NULL);
    if( m_pFlares[3] == NULL ){  return FALSE;    }

	m_pFlares[4] = RwTextureRead(RWSTRING("LF_Streak.tif"), NULL);
    if( m_pFlares[4] == NULL ){  return FALSE;    }

	m_pFlares[5] = RwTextureRead(RWSTRING("LF_Sec1.tif"), NULL);
    if( m_pFlares[5] == NULL ){  return FALSE;    }

	m_pFlares[6] = RwTextureRead(RWSTRING("LF_Sec2.tif"), NULL);
    if( m_pFlares[6] == NULL ){  return FALSE;    }

	m_pFlares[7] = RwTextureRead(RWSTRING("LF_Sec3.tif"), NULL);
    if( m_pFlares[7] == NULL ){  return FALSE;    }

	for(int t = 0; t<8; ++t)
	{
		RwTextureSetFilterMode(m_pFlares[t] ,rwFILTERLINEAR );
	}

	int i=0;
	m_stInfo[i].fSize = 0.15f;
	m_stInfo[i].fLinePos = 1.0f;
	m_stInfo[i].dwColor.red = 179;	m_stInfo[i].dwColor.green = 128;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 125;
	m_stInfo[i].tex_id = 0;

	++i;
	m_stInfo[i].fSize = 0.20f;
	m_stInfo[i].fLinePos = 1.0f;
	m_stInfo[i].dwColor.red = 239;	m_stInfo[i].dwColor.green = 228;
	m_stInfo[i].dwColor.blue = 180;	m_stInfo[i].dwColor.alpha = 165;
	m_stInfo[i].tex_id = 1;

	++i;
	m_stInfo[i].fSize = 0.24f;
	m_stInfo[i].fLinePos = 1.0f;
	m_stInfo[i].dwColor.red = 174;	m_stInfo[i].dwColor.green = 135;
	m_stInfo[i].dwColor.blue = 40;	m_stInfo[i].dwColor.alpha = 95;
	m_stInfo[i].tex_id = 2;

	++i;
	m_stInfo[i].fSize = 0.3f;
	m_stInfo[i].fLinePos = 1.0f;
	m_stInfo[i].dwColor.red = 135;	m_stInfo[i].dwColor.green = 100;
	m_stInfo[i].dwColor.blue = 20;	m_stInfo[i].dwColor.alpha = 85;
	m_stInfo[i].tex_id = 3;

	++i;
	m_stInfo[i].fSize = 0.35f;
	m_stInfo[i].fLinePos = 1.0f;
	m_stInfo[i].dwColor.red = 239;	m_stInfo[i].dwColor.green = 228;
	m_stInfo[i].dwColor.blue = 180;	m_stInfo[i].dwColor.alpha = 95;
	m_stInfo[i].tex_id = 4;

	++i; // 5
	m_stInfo[i].fSize = 0.15f;
	m_stInfo[i].fLinePos = 0.7f;
	m_stInfo[i].dwColor.red = 180;	m_stInfo[i].dwColor.green = 132;
	m_stInfo[i].dwColor.blue = 50;	m_stInfo[i].dwColor.alpha = 90;
	m_stInfo[i].tex_id = 0;

	++i; // 6
	m_stInfo[i].fSize = 0.19f;
	m_stInfo[i].fLinePos = 0.4f;
	m_stInfo[i].dwColor.red = 180;	m_stInfo[i].dwColor.green = 132;
	m_stInfo[i].dwColor.blue = 120;	m_stInfo[i].dwColor.alpha = 80;
	m_stInfo[i].tex_id = 5;

	++i; // 7
	m_stInfo[i].fSize = 0.25f;
	m_stInfo[i].fLinePos = -0.1f;
	m_stInfo[i].dwColor.red = 180;	m_stInfo[i].dwColor.green = 152;
	m_stInfo[i].dwColor.blue = 120;	m_stInfo[i].dwColor.alpha = 60;
	m_stInfo[i].tex_id = 5;

	++i; // 8
	m_stInfo[i].fSize = 0.28f;
	m_stInfo[i].fLinePos = -0.4f;
	m_stInfo[i].dwColor.red = 190;	m_stInfo[i].dwColor.green = 112;
	m_stInfo[i].dwColor.blue = 50;	m_stInfo[i].dwColor.alpha = 40;
	m_stInfo[i].tex_id = 6;

	++i; // 9
	m_stInfo[i].fSize = 0.3f;
	m_stInfo[i].fLinePos = -0.75f;
	m_stInfo[i].dwColor.red = 170;	m_stInfo[i].dwColor.green = 172;
	m_stInfo[i].dwColor.blue = 110;	m_stInfo[i].dwColor.alpha = 20;
	m_stInfo[i].tex_id = 7;
	
	/*
	//------------------------------------------------------------------	tex = 1
	int i=0;
	m_stInfo[i].fSize = 0.10f;
	m_stInfo[i].fLinePos = 0.8f;
	m_stInfo[i].dwColor.red = 179;	m_stInfo[i].dwColor.green = 128;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 52;
	m_stInfo[i].tex_id = 1;

	++i; 
	m_stInfo[i].fSize = 0.01f;
	m_stInfo[i].fLinePos = 0.7f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 0;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 179;
	m_stInfo[i].tex_id = 1;

	++i;
	m_stInfo[i].fSize = 0.05f;
	m_stInfo[i].fLinePos = 0.1f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 255;
	m_stInfo[i].dwColor.blue = 128;	m_stInfo[i].dwColor.alpha = 128;
	m_stInfo[i].tex_id = 1;

	++i;
	m_stInfo[i].fSize = 0.09f;
	m_stInfo[i].fLinePos = -0.3f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 255;
	m_stInfo[i].dwColor.blue = 153;	m_stInfo[i].dwColor.alpha = 128;
	m_stInfo[i].tex_id = 1;

	++i;
	m_stInfo[i].fSize = 0.32f;
	m_stInfo[i].fLinePos = -1.0f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 179;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 102;
	m_stInfo[i].tex_id = 1;

	//------------------------------------------------------------------  tex = 0
	//5
	++i;
	m_stInfo[i].fSize = 0.05f;
	m_stInfo[i].fLinePos = 0.6f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 255;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 128;
	m_stInfo[i].tex_id = 0;

	++i;
	m_stInfo[i].fSize = 0.05f;
	m_stInfo[i].fLinePos = 0.4f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 128;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 255;
	m_stInfo[i].tex_id = 0;

	++i;
	m_stInfo[i].fSize = 0.05f;
	m_stInfo[i].fLinePos = -0.2f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 0;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 255;
	m_stInfo[i].tex_id = 0;

	++i;
	m_stInfo[i].fSize = 0.15f;
	m_stInfo[i].fLinePos = -0.4f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 179;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 77;
	m_stInfo[i].tex_id = 0;

	//------------------------------------------------------------------	tex = 2
	//9
	++i;
	m_stInfo[i].fSize = 0.13f;
	m_stInfo[i].fLinePos = 0.5f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 255;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 128;
	m_stInfo[i].tex_id = 2;

	++i;
	m_stInfo[i].fSize = 0.15f;
	m_stInfo[i].fLinePos = -0.7f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 128;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 52;
	m_stInfo[i].tex_id = 2;

	++i;
	m_stInfo[i].fSize = 0.40f;
	m_stInfo[i].fLinePos = -1.3f;
	m_stInfo[i].dwColor.red = 255;	m_stInfo[i].dwColor.green = 0;
	m_stInfo[i].dwColor.blue = 0;	m_stInfo[i].dwColor.alpha = 179;
	m_stInfo[i].tex_id = 2;
	
	//---------------------------------------------------------------------*/
