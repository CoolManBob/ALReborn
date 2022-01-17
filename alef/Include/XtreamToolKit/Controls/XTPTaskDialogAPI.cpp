// XTPTaskDialogAPI.cpp
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

#include "Common/XTPDrawHelpers.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPTaskDialogAPI.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace XTPTaskDialogAPI
{
	_XTP_EXT_CLASS int AFXAPI XTPLoadStringInst(HINSTANCE hInstance, UINT nID, CString* pString)
	{
		#ifdef _UNICODE
		const int nChar = 1;    // one TCHAR unused is good enough
		#else
		const int nChar = 2;    // two BYTES unused for case of DBC last char
		#endif
		const int nMax  = 256;

		TCHAR szBuffer[nMax];
		int nLen = ::LoadString(hInstance, nID, szBuffer, (nMax-1));
		if ((nMax-1) - nLen > nChar)
		{
			*pString = nLen > 0 ? szBuffer : _T("");
			return nLen;
		}

		int nSize = nMax;
		do
		{
			nSize += nMax;
			nLen = ::LoadString(hInstance, nID, pString->GetBuffer(nSize - 1), nSize);
		}
		while (nSize - nLen <= nChar);
		pString->ReleaseBuffer();

		return nLen;
	}

	_XTP_EXT_CLASS CPoint AFXAPI XTPDlu2Pix(int dluX, int dluY)
	{
		CPoint baseXY(::GetDialogBaseUnits());
		CPoint pixXY(0,0);
		pixXY.x = ::MulDiv(dluX, baseXY.x, 4);
		pixXY.y = ::MulDiv(dluY, baseXY.y, 8);
		return pixXY;
	}

	_XTP_EXT_CLASS CPoint AFXAPI XTPPix2Dlu(int pixX, int pixY)
	{
		CPoint baseXY(::GetDialogBaseUnits());
		CPoint dluXY;
		dluXY.x = ::MulDiv(pixX, 4, baseXY.x);
		dluXY.y = ::MulDiv(pixY, 8, baseXY.y);
		return dluXY;
	}

	_XTP_EXT_CLASS BOOL AFXAPI XTPCalcTextSize(const CString& strText, CSize& sizeText, CFont& font, BOOL* pbWordWrap /*=NULL*/)
	{
		if (strText.IsEmpty() || font.m_hObject == NULL)
		{
			sizeText = CSize(0,0);
			return FALSE;
		}

		CWindowDC dc( NULL );
		CXTPFontDC fontDC( &dc, &font );

		CRect rectText = XTPSize2Rect(sizeText);
		if (dc.DrawText(strText, &rectText,
			DT_CALCRECT | DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK) != 0)
		{
			sizeText = rectText.Size();

			if (pbWordWrap)
				*pbWordWrap = (sizeText.cy > dc.GetTextExtent(strText).cy);

			return TRUE;
		}

		sizeText = CSize(0,0);
		return FALSE;
	}

	_XTP_EXT_CLASS BOOL AFXAPI XTPCalcIdealTextSize(const CString& strText, CSize& sizeText, CFont& font, int dluMax /*=xtpDluMax*/, int dluGrow /*=xtpDluAdd*/)
	{
		if ( strText.IsEmpty() || font.m_hObject == NULL )
			return FALSE;

		CWindowDC dc( NULL );
		CXTPFontDC fontDC( &dc, &font );
		CSize sizeExtent = dc.GetTextExtent( strText );

		int nMaxPix = XTPDlu2Pix( dluMax, 0 ).x;
		if ( sizeExtent.cx > nMaxPix )
		{
			sizeText.cx += MulDiv( sizeExtent.cx, XTPDlu2Pix( dluGrow, 0 ).x, nMaxPix);
			return XTPCalcTextSize( strText, sizeText, font );
		}

		sizeText = sizeExtent;
		return FALSE;
	}

	_XTP_EXT_CLASS CString AFXAPI XTPExtractSubString(CString strFullString, int iSubString)
	{
		if (strFullString.Find(_T('\n')) != -1)
		{
			CString strSubString;
			AfxExtractSubString(strSubString, strFullString, iSubString, _T('\n'));
			return strSubString;
		}
		return ((iSubString == 0)? strFullString: _T(""));
	}

	_XTP_EXT_CLASS CRect AFXAPI XTPSize2Rect(CSize size)
	{
		return CRect(CPoint(0, 0), size);
	}
}
