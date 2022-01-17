// XTPMacros.h : common macros
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
#if !defined(__XTPMACROS_H__)
#define __XTPMACROS_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//-----------------------------------------------------------------------------
// Summary:
//     Frees dynamically allocated memory.
// Parameters:
//     ptr - Points to the memory to free.
// Remarks:
//     This macro will first check to see if the block of memory specified by
//     <i>ptr</i> is NULL. If <i>ptr</i> is not NULL, SAFE_DELETE will deallocate
//     the block of memory and set its value to NULL. The pointer argument
//     must refer to a block of memory previously allocated for an object
//     created with the new operator.
// Example:
// <code>
// CName* pName = new CName;          // Allocate memory for the object
// pName->SetName("Firstname", "Lastname");
// SAFE_DELETE(pName);                // Deallocate memory for the object
// ASSERT(pName == NULL);
// </code>
//-----------------------------------------------------------------------------
#define SAFE_DELETE(ptr)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	if (ptr) { delete ptr; ptr = NULL; }
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     Frees dynamically allocated memory for an array.
// Parameters:
//     ptr - Points to the memory array to free.
// Remarks:
//     This macro will first check to see if the block of memory specified by
//     <i>ptr</i> is NULL. If <i>ptr</i> is not NULL, SAFE_DELETE_AR will deallocate
//     the block of memory and set its value to NULL. The pointer argument
//     must refer to a block of memory previously allocated for an object
//     created with the new operator.
// Example:
// <code>
// char* pCharArray = new char[256];  // Allocate memory for the array
// strcpy(pCharArray, "Array of characters");
// SAFE_DELETE_AR(pCharArray);        // Deallocate memory for the array
// ASSERT(pCharArray == NULL);
// </code>
//-----------------------------------------------------------------------------
#define SAFE_DELETE_AR(ptr)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_DELETE_AR
#define SAFE_DELETE_AR(ptr) \
	if (ptr) { delete [] ptr; ptr = NULL; }
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     Decrements the reference count for the specified object.
// Parameters:
//     comPointer - Points to the COM object to release.
// Remarks:
//     This macro will first check to see if the COM object specified by <i>comPointer</i>
//     is NULL. If the object is not NULL, the macro calls IUnknown::Release
//     COM object specified by <i>comPointer</i> and set its value to NULL.
// See Also:
//     IUnknown::Release
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(comPointer)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_RELEASE
#define SAFE_RELEASE(comPointer) \
	if (comPointer) { (comPointer)->Release(); (comPointer)=NULL; }
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     Checks object pointer and if not NULL calls a specified function.
// Parameters:
//     classPointer - Pointer to the class that contains the function to call.
//     functionName - Name of the function to call.
// Remarks:
//     This macro will first check to see if the class specified by <i>classPointer</i>
//     is NULL. If the class is not NULL, the macro calls the function specified
//     by <i>functionName</i>.
// Example:
// <code>
//  SAFE_CALLPTR(m_pItem, OnConstraintsChanged());
// </code>
//-----------------------------------------------------------------------------
#define SAFE_CALLPTR(classPointer, functionName)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_CALLPTR
#define SAFE_CALLPTR(classPointer, functionName) \
	if (classPointer) classPointer->functionName
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     Destroys an icon and frees any memory the icon occupied.
// Parameters:
//     hIcon - Handle to the icon to free.
// Remarks:
//     This macro will first check to see if the icon handle specified by
//     <i>hIcon</i> is NULL. If <i>hIcon</i> is not NULL, SAFE_DELETE_HICON will
//     destroy the icon handle and set its value to NULL.
// Example:
// <code>
// HICON hIcon = XTPResourceManager()->LoadIcon(
//     MAKEINTRESOURCE(dwID), item.size);
// ::DrawIconEx(pDC->GetSafeHdc(), point.x, point.y, hIcon,
//     size.cx, size.cy, 0, NULL, DI_NORMAL);
// SAFE_DELETE_HICON(hIcon);
// ASSERT(hIcon == NULL);
// </code>
//-----------------------------------------------------------------------------
#define SAFE_DELETE_HICON(hIcon)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_DELETE_HICON
#define SAFE_DELETE_HICON(hIcon) \
	if (hIcon) {::DestroyIcon(hIcon); hIcon = NULL;}
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     Destroys a GDI object and frees the handle.
// Parameters:
//     obj - Handle to the GDI object free.
// Remarks:
//     This macro will first check to see if the GDI object specified by
//     <i>obj</i> is NULL. If <i>obj</i> is not NULL, SAFE_DELETE_OBJ will
//     delete the GDI object and set its value to NULL.
// Example:
// <code>
// extern CFont m_font;
// extern LOGFONT lf;
// SAFE_DELETE_OBJ(m_font);
// m_font.CreateFontIndirect(&lf);
// </code>
//-----------------------------------------------------------------------------
#define SAFE_DELETE_OBJ(obj)
//{{AFX_CODEJOCK_PRIVATE
#undef SAFE_DELETE_OBJ
#define SAFE_DELETE_OBJ(obj) \
	if (obj.GetSafeHandle()) {obj.DeleteObject();}
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
//-----------------------------------------------------------------------------
// Uncomment this definition if your application uses a resource dll, and add
// #include "XTResource.rc" to your resource dll's .rc2 file.  You will have to
// call XTAuxData().InitResources(HINSTANCE), once you have a resource handle, to
// initialize the Xtreme Toolkit resources.  You will need to rebuild the
// library after you have done this.
//-----------------------------------------------------------------------------
//#define _XT_USES_RESOURCE_DLL

//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------
// Summary:
//     XT_ASSERT_MSG is similar to ASSERT, except that it allows a custom
//     message to be specified.
// Parameters:
//     exp - Specifies an expression (including pointer values)
//           that evaluates to nonzero or 0.
//     msg - Specifies a NULL terminated user defined string of
//           the message to display to the user.
// Remarks:
//     Evaluates its argument. If the result is 0, the macro prints a
//     diagnostic message and aborts the program. If the condition is
//     nonzero, it does nothing.<p/>
//
//     The diagnostic message has the form "assertion failed in file
//     <i>name</i> in line <i>num</i>.<p/>
//
//     where <i>name</i> is the name of the source file, and <i>num</i>
//     is the line number of the assertion that failed in the source file.<p/>
//
//     In the Release version of MFC, XT_ASSERT_MSG does not evaluate the
//     expression and thus will not interrupt the program. If the expression
//     must be evaluated regardless of environment, use the VERIFY macro in
//     place of XT_ASSERT_MSG.<p/>
//
//     <b>Note</b> This function is available only in the Debug version of
//     MFC.
// Example:
// <code>
// // example for XT_ASSERT_MSG
// CAge* pAge = new CAge(21); // CAge is derived from CObject.
//
// // Terminates program only if pAge is NOT a CAge*.
// XT_ASSERT_MSG(pAge!= NULL, _T("Failed to allocate memory."));
// XT_ASSERT_MSG(pAge->IsKindOf(RUNTIME_CLASS(CAge)), _T("This is not a CAge object."));
// </code>
// See Also:
//     ASSERT, VERIFY
//-----------------------------------------------------------------------------
#define XT_ASSERT_MSG(exp, msg)
//{{AFX_CODEJOCK_PRIVATE
#undef XT_ASSERT_MSG
#ifdef _DEBUG
#ifndef _AFX_NO_DEBUG_CRT
#define XT_ASSERT_MSG(exp, msg) \
{ \
	if (!(exp) && (_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "\n-----------------------\n" msg "\n-----------------------"))) \
		AfxDebugBreak(); \
} \

#else
#define XT_ASSERT_MSG(exp, msg) (void)((exp) || (_assert("\n-----------------------\n" msg "\n-----------------------", __FILE__, __LINE__), 0))
#endif//_AFX_NO_DEBUG_CRT

#else
#define XT_ASSERT_MSG(exp, msg) ((void)0)
#endif//_DEBUG
//}}AFX_CODEJOCK_PRIVATE

//{{AFX_CODEJOCK_PRIVATE
#ifdef _DEBUG
#ifndef _AFX_NO_DEBUG_CRT
#define XT_ERROR_MSG(msg) \
{ \
	if ((_CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "\n-----------------------\n" msg "\n-----------------------"))) \
		AfxDebugBreak(); \
} \

#else
#define XT_ERROR_MSG(msg) (void)((_assert("\n-----------------------\n" msg "\n-----------------------", __FILE__, __LINE__), 0))
#endif//_AFX_NO_DEBUG_CRT

#else
#define XT_ERROR_MSG(msg) ((void)0)
#endif//_DEBUG




#if _MSC_VER >= 1400
#define _XTP_DEPRECATE(_Message) __declspec(deprecated(_Message))
#elif _MSC_VER >= 1300
#define _XTP_DEPRECATE(_Message) __declspec(deprecated)
#else
#define _XTP_DEPRECATE(_Message)
#endif




#ifdef _UNICODE
#ifndef UNICODE
#define UNICODE
#endif
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)"): "

// usage: #pragma message(__LOC__"Need to do 3D collision testing")

#ifdef _AFXDLL
	#define SAFE_MANAGE_STATE(s) AFX_MANAGE_STATE(s)
#else
	#define SAFE_MANAGE_STATE(s)
#endif

#ifndef AFX_INLINE
#define AFX_INLINE inline
#endif

#ifndef CBRS_GRIPPER
#define CBRS_GRIPPER                    0x00400000L
#endif

#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL                 0x00400000L
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#if (_MSC_VER >= 1300)
template<> AFX_INLINE UINT AFXAPI HashKey<UINT_PTR>(UINT_PTR key) {
	return (DWORD)(((DWORD_PTR)key)>>4);
}
#endif


#if (_MSC_VER <= 1200) && !defined(_WIN64)

// IA64 Macros:
#ifndef DWORD_PTR
#define DWORD_PTR DWORD
#endif

#ifndef UINT_PTR
#define UINT_PTR UINT
#endif

#ifndef INT_PTR
#define INT_PTR int
#endif

#ifndef ULONG_PTR
#define ULONG_PTR ULONG
#endif

#ifndef LONG_PTR
#define LONG_PTR long
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr GetWindowLong
#endif

#ifndef GetClassLongPtr
#define GetClassLongPtr GetClassLong
#endif

#ifndef SetClassLongPtr
#define SetClassLongPtr SetClassLong
#endif

#ifndef GCLP_MENUNAME
#define GCLP_MENUNAME       (-8)
#endif

#ifndef GCLP_HBRBACKGROUND
#define GCLP_HBRBACKGROUND  (-10)
#endif

#ifndef GCLP_HCURSOR
#define GCLP_HCURSOR        (-12)
#endif

#ifndef GCLP_HICON
#define GCLP_HICON          (-14)
#endif

#ifndef GCLP_HMODULE
#define GCLP_HMODULE        (-16)
#endif

#ifndef GCLP_WNDPROC
#define GCLP_WNDPROC        (-24)
#endif

#ifndef GCLP_HICONSM
#define GCLP_HICONSM        (-34)
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC        (-4)
#endif

#ifndef GWLP_HINSTANCE
#define GWLP_HINSTANCE      (-6)
#endif

#ifndef GWLP_HWNDPARENT
#define GWLP_HWNDPARENT     (-8)
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA       (-21)
#endif

#ifndef GWLP_ID
#define GWLP_ID             (-12)
#endif

#endif

#define ON_WM_NCHITTEST_EX() \
	{ WM_NCHITTEST, 0, 0, 0, AfxSig_wp, \
		(AFX_PMSG)(AFX_PMSGW) \
		(static_cast< LRESULT (AFX_MSG_CALL CWnd::*)(CPoint) > (OnNcHitTest)) },


#define CMDTARGET_RELEASE(x) if (x) { x->InternalRelease(); x = 0;}
#define CMDTARGET_ADDREF(x) if (x) { x->InternalAddRef(); }
#define SAFE_INVALIDATE(pWnd) if (pWnd && pWnd->GetSafeHwnd()) pWnd->Invalidate(FALSE);

#define CXTPCmdTarget CCmdTarget

#define DISABLE_COPY_OPERATOR(theClass) \
	private:\
	theClass& operator=(const theClass &) { ASSERT(FALSE); return *this; }

#define DISABLE_WNDCREATE() \
	private:\
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {\
		return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);}

#define DISABLE_WNDCREATEEX() \
	private:\
	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,\
		int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU nIDorHMenu, LPVOID lpParam = NULL) {\
		return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, nIDorHMenu, lpParam);\
	}\
	BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,\
		const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL) {\
		return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);\
	}

//}}AFX_CODEJOCK_PRIVATE


//{{AFX_CODEJOCK_PRIVATE
#ifdef __XTPDLLEXPORTS_H__

#if (_MSC_VER > 1200) && !defined(_WIN64)

#ifdef SetWindowLongPtrA
#undef SetWindowLongPtrA
inline LONG_PTR SetWindowLongPtrA( HWND hWnd, int nIndex, LONG_PTR dwNewLong )
{
	return( ::SetWindowLongA( hWnd, nIndex, LONG( dwNewLong ) ) );
}
#endif

#ifdef SetWindowLongPtrW
#undef SetWindowLongPtrW
inline LONG_PTR SetWindowLongPtrW( HWND hWnd, int nIndex, LONG_PTR dwNewLong )
{
	return( ::SetWindowLongW( hWnd, nIndex, LONG( dwNewLong ) ) );
}
#endif

#ifdef GetWindowLongPtrA
#undef GetWindowLongPtrA
inline LONG_PTR GetWindowLongPtrA( HWND hWnd, int nIndex )
{
	return( ::GetWindowLongA( hWnd, nIndex ) );
}
#endif

#ifdef GetWindowLongPtrW
#undef GetWindowLongPtrW
inline LONG_PTR GetWindowLongPtrW( HWND hWnd, int nIndex )
{
	return( ::GetWindowLongW( hWnd, nIndex ) );
}
#endif

#ifdef SetClassLongPtrA
#undef SetClassLongPtrA
inline LONG_PTR SetClassLongPtrA( HWND hWnd, int nIndex, LONG_PTR dwNewLong )
{
	return( ::SetClassLongA( hWnd, nIndex, LONG( dwNewLong ) ) );
}
#endif

#ifdef SetClassLongPtrW
#undef SetClassLongPtrW
inline LONG_PTR SetClassLongPtrW( HWND hWnd, int nIndex, LONG_PTR dwNewLong )
{
	return( ::SetClassLongW( hWnd, nIndex, LONG( dwNewLong ) ) );
}
#endif

#ifdef GetClassLongPtrA
#undef GetClassLongPtrA
inline LONG_PTR GetClassLongPtrA( HWND hWnd, int nIndex )
{
	return( ::GetClassLongA( hWnd, nIndex ) );
}
#endif

#ifdef GetClassLongPtrW
#undef GetClassLongPtrW
inline LONG_PTR GetClassLongPtrW( HWND hWnd, int nIndex )
{
	return( ::GetClassLongW( hWnd, nIndex ) );
}
#endif

#endif


// Note: afxData.cxBorder and afxData.cyBorder aren't used anymore
#define CX_BORDER   1
#define CY_BORDER   1

#define _AfxGetDlgCtrlID(hWnd) ((UINT)(WORD)::GetDlgCtrlID(hWnd))

#define AFX_WNDCOMMCTLS_REG             0x00010 // means all original Win95
#define AFX_WNDCOMMCTL_BAR_REG          0x01000
#define AFX_WNDCOMMCTL_USEREX_REG       0x10000
#define AFX_WNDCOMMCTL_DATE_REG         0x20000

#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)

#if (_MSC_VER <= 1100)
extern BOOL AFXAPI AfxEndDeferRegisterClass(short fClass);
#else
BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#endif

HWND AFXAPI AfxGetParentOwner(HWND hWnd);


#endif // __XTPDLLEXPORTS_H__
//}}AFX_CODEJOCK_PRIVATE

//-----------------------------------------------------------------------------

#endif // #if !defined(__XTPMACROS_H__)
