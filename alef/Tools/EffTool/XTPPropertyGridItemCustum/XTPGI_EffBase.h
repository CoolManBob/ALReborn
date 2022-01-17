// XTPGI_EffBase.h: interface for the CXTPGI_EffBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_EFFBASE_H__7DE71727_CEE0_4604_81AE_584382CBB3C3__INCLUDED_)
#define AFX_XTPGI_EFFBASE_H__7DE71727_CEE0_4604_81AE_584382CBB3C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "XTPGI_UINT.h"
#include "XTPPropertyGridItemEnum.h"
#include "AgcdEffBase.h"

//----------------------- CXTPGI_EffBase -----------------------
enum eTblDir;
class CXTPGI_EffBase : public CXTPPropertyGridItem  
{
public:
	CXTPGI_EffBase(CString strCaption, AgcdEffBase* pEffBase);
	virtual ~CXTPGI_EffBase()	{	}

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffBase*			m_pEffBase;

	CXTPGI_String*			m_pItemTitle;
	CXTPGI_UINT*			m_pItemDelay;
	CXTPGI_UINT*			m_pItemLife;
	CXTPGI_Enum<eTblDir>*	m_pItemELoopOpt;
	
	CXTPGI_FlagBool*		m_pItemBool_BILLBOARD;
	CXTPGI_FlagBool*		m_pItemBool_BILLBOARDY;
	CXTPGI_FlagBool*		m_pItemBool_REPEAT;
};

//----------------------- CXTPGI_EffRenderBase -----------------------
class CXTPGI_EffRenderBase : public CXTPGI_EffBase
{
	class CXTPGI_InitAngle;
	class CXTPGI_InitPos;

	friend class CXTPGI_InitAngle;
	friend class CXTPGI_InitPos;

public:
	CXTPGI_EffRenderBase(CString strCaption, AgcdEffRenderBase* pEffRenderBase);
	virtual ~CXTPGI_EffRenderBase()	{	}

protected:
	virtual void OnAddChildItem();

protected:
	void tUpdateMatTrix(void);

private:
	AgcdEffRenderBase*		m_pEffRenderBase;
	RwChar					m_szTex[EFF2_FILE_NAME_MAX];
	RwChar					m_szMask[EFF2_FILE_NAME_MAX];

	CXTPGI_Enum<AgcdEffRenderBase::E_EFFBLENDTYPE>*	
							m_pItemEBlendType;
	CXTPGI_FileName*		m_pItemTex;
	CXTPGI_FileName*		m_pItemMask;
	CXTPGI_InitAngle*		m_pItemInitAngle;
	CXTPGI_InitPos*			m_pItemInitPos;

};

#endif // !defined(AFX_XTPGI_EFFBASE_H__7DE71727_CEE0_4604_81AE_584382CBB3C3__INCLUDED_)
