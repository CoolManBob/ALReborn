// XTPGraphicBitmapPng.h interface for the XTPGraphicBitmapPng class.
//
// This file is a part of the XTREME TOOLKITPRO MFC class library.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPGRAPHICBITMAPPNG_H__)
#define __XTPGRAPHICBITMAPPNG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class _XTP_EXT_CLASS CXTPGraphicBitmapPng : public CBitmap
{
public:
	CXTPGraphicBitmapPng();

public:
	BOOL LoadFromFile(LPCTSTR lpszFileName);
	BOOL LoadFromResource(HMODULE hModule, HRSRC hRes);

	BOOL LoadFromFile(CFile* pFile);

	BOOL IsAlpha() const;

private:
	HBITMAP ConvertToBitmap(BYTE* pbImage, CSize szImage,  int cImgChannels) const;

protected:
	BOOL m_bAlpha;
};

AFX_INLINE BOOL CXTPGraphicBitmapPng::IsAlpha() const {
	return m_bAlpha;
}

#endif // #if !defined(__XTPGRAPHICBITMAPPNG_H__)
