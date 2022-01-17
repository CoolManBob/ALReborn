// XTPGI_UINT.cpp: implementation of the CXTPGI_UINT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPGI_UINT.h"

#include "rwcore.h"
#include "rwplcore.h"

#include "AgcuEffUtil.h"
#include "AgcuEffAnimUtil.h"
#include "AgcdEffAnim.h"

#include "AgcdEffTex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//--------------------- CXTPGI_UINT ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_UINT, CXTPPropertyGridItem)
CXTPGI_UINT::CXTPGI_UINT(CString strCaption, UINT* pBind, UINT initVal ) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pBind)
{
	ASSERT(m_pVal);
	tSetVal(initVal);
}

void CXTPGI_UINT::tSetVal(UINT ulVal)
{
	ASSERT( m_pVal );
	*m_pVal	= ulVal;
	CString	strVal;
	strVal.Format("%u", ulVal);
	CXTPPropertyGridItem::SetValue(strVal);
};

void CXTPGI_UINT::SetValue(CString strValue)
{
	ASSERT( m_pVal );
	LPTSTR	pchEnd;
	*m_pVal = _tcstoul(strValue, &pchEnd, 10);
	CXTPPropertyGridItem::SetValue(strValue);
};

//--------------------- CXTPGI_String ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_String, CXTPPropertyGridItem)
CXTPGI_String::CXTPGI_String(CString strCaption, INT nMaxLen, LPTSTR szVal, LPCTSTR szInit ) : CXTPPropertyGridItem(strCaption)
 ,m_nMaxLen(nMaxLen)
 , m_szVal(szVal)
{
	ASSERT(m_szVal);
	tSetVal(szInit);
}

void CXTPGI_String::tSetVal(CString strVal)
{
	ASSERT(m_szVal);

	if( strVal.GetLength() >= m_nMaxLen )
	{
		strncpy( m_szVal, strVal.GetBuffer(0), m_nMaxLen-2 );
		ErrToWnd("string overflow");
		ToWnd( Eff2Ut_FmtMsg("max : %d, len : %d, str : %s", m_nMaxLen, strVal.GetLength(), strVal) );
		m_szVal[m_nMaxLen-1]	= '\0';
	}
	else
		strcpy(m_szVal, strVal.GetBuffer(0));

	CXTPPropertyGridItem::SetValue(m_szVal);
};
void CXTPGI_String::SetValue(CString strValue)
{
	ASSERT( m_szVal );

	tSetVal(strValue);
};

//--------------------- CXTPGI_FileName ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_FileName, CXTPGI_String)
CXTPGI_FileName::CXTPGI_FileName(CString strCaption, INT nMaxLen, LPTSTR szVal, LPCTSTR szInit, LPCTSTR szDir, LPCTSTR szFilter )
 : CXTPGI_String(strCaption, nMaxLen, szVal, szInit)
 , m_strDir(szDir)
 , m_szFilter(szFilter)
{
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasExpandButton;
};

void CXTPGI_FileName::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
	TCHAR	currDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, currDir);

	CString	strpath(currDir);
	strpath += m_strDir;
	strpath += _T("*.*");
	CFileDialog dlgFile(TRUE,NULL,strpath,OFN_READONLY,m_szFilter,this->GetGrid());
	if( IDOK == dlgFile.DoModal() )
	{
		ToWnd( dlgFile.GetFileName() );

		CXTPGI_String::SetValue( dlgFile.GetFileName() );
		::SetCurrentDirectory(currDir);
		CXTPPropertyGridItem::OnValueChanged( dlgFile.GetFileName() );
	}
	
	::SetCurrentDirectory(currDir);
};

//--------------------- CXTPGI_Float ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_Float, CXTPPropertyGridItem)
CXTPGI_Float::CXTPGI_Float(CString strCaption, float* pVal, float fInit ) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
{
	ASSERT(m_pVal);
	tSetVal(fInit);
}

void CXTPGI_Float::tSetVal(float fVal)
{
	ASSERT(m_pVal);

	*m_pVal = fVal;
	CString	strVal;
	strVal.Format("%.4f", *m_pVal);
	CXTPPropertyGridItem::SetValue(strVal);
};

void CXTPGI_Float::SetValue(CString strValue)
{
	ASSERT( m_pVal );	
	*m_pVal = static_cast<float>(atof(strValue));
	CXTPPropertyGridItem::SetValue(strValue);
};

//--------------------- CXTPGI_STANGLE::CXTPGI_FloatYaw ---------------------
class CXTPGI_STANGLE::CXTPGI_FloatYaw : public CXTPGI_Float
{
public:
	CXTPGI_FloatYaw(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_STANGLE*)m_pParent)->tSetYaw(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_STANGLE::CXTPGI_FloatPitch ---------------------
class CXTPGI_STANGLE::CXTPGI_FloatPitch : public CXTPGI_Float
{
public:
	CXTPGI_FloatPitch(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_STANGLE*)m_pParent)->tSetPitch(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_STANGLE::CXTPGI_FloatRoll ---------------------
class CXTPGI_STANGLE::CXTPGI_FloatRoll : public CXTPGI_Float
{
public:
	CXTPGI_FloatRoll(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_STANGLE*)m_pParent)->tSetRoll(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_STANGLE ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_STANGLE, CXTPPropertyGridItem)
CXTPGI_STANGLE::CXTPGI_STANGLE(CString strCaption, tagStAngle* pVal, tagStAngle initVal) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemYaw(NULL)
 ,m_pItemPitch(NULL)
 ,m_pItemRoll(NULL)
{
	ASSERT( m_pVal );
	tSetVal(initVal);
};

void CXTPGI_STANGLE::tSetVal(const tagStAngle& stAngle)
{
	ASSERT( m_pVal );

	*m_pVal	= stAngle;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_STANGLE::tSetYaw(float yaw)
{
	ASSERT( m_pVal );
	m_pVal->m_fYaw = yaw;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_STANGLE::tSetPitch(float pitch)
{
	ASSERT( m_pVal );
	m_pVal->m_fPitch = pitch;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_STANGLE::tSetRoll(float roll)
{
	ASSERT( m_pVal );
	m_pVal->m_fRoll	= roll;
	OnValueChanged( tToString(*m_pVal) );
};	

CString CXTPGI_STANGLE::tToString(const tagStAngle& stAngle)
{
	CString strVal;
	strVal.Format("%.4f;%.4f;%.4f", stAngle.m_fYaw, stAngle.m_fPitch, stAngle.m_fRoll);
	return strVal;
};

tagStAngle CXTPGI_STANGLE::tToSTANGLE(CString strVal)
{
	CString strY, strP, strR;
	AfxExtractSubString(strY, strVal, 0, _T(';'));
	AfxExtractSubString(strP, strVal, 1, _T(';'));
	AfxExtractSubString(strR, strVal, 2, _T(';'));

	tagStAngle	r;
	r.m_fYaw = static_cast<float>(atof(strY));
	r.m_fPitch = static_cast<float>(atof(strP));
	r.m_fRoll = static_cast<float>(atof(strR));
	return r;
};

void CXTPGI_STANGLE::tUpdateChilds()
{
	ASSERT( m_pVal );

	if(m_pItemYaw	)	m_pItemYaw	->tSetVal(m_pVal->m_fYaw	);
	if(m_pItemPitch	)	m_pItemPitch->tSetVal(m_pVal->m_fPitch	);
	if(m_pItemRoll	)	m_pItemRoll	->tSetVal(m_pVal->m_fRoll	);
};

void CXTPGI_STANGLE::OnAddChildItem()
{
	m_pItemYaw		= (CXTPGI_FloatYaw*)AddChildItem(new CXTPGI_FloatYaw(_T("Yaw"), &m_pVal->m_fYaw	, m_pVal->m_fYaw ));
	m_pItemPitch	= (CXTPGI_FloatPitch*)AddChildItem(new CXTPGI_FloatPitch(_T("Pitch"), &m_pVal->m_fPitch, m_pVal->m_fPitch ));
	m_pItemRoll		= (CXTPGI_FloatRoll*)AddChildItem(new CXTPGI_FloatRoll(_T("Roll"), &m_pVal->m_fRoll, m_pVal->m_fRoll ));

	tUpdateChilds();
};

void CXTPGI_STANGLE::SetValue(CString strValue)
{
	ASSERT( m_pVal );
	tSetVal( tToSTANGLE(strValue) );
};

//--------------------- CXTPGI_RwV3d::CXTPGI_FloatX ---------------------
class CXTPGI_RwV3d::CXTPGI_FloatX : public CXTPGI_Float
{
public:
	CXTPGI_FloatX(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwV3d*)m_pParent)->tSetX(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwV3d::CXTPGI_FloatY ---------------------
class CXTPGI_RwV3d::CXTPGI_FloatY : public CXTPGI_Float
{
public:
	CXTPGI_FloatY(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwV3d*)m_pParent)->tSetY(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwV3d::CXTPGI_FloatZ ---------------------
class CXTPGI_RwV3d::CXTPGI_FloatZ : public CXTPGI_Float
{
public:
	CXTPGI_FloatZ(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwV3d*)m_pParent)->tSetZ(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwV3d ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_RwV3d, CXTPPropertyGridItem)
CXTPGI_RwV3d::CXTPGI_RwV3d(CString strCaption, RwV3d* pVal, float x, float y, float z) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemX(NULL)
 ,m_pItemY(NULL)
 ,m_pItemZ(NULL)
{
	ASSERT( m_pVal );
	tSetVal(x,y,z);
};

CXTPGI_RwV3d::CXTPGI_RwV3d(CString strCaption, RwV3d* pVal, const RwV3d& vInit) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemX(NULL)
 ,m_pItemY(NULL)
 ,m_pItemZ(NULL)
{
	ASSERT( m_pVal );
	tSetVal(vInit);
};

void CXTPGI_RwV3d::tSetVal(const RwV3d& v3dVal)
{
	ASSERT( m_pVal );

	*m_pVal	= v3dVal;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_RwV3d::tSetVal(float x, float y, float z)
{
	ASSERT( m_pVal );

	m_pVal->x	= x;
	m_pVal->y	= y;
	m_pVal->z	= z;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_RwV3d::tSetX(float x)
{
	ASSERT( m_pVal );

	m_pVal->x = x;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwV3d::tSetY(float y)
{
	ASSERT( m_pVal );

	m_pVal->y = y;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwV3d::tSetZ(float z)
{
	ASSERT( m_pVal );

	m_pVal->z = z;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_RwV3d::tToString(const RwV3d& v3dVal)
{
	CString strVal;
	strVal.Format("%.4f;%.4f;%.4f", v3dVal.x, v3dVal.y, v3dVal.z);
	return strVal;
};

RwV3d CXTPGI_RwV3d::tToRwV3d(CString strVal)
{
	CString strx, stry, strz;
	AfxExtractSubString(strx, strVal, 0, _T(';'));
	AfxExtractSubString(stry, strVal, 1, _T(';'));
	AfxExtractSubString(strz, strVal, 2, _T(';'));

	RwV3d	vr = { (float)(atof(strx)), (float)(atof(stry)), (float)(atof(strz)) };
	return vr;
};

void CXTPGI_RwV3d::tUpdateChilds()
{
	ASSERT( m_pVal );//&& m_pItemX && m_pItemY && m_pItemZ );

	if(m_pItemX)	m_pItemX->tSetVal(m_pVal->x);
	if(m_pItemY)	m_pItemY->tSetVal(m_pVal->y);
	if(m_pItemZ)	m_pItemZ->tSetVal(m_pVal->z);
};

void CXTPGI_RwV3d::OnAddChildItem()
{
	m_pItemX = (CXTPGI_FloatX*)AddChildItem(new CXTPGI_FloatX(_T("x"), &m_pVal->x, m_pVal->x));
	m_pItemY = (CXTPGI_FloatY*)AddChildItem(new CXTPGI_FloatY(_T("y"), &m_pVal->y, m_pVal->y));
	m_pItemZ = (CXTPGI_FloatZ*)AddChildItem(new CXTPGI_FloatZ(_T("z"), &m_pVal->z, m_pVal->z));

	tUpdateChilds();
};

void CXTPGI_RwV3d::SetValue(CString strValue)
{
	ASSERT( m_pVal );
	tSetVal( tToRwV3d(strValue) );
};

//--------------------- CXTPGI_RwSphere::CXTPGI_FloatRadius ---------------------
class CXTPGI_RwSphere::CXTPGI_FloatRadius : public CXTPGI_Float
{
public:
	CXTPGI_FloatRadius(CString strCaption, float* pVal, float fInit=0.f) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwSphere*)m_pParent)->tSetRadius(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_RwSphere::CXTPGI_RwV3dCenter ---------------------
class CXTPGI_RwSphere::CXTPGI_RwV3dCenter : public CXTPGI_RwV3d
{
public:
	CXTPGI_RwV3dCenter(CString strCaption, RwV3d* pVal, float x=0.f, float y=0.f, float z=0.f)
		: CXTPGI_RwV3d(strCaption, pVal, x, y, z)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwSphere*)m_pParent)->tSetCenter(CXTPGI_RwV3d::tToRwV3d(strVal));
	};
};

//--------------------- CXTPGI_RwSphere ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_RwSphere, CXTPPropertyGridItem)
CXTPGI_RwSphere::CXTPGI_RwSphere(CString strCaption, RwSphere* pVal, float radius, float cx, float cy, float cz)
 : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemRadius(NULL)
 ,m_pItemCenter(NULL)
{
	ASSERT( m_pVal );
	RwSphere	sphere = { {cx,cy,cz},radius };
	tSetVal(sphere);
};

void CXTPGI_RwSphere::tSetVal(const RwSphere& sphere)
{
	ASSERT( m_pVal );

	*m_pVal = sphere;
	CXTPPropertyGridItem::SetValue(tToString(sphere));
	tUpdateChilds();
};

void CXTPGI_RwSphere::tSetVal(float fradius, const RwV3d& center)
{
	ASSERT( m_pVal );

	m_pVal->radius	= fradius;
	m_pVal->center	= center;
	CXTPPropertyGridItem::SetValue(tToString(*m_pVal));
	tUpdateChilds();
};

void CXTPGI_RwSphere::tSetRadius(float radius)
{
	ASSERT( m_pVal );

	m_pVal->radius	= radius;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwSphere::tSetCenter(const RwV3d& center)
{
	ASSERT( m_pVal );

	m_pVal->center	= center;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_RwSphere::tToString(const RwSphere& sphere)
{
	CString strVal;
	strVal.Format( "%.4f;%.4f;%.4f;%.4f" ,sphere.center.x ,sphere.center.y ,sphere.center.z ,sphere.radius );
	return strVal;
};

RwSphere CXTPGI_RwSphere::tToRwSphere(CString strVal)
{
	CString strx, stry, strz, strr;
	AfxExtractSubString(strx, strVal, 0, _T(';'));
	AfxExtractSubString(stry, strVal, 1, _T(';'));
	AfxExtractSubString(strz, strVal, 2, _T(';'));
	AfxExtractSubString(strr, strVal, 3, _T(';'));
	RwSphere sr	= { (float)(atof(strx)) ,(float)(atof(stry)) ,(float)(atof(strz)) ,(float)(atof(strr)) };
	return sr;
};

void CXTPGI_RwSphere::tUpdateChilds()
{
	ASSERT( m_pVal );

	if(m_pItemRadius) m_pItemRadius->tSetVal(m_pVal->radius);
	if(m_pItemCenter) m_pItemCenter->tSetVal(m_pVal->center);
};

void CXTPGI_RwSphere::OnAddChildItem()
{
	m_pItemCenter = (CXTPGI_RwV3dCenter*)AddChildItem(new CXTPGI_RwV3dCenter(_T("center"), &m_pVal->center, m_pVal->center.x, m_pVal->center.y, m_pVal->center.z ) );
	m_pItemRadius = (CXTPGI_FloatRadius*)AddChildItem(new CXTPGI_FloatRadius(_T("radius"), &m_pVal->radius, m_pVal->radius));
	tUpdateChilds();
};

void CXTPGI_RwSphere::SetValue(CString strValue)
{
	tSetVal( tToRwSphere(strValue) );
};

//--------------------- CXTPGI_RwBBox::CXTPGI_RwV3dSup ---------------------
class CXTPGI_RwBBox::CXTPGI_RwV3dSup : public CXTPGI_RwV3d
{
public:
	CXTPGI_RwV3dSup(CString strCaption, RwV3d* pVal, const RwV3d& vInit) : CXTPGI_RwV3d(strCaption, pVal, vInit.x, vInit.y, vInit.z)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwBBox*)m_pParent)->tSetSup(CXTPGI_RwV3d::tToRwV3d(strVal));
	};
};

//--------------------- CXTPGI_RwBBox::CXTPGI_RwV3dInf ---------------------
class CXTPGI_RwBBox::CXTPGI_RwV3dInf : public CXTPGI_RwV3d
{
public:
	CXTPGI_RwV3dInf(CString strCaption, RwV3d* pVal, const RwV3d& vInit) : CXTPGI_RwV3d(strCaption, pVal, vInit.x, vInit.y, vInit.z)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwBBox*)m_pParent)->tSetInf(CXTPGI_RwV3d::tToRwV3d(strVal));
	};
};

//--------------------- CXTPGI_RwBBox ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_RwBBox, CXTPPropertyGridItem)
CXTPGI_RwBBox::CXTPGI_RwBBox(CString strCaption, RwBBox* pVal, const RwBBox& bbox) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemSup(NULL)
 ,m_pItemInf(NULL)
{
	ASSERT( m_pVal );
	tSetVal(bbox);
};

void CXTPGI_RwBBox::tSetVal(const RwBBox& bbox)
{
	ASSERT( m_pVal );

	*m_pVal = bbox;
	CXTPPropertyGridItem::SetValue(tToString(*m_pVal));
	tUpdateChilds();
};

void CXTPGI_RwBBox::tSetVal(const RwV3d& sup, const RwV3d& inf)
{
	ASSERT( m_pVal );

	RwBBox	box = { sup.x, sup.y, sup.z, inf.x, inf.y, inf.z };
	CXTPPropertyGridItem::SetValue(tToString(box));
	tUpdateChilds();
};

void CXTPGI_RwBBox::tSetSup(const RwV3d& sup)
{
	ASSERT( m_pVal );
	m_pVal->sup	= sup;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwBBox::tSetInf(const RwV3d& inf)
{
	ASSERT( m_pVal );
	m_pVal->inf	= inf;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_RwBBox::tToString(const RwBBox& box)
{
	CString strVal;
	strVal.Format("%.4f;%.4f;%.4f;%.4f;%.4f;%.4f" ,box.sup.x ,box.sup.y ,box.sup.z ,box.inf.x ,box.inf.y ,box.inf.z );
	return strVal;
};

RwBBox CXTPGI_RwBBox::tToRwBBox(CString strVal)
{
	CString strsx, strsy, strsz, strix, striy, striz;
	AfxExtractSubString(strsx, strVal, 0, _T(';'));
	AfxExtractSubString(strsy, strVal, 1, _T(';'));
	AfxExtractSubString(strsz, strVal, 2, _T(';'));
	AfxExtractSubString(strix, strVal, 3, _T(';'));
	AfxExtractSubString(striy, strVal, 4, _T(';'));
	AfxExtractSubString(striz, strVal, 5, _T(';'));

	RwBBox br = { (float)(atof(strsx)) ,(float)(atof(strsy)) ,(float)(atof(strsz))
				 ,(float)(atof(strix)) ,(float)(atof(striy)) ,(float)(atof(striz)) };
	return br;
};

void CXTPGI_RwBBox::tUpdateChilds()
{
	ASSERT( m_pVal );

	if(m_pItemSup) m_pItemSup->tSetVal(m_pVal->sup);
	if(m_pItemInf) m_pItemInf->tSetVal(m_pVal->inf);
};

void CXTPGI_RwBBox::OnAddChildItem()
{
	ASSERT( m_pVal );
	m_pItemSup = (CXTPGI_RwV3dSup*)AddChildItem(new CXTPGI_RwV3dSup(_T("sup"), &m_pVal->sup, m_pVal->sup));
	m_pItemInf = (CXTPGI_RwV3dInf*)AddChildItem(new CXTPGI_RwV3dInf(_T("inf"), &m_pVal->inf, m_pVal->inf));
	tUpdateChilds();
};

void CXTPGI_RwBBox::SetValue(CString strVal)
{
	tSetVal(tToRwBBox(strVal));
};

//--------------------- CXTPGI_FlagBool ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_FlagBool, CXTPPropertyGridItemBool)
CXTPGI_FlagBool::CXTPGI_FlagBool(UINT* pFlag, const UINT FLAG, CString strCaption, BOOL* pBindBool)
 : CXTPPropertyGridItemBool(strCaption, (DEF_FLAG_CHK(*pFlag, FLAG) ? TRUE : FALSE), pBindBool)
 ,m_pulFlag(pFlag)
 ,THEFLAG(FLAG)
{
	ASSERT(m_pulFlag);
};

void CXTPGI_FlagBool::SetValue(CString strVal)
{
	ASSERT(m_pulFlag);

	CXTPPropertyGridItem::SetValue(strVal);

	if( "True" == strVal )
	{
		DEF_FLAG_ON(*m_pulFlag, THEFLAG);
		ToWnd( "DEF_FLAG_ON" );
	}
	else
	{
		DEF_FLAG_OFF(*m_pulFlag, THEFLAG);
		ToWnd( "DEF_FLAG_OFF" );
	}

	CHAR	szBinary[MAX_PATH] = "";
	Eff2Ut_ShowBinaryVal(szBinary, THEFLAG, FALSE, FALSE);
	ToWnd( Eff2Ut_FmtMsg( "0x%08x -- %s [flag]", THEFLAG, szBinary ) );
	Eff2Ut_ShowBinaryVal(szBinary, *m_pulFlag, FALSE, FALSE);
	ToWnd( Eff2Ut_FmtMsg( "0x%08x -- %s [value]", *m_pulFlag, szBinary ) );
};

//--------------------- CXTPGI_RwSurfaceProperties::CXTPGI_FloatAmbient ---------------------
class CXTPGI_RwSurfaceProperties::CXTPGI_FloatAmbient : public CXTPGI_Float
{
public:
	CXTPGI_FloatAmbient(float* pVal, float fInit=0.f ) : CXTPGI_Float(_T("ambient"), pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwSurfaceProperties*)m_pParent)->tSetAmbient(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwSurfaceProperties::CXTPGI_FloatSpecular ---------------------
class CXTPGI_RwSurfaceProperties::CXTPGI_FloatSpecular : public CXTPGI_Float
{
public:
	CXTPGI_FloatSpecular(float* pVal, float fInit=0.f ) : CXTPGI_Float(_T("specular"), pVal, fInit)
	{
	};
		
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwSurfaceProperties*)m_pParent)->tSetSpecular(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwSurfaceProperties::CXTPGI_FloatDiffuse ---------------------
class CXTPGI_RwSurfaceProperties::CXTPGI_FloatDiffuse : public CXTPGI_Float
{
public:
	CXTPGI_FloatDiffuse(float* pVal, float fInit=0.f ) : CXTPGI_Float(_T("diffuse"), pVal, fInit)
	{
	};

	virtual void OnValueChanged(CString strVal)	{
		((CXTPGI_RwSurfaceProperties*)m_pParent)->tSetDiffuse(static_cast<float>(atof(strVal)));
	}
};

//--------------------- CXTPGI_RwSurfaceProperties ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_RwSurfaceProperties, CXTPPropertyGridItemBool)
CXTPGI_RwSurfaceProperties::CXTPGI_RwSurfaceProperties(CString strCaption, RwSurfaceProperties* pBind, const RwSurfaceProperties& initVal )
 : CXTPPropertyGridItem(strCaption)
 ,m_pVal( pBind )
 ,m_pItemAmbient( NULL )
 ,m_pItemSpecular( NULL )
 ,m_pItemDiffuse( NULL )
{
	ASSERT( m_pVal );
	tSetVal( initVal );
};

void CXTPGI_RwSurfaceProperties::tSetVal(const RwSurfaceProperties& val)
{
	ASSERT( m_pVal );

	*m_pVal	= val;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_RwSurfaceProperties::tSetAmbient(float ambient)
{
	ASSERT( m_pVal );

	m_pVal->ambient	= ambient;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwSurfaceProperties::tSetSpecular(float specular)
{
	ASSERT( m_pVal );
	m_pVal->specular	= specular;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwSurfaceProperties::tSetDiffuse(float diffuse)
{
	ASSERT( m_pVal );
	m_pVal->diffuse	= diffuse;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_RwSurfaceProperties::tToString(const RwSurfaceProperties& val)
{
	CString str;
	str.Format("%.4f;%.4f;%.4f", val.ambient, val.specular, val.diffuse);
	return str;
};

RwSurfaceProperties CXTPGI_RwSurfaceProperties::tToRwSurfaceProperties(CString strVal)
{
	CString stra, strs, strd;
	AfxExtractSubString(stra, strVal, 0, _T(';'));
	AfxExtractSubString(strs, strVal, 1, _T(';'));
	AfxExtractSubString(strd, strVal, 2, _T(';'));
	
	RwSurfaceProperties r = {  (float)(atof(stra)), (float)(atof(strs)), (float)(atof(strd)) };
	return r;
};

void CXTPGI_RwSurfaceProperties::tUpdateChilds()
{
	ASSERT( m_pVal );

	if(m_pItemAmbient) m_pItemAmbient->tSetVal(m_pVal->ambient);
	if(m_pItemSpecular) m_pItemSpecular->tSetVal(m_pVal->specular);
	if(m_pItemDiffuse) m_pItemDiffuse->tSetVal(m_pVal->diffuse);
};

void CXTPGI_RwSurfaceProperties::OnAddChildItem()
{
	ASSERT( m_pVal );

	m_pItemAmbient = (CXTPGI_FloatAmbient*)AddChildItem(new CXTPGI_FloatAmbient(&m_pVal->ambient, m_pVal->ambient));
	m_pItemSpecular = (CXTPGI_FloatSpecular*)AddChildItem(new CXTPGI_FloatSpecular(&m_pVal->specular, m_pVal->specular));
	m_pItemDiffuse = (CXTPGI_FloatDiffuse*)AddChildItem(new CXTPGI_FloatDiffuse(&m_pVal->diffuse, m_pVal->diffuse));
	tUpdateChilds();
}

void CXTPGI_RwSurfaceProperties::SetValue(CString strVal)
{
	tSetVal(tToRwSurfaceProperties(strVal));
};

//--------------------- CXTPGI_UINT8 ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_UINT8, CXTPPropertyGridItem)
CXTPGI_UINT8::CXTPGI_UINT8(CString strCaption, UINT8* pVal, UINT8 init ) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
{
	ASSERT(m_pVal);
	tSetVal(init);
}

void CXTPGI_UINT8::tSetVal(UINT8 val)
{
	ASSERT(m_pVal);

	*m_pVal = val;
	CString	strVal;
	strVal.Format("%d", *m_pVal);
	CXTPPropertyGridItem::SetValue(strVal);
};

void CXTPGI_UINT8::SetValue(CString strValue)
{
	ASSERT( m_pVal );	
	*m_pVal = static_cast<UINT8>(atoi(strValue));
	CXTPPropertyGridItem::SetValue(strValue);
};

//--------------------- CXTPGI_RwRGBA::CXTPGI_ALPHA ---------------------
class CXTPGI_RwRGBA::CXTPGI_ALPHA : public CXTPGI_UINT8
{
public:
	CXTPGI_ALPHA(UINT8* pVal, UINT8 init=0 ) : CXTPGI_UINT8(_T("alpha"), pVal, init)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwRGBA*)m_pParent)->tSetA(static_cast<UINT8>(atoi(strVal)));
	}
};

//--------------------- CXTPGI_RwRGBA::CXTPGI_RED ---------------------
class CXTPGI_RwRGBA::CXTPGI_RED : public CXTPGI_UINT8
{
public:
	CXTPGI_RED(UINT8* pVal, UINT8 init=0 ) : CXTPGI_UINT8(_T("red"), pVal, init)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwRGBA*)m_pParent)->tSetR(static_cast<UINT8>(atoi(strVal)));
	}
};

//--------------------- CXTPGI_RwRGBA::CXTPGI_GREEN ---------------------
class CXTPGI_RwRGBA::CXTPGI_GREEN : public CXTPGI_UINT8
{
public:
	CXTPGI_GREEN(UINT8* pVal, UINT8 init=0 ) : CXTPGI_UINT8(_T("green"), pVal, init)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwRGBA*)m_pParent)->tSetG(static_cast<UINT8>(atoi(strVal)));
	}
};

//--------------------- CXTPGI_RwRGBA::CXTPGI_BLUE ---------------------
class CXTPGI_RwRGBA::CXTPGI_BLUE : public CXTPGI_UINT8
{
public:
	CXTPGI_BLUE(UINT8* pVal, UINT8 init=0 ) : CXTPGI_UINT8(_T("blue"), pVal, init)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_RwRGBA*)m_pParent)->tSetB(static_cast<UINT8>(atoi(strVal)));
	}
};

//--------------------- CXTPGI_RwRGBA ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_RwRGBA, CXTPPropertyGridItemColor)
CXTPGI_RwRGBA::CXTPGI_RwRGBA(CString strCaption, RwRGBA* pVal, RwRGBA init ) : CXTPPropertyGridItemColor(strCaption )
 ,m_pVal(pVal)
 ,m_pItemA(NULL)
 ,m_pItemR(NULL)
 ,m_pItemG(NULL)
 ,m_pItemB(NULL)
{
	ASSERT(m_pVal);

	tSetVal(init);
	m_nFlags = xtpGridItemHasEdit | xtpGridItemHasComboButton;
}

void CXTPGI_RwRGBA::tSetVal(RwRGBA val)
{
	ASSERT( m_pVal );
	m_pVal->alpha = val.alpha;
	m_pVal->red = val.red;
	m_pVal->green = val.green;
	m_pVal->blue = val.blue;

	CXTPPropertyGridItem::SetValue(tToString(val));
	tUpdateChilds();
};

void CXTPGI_RwRGBA::tSetA(UINT8 A)
{
	ASSERT( m_pVal );
	m_pVal->alpha	= A;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwRGBA::tSetR(UINT8 R)
{
	ASSERT( m_pVal );
	m_pVal->red	= R;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwRGBA::tSetG(UINT8 G)
{
	ASSERT( m_pVal );
	m_pVal->green	= G;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_RwRGBA::tSetB(UINT8 B)
{
	ASSERT( m_pVal );
	m_pVal->blue	= B;
	OnValueChanged( tToString(*m_pVal) );
};

CString	CXTPGI_RwRGBA::tToString(RwRGBA val)
{
	CString strVal;
	strVal.Format("%d;%d;%d;%d", val.alpha, val.red, val.green, val.blue );
	return strVal;
};

RwRGBA CXTPGI_RwRGBA::tToRwRGBA(CString strVal)
{
	CString stra, strr, strg, strb;
	AfxExtractSubString(stra, strVal, 0, _T(';'));
	AfxExtractSubString(strr, strVal, 1, _T(';'));
	AfxExtractSubString(strg, strVal, 2, _T(';'));
	AfxExtractSubString(strb, strVal, 3, _T(';'));

	RwRGBA	r = { (UINT8)(atoi(strr)), (UINT8)(atoi(strg)), (UINT8)(atoi(strb)), (UINT8)(atoi(stra)) };
	return r;
};

void CXTPGI_RwRGBA::tUpdateChilds()
{
	ASSERT( m_pVal );

	if(m_pItemA) m_pItemA->tSetVal(m_pVal->alpha);
	if(m_pItemR) m_pItemR->tSetVal(m_pVal->red);
	if(m_pItemG) m_pItemG->tSetVal(m_pVal->green);
	if(m_pItemB) m_pItemB->tSetVal(m_pVal->blue);
};

void CXTPGI_RwRGBA::OnAddChildItem()
{
	ASSERT( m_pVal );

	m_pItemA = (CXTPGI_ALPHA*)AddChildItem(new CXTPGI_ALPHA(&m_pVal->alpha, m_pVal->alpha));
	m_pItemR = (CXTPGI_RED*)AddChildItem(new CXTPGI_RED(&m_pVal->red, m_pVal->red));
	m_pItemG = (CXTPGI_GREEN*)AddChildItem(new CXTPGI_GREEN(&m_pVal->green, m_pVal->green));
	m_pItemB = (CXTPGI_BLUE*)AddChildItem(new CXTPGI_BLUE(&m_pVal->blue, m_pVal->blue));

	tUpdateChilds();
};

void CXTPGI_RwRGBA::SetValue(CString strVal)
{
	tSetVal(tToRwRGBA(strVal));
};

COLORREF RwRGBAToCOLORREF(const RwRGBA* prgba)
{
	if( !prgba )
		return 0x00000000;

	return((prgba->blue << 16) | (prgba->green << 8) | (prgba->red));
};

VOID COLORREFToRwRGBA(RwRGBA* prgba, COLORREF clr)
{
	if( !prgba )	return;

	prgba->red = GetRValue(clr);
	prgba->green = GetGValue(clr);
	prgba->blue = GetBValue(clr);
};

BOOL CXTPGI_RwRGBA::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CXTPPenDC pen(dc, clr);
	CXTPBrushDC brush(dc, RwRGBAToCOLORREF(m_pVal));
	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText( m_strValue, rcText,  DT_SINGLELINE|DT_VCENTER);

	return TRUE;
};

void CXTPGI_RwRGBA::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
	if( !m_pVal )		return;
	COLORREF	clr = RwRGBAToCOLORREF(m_pVal);

#ifdef _INCLUDE_CONTROLS 
	CColorDialog dlg( clr, clr, CPS_XTP_SHOW3DSELECTION|CPS_XTP_SHOWHEXVALUE, m_pGrid );
#else
	CColorDialog dlg( clr, 0, m_pGrid );
#endif

	if ( dlg.DoModal( ) == IDOK )
	{
		COLORREFToRwRGBA(m_pVal, dlg.GetColor());

		OnValueChanged( tToString( *m_pVal ) );
		m_pGrid->Invalidate( FALSE );
	}
};

//--------------------- CXTPGI_UVRect::CXTPGI_L ---------------------
class CXTPGI_UVRect::CXTPGI_L : public CXTPGI_Float
{
public:
	CXTPGI_L(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_UVRect*)m_pParent)->tSetLeft(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_UVRect::CXTPGI_T ---------------------
class CXTPGI_UVRect::CXTPGI_T : public CXTPGI_Float
{
public:
	CXTPGI_T(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_UVRect*)m_pParent)->tSetTop(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_UVRect::CXTPGI_R ---------------------
class CXTPGI_UVRect::CXTPGI_R : public CXTPGI_Float
{
public:
	CXTPGI_R(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_UVRect*)m_pParent)->tSetRight(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_UVRect::CXTPGI_B ---------------------
class CXTPGI_UVRect::CXTPGI_B : public CXTPGI_Float
{
public:
	CXTPGI_B(CString strCaption, float* pVal, float fInit=0.f ) : CXTPGI_Float(strCaption, pVal, fInit)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_UVRect*)m_pParent)->tSetBottom(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_UVRect ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_UVRect, CXTPPropertyGridItem)
CXTPGI_UVRect::CXTPGI_UVRect(CString strCaption, StUVRect* pVal, const StUVRect& initVal) : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemLeft(NULL)
 ,m_pItemTop(NULL)
 ,m_pItemRight(NULL)
 ,m_pItemBottom(NULL)
{
	ASSERT( m_pVal );
	tSetVal(initVal);
};

void CXTPGI_UVRect::tSetVal(const StUVRect& val)
{
	ASSERT(m_pVal);
	*m_pVal = val;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_UVRect::tSetLeft(float left)
{
	ASSERT(m_pVal);
	m_pVal->m_fLeft = left;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_UVRect::tSetTop(float top)
{
	ASSERT(m_pVal);
	m_pVal->m_fTop = top;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_UVRect::tSetRight(float right)
{
	ASSERT(m_pVal);
	m_pVal->m_fRight = right;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_UVRect::tSetBottom(float bottom)
{
	ASSERT(m_pVal);
	m_pVal->m_fBottom = bottom;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_UVRect::tToString(const StUVRect& val)
{
	CString strval;
	strval.Format("%.4f;%.4f;%.4f;%.4f", val.m_fLeft, val.m_fTop, val.m_fRight, val.m_fBottom );
	return strval;
};

StUVRect CXTPGI_UVRect::tToUVRect(CString strVal)
{
	CString strL,strT,strR,strB;
	AfxExtractSubString(strL, strVal, 0, _T(';'));
	AfxExtractSubString(strT, strVal, 1, _T(';'));
	AfxExtractSubString(strR, strVal, 2, _T(';'));
	AfxExtractSubString(strB, strVal, 3, _T(';'));

	StUVRect ret( (float)(atof(strL)) ,(float)(atof(strT)) ,(float)(atof(strR)) ,(float)(atof(strB)) );
	return ret;
};

void CXTPGI_UVRect::tUpdateChilds()
{
	ASSERT( m_pVal );
	if(m_pItemLeft	)	m_pItemLeft	->tSetVal(m_pVal->m_fLeft);
	if(m_pItemTop	)	m_pItemTop->tSetVal(m_pVal->m_fTop);
	if(m_pItemRight	)	m_pItemRight->tSetVal(m_pVal->m_fRight);
	if(m_pItemBottom)	m_pItemBottom->tSetVal(m_pVal->m_fBottom);
};

void CXTPGI_UVRect::OnAddChildItem()
{
	ASSERT( m_pVal );
	m_pItemLeft = (CXTPGI_L*)AddChildItem(new CXTPGI_L(_T("left"), &m_pVal->m_fLeft, m_pVal->m_fLeft ));
	m_pItemTop = (CXTPGI_T*)AddChildItem(new CXTPGI_T(_T("top"), &m_pVal->m_fTop, m_pVal->m_fTop ));
	m_pItemRight = (CXTPGI_R*)AddChildItem(new CXTPGI_R(_T("right"), &m_pVal->m_fRight, m_pVal->m_fRight ));
	m_pItemBottom = (CXTPGI_B*)AddChildItem(new CXTPGI_B(_T("bottom"), &m_pVal->m_fBottom, m_pVal->m_fBottom ));

	tUpdateChilds();
};

void CXTPGI_UVRect::SetValue(CString strValue)
{
	ASSERT( m_pVal );
	*m_pVal = tToUVRect(strValue);
	CXTPPropertyGridItem::SetValue(strValue);
};

//--------------------- CXTPGI_Rev::CXTPGI_Height ---------------------
class CXTPGI_Rev::CXTPGI_Height : public CXTPGI_Float
{
public:
	CXTPGI_Height(float* pVal ) : CXTPGI_Float(_T("height"), pVal, *pVal)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_Rev*)m_pParent)->tSetHeight(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_Rev::CXTPGI_Radius ---------------------
class CXTPGI_Rev::CXTPGI_Radius : public CXTPGI_Float
{
public:
	CXTPGI_Radius(float* pVal ) : CXTPGI_Float(_T("radius"), pVal, *pVal)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_Rev*)m_pParent)->tSetRadius(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_Rev::CXTPGI_Degree ---------------------
class CXTPGI_Rev::CXTPGI_Degree : public CXTPGI_Float
{
public:
	CXTPGI_Degree(float* pVal ) : CXTPGI_Float(_T("degree"), pVal, *pVal)
	{
	};
	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_Rev*)m_pParent)->tSetDegree(static_cast<float>(atof(strVal)));
	};
};

//--------------------- CXTPGI_Rev ---------------------
IMPLEMENT_DYNAMIC(CXTPGI_Rev, CXTPPropertyGridItem)
CXTPGI_Rev::CXTPGI_Rev(CString strCaption, AgcdEffAnim_Rev::LPSTREVOLUTION pVal, const AgcdEffAnim_Rev::STREVOLUTION& initVal)
 : CXTPPropertyGridItem(strCaption)
 ,m_pVal(pVal)
 ,m_pItemHeight(NULL)
 ,m_pItemRadius(NULL)
 ,m_pItemDegree(NULL)
{
	ASSERT( m_pVal );
	tSetVal(initVal);
};

void CXTPGI_Rev::tSetVal(const AgcdEffAnim_Rev::STREVOLUTION& val)
{
	ASSERT(m_pVal);
	*m_pVal = val;
	CXTPPropertyGridItem::SetValue( tToString(*m_pVal) );
	tUpdateChilds();
};

void CXTPGI_Rev::tSetHeight(float height)
{
	ASSERT(m_pVal);
	m_pVal->m_fHeight = height;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_Rev::tSetRadius(float radius)
{
	ASSERT(m_pVal);
	m_pVal->m_fRadius = radius;
	OnValueChanged( tToString(*m_pVal) );
};

void CXTPGI_Rev::tSetDegree(float degree)
{
	ASSERT(m_pVal);
	m_pVal->m_fAngle = degree;
	OnValueChanged( tToString(*m_pVal) );
};

CString CXTPGI_Rev::tToString(const AgcdEffAnim_Rev::STREVOLUTION& val)
{
	CString ret;
	ret.Format("%.4f;%.4f;%.4f",val.m_fHeight,val.m_fRadius,val.m_fAngle);
	return ret;
};

AgcdEffAnim_Rev::STREVOLUTION CXTPGI_Rev::tToStRevolution(CString strVal)
{
	CString strHeight, strRadius, strDegree;
	AfxExtractSubString(strHeight, strVal, 0, _T(';'));
	AfxExtractSubString(strRadius, strVal, 1, _T(';'));
	AfxExtractSubString(strDegree, strVal, 2, _T(';'));

	AgcdEffAnim_Rev::STREVOLUTION ret( (float)(atof(strHeight)), (float)(atof(strRadius)), (float)(atof(strDegree)) );
	return ret;
};

void CXTPGI_Rev::tUpdateChilds()
{
	ASSERT(m_pVal);

	if( m_pItemHeight )	m_pItemHeight->tSetVal(m_pVal->m_fHeight);
	if( m_pItemRadius )	m_pItemRadius->tSetVal(m_pVal->m_fRadius);
	if( m_pItemDegree )	m_pItemDegree->tSetVal(m_pVal->m_fAngle);
};

void CXTPGI_Rev::OnAddChildItem()
{
	ASSERT(m_pVal);	
	m_pItemHeight = (CXTPGI_Height*)AddChildItem(new CXTPGI_Height(&m_pVal->m_fHeight));
	m_pItemRadius = (CXTPGI_Radius*)AddChildItem(new CXTPGI_Radius(&m_pVal->m_fRadius));
	m_pItemDegree = (CXTPGI_Degree*)AddChildItem(new CXTPGI_Degree(&m_pVal->m_fAngle));
};

void CXTPGI_Rev::SetValue(CString strValue)
{
	ASSERT( m_pVal );	
	*m_pVal = tToStRevolution(strValue);
	CXTPPropertyGridItem::SetValue(strValue);
};

//--------------------- CXTPGI_Tex ---------------------
CXTPGI_Tex::CXTPGI_Tex(AgcdEffTex* pTex) : CXTPPropertyGridItem( "EffTex" )
 ,m_pTex(pTex)
{
	ASSERT( m_pTex );
	CString ret;
	ret.Format("index : %d",m_pTex->bGetIndex());
	CXTPPropertyGridItem::SetValue( ret );
}

void CXTPGI_Tex::OnAddChildItem()
{
	ASSERT( m_pTex );
	AddChildItem(new CXTPGI_String(_T("tex"), EFF2_FILE_NAME_MAX, m_pTex->bGetTexName(), m_pTex->bGetTexName() ))->SetReadOnly(TRUE);
	AddChildItem(new CXTPGI_String(_T("mask"), EFF2_FILE_NAME_MAX, m_pTex->bGetMaskName(), m_pTex->bGetMaskName() ))->SetReadOnly(TRUE);
	Expand();
}