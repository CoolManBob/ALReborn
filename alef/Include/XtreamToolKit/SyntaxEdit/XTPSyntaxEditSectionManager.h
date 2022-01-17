// XTPSyntaxEditSectionManager.h: interface for the CXTPSyntaxEditSectionManager class.
//
// This file is a part of the XTREME TOOLKIT PRO MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME SYNTAX EDIT LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSYNTAXEDITSECTIONMANAGER_H__)
#define __XTPSYNTAXEDITSECTIONMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE
//=======================================================================
// struct: XTP_EDIT_SCHEMAFILEINFO
//=======================================================================
struct _XTP_EXT_CLASS XTP_EDIT_SCHEMAFILEINFO
{
	XTP_EDIT_SCHEMAFILEINFO();

	XTP_EDIT_SCHEMAFILEINFO(const XTP_EDIT_SCHEMAFILEINFO& info);

	XTP_EDIT_SCHEMAFILEINFO(const CString& srcName, const CString& srcValue);

	const XTP_EDIT_SCHEMAFILEINFO& operator=(const XTP_EDIT_SCHEMAFILEINFO& info);

	BOOL operator==(const XTP_EDIT_SCHEMAFILEINFO& info) const;

public:
	CString csName;
	CString csValue;
	CString csDesc;
	UINT    uValue;
};

//=======================================================================
// class: CXTPSyntaxEditSchemaFileInfoList
//=======================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditSchemaFileInfoList : public CList<XTP_EDIT_SCHEMAFILEINFO,XTP_EDIT_SCHEMAFILEINFO&>
{
public:

	POSITION LookupName (const CString& csName, XTP_EDIT_SCHEMAFILEINFO& info);

	POSITION LookupValue(const CString& csValue, XTP_EDIT_SCHEMAFILEINFO& info);

	POSITION LookupValue(const UINT& uValue, XTP_EDIT_SCHEMAFILEINFO& info);

	POSITION LookupDesc (const CString& csDesc, XTP_EDIT_SCHEMAFILEINFO& info);
};

//=======================================================================
// class: CXTPSyntaxEditSectionManager
//=======================================================================
class _XTP_EXT_CLASS CXTPSyntaxEditSectionManager
{
public:
	CXTPSyntaxEditSectionManager();

	virtual ~CXTPSyntaxEditSectionManager();

public:
	int GetSectionNames(CStringArray& arSections, LPCTSTR lpszFilePath);

	int GetSectionKeyList(CXTPSyntaxEditSchemaFileInfoList& infoList, LPCTSTR lpszFilePath, LPCTSTR lpszSectionName);

	BOOL ParseSectionKey(XTP_EDIT_SCHEMAFILEINFO& info, const CString& csBuffer);

	BOOL SplitString(CString& csLeft, CString& csRight, const CString& csBuffer, TCHAR chSep);
};

//}}AFX_CODEJOCK_PRIVATE
//////////////////////////////////////////////////////////////////////

#endif // !defined(__XTPSYNTAXEDITSECTIONMANAGER_H__)
