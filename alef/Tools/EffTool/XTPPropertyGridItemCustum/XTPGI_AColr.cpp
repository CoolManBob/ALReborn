// XTPGI_AColr.cpp: implementation of the CXTPGI_AColr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_AColr.h"

#include "XTPGI_UINT.h"

#include "AgcuEffPath.h"
#include "AgcdEffAnim.h"

#include "../dlg/dlg_texuvselect.h"
#include "../ToolOption.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-------------------- CXTPGI_AColr::CXTPGI_Color --------------------------
class CXTPGI_AColr::CXTPGI_Color : public CXTPGI_RwRGBA
{
public:
	CXTPGI_Color(CString strCaption, RwRGBA* pRGBA) : CXTPGI_RwRGBA(strCaption, pRGBA, *pRGBA)
	{
	};
};


//-------------------- CXTPGI_AColr --------------------------
CXTPGI_AColr::CXTPGI_AColr(CString strCaption, AgcdEffAnim_Colr* pAnimColr) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimColr )
 , m_pAnimColr(pAnimColr)
{
	ASSERT(m_pAnimColr);
}

void CXTPGI_AColr::OnAddChildItem()
{
	ASSERT(m_pAnimColr);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem( new CXTPPropertyGridItem( _T("-- AgcdEffAnim_Colr --"), _T("category") ) );
	pAnim->SetReadOnly(TRUE);

	int nSize = (int)m_pAnimColr->m_stTblColr.m_stlvecTimeVal.size();
	pAnim->AddChildItem( new CXTPPropertyGridItemNumber( _T("num"), nSize, (long*)&nSize ) );
	pAnim->AddChildItem( new CXTPGI_TTbl<RwRGBA, CXTPGI_Color>( _T("color anim"), &m_pAnimColr->m_stTblColr ) );
	pAnim->Expand();
};

//-------------------- CXTPGI_ATuTv::CXTPGI_UVR --------------------------
class CXTPGI_ATuTv::CXTPGI_UVR : public CXTPGI_UVRect
{
public:
	CXTPGI_UVR(CString strCaption, StUVRect* pUVRect) : CXTPGI_UVRect(strCaption, pUVRect, *pUVRect)	
	{
	};
	
	virtual void OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
	{
		AgcdEffRenderBase*	pRndBase = dynamic_cast<AgcdEffRenderBase*>( NSUtFn::CSelectedInfo::bGetInst().bGetPtrSelectedBase() );
		if( pRndBase )	return;

		CDlg_TexUVSelect	dlgUV( pRndBase->m_cEffTexInfo.bGetPtrTex(), NULL);
		if( IDOK == dlgUV.DoModal() )
		{
			StUVRect	tmp( dlgUV.m_fLeft, dlgUV.m_fTop, dlgUV.m_fRight, dlgUV.m_fBottom );
			CXTPGI_UVRect::tSetVal( tmp );
		}
	};
};

//-------------------- CXTPGI_ATuTv --------------------------
CXTPGI_ATuTv::CXTPGI_ATuTv(CString strCaption, AgcdEffAnim_TuTv* pAnimTuTv) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimTuTv )
 , m_pAnimTuTv(pAnimTuTv)
{
	ASSERT( m_pAnimTuTv );
};

void CXTPGI_ATuTv::OnAddChildItem()
{
	ASSERT(m_pAnimTuTv);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem( new CXTPPropertyGridItem( _T("-- AgcdEffAnim_TuTv --"), _T("category") ) );
	pAnim->SetReadOnly(TRUE);

	CXTPPropertyGridItem* pItem = pAnim->AddChildItem( new CXTPGI_TTbl<StUVRect, CXTPGI_UVR>( _T("uv anim"), &m_pAnimTuTv->m_stTblRect ) );
	pItem->SetReadOnly(TRUE);
	pItem->SetFlags(xtpGridItemHasExpandButton);
	pAnim->Expand();
}

//-------------------- CXTPGI_AMissile --------------------------
CXTPGI_AMissile::CXTPGI_AMissile(CString strCaption, AgcdEffAnim_Missile* pAnimMissile) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimMissile )
 ,m_pAnimMissile(pAnimMissile)
{
	ASSERT(m_pAnimMissile);
}

void CXTPGI_AMissile::OnAddChildItem()
{
	ASSERT(m_pAnimMissile);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_Missile --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	pAnim->AddChildItem(new CXTPGI_Float(_T("speed0"), &m_pAnimMissile->m_fSpeed0, m_pAnimMissile->m_fSpeed0));
	pAnim->AddChildItem(new CXTPGI_Float(_T("accel"), &m_pAnimMissile->m_fAccel, m_pAnimMissile->m_fAccel));
	pAnim->Expand();
};

//-------------------- CXTPGI_ALinear::CXTPGI_Pos --------------------------
class CXTPGI_ALinear::CXTPGI_Pos : public CXTPGI_RwV3d
{
public:
	CXTPGI_Pos(CString strCaption, RwV3d* pV3d) : CXTPGI_RwV3d(strCaption, pV3d, *pV3d)
	{
	};
};

//-------------------- CXTPGI_ALinear --------------------------
CXTPGI_ALinear::CXTPGI_ALinear(CString strCaption, AgcdEffAnim_Linear* pAnimLinear) : CXTPGI_Anim(strCaption, (AgcdEffAnim*)pAnimLinear )
 ,m_pAnimLinear(pAnimLinear)
{
	ASSERT(m_pAnimLinear);
}

void CXTPGI_ALinear::OnAddChildItem()
{
	ASSERT(m_pAnimLinear);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem( new CXTPPropertyGridItem( _T("-- AgcdEffAnim_Linear --"), _T("category") ) );
	pAnim->SetReadOnly(TRUE);
	pAnim->AddChildItem( new CXTPGI_TTbl<RwV3d, CXTPGI_Pos>(_T("linear anim"), &m_pAnimLinear->m_stTblPos) )->SetReadOnly(TRUE);
	pAnim->Expand();
};

//-------------------- CXTPGI_ARev::CXTPGI_R --------------------------
class CXTPGI_ARev::CXTPGI_R : public CXTPGI_Rev
{
public:
	CXTPGI_R(CString strCaption, AgcdEffAnim_Rev::LPSTREVOLUTION pVal) : CXTPGI_Rev(strCaption, pVal, *pVal)
	{
	};
};

//-------------------- CXTPGI_ARev --------------------------
CXTPGI_ARev::CXTPGI_ARev(CString strCaption, AgcdEffAnim_Rev* pAnimRev) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimRev )
 ,m_pAnimRev(pAnimRev)
{
	ASSERT(m_pAnimRev);
};

void CXTPGI_ARev::OnAddChildItem()
{
	ASSERT(m_pAnimRev);

	CXTPGI_Anim::OnAddChildItem();
	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_Rev --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	pAnim->AddChildItem(new CXTPGI_RwV3d(_T("rotAxis"), &m_pAnimRev->m_v3dRotAxis, m_pAnimRev->m_v3dRotAxis));
	pAnim->AddChildItem(new CXTPGI_TTbl<AgcdEffAnim_Rev::STREVOLUTION, CXTPGI_R>(_T("revolution anim"), &m_pAnimRev->m_stTblRev))->SetReadOnly(TRUE);
	pAnim->Expand();
};

//-------------------- CXTPGI_ARot::CXTPGI_Degree --------------------------
class CXTPGI_ARot::CXTPGI_Degree : public CXTPGI_Float
{
public:
	CXTPGI_Degree(CString strCaption, float* pVal) : CXTPGI_Float(strCaption, pVal, *pVal)
	{
	};
};

CXTPGI_ARot::CXTPGI_ARot(CString strCaption, AgcdEffAnim_Rot* pAnimRot) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimRot )
 ,m_pAnimRot(pAnimRot)
{
	ASSERT(m_pAnimRot);
};

void CXTPGI_ARot::OnAddChildItem()
{
	ASSERT(m_pAnimRot);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_Rot --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	pAnim->AddChildItem(new CXTPGI_RwV3d(_T("rotAxis"), &m_pAnimRot->m_v3dRotAxis, m_pAnimRot->m_v3dRotAxis));
	pAnim->AddChildItem(new CXTPGI_TTbl<RwReal, CXTPGI_Degree>(_T("rotation anim"), &m_pAnimRot->m_stTblDeg))->SetReadOnly(TRUE);
	pAnim->Expand();
};

//-------------------- CXTPGI_ARpSpline --------------------------
CXTPGI_ARpSpline::CXTPGI_ARpSpline(CString strCaption, AgcdEffAnim_RpSpline* pAnimSpline) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimSpline )
 ,m_pAnimSpline(pAnimSpline)
{
	ASSERT(m_pAnimSpline);
};

void CXTPGI_ARpSpline::OnAddChildItem()
{
	ASSERT(m_pAnimSpline);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_RpSpline --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	LPCTSTR	strFilter = "(*.rws)|*.rws|";
	pAnim->AddChildItem( new CXTPGI_FileName( _T("file") , EFF2_FILE_NAME_MAX
											,m_pAnimSpline->m_szSplineFName , m_pAnimSpline->m_szSplineFName
											,AgcuEffPath::bGetPath_Anim(), strFilter ) );
	pAnim->Expand();
};

//-------------------- CXTPGI_ARtAnim --------------------------
CXTPGI_ARtAnim::CXTPGI_ARtAnim(CString strCaption, AgcdEffAnim_RtAnim* pAnimRtAnim) : CXTPGI_Anim(strCaption, (AgcdEffAnim*)pAnimRtAnim )
 ,m_pAnimRtAnim(pAnimRtAnim)
{
	ASSERT(m_pAnimRtAnim);
};

void CXTPGI_ARtAnim::OnAddChildItem()
{
	ASSERT(m_pAnimRtAnim);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_RtAnim --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	LPCTSTR	strFilter = "(*.rws)|*.rws|";
	pAnim->AddChildItem( new CXTPGI_FileName( _T("file") , EFF2_FILE_NAME_MAX,
											m_pAnimRtAnim->m_szAnimFName, m_pAnimRtAnim->m_szAnimFName,
											AgcuEffPath::bGetPath_Anim(), strFilter ) );
	pAnim->Expand();
};

//-------------------- CXTPGI_AScale::CXTPGI_Scale --------------------------
class CXTPGI_AScale::CXTPGI_Scale : public CXTPGI_RwV3d
{
public:
	CXTPGI_Scale(CString strCaption, RwV3d* pVal) : CXTPGI_RwV3d(strCaption, pVal, *pVal)
	{
	};
};

//-------------------- CXTPGI_AScale --------------------------
CXTPGI_AScale::CXTPGI_AScale(CString strCaption, AgcdEffAnim_Scale* pAnimScale) : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimScale )
 ,m_pAnimScale(pAnimScale)
{
}

void CXTPGI_AScale::OnAddChildItem()
{
	ASSERT(m_pAnimScale);

	CXTPGI_Anim::OnAddChildItem();

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim_Scale --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	CXTPGI_TTbl<RwV3d, CXTPGI_Scale>* pItem = new CXTPGI_TTbl<RwV3d, CXTPGI_Scale>( _T("scale anim"), &m_pAnimScale->m_stTblScale );
	pAnim->AddChildItem( pItem )->SetReadOnly( TRUE );
	pAnim->Expand();
}

//-------------------- CXTPGI_AParticlePosScale --------------------------
CXTPGI_AParticlePosScale::CXTPGI_AParticlePosScale(CString strCaption, AgcdEffAnim_ParticlePosScale* pAnimParticlePosScale)
 : CXTPGI_Anim( strCaption, (AgcdEffAnim*)pAnimParticlePosScale )
 ,m_pAnimParticlePosScale(pAnimParticlePosScale)
{
};

void CXTPGI_AParticlePosScale::OnAddChildItem()
{
};