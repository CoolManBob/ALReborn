// XTPOffice2007Image.h: interface for the CXTPOffice2007Image class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
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
#if !defined(__XTPOFFICE2007IMAGE_H__)
#define __XTPOFFICE2007IMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE

#include "Common/XTPSystemHelpers.h"

// Private image wrappers:

#define XTP_DI_COMPOSITE 1

class CXTPOffice2007Images;
class CXTPOffice2007ImageList;

class _XTP_EXT_CLASS CXTPOffice2007Image : public CXTPCmdTarget
{
public:
	CXTPOffice2007Image();
	virtual ~CXTPOffice2007Image();

public:
	BOOL LoadFile(HMODULE hModule, LPCTSTR lpszBitmapFileName);
	void SetBitmap(HBITMAP hBitmap, BOOL bAlpha = FALSE);

	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, const CRect& rcSizingMargins, COLORREF clrTransparent, DWORD dwFlags);
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, const CRect& rcSizingMargins, COLORREF clrTransparent);
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc, CRect rcSizingMargins);
	void DrawImage(CDC* pDC, const CRect& rcDest, CRect rcSrc);

	HBITMAP GetBitmap() const;

	DWORD GetHeight() const;
	DWORD GetWidth() const;
	CSize GetExtent() const;

	BOOL IsAlphaImage() const;
	void Premultiply();

	void PreserveOrientation() {
		m_bImageOrientaionPreserved = TRUE;
	}

	CRect GetSource(int nState = 0, int nCount = 1) const;


protected:
	BOOL DrawImagePart(CDC* pDCDest, const CRect& rcDest,
		CDC* pDCSrc, const CRect& rcSrc) const;

	BOOL DrawImageTile(CDC* pDCDest, const CRect& rcDest,
		CDC* pDCSrc, const CRect& rcSrc, BOOL bTile) const;

	void InvertBitmap();

protected:
	HBITMAP m_hBitmap;
	BOOL m_bAlpha;
	BOOL m_bMultiply;
	BOOL m_bImageOrientaionPreserved;
	BOOL m_bInvert;

	CXTPCriticalSection* m_pCS;

	friend class CXTPOffice2007Images;
	friend class CXTPOffice2007ImageList;
};

class _XTP_EXT_CLASS CXTPOffice2007Images
{
public:
	CXTPOffice2007Images();
	~CXTPOffice2007Images();
public:
	void RemoveAll();

	CXTPOffice2007Image* LoadFile(LPCTSTR lpszImageFile);

	BOOL SetHandle(HMODULE hResource, LPCTSTR lpszIniFileName = NULL, BOOL bFreeOnRelease = FALSE);
	BOOL SetHandle(LPCTSTR lpszFile, LPCTSTR lpszIniFileName = NULL);
	BOOL InitResourceHandle(LPCTSTR lpszTestImageFile, LPCTSTR lpResType = RT_BITMAP);

	COLORREF GetImageColor(CString strSectionName, CString strKeyName);
	CString GetImageString(CString strSectionName, CString strKeyName);
	CString GetImageValue(CString strSectionName, CString strKeyName);
	BOOL IsValid() const;

	CString m_strHandle;

protected:
	CXTPOffice2007Image* LoadImage(CString strImageFile);
	CString _ImageNameToResourceName(CString strImageFile);

private:
	void RefreshSettings();
	BOOL ReadString(CString& str, LPWSTR& lpTextFile);

	static BOOL CALLBACK EnumResNameProc(HMODULE hModule,
		LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam);
	HRSRC FindResourceIniFile(HMODULE hResource);


protected:
	HMODULE m_hResource;
	LPWSTR m_lpTextFile;
	LPWSTR m_lpTextFileEnd;
	BOOL m_bFreeOnRelease;
	CXTPCriticalSection m_cs;

protected:
	CMapStringToPtr m_mapImages;

	friend class CXTPOffice2007Image;

};

class _XTP_EXT_CLASS CXTPOffice2007ImageList : public CXTPCmdTarget
{
	DECLARE_DYNCREATE(CXTPOffice2007ImageList)
	friend class CXTPOffice2007Image;
public:
	CXTPOffice2007ImageList();
	virtual ~CXTPOffice2007ImageList();

	CXTPOffice2007Image* GetBitmap(UINT nID);

	CXTPOffice2007Image* SetBitmap(HBITMAP hBitmap, UINT nID, BOOL bAlptha, BOOL bCopyBitmap = TRUE);
	BOOL SetBitmap(CXTPOffice2007Image* pImage, UINT nID, BOOL bCallAddRef);

	BOOL LoadBitmap(LPCTSTR lpcszPath, UINT nID);

	BOOL Remove(UINT nID);
	void RemoveAll();

protected:
	CMap<UINT, UINT, CXTPOffice2007Image*, CXTPOffice2007Image*> m_mapID2Image;

	CXTPCriticalSection m_cs;
protected:
};

_XTP_EXT_CLASS CXTPOffice2007Images* AFX_CDECL XTPOffice2007Images();

//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPOFFICE2007IMAGE_H__)
