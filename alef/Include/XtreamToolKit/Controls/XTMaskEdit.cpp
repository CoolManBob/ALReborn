// XTMaskEdit.cpp : implementation of the CXTMaskEdit class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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

#include "stdafx.h"
#include "Common/XTPVC80Helpers.h"  // Visual Studio 2005 helper functions

#include "XTGlobal.h"
#include "XTMaskEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTMaskEdit
/////////////////////////////////////////////////////////////////////////////

CXTMaskEdit::CXTMaskEdit()
{
}


IMPLEMENT_DYNAMIC(CXTMaskEdit, CEdit)

BEGIN_MESSAGE_MAP(CXTMaskEdit, CEdit)
	ON_MASKEDIT_REFLECT()
END_MESSAGE_MAP()

BOOL CXTMaskEdit::SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszDefault /*= NULL*/)
{
	return CXTMaskEditT<CEdit>::SetEditMask(lpszMask, lpszLiteral, lpszDefault);
}

bool CXTMaskEdit::Initialize(CWnd* /*pParentWnd*/)
{
	SetFont(&XTAuxData().font);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CXTDateEdit class
/////////////////////////////////////////////////////////////////////////////

CXTDateEdit::CXTDateEdit()
{
	m_bUseMask = true;
	m_strMask = _T("00/00/0000");
	m_strLiteral = _T("__/__/____");
	m_strDefault = _T("00/00/0000");
}

COleDateTime CXTDateEdit::ReadOleDateTime(LPCTSTR lpszData)
{
	COleDateTime dt;
	dt.ParseDateTime(lpszData);
	return dt;
}

IMPLEMENT_DYNAMIC(CXTDateEdit, CXTMaskEdit)

void CXTDateEdit::FormatOleDateTime(CString &strData, COleDateTime dt)
{
	strData = dt.Format(_T("%d/%m/%Y"));
}

void CXTDateEdit::SetDateTime(COleDateTime& dt)
{
	CString strText;
	FormatOleDateTime(strText, dt);
	m_strWindowText = m_strDefault = strText;
	SetWindowText(strText);
}

void CXTDateEdit::SetDateTime(LPCTSTR strDate)
{
	m_strWindowText = m_strDefault = strDate;
	SetWindowText(strDate);
}

COleDateTime CXTDateEdit::GetDateTime()
{
	CString strText;
	GetWindowText(strText);
	return ReadOleDateTime(strText);
}

CString CXTDateEdit::GetWindowDateTime()
{
	CString strText;
	GetWindowText(strText);
	return strText;
}

BOOL CXTDateEdit::ProcessMask(TCHAR& nChar, int nEndPos)
{
	// check the key against the mask
	if (m_strMask[nEndPos] == _T('0') && _istdigit((TCHAR)nChar))
	{
		if (nEndPos == 0)
		{
			if (nChar > '3')
				return FALSE;
		}
		if (nEndPos == 1)
		{
			if (m_strWindowText.GetAt(0) == _T('3'))
			{
				if (nChar > _T('1'))
					return FALSE;
			}
		}
		if (nEndPos == 3)
		{
			if (nChar > _T('1'))
				return FALSE;
		}
		if (nEndPos == 4)
		{
			if (m_strWindowText.GetAt(3) == _T('1'))
			{
				if (nChar > _T('2'))
					return FALSE;
			}
		}
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CXTTimeEdit class
/////////////////////////////////////////////////////////////////////////////

CXTTimeEdit::CXTTimeEdit()
{
	m_bMilitary = false;
	m_bUseMask = true;
	m_strMask = _T("00:00");
	m_strLiteral = _T("__:__");
	m_strDefault = _T("00:00");
	m_iHours = 0;
	m_iMins = 0;
}

IMPLEMENT_DYNAMIC(CXTTimeEdit, CXTDateEdit)

void CXTTimeEdit::FormatOleDateTime(CString &strData, COleDateTime dt)
{
	if (dt.m_dt == 0)
	{
		strData = _T("00:00");
	}
	else
	{
		strData = dt.Format(_T("%H:%M"));
	}
}

BOOL CXTTimeEdit::ProcessMask(TCHAR& nChar, int nEndPos)
{
	// check the key against the mask
	if (m_strMask[nEndPos] == _T('0') && _istdigit(nChar))
	{
		switch (nEndPos)
		{
		case 0:
			if (m_bMilitary)
			{
				if (nChar > _T('2'))
					return FALSE;
			}
			else
			{
				if (nChar > _T('1'))
					return FALSE;
			}
			return TRUE;

		case 1:
			if (m_bMilitary)
			{
				if (m_strWindowText.GetAt(0) == _T('2'))
				{
					if (nChar > _T('3'))
						return FALSE;
				}
			}
			else
			{
				if (m_strWindowText.GetAt(0) == _T('1'))
				{
					if (nChar > _T('2'))
						return FALSE;
				}
			}
			return TRUE;

		case 3:
			if (nChar > _T('5'))
				return FALSE;
			return TRUE;

		case 4:
			return TRUE;
		}
	}

	return FALSE;
}

void CXTTimeEdit::SetHours(int nHours)
{
	m_iHours = nHours;

	CString strText;
	strText.Format(_T("%02d:%02d"), m_iHours, m_iMins);
	SetWindowText(strText);
}

void CXTTimeEdit::SetMins(int nMins)
{
	m_iMins = nMins;

	CString strText;
	strText.Format(_T("%02d:%02d"), m_iHours, m_iMins);
	SetWindowText(strText);
}

void CXTTimeEdit::SetTime(int nHours, int nMins)
{
	m_iHours = nHours;
	m_iMins = nMins;

	CString strText;
	strText.Format(_T("%02d:%02d"), m_iHours, m_iMins);
	SetWindowText(strText);
}

/////////////////////////////////////////////////////////////////////////////
// DDX_ routines
/////////////////////////////////////////////////////////////////////////////

_XTP_EXT_CLASS void AFXAPI DDX_XTOleDateTime(CDataExchange* pDX, int nIDC, CXTDateEdit& rControl, COleDateTime& rDateTime)
{
	DDX_Control(pDX, nIDC, rControl);

	if (pDX->m_bSaveAndValidate)
	{
		rDateTime = rControl.GetDateTime();
	}
	else
	{
		rControl.SetDateTime(rDateTime);
	}
}
