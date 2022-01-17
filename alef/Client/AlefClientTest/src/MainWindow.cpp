// MainWindow.cpp: implementation of the MainWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpcollis.h"
#include "rtpick.h"
#include "rtcharse.h"

#ifdef USE_DPVS
#include "rpdpvs.h"
#endif //USE_DPVS

#include <skeleton.h>
#include <menu.h>
//#include "events.h"
#include <camera.h>
#include <MagDebug.h>
#include "..\MyEngine.h"
#include "MainWindow.h"
#include "SubWindow.h"
#include "win.h"
//#include "texshadow.h"

#include "AgcmEffect.h"

#define MAP_WIDTH	(10)
#define MAP_HEIGHT	(10)

#define MAP_SIZE	(MAP_WIDTH * MAP_HEIGHT)

#define NOV			(MAP_SIZE * 4)
#define NOT			((MAP_WIDTH - 1) * (MAP_HEIGHT - 1) * 2)

#define MAP(x,y)	(VertexList[(y) * MAP_WIDTH + (x)])

RwV3d VertexList[MAP_SIZE];

extern SubWindow	g_SubWindow		;

static RpAtomicCallBackRender	DefaultAtomicRenderCallback	;
static SortAtomic				*AlphaSortedAtomics			;
static UINT32					lCurrent;

static RwV3d	g_stCharacterPos[23] = 
{
	{-6564,206,-7990}, 
	{-8817,148,-6313}, 
	{-9814,247,-9516}, 
	{-7105,136,-6987}, 
	{-8837,99,-9516}, 

	{-8388,241,-6180}, 
	{-10589,96,-8405}, 
	{-10508,22,-6544}, 
	{-7587,224,-9109}, 
	{-7142,218,-6366}, 

	{-9298,131,6908}, 
	{-8530,353,14636}, 
	{-3677,772,14031}, 
	{1978,349,1423}, 
	{6342,163,15325}, 
	{12272,360,11492}, 
	{15854,142,14876}, 
	{16285,438,8198}, 
	{13188,1188,1999}, 
	{14405,160,-3639}, 
	{12485,317,-8673}, 
	{8074,422,-4840}, 
	{4482,162,-10078}
};

static RwReal	g_fDegree[23] = 
{
	30,		40,		-30,	90,	0, 
	0,		30,	90,	30,		-40, 
	0,		0,		0,		0,		0, 
	0,		0,		0,		0,		0, 
	0,		180,	0
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MainWindow::MainWindow()
{
	m_pBmp		= NULL;
	m_pRaster	= NULL;
	m_nImageX	= 0;
	m_nImageY	= 0;
	m_bLButtonDown		= FALSE;
	m_bRButtonDown		= FALSE;
	m_pSelectedAtomic	= NULL;

	m_bLCtrl	= FALSE;
	m_bRCtrl	= FALSE;

	m_bLShift	= FALSE;

	m_pPlayerFrame	= NULL;

	m_fTurn = 0;
	m_fTilt = 0;

	m_fCameraPanInitial	= 15.0f;
	m_fCameraPanUnit	= 5.0f;
	m_fCameraPan		= m_fCameraPanInitial;

	m_ulPrevTime = 0;

	m_nCharacter = 0;

	m_nPCType	= 0;
	m_nArmorPart = 0;

	m_fSpeed = 2.0f;

	m_bMove = FALSE;
	m_bTurn = FALSE;
}

MainWindow::~MainWindow()
{

}

BOOL MainWindow::OnInit			(					)	// 초기화 , 데이타 수집 & 그래픽 데이타 로딩.	WindowUI 의경우 Open돼는 시점, Full UI의 경우 Setting돼는 시점.
{
	RwFrame		*pstCameraFrame = RwCameraGetFrame( g_pEngine->m_pCamera	);
	RwV3d stXAxis = {1.0f, 0.0f, 0.0f};
	RwV3d stCameraPos = {0.0f, 400.0f, -800.0f};

	m_pPlayerFrame = RwFrameCreate();
	if (!m_pPlayerFrame)
		return FALSE;

	if (!RwFrameAddChild(m_pPlayerFrame, pstCameraFrame))
		return FALSE;

	RwFrameRotate(pstCameraFrame, &stXAxis, m_fCameraPanInitial, rwCOMBINEREPLACE);
	RwFrameTranslate(pstCameraFrame, &stCameraPos, rwCOMBINEPOSTCONCAT);

	if (!MYENGINE->m_csApmObject.StreamReadTemplate("Ini\\AlefObjectTemplates.ini"))
		return FALSE;

	if (!MYENGINE->m_csAgpmCharacter.StreamReadTemplate("Ini\\CharacterTemplate.ini"))
		return FALSE;

	if (!MYENGINE->m_csAgpmItem.StreamReadTemplate("Ini\\ItemTemplate.ini"))
		return FALSE;

	if (!InitCharacters())
		return FALSE;

	if (!MYENGINE->m_csApmObject.StreamRead("Ini\\AlefObjects.ini"))
		return FALSE;

	{
		RwReal	fScale	= 20.0f;
		RwV3d	scale	= {18.0f, 8.0f, 18.0f};
		RwV3d	pos;

		m_pSkyBox = MYENGINE->m_csAgcmObject.LoadClump("skybox.dff");
		if (m_pSkyBox)
		{
#ifdef USE_DPVS
			RpDPVSWorldAddOccludingClump(g_pEngine->m_pWorld, m_pSkyBox);
#else
			RpWorldAddClump(g_pEngine->m_pWorld, m_pSkyBox);
#endif //USE_DPVS

			pos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(m_pSkyBox)));
			pos.y = 800.0f;

			RwFrameScale(RpClumpGetFrame(m_pSkyBox), &scale, rwCOMBINEREPLACE);
			RwFrameTranslate(RpClumpGetFrame(m_pSkyBox), &pos, rwCOMBINEPOSTCONCAT);
		}
	}

	return TRUE;
}
BOOL MainWindow::OnLButtonDown	( RsMouseStatus *ms	)
{
	// m_nImageX	= ( INT32 ) ms->pos.x;
	// m_nImageY	= ( INT32 ) ms->pos.y;

	m_bLButtonDown		= TRUE	;

	m_fMoveDirection	= 0;
	m_bMove				= TRUE	;
	m_bTurn				= TRUE	;

	if (m_bLCtrl)
		MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_RUN, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
	else
		MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

	m_PrevMouseStatus	= *ms	;

	m_pSelectedAtomic = RwCameraPickAtomicOnPixel( g_pEngine->m_pCamera, &ms->pos );

    if (m_pSelectedAtomic)
    {
        AtomicGetBBox( m_pSelectedAtomic , &PickBox );
    }
	
	return FALSE;
}

BOOL MainWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	//g_pEngine->AddChild( & g_SubWindow );

	m_bRButtonDown		= TRUE;
	MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_NORMALATTACK1, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE);

	m_PrevMouseStatus	= *ms	;
	
	return FALSE;
}

BOOL MainWindow::OnLButtonUp	( RsMouseStatus *ms	)
{
	m_bLButtonDown		= FALSE	;
	m_bMove				= FALSE ;
	m_bTurn				= FALSE ;

	if (!m_bRButtonDown)
		MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_NORMAL, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

	//m_PrevMouseStatus	= *ms	;
	m_pSelectedAtomic	= NULL	;
	return TRUE;
}

BOOL MainWindow::OnRButtonUp	( RsMouseStatus *ms	)
{
	m_bRButtonDown	= FALSE;

	return FALSE;
}

void MainWindow::OnPreRender	( RwRaster *raster	)	// 월드 렌더하기전.
{
//	SetRenderCallBackForAllAtomic();
}

void MainWindow::OnRender		( RwRaster *raster	)	// 렌더는 여기서;
{
//	RenderAlphaSortedAtomics();
//	RenderTextureShadow();
}

void MainWindow::OnPostRender	( RwRaster *raster	)	// 렌더는 여기서;
{
//	DestroyAlphaAtomicsList();

	char		str[ 256 ];
	AgpdCharacter *pcsCharacter;
	AgcdCharacter *pcsAgcdCharacter;
	INT32		lIndex = 0;
	INT32		i = 1;

	for (pcsCharacter = MYENGINE->m_csAgpmCharacter.GetCharSequence(&lIndex); pcsCharacter; pcsCharacter = MYENGINE->m_csAgpmCharacter.GetCharSequence(&lIndex))
	{
		pcsAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pcsCharacter);

		INT16	nHP = 0;
		AgpdFactorCharPoint	*pcsFactorCharPoint = (AgpdFactorCharPoint *) MYENGINE->m_csAgpmFactors.GetFactor(&pcsCharacter->m_csFactor, AGPM_FACTORS_TYPE_CHAR_POINT);
		if (pcsFactorCharPoint)
			nHP = pcsFactorCharPoint->nHP;

		sprintf(str, "Player %s (%s%s) Pos ( %3.3f , %3.3f , %3.3f ) , HP(%d) , Status(%d)", 
			pcsCharacter->m_szID, 
			pcsCharacter->m_bMove ? "Moving":"Stop", 
			pcsAgcdCharacter->m_bBlending ? "<Blending>":"", 
			pcsCharacter->m_stPos.x, 
			pcsCharacter->m_stPos.y, 
			pcsCharacter->m_stPos.z,
			nHP,
			pcsCharacter->m_unCurrentStatus);
		HanFontDrawText(raster, 0, 12 * i, str);
		i++;
	}
}

/*
 *****************************************************************************
 */
static RpClump *
_destroyClumpCB(RpClump *c, void *d)
{
    RpWorldRemoveClump( g_pEngine->m_pWorld , c );
    RpClumpDestroy(c);

    return c;
}


void MainWindow::OnClose		(					)	// 마무리 , 그래픽 데이타 릴리즈. WindowUI 의 경우 Close돼는 시점,  FullUI의 경우 다른 UI가 Setting돼기 전.
{
	MYENGINE->m_csAgcmEffect.EffectSetAllDataRemove();
	// 클럼프 디스트로이. 
//	RpWorldForAllClumps( g_pEngine->m_pWorld , _destroyClumpCB , NULL );

	if( m_pBmp )
	{
		RwImageDestroy( m_pBmp );
	}

	if( m_pRaster )
	{
		RwRasterDestroy( m_pRaster );
	}

	if( m_pSkyBox )
	{
#ifdef USE_DPVS
		RpDPVSWorldRemoveClump( g_pEngine->m_pWorld, m_pSkyBox );
#else
		RpWorldRemoveClump( g_pEngine->m_pWorld, m_pSkyBox );
#endif //USE_DPVS

		RpClumpDestroy( m_pSkyBox );
	}

//	DestroyTextureShadow();
}

BOOL MainWindow::OnKeyDown		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
        {
			AgcmCharacterAnimType	nAnimType = (AgcmCharacterAnimType) (ks->keyCharCode - '1');
			AgpdCharacter	*pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.GetCharacter(1);
			AgcdCharacter	*pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);
			//INT16			nIndex;

			if (m_nPCType == 4 && ks->keyCharCode == '5')
			{
				RwV3d Pos = { 0.0f, 100.0f, 0.0f };
				MYENGINE->m_csAgcmEffect.UseEffectSet(827003, &Pos, m_ulPrevTime, 1, RpClumpGetFrame(pstAgcdCharacter->m_pClump));
				MYENGINE->m_csAgcmEffect.UseEffectSet(827001, NULL, m_ulPrevTime, 1, pstAgcdCharacter->m_pInHierarchy->pNodeInfo[RpHAnimIDGetIndex(pstAgcdCharacter->m_pInHierarchy, 5)].pFrame);
			}
			else if (m_nPCType == 2 && ks->keyCharCode == '8')
			{
				MYENGINE->m_csAgcmEffect.UseEffectSet(828002, NULL, m_ulPrevTime, 1, RpClumpGetFrame(pstAgcdCharacter->m_pClump));
			}
			else if (m_nPCType == 3 && ks->keyCharCode == '5')
			{
				MYENGINE->m_csAgcmEffect.UseEffectSet(828003, NULL, m_ulPrevTime, 1, pstAgcdCharacter->m_pInHierarchy->pNodeInfo[RpHAnimIDGetIndex(pstAgcdCharacter->m_pInHierarchy, 5)].pFrame);
				MYENGINE->m_csAgcmEffect.UseEffectSet(828004, NULL, m_ulPrevTime, 1, pstAgcdCharacter->m_pInHierarchy->pNodeInfo[RpHAnimIDGetIndex(pstAgcdCharacter->m_pInHierarchy, 9)].pFrame);
			}

			MYENGINE->m_csAgcmCharacter.StartAnimation(1, nAnimType, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE);
        }
		break;
	case rsF1:
		{
			AgpdItem	*pcsAgpdItem;

			if (m_nArmorPart >= 8)
				break;

			MYENGINE->m_csAgpmItem.RemoveItem(m_nArmorPart + 2);

			if (m_nPCType == 1)
			{

				pcsAgpdItem = MYENGINE->m_csAgpmItem.AddItem(m_nArmorPart + 2, 112 + m_nArmorPart);
			}

			else if (m_nPCType == 4)
			{
				pcsAgpdItem = MYENGINE->m_csAgpmItem.AddItem(m_nArmorPart + 2, 122 + m_nArmorPart);
			}

			MYENGINE->m_csAgpmItem.EquipItem(1, m_nArmorPart + 2);

			m_nArmorPart++;
		}
		break;

	case rsF2:
		{
			AgcdCharacter	*pstAgcdCharacter;
			AgpdCharacter	*pstAgpdCharacter;
			//AgpdCharacterTemplate	*pstAgpdCharacterTemplate;
			//AgpdItemADCharTemplate	*pstAgpdItemADCharTemplate;
			INT16					nIndex;

			m_nArmorPart = 0;

			m_nPCType++;
			if (m_nPCType > 4)
				m_nPCType = 1;

			MYENGINE->m_csAgcmEffect.EffectSetAllDataRemove();

			pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.GetCharacter(1);
			if (pstAgpdCharacter)
			{
				for (nIndex = 1; nIndex <= 10; ++nIndex)
				{
					MYENGINE->m_csAgpmItem.RemoveItem(nIndex);
				}
				pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);
				RwFrameRemoveChild(RpClumpGetFrame(pstAgcdCharacter->m_pClump));
				MYENGINE->m_csAgpmCharacter.RemoveCharacter(1);
			}

			pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.AddCharacter(1, m_nPCType, NULL);
			if (pstAgpdCharacter)
			{
				pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);

				RwFrameAddChild(m_pPlayerFrame, RpClumpGetFrame(pstAgcdCharacter->m_pClump));

//				MYENGINE->m_csAgpmItem.EquipItemDefault(1, 1);
//				TransformWeapon(1, m_nPCType);

				MovePlayer(RpClumpGetFrame(pstAgcdCharacter->m_pClump), 0, 0);
			}
		}
		break;

	case rsF3:
		{
			INT16	nIndex;

			for (nIndex = 12; nIndex < 24; ++nIndex)
			{
				MYENGINE->m_csAgcmCharacter.StartAnimation(nIndex, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE);
			}
		}
		break;

	case rsF5:
		{
			/*
			if (MYENGINE->m_csApmObject.StreamRead("Object\\AlefObjects.ini"))
			{
				ApdObject *pstObject;
				AgcdObject *pstAgcdObject;
				INT32	lIndex = 0;

				for (pstObject = MYENGINE->m_csApmObject.GetObjectSequence(&lIndex); pstObject; pstObject = MYENGINE->m_csApmObject.GetObjectSequence(&lIndex))
				{
					pstAgcdObject = MYENGINE->m_csAgcmObject.GetObjectData(pstObject);
				}
			}
			*/
		}
		break;

	case rsF8:
		{
			if (g_pEngine->m_pLightDirect)
			{
				RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

				RwFrameRotate( RpLightGetFrame(g_pEngine->m_pLightDirect) , & xaxis , 70.0f , rwCOMBINEREPLACE );
			}
		}
		break;

	case rsF9:
		{
			if (g_pEngine->m_pLightDirect)
			{
				RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

				RwFrameRotate( RpLightGetFrame(g_pEngine->m_pLightDirect) , & xaxis , 10.0f , rwCOMBINEPOSTCONCAT );
			}
		}
		break;

	case rsF10:
		{
			if (g_pEngine->m_pLightDirect)
			{
				RwV3d xaxis = { 1.0f , 0.0f , 0.0f };

				RwFrameRotate( RpLightGetFrame(g_pEngine->m_pLightDirect) , & xaxis , -10.0f , rwCOMBINEPOSTCONCAT );
			}
		}
		break;

	case rsF11:
		{
			RwReal	fScale	= 20.0f;
			RwV3d	scale	= {18.0f, 8.0f, 18.0f};
			RwV3d	pos;

			if (!m_pSkyBox)
			{
				m_pSkyBox = MYENGINE->m_csAgcmObject.LoadClump("skybox.dff");
#ifdef USE_DPVS
				RpDPVSWorldAddOccludingClump(g_pEngine->m_pWorld, m_pSkyBox);
#else
				RpWorldAddClump(g_pEngine->m_pWorld, m_pSkyBox);
#endif //USE_DPVS

				pos = *RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(m_pSkyBox)));
				pos.y = 800.0f;

				RwFrameScale(RpClumpGetFrame(m_pSkyBox), &scale, rwCOMBINEREPLACE);
				RwFrameTranslate(RpClumpGetFrame(m_pSkyBox), &pos, rwCOMBINEPOSTCONCAT);
			}
			else
			{
#ifdef USE_DPVS
				RpDPVSWorldRemoveClump(g_pEngine->m_pWorld, m_pSkyBox);
#else
				RpWorldRemoveClump(g_pEngine->m_pWorld, m_pSkyBox);
#endif //USE_DPVS
				RpClumpDestroy(m_pSkyBox);
				m_pSkyBox = NULL;
			}
		}
		break;

	case rsLSHIFT:
		{
			m_bLShift = TRUE;
		}
		break;

    case rsLCTRL:
		{
			if (m_bMove)
				MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_RUN, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

			m_bLCtrl = TRUE;
		}
		break;
	
    case rsRCTRL:
		{
			m_bRCtrl = TRUE;
		}
		break;

	case 'q':
		{
			CameraTurn(-30);
		}
		break;

	case 'e':
		{
			CameraTurn(30);
		}
		break;

	case '+':
		{
			m_fSpeed += 0.5;
		}
		break;

	case '-':
		{
			m_fSpeed -= 0.5;
		}
		break;

	case 'w':
		{
			if (m_bLCtrl)
				MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_RUN, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
			else
				MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

			m_fMoveDirection = 0;
			m_bMove = TRUE;
		}
		break;

	case 'a':
		{
			if (m_bLShift)
			{
				if (m_bLCtrl)
					MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_RUN, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
				else
					MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

				m_fMoveDirection = 1;
				m_bMove = TRUE;
			}
			else
			{
				m_fTurnDegree = 30;
				m_bTurn = TRUE;
			}
		}
		break;

	case 'd':
		{
			if (m_bLShift)
			{
				if (m_bLCtrl)
					MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_RUN, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
				else
					MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

				m_fMoveDirection = -1;
				m_bMove = TRUE;
			}
			else
			{
				m_fTurnDegree = -30;
				m_bTurn = TRUE;
			}
		}
		break;

	default:
		break;
	}
	return TRUE;
}

BOOL MainWindow::OnKeyUp		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
    case rsLCTRL:
		{
			if (m_bMove)
				MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_WALK, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);

			m_bLCtrl = FALSE;
		}
		break;
	
    case rsRCTRL:
		{
			m_bRCtrl = FALSE;
		}
		break;

	case rsLSHIFT:
		{
			m_bLShift = FALSE;
		}
		break;

	case 'w':
		{
			MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_NORMAL, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
			m_bMove = FALSE;
		}
		break;

	case 'a':
	case 'd':
		{
			if (m_bLShift)
			{
				MYENGINE->m_csAgcmCharacter.StartAnimation(1, AGCMCHAR_ANIM_TYPE_NORMAL, AGCMCHAR_ANIM_EQUIP_EMPTY, TRUE, TRUE);
				m_bMove = FALSE;
			}
			else
				m_bTurn = FALSE;
		}
		break;

	default:
		break;
	}

	return TRUE;
}
BOOL MainWindow::OnMouseMove	( RsMouseStatus *ms	)
{
	if( m_bLButtonDown)
	{
		if( m_fTilt + ms->delta.y * 0.1f > 89.0f )
		{
			m_fTilt = 89.0f;

		}
		else if( m_fTilt + ms->delta.y * 0.1f < -89.0f )
		{
			m_fTilt = -89.0f;
		}
		else
		{
			m_fTilt += ms->delta.y * 0.1f;
		}
	}

	m_PrevMouseStatus	= *ms	;
	return TRUE;
}

BOOL MainWindow::OnMouseWheel	( BOOL bForward		)
{
	RwFrame	*pstCameraFrame	= RwCameraGetFrame(g_pEngine->m_pCamera);
	RwV3d	stXAxis			= {1.0f, 0.0f, 0.0f};
	RwV3d	stZoom			= {0.0f, 0.0f, bForward ? -20.0f:20.0f};
	FLOAT	fPan = bForward ? m_fCameraPanUnit:-m_fCameraPanUnit;

	if (m_bLCtrl)
	{
		RwFrameRotate(pstCameraFrame, &stXAxis, fPan, rwCOMBINEPRECONCAT);
	}
	else if (m_bLShift)
	{
		RwFrameTranslate(pstCameraFrame, &stZoom, rwCOMBINEPRECONCAT);
	}
	else
	{
		m_fCameraPan += fPan;
		if (m_fCameraPan < 0 || m_fCameraPan >= 80)
			m_fCameraPan -= fPan;
		else
			RwFrameRotate(pstCameraFrame, &stXAxis, fPan, rwCOMBINEPOSTCONCAT);
	}

	return TRUE;
}


void MainWindow::HighlightRender(void)
{
    RwMatrix       *ltm = RwFrameGetLTM(RpAtomicGetFrame(m_pSelectedAtomic));
    RwIm3DVertex    vertices[8];
    RwInt32         i;
    RwImVertexIndex indices[24] = { 0, 1, 1, 3, 3, 2, 2, 0,
                                    4, 5, 5, 7, 7, 6, 6, 4,
                                    0, 4, 1, 5, 2, 6, 3, 7 };

    for (i = 0; i < 8; ++i)
    {
        RwIm3DVertexSetPos(vertices+i,
            i&1 ? PickBox.sup.x : PickBox.inf.x,
            i&2 ? PickBox.sup.y : PickBox.inf.y,
            i&4 ? PickBox.sup.z : PickBox.inf.z);
        RwIm3DVertexSetRGBA(vertices+i, 255, 0, 0, 255); 
    }

    if (RwIm3DTransform(vertices, 8, ltm, 0))
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 24);
        RwIm3DEnd();
    }
}

BOOL MainWindow::OnIdle			( UINT32 ulClockCount )
{
	UINT32	ulCurrTime = RsTimer();
	UINT32	ulDeltaTime = ulCurrTime - m_ulPrevTime;

	if( m_bMove | m_bTurn )
	{
		if (m_bLButtonDown)
		{
			RECT stClientRect;

			GetClientRect(PsGlobal.window, &stClientRect);
			m_fTurn += ((stClientRect.right - stClientRect.left) / 2.0f - m_PrevMouseStatus.pos.x) * (ulDeltaTime / 10000.0f);
		}
		else if (m_bTurn)
		{
			m_fTurn += m_fTurnDegree * (ulDeltaTime / 1000.0f);
		}

		MovePlayer(m_pPlayerFrame, m_fTurn, ulDeltaTime, m_bMove, m_bTurn, m_fMoveDirection);
	}
	if( m_bLCtrl )
	{
	}
	if( m_bRCtrl )
	{
	}

	m_ulPrevTime = ulCurrTime;

//	UpdateTextureShadow();

	return FALSE;
}

BOOL MainWindow::MovePlayer(RwFrame *pstCharacterFrame, FLOAT fTurn, UINT32 ulDeltaTime, BOOL bMove, BOOL bTurn, FLOAT fMoveDirection)
{
	RwV3d			stPosition		= *RwMatrixGetPos(RwFrameGetMatrix(pstCharacterFrame));
	RwV3d			*pstAt			= RwMatrixGetAt(RwFrameGetMatrix(pstCharacterFrame));
	RwV3d			stDelta;
	RpIntersection	stIntersection;
	const RwV3d		stYAxis			= {0, 1, 0};

	if (bMove)
	{
		if (fMoveDirection)
		{
			pstAt = RwMatrixGetRight(RwFrameGetMatrix(pstCharacterFrame));
			RwV3dScale(pstAt, pstAt, fMoveDirection);
		}

		RwV3dScale(&stDelta, pstAt, ulDeltaTime / (m_bLCtrl ? 5.0f/m_fSpeed:5.0f));
		RwV3dAdd(&stPosition, &stPosition, &stDelta);

		stIntersection.type = rpINTERSECTLINE;

		stIntersection.t.line.start = stPosition;
		stIntersection.t.line.start.y += 500;

		stIntersection.t.line.end = stPosition;
		stIntersection.t.line.end.y -= 500;

		RpCollisionWorldForAllIntersections(g_pEngine->m_pWorld, &stIntersection, CollisionWorldFirstCallback, &stPosition);
	}

	RwFrameRotate(pstCharacterFrame, &stYAxis, fTurn, rwCOMBINEREPLACE);
	RwFrameTranslate(pstCharacterFrame, &stPosition, rwCOMBINEPOSTCONCAT);

	if (pstCharacterFrame == m_pPlayerFrame && m_pSkyBox)
	{
		if (m_pSkyBox)
		{
			RwReal	fScale = 20.0f;
			RwV3d	scale = {18.0f, 10.0f, 18.0f};

			stPosition.y = 7000.0f;
			RwFrameScale(RpClumpGetFrame(m_pSkyBox), &scale, rwCOMBINEREPLACE);
			RwFrameTranslate(RpClumpGetFrame(m_pSkyBox), &stPosition, rwCOMBINEPOSTCONCAT);
		}
	}

	return TRUE;
}

RpCollisionTriangle *MainWindow::CollisionWorldFirstCallback(RpIntersection *pstIntersection,
							RpWorldSector *pstSector, 
							RpCollisionTriangle *pstCollTriangle,
							RwReal fDistance, 
							void *pvData)
{
	RwV3d	*pstPosition = (RwV3d *) pvData;
	
	pstPosition->x = pstIntersection->t.line.start.x + (pstIntersection->t.line.end.x - pstIntersection->t.line.start.x) * fDistance;
	pstPosition->y = pstIntersection->t.line.start.y + (pstIntersection->t.line.end.y - pstIntersection->t.line.start.y) * fDistance;
	pstPosition->z = pstIntersection->t.line.start.z + (pstIntersection->t.line.end.z - pstIntersection->t.line.start.z) * fDistance;

	return NULL;
}


void MainWindow::AtomicAddToSortedList(SortAtomic *entry)
{
	SortAtomic *cur;

    /*
     * If current list is empty, start a new list...
     */
    if( AlphaSortedAtomics == NULL )
    {
       AlphaSortedAtomics = entry;

       return;
    }

    /*
     * ...otherwise find correct place in list...
     */
    for(cur=AlphaSortedAtomics; cur; cur=cur->next)
    {
        if( entry->depth >= cur->depth )
        {
            /*
             * Add to start of list... 
             */
            entry->next = AlphaSortedAtomics;
            AlphaSortedAtomics = entry;
            
            break;
        }

        if( cur->next == NULL )
        {
           /*
            * Add to end of list... 
            */
           cur->next = entry;

           break;
        }

        if( entry->depth < cur->depth && entry->depth >= cur->next->depth )
        {
            /*
             * Insert into list...
             */
            entry->next = cur->next;
            cur->next = entry;

            break;
        }
    }

    return;
}

RpAtomic* MainWindow::AtomicSetRenderCallback(RpAtomic *atomic, void *data)
{
	if( DefaultAtomicRenderCallback == NULL )
          DefaultAtomicRenderCallback = RpAtomicGetRenderCallBack(atomic);
    
    RpAtomicSetRenderCallBack(atomic, AlphaSortedAtomicRenderCallback);
        
    return atomic;
}

RpAtomic* MainWindow::AlphaSortedAtomicRenderCallback(RpAtomic *atomic)
{
//	AlphaAtomic *entry = AlphaAtomics;
  //  RwBool hasAlpha = FALSE;
 //   RwChar id = '\0';

    /*
     * Is this atomic a transparent one?...
     */
  /*  while( entry )
    {
        if( atomic == entry->atomic )
        {
            id = entry->id;

            hasAlpha = TRUE;

            break;
        }

        entry = entry->next;
    }	*/

    
        /*
         * Add atomic to deferred list of alpha atomics...
         */
        SortAtomic *entry;

        entry = (SortAtomic *)RwMalloc(sizeof(SortAtomic));            

        entry->atomic = atomic;
        entry->depth = AtomicGetCameraDistance(atomic);
        entry->next = NULL;

        AtomicAddToSortedList(entry);
    

    return atomic;
	
}

RwReal MainWindow::AtomicGetCameraDistance(RpAtomic *atomic)
{
    RwFrame *frame;
    RwV3d *camPos, atomicPos, temp;
    RwSphere *atomicBSphere;
    RwReal distance2;

    /*
     * Atomic's bounding-sphere world-space position...
     */
    atomicBSphere = RpAtomicGetBoundingSphere(atomic);
    RwV3dTransformPoints(&atomicPos, &atomicBSphere->center, 
        1, RwFrameGetLTM(RpAtomicGetFrame(atomic)));

    /*
     * ...camera position...
     */
    frame = RwCameraGetFrame(g_pEngine->m_pCamera);
    camPos = RwMatrixGetPos(RwFrameGetLTM(frame));

    /*
     * ...vector from camera to atomic...
     */
    RwV3dSub(&temp, &atomicPos, camPos);

    /*
     * Squared distance...
     */
    distance2 = RwV3dDotProduct(&temp, &temp);

    return distance2;
}

void MainWindow::SetRenderCallBackForAllAtomic()
{
	RpWorldForAllWorldSectors( g_pEngine->m_pWorld, WorldSectorSetRenderCallBack, NULL );
}

RpWorldSector* MainWindow::WorldSectorSetRenderCallBack(RpWorldSector *sector, void *data)
{
	RpWorldSectorForAllAtomics( sector, AtomicSetRenderCallback, NULL );

	return sector;
}

void MainWindow::RenderAlphaSortedAtomics()
{
	while( AlphaSortedAtomics )
    {
        SortAtomic *temp;
        
        /*
         * Render atomic...
         */
        DefaultAtomicRenderCallback(AlphaSortedAtomics->atomic);

        /*
         * Remove the atomic from the list...
         */
        temp = AlphaSortedAtomics;
        AlphaSortedAtomics = AlphaSortedAtomics->next;
        RwFree(temp);
    }

    return;
}

void MainWindow::DestroyAlphaAtomicsList(void)
{
    SortAtomic *entry = AlphaSortedAtomics;
    SortAtomic *temp;

    while( entry )
    {
        temp = entry->next;

        RwFree(entry);

        entry = temp;
    }

    return;
}

BOOL MainWindow::InitCharacters()
{
	AgcdCharacter	*pstAgcdCharacter;
	AgpdCharacter	*pstAgpdCharacter;
	AgpdCharacterTemplate	*pstAgpdCharacterTemplate;
	AgpdItemADCharTemplate	*pstAgpdItemADCharTemplate;
	INT16					nIndex;
	RwV3d					yAxis = {0, 1, 0};
	RwV3d					pos = {-198991,811,-218542};

	m_nPCType = 1;
	pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.AddCharacter(1, m_nPCType, NULL);
	if (pstAgpdCharacter)
	{
        RwUInt32            lightIndexArray[] = { 0 };

		MYENGINE->m_csAgpmCharacter.UpdateInit(pstAgpdCharacter);

		pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);

		RwFrameAddChild(m_pPlayerFrame, RpClumpGetFrame(pstAgcdCharacter->m_pClump));

//		MYENGINE->m_csAgpmItem.EquipItemDefault(1, 1);
//		TransformWeapon(1, m_nPCType);

		RwFrameRotate(m_pPlayerFrame, &yAxis, -90, rwCOMBINEREPLACE);
		RwFrameTranslate(m_pPlayerFrame, &pos, rwCOMBINEPOSTCONCAT);

		MovePlayer(RpClumpGetFrame(pstAgcdCharacter->m_pClump), 0, 0);

		/*
		RwV3d					sun = {1000, 1000, -1000};
		CreateTextureShadow(pstAgcdCharacter->m_pClump, g_pEngine->m_pWorld, 1, &sun, g_pEngine->m_pLightAmbient, g_pEngine->m_pLightDirect);
		SetTextureShadowLightsToUse(1, lightIndexArray, 1);
		*/
	}

	for (nIndex = 0; nIndex < 23; ++nIndex)
	{
		pstAgpdCharacterTemplate = MYENGINE->m_csAgpmCharacter.GetCharacterTemplate(nIndex + 1);
		if (pstAgpdCharacterTemplate)
		{
			MYENGINE->m_csAgpmCharacter.RemoveCharacter(nIndex + 2);

			pstAgpdItemADCharTemplate = MYENGINE->m_csAgpmItem.GetADCharacterTemplate(pstAgpdCharacterTemplate);
			pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.AddCharacter(nIndex + 2, nIndex + 1, NULL);
			if (pstAgpdCharacter)
			{
				MYENGINE->m_csAgpmCharacter.UpdateInit(pstAgpdCharacter);

				pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);

//				MYENGINE->m_csAgpmItem.EquipItemDefault(nIndex + 2, nIndex * 10 + 11);
//				TransformWeapon(nIndex + 2, nIndex + 1);

				RwFrameTranslate(RpClumpGetFrame(pstAgcdCharacter->m_pClump), g_stCharacterPos + nIndex, rwCOMBINEPOSTCONCAT);
				MovePlayer(RpClumpGetFrame(pstAgcdCharacter->m_pClump), 0, 0);
				RwFrameRotate(RpClumpGetFrame(pstAgcdCharacter->m_pClump), &yAxis, g_fDegree[nIndex], rwCOMBINEPRECONCAT);
			}
		}
	}

	return TRUE;
}

void MainWindow::CameraTurn(FLOAT fDegree)
{
	RwV3d	yAxis = {0, 1, 0};

	RwFrameRotate(RwCameraGetFrame(g_pEngine->m_pCamera), &yAxis, fDegree, rwCOMBINEPOSTCONCAT);
}

static RpAtomic* TransItemCB(RpAtomic* atomic, PVOID pData)
{
	RpGeometry      *pAtomicGeom          = RpAtomicGetGeometry(atomic);
	RwFrame			*frame				  = RpAtomicGetFrame(atomic);
	RwInt32          nUserDataArrayCount  = RpGeometryGetUserDataArrayCount(pAtomicGeom);
//	RwInt32          nUserDataArrayCount  = RwFrameGetUserDataArrayCount(frame);
	RpUserDataArray *pUserDataArray;
	RwChar*          strName;
	RwInt16          nPartId;
	RwV3d		xAxis = {1, 0, 0};
	RwV3d		yAxis = {0, 1, 0};
	RwV3d		zAxis = {0, 0, 1};
	RwV3d		pos;
	INT32		lType = (INT32) pData;

	for(RwInt32 nIndex = 0; nIndex < nUserDataArrayCount; ++nIndex)
	{
		pUserDataArray = RpGeometryGetUserDataArray(pAtomicGeom, nIndex);
//		pUserDataArray = RwFrameGetUserDataArray(frame, nIndex);
		strName        = RpUserDataArrayGetName(pUserDataArray);
		
		if(strcmp(strName, "PartID") == 0)
		{
			nPartId = RpUserDataArrayGetInt(pUserDataArray, 0);
		}
	}

	if (nPartId == AGPMITEM_PART_HAND_RIGHT)
//	if (nPartId == 1)
	{
		switch (lType)
		{
		case 1:
			{
				pos.x = 10;
				pos.y = 5;
				pos.z = 0;

				RwFrameRotate(frame, &yAxis, 180, rwCOMBINEREPLACE);
				RwFrameRotate(frame, &xAxis, 90, rwCOMBINEPOSTCONCAT);
				RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
			}
			break;

		case 2:
			{
				pos.x = 10;
				pos.y = 5;
				pos.z = -5;

				RwFrameRotate(frame, &yAxis, 180, rwCOMBINEREPLACE);
				RwFrameRotate(frame, &xAxis, 90, rwCOMBINEPOSTCONCAT);
				RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

				MYENGINE->m_csAgcmEffect.UseEffectSet(826001, NULL, lCurrent, 1, frame);
			}
			break;

		case 4:
			{
				pos.x = 10;
				pos.y = 5;
				pos.z = 0;

				RwFrameRotate(frame, &zAxis, 180, rwCOMBINEREPLACE);
				RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
			}
		}
	}
	else if (nPartId == AGPMITEM_PART_HAND_LEFT)
	{
		switch (lType)
		{
		case 2:
			{
				pos.x = 10;
				pos.y = 0;
				pos.z = 0;

				RwFrameRotate(frame, &xAxis, 90, rwCOMBINEREPLACE);
				RwFrameRotate(frame, &yAxis, 180, rwCOMBINEPOSTCONCAT);
				RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
			}
			break;

		case 3:
			{
				pos.x = 10;
				pos.y = 5;
				pos.z = 0;

				RwFrameRotate(frame, &yAxis, 180, rwCOMBINEREPLACE);
				RwFrameRotate(frame, &zAxis, 90, rwCOMBINEPOSTCONCAT);
				RwFrameRotate(frame, &xAxis, 90, rwCOMBINEPOSTCONCAT);
				RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);
			}
			break;
		}
	}

	return atomic;
}


BOOL MainWindow::TransformWeapon(INT32 lCID, INT32 lType)
{
	RwFrame	*frame;
	AgpdCharacter	*pstAgpdCharacter;
	AgcdCharacter	*pstAgcdCharacter;

	pstAgpdCharacter = MYENGINE->m_csAgpmCharacter.GetCharacter(lCID);
	pstAgcdCharacter = MYENGINE->m_csAgcmCharacter.GetCharacterData(pstAgpdCharacter);
	frame = RpClumpGetFrame(pstAgcdCharacter->m_pClump);

	lCurrent = m_ulPrevTime;

	RpClumpForAllAtomics(pstAgcdCharacter->m_pClump, TransItemCB, (void *) lType);

	return TRUE;
}
