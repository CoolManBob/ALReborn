// XTPGI_Light.cpp: implementation of the CXTPGI_Light class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Light.h"

#include "AgcdEffLight.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//--------------------- CXTPGI_Light::CXTPGI_Pos ----------------------------
class CXTPGI_Light::CXTPGI_Pos : public CXTPGI_RwV3d
{
public:
	CXTPGI_Pos(RwV3d* pVal, RwV3d init ) : CXTPGI_RwV3d(_T("center"), pVal, init){};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_Light*)m_pParent)->tUpdateMat();
	}
};

//--------------------- CXTPGI_Light::CXTPGI_Angle ----------------------------
class CXTPGI_Light::CXTPGI_Angle : public CXTPGI_STANGLE
{
public:
	CXTPGI_Angle(tagStAngle* pVal, tagStAngle init ) : CXTPGI_STANGLE(_T("angle"), pVal, init)	{	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_Light*)m_pParent)->tUpdateMat();
	}
};

//--------------------- CXTPGI_Light ----------------------------
CXTPGI_Light::CXTPGI_Light(CString strCaption, AgcdEffLight* pLight) : CXTPGI_EffBase(strCaption, pLight)
 ,m_pLight(pLight)
 ,m_pItemType(NULL)
 ,m_pItemPos(NULL)
 ,m_pItemAngle(NULL)
 ,m_pItemConAngle(NULL)
 ,m_pItemSurfProp(NULL)
 ,m_pItemMaterialColr(NULL)
{
	ASSERT( m_pLight );
}

void CXTPGI_Light::OnAddChildItem()
{
	ASSERT( m_pLight );

	CXTPGI_EffBase::OnAddChildItem();

	INT		nMode[3] = { AgcdEffLight::E_EFFLIGHT_POINT, AgcdEffLight::E_EFFLIGHT_SPOT, AgcdEffLight::E_EFFLIGHT_SOFTSPOT };
	static LPCTSTR strLight[3] = { "POINT", "SPOT", "SOFTSPOT" };
	
	m_pItemType = (CXTPGI_EnumGen<AgcdEffLight::E_EFFLIGHTTYPE>*)AddChildItem(new CXTPGI_EnumGen<AgcdEffLight::E_EFFLIGHTTYPE>
		( _T("type") ,3 ,m_pLight->m_eEffLightType ,&m_pLight->m_eEffLightType ,nMode ,strLight ) );
		
	m_pItemPos = (CXTPGI_Pos*)AddChildItem(new CXTPGI_Pos(&m_pLight->m_v3dCenter,m_pLight->m_v3dCenter));
	m_pItemAngle = (CXTPGI_Angle*)AddChildItem(new CXTPGI_Angle(&m_pLight->m_stAngle, m_pLight->m_stAngle));
	m_pItemConAngle = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("ConeAngle"), &m_pLight->m_fConAngle, m_pLight->m_fConAngle));
	m_pItemSurfProp = (CXTPGI_RwSurfaceProperties*)AddChildItem(new CXTPGI_RwSurfaceProperties(_T("surfProp"), &m_pLight->m_SurfProp, m_pLight->m_SurfProp));
	m_pItemMaterialColr = (CXTPGI_RwRGBA*)AddChildItem(new CXTPGI_RwRGBA(_T(""), &m_pLight->m_ColrMaterial, m_pLight->m_ColrMaterial));
};

void CXTPGI_Light::tUpdateMat()
{
	ASSERT( m_pLight );

	m_pLight->bSetPos(m_pLight->m_v3dCenter);
	m_pLight->bSetAngle(m_pLight->m_stAngle);
};
