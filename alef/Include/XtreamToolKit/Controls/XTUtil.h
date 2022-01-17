// XTUtil.h: interface for utility classes.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTUTIL_H__)
#define __XTUTIL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//===========================================================================
// Summary:
//     CXTIconHandle is a stand alone helper class.  It is used to
//     automatically destroy dynamically created hIcon handles.
//===========================================================================
class _XTP_EXT_CLASS CXTIconHandle
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTIconHandle object.
	// Parameters:
	//     hIcon - Handle to a dynamically created icon.
	//-----------------------------------------------------------------------
	CXTIconHandle();
	CXTIconHandle(HICON hIcon); // <combine CXTIconHandle::CXTIconHandle>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTIconHandle object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTIconHandle();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator is used to retrieve a handle to the icon
	//     associated with the CXTIconHandle object.
	// Returns:
	//     An HICON handle to the icon.
	//-----------------------------------------------------------------------
	operator HICON() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This operator is used to initialize the icon associated with
	//     the CXTIconHandle object.
	// Parameters:
	//     hIcon - Handle to a dynamically created icon.
	// Returns:
	//     A reference to a CXTIconHandle object.
	//-----------------------------------------------------------------------
	CXTIconHandle& operator=(HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function gets the extent of an icon.
	// Parameters:
	//     hIcon - Icon handle whose dimensions are to be retrieved.
	// Returns:
	//     A CSize object.
	//-----------------------------------------------------------------------
	CSize GetExtent() const;
	static CSize GetExtent(HICON hIcon); // <combine CXTIconHandle::GetExtent@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function scales an icon to fit into a rectangle.
	//     The width and height ration is retained as much as possible.
	//     The caller assumes ownership of the returned icon handle.
	// Parameters:
	//     hIcon         - Icon to be fitted.
	//     desiredExtent - Desired icon extent.
	// Returns:
	//     An icon handle.
	//-----------------------------------------------------------------------
	HICON ScaleToFit(CSize desiredExtent) const;
	static HICON ScaleToFit(HICON hIcon, CSize desiredExtent); // <combine CXTIconHandle::ScaleToFit@CSize@const>

protected:
	HICON m_hIcon;  // Handle to a dynamically created icon.
};


AFX_INLINE CXTIconHandle::operator HICON() const {
	return m_hIcon;
}


#define CXTNoFlickerWnd CXTPNoFlickerWnd

//===========================================================================
// Summary:
//     CXTSplitPath conveniently wraps the _splitpath API for easier
//     access to path components such as file name and extension.
//===========================================================================
class _XTP_EXT_CLASS CXTSplitPath
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTSplitPath object and breaks a path into its
	//     four components.
	// Parameters:
	//     lpszPathBuffer - A NULL terminated string that represents the
	//                      path you
	//-----------------------------------------------------------------------
	CXTSplitPath(LPCTSTR lpszPathBuffer = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTSplitPath object, handles cleanup and de-
	//     allocation.
	//-----------------------------------------------------------------------
	virtual ~CXTSplitPath();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call SplitPath to break a path into its four components.
	// Parameters:
	//     lpszPathBuffer - A NULL terminated string that represents the
	//                      path you
	//-----------------------------------------------------------------------
	void SplitPath(LPCTSTR lpszPathBuffer);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetDrive() to return the drive letter, followed by a
	//     colon (:) for the path split.
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetDrive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetDir() to return the directory path, including
	//     trailing slash. Forward slashes (/), backslashes (\), or
	//     both may be used.
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetDir() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetFName() to return the base filename without extension
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetFName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetExt() to return the filename extension, including
	//     leading period (.)
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetExt() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetFullPath() to return the full path that was split.
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetFullPath() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call GetFullName() to return the file name plus extension
	//     only for the path that was split.
	// Returns:
	//     A NULL terminated string.
	//-----------------------------------------------------------------------
	CString GetFullName() const;

protected:
	TCHAR m_szDrive[_MAX_DRIVE]; // Optional drive letter, followed by a colon (:)
	TCHAR m_szDir[_MAX_DIR];     // Optional directory path, including trailing slash. Forward slashes (/), backslashes (\), or both may be used.
	TCHAR m_szFName[_MAX_FNAME]; // Base filename (no extension)
	TCHAR m_szExt[_MAX_EXT];     // Optional filename extension, including leading period (.)
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(__XTUTIL_H__)
