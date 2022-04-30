// AgcuCamMode.cpp
// -----------------------------------------------------------------------------
//                             _____                 __  __           _                           
//     /\                     / ____|               |  \/  |         | |                          
//    /  \    __ _  ___ _   _| |      __ _ _ __ ___ | \  / | ___   __| | ___      ___ _ __  _ __  
//   / /\ \  / _` |/ __| | | | |     / _` | '_ ` _ \| |\/| |/ _ \ / _` |/ _ \    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| |_| | |____| (_| | | | | | | |  | | (_) | (_| |  __/ _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|\_____|\__,_|_| |_| |_|_|  |_|\___/ \__,_|\___|(_) \___| .__/| .__/ 
//            __/ |                                                                  | |   | |    
//           |___/                                                                   |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 01/29/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "AgcuCamMode.h"
#include "AuXmlParser.h"

extern RwReal	g_offsetMin	;
extern RwReal	g_offsetMax	;
extern RwReal	g_zoomcoef	;

inline void	ADD_CAMERA_SETTING( INT32 tid , FLOAT characterheight , FLOAT faceheight , 
							   FLOAT minlen1 , FLOAT maxlen1 , FLOAT minPitch1 , FLOAT maxPitch1 , // Free Camera
							   FLOAT minlen2 = 1100.f, FLOAT maxlen2 = 2750.f, FLOAT minPitch2 = 79.f, FLOAT maxPitch2 = 89.f, // Top
							   FLOAT minlen3 = 200.f, FLOAT maxlen3 = 1571.f, FLOAT minPitch3 = 35.f , FLOAT maxPitch3 = 65.f, // Quater
							   FLOAT minlen4 = 0.1f, FLOAT maxlen4 = 0.1f, FLOAT minPitch4 = -89.f, FLOAT maxPitch4 = 89.f) // FPS
{
	AgcuCamMode::CameraInfoSet	stInfo;
	stInfo.nCharacterTemplateID			= tid		;
	stInfo.m_fCharacterHeight			= characterheight		;	// Default를 사용함.
	stInfo.m_fCharacterFaceHeight		= faceheight		;	// Default를 사용함.

	stInfo.m_stInfo[ AgcuCamMode::eMode_Free ].m_fMinLen		= minlen1	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Free ].m_fMaxLen		= maxlen1	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Free ].m_fMinPitch		= minPitch1	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Free ].m_fMaxPitch		= maxPitch1	;

	stInfo.m_stInfo[ AgcuCamMode::eMode_Top ].m_fMinLen			= minlen2	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Top ].m_fMaxLen			= maxlen2	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Top ].m_fMinPitch		= minPitch2	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Top ].m_fMaxPitch		= maxPitch2	;

	stInfo.m_stInfo[ AgcuCamMode::eMode_Quart ].m_fMinLen		= minlen3	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Quart ].m_fMaxLen		= maxlen3	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Quart ].m_fMinPitch		= minPitch3	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_Quart ].m_fMaxPitch		= maxPitch3	;

	stInfo.m_stInfo[ AgcuCamMode::eMode_1stPerson ].m_fMinLen	= minlen4	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_1stPerson ].m_fMaxLen	= maxlen4	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_1stPerson ].m_fMinPitch	= minPitch4	;
	stInfo.m_stInfo[ AgcuCamMode::eMode_1stPerson ].m_fMaxPitch	= maxPitch4	;

	AgcuCamMode::bGetInst().AddCameraInfo( &stInfo );
}


// -----------------------------------------------------------------------------
AgcuCamMode::AgcuCamMode()
: m_state(eState_End)
, m_currmode(eMode_Free)
, m_accumOnePlusSin(90.f, 270.f, 0.f, 0.5f)
{

	ReadXMLData("ini\\CamSetting.xml");

	m_vEyeSubAt.x	= 
		m_vEyeSubAt.y	= 
		m_vEyeSubAt.z	= 0.f;
	m_uvTrans.x		= 
		m_uvTrans.y		= 
		m_uvTrans.z		= 0.f;
	m_lenTrans		= 0.f;
}

// -----------------------------------------------------------------------------
AgcuCamMode::~AgcuCamMode()
{
}

// -----------------------------------------------------------------------------
AgcuCamMode& AgcuCamMode::bGetInst()
{
	static AgcuCamMode inst;
	return inst;
}
// -----------------------------------------------------------------------------
void AgcuCamMode::bChageMode(AgcmCamera2& cam2, eMode mode)
{
	if( m_state == eState_Changing ||
		m_currmode == mode			)
		return;

	if( mode < eMode_Free ||
		mode > eMode_1stPerson )
	{
		//err
		return;
	}

	if( m_currmode == eMode_1stPerson )
	{
		cam2.bOptFlagOn( AgcmCamera2::e_flag_use_dampingforce );
		//main charac -> alpha : 255
		cam2.bTransparentMainCharac();
	}
	else if( mode == eMode_1stPerson )
	{
		cam2.bOptFlagOff( AgcmCamera2::e_flag_use_dampingforce );
		//main charac -> alpha : 0
		cam2.bTransparentMainCharac();
	}

	//save current state
	m_amode[m_currmode].m_fSavLen	= cam2.m_fDesiredLen;
	m_amode[m_currmode].m_fSavPitch	= cam2.m_fDesiredPitch;

	//lock and unlock camera's act
	cam2.bUnlockAct((AgcmCamera2::e_upt_mask)m_amode[m_currmode].m_ulLockFlag);
	cam2.bLockAct((AgcmCamera2::e_upt_mask)m_amode[mode].m_ulLockFlag);

	//set current mode and state
	m_currmode = mode;
	m_state = eState_Changing;

	//set camra info
	cam2.m_fMaxLen	= m_amode[m_currmode].m_fMaxLen;
	cam2.m_fMinLen	= m_amode[m_currmode].m_fMinLen;
	cam2.m_fMaxPitch= m_amode[m_currmode].m_fMaxPitch;
	cam2.m_fMinPitch= m_amode[m_currmode].m_fMinPitch;

	cam2.bSetIdleType( AgcmCamera2::e_idle_chagemode );

	//eyesubat
	RwV3dSub( &m_vEyeSubAt, cam2.bGetPtrEye(), &cam2.m_v3dLookat );
	//trans	
	RwReal	xzlen	= m_amode[m_currmode].m_fDftLen 
		* cosf( DEF_D2R(m_amode[m_currmode].m_fDftPitch ) );
	RwReal	ylen	= m_amode[m_currmode].m_fDftLen 
		* sinf( DEF_D2R(m_amode[m_currmode].m_fDftPitch ) );

	D3DXVECTOR2 v2xz( m_vEyeSubAt.x, m_vEyeSubAt.z );
	D3DXVec2Normalize( &v2xz,&v2xz );
	D3DXVec2Scale( &v2xz,&v2xz,xzlen );
	RwV3d	vdst	= {v2xz.x, ylen, v2xz.y};
	RwV3dSub( &m_uvTrans, &vdst, &m_vEyeSubAt );
	m_lenTrans = RwV3dLength ( &m_uvTrans );
	RwV3dNormalize ( &m_uvTrans, &m_uvTrans );

	new(&m_accumOnePlusSin) AgcuAccmOnePlusSin(90.f, 270.f, m_lenTrans, 0.5f);
}
// -----------------------------------------------------------------------------
const AgcuCamMode::stModeData& AgcuCamMode::bGetCurrModeData(void)
{
	return m_amode[m_currmode];
}
// -----------------------------------------------------------------------------
void AgcuCamMode::bOnIdle(AgcmCamera2& cam2, float fElapsed)
{
	float	len		= 0.f;
	bool	bchk	= m_accumOnePlusSin.bOnIdle( &len, fElapsed );
	if( bchk )
	{
		m_state = eState_End;
		cam2.bSetIdleType( AgcmCamera2::e_idle_default );
	}

	//calculate eye's position
	RwV3d	eye;
	RwV3dAdd( &eye, &cam2.m_v3dLookat, &m_vEyeSubAt );
	RwV3d	trans;
	RwV3dScale( &trans, &m_uvTrans, len );
	RwV3dAdd( &eye, &eye, &trans );

	cam2.bSetCamFrm( cam2.m_v3dLookat, eye );
}
// -----------------------------------------------------------------------------
// AgcuCamMode.cpp - End of file
// -----------------------------------------------------------------------------

void	AgcuCamMode::AddCameraInfo( CameraInfoSet * pSet )
{
	m_vecCameraInfo.push_back( *pSet );
}

void	AgcuCamMode::SetCameraInfo( AgcmCamera2 * pAgcmCamera2 , AgpdCharacter	*pstAgpdCharacter )
{
	// TID 준비~
	INT32	nTID = pstAgpdCharacter->m_pcsCharacterTemplate->m_lID;
	if( pAgcmCamera2->m_pAgpmCharacter->IsRideOn( pstAgpdCharacter ) )	// 탈것에 타고 있는지 확인.
	{
		nTID = pAgcmCamera2->m_pAgpmCharacter->GetVehicleTID(pstAgpdCharacter);
	}

	CameraInfoSet	*pstInfoSet = NULL;

	CameraInfoSet	stDefault;
	stDefault.m_fCharacterHeight			= 0.0f		;	// Default를 사용함.
	stDefault.m_fCharacterFaceHeight		= 0.0f		;	// Default를 사용함.

	for(int i = (int)eMode_Free ; i < (int)eMode_Num ; ++i)
	{
		stDefault.m_stInfo[ i ].m_fMinLen		= m_amode[i].m_fMinLen;
		stDefault.m_stInfo[ i ].m_fMaxLen		= m_amode[i].m_fMaxLen;
		stDefault.m_stInfo[ i ].m_fMinPitch		= m_amode[i].m_fMinPitch;
		stDefault.m_stInfo[ i ].m_fMaxPitch		= m_amode[i].m_fMaxPitch;
	}

	for( vector< CameraInfoSet >::iterator	iTer = m_vecCameraInfo.begin();
		iTer != m_vecCameraInfo.end();
		iTer ++ )
	{
		if( ( *iTer ).nCharacterTemplateID == nTID )
		{
			pstInfoSet = & ( *iTer );
			break;
		}
	}

	if( NULL == pstInfoSet )
	{
		pstInfoSet = & stDefault;
	}

	if( pstInfoSet->m_fCharacterHeight		== 0.0f ||
		pstInfoSet->m_fCharacterFaceHeight	== 0.0f )
	{
		// 디폴트 값 계산.
		AgpdCharacterTemplate 	* pTemplate		= pAgcmCamera2->m_pAgpmCharacter->GetCharacterTemplate( nTID );
		AgcdCharacterTemplate 	* pCTemplate	= pTemplate ? pAgcmCamera2->m_pAgcmCharacter->GetTemplateData( pTemplate) : NULL ;

		pstInfoSet->m_fCharacterHeight		= 100.0f;
		pstInfoSet->m_fCharacterFaceHeight	= 160.0f;

		if( pTemplate && pCTemplate )
		{
			RwReal	fRadius;

			fRadius = pCTemplate->m_stBSphere.radius * pCTemplate->m_fScale;
			pstInfoSet->m_fCharacterFaceHeight = 160.f*fRadius/111.37224f;
		}
	}

	// 드디어 값 설정.
	g_offsetMin = pAgcmCamera2->m_fCharacterHeight = pstInfoSet->m_fCharacterHeight;
	g_offsetMax	= pstInfoSet->m_fCharacterFaceHeight;

	// 	for( int i = 0 ; i < eMode_Num ; i ++ )
	// 	{
	// 		m_amode[ i ].m_fMinLen		= pstInfoSet->m_stInfo[ i ].m_fMinLen	;
	// 		m_amode[ i ].m_fMaxLen		= pstInfoSet->m_stInfo[ i ].m_fMaxLen	;
	// 		m_amode[ i ].m_fMinPitch	= pstInfoSet->m_stInfo[ i ].m_fMinPitch	;
	// 		m_amode[ i ].m_fMaxPitch	= pstInfoSet->m_stInfo[ i ].m_fMaxPitch	;
	// 	}

	// 아싸~

	pAgcmCamera2->m_fMinLen		= m_amode[ m_currmode ].m_fMinLen	;
	pAgcmCamera2->m_fMaxLen		= m_amode[ m_currmode ].m_fMaxLen	;
	pAgcmCamera2->m_fMinPitch	= m_amode[ m_currmode ].m_fMinPitch	;
	pAgcmCamera2->m_fMaxPitch	= m_amode[ m_currmode ].m_fMaxPitch	;

	// g_zoomcoef	= 6.f; // 디폴트
	g_zoomcoef = ( pAgcmCamera2->m_fMaxLen - pAgcmCamera2->m_fMinLen ) / 1000.0f * 6.0f;

	AgpdCharacter * pstCharacter = pAgcmCamera2->m_pAgcmCharacter->GetSelfCharacter();

	if( pstCharacter )
	{
		AgcdCharacter	*pstAgcdCharacter	= pAgcmCamera2->m_pAgcmCharacter->GetCharacterData(pstCharacter);

		RwV3d	vOffset = {0.f, 100.0f , 0.f};
		vOffset.y = pAgcmCamera2->m_fCharacterHeight;
		pAgcmCamera2->bSetTargetFrame( RpClumpGetFrame(pstAgcdCharacter->m_pClump), vOffset );
	}
}

BOOL AgcuCamMode::ReadXMLData( CONST string& strFileName )
{
	AuXmlParser	Parser;

	if(!Parser.LoadXMLFile(strFileName))
		return FALSE;

	AuXmlNode* pNode = Parser.FirstChild("CamSetting");

	if(pNode)
	{
		AuXmlNode* pSettingNode = pNode->FirstChild("Default");

		if(pSettingNode)
		{
			AuXmlElement*	pElement = pSettingNode->FirstChildElement("Mode");
			while(pElement)
			{
				eMode	_eMode;

				AuXmlElement* pDistElement	= NULL;
				AuXmlElement* pPitchElement	= NULL;

				unsigned int ulLockFlag;
				double	fDftLen, fMinLen, fMaxLen;
				double	fDftPitch, fMinPitch, fMaxPitch;

				pElement->Attribute("Index", (int*)&_eMode);
				pElement->Attribute("MaskFlag", (int*)&ulLockFlag);

				pDistElement	= pElement->FirstChildElement("Distance");
				pPitchElement	= pElement->FirstChildElement("Pitch");

				if(pDistElement && pPitchElement)
				{
					pDistElement->Attribute("Default", &fDftLen);
					pDistElement->Attribute("Min", &fMinLen);
					pDistElement->Attribute("Max", &fMaxLen);

					pPitchElement->Attribute("Default", &fDftPitch);
					pPitchElement->Attribute("Min", &fMinPitch);
					pPitchElement->Attribute("Max", &fMaxPitch);

					m_amode[_eMode].m_fSavLen = m_amode[_eMode].m_fDftLen = (float)fDftLen;
					m_amode[_eMode].m_fMinLen = (float)fMinLen;
					m_amode[_eMode].m_fMaxLen = (float)fMaxLen;

					m_amode[_eMode].m_fSavPitch = m_amode[_eMode].m_fDftPitch = (float)fDftPitch;
					m_amode[_eMode].m_fMinPitch = (float)fMinPitch;
					m_amode[_eMode].m_fMaxPitch = (float)fMaxPitch;

					m_amode[_eMode].m_ulLockFlag = ulLockFlag;

					pElement = pElement->NextSiblingElement("Mode");
				}
			}
		}

		pSettingNode = pNode->FirstChild("Specific");
		while(pSettingNode)
		{
			AuXmlElement* pCharElement	= NULL;
			AuXmlElement* pDistElement	= NULL;
			AuXmlElement* pPitchElement	= NULL;

			int iTID = 0;
			double	fHeight, fFace;
			double	fMinLen, fMaxLen;
			double	fMinPitch, fMaxPitch;

			pCharElement = pSettingNode->FirstChildElement("Character");
			pDistElement = pSettingNode->FirstChildElement("Distance");
			pPitchElement = pSettingNode->FirstChildElement("Pitch");

			if(pCharElement && pDistElement && pPitchElement)
			{
				pCharElement->Attribute("TID", &iTID);
				pCharElement->Attribute("Height", &fHeight);
				pCharElement->Attribute("Face", &fFace);

				pDistElement->Attribute("Min", &fMinLen);
				pDistElement->Attribute("Max", &fMaxLen);

				pPitchElement->Attribute("Min", &fMinPitch);
				pPitchElement->Attribute("Max", &fMaxPitch);

				/*ADD_CAMERA_SETTING(
					iTID, (float)fHeight, (float)fFace,
					(float)fMinLen, (float)fMaxLen,
					(float)fMinPitch, (float)fMaxPitch);*/

				pSettingNode = pSettingNode->NextSibling("Specific");
			}
		}
	}
	return TRUE;
}

void AgcuCamMode::ReloadCamSetting()
{
	m_vecCameraInfo.clear();
	ReadXMLData("ini\\CamSetting.xml");
}
