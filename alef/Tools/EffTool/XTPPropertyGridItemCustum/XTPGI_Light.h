// XTPGI_Light.h: interface for the CXTPGI_Light class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_LIGHT_H__C5FFD6A6_5C7A_485A_AC8C_96F1C56FA97D__INCLUDED_)
#define AFX_XTPGI_LIGHT_H__C5FFD6A6_5C7A_485A_AC8C_96F1C56FA97D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"
#include "AgcdEffLight.h"

class CXTPGI_Light : public CXTPGI_EffBase  
{
	class	CXTPGI_Pos;
	class	CXTPGI_Angle;

	friend class CXTPGI_Pos;
	friend class CXTPGI_Angle;

public:
	CXTPGI_Light(CString strCaption, AgcdEffLight* pLight);
	virtual ~CXTPGI_Light()	{	}

protected:
	virtual void OnAddChildItem();

protected:
	void	tUpdateMat();

private:
	AgcdEffLight*	m_pLight;

	CXTPGI_EnumGen<AgcdEffLight::E_EFFLIGHTTYPE>*	m_pItemType;
	CXTPGI_Pos*										m_pItemPos;
	CXTPGI_Angle*									m_pItemAngle;
	CXTPGI_Float*									m_pItemConAngle;
	CXTPGI_RwSurfaceProperties*						m_pItemSurfProp;
	CXTPGI_RwRGBA*									m_pItemMaterialColr;
};

#endif // !defined(AFX_XTPGI_LIGHT_H__C5FFD6A6_5C7A_485A_AC8C_96F1C56FA97D__INCLUDED_)
