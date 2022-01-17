// XTPGI_EffSet.cpp: implementation of the CXTPGI_EffSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPGI_EffSet.h"

#include "AgcdEffSet.h"
#include "AgcuEffPath.h"

#include "../ToolOption.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const TCHAR* SZELOOPOPT[] =
{
	"none",
	"infinity",
	"oned",
	"posi",
	"nega",
	"end",
};

const TCHAR* SZEMISSILEMOVETYPE[] = 
{
	"linear",
	"bezier3",
};

//--------------------- CXTPGI_EffSet::CXTPGI_MissileInfo ----------------------
class CXTPGI_EffSet::CXTPGI_MissileInfo : public CXTPPropertyGridItem
{
public:
	CXTPGI_MissileInfo(CString strCaption, LPSTMISSILEINFO pMissileInfo) : CXTPPropertyGridItem(strCaption, _T("-- CATEGORY --"))
	 ,m_pItemType(NULL)
	 ,m_pItemOffSet(NULL)
	 ,m_pMissileInfo(pMissileInfo)
	{
		ASSERT(m_pMissileInfo);
		CXTPPropertyGridItem::SetReadOnly(TRUE);
	};
	virtual ~CXTPGI_MissileInfo()	{	};

protected:
	virtual void OnAddChildItem()
	{
		m_pItemType = (CXTPGI_Enum<eMissileMoveType>*)AddChildItem(
			new CXTPGI_Enum<eMissileMoveType>(
			_T("type")
			, e_missile_num
			, m_pMissileInfo->m_eMissileType
			, &m_pMissileInfo->m_eMissileType
			, SZEMISSILEMOVETYPE ) );
		m_pItemOffSet = (CXTPGI_RwV3d*)AddChildItem(
			new CXTPGI_RwV3d(
			_T("offset")
			, &m_pMissileInfo->m_v3dOffset
			, m_pMissileInfo->m_v3dOffset.x
			, m_pMissileInfo->m_v3dOffset.y
			, m_pMissileInfo->m_v3dOffset.z));
		m_pItemType->Expand();
		m_pItemOffSet->Expand();
	};

private:
	LPSTMISSILEINFO					m_pMissileInfo;
	CXTPGI_Enum<eMissileMoveType>*	m_pItemType;
	CXTPGI_RwV3d*					m_pItemOffSet;
};

//-----------------------------------------------------------------------------
// class CXTPGI_EffSet::CXTPGI_VarSize
//-----------------------------------------------------------------------------
class CXTPGI_EffSet::CXTPGI_VarSize : public CXTPPropertyGridItem
{
public:
	CXTPGI_VarSize(CString strCaption, AgcdEffSet::StVarSizeInfo* pVarSize) : CXTPPropertyGridItem(strCaption, _T("-- CATEGORY --"))
	 ,m_pItemNumTex(NULL)
	 ,m_pItemNumBase(NULL)
	 ,m_pItemNumDpnd(NULL)
	 ,m_pVarSize(pVarSize)
	{
		ASSERT(m_pVarSize);
		CXTPPropertyGridItem::SetReadOnly(TRUE);
	}
	virtual ~CXTPGI_VarSize(){};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT(m_pVarSize);
		m_pItemNumTex = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("num of tex"), m_pVarSize->m_nNumOfTex, (long*)&m_pVarSize->m_nNumOfTex ) );
		m_pItemNumBase = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("num of base"), m_pVarSize->m_nNumOfBase, (long*)&m_pVarSize->m_nNumOfBase ) );
		m_pItemNumDpnd = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("num of depandancy"), m_pVarSize->m_nNumOfBaseDependancy, (long*)&m_pVarSize->m_nNumOfBaseDependancy ) );
		m_pItemNumTex->SetReadOnly(TRUE);
		m_pItemNumBase->SetReadOnly(TRUE);
		m_pItemNumDpnd->SetReadOnly(TRUE);
	}

private:
	CXTPPropertyGridItemNumber*	m_pItemNumTex;
	CXTPPropertyGridItemNumber*	m_pItemNumBase;
	CXTPPropertyGridItemNumber*	m_pItemNumDpnd;
	AgcdEffSet::StVarSizeInfo*	m_pVarSize;
};

//-----------------------------------------------------------------------------
// class CXTPGI_EffSet::CXTPGI_EffTex
//-----------------------------------------------------------------------------
class CXTPGI_EffSet::CXTPGI_EffTex : public CXTPPropertyGridItem
{
public:
	CXTPGI_EffTex(CString strCaption, LPEFFTEX pEffTex)
		: CXTPPropertyGridItem(strCaption, _T("-- CATEGORY --"))
		, m_pItemIndex(NULL)
		, m_pItemTex(NULL)
		, m_pItemMask(NULL)
		, m_pEffTex(pEffTex)
	{
		ASSERT( m_pEffTex );
		CXTPPropertyGridItem::SetReadOnly(TRUE);
	};
	virtual ~CXTPGI_EffTex(){};

protected:
	virtual void OnAddChildItem()
	{
		LPCTSTR	TEXFILTER	= "(*.tif)|*.tif|(*.bmp)|*.bmp|(*.png)|*.png|(*.dds)|*dds|all (*.tif;*.bmp;*.png;*.dds)|*.tif; *.bmp; *.png; *.dds|";
		ASSERT(m_pEffTex);
		m_pItemIndex = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("index"), m_pEffTex->m_nIndex, (LONG*)&m_pEffTex->m_nIndex ) );
		m_pItemIndex->SetReadOnly(TRUE);
		m_pItemTex = (CXTPGI_FileName*)AddChildItem(new CXTPGI_FileName(_T("tex"), EFF2_FILE_NAME_MAX, m_pEffTex->m_szTex, m_pEffTex->m_szTex, AgcuEffPath::bGetPath_Tex(), TEXFILTER ) );
		m_pItemMask = (CXTPGI_FileName*)AddChildItem(new CXTPGI_FileName(_T("mask"), EFF2_FILE_NAME_MAX, m_pEffTex->m_szMask, m_pEffTex->m_szMask, AgcuEffPath::bGetPath_Tex(), TEXFILTER  ) );
	}

private:
	CXTPPropertyGridItemNumber*	m_pItemIndex;
	CXTPGI_FileName*			m_pItemTex;
	CXTPGI_FileName*			m_pItemMask;
	LPEFFTEX					m_pEffTex;
};

//------------------------------- CXTPGI_DPND ------------------------------- 
void CXTPGI_DPND::OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
{
	NSUtFn::CSelectedInfo::bGetInst().bDelEffBaseDpnd( m_nIndex );
}

//------------------------------- CXTPGI_EffSet ------------------------------- 
CXTPGI_EffSet::CXTPGI_EffSet(CString strCaption, AgcdEffSet* pEffSet) : CXTPPropertyGridItem(strCaption, pEffSet->bGetTitle())
 , m_pEffSet( pEffSet )
{
	ASSERT(pEffSet);
	CXTPPropertyGridItem::SetReadOnly(TRUE);
}

CXTPGI_EffSet::~CXTPGI_EffSet()
{
}

void CXTPGI_EffSet::OnAddChildItem()
{
	ASSERT(m_pEffSet);
	if( !m_pEffSet )		return;
		
	m_pItemID	= (CXTPGI_UINT*)AddChildItem( new CXTPGI_UINT( _T("ID"), &m_pEffSet->m_dwEffSetID,m_pEffSet->m_dwEffSetID ) );
	m_pItemID->SetReadOnly(TRUE);
	m_pItemTitle = (CXTPGI_String*)AddChildItem(new CXTPGI_String(_T("title"),EFF2_BASE_TITLE_MAX_NUM,m_pEffSet->m_szTitle,m_pEffSet->m_szTitle));
	m_pItemLife = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("life"), &m_pEffSet->m_dwEffSetLife,m_pEffSet->m_dwEffSetLife));
	m_pItemELoopOpt = (CXTPGI_Enum<E_LOOPOPT>*)AddChildItem(new CXTPGI_Enum<E_LOOPOPT>(_T("loop"),e_TblDir_num,m_pEffSet->m_eLoopOpt,&m_pEffSet->m_eLoopOpt,SZELOOPOPT));

	m_pItemMissileInfo = (CXTPGI_MissileInfo*)AddChildItem(new CXTPGI_MissileInfo(_T("missile info"), &m_pEffSet->m_stMissileInfo));
	m_pItemRwSphere = (CXTPGI_RwSphere*)AddChildItem(new CXTPGI_RwSphere(_T("sphere"), &m_pEffSet->m_BSphere, m_pEffSet->m_BSphere.radius, m_pEffSet->m_BSphere.center.x, m_pEffSet->m_BSphere.center.y, m_pEffSet->m_BSphere.center.z));
	m_pItemRwBBox = (CXTPGI_RwBBox*)AddChildItem(new CXTPGI_RwBBox(_T("box"), &m_pEffSet->m_BBox, m_pEffSet->m_BBox));
	m_pItemVarSize = (CXTPGI_VarSize*)AddChildItem(new CXTPGI_VarSize(_T("var size"), &m_pEffSet->m_stVarSizeInfo));
	m_pItemVarSize->SetReadOnly(TRUE);
	
	CXTPPropertyGridItem*	pParent	= (CXTPPropertyGridItem*)GetParentItem();
	if( !pParent )
	{
		ErrToWnd( _T("!pParent") );
		return;
	}
	CXTPPropertyGrid*		pGrid	= (CXTPPropertyGrid*)pParent->GetGrid();
	if( !pGrid )
	{
		ErrToWnd( _T("!pGrid") );
		return;
	}

	CXTPPropertyGridItem* pCategory	= AddChildItem(new CXTPPropertyGridItem(_T("Flag")));
	pCategory->SetReadOnly(TRUE);
	m_pItemBool_BILLBOARD = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_BILLBOARD, _T("flag billboard") ));
	m_pItemBool_TARGET = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_TARGET, _T("flag target") ));
	m_pItemBool_ONLYTARGET = (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool(&m_pEffSet->m_dwEffSetBitFlags, FLAG_EFFSET_ONLYTARGET, _T("flag onlytarget") ));
	pCategory->Expand();

	pCategory	= AddChildItem( new CXTPPropertyGridItem(_T("Dependancy")) );
	pCategory->SetReadOnly(TRUE);

	int	num = m_pEffSet->m_vecBaseDependancy.size();
	for( int index = 0; index < num; ++index )
	{
		AgcdEffSet::stBaseDependancy& node = m_pEffSet->m_vecBaseDependancy[index];
		CString strCaption;
		strCaption.Format("dpnd:%d", index);
		CXTPGI_DPND* pAdded = (CXTPGI_DPND*)pCategory->AddChildItem( new CXTPGI_DPND( strCaption, &node, index ) );
		pAdded->SetFlags( xtpGridItemHasExpandButton );
	};
	pCategory->Expand();

	pCategory	= AddChildItem(new CXTPPropertyGridItem(_T("using texture")));
	pCategory->SetReadOnly(TRUE);
	num = m_pEffSet->m_vecLPEffectEx.size();
	for( int index = 0; index < num; ++index )
	{
		LPEFFTEX pTex = m_pEffSet->bGetPtrEffTex(index);
		CXTPGI_Tex* ptm	= (CXTPGI_Tex*)pCategory->AddChildItem( new CXTPGI_Tex( pTex ) );
		ptm->SetReadOnly(TRUE);
	}
	pCategory->Expand();

	RwChar	szErr[1024] = "";
	if( !m_pEffSet->bForTool_IsAllTexUsed(szErr) )
		ErrToWnd(szErr);
};
