// AgcuEffIniMng.cpp: implementation of the AgcuEffIniMng class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEffIniMng.h"

#include "AgcuEffPath.h"
#include "AgcuEffUtil.h"
#include "AgcuEffAnimUtil.h"

#include "AgcdEffSet.h"

#include "AgcdEffGlobal.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const	RwChar*	EFFINIFILEFORM	= "%s%d_eff.ini";//path,id
const	RwChar*	EFFINIVER	= "1.0";

RwChar* SECT(eEffSect_Name eSect, int index=-1);
RwChar*	KEY(eEffKey_Name eKey, int index=-1);
RwChar* VALUE(eEffKey_Name eKey, ... );

const char* EFF_SECT[eEffSect_Num]	= {
	"EffSet"				, //eEffSect_Set		

	"AgcdEffBoard"			, //eEffSect_effbase_board		
	"AgcdEffLight"			, //eEffSect_effbase_light		
	"AgcdEffMFrm"			, //eEffSect_effbase_mfrm		
	"AgcdEffObj"			, //eEffSect_effbase_obj			
	"AgcdEffSound"			, //eEffSect_effbase_sound		
	"AgcdEffTail"			, //eEffSect_effbase_tail		
	"AgcdEffPSyst"			, //eEffSect_effbase_psyst		
	"AgcdEffPSyst_SBH"		, //eEffSect_effbase_psyst_SBH

	"AgcdEffTerrainBoard"	, //eEffSect_effbase_terrainb
	"AgcdEffPostFX"			, //eEffSect_effbase_postfx
};

const char* EFF_KEY[eEffKey_Num][e_EffKeyAndForm_Num]	= {
	"id"					, "%d", 						//eEffKey_set_id		
	"ini_ver"				, "%s",							//eEffKey_set_iniver			
	"title"					, "%s", 						//eEffKey_set_title			
	"life"					, "%d", 						//eEffKey_set_life
	"loopOption"			, "%d", 						//eEffKey_set_loopOpt
	"flag"					, "%x", 						//eEffKey_set_flag		

	"missileInfo"			, "%d:%f:%f:%f",				//eEffKey_set_missileInfo

	"sphere"				, "%f:%f:%f:%f",				//eEffKey_set_sphere			
	"box"					, "%f:%f:%f:%f:%f:%f",			//eEffKey_set_box				
	"varsize"				, "%d:%d:%d",					//eEffKey_set_varsize			
	"efftex"				, "%s",							//eEffKey_set_efftex			
	"effmask"				, "%s",							//eEffKey_set_effmask		
	"depnd"					, "%d:%d:%x",					//eEffKey_set_depnd			

	"AgcdEffBase_title"		, "%s",							//eEffKey_effbase_title
	"AgcdEffBase"			, "%d:%d:%d:%d:%x:%d",			//eEffKey_effbase_base		
	"AgcdEffRenderBase"		, "%d:%d:%f:%f:%f:%f:%f:%f",	//eEffKey_effbase_renderbase	
	
	//light
	"LightType"				, "%d",							//eEffKey_effLight_type
	"pos"					, "%f:%f:%f",					//eEffKey_effLight_pos
	"rotAngle"				, "%f:%f:%f",					//eEffKey_effLight_angle
	"conAngle"				, "%f",							//eEffKey_effLight_conAngle
	"surfaceProp"			, "%f:%f:%f",					//eEffKey_effLight_surfProp
	"materialColr"			, "%d:%d:%d:%d",				//eEffKey_effLight_matColr

	//moving frame
	"MFrmType"				, "%d", 						//eEffKey_effMFrm_type
	"ShakeFrm_Axis"			, "%d", 						//eEffKey_effMFrm_shake_axis
	"ShakeFrm_Whose"		, "%d", 						//eEffKey_effMFrm_shake_whose
	"ShakeFrm_Amplitude"	, "%f", 						//eEffKey_effMFrm_shake_Amplitude
	"ShakeFrm_Duration"		, "%d", 						//eEffKey_effMFrm_shake_duration
	"ShakeFrm_Cycle"		, "%f", 						//eEffKey_effMFrm_shake_cycle

	//object
	"ClumpFile"				, "%s",							//eEffKey_effObj_clumpFile
	"PreLitColr"			, "%d:%d:%d:%d",				//eEffKey_effObj_Prelit

	//sound
	"SoundType"				, "%d", 						//eEffKey_effSound_type
	"File"					, "%s", 						//eEffKey_effSound_file
	"MonoFile"				, "%s", 						//eEffKey_effSound_monofile
	"loopCnt"				, "%d", 						//eEffKey_effSound_loopCnt
	"volume"				, "%f", 						//eEffKey_effSound_volume
						
	//tail
	"capacity"				, "%d", 						//eEffKey_effTail_capacity
	"insertTimeGap"			, "%d", 						//eEffKey_effTail_timegap
	"PointLife"				, "%d", 						//eEffKey_effTail_pointLife
	"height"				, "%f:%f",						//eEffKey_effTail_height

	//particle system
	"capacity"				, "%d", 						//eEffKey_effPSys_capacity
	"shootDelay"			, "%d", 						//eEffKey_effPSys_shootDelay
	"clumpFile"				, "%s", 						//eEffKey_effPSys_clumpFile
	//emiter
	"emt_power"				, "%f:%f", 						//eEffKey_effPSys_emt_power		
	"emt_gunLength"			, "%f:%f", 						//eEffKey_effPSys_emt_gunLen		
	"emt_oneShootNum"		, "%d:%d", 						//eEffKey_effPSys_emt_oneshootnum	
	"emt_omegaYawPitch"		, "%f:%f", 						//eEffKey_effPSys_emt_omegaYawPitch
	"emt_yawMinMax"			, "%f:%f", 						//eEffKey_effPSys_emt_yawMinMax	
	"emt_pitchMinMax"		, "%f:%f", 						//eEffKey_effPSys_emt_pitchMinMax	
	"emt_initDirection"		, "%f:%f:%f",					//eEffKey_effPSys_emt_initDir		
	"emt_conAngle"			, "%f", 						//eEffKey_effPSys_emt_conAngle	
	"emt_flag"				, "%x", 						//eEffKey_effPSys_emt_flag		
	"emt_ePGroup"			, "%d", 						//eEffKey_effPSys_emt_ePGroup
	"emt_PG_box"			, "%f:%f:%f",					//eEffKey_effPSys_emt_pgBox
	"emt_PG_cylinder"		, "%f:%f",						//eEffKey_effPSys_emt_pgCylinder
	"emt_PG_sphere"			, "%f",							//eEffKey_effPSys_emt_pgSphere
	//particle property
	"pProp_omega"			, "%f:%f",						//eEffKey_effPSys_pProp_omega		
	"pProp_life"			, "%d:%d",						//eEffKey_effPSys_pProp_life		
	"pProp_cofEnvirenment"	, "%f:%f:%f:%f",				//eEffKey_effPSys_pProp_cofEnvrn	
	"pProp_flag"			, "%x",							//eEffKey_effPSys_pProp_flag		

	//particle system simple black hole
	"capacity"				, "%d",							//eEffKey_effPSysSBH_capacity
	"numOneShoot"			, "%d:%d",						//eEffKey_effPSysSBH_numOneShoot
	"ShootDelay"			, "%d",							//eEffKey_effPSysSBH_shootDelay
	"particleLife"			, "%d",							//eEffKey_effPSysSBH_particleLife
	"initSpeed"				, "%f:%f",						//eEffKey_effPSysSBH_initspeed
	"rollRange"				, "%f:%f",						//eEffKey_effPSysSBH_rollRange
	"radius"				, "%f",							//eEffKey_effPSysSBH_radius

	//effAnimation
	"anim_type"				, "%d", 						//eEffKey_effAnim_type
	"anim_flag"				, "%x", 						//eEffKey_effAnim_flag
	"anim_life"				, "%d", 						//eEffKey_effAnim_life
	"anim_loopOpt"			, "%d",							//eEffKey_effAnim_loopOpt

	//AgcdEffAnim_Colr
	"animColr_num"			, "%d",							//eEffKey_effAnimColr_num
	"animColr_rbga"			, "%d:%d:%d:%d:%d",				//eEffKey_effAnimColr

	//AgcdEffAnim_TuTv
	"animTuTv_num"			, "%d",							//eEffKey_effAnimTuTv_num
	"animTuTv_rect"			, "%d:%f:%f:%f:%f",				//eEffKey_effAnimTuTvRect
	
	//AgcdEffAnim_Missile
	"animMissile_initSpeed"	, "%f", 						//eEffKey_effAnimMissile_initSped
//	"animMissile_num"		, "",							//eEffKey_effAnimMissile_num
	"animMissile_accel"		, "%f", 						//eEffKey_effAnimMissile_accel

	"animMissile_rotate"		, "%f", 						//eEffKey_effAnimMissile_rotate
	"animMissile_radius"		, "%f", 						//eEffKey_effAnimMissile_radius
	"animMissile_zigzaglength"	, "%f", 						//eEffKey_effAnimMissile_zigzaglength

	"animMissile_minspeed"		, "%f", 						//eEffKey_effAnimMissile_minspeed
	"animMissile_maxspeed"		, "%f", 						//eEffKey_effAnimMissile_maxspeed

	"animCamera_type"			, "%d",							//eEffKey_effAnimCamera_type,
	"animCamera_pos"			, "%f:%f:%f",					//eEffKey_effAnimCamera_pos,
	"animCamera_dir"			, "%f:%f:%f",					//eEffKey_effAnimCamera_dir,
	"animCamera_speed"			, "%f",							//eEffKey_effAnimCamera_speed,
	"animCamera_rotate"			, "%f",							//eEffkey_effAnimCamera_rotate,
	"animCamera_rotatecount"	, "%f",							//eEffKey_effAnimCamera_rotatecount,
	"animCamera_movelength"		, "%f",							//aeEffkey_effAnimCamera_movelength,


	//AgcdEffAnim_Linear
	"animLinear_num"		, "%d",							//eEffKey_effAnimLinear_num
	"animLinear_pos"		, "%d:%f:%f:%f",				//eEffKey_effAnimLinear_pos

	//AgcdEffAnim_Revolution
	"animRev_axis"			, "%f:%f:%f",					//eEffKey_effAnimRev_axis
	"animRev_num"			, "%d",							//eEffKey_effAnimRev_num
	"animRev_rev"			, "%d:%f:%f:%f",				//eEffKey_effAnimRev_rev
	
	//AgcdEffAnim_Rotation
	"animRot_axis"			, "%f:%f:%f",					//eEffKey_effAnimRot_axis
	"animRot_num"			, "%d",							//eEffKey_effAnimRot_num
	"animRot_rot"			, "%d:%f",						//eEffKey_effAnimRot_rot

	//AgcdEffAnim_Spline
	"animSpline_file"		, "%s",							//eEffKey_effAnimSpline_file

	//AgcdEffAnim_RtAnim
	"animRtAnim_file"		, "%s",							//eEffKey_effAnimRtAnim_file

	//AgcdEffAnim_Scale
	"animScale_num"			, "%d",							//eEffKey_effAnimScale_num
	"animScale_scale"		, "%d:%f:%f:%f",				//eEffKey_effAnimScale_scale
	
	//@{ kday 20050421
	//AgcdEffPostFX
	"sharedparam"			, "%d:%f:%f",					//eEffKey_effPostFX_sharedparam, type:center.x:center.y
	"waveparam"				, "%f:%f:%s",					//eEffKey_effPostFX_wave, amplitude:frequency:texname
	"shockwaveparam"		, "%f:%f:%s",					//eEffKey_effPostFX_shockwave, bias:width:texname
	"rippleparam"			, "%f:%f",						//eEffKey_effPostFX_ripple, scale:frequency
	//@} kday
};

RwChar* SECT(eEffSect_Name eSect, int index)
{
	static	RwChar SZSECT[MAX_PATH];
	Eff2Ut_ZEROBLOCK( SZSECT );

	if( T_ISMINUS4(index) )
		sprintf(SZSECT, "%s", EFF_SECT[eSect]);

	else
		sprintf(SZSECT, "%s:%d", EFF_SECT[eSect], index);

	return SZSECT;
}

RwChar*	KEY(eEffKey_Name eKey, int index)
{
	static	RwChar SZKEY[MAX_PATH];

	Eff2Ut_ZEROBLOCK( SZKEY );

	if( T_ISMINUS4(index) )
	{
		ASSERT( sizeof(SZKEY) > strlen(EFF_KEY[eKey][e_key]) );
		strcpy(SZKEY, EFF_KEY[eKey][e_key]);
	}
	else
		sprintf(SZKEY, "%s:%d", EFF_KEY[eKey][e_key], index);

	return SZKEY;
}

RwChar* VALUE(eEffKey_Name eKey, ... )
{
	static	RwChar SZVALUE[MAX_PATH*4];

	Eff2Ut_ZEROBLOCK( SZVALUE );

	va_list	ap;

	va_start( ap, eKey );
	vsprintf( SZVALUE, EFF_KEY[eKey][e_form], ap );
	va_end(ap);

	return SZVALUE;
}

eEffSect_Name BASE_SECT(AgcdEffBase* pIn)
{
	switch( pIn->bGetBaseType() )
	{
	case AgcdEffBase::E_EFFBASE_BOARD				:	return eEffSect_effbase_board		;
	case AgcdEffBase::E_EFFBASE_PSYS				:	return eEffSect_effbase_psyst		;
	case AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:	return eEffSect_effbase_psyst_SBH	;
	case AgcdEffBase::E_EFFBASE_TAIL				:	return eEffSect_effbase_tail		;
	case AgcdEffBase::E_EFFBASE_OBJECT				:	return eEffSect_effbase_obj			;
	case AgcdEffBase::E_EFFBASE_LIGHT				:	return eEffSect_effbase_light		;
	case AgcdEffBase::E_EFFBASE_SOUND				:	return eEffSect_effbase_sound		;
	case AgcdEffBase::E_EFFBASE_MOVINGFRAME			:	return eEffSect_effbase_mfrm		;
	case AgcdEffBase::E_EFFBASE_TERRAINBOARD		:	return eEffSect_effbase_terrainb	;
	case AgcdEffBase::E_EFFBASE_POSTFX				:	return eEffSect_effbase_postfx		;
	}
	ASSERT( "kday" && !"unknown base type" );
	return eEffSect_Num;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
AgcuEffIniMng::AgcuEffIniMng()
{
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
AgcuEffIniMng::~AgcuEffIniMng()
{
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
eEffSect_Name AgcuEffIniMng::_FindSect( const char* szSectName )
{
	RwChar	szTempSect[MAX_PATH]			= "";
	
	ASSERT( sizeof(szTempSect) > strlen(szSectName) );
	strcpy( szTempSect, szSectName );
	szTempSect[strcspn( szSectName, ":" )]	= '\0';

	for( RwInt32 i=0; i<eEffSect_Num; ++i )
	{
		if( strcmp( szTempSect, EFF_SECT[i] ) )
			continue;
		
		return static_cast<eEffSect_Name>(i);
	}

	return eEffSect_Num;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
eEffKey_Name AgcuEffIniMng::_FindKey( eEffSect_Name eSect, const char* szKeyName )
{
	RwChar	szTempKey[MAX_PATH]				= "";
	ASSERT( sizeof(szTempKey) > strlen(szKeyName) );
	strcpy( szTempKey, szKeyName );
	szTempKey[strcspn( szTempKey, ":" )]	= '\0';

	for( RwInt32 i=0; i<eEffKey_Num; ++i )
	{
		if( !strcmp( szTempKey, EFF_KEY[i][e_key] ) )
			return static_cast<eEffKey_Name>(i);
	}

	return eEffKey_Num;
}



//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::Read_txt( const RwChar* szFName, AgcdEffSet* pOut, BOOL bFullRead )
{
	BOOL	br	= TRUE;

	m_cIniManager.SetPath( szFName );
	br	= m_cIniManager.ReadFile();
	if( !br )
		return FALSE;

	//유효 ini파일 검사.
	if( m_cIniManager.GetNumSection() < 1 )
		return FALSE;

	//버전 체크.
	if( strcmp( EFFINIVER, m_cIniManager.GetValue(0, eEffKey_set_iniver ) ) )
		return FALSE;

	//read effSet
	return _Read_EffSet_txt( pOut, bFullRead );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::Read_txt( RwUInt32 dwEffSetID, AgcdEffSet* pOut, BOOL bFullRead )
{
	ASSERT( pOut && "@ AgcuEffIniMng::bRead_txt" );

	PROFILE("AgcuEffIniMng::bRead_txt");

	BOOL	br	= TRUE;

	//from ini file
	RwChar	szFName[MAX_PATH]		= "";
	sprintf( szFName, "%d_eff.ini", dwEffSetID );
	RwChar	szFullName[MAX_PATH]	= "";
	sprintf( szFullName, "%s%s",AgcuEffPath::GetPath_Ini(),szFName);
	m_cIniManager.SetPath( szFullName );

	{
	PROFILE("AgcuEffIniMng::bRead_txt - readfile");
	br	= m_cIniManager.ReadFile();
	if( !br )
		return FALSE;
	}

	//유효 ini파일 검사.
	if( m_cIniManager.GetNumSection() < 1 )
		return FALSE;

	//버전 체크.
	if( strcmp( EFFINIVER, m_cIniManager.GetValue(0, eEffKey_set_iniver ) ) )
		return FALSE;

	//read effSet
	return  _Read_EffSet_txt( pOut, bFullRead );

}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffSet_txt( AgcdEffSet* pOut, BOOL bFullRead )
{
	BOOL	br	= TRUE;
	RwInt32	ir	= 0;

	CONST	CHAR*		szSectionName	=	m_cIniManager.GetSectionName( eEffSect_Set );

#ifndef USE_MFC
	if( !bFullRead )
#endif//USE_MFC
	{//초기에 이펙트셋 맵을 구성할때..
		//id
		pOut->bSetID( atol( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_id ) ) );
		//title
		pOut->bSetTitle( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_title ) );
		//life
		pOut->bSetLife( atol( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_life ) ) );
		//loopOption
		pOut->bSetLoopOpt( 
			static_cast<E_LOOPOPT>( atoi( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_loopOpt ) ) ) );

		//flag
		RwUInt32	dwFlag	= 0LU;
		ir	= sscanf( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_flag )
					, "%x"
					, &dwFlag
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffSet_txt" );
			return FALSE;
		}
		pOut->bSetBitFlags( dwFlag );

		//missile info
		STMISSILEINFO	stmissileinfo;
		ir = sscanf( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_missileInfo )
				, "%d:%f:%f:%f"
				, &stmissileinfo.m_eMissileType
				, &stmissileinfo.m_v3dOffset.x
				, &stmissileinfo.m_v3dOffset.y
				, &stmissileinfo.m_v3dOffset.z
				);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffSet_txt" );
			return FALSE;
		}
		pOut->bSetMissileInfo( stmissileinfo );

		//bSphere
		RwSphere	bsphere;
		ir	= sscanf( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_sphere )
				, "%f:%f:%f:%f"		 
				, &bsphere.center.x
				, &bsphere.center.y
				, &bsphere.center.z
				, &bsphere.radius 
				);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffSet_txt" );
			return FALSE;
		}
		pOut->bSetBSphere( bsphere );

		//bbox
		RwBBox	bbox;
		ir	= sscanf( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_box )
				, "%f:%f:%f:%f:%f:%f"		  
				, &bbox.sup.x
				, &bbox.sup.y
				, &bbox.sup.z
				, &bbox.inf.x
				, &bbox.inf.y
				, &bbox.inf.z 
				);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffSet_txt" );
			return FALSE;
		}
		pOut->bSetBBox( bbox );

		//variable size
		AgcdEffSet::StVarSizeInfo	stVarSize;
		ir	= sscanf( m_cIniManager.GetValue( eEffSect_Set, eEffKey_set_varsize )
				, "%d:%d:%d"
				, &stVarSize.m_nNumOfTex
				, &stVarSize.m_nNumOfBase
				, &stVarSize.m_nNumOfBaseDependancy
				);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffSet_txt" );
			return FALSE;
		}
		pOut->bSetVarSizeInfo( stVarSize );

#ifndef USE_MFC
		return TRUE;
#endif//USE_MFC	
	}
#ifndef USE_MFC
	else
#else
	if( bFullRead )
#endif//USE_MFC
	{//이펙트셋 reference counter 가 0 일때.

		ASSERT( pOut->bGetRefCnt() == 0 && "@ AgcuEffIniMng::vRead_EffSet_txt" );

		//effTex
		RwInt32	i=0;
		RwInt32	nCurrKey	= eEffKey_set_efftex;
		const RwChar*	pKeyVal1	= NULL;
		const RwChar*	pKeyVal2	= NULL;

		CHAR			szEffTex[ MAX_PATH ];
		CHAR			szEffMask[ MAX_PATH ];

		for( i=0; i<pOut->bGetVarSizeInfo().m_nNumOfTex; ++i, ++nCurrKey )
		{
			sprintf_s( szEffTex , sizeof(szEffTex), "%s:%d" , EFF_KEY[ eEffKey_set_efftex ][e_key] , i  );
			sprintf_s( szEffMask , sizeof(szEffMask), "%s:%d" , EFF_KEY[ eEffKey_set_effmask ][e_key] , i  );

			pKeyVal1	= m_cIniManager.GetValue( szSectionName, szEffTex );
			pKeyVal2	= m_cIniManager.GetValue( szSectionName, szEffMask );

			if( pOut->bInsEffTex( pKeyVal1, pKeyVal2 ) < 0 )
			{
				Eff2Ut_ERR( "pOut->bInsEffTex failed @ AgcuEffIniMng::vRead_EffSet_txt" );
				return FALSE;
			}
		}

		AgcdEffSet::stBaseDependancy	stBaseDpnd;
		CHAR		szBaseDpnd[ MAX_PATH ];

		for( i=0; i<pOut->bGetVarSizeInfo().m_nNumOfBaseDependancy; ++i, ++nCurrKey )
		{

			sprintf_s( szBaseDpnd , sizeof(szBaseDpnd), "%s:%d" , EFF_KEY[ eEffKey_set_depnd ][e_key] , i );

			ir = stBaseDpnd.SetDataFromStream( m_cIniManager.GetValue( szSectionName, szBaseDpnd ) );
			if( ir == EOF )
			{
				Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffSet_txt" );
				return FALSE;
			}

			pOut->bInsDependancy( stBaseDpnd );
		}

		//effBase
		for( i=0; i<pOut->bGetVarSizeInfo().m_nNumOfBase; ++i )
		{
			switch( _FindSect( m_cIniManager.GetSectionName( i+1 ) ) )
			{
			case eEffSect_effbase_board		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_BOARD ) < 0 )
						return FALSE;

					//read					
					AgcdEffBoard* pEffBoard	= static_cast< AgcdEffBoard* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffBoard_txt( i+1, pEffBoard );
					if( !br )
						return br;

					//set Tex
					ir	= pEffBoard->bSetPtrEffTex( pOut->bGetPtrEffTex( pEffBoard->bGetTexIndex() ) );
					if( ir < 0 )
						return FALSE;

				}
				break;

			case eEffSect_effbase_light		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_LIGHT ) < 0 )
						return FALSE;

					//read
					AgcdEffLight* pEffLight	= static_cast< AgcdEffLight* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffLight_txt( i+1, pEffLight );
					if( !br )
						return br;

				}
				break;

			case eEffSect_effbase_mfrm		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_MOVINGFRAME ) < 0 )
						return FALSE;

					//read
					AgcdEffMFrm* pEffMFrm	= static_cast< AgcdEffMFrm* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffMFrm_txt( i+1, pEffMFrm );
					if( !br )
						return br;

				}
				break;


			case eEffSect_effbase_obj		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_OBJECT ) < 0 )
						return FALSE;

					//read
					AgcdEffObj* pEffObj	= static_cast< AgcdEffObj* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffObj_txt( i+1, pEffObj );
					if( !br )
						return br;

				}
				break;


			case eEffSect_effbase_sound		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_SOUND ) < 0 )
						return FALSE;

					//read
					AgcdEffSound* pEffSound	= static_cast< AgcdEffSound* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffSound_txt( i+1, pEffSound );
					if( !br )
						return br;
				}
				break;


			case eEffSect_effbase_tail		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_TAIL ) < 0 )
						return FALSE;

					//read
					AgcdEffTail* pEffTail	= static_cast< AgcdEffTail* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffTail_txt( i+1, pEffTail );
					if( !br )
						return br;

					//set Tex
					ir	= pEffTail->bSetPtrEffTex( pOut->bGetPtrEffTex( pEffTail->bGetTexIndex() ) );
					if( ir < 0 )
						return FALSE;

				}
				break;


			case eEffSect_effbase_psyst		:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_PSYS ) < 0 )
						return FALSE;

					//read
					AgcdEffParticleSystem* pEffPSyst	= static_cast< AgcdEffParticleSystem* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffPSys_txt( i+1, pEffPSyst );
					if( !br )
						return br;

					//set Tex
					ir	= pEffPSyst->bSetPtrEffTex( pOut->bGetPtrEffTex( pEffPSyst->bGetTexIndex() ) );
					if( ir < 0 )
						return FALSE;

				}
				break;


			case eEffSect_effbase_psyst_SBH	:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE ) < 0 )
						return FALSE;

					//read
					AgcdEffParticleSys_SimpleBlackHole* pEffPSystSBH	= static_cast< AgcdEffParticleSys_SimpleBlackHole* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffPSysSBH_txt( i+1, pEffPSystSBH );
					if( !br )
						return br;

					//set Tex
					ir	= pEffPSystSBH->bSetPtrEffTex( pOut->bGetPtrEffTex( pEffPSystSBH->bGetTexIndex() ) );
					if( ir < 0 )
						return FALSE;

				}
				break;

			case eEffSect_effbase_terrainb:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_TERRAINBOARD ) < 0 )
						return FALSE;

					//read
					AgcdEffTerrainBoard* pEffTerrainB	= static_cast< AgcdEffTerrainBoard* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffTerrainB_txt( i+1, pEffTerrainB );
					if( !br )
						return br;

					//set Tex
					ir	= pEffTerrainB->bSetPtrEffTex( pOut->bGetPtrEffTex( pEffTerrainB->bGetTexIndex() ) );
					if( ir < 0 )
						return FALSE;

				}
				break;

			case eEffSect_effbase_postfx:
				{
					//insert
					if( pOut->bInsEffBase( AgcdEffBase::E_EFFBASE_POSTFX ) < 0 )
						return FALSE;

					//read
					AgcdEffPostFX* pEffPostFX	= static_cast< AgcdEffPostFX* > ( pOut->bGetPtrEffBase( i ) );
					br	= _Read_EffPostFX_txt( i+1, pEffPostFX );
					if( !br )
						return br;

				}break;

			default:
				return FALSE;

			}//switch
		}//for

	}// if(bFullRead) {}else
	
	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffBase_txt( RwInt32 nSect, AgcdEffBase* pOut )
{
	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffBase_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffBase_txt" );
		return FALSE;
	}

	RwInt32	ir	= 0;

	if( pOut->bSetTitle( m_cIniManager.GetValue( nSect, 0 ) ) < 0 )
	{
		Eff2Ut_ERR( "pOut->bSetTitle failed @ AgcuEffIniMng::vRead_EffBase_txt" );
		return FALSE;
	}

	RwUInt32					dwDelay;
	RwUInt32					dwLife;
	E_LOOPOPT					eLoopOpt;
	RwUInt32					dwBitFlags;
	AgcdEffBase::StVarSizeInfo	StVarSizeInfo;
	AgcdEffBase::E_EFFBASETYPE	eBaseType;


	ir	= sscanf( m_cIniManager.GetValue( nSect, 1 )
				, "%d:%d:%d:%d:%x:%d"
				, &eBaseType
				, &dwDelay
				, &dwLife
				, &eLoopOpt
				, &dwBitFlags
				, &StVarSizeInfo.m_nNumOfAnim
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffBase_txt" );
		return FALSE;
	}
	ASSERT( eBaseType == pOut->bGetBaseType() && "@ AgcuEffIniMng::vRead_EffBase_txt" );

	if( eBaseType == pOut->bGetBaseType() )
	{
		pOut->bSetDelay( dwDelay );
		pOut->bSetLife( dwLife );
		pOut->bSetLoopOpt( eLoopOpt );
		pOut->bSetFlag( dwBitFlags );
		pOut->bSetVarSizeInfo( StVarSizeInfo );
	}
	else
	{
		Eff2Ut_ERR( "eBaseType != pOut->bGetBaseType() @ AgcuEffIniMng::vRead_EffBase_txt" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffRenderBase_txt( RwInt32 nSect, AgcdEffRenderBase* pOut )
{
	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffRenderBase_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffRenderBase_txt" );
		return FALSE;
	}

	RwInt32	ir	= 0;

	AgcdEffRenderBase::E_EFFBLENDTYPE		eBlendType;
	RwInt32									nTexIndex;
	RwV3d									v3dInitPos;
	STANGLE									stInitAngle;

	ir	= sscanf( m_cIniManager.GetValue( nSect, 2 )
				, "%d:%d:%f:%f:%f:%f:%f:%f"
				, &eBlendType
				, &nTexIndex
				, &v3dInitPos.x
				, &v3dInitPos.y
				, &v3dInitPos.z
				, &stInitAngle.m_fPitch
				, &stInitAngle.m_fYaw
				, &stInitAngle.m_fRoll
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf @ AgcuEffIniMng::vRead_EffRenderBase_txt" );
		return FALSE;
	}

	pOut->bSetBlendType( eBlendType );
	pOut->bSetTexIndex( nTexIndex );
	pOut->bSetInitPos( v3dInitPos );
	pOut->bSetInitAngle( stInitAngle );

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffBoard_txt( RwInt32 nSect, AgcdEffBoard* pOut )
{
	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffBoard_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffBoard_txt" );
		return FALSE;
	}
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;
	
	//EffAnim
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
			return br;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffTerrainB_txt( RwInt32 nSect, AgcdEffTerrainBoard* pOut )
{
	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffTerrainB_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffTerrainB_txt" );
		return FALSE;
	}
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffTerrainB_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffTerrainB_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;
	
	//EffAnim
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
			return br;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffPostFX_txt( RwInt32 nSect, AgcdEffPostFX* pOut )
{

	CONST	CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffTerrainB_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffTerrainB_txt" );
		return FALSE;
	}
	
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffTerrainB_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 2;
	//sharedparam

	stSharedParam	tmpSharedparam;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPostFX_sharedparam ][e_key] )
				, EFF_KEY[ eEffKey_effPostFX_sharedparam ] [e_form]
				, &tmpSharedparam.type
				, &tmpSharedparam.center.x
				, &tmpSharedparam.center.y
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}
	pOut->bSetSharedParam( tmpSharedparam.center, tmpSharedparam.type );
	++nCurrKey;

    //unionparam
	switch( pOut->bGetSharedParam().type )
	{
	case e_fx_ClampingCircle: break;
	case e_fx_Wave			:
		{
			stWaveParam	tmp;
			ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPostFX_wave ][e_key] )
						, EFF_KEY[ eEffKey_effPostFX_wave ] [e_form]
						, &tmp.amplitude
						, &tmp.frequency
						, &tmp.tex
						);
			if( ir == EOF )
			{
				Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
				return FALSE;
			}
			pOut->bSetWaveParam( tmp.amplitude, tmp.frequency, tmp.tex );
			++nCurrKey;
		}break;
	case e_fx_Shockwave		: 
		{
			stShockwaveParam	tmp;
			ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPostFX_shockwave ][e_key] )
						, EFF_KEY[ eEffKey_effPostFX_shockwave ] [e_form]
						, &tmp.bias
						, &tmp.width
						, &tmp.tex
						);
			if( ir == EOF )
			{
				Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
				return FALSE;
			}
			pOut->bSetShockwaveParam( tmp.bias, tmp.width, tmp.tex );
			++nCurrKey;
		}break;
	case e_fx_Darken		: break;
	case e_fx_Brighten		: break;
	case e_fx_Ripple		: 
		{

			stRippleParam	tmp;
			ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPostFX_ripple ][e_key] )
						, EFF_KEY[ eEffKey_effPostFX_ripple ] [e_form]
						, &tmp.scale
						, &tmp.frequency
						);
			if( ir == EOF )
			{
				Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
				return FALSE;
			}
			pOut->bSetRippleParam( tmp.scale, tmp.frequency );
			++nCurrKey;
		}break;
	case e_fx_Twist			: break;
	default:
		ASSERT( "kday" && !"unknown postfx type" );
		return FALSE;
	}

	//EffAnim
	ASSERT( "kday" && pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim == 1);
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
			return br;
		}
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffTail_txt( RwInt32 nSect, AgcdEffTail* pOut )
{
	CONST	CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffTail_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffTail_txt" );
		return FALSE;
	}
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;

	//capacity
	pOut->bSetCapacity( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effTail_capacity ][e_key] ) );

	//insertTimeGap
	ASSERT( !strncmp( EFF_KEY[ eEffKey_effTail_timegap ][e_key]
					, m_cIniManager.GetKeyName( nSect, nCurrKey + 1 )
					, strlen( EFF_KEY[ eEffKey_effTail_timegap ][e_key] )
					)
		   );
	if( strncmp( EFF_KEY[ eEffKey_effTail_timegap ][e_key]
					, m_cIniManager.GetKeyName( nSect, nCurrKey + 1 )
					, strlen( EFF_KEY[ eEffKey_effTail_timegap ][e_key] )
					) )
	{
		Eff2Ut_ERR( "!strncmp" );
		return FALSE;
	}
	pOut->bSetTimeGap( static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[eEffKey_effTail_timegap][e_key] ) ) ) );
	pOut->bSetPointLife( static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effTail_pointLife ][e_key] ) ) ) );

	RwReal	fh1=0.f, fh2=0.f;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effTail_height ][e_key] )
				, "%f:%f"
				, &fh1
				, &fh2
				);
	pOut->bSetHeight(fh1, fh2);

	//EffAnim
	++nCurrKey;
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffBoard_txt" );
			return br;
		}
	}

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffPSys_txt( RwInt32 nSect, AgcdEffParticleSystem* pOut )
{
	CONST	CHAR*			szSectionName		=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffPSys_txt" );
	if( !pOut )
	{
		Eff2Ut_ERR( "pOut == NULL @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;
	//capacity
	pOut->bSetCapacity( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_capacity ][e_key] ) );
	pOut->bSetShootDelay( static_cast<RwUInt32>(m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_shootDelay ][e_key] ) ) );
	pOut->bSetClumpFile( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_clumpFile ][e_key] ) );

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_power ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefEmiter().m_fPower
				, &pOut->bGetRefEmiter().m_fPowerOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}	

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_gunLen ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefEmiter().m_fGunLength
				, &pOut->bGetRefEmiter().m_fGunLengthOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_oneshootnum ][e_key] )
				, "%d:%d"
				, &pOut->bGetRefEmiter().m_nNumOfOneShoot
				, &pOut->bGetRefEmiter().m_nNumOfOneShootOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_omegaYawPitch ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefEmiter().m_fOmegaYaw_World
				, &pOut->bGetRefEmiter().m_fOmegaPitch_Local
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_yawMinMax ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefEmiter().m_fMinYaw
				, &pOut->bGetRefEmiter().m_fMaxYaw
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_pitchMinMax ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefEmiter().m_fMinPitch
				, &pOut->bGetRefEmiter().m_fMaxPitch
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_initDir ][e_key] )
				, "%f:%f:%f"
				, &pOut->bGetRefEmiter().m_vDir.x
				, &pOut->bGetRefEmiter().m_vDir.y
				, &pOut->bGetRefEmiter().m_vDir.z
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}	
	pOut->bGetRefEmiter().bUpdateSideVector();

	//eEffKey_effPSys_emt_conAngle	
	pOut->bGetRefEmiter().m_fConAngle	=
		static_cast<RwReal>( m_cIniManager.GetValueF( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_conAngle ][e_key] ) );

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_flag ][e_key] )
				, "%x"
				, &pOut->bGetRefEmiter().m_dwFlagOfEmiter
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}			

	pOut->bGetRefEmiter().m_ePGroup	=
		static_cast<ePGroup>( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_ePGroup ][e_key] ) );
	
	//eEffKey_effPSys_emt_pgBox
	//union 멤버 이므로 가장 큰 스트럭처로 읽는다.
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_emt_pgBox ][e_key] )
				, "%f:%f:%f"
				, &pOut->bGetRefEmiter().m_pgroupBox.m_fHWidth
				, &pOut->bGetRefEmiter().m_pgroupBox.m_fHHeight
				, &pOut->bGetRefEmiter().m_pgroupBox.m_fHDepth
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}
	
	//particle property
	//eEffKey_effPSys_pProp_omega
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_pProp_omega ][e_key] )
				, "%f:%f"
				, &pOut->bGetRefPProp().m_fPAngularspeed
				, &pOut->bGetRefPProp().m_fPAngularspeedOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}			

	//eEffKey_effPSys_pProp_life		
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_pProp_life ][e_key] )
				, "%d:%d"
				, &pOut->bGetRefPProp().m_dwParticleLife
				, &pOut->bGetRefPProp().m_dwParticleLifeOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}		

	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSys_pProp_cofEnvrn ][e_key] )
				, "%f:%f:%f:%f"
				, &pOut->bGetRefPProp().m_stCofEnvrn.m_fCofGrav
				, &pOut->bGetRefPProp().m_stCofEnvrn.m_fCofAirResistance
				, &pOut->bGetRefPProp().m_stCofEnvrnOffset.m_fCofGrav
				, &pOut->bGetRefPProp().m_stCofEnvrnOffset.m_fCofAirResistance
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}		

	ir	= sscanf( m_cIniManager.GetValue( szSectionName,(CHAR*)EFF_KEY[ eEffKey_effPSys_pProp_flag ][e_key] )
				, "%x"
				, &pOut->bGetRefPProp().m_dwFlagOfParticle
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
		return FALSE;
	}			

	//EffAnim
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffPSys_txt" );
			return br;
		}
	}

//	Eff2Ut_TOFILE( "파티클분석.txt"
//		, Eff2Ut_FmtMsg("capacity : %d, plife : %d, pLifeOffset : %d\n"
//		, pOut->bGetCapacity()
//		, pOut->bGetCRefPProp().m_dwParticleLife
//		, pOut->bGetCRefPProp().m_dwParticleLifeOffset ) );

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffPSysSBH_txt( RwInt32 nSect, AgcdEffParticleSys_SimpleBlackHole* pOut )
{
	CONST CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;

	pOut->bSetCapacity( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_capacity ][e_key] ) );


	RwUInt32	dwOneShootNum		= 0UL,
				dwOneShootNumOffset	= 0UL;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_numOneShoot ][e_key] )
				, "%d:%d"
				, &dwOneShootNum
				, &dwOneShootNumOffset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return FALSE;
	}
	pOut->bSetOneShootNum( dwOneShootNum, dwOneShootNumOffset );

	//eEffKey_effPSysSBH_shootDelay
	pOut->bSetShootDelay( 
		static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName,(CHAR*)EFF_KEY[ eEffKey_effPSysSBH_shootDelay ][e_key] ) ) ) );

	//eEffKey_effPSysSBH_particleLife
	pOut->bSetPLife( 
		static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_particleLife ][e_key] ) ) ) );

	//eEffKey_effPSysSBH_initspeed

	RwReal	fSpeed0			= 0UL,
			fSpeed0Offset	= 0UL;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_initspeed ][e_key] )
				, "%f:%f"
				, &fSpeed0
				, &fSpeed0Offset
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return FALSE;
	}
	pOut->bSetInitSpeed( fSpeed0, fSpeed0Offset );

	//eEffKey_effPSysSBH_rollRange
	RwReal	fRollMin = 0.f,
			fRollMax = 0.f;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_rollRange ][e_key] )
				, "%f:%f"
				, &fRollMin
				, &fRollMax
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return FALSE;
	}
	pOut->bSetRollMinMax( fRollMin, fRollMax );

	//eEffKey_effPSysSBH_radius
	RwReal	fRadius = 0.f;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effPSysSBH_radius ][e_key] )
				, "%f"
				, &fRadius
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
		return FALSE;
	}
	pOut->bSetRadius( fRadius );
	
	//EffAnim
	++nCurrKey;
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
			return br;
		}
	}

//	Eff2Ut_TOFILE( "파티클분석.txt"
//		, Eff2Ut_FmtMsg("capacity : %d, plife : %d\n"
//		, pOut->bGetCapacity()
//		, pOut->bGetParticleLife() ) );
	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffObj_txt( RwInt32 nSect, AgcdEffObj* pOut )
{

	CONST	CHAR*		szSectionName		=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffObj_txt" );
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffObj_txt" );
		return br;
	}
	//effRenderBase
	br	= _Read_EffRenderBase_txt( nSect, static_cast<AgcdEffRenderBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffObj_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 3;

	//eEffKey_effObj_clumpFile
	if( DEF_FLAG_CHK( pOut->bGetFlag(), FLAG_EFFBASEOBJ_DUMMY ) )
	{
		ir	= pOut->bSetClumpName( "" );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bSetClumpName failed @ AgcuEffIniMng::vRead_EffObj_txt" );
			return FALSE;
		}
		++nCurrKey;
	}
	else
	{
		ir	= pOut->bSetClumpName( m_cIniManager.GetValue( szSectionName , (CHAR*)EFF_KEY[ eEffKey_effObj_clumpFile ][e_key] ) );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bSetClumpName failed @ AgcuEffIniMng::vRead_EffObj_txt" );
			return FALSE;
		}

	}

	//eEffKey_effObj_Prelit
	int	nr=0,ng=0,nb=0,na=0;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effObj_Prelit ][e_key] )
				, "%d:%d:%d:%d"
				, &nr
				, &ng
				, &nb
				, &na
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffObj_txt" );
		return FALSE;
	}
	RwRGBA	rgba = {nr,ng,nb,na};
	pOut->bSetPreLit(rgba);
	
	//EffAnim
	++nCurrKey;
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
			return br;
		}
	}

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffLight_txt( RwInt32 nSect, AgcdEffLight* pOut )
{
	CONST	CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffLight_txt" );
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffLight_txt" );
		return br;
	}

	RwInt32	ir			= 0,
			i			= 0,
			nCurrKey	= 2;

	//eEffKey_effLight_type
	AgcdEffLight::E_EFFLIGHTTYPE eEffLightType	=
		static_cast<AgcdEffLight::E_EFFLIGHTTYPE>( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_type ][e_key] ) );
	pOut->bSetLightType( eEffLightType );
	//eEffKey_effLight_pos
	RwV3d	v3dPos;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_pos ][e_key] )
				, "%f:%f:%f"
				, &v3dPos.x
				, &v3dPos.y
				, &v3dPos.z
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffLight_txt" );
		return FALSE;
	}
	//eEffKey_effLight_angle
	STANGLE	stAngle;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_angle ][e_key] )
				, "%f:%f:%f"
				, &stAngle.m_fYaw
				, &stAngle.m_fPitch
				, &stAngle.m_fRoll
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffLight_txt" );
		return FALSE;
	}
	//eEffKey_effLight_conAngle
	pOut->bSetConAngle(
		static_cast<RwReal>( atof( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_conAngle ][e_key] ) ) ) );

	//eEffKey_effLight_surfProp
	RwSurfaceProperties	rwSurfProp;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_surfProp ][e_key] )
				, "%f:%f:%f"
				, &rwSurfProp.ambient
				, &rwSurfProp.specular
				, &rwSurfProp.diffuse
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffLight_txt" );
		return FALSE;
	}
	pOut->bSetSurfProp( rwSurfProp );

	//eEffKey_effLight_matColr
	RwRGBA rgbaMaterial;
	int	nr=0,ng=0,nb=0,na=0;
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effLight_matColr ][e_key] )
				, "%d:%d:%d:%d"
				, &nr
				, &ng
				, &nb
				, &na
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffLight_txt" );
		return FALSE;
	}
	rgbaMaterial.alpha	= na;
	rgbaMaterial.red	= nr;
	rgbaMaterial.green	= ng;
	rgbaMaterial.blue	= nb;
	pOut->bSetMaterialColor( rgbaMaterial );
	
	//EffAnim
	++nCurrKey;
	for( i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
			return br;
		}
	}

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffSound_txt( RwInt32 nSect, AgcdEffSound* pOut )
{	
	CONST	CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffSound_txt" );
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffSound_txt" );
		return br;
	}

	RwInt32	nCurrKey	= 2;
	
	//eEffKey_effSound_type
	pOut->bSetSoundType(
		static_cast<AgcdEffSound::eEffSoundType>
		( atoi( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effSound_type ][e_key] ) ) ) );

	//eEffKey_effSound_file
	pOut->bSetSoundName( m_cIniManager.GetValue( szSectionName,(CHAR*)EFF_KEY[ eEffKey_effSound_file ][e_key] ) );
	//eEffKey_effSound_monofile
	pOut->bSetMonoFName( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effSound_monofile ][e_key] ) );
	//eEffKey_effSound_loopCnt
	pOut->bSetLoopCnt( 
		static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName , (CHAR*)EFF_KEY[ eEffKey_effSound_loopCnt ][e_key] ) ) ) );
	//eEffKey_effSound_volume
	pOut->bSetVolume( 
		static_cast<RwReal>( atof( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effSound_volume ][e_key] ) ) ) );

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffMFrm_txt( RwInt32 nSect, AgcdEffMFrm* pOut )
{

	CONST	CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	ASSERT( pOut && "@ AgcuEffIniMng::vRead_EffSound_txt" );
	//effBase
	BOOL br = _Read_EffBase_txt( nSect, static_cast<AgcdEffBase*>( pOut ) );
	if( !br )
	{
		Eff2Ut_ERR( "vRead_EffBase_txt failed @ AgcuEffIniMng::vRead_EffSound_txt" );
		return br;
	}

	RwInt32	nCurrKey	= 2;
	//eEffKey_effMFrm_type
	ASSERT( !strncmp( m_cIniManager.GetKeyName( nSect, nCurrKey )
					, EFF_KEY[ eEffKey_effMFrm_type ][e_key]
					, strlen( EFF_KEY[ eEffKey_effMFrm_type ][e_key] )
					)
		  );
	if( strncmp( m_cIniManager.GetKeyName( nSect, nCurrKey )
					, EFF_KEY[ eEffKey_effMFrm_type ][e_key]
					, strlen( EFF_KEY[ eEffKey_effMFrm_type ][e_key] )
					)
					)
	{
		Eff2Ut_ERR( "!strncmp" );
		return FALSE;
	}
	pOut->bSetMFrmType( 
		static_cast<AgcdEffMFrm::E_MFRM_TYPE>
		( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_type ][e_key] ) ) );

	//eEffKey_effMFrm_shake_axis
	pOut->bGetRefShakeFrm().bSetOscillationAxis(
		static_cast<AgcdEffMFrm::eOscillationAxis>
		( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_shake_axis ][e_key] ) ) );

	//eEffKey_effMFrm_shake_whose
	pOut->bGetRefShakeFrm().bSetWhose(
		static_cast<AgcdEffMFrm::eWhoseFrame>
		( m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_shake_whose ][e_key] ) ) );

	//eEffKey_effMFrm_shake_Amplitude
	pOut->bGetRefShakeFrm().bSetAmplitude(
		static_cast<RwReal>( m_cIniManager.GetValueF( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_shake_Amplitude ][e_key]) ) );

	//eEffKey_effMFrm_shake_duration
	pOut->bGetRefShakeFrm().bSetDuration(
		static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_shake_duration ][e_key] ) ) ) );

	//eEffKey_effMFrm_shake_cycle
	pOut->bGetRefShakeFrm().bSetCycle(
		static_cast<RwReal>( m_cIniManager.GetValueF( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effMFrm_shake_cycle ][e_key] ) ) );
	
	//EffAnim	
	++nCurrKey;
	for( RwInt32 i=0; i<pOut->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		//ASSERT( pOut->bGetMFrmType() == AgcdEffMFrm::EMFRM_SPLINE );

		br	= _Read_EffAnim_Base_txt( static_cast<AgcdEffBase*>(pOut), nSect, &nCurrKey, i );
		if( !br )
		{
			Eff2Ut_ERR( "vRead_EffAnim_Base_txt failed @ AgcuEffIniMng::vRead_EffPSysSBH_txt" );
			return br;
		}
	}

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Base_txt( AgcdEffBase* pOut
										  , RwInt32 nSect
										  , RwInt32* pCurrKey
										  , RwInt32 nIndex )
{
	CONST CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );
	CHAR			szTypeName[ MAX_PATH ];
	CHAR			szFlagName[ MAX_PATH ];
	CHAR			szLifeName[ MAX_PATH ];
	CHAR			szLoopName[ MAX_PATH ];

	sprintf_s( szTypeName , sizeof(szTypeName), "%s:%d" , EFF_KEY[ eEffKey_effAnim_type ][e_key] , nIndex );
	sprintf_s( szFlagName , sizeof(szFlagName), "%s:%d" , EFF_KEY[ eEffKey_effAnim_flag ][e_key] , nIndex );
	sprintf_s( szLifeName , sizeof(szLifeName), "%s:%d" , EFF_KEY[ eEffKey_effAnim_life ][e_key] , nIndex );
	sprintf_s( szLoopName , sizeof(szLoopName), "%s:%d" , EFF_KEY[ eEffKey_effAnim_loopOpt ][e_key] , nIndex );

	BOOL	br	= TRUE;
	stAnimBaseInfo	stBaseInfo;

	stBaseInfo.eEffAnimType	= 
		static_cast< AgcdEffAnim::E_EFFANIMTYPE >( m_cIniManager.GetValueI( szSectionName, szTypeName ) );
	if( EOF == sscanf( m_cIniManager.GetValue( szSectionName, szFlagName )
					 , "%x"
					 , &stBaseInfo.dwBitFlags
					 ) )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Base_txt" );
		return FALSE;
	}
	stBaseInfo.dwLifeTime		= 
		static_cast<RwUInt32>( atol( m_cIniManager.GetValue( szSectionName, szLifeName ) ) );
	stBaseInfo.eLoopOpt		= 
		static_cast<E_LOOPOPT>( atoi( m_cIniManager.GetValue( szSectionName, szLoopName ) ) );

	switch( stBaseInfo.eEffAnimType )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR:
		{
			br	= _Read_EffAnim_Colr_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_TUTV:
		{
			br	= _Read_EffAnim_TuTv_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_MISSILE:
		{
			br	= _Read_EffAnim_Missile_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_LINEAR:
		{
			br	= _Read_EffAnim_Linear_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION:
		{
			br	= _Read_EffAnim_Rev_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_ROTATION:
		{
			br	= _Read_EffAnim_Rot_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_RPSPLINE:
		{
			br	= _Read_EffAnim_RpSpline_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_RTANIM:
		{
			br	= _Read_EffAnim_RtAnim_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	case AgcdEffAnim::E_EFFANIM_SCALE:
		{
			br	= _Read_EffAnim_Scale_txt( pOut, nSect, pCurrKey, stBaseInfo );
		}break;
	}

	if( !br	)
	{
		Eff2Ut_ERR( "@ vRead_EffAnim_Base_txt" );
	}

	*pCurrKey +=1;

	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Colr_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{
	CONST CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	nSize		= atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimColr_num ][e_key] ) );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );
		//return FALSE;
	}

	RwInt32	ir	= 0;

	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;

	ir	= stCreateParam_effAnim.bSetForColr(  stAnimBaseInfo.dwBitFlags
											, stAnimBaseInfo.dwLifeTime
											, stAnimBaseInfo.eLoopOpt
											, nSize
											, NULL
											);
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForColr failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );		
		return FALSE;
	}

	RwInt32		i		= 0;
	RwUInt32	dwTime	= 0;
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szAnimColr[ MAX_PATH ];
	int	nr=0,ng=0,nb=0,na=0;
	for( i=0; i<nSize; ++i )
	{	
		sprintf_s( szAnimColr , sizeof(szAnimColr), "%s:%d" , EFF_KEY[ eEffKey_effAnimColr ][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( szSectionName, szAnimColr )
					, "%d:%d:%d:%d:%d"
					, &dwTime
					, &nr
					, &ng
					, &nb
					, &na
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );
			return FALSE;
		}
		RwRGBA	rgba = {nr,ng,nb,na};

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>( &rgba ) );		
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_TuTv_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{
	CONST CHAR*		szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	nSize		= atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimTuTv_num ][e_key] ) );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
		//return FALSE;
	}

	RwInt32	ir	= 0;
	
	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForTuTv(  stAnimBaseInfo.dwBitFlags
											, stAnimBaseInfo.dwLifeTime
											, stAnimBaseInfo.eLoopOpt
											, nSize
											, NULL
											);
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForTuTv failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );		
		return FALSE;
	}
	
	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Colr_txt" );		
		return FALSE;
	}


	RwInt32		i			= 0;
	RwUInt32	dwTime		= 0;
	STUVRECT	StUvRect;
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szAnimTuTvRect[ MAX_PATH ];
	for( i=0; i<nSize; ++i )
	{

		sprintf_s( szAnimTuTvRect , sizeof(szAnimTuTvRect), "%s:%d" , EFF_KEY[ eEffKey_effAnimTuTvRect ][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( szSectionName, szAnimTuTvRect )
					, "%d:%f:%f:%f:%f"
					, &dwTime
					, &StUvRect.m_fLeft
					, &StUvRect.m_fTop
					, &StUvRect.m_fRight
					, &StUvRect.m_fBottom
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>( &StUvRect ) );		
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgcuEffIniMng::_Read_EffAnim_PostFX_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{

	CONST CHAR*			szSectionName		=	m_cIniManager.GetSectionName( nSect );

	RwInt32	nSize		= atol( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimTuTv_num ][e_key] ) );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
		//return FALSE;
	}

	RwInt32	ir	= 0;

	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForPostFX(  stAnimBaseInfo.dwBitFlags
												, stAnimBaseInfo.dwLifeTime
												, stAnimBaseInfo.eLoopOpt
												, nSize
												, NULL
												);
	if( ir < 0 )
	{
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		return FALSE;
	}


	RwInt32		i			= 0;
	RwUInt32	dwTime		= 0;
	STUVRECT	StUvRect;
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szAnimTuTvRect[ MAX_PATH ];
	for( i=0; i<nSize; ++i )
	{
		sprintf_s( szAnimTuTvRect , sizeof(szAnimTuTvRect), "%s:%d" , EFF_KEY[ eEffKey_effAnimTuTvRect ][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( nSect, ( *pCurrKey += 1 ) )
			, "%d:%f:%f:%f:%f"
			, &dwTime
			, &StUvRect.m_fLeft
			, &StUvRect.m_fTop
			, &StUvRect.m_fRight
			, &StUvRect.m_fBottom
			);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>( &StUvRect ) );		
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_TuTv_txt" );
			return FALSE;
		}
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Missile_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{
	
	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwReal	fAccel	=	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_accel][e_key]			);
	RwReal	fSpeed	=	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_initSped][e_key]		);
	RwReal	fRotate =	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_rotate][e_key]		);
	RwReal	fRadius =	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_radius][e_key]		);
	RwReal	fZigzag	=	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_zigzaglength][e_key]	);
	RwReal	fMinSpd	=	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_minspeed][e_key]		);
	RwReal	fMaxSpd	=	m_cIniManager.GetValueF(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimMissile_maxspeed][e_key]		);

	RwInt32	ir	= 0;
	
	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForMissile( stAnimBaseInfo.dwBitFlags
											  , stAnimBaseInfo.dwLifeTime
											  , fSpeed
											  , fAccel
											  , fRotate
											  , fRadius
											  , fZigzag
											  , fMinSpd
											  , fMaxSpd
											  );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForMissile failed @ AgcuEffIniMng::vRead_EffAnim_Missile_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Missile_txt" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Linear_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{
	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	nSize	=	m_cIniManager.GetValueI(szSectionName , (CHAR*)EFF_KEY[eEffKey_effAnimLinear_num][e_key]		);
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
		//return FALSE;
	}
	
	RwInt32	ir	= 0;
	
	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForLinear( stAnimBaseInfo.dwBitFlags
											  , stAnimBaseInfo.dwLifeTime
											  , stAnimBaseInfo.eLoopOpt
											  , nSize
											  , NULL
											  );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForLinear failed @ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
		return FALSE;
	}

	RwInt32		i			= 0;
	RwUInt32	dwTime		= 0;
	RwV3d		v3dPos		= {0.f, 0.f, 0.f};
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szLinear_Pos[ MAX_PATH ];

	for( i=0; i<nSize; ++i )
	{
		sprintf_s( szLinear_Pos , sizeof(szLinear_Pos), "%s:%d" , EFF_KEY[eEffKey_effAnimLinear_pos][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( szSectionName , szLinear_Pos  )
					, "%d:%f:%f:%f"
					, &dwTime
					, &v3dPos.x
					, &v3dPos.y
					, &v3dPos.z
					);

		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>(&v3dPos) );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Linear_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Rev_txt( AgcdEffBase* pOut
										 , RwInt32 nSect
										 , RwInt32* pCurrKey
										 , const stAnimBaseInfo& stAnimBaseInfo )
{

	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	ir		= 0;
	RwV3d	vAxis	= { 0.f, 0.f, 0.f };
	ir	= sscanf( m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimRev_axis ][e_key] )
				, "%f:%f:%f"
				, &vAxis.x
				, &vAxis.y
				, &vAxis.z
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
		return FALSE;
	}

	RwInt32	nSize	= m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimRev_num ][e_key] );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
		//return FALSE;
	}


	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForRev( stAnimBaseInfo.dwBitFlags
										, stAnimBaseInfo.dwLifeTime
										, stAnimBaseInfo.eLoopOpt
										, nSize
										, vAxis
										, NULL
										);
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForRev failed @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
		return FALSE;
	}

	RwInt32							i			=0;
	RwUInt32						dwTime		= 0;
	AgcdEffAnim_Rev::STREVOLUTION	stRev;
	RwInt32							nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR							szAnimRecv[ MAX_PATH ];

	for( i=0; i<nSize; ++i )
	{

		sprintf_s( szAnimRecv , sizeof(szAnimRecv), "%s:%d" , EFF_KEY[ eEffKey_effAnimRev_rev ][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( szSectionName, szAnimRecv )
					, "%d:%f:%f:%f"
					, &dwTime
					, &stRev.m_fHeight
					, &stRev.m_fAngle
					, &stRev.m_fRadius
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>(&stRev) );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Rev_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Rot_txt( AgcdEffBase* pOut
										 , RwInt32 nSect
										 , RwInt32* pCurrKey
										 , const stAnimBaseInfo& stAnimBaseInfo )
{

	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	ir		= 0;
	RwV3d	vAxis	= { 0.f, 0.f, 0.f };
	ir	= sscanf( m_cIniManager.GetValue( szSectionName,  (CHAR*)EFF_KEY[ eEffKey_effAnimRot_axis ][e_key] )
				, "%f:%f:%f"
				, &vAxis.x
				, &vAxis.y
				, &vAxis.z
				);
	if( ir == EOF )
	{
		Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
		return FALSE;
	}

	RwInt32	nSize	=  m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimRot_num ][e_key] );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
		//return FALSE;
	}

	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	ir	= stCreateParam_effAnim.bSetForRot( stAnimBaseInfo.dwBitFlags
										, stAnimBaseInfo.dwLifeTime
										, stAnimBaseInfo.eLoopOpt
										, nSize
										, vAxis
										, NULL
										);
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForRev failed @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
		return FALSE;
	}


	RwInt32		i			= 0;
	RwUInt32	dwTime		= 0;
	RwReal		fRot		= 0.f;
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szAnimRot[ MAX_PATH ];
	for( i=0; i<nSize; ++i )
	{

		sprintf_s( szAnimRot , sizeof(szAnimRot), "%s:%d" , EFF_KEY[ eEffKey_effAnimRot_rot ][e_key]  , i );
		ir	= sscanf( m_cIniManager.GetValue( szSectionName, szAnimRot )
					, "%d:%f"
					, &dwTime
					, &fRot
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>(&fRot) );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Rot_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_RpSpline_txt( AgcdEffBase* pOut
											  , RwInt32 nSect
											  , RwInt32* pCurrKey
											  , const stAnimBaseInfo& stAnimBaseInfo )
{

	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	RwInt32	ir	=
	stCreateParam_effAnim.bSetForRpSpline( stAnimBaseInfo.dwBitFlags
										 , stAnimBaseInfo.dwLifeTime
										 , stAnimBaseInfo.eLoopOpt
										 , m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimSpline_file ][e_key] )
										 );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForRpSpline failed @ AgcuEffIniMng::vRead_EffAnim_RpSpline_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_RpSpline_txt" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_RtAnim_txt( AgcdEffBase* pOut
											, RwInt32 nSect
											, RwInt32* pCurrKey
											, const stAnimBaseInfo& stAnimBaseInfo )
{
	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	RwInt32	ir	=
	stCreateParam_effAnim.bSetForRtAnim( stAnimBaseInfo.dwBitFlags
									   , stAnimBaseInfo.dwLifeTime
									   , stAnimBaseInfo.eLoopOpt
									   , m_cIniManager.GetValue( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimRtAnim_file ][e_key] )
									   );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForRtAnim failed @ AgcuEffIniMng::vRead_EffAnim_RtAnim_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_RtAnim_txt" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Read_EffAnim_Scale_txt( AgcdEffBase* pOut
										   , RwInt32 nSect
										   , RwInt32* pCurrKey
										   , const stAnimBaseInfo& stAnimBaseInfo )
{
	CONST CHAR*	szSectionName	=	m_cIniManager.GetSectionName( nSect );

	RwInt32	nSize	= m_cIniManager.GetValueI( szSectionName, (CHAR*)EFF_KEY[ eEffKey_effAnimScale_num ][e_key] );
	ASSERT( nSize >= 1 && "@ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
	if( !nSize )
	{
		Eff2Ut_ERR( "nSize < 1 @ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
		//return FALSE;
	}
	
	STCREATEPARAM_EFFANIM	stCreateParam_effAnim;
	RwInt32	ir	=
	stCreateParam_effAnim.bSetForScale( stAnimBaseInfo.dwBitFlags
									  , stAnimBaseInfo.dwLifeTime
									  , stAnimBaseInfo.eLoopOpt
									  , nSize
									  , NULL
									  );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "stCreateParam_effAnim.bSetForScale failed @ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
		return FALSE;
	}

	ir	= pOut->bInsEffAnim( &stCreateParam_effAnim );
	if( ir < 0 )
	{
		Eff2Ut_ERR( "pOut->bInsEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
		return FALSE;
	}

	RwInt32		i			= 0;
	RwUInt32	dwTime		= 0;
	RwV3d		v3dScale	= {0.f, 0.f, 0.f};
	RwInt32		nEffAnim	= pOut->bGetEffAnimSize() - 1;
	CHAR		szAnimScale[ MAX_PATH ];

	for( i=0; i<nSize; ++i )
	{
		sprintf_s( szAnimScale, sizeof(szAnimScale), "%s:%d" , EFF_KEY[ eEffKey_effAnimScale_scale ][e_key] , i );

		ir	= sscanf( m_cIniManager.GetValue( szSectionName, szAnimScale )
					, "%d:%f:%f:%f"
					, &dwTime
					, &v3dScale.x
					, &v3dScale.y
					, &v3dScale.z
					);
		if( ir == EOF )
		{
			Eff2Ut_ERR( "sscanf failed @ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
			return FALSE;
		}

		ir	= pOut->bEditEffAnim( nEffAnim, i, dwTime, static_cast<void*>( &v3dScale ) );
		if( ir < 0 )
		{
			Eff2Ut_ERR( "pOut->bEditEffAnim failed @ AgcuEffIniMng::vRead_EffAnim_Scale_txt" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::Write_txt(AgcdEffSet* pIn, const RwChar* szPath)
{
	RwChar	szFullPath[MAX_PATH]	= "";
	sprintf( szFullPath, EFFINIFILEFORM, szPath ? szPath : AgcuEffPath::GetPath_Ini(), pIn->bGetID() );

	
	m_cIniManager.SetPath( szFullPath );
	m_cIniManager.ClearAllSectionKey();

	_Write_EffSet_txt( pIn );
	
	if(!m_cIniManager.WriteFile())
	{
		Eff2Ut_ERR( "m_cIniManager.WriteFile failed @ AgcuEffIniMng::bWrite_txt" );
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffSet_txt( AgcdEffSet* pIn )
{
	BOOL	br	= FALSE;

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_id)
		, VALUE(eEffKey_set_id, pIn->bGetID())
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_iniver)
		, VALUE(eEffKey_set_iniver, EFFINIVER )
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_title)
		, VALUE(eEffKey_set_title, pIn->bGetTitle() )
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_life)
		, VALUE(eEffKey_set_life, pIn->bGetLife() )
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_loopOpt)
		, VALUE(eEffKey_set_loopOpt, pIn->bGetLoopOpt() )
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_flag)
		, VALUE(eEffKey_set_flag, pIn->bGetFlag() )
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_missileInfo)
		, VALUE(eEffKey_set_missileInfo
		, pIn->bGetMissileInfo().m_eMissileType
		, pIn->bGetMissileInfo().m_v3dOffset.x
		, pIn->bGetMissileInfo().m_v3dOffset.y
		, pIn->bGetMissileInfo().m_v3dOffset.z 
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_sphere)
		, VALUE(eEffKey_set_sphere
		, pIn->bGetBSphere().center.x
		, pIn->bGetBSphere().center.y
		, pIn->bGetBSphere().center.z
		, pIn->bGetBSphere().radius 
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_box)
		, VALUE(eEffKey_set_box
		, pIn->bGetBBox().sup.x
		, pIn->bGetBBox().sup.y
		, pIn->bGetBBox().sup.z
		, pIn->bGetBBox().inf.x
		, pIn->bGetBBox().inf.y
		, pIn->bGetBBox().inf.z
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
		, KEY(eEffKey_set_varsize)
		, VALUE(eEffKey_set_varsize
		, pIn->bGetVarSizeInfo().m_nNumOfTex
		, pIn->bGetVarSizeInfo().m_nNumOfBase
		, pIn->bGetVarSizeInfo().m_nNumOfBaseDependancy
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	int	i=0;
	for( i=0; i<pIn->bGetVarSizeInfo().m_nNumOfTex; ++i )
	{
		br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
			, KEY(eEffKey_set_efftex, i)
			, VALUE(eEffKey_set_efftex
			, pIn->bGetPtrEffTex(i)->bGetTexName()
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
		
		br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
			, KEY(eEffKey_set_effmask, i)
			, VALUE(eEffKey_set_effmask
			, pIn->bGetPtrEffTex(i)->bGetMaskName()
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	for( i=0; i<pIn->bGetVarSizeInfo().m_nNumOfBaseDependancy; ++i )
	{
		br	= m_cIniManager.SetValue( SECT(eEffSect_Set)
			, KEY(eEffKey_set_depnd, i)
			, VALUE(eEffKey_set_depnd
			, pIn->bGetCRefBaseDpnd(i).GetParentIndexItself()
			, pIn->bGetCRefBaseDpnd(i).GetChildIndex()
			, pIn->bGetCRefBaseDpnd(i).GetFlag()
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	RwChar*	szSect	= NULL;
	for( i=0; i<pIn->bGetVarSizeInfo().m_nNumOfBase; ++i )
	{
		if( !_Write_EffBase_txt( SECT( BASE_SECT( pIn->bGetPtrEffBase(i) ), i ), pIn->bGetPtrEffBase(i) ) )
			return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffBase_txt( RwChar* szSect, AgcdEffBase* pIn )
{
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effbase_title)
		, VALUE(eEffKey_effbase_title, pIn->bGetPtrTitle())
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effbase_base)
		, VALUE(eEffKey_effbase_base
		, pIn->bGetBaseType()
		, pIn->bGetDelay()
		, pIn->bGetLife()
		, pIn->bGetLoopOpt()
		, pIn->bGetFlag()
		, pIn->bGetPtrVarSizeInfo()->m_nNumOfAnim
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	if( pIn->bIsRenderBase() )
		if( !_Write_EffRenderBase_txt( szSect, static_cast<AgcdEffRenderBase*>(pIn) ) )
			return FALSE;

	switch( pIn->bGetBaseType() )
	{
	case AgcdEffBase::E_EFFBASE_BOARD				:	br = _Write_EffBoard_txt	(szSect, static_cast<AgcdEffBoard*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_TAIL				:	br = _Write_EffTail_txt		(szSect, static_cast<AgcdEffTail*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_PSYS				:	br = _Write_EffPSys_txt		(szSect, static_cast<AgcdEffParticleSystem*				>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_PSYS_SIMPLEBLACKHOLE:	br = _Write_EffPSysSBH_txt	(szSect, static_cast<AgcdEffParticleSys_SimpleBlackHole*>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_OBJECT				:	br = _Write_EffObj_txt		(szSect, static_cast<AgcdEffObj*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_LIGHT				:	br = _Write_EffLight_txt	(szSect, static_cast<AgcdEffLight*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_SOUND				:	br = _Write_EffSound_txt	(szSect, static_cast<AgcdEffSound*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_MOVINGFRAME			:	br = _Write_EffMFrm_txt		(szSect, static_cast<AgcdEffMFrm*						>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_TERRAINBOARD		:	br = _Write_EffTerrainB_txt	(szSect, static_cast<AgcdEffTerrainBoard*				>(pIn)); break;
	case AgcdEffBase::E_EFFBASE_POSTFX				:	br = _Write_EffPostFX_txt	(szSect, static_cast<AgcdEffPostFX*						>(pIn)); break;
	}

	for( int i=0; i<pIn->bGetPtrVarSizeInfo()->m_nNumOfAnim; ++i )
	{
		if( !_Write_EffAnim_Base_txt( pIn->bGetPtrEffAnim(i), szSect, i ) )
			return FALSE;
	}
	return br;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffRenderBase_txt( RwChar* szSect, AgcdEffRenderBase* pIn )
{
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effbase_renderbase)
		, VALUE(eEffKey_effbase_renderbase
		, pIn->bGetBlendType()
		, pIn->bGetTexIndex()
		, pIn->bGetInitPos().x
		, pIn->bGetInitPos().y
		, pIn->bGetInitPos().z
		, pIn->bGetInitAngle().m_fPitch
		, pIn->bGetInitAngle().m_fYaw
		, pIn->bGetInitAngle().m_fRoll
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffBoard_txt( RwChar* szSect, AgcdEffBoard* pIn )
{	
	szSect;
	pIn;
	BOOL	br	= FALSE;

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffTerrainB_txt( RwChar* szSect, AgcdEffTerrainBoard* pIn )
{	
	szSect;
	pIn;
	BOOL	br	= FALSE;

	return TRUE;
}
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffPostFX_txt( RwChar* szSect, AgcdEffPostFX* pIn )
{
	BOOL br	= FALSE;
	//sharedparam
	br = m_cIniManager.SetValue( szSect
		, KEY( eEffKey_effPostFX_sharedparam )
		, VALUE( eEffKey_effPostFX_sharedparam
		, pIn->bGetSharedParam().type
		, pIn->bGetSharedParam().center.x
		, pIn->bGetSharedParam().center.y
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

    //unionparam
	switch( pIn->bGetSharedParam().type )
	{
	case e_fx_ClampingCircle: break;
	case e_fx_Wave			:
		{
			br = m_cIniManager.SetValue( szSect
				, KEY( eEffKey_effPostFX_wave )
				, VALUE( eEffKey_effPostFX_wave
				, pIn->bGetWaveParam().amplitude
				, pIn->bGetWaveParam().frequency
				, pIn->bGetWaveParam().tex
				)
				);
			if( !br ){
				Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
				return FALSE;
			}
		}break;
	case e_fx_Shockwave		: 
		{
			br = m_cIniManager.SetValue( szSect
				, KEY( eEffKey_effPostFX_shockwave )
				, VALUE( eEffKey_effPostFX_shockwave
				, pIn->bGetShockwaveParam().bias
				, pIn->bGetShockwaveParam().width
				, pIn->bGetShockwaveParam().tex
				)
				);
			if( !br ){
				Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
				return FALSE;
			}
		}break;
	case e_fx_Darken		: break;
	case e_fx_Brighten		: break;
	case e_fx_Ripple		: 
		{
			br = m_cIniManager.SetValue( szSect
				, KEY( eEffKey_effPostFX_ripple )
				, VALUE( eEffKey_effPostFX_ripple
				, pIn->bGetRippleParam().scale
				, pIn->bGetRippleParam().frequency
				)
				);
			if( !br ){
				Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
				return FALSE;
			}
		}break;
	case e_fx_Twist			: break;
	default:
		ASSERT( "kday" && !"unknown postfx type" );
		return FALSE;
	}

	return TRUE;
};

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffTail_txt( RwChar* szSect, AgcdEffTail* pIn )
{
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effTail_capacity)
		, VALUE(eEffKey_effTail_capacity
		, pIn->bGetCapacity()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effTail_timegap)
		, VALUE(eEffKey_effTail_timegap
		, pIn->bGetTimeGap()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effTail_pointLife)
		, VALUE(eEffKey_effTail_pointLife
		, pIn->bGetPointLife()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	RwReal	fh1 = 0.f, fh2 = 0.f;
	pIn->bGetHeight(&fh1, &fh2);
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effTail_height)
		, VALUE(eEffKey_effTail_height
		, fh1
		, fh2
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffPSys_txt( RwChar* szSect, AgcdEffParticleSystem* pIn )
{

	//particle system
	//"capacity"				, "%d", 						//eEffKey_effPSys_capacity
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_capacity)
		, VALUE(eEffKey_effPSys_capacity
		, pIn->bGetCapacity()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	//"shootDelay"			, "%d", 						//eEffKey_effPSys_shootDelay
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_shootDelay)
		, VALUE(eEffKey_effPSys_shootDelay
		, pIn->bGetShootDelay()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	//"clumpFile"				, "%s", 						//eEffKey_effPSys_clumpFile
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_clumpFile)
		, VALUE(eEffKey_effPSys_clumpFile
		, pIn->bGetClumpFile()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	////emiter
	//"emt_power"				, "%f:%f", 						//eEffKey_effPSys_emt_power	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_power)
		, VALUE(eEffKey_effPSys_emt_power
		, pIn->bGetCRefEmiter().m_fPower
		, pIn->bGetCRefEmiter().m_fPowerOffset
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_gunLength"			, "%f:%f", 						//eEffKey_effPSys_emt_gunLen
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_gunLen)
		, VALUE(eEffKey_effPSys_emt_gunLen
		, pIn->bGetCRefEmiter().m_fGunLength
		, pIn->bGetCRefEmiter().m_fGunLengthOffset
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"emt_oneShootNum"		, "%d:%d", 						//eEffKey_effPSys_emt_oneshootnum	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_oneshootnum)
		, VALUE(eEffKey_effPSys_emt_oneshootnum
		, pIn->bGetCRefEmiter().m_nNumOfOneShoot
		, pIn->bGetCRefEmiter().m_nNumOfOneShootOffset
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"emt_omegaYawPitch"		, "%f:%f", 						//eEffKey_effPSys_emt_omegaYawPitch
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_omegaYawPitch)
		, VALUE(eEffKey_effPSys_emt_omegaYawPitch
		, pIn->bGetCRefEmiter().m_fOmegaYaw_World
		, pIn->bGetCRefEmiter().m_fOmegaPitch_Local
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"emt_yawMinMax"			, "%f:%f", 						//eEffKey_effPSys_emt_yawMinMax	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_yawMinMax)
		, VALUE(eEffKey_effPSys_emt_yawMinMax
		, pIn->bGetCRefEmiter().m_fMinYaw
		, pIn->bGetCRefEmiter().m_fMaxYaw
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"emt_pitchMinMax"		, "%f:%f", 						//eEffKey_effPSys_emt_pitchMinMax	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_pitchMinMax)
		, VALUE(eEffKey_effPSys_emt_pitchMinMax
		, pIn->bGetCRefEmiter().m_fMinPitch
		, pIn->bGetCRefEmiter().m_fMaxPitch
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_initDirection"		, "%f:%f:%f",					//eEffKey_effPSys_emt_initDir		
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_initDir)
		, VALUE(eEffKey_effPSys_emt_initDir
		, pIn->bGetCRefEmiter().m_vDir.x
		, pIn->bGetCRefEmiter().m_vDir.y
		, pIn->bGetCRefEmiter().m_vDir.z
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_conAngle"			, "%f", 						//eEffKey_effPSys_emt_conAngle			
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_conAngle)
		, VALUE(eEffKey_effPSys_emt_conAngle
		, pIn->bGetCRefEmiter().m_fConAngle
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_flag"				, "%x", 						//eEffKey_effPSys_emt_flag		
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_flag)
		, VALUE(eEffKey_effPSys_emt_flag
		, pIn->bGetCRefEmiter().m_dwFlagOfEmiter
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_ePGroup"			, "%d", 						//eEffKey_effPSys_emt_ePGroup
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_ePGroup)
		, VALUE(eEffKey_effPSys_emt_ePGroup
		, pIn->bGetCRefEmiter().m_ePGroup
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"emt_PG_box"			, "%f:%f:%f",					//eEffKey_effPSys_emt_pgBox
	//"emt_PG_cylinder"		, "%f:%f",						//eEffKey_effPSys_emt_pgCylinder
	//"emt_PG_sphere"			, "%f",							//eEffKey_effPSys_emt_pgSphere
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_emt_pgBox)
		, VALUE(eEffKey_effPSys_emt_pgBox
		, pIn->bGetCRefEmiter().m_pgroupBox.m_fHWidth
		, pIn->bGetCRefEmiter().m_pgroupBox.m_fHHeight
		, pIn->bGetCRefEmiter().m_pgroupBox.m_fHDepth
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	////particle property
	//"pProp_omega"			, "%f:%f",						//eEffKey_effPSys_pProp_omega	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_pProp_omega)
		, VALUE(eEffKey_effPSys_pProp_omega
		, pIn->bGetCRefPProp().m_fPAngularspeed
		, pIn->bGetCRefPProp().m_fPAngularspeedOffset
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}	
	//"pProp_life"			, "%d:%d",						//eEffKey_effPSys_pProp_life	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_pProp_life)
		, VALUE(eEffKey_effPSys_pProp_life
		, pIn->bGetCRefPProp().m_dwParticleLife
		, pIn->bGetCRefPProp().m_dwParticleLifeOffset
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"pProp_cofEnvirenment"	, "%f:%f:%f:%f",				//eEffKey_effPSys_pProp_cofEnvrn	
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_pProp_cofEnvrn)
		, VALUE(eEffKey_effPSys_pProp_cofEnvrn
		, pIn->bGetCRefPProp().m_stCofEnvrn.m_fCofGrav
		, pIn->bGetCRefPProp().m_stCofEnvrn.m_fCofAirResistance
		, pIn->bGetCRefPProp().m_stCofEnvrnOffset.m_fCofGrav
		, pIn->bGetCRefPProp().m_stCofEnvrnOffset.m_fCofAirResistance
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	//"pProp_flag"			, "%x",							//eEffKey_effPSys_pProp_flag
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSys_pProp_flag)
		, VALUE(eEffKey_effPSys_pProp_flag
		, pIn->bGetCRefPProp().m_dwFlagOfParticle
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}		
	
	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffPSysSBH_txt( RwChar* szSect, AgcdEffParticleSys_SimpleBlackHole* pIn )
{
	//particle system simple black hole
	//eEffKey_effPSysSBH_capacity
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_capacity)
		, VALUE(eEffKey_effPSysSBH_capacity
		, pIn->bGetCapacity()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_numOneShoot
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_numOneShoot)
		, VALUE(eEffKey_effPSysSBH_numOneShoot
		, pIn->bGetOneShootNum()
		, pIn->bGetOneShootNumOffset()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_shootDelay
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_shootDelay)
		, VALUE(eEffKey_effPSysSBH_shootDelay
		, pIn->bGetShootDelay()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_particleLife
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_particleLife)
		, VALUE(eEffKey_effPSysSBH_particleLife
		, pIn->bGetParticleLife()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_initspeed
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_initspeed)
		, VALUE(eEffKey_effPSysSBH_initspeed
		, pIn->bGetInitSpeed()
		, pIn->bGetInitSpeedOffset()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_rollRange
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_rollRange)
		, VALUE(eEffKey_effPSysSBH_rollRange
		, pIn->bGetRollMin()
		, pIn->bGetRollMax()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//eEffKey_effPSysSBH_radius
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effPSysSBH_radius)
		, VALUE(eEffKey_effPSysSBH_radius
		, pIn->bGetRadius()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffObj_txt( RwChar* szSect, AgcdEffObj* pIn )
{
	//object
	//"ClumpFile"				, "%s",							//eEffKey_effObj_clumpFile
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effObj_clumpFile)
		, VALUE(eEffKey_effObj_clumpFile
		, pIn->bGetClumpName()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"PreLitColr"			, "%d:%d:%d:%d",				//eEffKey_effObj_Prelit
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effObj_Prelit)
		, VALUE(eEffKey_effObj_Prelit
		, pIn->bGetPreLit().red
		, pIn->bGetPreLit().green
		, pIn->bGetPreLit().blue
		, pIn->bGetPreLit().alpha
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffLight_txt( RwChar* szSect, AgcdEffLight* pIn )
{
	//light
	//"LightType"				, "%d",							//eEffKey_effLight_type
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_type)
		, VALUE(eEffKey_effLight_type
		, pIn->bGetLightType()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"pos"					, "%f:%f:%f",					//eEffKey_effLight_pos
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_pos)
		, VALUE(eEffKey_effLight_pos
		, pIn->bGetCRefPos().x
		, pIn->bGetCRefPos().y
		, pIn->bGetCRefPos().z
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"rotAngle"				, "%f:%f:%f",					//eEffKey_effLight_angle
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_angle)
		, VALUE(eEffKey_effLight_angle
		, pIn->bGetCRefAngle().m_fYaw
		, pIn->bGetCRefAngle().m_fPitch
		, pIn->bGetCRefAngle().m_fRoll
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"conAngle"				, "%f",							//eEffKey_effLight_conAngle
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_conAngle)
		, VALUE(eEffKey_effLight_conAngle
		, pIn->bGetConAngle()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"surfaceProp"			, "%f:%f:%f",					//eEffKey_effLight_surfProp
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_surfProp)
		, VALUE(eEffKey_effLight_surfProp
		, pIn->bGetCRefSurfProp().ambient
		, pIn->bGetCRefSurfProp().specular
		, pIn->bGetCRefSurfProp().diffuse
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"materialColr"			, "%d:%d:%d:%d",				//eEffKey_effLight_matColr
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effLight_matColr)
		, VALUE(eEffKey_effLight_matColr
		, pIn->bGetMaterialColr().red
		, pIn->bGetMaterialColr().green
		, pIn->bGetMaterialColr().blue
		, pIn->bGetMaterialColr().alpha
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffSound_txt( RwChar* szSect, AgcdEffSound* pIn )
{
	//sound
	//"SoundType"				, "%d", 						//eEffKey_effSound_type
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effSound_type)
		, VALUE(eEffKey_effSound_type
		, pIn->bGetSoundType()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"File"					, "%s", 						//eEffKey_effSound_file
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effSound_file)
		, VALUE(eEffKey_effSound_file
		, pIn->bGetSoundFName()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"MonoFile"				, "%s", 						//eEffKey_effSound_monofile
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effSound_monofile)
		, VALUE(eEffKey_effSound_monofile
		, pIn->bGetMonoFName()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"loopCnt"				, "%d", 						//eEffKey_effSound_loopCnt
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effSound_loopCnt)
		, VALUE(eEffKey_effSound_loopCnt
		, pIn->bGetLoopCnt()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"volume"				, "%f", 						//eEffKey_effSound_volume
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effSound_volume)
		, VALUE(eEffKey_effSound_volume
		, pIn->bGetVolume()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffMFrm_txt( RwChar* szSect, AgcdEffMFrm* pIn )
{
	//moving frame
	//"MFrmType"				, "%d", 						//eEffKey_effMFrm_type
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_type)
		, VALUE(eEffKey_effMFrm_type
		, pIn->bGetMFrmType()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"ShakeFrm_Axis"			, "%d", 						//eEffKey_effMFrm_shake_axis
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_shake_axis)
		, VALUE(eEffKey_effMFrm_shake_axis
		, pIn->bGetRefShakeFrm().bGetOscillationAxis()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"ShakeFrm_Whose"		, "%d", 						//eEffKey_effMFrm_shake_whose
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_shake_whose)
		, VALUE(eEffKey_effMFrm_shake_whose
		, pIn->bGetRefShakeFrm().bGetWhose()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"ShakeFrm_Amplitude"	, "%f", 						//eEffKey_effMFrm_shake_Amplitude
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_shake_Amplitude)
		, VALUE(eEffKey_effMFrm_shake_Amplitude
		, pIn->bGetRefShakeFrm().bGetAmplitude()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"ShakeFrm_Duration"		, "%d", 						//eEffKey_effMFrm_shake_duration
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_shake_duration)
		, VALUE(eEffKey_effMFrm_shake_duration
		, pIn->bGetRefShakeFrm().bGetDuration()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}
	//"ShakeFrm_Cycle"		, "%f", 						//eEffKey_effMFrm_shake_cycle
	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effMFrm_shake_cycle)
		, VALUE(eEffKey_effMFrm_shake_cycle
		, pIn->bGetRefShakeFrm().bGetCycle()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Base_txt( AgcdEffAnim* pIn
											, RwChar* szSect
											, RwInt32 nIndex)
{
	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnim_type, nIndex)
		, VALUE(eEffKey_effAnim_type
		, pIn->bGetAnimType()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnim_flag, nIndex)
		, VALUE(eEffKey_effAnim_flag
		, pIn->bGetFlag()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnim_life, nIndex)
		, VALUE(eEffKey_effAnim_life
		, pIn->bGetLife()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnim_loopOpt, nIndex)
		, VALUE(eEffKey_effAnim_loopOpt
		, pIn->bGetLoopOpt()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	switch( pIn->bGetAnimType() )
	{
	case AgcdEffAnim::E_EFFANIM_COLOR		:	_Write_EffAnim_Colr_txt		(static_cast<AgcdEffAnim_Colr*		>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_TUTV		:	_Write_EffAnim_TuTv_txt		(static_cast<AgcdEffAnim_TuTv*		>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_MISSILE		:	_Write_EffAnim_Missile_txt	(static_cast<AgcdEffAnim_Missile*	>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_LINEAR		:	_Write_EffAnim_Linear_txt	(static_cast<AgcdEffAnim_Linear*	>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_REVOLUTION	:	_Write_EffAnim_Rev_txt		(static_cast<AgcdEffAnim_Rev*		>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_ROTATION	:	_Write_EffAnim_Rot_txt		(static_cast<AgcdEffAnim_Rot*		>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_RPSPLINE	:	_Write_EffAnim_RpSpline_txt	(static_cast<AgcdEffAnim_RpSpline*	>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_RTANIM		:	_Write_EffAnim_RtAnim_txt	(static_cast<AgcdEffAnim_RtAnim*	>(pIn), szSect); break;
	case AgcdEffAnim::E_EFFANIM_SCALE		:	_Write_EffAnim_Scale_txt	(static_cast<AgcdEffAnim_Scale*		>(pIn), szSect); break;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Colr_txt( AgcdEffAnim_Colr* pIn
											, RwChar* szSect )
{	
	if( !pIn->m_stTblColr.bForTool_GetNum() )
	{
		Eff2Ut_ERR( "pIn->m_stTblColr.bForTool_GetNum() < 1 @ AgcuEffIniMng::vWrite_EffAnim_Colr_txt" );
		//return FALSE;
	}

	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimColr_num)
		, VALUE(eEffKey_effAnimColr_num
		, pIn->m_stTblColr.bForTool_GetNum()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	for( int i=0; i<pIn->m_stTblColr.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimColr, i)
			, VALUE(eEffKey_effAnimColr
			, pIn->m_stTblColr.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblColr.bForTool_GetTbl()[i].m_tVal.red
			, pIn->m_stTblColr.bForTool_GetTbl()[i].m_tVal.green
			, pIn->m_stTblColr.bForTool_GetTbl()[i].m_tVal.blue
			, pIn->m_stTblColr.bForTool_GetTbl()[i].m_tVal.alpha
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}
											//pIn->bForTool_GetTTbl()
	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_TuTv_txt( AgcdEffAnim_TuTv* pIn
											, RwChar* szSect )
{
	if( !pIn->m_stTblRect.bForTool_GetNum() )
	{
		Eff2Ut_ERR( "pIn->m_stTblRect.bForTool_GetNum() < 1 @ AgcuEffIniMng::vWrite_EffAnim_TuTv_txt" );
		//return FALSE;
	}

	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimTuTv_num)
		, VALUE(eEffKey_effAnimTuTv_num
		, pIn->m_stTblRect.bForTool_GetNum()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	for( int i=0; i<pIn->m_stTblRect.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimTuTvRect, i)
			, VALUE(eEffKey_effAnimTuTvRect
			, pIn->m_stTblRect.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblRect.bForTool_GetTbl()[i].m_tVal.m_fLeft
			, pIn->m_stTblRect.bForTool_GetTbl()[i].m_tVal.m_fTop
			, pIn->m_stTblRect.bForTool_GetTbl()[i].m_tVal.m_fRight
			, pIn->m_stTblRect.bForTool_GetTbl()[i].m_tVal.m_fBottom
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Missile_txt( AgcdEffAnim_Missile* pIn
											 , RwChar* szSect )
{
	BOOL br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_initSped) , VALUE(eEffKey_effAnimMissile_initSped , pIn->m_fSpeed0) );
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_accel) , VALUE(eEffKey_effAnimMissile_accel , pIn->m_fAccel)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_zigzaglength) , VALUE(eEffKey_effAnimMissile_zigzaglength , pIn->m_fZigzagLength)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_rotate) , VALUE(eEffKey_effAnimMissile_rotate , pIn->m_fRotate)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_radius) , VALUE(eEffKey_effAnimMissile_radius , pIn->m_fRadius)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_minspeed) , VALUE(eEffKey_effAnimMissile_minspeed , pIn->m_fMinSpeed)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect , KEY(eEffKey_effAnimMissile_maxspeed) , VALUE(eEffKey_effAnimMissile_maxspeed , pIn->m_fMaxSpeed)	);
	if( !br )
	{
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Linear_txt( AgcdEffAnim_Linear* pIn
											 , RwChar* szSect )
{
	if( !pIn->m_stTblPos.bForTool_GetNum() )
	{
		Eff2Ut_ERR( "pIn->m_stTblPos.bForTool_GetNum() < 1 @ AgcuEffIniMng::vWrite_EffAnim_Linear_txt" );
		//return FALSE;
	}

	BOOL br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimLinear_num)
		, VALUE(eEffKey_effAnimLinear_num
		, pIn->m_stTblPos.bForTool_GetNum()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	for( int i=0; i<pIn->m_stTblPos.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimLinear_pos, i)
			, VALUE(eEffKey_effAnimLinear_pos
			, pIn->m_stTblPos.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblPos.bForTool_GetTbl()[i].m_tVal.x
			, pIn->m_stTblPos.bForTool_GetTbl()[i].m_tVal.y
			, pIn->m_stTblPos.bForTool_GetTbl()[i].m_tVal.z
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Rev_txt( AgcdEffAnim_Rev* pIn
											 , RwChar* szSect )
{
	if( !pIn->m_stTblRev.bForTool_GetNum() )
	{
		Eff2Ut_ERR( "pIn->m_stTblRev.bForTool_GetNum() < 1 @ AgcuEffIniMng::vWrite_EffAnim_Rev_txt" );
		//return FALSE;
	}

	BOOL br = m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimRev_axis)
		, VALUE(eEffKey_effAnimRev_axis
		, pIn->m_v3dRotAxis.x
		, pIn->m_v3dRotAxis.y
		, pIn->m_v3dRotAxis.z
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimRev_num)
		, VALUE(eEffKey_effAnimRev_num
		, pIn->m_stTblRev.bForTool_GetNum()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	for( int i=0; i<pIn->m_stTblRev.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimRev_rev, i)
			, VALUE(eEffKey_effAnimRev_rev
			, pIn->m_stTblRev.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblRev.bForTool_GetTbl()[i].m_tVal.m_fHeight
			, pIn->m_stTblRev.bForTool_GetTbl()[i].m_tVal.m_fAngle
			, pIn->m_stTblRev.bForTool_GetTbl()[i].m_tVal.m_fRadius
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Rot_txt( AgcdEffAnim_Rot* pIn
											 , RwChar* szSect )
{
	if( !pIn->m_stTblDeg.bForTool_GetNum() )
	{
		Eff2Ut_ERR( "pIn->m_stTblDeg.bForTool_GetNum() < 1 @ AgcuEffIniMng::vWrite_EffAnim_Rot_txt" );
		//return FALSE;
	}

	BOOL br = m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimRot_axis)
		, VALUE(eEffKey_effAnimRot_axis
		, pIn->m_v3dRotAxis.x
		, pIn->m_v3dRotAxis.y
		, pIn->m_v3dRotAxis.z
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	br	= m_cIniManager.SetValue( szSect
		, KEY(eEffKey_effAnimRot_num)
		, VALUE(eEffKey_effAnimRot_num
		, pIn->m_stTblDeg.bForTool_GetNum()
		)
		);
	if( !br ){
		Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
		return FALSE;
	}

	for( int i=0; i<pIn->m_stTblDeg.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimRot_rot, i)
			, VALUE(eEffKey_effAnimRot_rot
			, pIn->m_stTblDeg.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblDeg.bForTool_GetTbl()[i].m_tVal
			)
			);
		if( !br ){
			Eff2Ut_ERR( "m_cIniManager.SetValue failed" );
			return FALSE;
		}
	}

	return TRUE;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_RpSpline_txt( AgcdEffAnim_RpSpline* pIn
											 , RwChar* szSect )
{
	return  m_cIniManager.SetValue( szSect
									, KEY(eEffKey_effAnimSpline_file)
									, VALUE(eEffKey_effAnimSpline_file , pIn->m_szSplineFName)
									);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_RtAnim_txt( AgcdEffAnim_RtAnim* pIn , RwChar* szSect )
{
	return m_cIniManager.SetValue( szSect
									, KEY(eEffKey_effAnimRtAnim_file)
									, VALUE(eEffKey_effAnimRtAnim_file	, pIn->m_szAnimFName ) 
									);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
BOOL AgcuEffIniMng::_Write_EffAnim_Scale_txt( AgcdEffAnim_Scale* pIn
											 , RwChar* szSect )
{

	BOOL br	= m_cIniManager.SetValue( szSect
									, KEY(eEffKey_effAnimScale_num)
									, VALUE(eEffKey_effAnimScale_num , pIn->m_stTblScale.bForTool_GetNum() )
									);
	if( !br )
		return FALSE;
	
	for( int i=0; i<pIn->m_stTblScale.bForTool_GetNum(); ++i )
	{
		br	= m_cIniManager.SetValue( szSect
			, KEY(eEffKey_effAnimScale_scale, i)
			, VALUE(eEffKey_effAnimScale_scale
			, pIn->m_stTblScale.bForTool_GetTbl()[i].m_dwTime
			, pIn->m_stTblScale.bForTool_GetTbl()[i].m_tVal.x
			, pIn->m_stTblScale.bForTool_GetTbl()[i].m_tVal.y
			, pIn->m_stTblScale.bForTool_GetTbl()[i].m_tVal.z
			)
			);
		if( !br )
			return FALSE;
		
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AgcuEffIniExporter::AgcuEffIniExporter()
	: m_nTex(0)
	, m_nMask(0)
	, m_nClump(0)
	, m_nAnim(0)
	, m_nSpline(0)
	, m_pEffSet(NULL)
{
};

AgcuEffIniExporter::~AgcuEffIniExporter()
{
	vClear();
};

RwInt32 AgcuEffIniExporter::bSetEffSet(RwUInt32 ulEffID)
{
	vClear();

	m_pEffSet = AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->ForTool_FindEffSet(ulEffID);
	if( !m_pEffSet )
	{
		ASSERT(!"THE EFFID IS NOT EXIST");
		return -1;
	}

	if( T_ISMINUS4(m_pEffSet->bAddRef()) )
	{
		m_pEffSet = 0;
		//ASSERT(!"m_pEffSet->bAddRef() failed");
		return -1;
	}
	
	vGetTex();
	vGetMask();
	vGetClump();
	vGetAnim();
	vGetSpline();

	return 0;
};

void AgcuEffIniExporter::vClear()
{
	m_nTex		= 0;
	m_nMask		= 0;
	m_nClump	= 0;
	m_nAnim		= 0;
	m_nSpline	= 0;

	m_vecTex.clear();
	m_vecMask.clear();
	m_vecClump.clear();
	m_vecAnim.clear();
	m_vecSpline.clear();

	if( m_pEffSet )
	{
		m_pEffSet->bRelease();
		m_pEffSet = NULL;
	}
}

void AgcuEffIniExporter::vGetTex()
{
	ASSERT( m_pEffSet );

	m_nTex = m_pEffSet->bGetVarSizeInfo().m_nNumOfTex;
	m_vecTex.reserve(m_nTex);
	for( int i=0; i<m_nTex; ++i )
	{
		LPEFFTEX pTex = m_pEffSet->bGetPtrEffTex(i);
		ASSERT(pTex);
		char*	szTex = pTex->bGetTexName();
		ASSERT( szTex );
		m_vecTex.push_back( szTex );
	}
}

void AgcuEffIniExporter::vGetMask()
{
	ASSERT( m_pEffSet );

	m_nMask = m_pEffSet->bGetVarSizeInfo().m_nNumOfTex;
	m_vecMask.reserve(m_nMask);
	for( int i=0; i<m_nMask; ++i )
	{
		LPEFFTEX pTex = m_pEffSet->bGetPtrEffTex(i);
		ASSERT(pTex);
		char*	szMask = pTex->bGetMaskName();
		ASSERT( szMask );
		m_vecMask.push_back( szMask );
	}
}

void AgcuEffIniExporter::vGetClump()
{
	ASSERT( m_pEffSet );

	INT nBase = m_pEffSet->bGetVarSizeInfo().m_nNumOfBase;
	for( int i=0; i<nBase; ++i )
	{
		LPEFFBASE pBase = m_pEffSet->bGetPtrEffBase(i);
		ASSERT( pBase );
		if( pBase->bGetBaseType() != AgcdEffBase::E_EFFBASE_OBJECT )
			continue;

		AgcdEffObj* pBaseObj = static_cast<AgcdEffObj*>(pBase);
		if( pBaseObj->bFlagChk(FLAG_EFFBASEOBJ_DUMMY) )
			continue;
		char* szClump = pBaseObj->bGetClumpName();
		ASSERT( szClump );
		m_vecClump.push_back(szClump);
	}

	m_nClump = static_cast<int>(m_vecClump.size());
}

void AgcuEffIniExporter::vGetAnim()
{
	ASSERT( m_pEffSet );

	INT nBase = m_pEffSet->bGetVarSizeInfo().m_nNumOfBase;
	for( int i=0; i<nBase; ++i )
	{
		LPEFFBASE pBase = m_pEffSet->bGetPtrEffBase(i);
		ASSERT( pBase );
		if( pBase->bGetBaseType() != AgcdEffBase::E_EFFBASE_OBJECT )
			continue;

		AgcdEffObj* pBaseObj = static_cast<AgcdEffObj*>(pBase);
		ASSERT(pBaseObj);
		if( pBaseObj->bFlagChk(FLAG_EFFBASEOBJ_DUMMY) )
			continue;

		INT nAnim = pBaseObj->bGetPtrVarSizeInfo()->m_nNumOfAnim;
		for( int j=0; j<nAnim; ++j )
		{
			LPEFFANIM pAnim = pBaseObj->bGetPtrEffAnim(j);
			ASSERT( pAnim );
			if( pAnim->bGetAnimType() != AgcdEffAnim::E_EFFANIM_RTANIM )
				continue;
			LPEFFANIM_RTANIM	pRtAnim = static_cast<LPEFFANIM_RTANIM>(pAnim);
			ASSERT( pRtAnim );
			char* szRtAnim =pRtAnim->bGetAnimFName();
			ASSERT( szRtAnim );
			m_vecAnim.push_back(szRtAnim);
		}
	}

	m_nAnim = static_cast<int>(m_vecAnim.size());
}

void AgcuEffIniExporter::vGetSpline()
{
	ASSERT( m_pEffSet );

	INT nBase = m_pEffSet->bGetVarSizeInfo().m_nNumOfBase;
	for( int i=0; i<nBase; ++i )
	{
		LPEFFBASE pBase = m_pEffSet->bGetPtrEffBase(i);
		ASSERT( pBase );
		INT nAnim = pBase->bGetPtrVarSizeInfo()->m_nNumOfAnim;
		for( int j=0; j<nAnim; ++j )
		{
			LPEFFANIM pAnim = pBase->bGetPtrEffAnim(j);
			ASSERT( pAnim );
			if( pAnim->bGetAnimType() != AgcdEffAnim::E_EFFANIM_RPSPLINE )
				continue;
			LPEFFANIM_RPSPLINE	pSpline = static_cast<LPEFFANIM_RPSPLINE>(pAnim);
			ASSERT( pSpline );
			char* szSpline =pSpline->bGetSplineFName();
			ASSERT( szSpline );
			m_vecSpline.push_back(szSpline);
		}
	}

	m_nSpline = static_cast<int>(m_vecSpline.size());
}


RwInt32 AgcuEffIniExporter::bSaveFile(const char* fullName)
{
	ASSERT( m_pEffSet );
	BOOL br = AgcdEffGlobal::bGetInst().bGetPtrEffIniMng()->Write_txt( m_pEffSet, fullName );
	if( !br )
	{
		ASSERT( !"bSaveFile failed");
		return -1;
	}
	return 0;
}