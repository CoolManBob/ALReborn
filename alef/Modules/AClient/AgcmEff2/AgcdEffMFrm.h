// f1 = t^2
// f2 = sin( t )
// out = f1 * f2 = (t^2) * sin( t )
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_AGCDEFFMFRM_H__E4AC0C33_8938_4F82_B984_D06C6698C714__INCLUDED_)
#define AFX_AGCDEFFMFRM_H__E4AC0C33_8938_4F82_B984_D06C6698C714__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

class AgcdEffMFrm : public AgcdEffBase, public ApMemory<AgcdEffMFrm, 100>
{
	EFFMEMORYLOG_SMV;

public:
	// enum
	enum E_MFRM_TYPE
	{
		EMFRM_SHAKE		= 0,
		EMFRM_SPLINE,
		EMFRM_NUM,
	};

	enum	eOscillationAxis
	{
		EOSCILLATIONAXIS_LOCAL_X	= 0,
		EOSCILLATIONAXIS_LOCAL_Y	,
		EOSCILLATIONAXIS_LOCAL_Z	,
		
		EOSCILLATIONAXIS_WORLD_X	,
		EOSCILLATIONAXIS_WORLD_Y	,
		EOSCILLATIONAXIS_WORLD_Z	,
		
		EOSCILLATIONAXIS_CAMERA_X	,
		EOSCILLATIONAXIS_CAMERA_Y	,
		EOSCILLATIONAXIS_CAMERA_Z	,

		EOSCILLATIONAXIS_RANDOM		,

		EOSCILLATIONAXIS_NUM		,
	};

	enum	eWhoseFrame
	{
		EFRM_CAM		= 0,	//카메라..
		EFRM_SRC		,		//가해자..
		EFRM_DST		,		//피해자..

		EFRM_NUM		,
	};

	struct StShakeFrame
	{
	public:
		eOscillationAxis	m_eAxis;
		eWhoseFrame			m_eWhose;
		RwReal				m_fAmplitude;
		RwUInt32			m_dwDuration;
		RwReal				m_fTotalCycle;
		RwReal				m_fSpeed;
		RwReal				m_fCof;

	public:
		StShakeFrame()
		{
			Eff2Ut_ZEROBLOCK( *this );
		};

		//access	
		eOscillationAxis	bGetOscillationAxis			( VOID ) const			{ return m_eAxis;			};
		eWhoseFrame			bGetWhose					( VOID ) const			{ return m_eWhose;			};
		RwReal				bGetAmplitude				( VOID ) const			{ return m_fAmplitude;		};
		RwUInt32			bGetDuration				( VOID ) const			{ return m_dwDuration;		};
		RwReal				bGetCycle					( VOID ) const			{ return m_fTotalCycle;		};
		RwReal				bGetSpeed					( VOID ) const			{ return m_fSpeed;			};

		//setup
		void				bSetOscillationAxis			( eOscillationAxis	eAxis		)	{ m_eAxis		= eAxis			; };
		void				bSetWhose					( eWhoseFrame		eWhose		)	{ m_eWhose		= eWhose		; };
		void				bSetAmplitude				( RwReal			fAmplitude	)	{ m_fAmplitude	= fAmplitude	; };
		void				bSetDuration				( RwUInt32			dwDuration	)	{ m_dwDuration = dwDuration, m_fCof = m_fAmplitude/(RwReal)(dwDuration * dwDuration); };
		void				bSetCycle					( RwReal			fTotalCycle )	{ m_fTotalCycle	= fTotalCycle; vSetSpeed(); };

		void				vSetSpeed					( VOID )							{	m_fSpeed = m_fTotalCycle * 360.f / (RwReal)m_dwDuration;	}
		RwReal				bDampping					( RwUInt32 dwPastTime ) const		{	return ( m_fCof * ((RwReal)((dwPastTime - m_dwDuration) * (dwPastTime - m_dwDuration))) );	}
	};
	typedef StShakeFrame STSHAKEFRM, *PSTSHAKEFRM, *LPSTSHAKEFRM;

public:
	explicit AgcdEffMFrm(E_MFRM_TYPE eMFrmType=EMFRM_SHAKE);
	virtual ~AgcdEffMFrm();

	RwInt32					bShakeFrm					( RwFrame* pFrmShake, RwUInt32 dwPastTime );

	//setup
	void					bSetMFrmType				( E_MFRM_TYPE eMFrmType )	{ m_eMFrmType = eMFrmType; };
	STSHAKEFRM&				bGetRefShakeFrm				( VOID )					{ return m_stShakeFrm; };

	//access
	E_MFRM_TYPE				bGetMFrmType				( VOID ) const				{ return m_eMFrmType; };
	const STSHAKEFRM&		bGetCRefShakeFrm			( VOID ) const				{ return m_stShakeFrm; };
	
	//file in out
	RwInt32					bToFile						( FILE* fp );
	RwInt32					bFromFile					( FILE* fp );
	
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

private:
	AgcdEffMFrm( const AgcdEffMFrm& cpy ) : AgcdEffBase(cpy) {cpy;}
	AgcdEffMFrm& operator = ( const AgcdEffMFrm& cpy ) { cpy; return *this; }

public:
	E_MFRM_TYPE				m_eMFrmType;
	STSHAKEFRM				m_stShakeFrm;
};

#endif