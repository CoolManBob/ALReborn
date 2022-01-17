// AgcuEffIniMng.h: interface for the AgcuEffIniMng class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCUEFFINIMNG_H__B1302EC9_C76C_40B6_9925_CA487B429010__INCLUDED_)
#define AFX_AGCUEFFINIMNG_H__B1302EC9_C76C_40B6_9925_CA487B429010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcdEffBoard.h"
#include "AgcdEffLight.h"
#include "AgcdEffMFrm.h"
#include "AgcdEffObj.h"
#include "AgcdEffParticleSystem.h"
#include "AgcdEffSound.h"
#include "AgcdEffTail.h"
#include "AgcdEffTerrainBoard.h"
#include "AgcdEffPostFX.h"
#include "AgcdEffCamera.h"

#include "AgcdEffSet.h"
#include "AuIniManager.h"
#include <sys/stat.h>


enum eEffSect_Name{
	eEffSect_Set				= 0,

	eEffSect_effbase_board,
	eEffSect_effbase_light,
	eEffSect_effbase_mfrm,
	eEffSect_effbase_obj,
	eEffSect_effbase_sound,
	eEffSect_effbase_tail,
	eEffSect_effbase_psyst,
	eEffSect_effbase_psyst_SBH,
	eEffSect_effbase_terrainb,
	eEffSect_effbase_postfx,
	eEffSect_effbase_camera,
	eEffSect_Num,
};

enum eEffKey_Name
{
	eEffKey_set_id					= 0,
	eEffKey_set_iniver,
	eEffKey_set_title,
	eEffKey_set_life,
	eEffKey_set_loopOpt,
	eEffKey_set_flag,

	eEffKey_set_missileInfo, //20040910 Ãß°¡.

	eEffKey_set_sphere,
	eEffKey_set_box,
	eEffKey_set_varsize,
	eEffKey_set_efftex,
	eEffKey_set_effmask,
	eEffKey_set_depnd,
	
	eEffKey_effbase_title,
	eEffKey_effbase_base,
	eEffKey_effbase_renderbase,
	
	eEffKey_effLight_type,
	eEffKey_effLight_pos,
	eEffKey_effLight_angle,
	eEffKey_effLight_conAngle,
	eEffKey_effLight_surfProp,
	eEffKey_effLight_matColr,
	
	eEffKey_effMFrm_type,
	eEffKey_effMFrm_shake_axis,
	eEffKey_effMFrm_shake_whose,
	eEffKey_effMFrm_shake_Amplitude,
	eEffKey_effMFrm_shake_duration,
	eEffKey_effMFrm_shake_cycle,
	
	eEffKey_effObj_clumpFile,
	eEffKey_effObj_Prelit,
	
	eEffKey_effSound_type,
	eEffKey_effSound_file,
	eEffKey_effSound_monofile,
	eEffKey_effSound_loopCnt,
	eEffKey_effSound_volume,
	
	eEffKey_effTail_capacity,
	eEffKey_effTail_timegap,
	eEffKey_effTail_pointLife,
	eEffKey_effTail_height,
	
	eEffKey_effPSys_capacity,
	eEffKey_effPSys_shootDelay,
	eEffKey_effPSys_clumpFile,

	eEffKey_effPSys_emt_power,
	eEffKey_effPSys_emt_gunLen,
	eEffKey_effPSys_emt_oneshootnum,
	eEffKey_effPSys_emt_omegaYawPitch,
	eEffKey_effPSys_emt_yawMinMax,
	eEffKey_effPSys_emt_pitchMinMax,
	eEffKey_effPSys_emt_initDir,
	eEffKey_effPSys_emt_conAngle,
	eEffKey_effPSys_emt_flag,	
	eEffKey_effPSys_emt_ePGroup,
	eEffKey_effPSys_emt_pgBox,
	eEffKey_effPSys_emt_pgCylinder,
	eEffKey_effPSys_emt_pgSphere,

	eEffKey_effPSys_pProp_omega,
	eEffKey_effPSys_pProp_life,
	eEffKey_effPSys_pProp_cofEnvrn,
	eEffKey_effPSys_pProp_flag,
	
	eEffKey_effPSysSBH_capacity,
	eEffKey_effPSysSBH_numOneShoot,
	eEffKey_effPSysSBH_shootDelay,
	eEffKey_effPSysSBH_particleLife,
	eEffKey_effPSysSBH_initspeed,
	eEffKey_effPSysSBH_rollRange,
	eEffKey_effPSysSBH_radius,
	
	eEffKey_effAnim_type,
	eEffKey_effAnim_flag,
	eEffKey_effAnim_life,
	eEffKey_effAnim_loopOpt,
	
	eEffKey_effAnimColr_num,
	eEffKey_effAnimColr,
	
	eEffKey_effAnimTuTv_num,
	eEffKey_effAnimTuTvRect,
	
	eEffKey_effAnimMissile_initSped,
	eEffKey_effAnimMissile_accel,
	eEffKey_effAnimMissile_rotate,
	eEffKey_effAnimMissile_radius,
	eEffKey_effAnimMissile_zigzaglength,
	eEffKey_effAnimMissile_minspeed,
	eEffKey_effAnimMissile_maxspeed,

	eEffKey_effAnimCamera_type,
	eEffKey_effAnimCamera_pos,
	eEffKey_effAnimCamera_dir,
	eEffKey_effAnimCamera_speed,
	eEffkey_effAnimCamera_rotate,
	eEffKey_effAnimCamera_rotatecount,
	eEffkey_effAnimCamera_movelength,

	eEffKey_effAnimLinear_num,
	eEffKey_effAnimLinear_pos,

	eEffKey_effAnimRev_axis,
	eEffKey_effAnimRev_num,
	eEffKey_effAnimRev_rev,
	
	eEffKey_effAnimRot_axis,
	eEffKey_effAnimRot_num,
	eEffKey_effAnimRot_rot,
	
	eEffKey_effAnimSpline_file,
	
	eEffKey_effAnimRtAnim_file,
	
	eEffKey_effAnimScale_num,
	eEffKey_effAnimScale_scale,

	eEffKey_effPostFX_sharedparam,
	eEffKey_effPostFX_wave,
	eEffKey_effPostFX_shockwave,
	eEffKey_effPostFX_ripple,
	
	eEffKey_Num						,
};

enum e_EffKeyAndForm
{
	e_key	= 0,
	e_form,
	e_EffKeyAndForm_Num,
};

extern const char* EFF_SECT[eEffSect_Num];
extern const char* EFF_KEY[eEffKey_Num][e_EffKeyAndForm_Num];

class AgcuEffIniMng  
{
	struct stAnimBaseInfo
	{		
		AgcdEffAnim::E_EFFANIMTYPE	eEffAnimType;
		RwUInt32					dwBitFlags;
		RwUInt32					dwLifeTime;
		E_LOOPOPT					eLoopOpt;
	};

	AuIniManagerA	m_cIniManager;

public:
	AgcuEffIniMng			( VOID );
	virtual ~AgcuEffIniMng	( VOID );

public:
	BOOL			Read_txt					( const RwChar* szFName, AgcdEffSet* pOut, BOOL bFullRead=FALSE );
	BOOL			Read_txt					( RwUInt32 dwEffSetID, AgcdEffSet* pOut, BOOL bFullRead=FALSE );

	BOOL			Write_txt					( AgcdEffSet* pIn, const RwChar* szPath=NULL );

private:
	eEffSect_Name	_FindSect					( const char* szSecName );
	eEffKey_Name	_FindKey					( eEffSect_Name eSect, const char* szKeyName );

	BOOL			_Read_EffSet_txt			( AgcdEffSet* pOut, BOOL bFullRead );

	BOOL			_Read_EffBase_txt			( RwInt32 nSect, AgcdEffBase* pOut );
	BOOL			_Read_EffRenderBase_txt		( RwInt32 nSect, AgcdEffRenderBase* pOut );

	BOOL			_Read_EffBoard_txt			( RwInt32 nSect, AgcdEffBoard* pOut );
	BOOL			_Read_EffTail_txt			( RwInt32 nSect, AgcdEffTail* pOut );
	BOOL			_Read_EffPSys_txt			( RwInt32 nSect, AgcdEffParticleSystem* pOut );
	BOOL			_Read_EffPSysSBH_txt		( RwInt32 nSect, AgcdEffParticleSys_SimpleBlackHole* pOut );
	BOOL			_Read_EffObj_txt			( RwInt32 nSect, AgcdEffObj* pOut );
	BOOL			_Read_EffLight_txt			( RwInt32 nSect, AgcdEffLight* pOut );
	BOOL			_Read_EffSound_txt			( RwInt32 nSect, AgcdEffSound* pOut );
	BOOL			_Read_EffMFrm_txt			( RwInt32 nSect, AgcdEffMFrm* pOut );
	BOOL			_Read_EffTerrainB_txt		( RwInt32 nSect, AgcdEffTerrainBoard* pOut );
	BOOL			_Read_EffPostFX_txt			( RwInt32 nSect, AgcdEffPostFX* pOut );
	BOOL			_Read_EffCamera_txt			( RwInt32 nSect, AgcdEffCamera* pOut );

	BOOL			_Read_EffAnim_Base_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, RwInt32 nIndex );
	BOOL			_Read_EffAnim_Colr_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_TuTv_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Missile_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Linear_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Rev_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Rot_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_RpSpline_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_RtAnim_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Scale_txt		( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_PostFX_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );
	BOOL			_Read_EffAnim_Camera_txt	( AgcdEffBase* pOut, RwInt32 nSect, RwInt32* pCurrKey, const stAnimBaseInfo& stAnimBaseInfo );

	BOOL			_Write_EffSet_txt			( AgcdEffSet* pIn );

	BOOL			_Write_EffBase_txt			( RwChar* szSect, AgcdEffBase* pIn );
	BOOL			_Write_EffRenderBase_txt	( RwChar* szSect, AgcdEffRenderBase* pIn );

	BOOL			_Write_EffBoard_txt			( RwChar* szSect, AgcdEffBoard* pIn );
	BOOL			_Write_EffTail_txt			( RwChar* szSect, AgcdEffTail* pIn );
	BOOL			_Write_EffPSys_txt			( RwChar* szSect, AgcdEffParticleSystem* pIn );
	BOOL			_Write_EffPSysSBH_txt		( RwChar* szSect, AgcdEffParticleSys_SimpleBlackHole* pIn );
	BOOL			_Write_EffObj_txt			( RwChar* szSect, AgcdEffObj* pIn );
	BOOL			_Write_EffLight_txt			( RwChar* szSect, AgcdEffLight* pIn );
	BOOL			_Write_EffSound_txt			( RwChar* szSect, AgcdEffSound* pIn );
	BOOL			_Write_EffMFrm_txt			( RwChar* szSect, AgcdEffMFrm* pIn );
	BOOL			_Write_EffTerrainB_txt		( RwChar* szSect, AgcdEffTerrainBoard* pIn );
	BOOL			_Write_EffPostFX_txt		( RwChar* szSect, AgcdEffPostFX* pIn );
	BOOL			_Write_EffCamera_txt		( RwChar* szSect, AgcdEffCamera* pIn );

	BOOL			_Write_EffAnim_Base_txt		( AgcdEffAnim*				pIn, RwChar* szSect, RwInt32 nIndex );
	BOOL			_Write_EffAnim_Colr_txt		( AgcdEffAnim_Colr*			pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_TuTv_txt		( AgcdEffAnim_TuTv*			pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Missile_txt	( AgcdEffAnim_Missile*		pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Linear_txt	( AgcdEffAnim_Linear*		pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Rev_txt		( AgcdEffAnim_Rev*			pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Rot_txt		( AgcdEffAnim_Rot*			pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_RpSpline_txt	( AgcdEffAnim_RpSpline*		pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_RtAnim_txt	( AgcdEffAnim_RtAnim*		pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Scale_txt	( AgcdEffAnim_Scale*		pIn, RwChar* szSect );
	BOOL			_Write_EffAnim_Camera_txt	( AgcdEffAnim_Camera*		pIn, RwChar* szSect );
};

class AgcuEffIniExporter
{
	INT			m_nTex;
	INT			m_nMask;
	INT			m_nClump;
	INT			m_nAnim;
	INT			m_nSpline;

	typedef std::vector< char* >				STLVEC_LPCHAR;
	typedef STLVEC_LPCHAR::iterator				STLVEC_LPCHAR_ITR;

	STLVEC_LPCHAR	m_vecTex;
	STLVEC_LPCHAR	m_vecMask;
	STLVEC_LPCHAR	m_vecClump;
	STLVEC_LPCHAR	m_vecAnim;
	STLVEC_LPCHAR	m_vecSpline;
	
	AgcdEffSet*	m_pEffSet;

public:
	AgcuEffIniExporter		( VOID );
	~AgcuEffIniExporter		( VOID );

public:
	RwInt32			bSetEffSet		( RwUInt32 ulEffID );
	RwInt32			bSetEffSet		( AgcdEffSet* pEffSet );

	RwInt32			bGetNumTex		( VOID )const		{ return m_nTex; };
	RwInt32			bGetNumMask		( VOID )const		{ return m_nMask; };
	RwInt32			bGetNumClump	( VOID )const		{ return m_nClump; };
	RwInt32			bGetNumAnim		( VOID )const		{ return m_nAnim; };
	RwInt32			bGetNumSpline	( VOID )const		{ return m_nSpline; };

	CHAR*			bGetSzTex		( int index )		{ return m_vecTex[index]; };
	CHAR*			bGetSzMask		( int index )		{ return m_vecMask[index]; };
	CHAR*			bGetSzClump		( int index )		{ return m_vecClump[index]; };
	CHAR*			bGetSzAnim		( int index )		{ return m_vecAnim[index]; };
	CHAR*			bGetSzSpline	( int index )		{ return m_vecSpline[index]; };

	INT32			bSaveFile		( const char* fullName=NULL );
private:
	void			vClear			( VOID );

	void			vGetTex			( VOID );
	void			vGetMask		( VOID );
	void			vGetClump		( VOID );
	void			vGetAnim		( VOID );
	void			vGetSpline		( VOID );
};



#endif