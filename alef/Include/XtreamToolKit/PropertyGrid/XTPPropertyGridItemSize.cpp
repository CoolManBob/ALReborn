// XTPPropertyGridItemSize.cpp : implementation of the CXTPPropertyGridItemSize class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Common/XTPVC80Helpers.h"

#include "XTPPropertyGridInplaceEdit.h"
#include "XTPPropertyGridInplaceButton.h"
#include "XTPPropertyGridInplaceList.h"
#include "XTPPropertyGridItem.h"
#include "XTPPropertyGridItemSize.h"
#include "XTPPropertyGridItemNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemSize::CXTPPropertyGridItemSizeWidth

class CXTPPropertyGridItemSize::CXTPPropertyGridItemSizeWidth : public CXTPPropertyGridItemNumber
{
public:
	CXTPPropertyGridItemSizeWidth(LPCTSTR strCaption)
		: CXTPPropertyGridItemNumber(strCaption)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTPPropertyGridItemSize*)m_pParent)->SetWidth(strValue);
	}
	virtual BOOL GetReadOnly() const
	{
		return m_pParent->GetReadOnly();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemSize::CXTPPropertyGridItemSizeHeight

class CXTPPropertyGridItemSize::CXTPPropertyGridItemSizeHeight : public CXTPPropertyGridItemNumber
{
public:
	CXTPPropertyGridItemSizeHeight(LPCTSTR strCaption)
		: CXTPPropertyGridItemNumber(strCaption)
	{
	}

	virtual void OnValueChanged(CString strValue)
	{
		((CXTPPropertyGridItemSize*)m_pParent)->SetHeight(strValue);
	}
	virtual BOOL GetReadOnly() const
	{
		return m_pParent->GetReadOnly();
	}
};

/////////////////////////////////////////////////////////////////////////////
// CXTPPropertyGridItemSize
IMPLEMENT_DYNAMIC(CXTPPropertyGridItemSize, CXTPPropertyGridItem)

CXTPPropertyGridItemSize::CXTPPropertyGridItemSize(LPCTSTR strCaption, CSize size, CSize* pBindSize)
	: CXTPPropertyGridItem(strCaption)
{
	m_szValue = size;
	BindToSize(pBindSize);
	m_strDefaultValue = m_strValue = SizeToString(size);
}

CXTPPropertyGridItemSize::CXTPPropertyGridItemSize(UINT nID, CSize size, CSize* pBindSize)
	: CXTPPropertyGridItem(nID)
{
	m_szValue = size;
	BindToSize(pBindSize);
	m_strDefaultValue = m_strValue = SizeToString(size);
}

CXTPPropertyGridItemSize::~CXTPPropertyGridItemSize()
{

}

/////////////////////////////////////////////////////////////////////////////
//

void CXTPPropertyGridItemSize::OnAddChildItem()
{
	m_itemWidth = (CXTPPropertyGridItemSizeWidth*)AddChildItem(new CXTPPropertyGridItemSizeWidth(_T("Width")));
	m_itemHeight = (CXTPPropertyGridItemSizeHeight*)AddChildItem(new CXTPPropertyGridItemSizeHeight(_T("Height")));

	UpdateChilds();

	m_itemWidth->SetDefaultValue(m_itemWidth->GetValue());
	m_itemHeight->SetDefaultValue(m_itemHeight->GetValue());
}

CString CXTPPropertyGridItemSize::SizeToString(CSize size)
{
	CString str;
	str.Format(_T("%i; %i"), size.cx, size.cy);
	return str;
}

CSize CXTPPropertyGridItemSize::StringToSize(LPCTSTR str)
{
	CString strWidth, strHeight;

	AfxExtractSubString(strWidth, str, 0, ';');
	AfxExtractSubString(strHeight, str, 1, ';');

	return CSize(_ttoi(strWidth), _ttoi(strHeight));
}

void CXTPPropertyGridItemSize::SetValue(CString strValue)
{
	SetSize(StringToSize(strValue));
}

void CXTPPropertyGridItemSize::SetSize(CSize size)
{
	m_szValue = size;

	if (m_pBindSize)
	{
		*m_pBindSize = m_szValue;
	}

	CXTPPropertyGridItem::SetValue(SizeToString(m_szValue));
	UpdateChilds();
}

void CXTPPropertyGridItemSize::BindToSize(CSize* pBindSize)
{
	m_pBindSize = pBindSize;
	if (m_pBindSize)
	{
		*m_pBindSize = m_szValue;
	}
}

void CXTPPropertyGridItemSize::OnBeforeInsert()
{
	if (m_pBindSize && *m_pBindSize != m_szValue)
	{
		SetSize(*m_pBindSize);
	}
}

void CXTPPropertyGridItemSize::UpdateChilds()
{
	m_itemWidth->SetNumber(m_szValue.cx);
	m_itemHeight->SetNumber(m_szValue.cy);
}

void CXTPPropertyGridItemSize::SetWidth(LPCTSTR strWidth)
{
	OnValueChanged(SizeToString(CSize(_ttoi(strWidth), m_szValue.cy)));
}

void CXTPPropertyGridItemSize::SetHeight(LPCTSTR strHeight)
{
	OnValueChanged(SizeToString(CSize(m_szValue.cx, _ttoi(strHeight))));
}
