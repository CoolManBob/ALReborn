#if !defined(AFX_AGCDEFFGLOBAL_H__CF745D23_C879_4DE2_BF1F_43357BC43906__INCLUDED_)
#define AFX_AGCDEFFGLOBAL_H__CF745D23_C879_4DE2_BF1F_43357BC43906__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rwcore.h"
#include "rpworld.h"
#include "rtpitexd.h"
#include "ApBase.h"
#include "MagDebug.h"
#include "AgcuEffUtil.h"// kday
#include "AgcuEffIniMng.h"

#include "AgcmEff2.h"
#include "AgcmResourceLoader.h"
#include "AgcmRender.h"
#include "AgcmSound.h"
#include "AgcmPostFX.h"
#include "AgcmMap.h"
#include "AgcmDynamicLightmap.h"
#include "AgcmUVAnimation.h"
#include "AgcmEventEffect.h"

enum	E_GLOBAL_FLAG
{
	E_GFLAG_TONEDOWN	= 0x00000001,
	E_GFLAG_USEPACK		= 0x00000002,

	E_GFLAG_EFFOFF		= 0x00000004,

	//툴을 위한 옵션
	E_GFLAG_SHOWWIRE	= 0x00010000,
	E_GFLAG_SHOWSOLID	= 0x00020000,
	E_GFLAG_SHOWBOX		= 0x00040000,
	E_GFLAG_SHOWSPHERE	= 0x00080000,
	E_GFLAG_CALCBOUND	= 0x00100000,
	E_GFLAG_SHOWFRM		= 0x00200000,
};


#ifdef _DEBUG
typedef void (*fptrCBFrmRender)(RwFrame* pFrm);
#endif

class AgcdEffGlobal
{
#ifdef USE_MFC
	const AgcdEffBase*	m_pShowBase;
public:
	void				bSetShowEffBase(const AgcdEffBase* pbase) { m_pShowBase = pbase; };
	const AgcdEffBase*	bGetShowEffBase()const { return m_pShowBase; };
private:
#endif //USE_MFC

#ifdef _DEBUG
	fptrCBFrmRender		m_fptrCBFrmRender;
public:
	void bSetCBFrmRender( fptrCBFrmRender fptr ) { m_fptrCBFrmRender = fptr; };
	const fptrCBFrmRender bGetCBFrmRender()const { return m_fptrCBFrmRender;};
private:
#endif

public:
	static AgcdEffGlobal& bGetInst(void);

	AgcdEffGlobal();
	virtual ~AgcdEffGlobal();

#ifdef USE_MFC
	RwUInt32*			m_pBitFlags;
#endif

	//flag
	RwUInt32					bFlagChk					( E_GLOBAL_FLAG eFlag ) const	{ return DEF_FLAG_CHK(m_ulFlag, eFlag); };
	void						bFlagOn						( E_GLOBAL_FLAG eFlag )			{ DEF_FLAG_ON( m_ulFlag, eFlag ); };
	void						bFlagOff					( E_GLOBAL_FLAG eFlag )			{ DEF_FLAG_OFF( m_ulFlag, eFlag ); };
	RwUInt32*					bFoorTool_GetPtrFlag		( VOID )						{ return &m_ulFlag; };

	//access
	const AgcmEff2*				bGetPtrAgcmEff2				( VOID ) const	{ return m_pAgcmEff2; };
	const AgcmResourceLoader*	bGetPtrResoruceLoader		( VOID ) const	{ return m_pResourceLoader; };
	const AgcmMap*				bGetPtrAgcmMap				( VOID ) const	{ return m_pAgcmMap; };
	const AgcmOcTree*			bGetPtrAgcmOcTree			( VOID ) const	{ return m_pAgcmOcTree; };
	const ApmOcTree*			bGetPtrApmOcTree			( VOID ) const	{ return m_pApmOcTree; };
	const AgcmDynamicLightmap*	bGetPtrAgcmDynamicLightmap	( VOID ) const	{ return m_pAgcmDynamicLightmap; };
	const AgcmRender*			bGetPtrAgcmRender			( VOID ) const	{ return m_pRenderModule; };
	const AgcmSound*			bGetPtrAgcmSound			( VOID ) const	{ return m_pSoundModule; };	

	const RwFrame*				bGetPtrFrmMainCharac		( VOID ) const	{ return m_pMainCharacter; };
	const RpClump*				bGetPtrClumpMainCharac		( VOID ) const	{ return m_pClumpMainCharacter; };
	const RpWorld*				bGetPtrRpWorld				( VOID ) const	{ return m_pWorld; };
	const RwCamera*				bGetPtrRwCamera				( VOID ) const	{ return m_pCamera; };

	RwUInt32					bGetCurrTime				( VOID ) const	{ return m_dwNowTime; };
	RwUInt32					bGetDiffTimeMS				( VOID ) const	{ return m_dwDiffTime; };
	RwReal						bGetDiffTimeS				( VOID ) const	{ return m_fDiffTime; };

	AgcmEventEffect*			bGetPtrAgcmEventEffect		( VOID )		{ return m_pAgcmEventEffect; };
	AgcmEff2*					bGetPtrAgcmEff2				( VOID )		{ return m_pAgcmEff2; };
	AgcmResourceLoader*			bGetPtrResoruceLoader		( VOID )		{ return m_pResourceLoader; };
	AgcmMap*					bGetPtrAgcmMap				( VOID )		{ return m_pAgcmMap; };
	AgcmOcTree*					bGetPtrAgcmOcTree			( VOID )		{ return m_pAgcmOcTree; };
	ApmOcTree*					bGetPtrApmOcTree			( VOID )		{ return m_pApmOcTree; };
	AgcmDynamicLightmap*		bGetPtrAgcmDynamicLightmap	( VOID )		{ return m_pAgcmDynamicLightmap; };
	AgcmRender*					bGetPtrAgcmRender			( VOID )		{ return m_pRenderModule; };
	AgcmSound*					bGetPtrAgcmSound			( VOID )		{ return m_pSoundModule; };
	AgcmPostFX*					bGetPtrAgcmPostFX			( VOID )		{ return m_pPostFXModule; };
	AgcmUVAnimation*			bGetPtrAgcmUVAnimation		( VOID )		{ return m_pAgcmUVAnimation; };
	RwFrame*					bGetPtrFrmMainCharac		( VOID )		{ return m_pMainCharacter; };
	RpClump*					bGetPtrClumpMainCharac		( VOID )		{ return m_pClumpMainCharacter; };
	RpWorld*					bGetPtrRpWorld				( VOID )		{ return m_pWorld; };
	RwCamera*					bGetPtrRwCamera				( VOID )		{ return m_pCamera; };
	RwFrame*					bGetPtrCamFrm				( VOID )		{ return RwCameraGetFrame( m_pCamera ); };
	RwMatrix*					bGetPtrCamLTM				( VOID )		{ return RwFrameGetLTM( RwCameraGetFrame( m_pCamera ) ); };

	AgcuEffIniMng*				bGetPtrEffIniMng			( VOID )		{ return &m_cEffIniMng; };

	//set up
	void						bSetPtrAgcmEff2				( AgcmEff2*				pAgcmEff2		)		{ m_pAgcmEff2			= pAgcmEff2;	  };
	void						bSetPtrResoruceLoader		( AgcmResourceLoader*	pResouceLoader	)		{	m_pResourceLoader	= pResouceLoader; };
	void						bSetPtrAgcmMap				( AgcmMap*				pAgcmMap		)		{	m_pAgcmMap			= pAgcmMap		; };
	
	void						bSetPtrAgcmOcTree			( AgcmOcTree*			pAgcmOcTree		)		{	m_pAgcmOcTree		= pAgcmOcTree	; };
	void						bSetPtrApmOcTree			( ApmOcTree*			pApmOcTree		)		{	m_pApmOcTree		= pApmOcTree	; };

	void						bSetPtrAgcmEventEffect		( AgcmEventEffect*		pAgcmEventEffect)		{ m_pAgcmEventEffect		= pAgcmEventEffect	; };
	
	void						bSetPtrAgcmDynamicLightmap	( AgcmDynamicLightmap* pAgcmDynamicLightmap)	{ m_pAgcmDynamicLightmap	= pAgcmDynamicLightmap; };

	void						bSetPtrAgcmRender			( AgcmRender*			pRenderModule	)		{	m_pRenderModule		= pRenderModule	; };
	void						bSetPtrAgcmSound			( AgcmSound*			pSoundModule	)		{	m_pSoundModule		= pSoundModule	; };
	void						bSetPtrAgcmPostFX			( AgcmPostFX*			pPostFXModule	)		{ m_pPostFXModule		= pPostFXModule ; };

	void						bSetPtrAgcmUVAnimation		( AgcmUVAnimation*		pAgcmUVAnimation)		{ m_pAgcmUVAnimation	= pAgcmUVAnimation; };

	void						bSetPtrFrmMainCharac		( RwFrame*				MainCharacter	)		{	m_pMainCharacter	= MainCharacter	; };
	void						bSetPtrClumpMainCharac		( RpClump*				pClump			)		{	m_pClumpMainCharacter= pClump		; };
	void						bSetPtrRpWorld				( RpWorld*				pWorld			)		{	m_pWorld			= pWorld		; };	
	void						bSetPtrRwCamera				( RwCamera*				pCamera			)		{	m_pCamera			= pCamera		; };

	//매 프래임마다 업데이트.
	void					bSetCurrTime			( UINT32 dwCurrTime )
	{ 
		m_dwDiffTime = dwCurrTime - m_dwNowTime; 
		m_fDiffTime	= static_cast<RwReal>(m_dwDiffTime) * 0.001f;
		m_dwNowTime = dwCurrTime; 
	};

public:
	AgcmEff2*				m_pAgcmEff2;

	AgcmResourceLoader*		m_pResourceLoader;
	AgcmMap*				m_pAgcmMap;
	AgcmOcTree*				m_pAgcmOcTree;
	ApmOcTree*				m_pApmOcTree;
	AgcmDynamicLightmap*	m_pAgcmDynamicLightmap;
	AgcmRender*				m_pRenderModule;
	AgcmSound*				m_pSoundModule;
	AgcmPostFX*				m_pPostFXModule;
	AgcmEventEffect*		m_pAgcmEventEffect;
	AgcmUVAnimation*		m_pAgcmUVAnimation;

	RwFrame*				m_pMainCharacter;
	RpClump*				m_pClumpMainCharacter;
	RpWorld*				m_pWorld;
	RwCamera*				m_pCamera;

	AgcuEffIniMng			m_cEffIniMng;

	RwUInt32				m_dwNowTime;		// 현재의 시간 - Idle때 준다.	( milli_second )
	RwUInt32				m_dwDiffTime;		// 한프레임 경과 시간.			( milli_second )
	RwReal					m_fDiffTime;		// m_dwDiffTime * 0.001f;		( second )
	RwUInt32				m_ulFlag;

	static AgcdEffGlobal*	m_pThis;
};

#endif