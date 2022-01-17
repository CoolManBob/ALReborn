// XTUtil.cpp: implementation for utility classes.
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

#include "XTUtil.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//===========================================================================
// CXTIconHandle class
//===========================================================================

CXTIconHandle::CXTIconHandle()
	: m_hIcon(NULL)
{
}

CXTIconHandle::CXTIconHandle(HICON hIcon)
	: m_hIcon(hIcon)
{
}

CXTIconHandle::~CXTIconHandle()
{
	if (m_hIcon != NULL)
	{
		::DestroyIcon(m_hIcon);
	}
}

CXTIconHandle& CXTIconHandle::operator=(HICON hIcon)
{
	if (m_hIcon != NULL)
	{
		::DestroyIcon(m_hIcon);
	}

	m_hIcon = hIcon;

	return *this;
}

CSize CXTIconHandle::GetExtent(HICON hIcon)
{
	CSize extent(0);
	if (hIcon)
	{
		ICONINFO iconinfo;
		if (::GetIconInfo(hIcon, &iconinfo))
		{
			BITMAP bmpinfo;
			ZeroMemory(&bmpinfo, sizeof(BITMAP));

			if (::GetObject(iconinfo.hbmMask, sizeof(bmpinfo), &bmpinfo))
			{
				extent.cx = (int)bmpinfo.bmWidth;
				extent.cy = (int)bmpinfo.bmHeight;
				if (!iconinfo.hbmColor)
				{
					// b/w icons have double size for XOR and AND masks
					extent.cy /= 2;
				}
			}
			// cleanup GDI
			if (iconinfo.hbmMask)
			{
				::DeleteObject(iconinfo.hbmMask);
			}
			if (iconinfo.hbmColor)
			{
				::DeleteObject(iconinfo.hbmColor);
			}
		}
	}
	return extent;
}

CSize CXTIconHandle::GetExtent() const
{
	return GetExtent(m_hIcon);
}

HICON CXTIconHandle::ScaleToFit(HICON hIcon, CSize desiredExtent)
{
	if (desiredExtent == CSize(0))
	{
		// invalid arg
		return NULL;
	}

	CSize realExtent = GetExtent(hIcon);
	if (realExtent == CSize(0))
	{
		// icon destroyed or not created yet
		return NULL;
	}


	// ensure icon retains aspect after scaling
	int delta = desiredExtent.cx * realExtent.cy - desiredExtent.cy * realExtent.cx;
	if (delta < 0)
	{
		desiredExtent.cy = MulDiv(realExtent.cy, desiredExtent.cx, realExtent.cx);
	}
	else
	if (delta > 0)
	{
		desiredExtent.cx = MulDiv(realExtent.cx, desiredExtent.cy, realExtent.cy);
	}

	// scale the icon
	CImageList images;
	VERIFY(images.Create(desiredExtent.cx, desiredExtent.cy, ILC_COLOR32 | ILC_MASK, 1, 1));
	images.Add(hIcon);
	return images.ExtractIcon(0);
}

HICON CXTIconHandle::ScaleToFit(CSize desiredExtent) const
{
	return ScaleToFit(m_hIcon, desiredExtent);
}

//===========================================================================
// CXTSplitPath class
//===========================================================================

CXTSplitPath::CXTSplitPath(LPCTSTR lpszPathBuffer/*= NULL*/)
{
	m_szDrive[0] = 0;
	m_szDir[0] = 0;
	m_szFName[0] = 0;
	m_szExt[0] = 0;

	if (lpszPathBuffer != NULL)
	{
		SplitPath(lpszPathBuffer);
	}
}

CXTSplitPath::~CXTSplitPath()
{

}

void CXTSplitPath::SplitPath(LPCTSTR lpszPathBuffer)
{
	SPLITPATH_S(lpszPathBuffer, m_szDrive, m_szDir, m_szFName, m_szExt);
}

CString CXTSplitPath::GetDrive() const
{
	return CString(m_szDrive);
}

CString CXTSplitPath::GetDir() const
{
	return CString(m_szDir);
}

CString CXTSplitPath::GetFName() const
{
	return CString(m_szFName);
}

CString CXTSplitPath::GetExt() const
{
	return CString(m_szExt);
}

CString CXTSplitPath::GetFullPath() const
{
	return GetDrive() + GetDir();
}

CString CXTSplitPath::GetFullName() const
{
	return GetFName() + GetExt();
}
