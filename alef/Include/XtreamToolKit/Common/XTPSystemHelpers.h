// XTPSystemHelpers
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
#if !defined(__XTPSYSTEMHELPERS_H__)
#define __XTPSYSTEMHELPERS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <oleacc.h>

class CXTPSystemVersion;
// -----------------------------------------------------------------------
// Summary:
//     The XTPSystemVersion function is used for access to the
//     CXTPSystemVersion class.
// Remarks:
//     Call this function to access CXTPSystemVersion members. Since
//     this class is designed as a single instance object you can only
//     access its members through this method. You cannot directly
//     instantiate an object of type CXTPSystemVersion.
// Returns:
//     A pointer to the one and only CXTPSystemVersion instance.
// Example:
//     The following example demonstrates the use of XTPSystemVersion.
// <code>
// bool bIsWinNT = XTPSystemVersion()-\>IsWinNT4();
// </code>
// See Also:
//     CXTPSystemVersion
// -----------------------------------------------------------------------
_XTP_EXT_CLASS CXTPSystemVersion* AFXAPI XTPSystemVersion();

//===========================================================================
// Summary:
//     CXTPSystemVersion is a OSVERSIONINFO derived class. This class
//     wraps the Win32 API GetVersionEx(...), used to get the current
//     Windows OS version. CXTPSystemVersion is a single instance, or
//     "singleton" object, that is accessed with the Get() method.
//===========================================================================
class _XTP_EXT_CLASS CXTPSystemVersion : public OSVERSIONINFO
{
private:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSystemVersion object.
	//-----------------------------------------------------------------------
	CXTPSystemVersion();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 3.1.
	// Returns:
	//     true if the OS is Windows 3.1, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin31() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 95.
	// Returns:
	//     true if the OS is Windows 95, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin95() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 98.
	// Returns:
	//     true if the OS is Windows 98, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin98() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows ME.
	// Returns:
	//     true if the OS is Windows ME, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinME() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows NT 4.
	// Returns:
	//     true if the OS is Windows NT 4, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinNT4() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows 2000.
	// Returns:
	//     true if the OS is Windows 2000, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin2K() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is Windows XP.
	// Returns:
	//     true if the OS is Windows XP, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinXP() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is greater than or equal to Windows 3.1.
	// Returns:
	//     true if the OS is greater than or equal to Windows 3.1,
	//     otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin31OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to check whether or not the
	//      operating system is of the Windows 9x family, and if it is
	//      Windows 95 or a later version.
	// Returns:
	//      true if the OS is of the Windows 9x family, and is Windows 95
	///     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin95OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family, and if it is
	//     Windows 98 or a later version.
	// Returns:
	//     true if the OS is of the Windows 9x family, and is Windows 98
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin98OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family
	// Returns:
	//     true if the OS is of the Windows 9x family
	//-----------------------------------------------------------------------
	bool IsWin9x() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows 9x family, and if it is
	//     Windows ME or a later version.
	// Returns:
	//     true if the OS is of the Windows 9x family, and is Windows ME
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinMEOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows NT 4 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows NT
	//     4 or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinNT4OrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows 2000 or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows
	//     2000 or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWin2KOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows XP or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows XP
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinXPOrGreater() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to check whether or not the
	//     operating system is of the Windows NT family, and if it is
	//     Windows Vista or a later version.
	// Returns:
	//     true if the OS is of the Windows NT family, and is Windows Vista
	//     or a later version, otherwise returns false.
	//-----------------------------------------------------------------------
	bool IsWinVistaOrGreater() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Use this member function to return the version number of the
	//      comctl32.dll. The high-order word of the return
	//     value represents the major version number and the low-order
	//     word of the returned value represents the minor version number.
	// Returns:
	//     A DWORD value if successful, otherwise 0L.
	//-----------------------------------------------------------------------
	DWORD GetComCtlVersion() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if right-to-left (RTL) formatting is supported by current OS
	// Returns:
	//     TRUE if right-to-left (RTL) formatting is supported.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTLSupported();

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if ClearType text is supported by current OS
	//-----------------------------------------------------------------------
	BOOL IsClearTypeTextQualitySupported();


private:

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks to see if the OS is greater or equal to the specified
	//     version number
	// Returns:
	//     true if the OS is greater than or equal to the specified
	//     version.
	//-----------------------------------------------------------------------
	bool GreaterThanEqualTo(const DWORD maj, const DWORD min) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Checks to see if the OS is equal to the specified version
	//     number
	// Returns:
	//     true if the OS is equal to the specified version.
	//-----------------------------------------------------------------------
	bool EqualTo(const DWORD maj, const DWORD min) const;

private:

	friend _XTP_EXT_CLASS CXTPSystemVersion* AFXAPI XTPSystemVersion();
};

// Depricated
#define XTOSVersionInfo XTPSystemVersion



//===========================================================================
// CXTPModuleHandle class is helper for LoadLibray/GetModuleHandle methods
//===========================================================================
class _XTP_EXT_CLASS CXTPModuleHandle
{
public:
	//-----------------------------------------------------------------------
	// Module state
	//-----------------------------------------------------------------------
	enum XTPModuleState
	{
		xtpModNone,     // Indicates no module found.
		xtpModMapped,   // Indicates the module handle was set using GetModuleHandle
		xtpModLoaded    // Indicates the module handle was set using LoadLibrary.
	};
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPModuleHandle object.
	//-----------------------------------------------------------------------
	CXTPModuleHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPModuleHandle object.
	// Parameters:
	//     lpszModuleName - Pointer to a null-terminated string that names
	//                  the .DLL file. The name specified is the
	//                  filename of the module and is not related to the
	//                  name stored in the library module itself, as
	//                  specified by the LIBRARY keyword in the module-
	//                  definition (.DEF) file.
	//                  If the string specifies a path but the file does
	//                  not exist in the specified directory, the
	//                  function fails.  When specifying a path, be sure
	//                  to use backslashes (\), not forward slashes (/).
	//-----------------------------------------------------------------------
	CXTPModuleHandle(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPModuleHandle object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPModuleHandle();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to LoadLibrary
	//-----------------------------------------------------------------------
	BOOL Init(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to load the library specified
	//     by 'lpszModule'.  Once the library is loaded, you can
	//     retrieve the instance handle by using the HINSTANCE operator.
	// Parameters:
	//     lpszModuleName - Pointer to a null-terminated string that names
	//                  the .DLL file. The name specified is the
	//                  filename of the module and is not related to the
	//                  name stored in the library module itself, as
	//                  specified by the LIBRARY keyword in the module-
	//                  definition (.DEF) file.
	//                  If the string specifies a path but the file does
	//                  not exist in the specified directory, the
	//                  function fails.  When specifying a path, be sure
	//                  to use backslashes (\), not forward slashes (/).
	//-----------------------------------------------------------------------
	BOOL LoadLibrary(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     The GetModuleHandle function retrieves a module handle for the specified module
	//     if the file has been mapped into the address space of the calling process.
	// Parameters:
	//     lpModuleName - Pointer to a null-terminated string that contains the name
	//     of the module (either a .dll or .exe file).
	//-----------------------------------------------------------------------
	BOOL GetModuleHandle(LPCTSTR lpszModuleName);

	//-----------------------------------------------------------------------
	// Summary:
	//     The FreeLibrary function decrements the reference count of the loaded
	//     dynamic-link library (DLL).
	//-----------------------------------------------------------------------
	BOOL FreeLibrary();

	//-----------------------------------------------------------------------
	// Summary:
	//     The GetProcAddress function retrieves the address of the specified
	//     exported dynamic-link library (DLL) function.
	//-----------------------------------------------------------------------
	BOOL GetProcAddress(FARPROC* ppFnPtr, LPCSTR lpProcName, DWORD dwMinVer = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Use this member function to return the version number of the
	//     module attached to this CLoadLibrary object. The high-order
	//     word of the return value represents the major version number
	//     and the low-order word of the returned value represents the
	//     minor version number.
	// Returns:
	//     A DWORD value if successful, otherwise 0L.
	//-----------------------------------------------------------------------
	DWORD GetVersion();

	//-----------------------------------------------------------------------
	// Summary:
	//     This overloaded operator returns a handle to the module
	//     indicating success. NULL indicates failure.
	// Returns:
	//     A handle to the module if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	operator HMODULE () const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method returns a handle to the module
	//     indicating success. NULL indicates failure.
	// Returns:
	//     A handle to the module if successful, otherwise returns NULL.
	//-----------------------------------------------------------------------
	HMODULE GetHandle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns Path to loaded library
	//-----------------------------------------------------------------------
	CString GetName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns module state
	//-----------------------------------------------------------------------
	XTPModuleState GetModuleState() const;

private:
	BOOL GetVersionInfo();

	struct XTP_DLLVERSIONINFO
	{
		DWORD cbSize;
		DWORD dwMajorVersion;                   // Major version
		DWORD dwMinorVersion;                   // Minor version
		DWORD dwBuildNumber;                    // Build number
		DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
	};

	CString            m_strModuleName;         // Module path
	HMODULE            m_hModule;               // A handle to the module indicates success.
	XTPModuleState     m_eModuleState;          // Module state
	XTP_DLLVERSIONINFO m_dvInfo;                // Version information
};

AFX_INLINE HMODULE CXTPModuleHandle::GetHandle() const {
	return m_hModule;
}
AFX_INLINE CXTPModuleHandle::operator HMODULE () const {
	return m_hModule;
}
AFX_INLINE CString CXTPModuleHandle::GetName() const {
	return m_strModuleName;
}
AFX_INLINE CXTPModuleHandle::XTPModuleState CXTPModuleHandle::GetModuleState() const {
	return m_eModuleState;
}

// Obsolete name
//#define CXTLoadLibrary CXTPModuleHandle


//-----------------------------------------------------------------------
// Summary:
//     MultiMonitor API wrapper class
//-----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPMultiMonitor
{
private:
	DECLARE_HANDLE(XTP_HMONITOR);

private:
	struct XTP_MONITORINFO
	{
		DWORD   cbSize;
		RECT    rcMonitor;
		RECT    rcWork;
		DWORD   dwFlags;
	};

private:
	CXTPMultiMonitor();


public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This function retrieves work area for the display monitor that
	//     the mouse cursor is currently positioned over.
	// Parameters:
	//     rect  - Reference to a CRect class that specifies the
	//             coordinates of the rectangle of interest in virtual
	//             screen coordinates.
	//     pWnd   - Pointer to the window of interest.
	//     point   - Point of interest.
	// Returns:
	//     If the mouse cursor is positioned over a display monitor, the
	//     return value is a CRect object that specifies the work area
	//     rectangle of the display monitor, expressed in virtual-screen
	//     coordinates.  Otherwise, returns the size of the work area on
	//     the primary display monitor.
	//-----------------------------------------------------------------------
	CRect GetWorkArea();
	CRect GetWorkArea(const POINT& point);  //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(LPCRECT rect); //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(const CWnd* pWnd);  //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetWorkArea(HWND hWnd); //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(const CWnd* pWnd);  //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(HWND hWnd); //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(const POINT& ptScreenCoords); //<COMBINE CXTPMultiMonitor::GetWorkArea>
	CRect GetScreenArea(LPCRECT rect); //<COMBINE CXTPMultiMonitor::GetWorkArea>

private:
	BOOL GetMonitorInfo(XTP_HMONITOR hMonitor, XTP_MONITORINFO* lpMonitorInfo);
	CRect GetWorkArea(XTP_HMONITOR hMonitor);
	CRect GetScreenArea(XTP_HMONITOR hMonitor);

	int      (WINAPI* m_pfnGetSystemMetrics)(int);
	XTP_HMONITOR (WINAPI* m_pfnMonitorFromWindow)(HWND, DWORD);
	XTP_HMONITOR (WINAPI* m_pfnMonitorFromRect)(LPCRECT, DWORD);
	XTP_HMONITOR (WINAPI* m_pfnMonitorFromPoint)(POINT, DWORD);
	BOOL     (WINAPI* m_pfnGetMonitorInfo)(XTP_HMONITOR, XTP_MONITORINFO*);
	CXTPModuleHandle m_modUser32;

	friend _XTP_EXT_CLASS CXTPMultiMonitor* AFX_CDECL XTPMultiMonitor();
};


//---------------------------------------------------------------------------
// Summary:
//     Call this function to access CXTPMultiMonitor members.
//     Since this class is designed as a single instance object you can
//     only access its members through this method. You <b>cannot</b>
//     directly instantiate an object of type CXTPMultiMonitor.
// Example:
//     <code>CRect rcWork = XTPMultiMonitor()->GetWorkArea();</code>
//---------------------------------------------------------------------------
_XTP_EXT_CLASS CXTPMultiMonitor* AFX_CDECL XTPMultiMonitor();


//===========================================================================
// Summary: Critical Section Wrapper
//===========================================================================
class _XTP_EXT_CLASS CXTPCriticalSection
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCriticalSection object
	//-----------------------------------------------------------------------
	CXTPCriticalSection();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCriticalSection object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPCriticalSection();

public:

	//-------------------------------------------------------------------------
	// Summary:
	//     The EnterCriticalSection function waits for ownership of the
	//     specified critical section object
	//-------------------------------------------------------------------------
	void EnterCritical();

	//-------------------------------------------------------------------------
	// Summary:
	//     The LeaveCriticalSection function releases ownership of the
	//     specified critical section object
	//-------------------------------------------------------------------------
	void LeaveCritical();

private:
	CRITICAL_SECTION m_csMutex;
};

//===========================================================================
// Summary: Critical Sections Lock/Unlock helper
//===========================================================================
class _XTP_EXT_CLASS CXTPLockGuard
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPLockGuard object
	// Parameters:
	//     key - CXTPCriticalSection used to lock thread
	//-----------------------------------------------------------------------
	CXTPLockGuard(CXTPCriticalSection& key);

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPCriticalSection object, handles cleanup and deallocation
	//-------------------------------------------------------------------------
	~CXTPLockGuard();

public:

	//-------------------------------------------------------------------------
	// Summary: This method call EnterCritical of key object
	//-------------------------------------------------------------------------
	void LockThread();

	//-------------------------------------------------------------------------
	// Summary: This method call LeaveCritical of key object
	//-------------------------------------------------------------------------
	void UnLockThread();

private:
	DISABLE_COPY_OPERATOR(CXTPLockGuard)

private:
	CXTPCriticalSection& m_key;
};



//{{AFX_CODEJOCK_PRIVATE

#ifndef CHILDID_SELF
#define CHILDID_SELF 0
#endif

#ifndef WM_GETOBJECT
#define WM_GETOBJECT 0x003D
#endif

#ifndef EVENT_OBJECT_FOCUS
#define EVENT_OBJECT_FOCUS 0x8005
#endif

#ifndef OBJID_MENU
#define OBJID_MENU 0xFFFFFFFD
#endif

#ifndef OBJID_CLIENT
#define OBJID_CLIENT 0xFFFFFFFC
#endif

#ifndef OBJID_WINDOW
#define OBJID_WINDOW 0x00000000
#endif

class _XTP_EXT_CLASS CXTPAccessible
{
public:
	CXTPAccessible();
	virtual ~CXTPAccessible();

public:
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleValue(VARIANT varChild, BSTR* pszValue);
	virtual HRESULT GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual HRESULT GetAccessibleHelp(VARIANT varChild, BSTR* pszHelp);
	virtual HRESULT GetAccessibleHelpTopic(BSTR *pszHelpFile, VARIANT varChild, long* pidTopic);
	virtual HRESULT GetAccessibleKeyboardShortcut(VARIANT varChild, BSTR* pszKeyboardShortcut);
	virtual HRESULT GetAccessibleFocus(VARIANT* pvarChild);
	virtual HRESULT GetAccessibleSelection(VARIANT* pvarChildren);
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleNavigate(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	//Obsolete
	virtual HRESULT PutAccessibleName(VARIANT varChild, BSTR szName);
	//Obsolete
	virtual HRESULT PutAccessibleValue(VARIANT varChild, BSTR szValue);

	virtual CCmdTarget* GetAccessible();

	void AccessibleNotifyWinEvent(DWORD event, HWND hwnd, LONG idObject, LONG idChild);
protected:
	HRESULT AccessibleObjectFromWindow(HWND hwnd, DWORD dwId, REFIID riid, void** ppvObject);
	LRESULT LresultFromObject(REFIID riid, WPARAM wParam, LPUNKNOWN punk);

	long GetChildIndex(VARIANT* varChild);

public:
	BEGIN_INTERFACE_PART(Accessible, IAccessible)
		STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo);
		STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo);
		STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
			LCID lcid, DISPID FAR* rgdispid);
		STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
			DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
			UINT FAR* puArgErr);

		STDMETHOD(get_accParent)(IDispatch* FAR* ppdispParent);
		STDMETHOD(get_accChildCount)(long FAR* pChildCount);
		STDMETHOD(get_accChild)(VARIANT varChildIndex, IDispatch* FAR* ppdispChild);

		STDMETHOD(get_accName)(VARIANT varChild, BSTR* pszName);
		STDMETHOD(get_accValue)(VARIANT varChild, BSTR* pszValue);
		STDMETHOD(get_accDescription)(VARIANT varChild, BSTR FAR* pszDescription);
		STDMETHOD(get_accRole)(VARIANT varChild, VARIANT* pvarRole);
		STDMETHOD(get_accState)(VARIANT varChild, VARIANT* pvarState);
		STDMETHOD(get_accHelp)(VARIANT varChild, BSTR* pszHelp);
		STDMETHOD(get_accHelpTopic)(BSTR* pszHelpFile, VARIANT varChild, long* pidTopic);
		STDMETHOD(get_accKeyboardShortcut)(VARIANT varChild, BSTR* pszKeyboardShortcut);
		STDMETHOD(get_accFocus)(VARIANT FAR* pvarFocusChild);
		STDMETHOD(get_accSelection)(VARIANT FAR* pvarSelectedChildren);
		STDMETHOD(get_accDefaultAction)(VARIANT varChild, BSTR* pszDefaultAction);

		STDMETHOD(accSelect)(long flagsSelect, VARIANT varChild);
		STDMETHOD(accLocation)(long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varChild);
		STDMETHOD(accNavigate)(long navDir, VARIANT varStart, VARIANT* pvarEndUpAt);
		STDMETHOD(accHitTest)(long xLeft, long yTop, VARIANT* pvarChildAtPoint);
		STDMETHOD(accDoDefaultAction)(VARIANT varChild);

		STDMETHOD(put_accName)(VARIANT varChild, BSTR szName);
		STDMETHOD(put_accValue)(VARIANT varChild, BSTR pszValue);
	END_INTERFACE_PART(ExternalAccessible)

private:
	typedef VOID (WINAPI* LPFNNOTIFYWINEVENT)(DWORD event, HWND hwnd, LONG idObject, LONG idChild);

	LPFNNOTIFYWINEVENT m_pNotifyWinEvent;
	LPFNLRESULTFROMOBJECT m_pLresultFromObject;
	LPFNACCESSIBLEOBJECTFROMWINDOW m_pAccessibleObjectFromWindow;

	CXTPModuleHandle m_modUser32;
	CXTPModuleHandle m_modOleAcc;
};

//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTPMANAGESTATE_H__)
