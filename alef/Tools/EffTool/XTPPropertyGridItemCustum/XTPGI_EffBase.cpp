// XTPGI_EffBase.cpp: implementation of the CXTPGI_EffBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_EffBase.h"

#include "AgcdEffBase.h"
#include "AgcuEffAnimUtil.h"
#include "AgcuEffPath.h"

#include "../ToolOption.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern const TCHAR*	SZELOOPOPT[e_TblDir_num];

//------------------------- CXTPGI_EffBase ---------------------------
CXTPGI_EffBase::CXTPGI_EffBase(CString strCaption, AgcdEffBase* pEffBase) : CXTPPropertyGridItem(strCaption, pEffBase->bGetPtrTitle())
 , m_pEffBase(pEffBase)
 , m_pItemTitle(NULL)
 , m_pItemDelay(NULL)
 , m_pItemLife(NULL)
 , m_pItemELoopOpt(NULL)
 , m_pItemBool_BILLBOARD(NULL)
 , m_pItemBool_BILLBOARDY(NULL)
 , m_pItemBool_REPEAT(NULL)
{
	ASSERT(m_pEffBase);
	CXTPPropertyGridItem::SetReadOnly(TRUE);
}

void CXTPGI_EffBase::OnAddChildItem()
{
	ASSERT( m_pEffBase );
	if(!m_pEffBase)
	{
		ErrToWnd("!m_pEffBase");
		return;
	}

	CXTPPropertyGridItem*	pBase = AddChildItem(new CXTPPropertyGridItem(_T("-- base --"), _T("catergory")));
	pBase->SetReadOnly(TRUE);

	m_pItemTitle = (CXTPGI_String*)pBase->AddChildItem(new CXTPGI_String(_T("title"),EFF2_BASE_TITLE_MAX_NUM,m_pEffBase->m_szBaseTitle,m_pEffBase->m_szBaseTitle));
	m_pItemDelay = (CXTPGI_UINT*)pBase->AddChildItem(new CXTPGI_UINT(_T("delay"), &m_pEffBase->m_dwDelay, m_pEffBase->m_dwDelay));
	m_pItemLife = (CXTPGI_UINT*)pBase->AddChildItem(new CXTPGI_UINT(_T("lief"), &m_pEffBase->m_dwLife, m_pEffBase->m_dwLife));
	m_pItemELoopOpt = (CXTPGI_Enum<E_LOOPOPT>*)pBase->AddChildItem(new CXTPGI_Enum<E_LOOPOPT>(_T("loop"),e_TblDir_num,m_pEffBase->m_eLoopOpt,&m_pEffBase->m_eLoopOpt,SZELOOPOPT));

	pBase->Expand();

	CXTPPropertyGridItem* pCategory	= pBase->AddChildItem(new CXTPPropertyGridItem(_T("Flag")));
	pCategory->SetReadOnly(TRUE);
	m_pItemBool_BILLBOARD = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffBase->m_dwBitFlags, FLAG_EFFBASE_BILLBOARD, _T("BILLBOARD") ));
	m_pItemBool_BILLBOARD = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffBase->m_dwBitFlags, FLAG_EFFBASE_BILLBOARDY, _T("BILLBOARDY") ));
	m_pItemBool_BILLBOARD = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffBase->m_dwBitFlags, FLAG_EFFBASE_REPEAT, _T("REPEAT") ));
	pCategory->Expand();
};

//------------------------- CXTPGI_EffRenderBase::CXTPGI_InitAngle ---------------------------
class CXTPGI_EffRenderBase::CXTPGI_InitAngle : public CXTPGI_STANGLE
{
public:
	CXTPGI_InitAngle(CString strCaption, tagStAngle* pVal, tagStAngle initVal) : CXTPGI_STANGLE(strCaption, pVal, initVal)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_EffRenderBase*)m_pParent->GetParentItem())->tUpdateMatTrix();
		CXTPPropertyGridItem::OnValueChanged(strVal);
	};
};

//------------------------- CXTPGI_EffRenderBase::CXTPGI_InitPos ---------------------------
class CXTPGI_EffRenderBase::CXTPGI_InitPos : public CXTPGI_RwV3d
{
public:
	CXTPGI_InitPos(CString strCaption, RwV3d* pVal,const RwV3d& initVal) : CXTPGI_RwV3d(strCaption, pVal, initVal)
	{
	};

	virtual void OnValueChanged(CString strVal)
	{
		((CXTPGI_EffRenderBase*)m_pParent->GetParentItem())->tUpdateMatTrix();
		CXTPPropertyGridItem::OnValueChanged(strVal);
	};
};
LPCTSTR	SZEBLENDTYPE[] = {
	"NONE				",

	"ADD_TEXSTAGE		",
	"ADD_ONE_ONE		",
	"ADD_SRCA_ONE		",
	"ADD_SRCA_INVSRCA	",
	"ADD_SRCC_INVSRCC	",

	"REVSUB_ONE_ONE		",
	"REVSUB_SRCA_ONE	",
	"REVSUB_SRCA_INVSRCA",
	"REVSUB_SRCC_INVSRCC",

	"SUB_ONE_ONE		",
	"SUB_SRCA_ONE		",
	"SUB_SRCA_INVSRCA	",
	"SUB_SRCC_INVSRCC	",
};

//------------------------- CXTPGI_EffRenderBase ---------------------------
CXTPGI_EffRenderBase::CXTPGI_EffRenderBase(CString strCaption, AgcdEffRenderBase* pEffRenderBase) : CXTPGI_EffBase(strCaption, pEffRenderBase)
 ,m_pEffRenderBase(pEffRenderBase)
 ,m_pItemEBlendType(NULL)
 ,m_pItemTex(NULL)
 ,m_pItemMask(NULL)
 ,m_pItemInitAngle(NULL)
 ,m_pItemInitPos(NULL)
{
	memset( m_szTex, 0, sizeof( m_szTex ) );
	memset( m_szMask, 0, sizeof( m_szMask ) );

	ASSERT(m_pEffRenderBase);
	if( !T_ISMINUS4(m_pEffRenderBase->m_cEffTexInfo.m_nIndex) )
	{
		if( !m_pEffRenderBase->m_cEffTexInfo.m_lpAgcdEffTex )
		{
			ToWnd( _T("추가된 베이스로서 아직 텍스쳐가 지정되어 있지 안습니다.") );
		}
		else
		{
			strcpy( m_szTex, m_pEffRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szTex );
			strcpy( m_szMask, m_pEffRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szMask );
		}
	}else
		ToWnd(_T("요넘 타입은 오브젝트이어야 합니다...!"));
	
	SetReadOnly(TRUE);
};

void CXTPGI_EffRenderBase::OnAddChildItem()
{
	CXTPGI_EffBase::OnAddChildItem();

	LPCTSTR	TEXFILTER = "(*.tif)|*.tif|(*.bmp)|*.bmp|(*.png)|*.png|(*.dds)|*dds|all (*.tif;*.bmp;*.png;*.dds)|*.tif; *.bmp; *.png; *.dds|";
	ASSERT(m_pEffRenderBase);
	if( !m_pEffRenderBase ){
		ErrToWnd("!m_pEffRenderBase");
		return;
	}
	CXTPPropertyGridItem*	pBase = AddChildItem(new CXTPPropertyGridItem(_T("-- render base --"), _T("catergory")));
	pBase->SetReadOnly(TRUE);

	m_pItemEBlendType = (CXTPGI_Enum<AgcdEffRenderBase::E_EFFBLENDTYPE>*)pBase->AddChildItem(new CXTPGI_Enum<AgcdEffRenderBase::E_EFFBLENDTYPE>(_T("blend mode"), AgcdEffRenderBase::EFFBLEND_NUM, m_pEffRenderBase->m_eBlendType, &m_pEffRenderBase->m_eBlendType, SZEBLENDTYPE));
	
	if( !T_ISMINUS4(m_pEffRenderBase->m_cEffTexInfo.m_nIndex) )
	{
		CXTPGI_FileName*	pTex = (CXTPGI_FileName*)
		m_pItemTex = (CXTPGI_FileName*)pBase->AddChildItem(
			new CXTPGI_FileName(
			_T("tex")
			, EFF2_FILE_NAME_MAX
			, NSUtFn::CSelectedInfo::bGetInst().bGetPtrStrTex()
			, m_pEffRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szTex
			, AgcuEffPath::bGetPath_Tex()
			, TEXFILTER ) );

		TEXFILTER = "(*.bmp)|*.bmp|";
		CXTPGI_FileName*	pMask = (CXTPGI_FileName*)
		m_pItemMask = (CXTPGI_FileName*)pBase->AddChildItem(
			new CXTPGI_FileName(
			_T("mask")
			, EFF2_FILE_NAME_MAX
			, NSUtFn::CSelectedInfo::bGetInst().bGetPtrStrMask()
			, m_pEffRenderBase->m_cEffTexInfo.m_lpAgcdEffTex->m_szMask
			, AgcuEffPath::bGetPath_Tex()
			, TEXFILTER ) );

		NSUtFn::CSelectedInfo::bGetInst().bOnAddCTXPGI_TexMask( pTex, pMask );
	}else{
		ToWnd(_T("요넘 타입은 오브젝트이어야 합니다...!"));
	}
	m_pItemInitAngle = (CXTPGI_InitAngle*)pBase->AddChildItem(new CXTPGI_InitAngle(_T("initAngle"), &m_pEffRenderBase->m_stInitAngle, m_pEffRenderBase->m_stInitAngle));
	m_pItemInitPos = (CXTPGI_InitPos*)pBase->AddChildItem(new CXTPGI_InitPos(_T("initPos"), &m_pEffRenderBase->m_v3dInitPos, m_pEffRenderBase->m_v3dInitPos));

	pBase->Expand();
};

void CXTPGI_EffRenderBase::tUpdateMatTrix(void)
{
	ASSERT(m_pEffRenderBase);
	if( !m_pEffRenderBase )
	{
		ErrToWnd("!m_pEffRenderBase");
		return;
	}

	m_pEffRenderBase->bSetInitAngle(m_pEffRenderBase->m_stInitAngle);
	m_pEffRenderBase->bSetInitPos(m_pEffRenderBase->m_v3dInitPos);
};