// XTPSkinManagerResource.h: interface for the CXTPSkinManagerResourceFile class.
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
#if !defined(__XTPSKINMANAGERRESOURCE_H__)
#define __XTPSKINMANAGERRESOURCE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSkinImage;
class CXTPSkinManagerSchema;
class CXTPSkinManager;

//===========================================================================
// Summary:
//     CXTPSkinManagerResourceFile is a standalone class that represents a
//     manager of visual style resources
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerResourceFile
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerResourceFile object.
	//-----------------------------------------------------------------------
	CXTPSkinManagerResourceFile();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerResourceFile object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerResourceFile();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to open visual style file
	// Parameters:
	//     lpszResourcePath - Path to specified visual style
	//     lpszIniFileName  - Name of the ini file to load
	//-----------------------------------------------------------------------
	virtual BOOL Open(LPCTSTR lpszResourcePath, LPCTSTR lpszIniFileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to read ini file
	// Parameters:
	//     str - reference to the string used to load next line from resource file.
	//-----------------------------------------------------------------------
	virtual BOOL ReadString(CString& str);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to close visual style file
	//-----------------------------------------------------------------------
	virtual void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to load image from resource file
	// Parameters:
	//     strImageFile - Path to specified image
	// Returns:
	//     New CXTPSkinImage contained loaded bitmap
	//-----------------------------------------------------------------------
	virtual CXTPSkinImage* LoadImage(CString strImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to create CXTPSkinManagerSchema class associated with loaded resources
	// Returns: New CXTPSkinManagerSchema class that will be used to read resource
	// See Also: CXTPSkinManagerSchema
	//-----------------------------------------------------------------------
	virtual CXTPSkinManagerSchema* CreateSchema();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves resource path
	//-----------------------------------------------------------------------
	CString GetResourcePath() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves resource path
	//-----------------------------------------------------------------------
	CString GetIniFileName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves handle to visual style dll file handle.
	//-----------------------------------------------------------------------
	HMODULE GetModuleHandle() const;

private:
	CString GetDefaultIniFileName();

protected:
	LPWSTR m_lpTextFile;            // Pointer to next string
	LPWSTR m_lpTextFileEnd;         // Pointer to end of the file
	HMODULE m_hModule;              // Handle of the visual style

	CString m_strResourcePath;      // Resource path
	CString m_strIniFileName;       // Ini file name.
	CXTPSkinManager* m_pManager;    // Parent manager class.

	CStdioFile* m_pIniFile;         // POinter to ini file on disk

	friend class CXTPSkinManager;
};

AFX_INLINE CString CXTPSkinManagerResourceFile::GetIniFileName() const {
	return m_strIniFileName;
}
AFX_INLINE CString CXTPSkinManagerResourceFile::GetResourcePath() const {
	return m_strResourcePath;
}
AFX_INLINE HMODULE CXTPSkinManagerResourceFile::GetModuleHandle() const {
	return m_hModule;
}


#endif // !defined(__XTPSKINMANAGERRESOURCE_H__)
