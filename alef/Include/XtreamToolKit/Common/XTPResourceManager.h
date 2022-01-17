// XTPResourceManager.h: interface for the CXTPResourceManager class.
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
#if !defined(__XTPRESOURCEMANAGER_H__)
#define __XTPRESOURCEMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPXMLHelpers.h"

#pragma warning(disable: 4275) // ID: Q134980
#pragma warning(disable: 4251) // ID: Q134980

//===========================================================================
// Summary:
//     Standalone class used to managed toolkit resources.
//===========================================================================
class _XTP_EXT_CLASS CXTPResourceManager : public CNoTrackObject
{
public:
	//=============================================================================
	// Summary:
	//     CManageState is used internally by the toolkit to switch module
	//     states.  Specifically, the class enables the toolkit to read its
	//     own .dll resources, and not that of the host application (or .dll).
	//     Future versions of the class may handle other such details
	//=============================================================================
	class _XTP_EXT_CLASS CManageState
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CManageState object
		//-------------------------------------------------------------------------
		CManageState();

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CManageState object, handles clean up and deallocation
		//-------------------------------------------------------------------------
		~CManageState();

	public:
		//-------------------------------------------------------------------------
		// Summary:
		//     Call this member function to UNDO managed state
		//-------------------------------------------------------------------------
		void Undo();

		//-------------------------------------------------------------------------
		// Summary:
		//     Call this member function to REDO managed state
		//-------------------------------------------------------------------------
		void Redo();

	protected:
		bool            m_bSet;             // true to set
		HMODULE         m_hModOldResource;  // Handle to previous resource.
	};
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPResourceManager object.
	//-------------------------------------------------------------------------
	CXTPResourceManager();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPResourceManagerXML object, handles cleanup and de-
	//     allocation.
	//-------------------------------------------------------------------------
	~CXTPResourceManager();

public:
	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member to set resource file to be used (dll or xml)
	// Parameters:
	//     strResourceFile - Resource file to be used.
	// Remarks:
	//     Call Close method in ExitInstance of your application to free resources.
	// See Also: Close, SetResourceHandle
	//-------------------------------------------------------------------------
	void SetResourceFile(const CString& strResourceFile);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member to set the resource handle for the calling module.
	// Parameters:
	//     hModRes - Handle to modules resource.
	//-------------------------------------------------------------------------
	void SetResourceHandle(HMODULE hModRes);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the library resource handle.
	// Returns:
	//     Returns the resource handle defined for the library.
	//-------------------------------------------------------------------------
	HMODULE GetResourceHandle() const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows string resource, identified by nID, into an existing CString object
	// Parameters:
	//     pString     - Output string pointer
	//     nIDResource - A Windows string resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadString(CString* pString, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows string resource, identified by nID.
	// Parameters:
	//     nIDResource - A Windows string resource ID.
	//     pbError     - Pointer to a BOOL variable to report was resource load
	//                   successful or not. (Optional).
	// Returns:
	//     Loaded string or empty string if such string ID does not find in resource.
	//-----------------------------------------------------------------------
	CString LoadString(UINT nIDResource, BOOL* pbError = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows menu resource, identified by nID, into an existing CMenu object
	// Parameters:
	//     lpMenu      - Output menu pointer
	//     nIDResource - A Windows menu resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadMenu(CMenu* lpMenu, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to load the toolbar specified by CToolBar and nIDResource.
	// Parameters:
	//     pToolBar    - Result CToolBar pointer
	//     nIDResource - Resource ID of the toolbar to be loaded.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadToolBar(CToolBar* pToolBar, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows bitmap resource, identified by nID, into an existing CBitmap object
	// Parameters:
	//     pBitmap     - Result CBitmap pointer
	//     nIDResource -  A Windows bitmap resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadBitmap(CBitmap* pBitmap, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows cursor resource, identified by nID, into an existing cursor object
	// Parameters:
	//     nIDResource -  A Windows cursor resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual HCURSOR LoadCursor(UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows dialog resource, identified by pszResource
	// Parameters:
	//     nIDResource - A Windows dialog resource ID.
	// Returns:
	//     LPCDLGTEMPLATE pointer if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual LPCDLGTEMPLATE LoadDialogTemplate(UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows dialog template identified by pszResource into the memory.
	// Parameters:
	//     pszResource - A Windows dialog resource ID.
	// Returns:
	//     HGLOBAL handle if resource load was successful; otherwise 0.
	// Remarks:
	//     Can be useful to load custom templates for common dialogs.
	//     To access dialog template data use LockResource() API function.
	// See Also:
	//     LoadDialogTemplate()
	//-----------------------------------------------------------------------
	virtual HGLOBAL LoadDialogTemplate2(LPCTSTR pszTemplate);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows HTML resource, identified by nIDResource,
	//     into an existing CString object.
	// Parameters:
	//     pString     - Output string pointer
	//     nIDResource -  A Windows bitmap resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadHTML(CString* pText, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//      The LoadIcon function loads an icon by locating its resource handle.
	// Parameters:
	//      lpszID   - [in] Specifies the name of the icon to load.
	//      iconSize - [in] Specifies the width and height in pixels of the icon.
	// Returns:
	//      If the function succeeds, the return value is the handle of the newly
	//      loaded icon. If the function fails, the return value is NULL. To get
	//      extended error information, call GetLastError.
	//-----------------------------------------------------------------------
	virtual HICON LoadIcon(LPCTSTR lpszID, CSize iconSize);

	//-----------------------------------------------------------------------
	// Summary:
	//      The LoadIcon function loads an icon by locating its resource handle.
	// Parameters:
	//      nID      - [in] Specifies the resource id of the icon to load.
	//      iconSize - [in] Specifies the width and height in pixels of the icon.
	// Returns:
	//      If the function succeeds, the return value is the handle of the newly
	//      loaded icon. If the function fails, the return value is NULL. To get
	//      extended error information, call GetLastError.
	//-----------------------------------------------------------------------
	virtual HICON LoadIcon(int nID, CSize iconSize);

	//-----------------------------------------------------------------------
	// Summary:
	//      The CreateIconFromResource function creates a new icon from the specified
	//      modules icon resource.
	// Parameters:
	//      hModule  - [in] Handle to the module whose executable file contains the
	//                      icon resource.
	//      lpszID   - [in] Specifies the name of the icon to load.
	//      iconSize - [in] Specifies the width and height in pixels of the icon.
	// Returns:
	//      If the function succeeds, the return value is a handle to the icon or
	//      cursor. If the function fails, the return value is NULL. To get extended
	//      error information, call GetLastError.
	//-----------------------------------------------------------------------
	virtual HICON CreateIconFromResource(HMODULE hModule, LPCTSTR lpszID, CSize iconSize);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Displays a message box on the screen
	// Parameters:
	//     nIDPrompt - A unique ID used to reference a string in the string table.
	//     nType     - The style of the message box.
	// Returns:
	//     See AfxMessageBox return values list
	//-----------------------------------------------------------------------
	int ShowMessageBox(UINT nIDPrompt, UINT nType = MB_OK);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method free used resource. Call it in ExitInstance of your application if you use
	//     external resource file.
	// See Also: SetResourceFile
	//-----------------------------------------------------------------------
	virtual void Close();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set application defined Resource manager
	// Parameters:
	//     pResourceManager - Application defined Resource manager
	// Returns:
	//     TRUE if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL SetResourceManager(CXTPResourceManager* pResourceManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get current resource language ID.
	// Remarks:
	//     Value for default resource is (LANG_ENGLISH, SUBLANG_ENGLISH_US)
	// Returns:
	//     Current resource language ID.
	//-----------------------------------------------------------------------
	virtual WORD GetResourcesLangID();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this static method to determine what language has resources
	// Parameters:
	//     hResModule - Resource instance
	//-----------------------------------------------------------------------
	static WORD AFX_CDECL GetResourceLanguage(HMODULE hResModule);


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method check resource identifier of loaded resources
	//-----------------------------------------------------------------------
	virtual void _UpdateResourcesLangID();


public:
	static CProcessLocal<CXTPResourceManager> m_managerInstance; // Manager instance.

protected:
	BOOL m_bValid;                      // TRUE if Resource manager is valid.
	HMODULE m_hResourceFile;            // Resource file handle.
	WORD    m_wResourcesLangID;         // Resource language

	static HMODULE  m_hModXTResource;   // Handle to toolkit resource.


//{{AFX_CODEJOCK_PRIVATE

	static BOOL CALLBACK EnumResTypeProc(HMODULE hModule, LPTSTR lpszType, LONG_PTR lParam);
	static BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam); // <combine CXTPResourceManager::EnumResTypeProc@HMODULE@LPTSTR@LONG_PTR>
	static BOOL CALLBACK EnumResLangProc(HMODULE hModule, LPCTSTR lpszType, LPCTSTR lpszName,
										 WORD wIDLanguage, LONG_PTR lParam); // <combine CXTPResourceManager::EnumResTypeProc@HMODULE@LPTSTR@LONG_PTR>
//}}AFX_CODEJOCK_PRIVATE
};


#ifndef  _XTP_EXCLUDE_XML

class CXTPPropExchangeXMLNode;
class CXTPPropExchange;

//===========================================================================
// Summary:
//     Class used to managed toolkit resources from XML file
//===========================================================================
class _XTP_EXT_CLASS CXTPResourceManagerXML : public CXTPResourceManager
{
private:
	struct MENUITEMTEMPLATEINFO;
	struct MENUTEMPLATEINFO;
	struct DLGTEMPLATEINFO;
	struct DLGITEMTEMPLATEINFO;
	struct DLGTEMPLATEEX;
	struct DLGITEMTEMPLATEEX;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPResourceManagerXML object.
	// Parameters:
	//     lpszFileName - XML file path
	//     pResourceRoot - Base XML Node
	//-----------------------------------------------------------------------
	CXTPResourceManagerXML(LPCTSTR lpszFileName);
	CXTPResourceManagerXML(CXTPPropExchangeXMLNode* pResourceRoot); // <combine CXTPResourceManagerXML::CXTPResourceManagerXML@LPCTSTR>

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPResourceManagerXML object, handles cleanup and de-
	//     allocation.
	//-------------------------------------------------------------------------
	~CXTPResourceManagerXML();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows string resource, identified by nID, into an existing CString object
	// Parameters:
	//     pString     - Output string pointer
	//     nIDResource - A Windows string resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadString(CString* pString, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows menu resource, identified by nID, into an existing CMenu object
	// Parameters:
	//     lpMenu      - Output menu pointer
	//     nIDResource - A Windows menu resource ID.
	// Returns:
	//     Nonzero if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL LoadMenu(CMenu* lpMenu, UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads a Windows dialog resource, identified by pszResource
	// Parameters:
	//     nIDResource - A Windows dialog resource ID.
	// Returns:
	//     LPCDLGTEMPLATE pointer if resource load was successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual LPCDLGTEMPLATE LoadDialogTemplate(UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method free used resource. Call it in ExitInstance of your application if you use
	//     external resource file.
	// See Also: SetResourceFile
	//-----------------------------------------------------------------------
	virtual void Close();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method check resource identifier of loaded resources
	//-----------------------------------------------------------------------
	virtual void _UpdateResourcesLangID();

private:
	BOOL CreateMenu(CMenu& menu, CXTPPropExchange* pPX);
	void AddMenuItems(CXTPPropExchange* pPX, MENUTEMPLATEINFO* pItems);
	LPDLGTEMPLATE CreateDialogTemplate(CXTPPropExchange* pPX);
	LPDLGTEMPLATE CreateDialogTemplate(DLGTEMPLATEINFO& dlgTemplate);

private:
	CXTPPropExchangeXMLNode* m_pResourceRoot;
	CMap<UINT, UINT, DLGTEMPLATE*, DLGTEMPLATE*> m_mapDialogs;
};

#endif

//-----------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPResourceManager members.
//     Since this class is designed as a single instance object you can
//     only access version info through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPResourceManager.
// Example:
// <code>
// XTPResourceManager()->LoadString(&m_strNoFieldsAvailable,
//     XTP_IDS_REPORT_NOFIELDSAVAILABLE);
// </code>
//-----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPResourceManager* AFX_CDECL XTPResourceManager();


AFX_INLINE WORD CXTPResourceManager::GetResourcesLangID() {
	return m_wResourcesLangID;
}

AFX_INLINE CString CXTPResourceManager::LoadString(UINT nIDResource, BOOL* pbError) {
	CString str;
	BOOL bRes = LoadString(&str, nIDResource);
	ASSERT(bRes);
	if (pbError) *pbError = !bRes;
	return str;
}

#endif // !defined(__XTPRESOURCEMANAGER_H__)
