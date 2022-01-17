// XTPShortcutManager.h : interface for the CXTPShortcutManager class.
//
// This file is a part of the XTREME COMMANDBARS MFC class library.
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
#if !defined(__XTPSHORTCUTMANAGER_H__)
#define __XTPSHORTCUTMANAGER_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXTPCommandBars;
class CXTPPropExchange;


//===========================================================================
// Summary:
//     CXTPShortcutManager is standalone class used to manipulate accelerators of frame.
//===========================================================================
class _XTP_EXT_CLASS CXTPShortcutManager: public CXTPCmdTarget
{
public:

//{{AFX_CODEJOCK_PRIVATE
	class _XTP_EXT_CLASS CKeyNameText
	{
	public:
		CKeyNameText();
		CString Translate(UINT nKey);
		void SetAt(UINT uiVirtKey, LPCTSTR lpszKeyNameText);

	protected:
		CMap<UINT, UINT, CString, CString> m_mapVirtualKeys;
	};

	class _XTP_EXT_CLASS CKeyHelper
	{
	public:

		//-----------------------------------------------------------------------
		// Summary:
		//     Constructs a CKeyHelper object.
		//-----------------------------------------------------------------------
		CKeyHelper(LPACCEL lpAccel, CXTPShortcutManager* pManager = NULL);

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CKeyHelper object, handles cleanup and deallocation
		//-------------------------------------------------------------------------
		virtual ~CKeyHelper();

	public:

		void Format (CString& str) const;
		int Priority();

		static BOOL AFX_CDECL EqualAccels(const ACCEL* pFirst, const ACCEL* pSecond)
		{
			return ((pFirst->fVirt | FNOINVERT) == (pSecond->fVirt | FNOINVERT)) && (pFirst->key == pSecond->key);
		}

		CXTPShortcutManager* GetShortcutManager() const
		{
			return m_pManager;
		}

		//-----------------------------------------------------------------------
		// Summary:
		//     The GetKeyNameText function retrieves a string that represents the name of a key.
		// Parameters:
		//     uiVirtKey - virtual-key of accelerator.
		//-----------------------------------------------------------------------
		static CString AFX_CDECL GetLocalKeyNameText(UINT uiVirtKey);

	protected:
		void AddVirtKeyStr (CString& str, UINT uiVirtKey, BOOL bLast = FALSE) const;

	protected:
		LPACCEL m_lpAccel;
		CXTPShortcutManager* m_pManager;
	};

	class _XTP_EXT_CLASS CKeyAssign : public CEdit
	{
	public:

		//-------------------------------------------------------------------------
		// Summary:
		//     Constructs a CKeyAssign object.
		//-------------------------------------------------------------------------
		CKeyAssign(CXTPShortcutManager* pManager = NULL);

		//-------------------------------------------------------------------------
		// Summary:
		//     Destroys a CKeyAssign object, handles cleanup and deallocation
		//-------------------------------------------------------------------------
		virtual ~CKeyAssign();

	public:
		BOOL IsKeyDefined() const;
		ACCEL* GetAccel();
		void SetAccel(ACCEL& hAccel);
		void ResetKey();

	protected:
		void SetAccelFlag (BYTE bFlag, BOOL bOn);
		virtual BOOL PreTranslateMessage(MSG* pMsg);

	public:
		BOOL        m_bExtendedOnly;
	protected:
		BOOL        m_bKeyDefined;
		ACCEL       m_hAccel;
		CKeyHelper  m_keyHelper;

	};
//}}AFX_CODEJOCK_PRIVATE

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPShortcutManager object.
	// Parameters:
	//     pCommandBars - Owner CommandBars object
	//-------------------------------------------------------------------------
	CXTPShortcutManager(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPShortcutManager object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPShortcutManager();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves shortcut text for specified command.
	// Parameters:
	//     nCmd         - Command which shortcut text need to retrieve
	//     strShortcut  - Shortcut text reference.
	// Returns:
	//     TRUE if shortcut found.
	//-----------------------------------------------------------------------
	virtual BOOL FindDefaultAccelerator (UINT nCmd, CString& strShortcut);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to retrieve text presentation of ACCEL.
	//     You can override this method.
	// Parameters:
	//     lpAccel - Accelerator need to format.
	//     pPriority - Returns priority of accelerator to be used to determine what accelerator used as default.
	//-----------------------------------------------------------------------
	virtual CString Format(LPACCEL lpAccel, int* pPriority);

	//-----------------------------------------------------------------------
	// Summary:
	//     The GetKeyNameText function retrieves a string that represents the name of a key. Override
	//     this member function to provide additional functionality.
	// Parameters:
	//     uiVirtKey - virtual-key of accelerator.
	//-----------------------------------------------------------------------
	virtual CString GetKeyNameText(UINT uiVirtKey);

	//-----------------------------------------------------------------------
	// Summary:
	//     The OnPreviewEditKey function called to determine if user defined
	//     shortcut is allowed.
	// Parameters:
	//     hAccel - User pressed key.
	// Returns:
	//     TRUE if key is allowed
	//-----------------------------------------------------------------------
	virtual BOOL OnPreviewEditKey(ACCEL hAccel);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before user add new shortcut
	// Parameters:
	//     pAccel - New shortcut to add
	// See Also: OnBeforeRemove
	//-----------------------------------------------------------------------
	virtual BOOL OnBeforeAdd(LPACCEL pAccel);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called before user remove shortcut
	// Parameters:
	//     pAccel - Shortcut to be removed
	// See Also: OnBeforeAdd
	//-----------------------------------------------------------------------
	virtual BOOL OnBeforeRemove(LPACCEL pAccel);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to localize shortcut key name text.
	// Parameters:
	//     uiVirtKey - virtual-key of accelerator.
	//     lpszKeyNameText - New text that will be associated with virtual-key
	// Example:
	//     <code>pCommandBars->GetShortcutManager()->SetKeyNameText(VK_CONTROL, _T("Strg"));</code>
	//-----------------------------------------------------------------------
	void SetKeyNameText(UINT uiVirtKey, LPCTSTR lpszKeyNameText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to use system localized shortcuts
	// Parameters:
	//     bSystemKey - TRUE to allow system to localize shortcuts instead of predefined shortcuts table.
	// Example:
	//     <code>pCommandBars->GetShortcutManager()->UseSystemKeyNameText(TRUE);</code>
	//-----------------------------------------------------------------------
	void UseSystemKeyNameText(BOOL bSystemKey);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to enable/disable shortcuts usage
	// Parameters:
	//     bDisable - TRUE to disable shortcuts
	//-----------------------------------------------------------------------
	void DisableShortcuts(BOOL bDisable);

	//-----------------------------------------------------------------------
	// Summary:
	//     The TranslateAccelerator function processes accelerator keys for menu commands.
	// Parameters:
	//     hWnd - Handle to the window whose messages are to be translated.
	//     lpMsg - Pointer to an MSG structure that contains message information
	//-----------------------------------------------------------------------
	BOOL TranslateAccelerator(HWND hWnd, LPMSG lpMsg);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to save the accelerators to the registry
	//     or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that
	//                       specifies the name of a section in the
	//                       initialization file or a key in the Windows
	//                       registry where state information is stored.
	//-----------------------------------------------------------------------
	void SaveShortcuts(LPCTSTR lpszProfileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve accelerators from the registry
	//     or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that
	//                       specifies the name of a section in the
	//                       initialization file or a key in the Windows
	//                       registry where state information is stored.
	//-----------------------------------------------------------------------
	void LoadShortcuts(LPCTSTR lpszProfileName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX    - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to create original accelerators
	// Parameters:
	//     nIDResource - Resource identifier of Accelerator table.
	//----------------------------------------------------------------------
	void SetAccelerators(UINT nIDResource);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set accelerator table
	// Parameters:
	//     hAccelTable - New accelerators to be set.
	//----------------------------------------------------------------------
	void SetDefaultAccelerator(HACCEL hAccelTable);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get accelerator table
	// Returns:
	//     Accelerator table handle
	//----------------------------------------------------------------------
	virtual HACCEL GetDefaultAccelerator() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get original accelerators
	// Parameters:
	//     Accelerator table handle
	//----------------------------------------------------------------------
	HACCEL GetOriginalAccelerator() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to reset shortcuts
	//----------------------------------------------------------------------
	virtual void Reset();

public:
//{{AFX_CODEJOCK_PRIVATE
	void CreateOriginalAccelTable();

	static TCHAR AFX_CDECL ToUpper(TCHAR vkTCHAR);
	static HACCEL AFX_CDECL CopyAccelTable(HACCEL hAccelTable);
	static BOOL AFX_CDECL CompareAccelKey(TCHAR chAccel, UINT wParam);

	void UpdateAcellTable(LPACCEL lpAccel, int nSize);

	void SerializeShortcuts(CArchive& ar);
	void AddShortcut(long fVirt, long key, long cmd);
	BOOL GetShortcut(int ID, ACCEL* pAccel);
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE

	// deprecated members
	void SaveShortcuts(CFrameWnd* pFrame, LPCTSTR lpszProfileName)
	{
		if (!m_hAccelTable)
			m_hAccelTable = CopyAccelTable(pFrame->m_hAccelTable);

		SaveShortcuts(lpszProfileName);
	}
	void LoadShortcuts(CFrameWnd* pFrame, LPCTSTR lpszProfileName)
	{
		if (!m_hOriginalAccelTable)
			m_hOriginalAccelTable = CopyAccelTable(pFrame->m_hAccelTable);

		LoadShortcuts(lpszProfileName);
	}
	void DoPropExchange(CXTPPropExchange* pPX, CFrameWnd* pFrame)
	{
		if (!m_hAccelTable)
			m_hAccelTable = CopyAccelTable(pFrame->m_hAccelTable);

		DoPropExchange(pPX);
	}

//}}AFX_CODEJOCK_PRIVATE


public:
	BOOL m_bAllowEscapeShortcut;        // TRUE to allow escape to use as shortcut
	BOOL m_bUseSystemKeyNameText;       // TRUE to use localized shortcuts
	int m_nDisableShortcuts;            // Number of shortcuts that have been disabled.

protected:
	CXTPCommandBars* m_pCommandBars;    // Owner command bars object

	HACCEL m_hAccelTable;               // Accelerator table
	HACCEL m_hOriginalAccelTable;       // Original accelerator table.

	CKeyNameText* m_pKeyNameText;       // KeyNameText map.
};

AFX_INLINE HACCEL CXTPShortcutManager::GetDefaultAccelerator() const {
	return m_hAccelTable;
}
AFX_INLINE HACCEL CXTPShortcutManager::GetOriginalAccelerator() const {
	return m_hOriginalAccelTable;
}
AFX_INLINE void CXTPShortcutManager::UseSystemKeyNameText(BOOL bSystemKey) {
	m_bUseSystemKeyNameText = bSystemKey;
}


#endif //#if !defined(__XTPSHORTCUTMANAGER_H__)
