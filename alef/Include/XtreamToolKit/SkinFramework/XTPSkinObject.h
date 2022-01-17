// XTPSkinObject.h: interface for the CXTPSkinObject class.
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
#if !defined(_XTPSKINOBJECT_H__)
#define _XTPSKINOBJECT_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSkinManager;
class CXTPSkinManagerClass;
class CXTPSkinManagerMetrics;

#include "Common/XTPSystemHelpers.h"


//{{AFX_CODEJOCK_PRIVATE
// Internal enumerator
enum XTPSkinDefaultProc
{
	xtpSkinDefaultHookMessage,
	xtpSkinDefaultCallWindowProc,
	xtpSkinDefaultDefWindowProc,
	xtpSkinDefaultDefMDIChildProc,
	xtpSkinDefaultDefFrameProc,
	xtpSkinDefaultDefDlgProc
};

class CXTPSkinObjectPaintDC : public CDC
{

// Constructors
public:
	CXTPSkinObjectPaintDC(CWnd* pWnd);
	virtual ~CXTPSkinObjectPaintDC();

// Attributes
protected:
	HWND m_hWnd;
public:
	PAINTSTRUCT m_ps;
};
//}}AFX_CODEJOCK_PRIVATE


//===========================================================================
// Summary:
//     CXTPSkinObject is a CWnd derived class. It represents base class for all
//     skinable windows.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObject : public CWnd
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObject object.
	//-----------------------------------------------------------------------
	CXTPSkinObject();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObject object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinObject();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent CXTPSkinManager object
	//-----------------------------------------------------------------------
	CXTPSkinManager* GetSkinManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves own classname.
	// See Also:
	//     GetSkinClass
	//-----------------------------------------------------------------------
	CString GetClassName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves CXTPSkinManagerClass object associated with class name
	// See Also:
	//     GetClassName
	//-----------------------------------------------------------------------
	CXTPSkinManagerClass* GetSkinClass() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get pointer to skin metrics
	// Returns:
	//     Pointer to CXTPSkinManagerMetrics class contained metrics of the skin
	//-----------------------------------------------------------------------
	CXTPSkinManagerMetrics* GetMetrics() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves system color used in skinmanager
	// Parameters:
	//      nIndex - Index of color to retrieve
	// Returns: Color of selected skin
	// See Also: GetSysColor
	//-----------------------------------------------------------------------
	COLORREF GetColor(int nIndex) const;

	BOOL IsSkinEnabled() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when skin was changed
	// Parameters:
	//     bPrevState - TRUE if previously skin was installed
	//     bNewState - TRUE if skin is installed
	//-----------------------------------------------------------------------
	virtual void OnSkinChanged(BOOL bPrevState, BOOL bNewState);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when visual metrics was changed
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when hook is attached
	//-----------------------------------------------------------------------
	virtual void OnHookAttached(LPCREATESTRUCT lpcs, BOOL bAuto);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when hook is detached
	//-----------------------------------------------------------------------
	virtual void OnHookDetached(BOOL bAuto);

//{{AFX_CODEJOCK_PRIVATE
public:
	UINT GetHeadMessage();

protected:
	virtual BOOL OnHookMessage(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	virtual BOOL OnHookDefWindowProc(UINT nMessage, WPARAM& wParam, LPARAM& lParam, LRESULT& lResult);
	void AttachHook(HWND hWnd, LPCREATESTRUCT lpcs, BOOL bAuto);
	void UnattachHook(BOOL bAuto);
	virtual void OnFinalRelease();
	virtual WNDPROC* GetSuperWndProcAddr();
	virtual BOOL IsDefWindowProcAvail(int nMessage) const;
	virtual LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreHookMessage(UINT nMessage);
#ifdef _DEBUG
	virtual void AssertValid() const {
	}
#endif

	void OnBeginHook(UINT nMessage, XTPSkinDefaultProc defProc, PROC defProcAddr, LPVOID defProcPrevWndFunc);
	void OnEndHook();
	LRESULT DoDefWindowProc (UINT nMsg, WPARAM wParam, LPARAM lParam);
//}}AFX_CODEJOCK_PRIVATE

protected:
	WNDPROC m_pOldWndProc;          // Old window window handler
	CXTPSkinManager* m_pManager;    // Parent CXTPSkinManager object
	CString m_strClassName;         // Own class name
	BOOL m_bActiveX;                // TRUE if ActiveX controls
	BOOL m_bWindowsForms;            // TRUE if .NET controls
	WNDPROC m_wndDefProcAddr;       // Address of previous window procedure
	BOOL m_bCustomDraw;
	static UINT m_nMsgQuerySkinState;
	static UINT m_nMsgUpdateSkinState;
	BOOL m_bUnicode;
	CXTPCriticalSection m_csDescriptors;


private:
	struct DEFWINDOW_DESCRIPTIOR
	{
		XTPSkinDefaultProc defProc;
		PROC defProcAddr;
		LPVOID lpPrev;
		UINT nMessage;
	};
	CList<DEFWINDOW_DESCRIPTIOR, DEFWINDOW_DESCRIPTIOR&> m_arrDescriptors;

protected:
	friend class CXTPSkinManager;
	friend class CXTPSkinManagerApiHook;
	friend class CXTPSkinObjectClassInfoActiveX;
};


//{{AFX_CODEJOCK_PRIVATE
class CXTPSkinObjectClassMap;

class _XTP_EXT_CLASS CXTPSkinObjectClassInfo : public CCmdTarget
{
public:
	CXTPSkinObjectClassInfo(CRuntimeClass* pClass);

public:
	virtual CXTPSkinObject* CreateObject(LPCTSTR lpszClassName, LPCREATESTRUCT lpcs);

public:
	void AddClass(LPCTSTR lpszClassName);
	void Remove();

protected:
	CRuntimeClass* m_pClass;
	CXTPSkinObjectClassMap* m_pMap;

	friend class CXTPSkinObjectClassMap;
};

class _XTP_EXT_CLASS CXTPSkinObjectClassInfoActiveX : public CXTPSkinObjectClassInfo
{
public:
	CXTPSkinObjectClassInfoActiveX(CRuntimeClass* pClass, BOOL bNoDefault)
		: CXTPSkinObjectClassInfo(pClass)
	{
		m_bNoDefault = bNoDefault;
	}

	virtual CXTPSkinObject* CreateObject(LPCTSTR lpszClassName, LPCREATESTRUCT lpcs)
	{
		CXTPSkinObject* pObject = CXTPSkinObjectClassInfo::CreateObject(lpszClassName, lpcs);
		if (pObject)
		{
			pObject->m_bActiveX = TRUE;
			if (m_bNoDefault) pObject->m_wndDefProcAddr = 0;
		}
		return pObject;
	}
	BOOL m_bNoDefault;
};


//}}AFX_CODEJOCK_PRIVATE

//===========================================================================
// Summary:
//     CXTPSkinObjectClassMap class is standalone class that help map windows controls to skin framework classes.
// Remarks:
//     CXTPSkinManager class use this class to find what skin framework class need to apply to new window to skin it right
//     For example, if new window created with class name "BUTTON", CXTPSkinObjectClassMap::Lookup will retrieve runtime class
//     of CXTPSkinObjectButton to apply it to new window and skin it.
// See Also: CXTPSkinManager, CXTPSkinObject, CXTPSkinManager::GetClassMap
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinObjectClassMap
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinObjectClassMap object.
	//-----------------------------------------------------------------------
	CXTPSkinObjectClassMap();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinObjectClassMap object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinObjectClassMap();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method called by framework to add all standard window classes
	// Remarks:
	//     CXTPSkinManager call this method to add standard window classes like "BUTTON", "LISTBOX", "SCROLLBAR"
	//-----------------------------------------------------------------------
	void AddStandardClasses();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to map window class name to skin framework class
	// Parameters:
	//     lpszClassName - Window class name
	//     pInfo - descriptor of skin framework class
	// Example:
	//     <code>pClassMap->AddClass(_T("ListBox"), new CXTPSkinObjectClassInfo(RUNTIME_CLASS(CXTPSkinObjectListBox)));</code>
	// See Also:
	//     AddSynonymClass,  AddStandardClasses
	//-----------------------------------------------------------------------
	CXTPSkinObjectClassInfo* AddClass(LPCTSTR lpszClassName, CXTPSkinObjectClassInfo* pInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to map one window class name to another
	// Parameters:
	//     lpszClassNameSynonym - New window class name
	//     lpszClassName - Standard window class name
	// Example:
	//     <code>pClassMap->AddSynonymClass(_T("SysDateTimePick32"), _T("Edit"));</code>
	// See Also:
	//     AddClass
	//-----------------------------------------------------------------------
	void AddSynonymClass(LPCTSTR lpszClassNameSynonym, LPCTSTR lpszClassName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove window class
	// Parameters:
	//     lpszClassName - Window class name to remove
	// See Also:
	//     AddClass
	//-----------------------------------------------------------------------
	void RemoveClass(LPCTSTR lpszClassName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove skin framework class
	// Parameters:
	//     lpszClassName - skin framework class name to remove
	// See Also:
	//     AddClass
	//-----------------------------------------------------------------------
	void RemoveClass(CXTPSkinObjectClassInfo* pInfo);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all classed
	// See Also:
	//     AddClass
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method map window class to skinframework class
	// Parameters:
	//     lpszClassName - Window Class name to map
	// See Also:
	//     AddClass
	//-----------------------------------------------------------------------
	CXTPSkinObjectClassInfo* Lookup(LPCTSTR lpszClassName);

protected:
	CMapStringToPtr m_mapInfo;  // General map
};


AFX_INLINE CXTPSkinManager* CXTPSkinObject::GetSkinManager() const {
	return m_pManager;
}
AFX_INLINE CString CXTPSkinObject::GetClassName() const {
	return m_strClassName;
}

#endif // !defined(_XTPSKINOBJECT_H__)
