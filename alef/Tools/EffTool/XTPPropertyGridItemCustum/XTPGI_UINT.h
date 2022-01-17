// XTPGI_UINT.h: interface for the CXTPGI_UINT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_UINT_H__E7B59F9D_19C4_40F4_8E90_FEAEE51B2EBC__INCLUDED_)
#define AFX_XTPGI_UINT_H__E7B59F9D_19C4_40F4_8E90_FEAEE51B2EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcdEffAnim.h"

//--------------------- CXTPGI_UINT ---------------------
class CXTPGI_UINT : public CXTPPropertyGridItem  
{
	DECLARE_DYNAMIC(CXTPGI_UINT);

public:
	CXTPGI_UINT(CString strCaption, UINT* pBind, UINT initVal=0LU );
	virtual ~CXTPGI_UINT()		{	}

protected:
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(UINT ulVal);

protected:
	UINT*	m_pVal;
};

//--------------------- CXTPGI_String ---------------------
class CXTPGI_String : public CXTPPropertyGridItem
{
	DECLARE_DYNAMIC(CXTPGI_String);

public:
	CXTPGI_String(CString strCaption, INT nMaxLen, LPTSTR szVal, LPCTSTR szInit="" );
	virtual ~CXTPGI_String()	{	}

protected:
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(CString strVal);

private:
	const INT	m_nMaxLen;
	LPTSTR		m_szVal;
};

//--------------------- CXTPGI_FileName ---------------------
class CXTPGI_FileName : public CXTPGI_String
{
	DECLARE_DYNAMIC(CXTPGI_FileName);

public:
	CXTPGI_FileName(CString strCaption, INT nMaxLen, LPTSTR szVal, LPCTSTR szInit, LPCTSTR szDir, LPCTSTR szFilter=NULL);
	virtual ~CXTPGI_FileName()	{	}

protected:	
	virtual void OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );

private:
	CString		m_strDir;
	LPCTSTR		m_szFilter;
};

//--------------------- CXTPGI_Float ---------------------
class CXTPGI_Rev;
class CXTPGI_UVRect;
class CXTPGI_RwV3d;
class CXTPGI_RwSphere;
class CXTPGI_STANGLE;
class CXTPGI_RwSurfaceProperties;
class CXTPGI_Float : public CXTPPropertyGridItem
{
	friend class CXTPGI_Rev;
	friend class CXTPGI_UVRect;
	friend class CXTPGI_RwV3d;
	friend class CXTPGI_RwSphere;
	friend class CXTPGI_STANGLE;
	friend class CXTPGI_RwSurfaceProperties;

	DECLARE_DYNAMIC(CXTPGI_Float);

public:
	CXTPGI_Float(CString strCaption, float* pVal, float fInit=0.f );
	virtual ~CXTPGI_Float()	{	}

protected:
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(float fVal);

private:
	float*	m_pVal;	
};

//--------------------- CXTPGI_STANGLE ---------------------
struct tagStAngle;
class CXTPGI_STANGLE : public CXTPPropertyGridItem
{
	class	CXTPGI_FloatYaw;
	class	CXTPGI_FloatPitch;
	class	CXTPGI_FloatRoll;

	friend class CXTPGI_FloatYaw;
	friend class CXTPGI_FloatPitch;
	friend class CXTPGI_FloatRoll;

	DECLARE_DYNAMIC(CXTPGI_STANGLE);

public:
	CXTPGI_STANGLE(CString strCaption, tagStAngle* pVal, tagStAngle initVal);
	virtual ~CXTPGI_STANGLE()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(const tagStAngle& stAngle);
	void	tSetYaw(float yaw);
	void	tSetPitch(float pitch);
	void	tSetRoll(float roll);
	void	tUpdateChilds();
	CString	tToString(const tagStAngle& stAngle);
	tagStAngle	tToSTANGLE(CString strVal);

private:
	tagStAngle*		m_pVal;
	CXTPGI_Float*	m_pItemYaw;
	CXTPGI_Float*	m_pItemPitch;
	CXTPGI_Float*	m_pItemRoll;
};

//--------------------- CXTPGI_RwV3d ---------------------
class CXTPGI_RwBBox;
struct RwV3d;
class CXTPGI_RwV3d : public CXTPPropertyGridItem
{
	class	CXTPGI_FloatX;
	class	CXTPGI_FloatY;
	class	CXTPGI_FloatZ;

	friend class CXTPGI_FloatX;
	friend class CXTPGI_FloatY;
	friend class CXTPGI_FloatZ;
	friend class CXTPGI_RwSphere;
	friend class CXTPGI_RwBBox;

	DECLARE_DYNAMIC(CXTPGI_RwV3d);

	RwV3d*			m_pVal;
	CXTPGI_Float*	m_pItemX;
	CXTPGI_Float*	m_pItemY;
	CXTPGI_Float*	m_pItemZ;

public:
	CXTPGI_RwV3d(CString strCaption, RwV3d* pVal, float x=0.f, float y=0.f, float z=0.f);
	CXTPGI_RwV3d(CString strCaption, RwV3d* pVal, const RwV3d& vInit);
	virtual ~CXTPGI_RwV3d()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(const RwV3d& v3dVal);
	void	tSetVal(float x, float y, float z);
	void	tSetX(float x);
	void	tSetY(float y);
	void	tSetZ(float z);
	void	tUpdateChilds();
	CString	tToString(const RwV3d& v3dVal);
	RwV3d	tToRwV3d(CString strVal);
};

//--------------------- CXTPGI_RwSphere ---------------------
struct RwSphere;
class CXTPGI_RwSphere : public CXTPPropertyGridItem
{
	class	CXTPGI_FloatRadius;
	class	CXTPGI_RwV3dCenter;

	friend class CXTPGI_FloatRadius;
	friend class CXTPGI_RwV3dCenter;

	DECLARE_DYNAMIC(CXTPGI_RwSphere);

public:
	CXTPGI_RwSphere(CString strCaption, RwSphere* pVal, float radius=0.f, float cx=0.f, float cy=0.f, float cz=0.f);
	virtual ~CXTPGI_RwSphere()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	
protected:
	void		tSetVal(const RwSphere& sphere);
	void		tSetVal(float radius, const RwV3d& center);
	void		tSetRadius(float radius);
	void		tSetCenter(const RwV3d& center);
	void		tUpdateChilds();
	CString		tToString(const RwSphere& sphere);
	RwSphere	tToRwSphere(CString strVal);

	RwSphere*			m_pVal;
	CXTPGI_FloatRadius*	m_pItemRadius;
	CXTPGI_RwV3dCenter*	m_pItemCenter;
};

//--------------------- CXTPGI_RwBBox ---------------------
struct RwBBox;
class CXTPGI_RwBBox : public CXTPPropertyGridItem
{
	class	CXTPGI_RwV3dSup;
	class	CXTPGI_RwV3dInf;

	friend class CXTPGI_RwV3dSup;
	friend class CXTPGI_RwV3dInf;

	DECLARE_DYNAMIC(CXTPGI_RwBBox);

public:
	CXTPGI_RwBBox(CString strCaption, RwBBox* pVal, const RwBBox& bbox);
	virtual ~CXTPGI_RwBBox()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strVal);

protected:
	void	tSetVal(const RwBBox& bbox);
	void	tSetVal(const RwV3d& sup, const RwV3d& inf);
	void	tSetSup(const RwV3d& sup);
	void	tSetInf(const RwV3d& inf);
	void	tUpdateChilds();
	CString tToString(const RwBBox& bbox);
	RwBBox	tToRwBBox(CString strVal);

private:
	RwBBox*				m_pVal;
	CXTPGI_RwV3dSup*	m_pItemSup;
	CXTPGI_RwV3dInf*	m_pItemInf;
};

//--------------------- CXTPGI_FlagBool ---------------------
class CXTPGI_FlagBool : public CXTPPropertyGridItemBool
{
	DECLARE_DYNAMIC(CXTPGI_FlagBool);

public:
	CXTPGI_FlagBool(UINT* pFlag, const UINT FLAG, CString strCaption, BOOL* pBindBool = NULL);
	virtual ~CXTPGI_FlagBool()	{	}

protected:
	virtual void SetValue(CString strVal);

private:
	UINT*		m_pulFlag;
	const UINT	THEFLAG;
};

//--------------------- CXTPGI_RwSurfaceProperties ---------------------
struct RwSurfaceProperties;
class CXTPGI_RwSurfaceProperties : public CXTPPropertyGridItem
{
	class	CXTPGI_FloatAmbient;
	class	CXTPGI_FloatSpecular;
	class	CXTPGI_FloatDiffuse;

	friend class CXTPGI_FloatAmbient;
	friend class CXTPGI_FloatSpecular;
	friend class CXTPGI_FloatDiffuse;

	DECLARE_DYNAMIC(CXTPGI_RwSurfaceProperties);

public:
	CXTPGI_RwSurfaceProperties(CString strCaption, RwSurfaceProperties* pBind, const RwSurfaceProperties& initVal );
	virtual ~CXTPGI_RwSurfaceProperties()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(const RwSurfaceProperties& val);

	void	tSetAmbient(float ambient);
	void	tSetSpecular(float specular);
	void	tSetDiffuse(float diffuse);
	void	tUpdateChilds();
	CString	tToString(const RwSurfaceProperties& val);
	RwSurfaceProperties	tToRwSurfaceProperties(CString strVal);

private:
	RwSurfaceProperties*	m_pVal;

	CXTPGI_FloatAmbient*	m_pItemAmbient;
	CXTPGI_FloatSpecular*	m_pItemSpecular;
	CXTPGI_FloatDiffuse*	m_pItemDiffuse;
};

//--------------------- CXTPGI_UINT8 ---------------------
class CXTPGI_RwRGBA;
class CXTPGI_UINT8 : public CXTPPropertyGridItem
{
	friend class CXTPGI_RwRGBA;

	DECLARE_DYNAMIC(CXTPGI_UINT8);

public:
	CXTPGI_UINT8(CString strCaption, UINT8* pBind, UINT8 initVal=0 );
	virtual ~CXTPGI_UINT8()	{	}

protected:
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(UINT8 val);

private:
	UINT8*	m_pVal;
};

//--------------------- CXTPGI_RwRGBA ---------------------
struct RwRGBA;
class CXTPGI_RwRGBA : public CXTPPropertyGridItemColor
{
	DECLARE_DYNAMIC(CXTPGI_RwRGBA);

	class CXTPGI_ALPHA;
	class CXTPGI_RED;
	class CXTPGI_GREEN;
	class CXTPGI_BLUE;

	friend class CXTPGI_ALPHA;
	friend class CXTPGI_RED;
	friend class CXTPGI_GREEN;
	friend class CXTPGI_BLUE;

public:
	CXTPGI_RwRGBA(CString strCaption, RwRGBA* pBind, RwRGBA initVal );
	virtual ~CXTPGI_RwRGBA()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual void OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );

protected:
	void	tSetVal(RwRGBA val);
	void	tSetA(UINT8 A);
	void	tSetR(UINT8 R);
	void	tSetG(UINT8 G);
	void	tSetB(UINT8 B);
	void	tUpdateChilds();
	CString	tToString(RwRGBA val);
	RwRGBA	tToRwRGBA(CString strVal);

private:
	RwRGBA*			m_pVal;

	CXTPGI_ALPHA*	m_pItemA;
	CXTPGI_RED*		m_pItemR;
	CXTPGI_GREEN*	m_pItemG;
	CXTPGI_BLUE*	m_pItemB;
};

//--------------------- CXTPGI_UVRect ---------------------
struct StUVRect;
class CXTPGI_UVRect : public CXTPPropertyGridItem
{
	class CXTPGI_L;
	class CXTPGI_T;
	class CXTPGI_R;
	class CXTPGI_B;

	friend class CXTPGI_L;
	friend class CXTPGI_T;
	friend class CXTPGI_R;
	friend class CXTPGI_B;

	DECLARE_DYNAMIC(CXTPGI_UVRect);

public:
	CXTPGI_UVRect(CString strCaption, StUVRect* pUVRect, const StUVRect& initVal);
	virtual ~CXTPGI_UVRect()	{	}

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(const StUVRect& val);
	void	tSetLeft(float left);
	void	tSetTop(float top);
	void	tSetRight(float right);
	void	tSetBottom(float bottom);
	void	tUpdateChilds();
	CString tToString(const StUVRect& val);
	StUVRect tToUVRect(CString strVal);

private:
	StUVRect*	m_pVal;
	CXTPGI_L*		m_pItemLeft;
	CXTPGI_T*		m_pItemTop;
	CXTPGI_R*		m_pItemRight;
	CXTPGI_B*		m_pItemBottom;
};

//--------------------- CXTPGI_Rev ---------------------
class CXTPGI_Rev : public CXTPPropertyGridItem
{
	class CXTPGI_Height;
	class CXTPGI_Radius;
	class CXTPGI_Degree;

	friend class CXTPGI_Height;
	friend class CXTPGI_Radius;
	friend class CXTPGI_Degree;

	DECLARE_DYNAMIC(CXTPGI_Rev);

public:
	CXTPGI_Rev(CString strCaption, AgcdEffAnim_Rev::LPSTREVOLUTION pVal, const AgcdEffAnim_Rev::STREVOLUTION& initVal);
	virtual ~CXTPGI_Rev()	{	};

protected:
	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);

protected:
	void	tSetVal(const AgcdEffAnim_Rev::STREVOLUTION& val);
	void	tSetHeight(float height);
	void	tSetRadius(float radius);
	void	tSetDegree(float degree);
	void	tUpdateChilds();
	CString tToString(const AgcdEffAnim_Rev::STREVOLUTION& val);

	AgcdEffAnim_Rev::STREVOLUTION		tToStRevolution(CString strVal);

private:
	AgcdEffAnim_Rev::LPSTREVOLUTION		m_pVal;
					
	CXTPGI_Height*		m_pItemHeight;
	CXTPGI_Radius*		m_pItemRadius;
	CXTPGI_Degree*		m_pItemDegree;
};

//--------------------- CXTPGI_Tex ---------------------
class AgcdEffTex;
class CXTPGI_Tex : public CXTPPropertyGridItem
{
public:
	CXTPGI_Tex(AgcdEffTex*	pTex);
	virtual ~CXTPGI_Tex()	{	};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffTex*	m_pTex;
};
#endif // !defined(AFX_XTPGI_UINT_H__E7B59F9D_19C4_40F4_8E90_FEAEE51B2EBC__INCLUDED_)
