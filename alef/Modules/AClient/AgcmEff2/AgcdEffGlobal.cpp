#include "AgcdEffGlobal.h"
#include "ApMemoryTracker.h"

AgcdEffGlobal*		AgcdEffGlobal::m_pThis = NULL;
AgcdEffGlobal& AgcdEffGlobal::bGetInst(void)
{
	static AgcdEffGlobal inst;
	return *m_pThis;
}

AgcdEffGlobal::AgcdEffGlobal()
: m_pResourceLoader	( NULL )
, m_pAgcmMap		( NULL )
, m_pRenderModule	( NULL )
, m_pSoundModule	( NULL )
, m_pPostFXModule	( NULL )
, m_pAgcmEventEffect( NULL )
, m_pMainCharacter	( NULL )
, m_pClumpMainCharacter( NULL )
, m_pWorld			( NULL )
, m_pCamera			( NULL )
, m_dwNowTime		( 0UL )
, m_dwDiffTime		( 0UL )
, m_fDiffTime		( 0.f )
#ifdef USE_MFC
, m_ulFlag			( 0UL )
, m_pShowBase		( NULL )
#else
, m_ulFlag			( E_GFLAG_USEPACK )
#endif //

#ifdef _DEBUG
, m_fptrCBFrmRender ( NULL )
#endif
{
	if( m_pThis ){
		Eff2Ut_ERR( "this is singleton class @ AgcdEffGlobal::AgcdEffGlobal()" );
	}else{
		m_pThis	= this;
	}
}

AgcdEffGlobal::~AgcdEffGlobal()
{
}
