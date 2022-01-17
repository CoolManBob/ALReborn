#if !defined(AFX_AGCDEFFLIGHT_H__1DFA9217_2FFC_41F6_A33D_3A81AFC314F1__INCLUDED_)
#define AFX_AGCDEFFLIGHT_H__1DFA9217_2FFC_41F6_A33D_3A81AFC314F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffBase.h"
#include "AgcuEff2ApMemoryLog.h"

#define		LIGHTRADIUSRATION	300.f

class AgcdEffLight : public AgcdEffBase, public ApMemory<AgcdEffLight, 100>
{
	EFFMEMORYLOG_SMV;

public:
	enum E_EFFLIGHTTYPE
	{
		E_EFFLIGHT_POINT		= rpLIGHTPOINT,
		E_EFFLIGHT_SPOT			= rpLIGHTSPOT,
		E_EFFLIGHT_SOFTSPOT		= rpLIGHTSPOTSOFT,
	};

public:
	explicit AgcdEffLight( E_EFFLIGHTTYPE eLightType = E_EFFLIGHT_POINT );
	virtual ~AgcdEffLight();

	//setup
	void			bSetLightType(E_EFFLIGHTTYPE eLightType)	{ m_eEffLightType = eLightType; };
	void			bSetPos( const RwV3d& v3dPos );
	void			bSetAngle( const STANGLE& stAngle );
	void			bSetConAngle( RwReal fConAngle )			{ m_fConAngle = fConAngle; };
	void			bSetSurfProp( const RwSurfaceProperties& surfProp )	
														{ m_SurfProp = surfProp; };
	void			bSetMaterialColor( RwRGBA rgba )			{ m_ColrMaterial = rgba; };

	//access
	E_EFFLIGHTTYPE	bGetLightType(void)const	{ return m_eEffLightType; };
	const RwV3d&	bGetCRefPos(void)const		{ return m_v3dCenter; };
	const STANGLE&	bGetCRefAngle(void)const	{ return m_stAngle; };
	RwReal			bGetConAngle(void)const		{ return m_fConAngle; };
	const RwSurfaceProperties&	bGetCRefSurfProp(void)const	{ return m_SurfProp; };
	RwRGBA			bGetMaterialColr(void)const	{ return m_ColrMaterial; };

	const RwMatrix*	 bGetCPtrMat_Trans(void)const { return &m_matTrans; };
	const RwMatrix*	 bGetCPtrMat_Rot(void)const { return &m_matRot; };
	
	//file in out
	RwInt32			bToFile(FILE* fp);
	RwInt32			bFromFile(FILE* fp);

private:
	AgcdEffLight( const AgcdEffLight& cpy ) : AgcdEffBase(cpy) {cpy;}
	AgcdEffLight& operator = ( const AgcdEffLight& cpy ) { cpy; return *this; }
	
	//for tool
#ifdef USE_MFC
	virtual 
	INT32		bForTool_Clone(AgcdEffBase* pEffBase);
#endif//USE_MFC

public:
	E_EFFLIGHTTYPE		m_eEffLightType;
	RwV3d				m_v3dCenter;	//pos
	STANGLE				m_stAngle;		//dir
	RwReal				m_fConAngle;	//con_angle

	//메터리얼.
	RwSurfaceProperties	m_SurfProp;
	RwRGBA				m_ColrMaterial;

	RwMatrix			m_matTrans;
	RwMatrix			m_matRot;
};

#endif