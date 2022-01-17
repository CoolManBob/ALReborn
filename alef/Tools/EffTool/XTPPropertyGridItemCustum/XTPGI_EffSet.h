// XTPGI_EffSet.h: interface for the CXTPGI_EffSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_EFFSET_H__2CC7E76C_9244_495F_8FBF_5931AAEA7C6C__INCLUDED_)
#define AFX_XTPGI_EFFSET_H__2CC7E76C_9244_495F_8FBF_5931AAEA7C6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_UINT.h"
#include "XTPPropertyGridItemEnum.h"
#include "AgcuEffAnimUtil.h"

#include "AgcdEffSet.h"

class AgcdEffSet;

//------------------------ CXTPGI_DPND ------------------------
class CXTPGI_DPND : public CXTPPropertyGridItem
{
public:
	CXTPGI_DPND(CString strCaption, AgcdEffSet::stBaseDependancy* pDpnd, INT dpndIndex) : CXTPPropertyGridItem(strCaption, _T("-- CATEGORY --"))
	 , m_pItemParent(NULL)
	 , m_pItemChild(NULL)
	 , m_pItemBool_OnlyPos(NULL)
	 , m_pItemBool_HASNODE(NULL)
	 , m_pDpnd(pDpnd)
	 , m_nIndex( dpndIndex )
	{
		ASSERT(m_pDpnd);
	};

	virtual ~CXTPGI_DPND()	{	};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT( m_pDpnd );
		ASSERT( sizeof(long) == sizeof(int) );

		m_pItemParent = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("parent"), m_pDpnd->GetParentIndexItself(), reinterpret_cast<long*>(m_pDpnd->ForTool_GetPtrParentIndex())) );
		m_pItemChild = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("child"), m_pDpnd->GetChildIndex(), reinterpret_cast<long*>(m_pDpnd->ForTool_GetPtrChildIndex())) );
		m_pItemParent->SetReadOnly(TRUE);
		m_pItemChild->SetReadOnly(TRUE);

		m_pItemBool_OnlyPos = (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(m_pDpnd->ForTool_GetPtrFlag(), FLAG_EFFBASE_DEPENDANCY_ONLYPOS, _T("flag onlypos") ));
		m_pItemBool_HASNODE = (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(m_pDpnd->ForTool_GetPtrFlag(), FLAG_EFFBASE_DEPENDANCY_HASNODE, _T("flag hasnode") ));
		m_pItemBool_HASNODE->SetReadOnly(TRUE);
	}
	
	virtual void OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton );

public:
	CXTPPropertyGridItemNumber*		m_pItemParent;
	CXTPPropertyGridItemNumber*		m_pItemChild;
	
	CXTPGI_FlagBool*				m_pItemBool_OnlyPos;
	CXTPGI_FlagBool*				m_pItemBool_HASNODE;

private:
	AgcdEffSet::stBaseDependancy*	m_pDpnd;
	INT								m_nIndex;
};

class CXTPGI_EffSet : public CXTPPropertyGridItem  
{
public:
	class CXTPGI_MissileInfo;
	class CXTPGI_VarSize;
	class CXTPGI_EffTex;


public:
	CXTPGI_EffSet(CString strCaption, AgcdEffSet* pEffSet);
	virtual ~CXTPGI_EffSet();

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffSet*					m_pEffSet;

	CXTPGI_UINT*				m_pItemID;
	CXTPGI_String*				m_pItemTitle;
	CXTPGI_UINT*				m_pItemLife;
	CXTPGI_Enum<E_LOOPOPT>*		m_pItemELoopOpt;

	//flag setting
	CXTPGI_FlagBool*			m_pItemBool_BILLBOARD	;
	CXTPGI_FlagBool*			m_pItemBool_TARGET		;
	CXTPGI_FlagBool*			m_pItemBool_ONLYTARGET	;

	CXTPGI_MissileInfo*			m_pItemMissileInfo;
	CXTPGI_RwSphere*			m_pItemRwSphere;
	CXTPGI_RwBBox*				m_pItemRwBBox;
	CXTPGI_VarSize*				m_pItemVarSize;
};

#endif // !defined(AFX_XTPGI_EFFSET_H__2CC7E76C_9244_495F_8FBF_5931AAEA7C6C__INCLUDED_)
