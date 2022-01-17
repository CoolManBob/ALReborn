// XTPGI_AColr.h: interface for the CXTPGI_AColr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_ACOLR_H__AAC9F680_6E02_4B08_9655_903EED3D22AD__INCLUDED_)
#define AFX_XTPGI_ACOLR_H__AAC9F680_6E02_4B08_9655_903EED3D22AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_Anim.h"
#include "AgcdEffBase.h"

//-------------------- CXTPGI_AColr --------------------------
class AgcdEffAnim_Colr;
class CXTPGI_AColr : public CXTPGI_Anim
{
	class CXTPGI_Color;
	
public:
	CXTPGI_AColr(CString strCaption, AgcdEffAnim_Colr* pAnimColr);
	virtual ~CXTPGI_AColr(){};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_Colr*	m_pAnimColr;
};

//-------------------- CXTPGI_ATuTv --------------------------
class AgcdEffAnim_TuTv;
class CXTPGI_ATuTv : public CXTPGI_Anim
{
	class CXTPGI_UVR;
	
public:
	CXTPGI_ATuTv(CString strCaption, AgcdEffAnim_TuTv* pAnimTuTv);
	virtual ~CXTPGI_ATuTv(){};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_TuTv*	m_pAnimTuTv;
};

//-------------------- CXTPGI_AMissile --------------------------
class AgcdEffAnim_Missile;
class CXTPGI_AMissile : public CXTPGI_Anim
{
	AgcdEffAnim_Missile*	m_pAnimMissile;

public:
	CXTPGI_AMissile(CString strCaption, AgcdEffAnim_Missile* pAnimMissile);
	virtual ~CXTPGI_AMissile(){};

protected:
	virtual void OnAddChildItem();
};

//-------------------- CXTPGI_ALinear --------------------------
class AgcdEffAnim_Linear;
class CXTPGI_ALinear : public CXTPGI_Anim
{
	class CXTPGI_Pos;
	
public:
	CXTPGI_ALinear(CString strCaption, AgcdEffAnim_Linear* pAnimLinear);
	virtual ~CXTPGI_ALinear(){};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_Linear*	m_pAnimLinear;
};

//-------------------- CXTPGI_ARev --------------------------
class AgcdEffAnim_Rev;
class CXTPGI_ARev : public CXTPGI_Anim
{
	class CXTPGI_R;

public:
	CXTPGI_ARev(CString strCaption, AgcdEffAnim_Rev* pAnimRev);
	virtual ~CXTPGI_ARev(){};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_Rev* m_pAnimRev;
};

//-------------------- CXTPGI_ARot --------------------------
class AgcdEffAnim_Rot;
class CXTPGI_ARot : public CXTPGI_Anim
{
	class CXTPGI_Degree;
	
public:
	CXTPGI_ARot(CString strCaption, AgcdEffAnim_Rot* pAnimRot);
	virtual ~CXTPGI_ARot()		{	};
protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_Rot*	m_pAnimRot;
};

//-------------------- CXTPGI_ARpSpline --------------------------
class AgcdEffAnim_RpSpline;
class CXTPGI_ARpSpline : public CXTPGI_Anim
{
public:
	CXTPGI_ARpSpline(CString strCaption, AgcdEffAnim_RpSpline* m_pAnimSpline);
	virtual ~CXTPGI_ARpSpline()	{	};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_RpSpline*	m_pAnimSpline;
};

//-------------------- CXTPGI_ARtAnim --------------------------
class AgcdEffAnim_RtAnim;
class CXTPGI_ARtAnim : public CXTPGI_Anim
{
public:
	CXTPGI_ARtAnim(CString strCaption, AgcdEffAnim_RtAnim* pAnimRtAnim);
	virtual ~CXTPGI_ARtAnim()	{	};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_RtAnim*	m_pAnimRtAnim;
};

//-------------------- CXTPGI_AScale --------------------------
class AgcdEffAnim_Scale;
class CXTPGI_AScale : public CXTPGI_Anim
{
	class CXTPGI_Scale;
	
public:
	CXTPGI_AScale(CString strCaption, AgcdEffAnim_Scale* pAnimScale);
	virtual ~CXTPGI_AScale()	{	};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_Scale*	m_pAnimScale;
};

//-------------------- CXTPGI_AScale --------------------------
class AgcdEffAnim_ParticlePosScale;
class CXTPGI_AParticlePosScale : public CXTPGI_Anim
{
public:
	CXTPGI_AParticlePosScale(CString strCaption, AgcdEffAnim_ParticlePosScale* pAnimParticlePosScale);
	virtual ~CXTPGI_AParticlePosScale(){};

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim_ParticlePosScale*	m_pAnimParticlePosScale;
};

#endif // !defined(AFX_XTPGI_ACOLR_H__AAC9F680_6E02_4B08_9655_903EED3D22AD__INCLUDED_)
