// XTPGI_Anim.h: interface for the CXTPGI_Anim class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_ANIM_H__B6D1D4D7_0DB8_47E2_B9C5_39B024A2FBA6__INCLUDED_)
#define AFX_XTPGI_ANIM_H__B6D1D4D7_0DB8_47E2_B9C5_39B024A2FBA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcuEffAnimUtil.h"
#include "XTPGI_UINT.h"

#define XTP_PGN_ITEMVALUE_DELANIMITEM_BTN	8055
#define XTP_PGN_ITEMVALUE_CHANGED_TIME		8056

//----------------------------- CXTPGI_Anim ------------------------------------
class AgcdEffAnim;
class CXTPGI_Anim : public CXTPPropertyGridItem  
{
public:
	CXTPGI_Anim(CString strCaption, AgcdEffAnim* pAnim);
	virtual ~CXTPGI_Anim();

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffAnim*	m_pAnim;
};

//----------------------------- CXTPGI_TIME ------------------------------------
class CXTPGI_TIME : public CXTPGI_UINT
{
public:
	CXTPGI_TIME(CString strCaption, UINT* pBind, UINT initVal=0LU ) : CXTPGI_UINT( strCaption, pBind, initVal )	{ }
	virtual ~CXTPGI_TIME()	{	};

	virtual void OnValueChanged(CString strValue)
	{
		SetValue(strValue);
		m_pGrid->Invalidate(FALSE);
		m_pGrid->GetParent()->GetOwner()->SendMessage(XTPWM_PROPERTYGRID_NOTIFY, XTP_PGN_ITEMVALUE_CHANGED_TIME, (LPARAM)NULL);
	}
};

//----------------------------- class CXTPGI_TVal ------------------------------------
template<class T, class CXTPGI_T>
class CXTPGI_TVal : public CXTPPropertyGridItem
{
	typename stTimeTable<T>::stTimeVal* m_pVal;

public:
	CXTPGI_TVal(INT nIndex, typename stTimeTable<T>::stTimeVal* pVal) : CXTPPropertyGridItem( _T("timeVal") )
	 , m_pVal(pVal)
	 , m_pItemTime(NULL)
	{
		ASSERT(m_pVal);

		TCHAR	buff[10] = "";
		_itot( nIndex, buff,10 );
		CXTPPropertyGridItem::SetValue( buff );
	};

	virtual ~CXTPGI_TVal(){};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT(m_pVal);

		m_pItemTime = AddChildItem( new CXTPGI_TIME(_T("time"), &m_pVal->m_dwTime, m_pVal->m_dwTime) );
		m_pItemTime->Expand();
				
		CXTPPropertyGridItem* pItem = AddChildItem( new CXTPGI_T( _T("value"), &m_pVal->m_tVal ) );
		pItem->SetFlags(xtpGridItemHasEdit | xtpGridItemHasExpandButton);
	}

	virtual void OnInplaceButtonDown( CXTPPropertyGridInplaceButton* pButton )
	{
		m_pGrid->GetParent()->GetOwner()->SendMessage(XTPWM_PROPERTYGRID_NOTIFY, XTP_PGN_ITEMVALUE_DELANIMITEM_BTN, (LPARAM)m_pItemTime);
	};

private:
	CXTPPropertyGridItem*			 m_pItemTime;
};

//----------------------------- class CXTPGI_TTbl ------------------------------------
template<class T, class CXTPGI_T>
class CXTPGI_TTbl : public CXTPPropertyGridItem
{
public:
	CXTPGI_TTbl(CString strCaption, stTimeTable<T>* pTbl) : CXTPPropertyGridItem(strCaption), m_pTbl( pTbl )	{	}
	virtual ~CXTPGI_TTbl(){};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT(m_pTbl);
		for( int i=0; i<m_pTbl->m_stlvecTimeVal.size(); ++i )
		{
			CXTPPropertyGridItem* pItem = AddChildItem( new CXTPGI_TVal<T, CXTPGI_T>( i, &m_pTbl->m_stlvecTimeVal[i] ) );
			pItem->SetFlags( xtpGridItemHasExpandButton );
		}
	}

private:
	stTimeTable<T>*		m_pTbl;
};

#endif // !defined(AFX_XTPGI_ANIM_H__B6D1D4D7_0DB8_47E2_B9C5_39B024A2FBA6__INCLUDED_)
