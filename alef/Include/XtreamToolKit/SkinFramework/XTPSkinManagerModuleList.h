// XTPSkinManagerModuleList.h: interface for the CXTPSkinManagerModuleList class.
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
#if !defined(__XTPSKINMANAGERMODULELIST_H__)
#define __XTPSKINMANAGERMODULELIST_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//{{AFX_CODEJOCK_PRIVATE

class _XTP_EXT_CLASS CXTPSkinManagerModuleList
{
	class CModuleEnumerator;
	class CPsapiModuleEnumerator;
	class CToolHelpModuleEnumerator;

public:
	class CSharedData
	{
	public:
		CSharedData();
		~CSharedData();
	public:
		HMODULE m_hPsapiDll;
		BOOL m_bExists;
	};

public:
	CXTPSkinManagerModuleList(DWORD dwProcessId);
	virtual ~CXTPSkinManagerModuleList();

public:
	static BOOL AFX_CDECL IsEnumeratorExists();
	static CSharedData& AFX_CDECL GetSharedData();

public:
	HMODULE GetFirstModule();
	HMODULE GetNextModule();

protected:
	CModuleEnumerator* m_pEnumerator;
};

//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPSKINMANAGERMODULELIST_H__)
