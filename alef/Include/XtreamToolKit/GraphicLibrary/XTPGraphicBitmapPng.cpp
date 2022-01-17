// XTPGraphicBitmapPng.cpp : implementation of the CXTPGraphicBitmapPng class.
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

#include "stdafx.h"

#include "libpng\png.h"

#include "XTPGraphicBitmapPng.h"


//////////////////////////////////////////////////////////////////////////
// CXTPGraphicBitmapPng

CXTPGraphicBitmapPng::CXTPGraphicBitmapPng()
{
	m_bAlpha = FALSE;
}

static void PNGAPI png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	CFile* pFile = (CFile*)png_ptr->io_ptr;

	png_size_t sizeRead = (png_size_t)pFile->Read(data, (UINT)length);

	if (sizeRead != length)
	{
		AfxThrowFileException(CFileException::endOfFile);
	}
}

static void png_cexcept_error(png_structp /*png_ptr*/, png_const_charp /*msg*/)
{
	AfxThrowFileException(CFileException::badSeek);
}

BOOL PngLoadImage(CFile* pFile, png_byte **ppbImageData,
	png_uint_32 *piWidth, png_uint_32 *piHeight, int *piChannels, png_color *pBkgColor)
{
	png_byte            pbSig[8];
	int                 iBitDepth;
	int                 iColorType;
	double              dGamma;
	png_color_16       *pBackground;
	png_uint_32         ulChannels;
	png_uint_32         ulRowBytes;
	png_byte           *pbImageData = *ppbImageData;
	png_byte          **ppbRowPointers = NULL;
	png_uint_32         i;
	png_structp         png_ptr = NULL;
	png_infop           info_ptr = NULL;

	// open the PNG input file

	if (!pFile)
	{
		*ppbImageData = pbImageData = NULL;
		return FALSE;
	}

	pFile->Read(pbSig, 8);

	if (!png_check_sig(pbSig, 8))
	{
		*ppbImageData = pbImageData = NULL;
		return FALSE;
	}

	// create the two png(-info) structures

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
		(png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
	if (!png_ptr)
	{
		*ppbImageData = pbImageData = NULL;
		return FALSE;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		*ppbImageData = pbImageData = NULL;
		return FALSE;
	}

	TRY
	{

		// initialize the png structure

		png_set_read_fn(png_ptr, (png_voidp)pFile, png_read_data);

		png_set_sig_bytes(png_ptr, 8);

		// read all PNG info up to image data

		png_read_info(png_ptr, info_ptr);

		// get width, height, bit-depth and color-type

		png_get_IHDR(png_ptr, info_ptr, piWidth, piHeight, &iBitDepth,
			&iColorType, NULL, NULL, NULL);

		// expand images of all color-type and bit-depth to 3x8 bit RGB images
		// let the library process things like alpha, transparency, background

		if (iBitDepth == 16)
			png_set_strip_16(png_ptr);
		if (iColorType == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (iBitDepth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (iColorType == PNG_COLOR_TYPE_GRAY ||
			iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);

		// set the background color to draw transparent and alpha images over.
		if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
		{
			png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
			pBkgColor->red   = (byte) pBackground->red;
			pBkgColor->green = (byte) pBackground->green;
			pBkgColor->blue  = (byte) pBackground->blue;
		}
		else
		{
			pBkgColor = NULL;
		}

		// if required set gamma conversion
		if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
			png_set_gamma(png_ptr, (double) 2.2, dGamma);

		// after the transformations have been registered update info_ptr data

		png_read_update_info(png_ptr, info_ptr);

		// get again width, height and the new bit-depth and color-type

		png_get_IHDR(png_ptr, info_ptr, piWidth, piHeight, &iBitDepth,
			&iColorType, NULL, NULL, NULL);


		// row_bytes is the width x number of channels

		ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
		ulChannels = png_get_channels(png_ptr, info_ptr);

		*piChannels = ulChannels;

		// now we can allocate memory to store the image

		if (pbImageData)
		{
			free (pbImageData);
			pbImageData = NULL;
		}

		pbImageData = (png_byte*)malloc(ulRowBytes * (*piHeight) * sizeof(png_byte));

		if (pbImageData == NULL)
		{
			AfxThrowFileException(CFileException::badSeek);
		}
		*ppbImageData = pbImageData;

		// and allocate memory for an array of row-pointers
		ppbRowPointers = (png_bytepp)malloc((*piHeight) * sizeof(png_bytep));

		if (ppbRowPointers == NULL)
		{
			AfxThrowFileException(CFileException::badSeek);
		}

		// set the individual row-pointers to point at the correct offsets

		for (i = 0; i < (*piHeight); i++)
			ppbRowPointers[i] = pbImageData + i * ulRowBytes;

		// now we can go ahead and just read the whole image

		png_read_image(png_ptr, ppbRowPointers);

		// read the additional chunks in the PNG file (not really needed)

		png_read_end(png_ptr, NULL);

		// and we're done

		free (ppbRowPointers);
		ppbRowPointers = NULL;

		// yepp, done
	}
	CATCH (CFileException, e)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		*ppbImageData = pbImageData = NULL;

		if (ppbRowPointers)
			free (ppbRowPointers);

		return FALSE;
	}
	END_CATCH

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return TRUE;
}

HBITMAP CXTPGraphicBitmapPng::ConvertToBitmap(BYTE* pbImage, CSize szImage,  int cImgChannels) const
{
	LONG xImg, yImg;
	BYTE *src, *dst;
	BYTE r, g, b, a;
	BITMAPINFO bmi;
	LPBYTE pBits;
	HBITMAP hbm;
	const int cDIChannels = 4;
	UINT wImgRowBytes;
	UINT wDIRowBytes;

	// Initialize header to 0s.
	ZeroMemory(&bmi, sizeof(bmi));

	// Fill out the fields you care about.
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = szImage.cx;
	bmi.bmiHeader.biHeight = -szImage.cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&pBits, NULL, 0);

	if (!hbm || !pBits)
	{
		return NULL;
	}
	wImgRowBytes = cImgChannels * szImage.cx;
	wDIRowBytes = cDIChannels * szImage.cx;

	// copy image to screen

	for (yImg = 0; yImg < szImage.cy; yImg++)
	{
		src = pbImage + yImg * wImgRowBytes;
		dst = pBits + yImg * wDIRowBytes;

		for (xImg = 0; xImg < szImage.cx; xImg++)
		{
			r = *src++;
			g = *src++;
			b = *src++;
			*dst++ = b;
			*dst++ = g;
			*dst++ = r;
			a = 0;

			if (cImgChannels == 4)
			{
				a = *src++;
			}
			*dst++ = a;
		}
	}

	return hbm;
}

BOOL CXTPGraphicBitmapPng::LoadFromFile(LPCTSTR lpszFileName)
{
	m_bAlpha = FALSE;
	DeleteObject();

	CFile file;
	if (!file.Open(lpszFileName, CFile::modeRead))
	{
		return FALSE;
	}

	return LoadFromFile(&file);
}

BOOL CXTPGraphicBitmapPng::LoadFromResource(HMODULE hModule, HRSRC hRes)
{
	m_bAlpha = FALSE;
	DeleteObject();

	if (!hRes)
		return FALSE;

	DWORD nBufferSize = ::SizeofResource(hModule, hRes);
	HGLOBAL hMem= ::LoadResource(hModule, hRes);
	if (!hMem)
		return FALSE;

	LPBYTE lpBuffer = (LPBYTE)LockResource(hMem);
	if (!lpBuffer || nBufferSize < 10)
		return FALSE;

	CMemFile file(lpBuffer, nBufferSize);

	return LoadFromFile(&file);
}

BOOL CXTPGraphicBitmapPng::LoadFromFile(CFile* pFile)
{
	m_bAlpha = FALSE;
	DeleteObject();

	BYTE* pbImage = NULL;
	png_uint_32 cxImgSize, cyImgSize;
	int cImgChannels;
	png_color bkgColor =  {127, 127, 127};

	if (!pFile)
		return FALSE;

	if (!PngLoadImage(pFile, &pbImage, &cxImgSize, &cyImgSize, &cImgChannels, &bkgColor))
	{
		if (pbImage)
		{
			free(pbImage);
			return FALSE;
		}
	}

	if (!pbImage)
		return FALSE;

	HBITMAP hBitmap = ConvertToBitmap(pbImage, CSize(cxImgSize, cyImgSize), cImgChannels);

	free(pbImage);

	if (!hBitmap)
		return FALSE;

	m_bAlpha = (cImgChannels == 4);
	Attach(hBitmap);

	return TRUE;
}
