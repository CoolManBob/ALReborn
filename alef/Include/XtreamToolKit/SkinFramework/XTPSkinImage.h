// XTPSkinImage.h: interface for the CXTPSkinImage class.
//
// This file is a part of the XTREME SKINFRAMEWORK MFC class library.
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
#if !defined(__XTPSKINIMAGE_H__)
#define __XTPSKINIMAGE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSkinManagerResourceFile;


//===========================================================================
// Summary:
//     CXTPSkinImage class represents simple bitmap holder and draw operations for Skin Framework
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinImage
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinImage object.
	//-----------------------------------------------------------------------
	CXTPSkinImage();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinImage object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinImage();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to load bitmap from dll using its name identifier
	// Parameters:
	//     hModule - Module handle
	//     lpszBitmapFileName - Bitmap file name
	//     lpszFileName       - File name on disk to load
	// Returns:
	//      TRUE if successful, otherwise returns FALSE
	//-----------------------------------------------------------------------
	BOOL LoadFile(LPCTSTR lpszFileName);
	BOOL LoadFile(HMODULE hModule, LPCTSTR lpszBitmapFileName); //<COMBINE CXTPSkinImage::LoadFile@LPCTSTR>


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to assign bitmap handle to CXTPSkinImage class
	// Parameters:
	//     hBitmap - Bitmap handle
	//     bAlpha - TRUE if bitmap contains alpha pixels
	//-----------------------------------------------------------------------
	void SetBitmap(HBITMAP hBitmap, BOOL bAlpha = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Draws the image in specified location.
	// Parameters:
	//     pDC    - Destination device context.
	//     rcDest - Specifies the location of the destination rectangle
	//     rcSrc  - Specifies the location of the source rectangle
	//     rcSizingMargins - Specifies sizing margins
	//     nSizingType - Stretch type
	//     bBorderOnly - TRUE to draw only borders
	//     clrTransparent - Transparent color
	//-----------------------------------------------------------------------
	void DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins, int nSizingType, BOOL bBorderOnly);
	void DrawImage(CDC* pDC, const CRect& rcDest, const CRect& rcSrc, const CRect& rcSizingMargins, COLORREF clrTransparent, int nSizingType, BOOL bBorderOnly); // <combine CXTPSkinImage::DrawImage@CDC*@const CRect&@const CRect&@const CRect&@int@BOOL>

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves height of the bitmap
	//-----------------------------------------------------------------------
	DWORD GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves width of the bitmap
	//-----------------------------------------------------------------------
	DWORD GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves extent of the bitmap
	//-----------------------------------------------------------------------
	CSize GetExtent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if bitmap has alpha pixels
	//-----------------------------------------------------------------------
	BOOL IsAlphaImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Calculates position of image
	// Parameters:
	//     nState - Image index to retrieve
	//     nCount - Total image count
	// Returns: Position of specified image
	//-----------------------------------------------------------------------
	CRect GetSource(int nState = 0, int nCount = 1) const;

private:
	BOOL DrawImagePart(CDC* pDCDest, const CRect& rcDest,
		CDC* pDCSrc, const CRect& rcSrc) const;

	BOOL DrawImageTile(CDC* pDCDest, const CRect& rcDest,
		CDC* pDCSrc, const CRect& rcSrc, BOOL bTile) const;

	void FilterImage(COLORREF clrTransparent);
	void InvertBitmap();

public:
	BOOL m_bMirrorImage;    // TRUE to Invert image in RTL mode

protected:
	HBITMAP m_hBitmap;      // Bitmap handle
	BOOL m_bAlpha;          // TRUE if bitmap has alpha pixels
	BOOL m_bFiltered;       // TRUE if image was filtered
	BOOL m_bInvert;
};

//===========================================================================
// Summary:
//     CXTPSkinImages represents cashed collection of CXTPSkinImage classes
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinImages
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinImages object.
	//-----------------------------------------------------------------------
	CXTPSkinImages();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinImages object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinImages();
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all images
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to load image from resource file
	// Parameters:
	//     pResourceFile - Resource file pointer
	//     lpszImageFile - Path to Image
	// Returns:
	//     New CXTPSkinImage object if successful, otherwise returns NULL
	//-----------------------------------------------------------------------
	CXTPSkinImage* LoadFile(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get extent of image in resource file.
	// Parameters:
	//     pResourceFile - Resource file pointer
	//     lpszImageFile - Path to Image
	// Returns:
	//     Extent of image.
	//-----------------------------------------------------------------------
	CSize GetExtent(CXTPSkinManagerResourceFile* pResourceFile, LPCTSTR lpszImageFile);

protected:
	CMapStringToPtr m_mapImages;    // Collection of images
};

#endif // !defined(__XTPSKINIMAGE_H__)
