/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2001-2002 by David Yuheng Zhao
//
// Distribute and change freely, except: don't remove my name from the source 
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Partly based on the _ThemeHelper struct in MFC7.0 source code (winctrl3.cpp), 
// and the difference is that this implementation wraps the full set of 
// visual style APIs from the platform SDK August 2001
//
// If you have any questions, I can be reached as follows:
//	yuheng_zhao@yahoo.com
//
//
// How to use:
// Instead of calling the API directly, 
//    OpenThemeData(...);
// use the global variable
//    g_xpStyle.OpenThemeData(...);
//
//
//
// Date: 2002-07-27
// This update was made by Mathias Tunared. 
// He changed the function variable to static to make the code faster. 
// He also added the function 'UseVisualStyles()' so you can check if to
// draw with the visual styles or not. That function checks first the 
// version of the 'ComCtl32.dll' and the checks if the theme is activated 
// and the HTHEME handle is not NULL.
//
// CAdvComboBox Control
// Version: 2.1
// Date: August 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "visualstylesxp.h"
#include <Shlwapi.h>


CVisualStylesXP g_xpStyle;

CVisualStylesXP::CVisualStylesXP(void)
{
	m_hThemeDll = LoadLibrary(_T("UxTheme.dll"));
}

CVisualStylesXP::~CVisualStylesXP(void)
{
	if (m_hThemeDll!=NULL)
		FreeLibrary(m_hThemeDll);
	m_hThemeDll = NULL;
}

void* CVisualStylesXP::GetProc(LPCSTR szProc, void* pfnFail)
{
	void* pRet = pfnFail;
	if (m_hThemeDll != NULL)
		pRet = GetProcAddress(m_hThemeDll, szProc);
	return pRet;
}

HTHEME CVisualStylesXP::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	static PFNOPENTHEMEDATA pfnOpenThemeData = NULL;
	if( !pfnOpenThemeData )
		pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProc("OpenThemeData", (void*)OpenThemeDataFail);
	return (*pfnOpenThemeData)(hwnd, pszClassList);
}

HRESULT CVisualStylesXP::CloseThemeData(HTHEME hTheme)
{
	static PFNCLOSETHEMEDATA pfnCloseThemeData = NULL;
	if( !pfnCloseThemeData )
		pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProc("CloseThemeData", (void*)CloseThemeDataFail);
	return (*pfnCloseThemeData)(hTheme);
}

HRESULT CVisualStylesXP::DrawThemeBackground(HTHEME hTheme, HDC hdc, 
											 int iPartId, int iStateId, const RECT *pRect, const RECT *pClipRect)
{
	static PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground = NULL;
	if( !pfnDrawThemeBackground )
		pfnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProc("DrawThemeBackground", (void*)DrawThemeBackgroundFail);
	return (*pfnDrawThemeBackground)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}


HRESULT CVisualStylesXP::DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, 
									   int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
									   DWORD dwTextFlags2, const RECT *pRect)
{
	static PFNDRAWTHEMETEXT pfn = NULL;
	if( !pfn )
		pfn = (PFNDRAWTHEMETEXT)GetProc("DrawThemeText", (void*)DrawThemeTextFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
}
HRESULT CVisualStylesXP::GetThemeBackgroundContentRect(HTHEME hTheme,  HDC hdc, 
													   int iPartId, int iStateId,  const RECT *pBoundingRect, 
													   RECT *pContentRect)
{
	static PFNGETTHEMEBACKGROUNDCONTENTRECT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProc("GetThemeBackgroundContentRect", (void*)GetThemeBackgroundContentRectFail);
	return (*pfn)(hTheme,  hdc, iPartId, iStateId,  pBoundingRect, pContentRect);
}
HRESULT CVisualStylesXP::GetThemeBackgroundExtent(HTHEME hTheme,  HDC hdc,
												  int iPartId, int iStateId, const RECT *pContentRect, 
												  RECT *pExtentRect)
{
	static PFNGETTHEMEBACKGROUNDEXTENT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEBACKGROUNDEXTENT)GetProc("GetThemeBackgroundExtent", (void*)GetThemeBackgroundExtentFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect);
}
HRESULT CVisualStylesXP::GetThemePartSize(HTHEME hTheme, HDC hdc, 
										  int iPartId, int iStateId, RECT * pRect, enum THEMESIZE eSize, SIZE *psz)
{
	static PFNGETTHEMEPARTSIZE pfnGetThemePartSize = NULL;
	if( !pfnGetThemePartSize )
		pfnGetThemePartSize = (PFNGETTHEMEPARTSIZE)GetProc("GetThemePartSize", (void*)GetThemePartSizeFail);
	return (*pfnGetThemePartSize)(hTheme, hdc, iPartId, iStateId, pRect, eSize, psz);
}

HRESULT CVisualStylesXP::GetThemeTextExtent(HTHEME hTheme, HDC hdc, 
											int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
											DWORD dwTextFlags,  const RECT *pBoundingRect, 
											RECT *pExtentRect)
{
	static PFNGETTHEMETEXTEXTENT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMETEXTEXTENT)GetProc("GetThemeTextExtent", (void*)GetThemeTextExtentFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags,  pBoundingRect, pExtentRect);
}

HRESULT CVisualStylesXP::GetThemeTextMetrics(HTHEME hTheme,  HDC hdc, 
											 int iPartId, int iStateId,  TEXTMETRIC* ptm)
{
	static PFNGETTHEMETEXTMETRICS pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMETEXTMETRICS)GetProc("GetThemeTextMetrics", (void*)GetThemeTextMetricsFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId,  ptm);
}

HRESULT CVisualStylesXP::GetThemeBackgroundRegion(HTHEME hTheme,  HDC hdc,  
												  int iPartId, int iStateId, const RECT *pRect,  HRGN *pRegion)
{
	static PFNGETTHEMEBACKGROUNDREGION pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEBACKGROUNDREGION)GetProc("GetThemeBackgroundRegion", (void*)GetThemeBackgroundRegionFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pRect, pRegion);
}

HRESULT CVisualStylesXP::HitTestThemeBackground(HTHEME hTheme,  HDC hdc, int iPartId, 
												int iStateId, DWORD dwOptions, const RECT *pRect,  HRGN hrgn, 
												POINT ptTest,  WORD *pwHitTestCode)
{
	static PFNHITTESTTHEMEBACKGROUND pfn = NULL;
	if( !pfn )
		pfn = (PFNHITTESTTHEMEBACKGROUND)GetProc("HitTestThemeBackground", (void*)HitTestThemeBackgroundFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode);
}

HRESULT CVisualStylesXP::DrawThemeEdge(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
									   const RECT *pDestRect, UINT uEdge, UINT uFlags,   RECT *pContentRect)
{
	static PFNDRAWTHEMEEDGE pfn = NULL;
	if( !pfn )
		pfn = (PFNDRAWTHEMEEDGE)GetProc("DrawThemeEdge", (void*)DrawThemeEdgeFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect);
}

HRESULT CVisualStylesXP::DrawThemeIcon(HTHEME hTheme, HDC hdc, int iPartId, 
									   int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex)
{
	static PFNDRAWTHEMEICON pfn = NULL;
	if( !pfn )
		pfn = (PFNDRAWTHEMEICON)GetProc("DrawThemeIcon", (void*)DrawThemeIconFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex);
}

BOOL CVisualStylesXP::IsThemePartDefined(HTHEME hTheme, int iPartId, 
										 int iStateId)
{
	static PFNISTHEMEPARTDEFINED pfn = NULL;
	if( !pfn )
		pfn = (PFNISTHEMEPARTDEFINED)GetProc("IsThemePartDefined", (void*)IsThemePartDefinedFail);
	return (*pfn)(hTheme, iPartId, iStateId);
}

BOOL CVisualStylesXP::IsThemeBackgroundPartiallyTransparent(HTHEME hTheme, 
															int iPartId, int iStateId)
{
	static PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT pfn = NULL;
	if( !pfn )
		pfn = (PFNISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)GetProc("IsThemeBackgroundPartiallyTransparent", (void*)IsThemeBackgroundPartiallyTransparentFail);
	return (*pfn)(hTheme, iPartId, iStateId);
}

HRESULT CVisualStylesXP::GetThemeColor(HTHEME hTheme, int iPartId, 
									   int iStateId, int iPropId,  COLORREF *pColor)
{
	static PFNGETTHEMECOLOR pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMECOLOR)GetProc("GetThemeColor", (void*)GetThemeColorFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pColor);
}

HRESULT CVisualStylesXP::GetThemeMetric(HTHEME hTheme,  HDC hdc, int iPartId, 
										int iStateId, int iPropId,  int *piVal)
{
	static PFNGETTHEMEMETRIC pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEMETRIC)GetProc("GetThemeMetric", (void*)GetThemeMetricFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, piVal);
}

HRESULT CVisualStylesXP::GetThemeString(HTHEME hTheme, 
										int iPartId, 
										int iStateId, 
										int iPropId,  
										LPWSTR pszBuff, 
										int cchMaxBuffChars)
{
	static PFNGETTHEMESTRING pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESTRING)GetProc("GetThemeString", (void*)GetThemeStringFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars);
}

HRESULT CVisualStylesXP::GetThemeBool(HTHEME hTheme, int iPartId, 
									  int iStateId, int iPropId,  BOOL *pfVal)
{
	static PFNGETTHEMEBOOL pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEBOOL)GetProc("GetThemeBool", (void*)GetThemeBoolFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pfVal);
}

HRESULT CVisualStylesXP::GetThemeInt(HTHEME hTheme, int iPartId, 
									 int iStateId, int iPropId,  int *piVal)
{
	static PFNGETTHEMEINT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEINT)GetProc("GetThemeInt", (void*)GetThemeIntFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, piVal);
}

HRESULT CVisualStylesXP::GetThemeEnumValue(HTHEME hTheme, int iPartId, 
										   int iStateId, int iPropId,  int *piVal)
{
	static PFNGETTHEMEENUMVALUE pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEENUMVALUE)GetProc("GetThemeEnumValue", (void*)GetThemeEnumValueFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, piVal);
}

HRESULT CVisualStylesXP::GetThemePosition(HTHEME hTheme, int iPartId, 
										  int iStateId, int iPropId,  POINT *pPoint)
{
	static PFNGETTHEMEPOSITION pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEPOSITION)GetProc("GetThemePosition", (void*)GetThemePositionFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pPoint);
}

HRESULT CVisualStylesXP::GetThemeFont(HTHEME hTheme,  HDC hdc, int iPartId, 
									  int iStateId, int iPropId,  LOGFONT *pFont)
{
	static PFNGETTHEMEFONT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEFONT)GetProc("GetThemeFont", (void*)GetThemeFontFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, pFont);
}

HRESULT CVisualStylesXP::GetThemeRect(HTHEME hTheme, int iPartId, 
									  int iStateId, int iPropId,  RECT *pRect)
{
	static PFNGETTHEMERECT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMERECT)GetProc("GetThemeRect", (void*)GetThemeRectFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pRect);
}

HRESULT CVisualStylesXP::GetThemeMargins(HTHEME hTheme,  HDC hdc, int iPartId, 
										 int iStateId, int iPropId,  RECT *prc,  MARGINS *pMargins)
{
	static PFNGETTHEMEMARGINS pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEMARGINS)GetProc("GetThemeMargins", (void*)GetThemeMarginsFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins);
}

HRESULT CVisualStylesXP::GetThemeIntList(HTHEME hTheme, int iPartId, 
										 int iStateId, int iPropId,  INTLIST *pIntList)
{
	static PFNGETTHEMEINTLIST pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEINTLIST)GetProc("GetThemeIntList", (void*)GetThemeIntListFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pIntList);
}

HRESULT CVisualStylesXP::GetThemePropertyOrigin(HTHEME hTheme, int iPartId, 
												int iStateId, int iPropId,  enum PROPERTYORIGIN *pOrigin)
{
	static PFNGETTHEMEPROPERTYORIGIN pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEPROPERTYORIGIN)GetProc("GetThemePropertyOrigin", (void*)GetThemePropertyOriginFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId, pOrigin);
}

HRESULT CVisualStylesXP::SetWindowTheme(HWND hwnd, LPCWSTR pszSubAppName, 
										LPCWSTR pszSubIdList)
{
	static PFNSETWINDOWTHEME pfn = NULL;
	if( !pfn )
		pfn = (PFNSETWINDOWTHEME)GetProc("SetWindowTheme", (void*)SetWindowThemeFail);
	return (*pfn)(hwnd, pszSubAppName, pszSubIdList);
}

HRESULT CVisualStylesXP::GetThemeFilename(HTHEME hTheme, int iPartId, 
										  int iStateId, int iPropId,  LPWSTR pszThemeFileName, int cchMaxBuffChars)
{
	static PFNGETTHEMEFILENAME pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEFILENAME)GetProc("GetThemeFilename", (void*)GetThemeFilenameFail);
	return (*pfn)(hTheme, iPartId, iStateId, iPropId,  pszThemeFileName, cchMaxBuffChars);
}

COLORREF CVisualStylesXP::GetThemeSysColor(HTHEME hTheme, int iColorId)
{
	static PFNGETTHEMESYSCOLOR pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSCOLOR)GetProc("GetThemeSysColor", (void*)GetThemeSysColorFail);
	return (*pfn)(hTheme, iColorId);
}

HBRUSH CVisualStylesXP::GetThemeSysColorBrush(HTHEME hTheme, int iColorId)
{
	static PFNGETTHEMESYSCOLORBRUSH pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSCOLORBRUSH)GetProc("GetThemeSysColorBrush", (void*)GetThemeSysColorBrushFail);
	return (*pfn)(hTheme, iColorId);
}

BOOL CVisualStylesXP::GetThemeSysBool(HTHEME hTheme, int iBoolId)
{
	static PFNGETTHEMESYSBOOL pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSBOOL)GetProc("GetThemeSysBool", (void*)GetThemeSysBoolFail);
	return (*pfn)(hTheme, iBoolId);
}

int CVisualStylesXP::GetThemeSysSize(HTHEME hTheme, int iSizeId)
{
	static PFNGETTHEMESYSSIZE pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSSIZE)GetProc("GetThemeSysSize", (void*)GetThemeSysSizeFail);
	return (*pfn)(hTheme, iSizeId);
}

HRESULT CVisualStylesXP::GetThemeSysFont(HTHEME hTheme, int iFontId,  LOGFONT *plf)
{
	static PFNGETTHEMESYSFONT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSFONT)GetProc("GetThemeSysFont", (void*)GetThemeSysFontFail);
	return (*pfn)(hTheme, iFontId, plf);
}

HRESULT CVisualStylesXP::GetThemeSysString(HTHEME hTheme, int iStringId, 
										   LPWSTR pszStringBuff, int cchMaxStringChars)
{
	static PFNGETTHEMESYSSTRING pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSSTRING)GetProc("GetThemeSysString", (void*)GetThemeSysStringFail);
	return (*pfn)(hTheme, iStringId, pszStringBuff, cchMaxStringChars);
}

HRESULT CVisualStylesXP::GetThemeSysInt(HTHEME hTheme, int iIntId, int *piValue)
{
	static PFNGETTHEMESYSINT pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMESYSINT)GetProc("GetThemeSysInt", (void*)GetThemeSysIntFail);
	return (*pfn)(hTheme, iIntId, piValue);
}

BOOL CVisualStylesXP::IsThemeActive()
{
	static PFNISTHEMEACTIVE pfn = NULL;
	if( !pfn )
		pfn = (PFNISTHEMEACTIVE)GetProc("IsThemeActive", (void*)IsThemeActiveFail);
	return (*pfn)();
}

BOOL CVisualStylesXP::IsAppThemed()
{
	static PFNISAPPTHEMED pfnIsAppThemed = NULL;
	if( !pfnIsAppThemed )
		pfnIsAppThemed = (PFNISAPPTHEMED)GetProc("IsAppThemed", (void*)IsAppThemedFail);
	return (*pfnIsAppThemed)();
}

HTHEME CVisualStylesXP::GetWindowTheme(HWND hwnd)
{
	static PFNGETWINDOWTHEME pfn = NULL;
	if( !pfn )
		pfn = (PFNGETWINDOWTHEME)GetProc("GetWindowTheme", (void*)GetWindowThemeFail);
	return (*pfn)(hwnd);
}

HRESULT CVisualStylesXP::EnableThemeDialogTexture(HWND hwnd, DWORD dwFlags)
{
	static PFNENABLETHEMEDIALOGTEXTURE pfn = NULL;
	if( !pfn )
		pfn = (PFNENABLETHEMEDIALOGTEXTURE)GetProc("EnableThemeDialogTexture", (void*)EnableThemeDialogTextureFail);
	return (*pfn)(hwnd, dwFlags);
}

BOOL CVisualStylesXP::IsThemeDialogTextureEnabled(HWND hwnd)
{
	static PFNISTHEMEDIALOGTEXTUREENABLED pfn = NULL;
	if( !pfn )
		pfn = (PFNISTHEMEDIALOGTEXTUREENABLED)GetProc("IsThemeDialogTextureEnabled", (void*)IsThemeDialogTextureEnabledFail);
	return (*pfn)(hwnd);
}

DWORD CVisualStylesXP::GetThemeAppProperties()
{
	static PFNGETTHEMEAPPPROPERTIES pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEAPPPROPERTIES)GetProc("GetThemeAppProperties", (void*)GetThemeAppPropertiesFail);
	return (*pfn)();
}

void CVisualStylesXP::SetThemeAppProperties(DWORD dwFlags)
{
	static PFNSETTHEMEAPPPROPERTIES pfn = NULL;
	if( !pfn )
		pfn = (PFNSETTHEMEAPPPROPERTIES)GetProc("SetThemeAppProperties", (void*)SetThemeAppPropertiesFail);
	(*pfn)(dwFlags);
}

HRESULT CVisualStylesXP::GetCurrentThemeName(
	LPWSTR pszThemeFileName, int cchMaxNameChars, 
	LPWSTR pszColorBuff, int cchMaxColorChars,
	LPWSTR pszSizeBuff, int cchMaxSizeChars)
{
	static PFNGETCURRENTTHEMENAME pfn = NULL;
	if( !pfn )
		pfn = (PFNGETCURRENTTHEMENAME)GetProc("GetCurrentThemeName", (void*)GetCurrentThemeNameFail);
	return (*pfn)(pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars);
}

HRESULT CVisualStylesXP::GetThemeDocumentationProperty(LPCWSTR pszThemeName,
													   LPCWSTR pszPropertyName,  LPWSTR pszValueBuff, int cchMaxValChars)
{
	static PFNGETTHEMEDOCUMENTATIONPROPERTY pfn = NULL;
	if( !pfn )
		pfn = (PFNGETTHEMEDOCUMENTATIONPROPERTY)GetProc("GetThemeDocumentationProperty", (void*)GetThemeDocumentationPropertyFail);
	return (*pfn)(pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars);
}


HRESULT CVisualStylesXP::DrawThemeParentBackground(HWND hwnd, HDC hdc,  RECT* prc)
{
	static PFNDRAWTHEMEPARENTBACKGROUND pfn = NULL;
	if( !pfn )
		pfn = (PFNDRAWTHEMEPARENTBACKGROUND)GetProc("DrawThemeParentBackground", (void*)DrawThemeParentBackgroundFail);
	return (*pfn)(hwnd, hdc, prc);
}

HRESULT CVisualStylesXP::EnableTheming(BOOL fEnable)
{
	static PFNENABLETHEMING pfn = NULL;
	if( !pfn )
		pfn = (PFNENABLETHEMING)GetProc("EnableTheming", (void*)EnableThemingFail);
	return (*pfn)(fEnable);
}


/////////////////////////////////////////////////////////////////////////////
// Added by Mathias Tunared 2002-07-27
//
BOOL CVisualStylesXP::UseVisualStyles()
{
	static BOOL bUse = -1;
	if( bUse != -1 )
		return bUse;

	HINSTANCE hinstDll;
	DLLVERSIONINFO dvi;
	ZeroMemory(&dvi, sizeof(dvi));

	hinstDll = LoadLibrary(TEXT("comctl32.dll"));
	if(hinstDll)
	{
		DLLGETVERSIONPROC pDllGetVersion;
		pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");
		if(pDllGetVersion)
		{
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);
			(*pDllGetVersion)(&dvi);
		}
		FreeLibrary(hinstDll);
	}

	if( dvi.dwMajorVersion == 6 && dvi.dwMinorVersion == 0 )
	{
		//+++
		WCHAR szName [256];
		WCHAR szColor [256];

		if (!GetCurrentThemeName(szName, 255, szColor, 255, NULL, 0))
		{
			CString strWinXPThemeColor = szColor;
			if (strWinXPThemeColor.CompareNoCase (_T("normalcolor")) == 0 ||
				strWinXPThemeColor.CompareNoCase (_T("homestead")) == 0 ||
				strWinXPThemeColor.CompareNoCase (_T("metallic")) == 0)
			{
				bUse = TRUE;
				return IsAppThemed() && m_hThemeDll; // No need to test HTHEME, but...
			}
		}
	}

	bUse = FALSE;
	return FALSE;
}

