// XTPSkinManagerSchema.h: interface for the CXTPSkinManagerSchema class.
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
#if !defined(_XTPSKINMANAGERSCHEMA_H__)
#define _XTPSKINMANAGERSCHEMA_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPSkinManagerResourceFile;
class CXTPSkinManagerClass;
class CXTPSkinManager;
class CXTPSkinObjectFrame;
class CXTPSkinImage;

struct XTP_SKINSCROLLBARPOSINFO;

#include "Common/XTPSystemHelpers.h"

//-----------------------------------------------------------------------
// Summary:
//     Available property for CXTPSkinManagerSchemaProperty class
// Example:
// <code>
// XTPSkinManager()->GetProperty(XTP_SKINPROPERTY_BOOL, iPartId, iStateId, TMT_TRANSPARENT);
// </code>
// See Also:
//     CXTPSkinManagerSchemaProperty
//-----------------------------------------------------------------------
enum XTPSkinManagerProperty
{
	XTP_SKINPROPERTY_UNKNOWN,       // Not defined
	XTP_SKINPROPERTY_STRING,        // CString property
	XTP_SKINPROPERTY_INT,           // int property
	XTP_SKINPROPERTY_BOOL,          // BOOL property
	XTP_SKINPROPERTY_COLOR,         // COLORREF property
	XTP_SKINPROPERTY_RECT,          // CRect property
	XTP_SKINPROPERTY_FONT,          // LOGFONT property
	XTP_SKINPROPERTY_INTLIST,       // CIntArray property
	XTP_SKINPROPERTY_ENUM,          // enum property
	XTP_SKINPROPERTY_POSITION,      // int property
	XTP_SKINPROPERTY_FILENAME = XTP_SKINPROPERTY_STRING,    // FileName property
	XTP_SKINPROPERTY_MARGINS = XTP_SKINPROPERTY_RECT,       // Margins property
	XTP_SKINPROPERTY_SIZE = XTP_SKINPROPERTY_INT            // Size property
};


class CXTPSkinManagerSchema;

//===========================================================================
// Summary:
//     CXTPSkinManagerSchemaProperty is standalone class represented single property of visual styles ini file
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchemaProperty
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchemaProperty object.
	//-------------------------------------------------------------------------
	CXTPSkinManagerSchemaProperty();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerSchemaProperty object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinManagerSchemaProperty();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to Color property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyColor(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set string property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyString(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to BOOL property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyBool(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to int property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyInt(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set enum property
	// Parameters:
	//     nEnumValue - Enumerator to set
	//-----------------------------------------------------------------------
	void SetPropertyEnum(int nEnumValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to size property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertySize(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to CRect property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyRect(LPCTSTR lpszValue);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to convert string to LOGFONT property
	// Parameters:
	//     lpszValue - String to convert property from
	//-----------------------------------------------------------------------
	void SetPropertyFont(LPCTSTR lpszValue);
	void ClearProperty();

	BOOL operator==(const CXTPSkinManagerSchemaProperty& obj) const;

public:
	XTPSkinManagerProperty propType;    // Property type
	int nPropertyCode;                  // Property code

	union
	{
		COLORREF clrVal;    // Color value of the property
		LPTSTR lpszVal;     // LPCTSTR value of the property
		BOOL bVal;          // BOOL value of the property
		int iVal;           // Integer value of the property
		RECT rcVal;         // Rect value of the property
		SIZE szVal;         // SIZE value of the property
		PLOGFONT lfVal;     // LOGFONT value
	};
};


//===========================================================================
// Summary:
//     CXTPSkinManagerSchema is standalone class used to read visual styles schema file
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchema
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchema object.
	// Parameters:
	//     pManager - Parent skin manager class.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchema(CXTPSkinManager* pManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerSchema object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerSchema();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to read all properties from resource file
	// Parameters:
	//     pResourceFile - Resource file to read
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT ReadProperties(CXTPSkinManagerResourceFile* pResourceFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns class hash code
	// Parameters:
	//     strClass - Class name
	// Returns:
	//     UINT class hash code
	//-----------------------------------------------------------------------
	static UINT AFX_CDECL GetClassCode(LPCTSTR strClass);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified string property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     strVal      - receives the string property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetStringProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CString& strVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified rect property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     rcVal       - receives the rect property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetRectProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CRect& rcVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified int property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     iVal        - receives the int property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetIntProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& iVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified BOOL property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     bVal        - receives the BOOL property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetBoolProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, BOOL& bVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified COLORREF property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     clrVal      - receives the COLORREF property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetColorProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, COLORREF& clrVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified enum property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     nVal        - receives the enum property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetEnumProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, int& nVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified LOGFONT property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     lfVal       - receives the LOGFONT property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetFontProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, LOGFONT& lfVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Get the value for the specified CSize property
	// Parameters:
	//     iClassId    - Class hash code
	//     iPartId     - Part number
	//     iStateId    - State number of part
	//     iPropId     - The property number to get the value for
	//     szVal       - Receives the CSize property value
	// Returns:
	//     S_OK if successful
	//-----------------------------------------------------------------------
	HRESULT GetSizeProperty(UINT iClassId, int iPartId, int iStateId, int iPropId, CSize& szVal);

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns schema name
	// Returns:
	//     Selected schema name
	//-----------------------------------------------------------------------
	CString GetSchemaName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent skin manager object
	// Returns:
	//     Pointer to CXTPSkinManager object
	//-----------------------------------------------------------------------
	CXTPSkinManager* GetSkinManager() const;


//{{AFX_CODEJOCK_PRIVATE
public:
	static UINT AFX_CDECL GetSchemaPartCode(CString strSchemaPart);
	static UINT AFX_CDECL GetClassPartCode(LPCTSTR lpszClass, LPCTSTR lpszClassPart);
	static UINT AFX_CDECL GetStateCode(LPCTSTR lpszClassPart, LPCTSTR lpszState);
	static int AFX_CDECL GetEnumCode(LPCTSTR lpszProperty, LPCTSTR lpszValue);
	static UINT AFX_CDECL GetPropertyCode(LPCTSTR lpszProperty, XTPSkinManagerProperty& nPropertyType);
	static CXTPSkinManagerSchemaProperty* AFX_CDECL CreateProperty(LPCTSTR lpszProperty, XTPSkinManagerProperty nPropertyType, LPCTSTR lpszValue);
	static UINT AFX_CDECL CalculatePropertyCode(UINT iClassId, int iPartId, int iStateId, int iPropId);
protected:
	void RemoveAllProperties();
	CXTPSkinManagerSchemaProperty* GetProperty(UINT iClassId, int iPartId, int iStateId, int iPropId);
//}}AFX_CODEJOCK_PRIVATE


public:

//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL DrawThemeBackground(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect) = 0;
	virtual void DrawThemeFrame(CDC* pDC, CXTPSkinObjectFrame* pFrame) = 0;
	virtual CRect CalcFrameBorders(CXTPSkinObjectFrame* pFrame) = 0;
	virtual HRGN CalcFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz) = 0;
	virtual void RefreshMetrcis() = 0;
	void DrawNonClientRect(CDC* pDC, CRect rcFrame, CXTPSkinObjectFrame* pFrame);
	void DrawClientEdge(CDC* pDC, const CRect& rcFrame, CXTPSkinObjectFrame* pFrame);
	virtual void DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame, XTP_SKINSCROLLBARPOSINFO* pSBInfo) = 0;
	virtual COLORREF GetScrollBarSizeBoxColor(CXTPSkinObjectFrame* pFrame);
//}}AFX_CODEJOCK_PRIVATE

protected:

//{{AFX_CODEJOCK_PRIVATE
	BOOL DrawThemeBackgroundGlyph(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect);
	BOOL DrawThemeBackgroundBorder(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect);
	int FindBestImageGlyphSize(CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const CRect& rcDest, int nImageCount, BOOL bHorizontalImageLayout);
//}}AFX_CODEJOCK_PRIVATE

public:
	BOOL m_bPreMultiplyImages;
protected:
	CMap<UINT, UINT, CXTPSkinManagerSchemaProperty*, CXTPSkinManagerSchemaProperty*> m_mapProperties;   // Properties collection
	CXTPSkinManager* m_pManager;        // Parent CXTPSkinManager class
	CString m_strSchemaName;            // Schema name
	CXTPCriticalSection m_csDraw;

protected:
	int m_nGlobalClassId;
	int m_nWindowClassId;

	friend class CXTPSkinManagerClass;
};


//===========================================================================
// Summary:
//     CXTPSkinManagerSchemaDefault is CXTPSkinManagerSchema derived class is used to
//     implement standard visual style.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchemaDefault : public CXTPSkinManagerSchema
{

	struct FRAMEREGION
	{
		int nCaptionHeight;
		BOOL bToolWindow;

		CRgn rgnFrameLeft;
		CRgn rgnFrameRight;
		CRect rcFrameSizingMargins;
		int nRgnFrameCenter;
		BOOL bAutoDelete;
	};

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchemaDefault object.
	// Parameters:
	//     pManager - Parent skin manager class.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchemaDefault(CXTPSkinManager* pManager);

	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerSchemaDefault object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPSkinManagerSchemaDefault();

protected:

//{{AFX_CODEJOCK_PRIVATE
	virtual BOOL DrawThemeBackground(CDC* pDC, CXTPSkinManagerClass* pClass, int iPartId, int iStateId, const RECT *pRect);
	virtual void DrawThemeFrame(CDC* pDC, CXTPSkinObjectFrame* pFrame);
	virtual int DrawThemeFrameButtons(CDC* pDC, CXTPSkinObjectFrame* pFrame);
	virtual CRect CalcFrameBorders(CXTPSkinObjectFrame* pFrame);
	virtual HRGN CalcFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz);
	virtual void RefreshMetrcis();
	virtual void DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame, XTP_SKINSCROLLBARPOSINFO* pSBInfo);
//}}AFX_CODEJOCK_PRIVATE

private:
	int GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient);
	void FreeFrameRegions();

	FRAMEREGION* CreateFrameRegion(CXTPSkinObjectFrame* pFrame, CSize sz);
	CSize GetCaptionButtonSize(CXTPSkinManagerClass* pClass, int yButton);


protected:
	int m_nFrameButtonOffset;           // Frame Button offset
	CArray<FRAMEREGION*, FRAMEREGION*> m_arrFrameRegions;   // Frame region
};


//===========================================================================
// Summary:
//     CXTPSkinManagerSchemaOffice2007 is CXTPSkinManagerSchema derived class is used
//     to implement Office2007 visual style.
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerSchemaOffice2007 : public CXTPSkinManagerSchemaDefault
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerSchemaOffice2007 object.
	// Parameters:
	//     pManager - Parent skin manager class.
	//-----------------------------------------------------------------------
	CXTPSkinManagerSchemaOffice2007(CXTPSkinManager* pManager);

protected:

//{{AFX_CODEJOCK_PRIVATE
	//virtual void DrawThemeFrame(CDC* pDC, CXTPSkinObjectFrame* pFrame);
	virtual void DrawThemeScrollBar(CDC* pDC, CXTPSkinObjectFrame* pFrame, XTP_SKINSCROLLBARPOSINFO* pSBInfo);
	virtual COLORREF GetScrollBarSizeBoxColor(CXTPSkinObjectFrame* pFrame);
//}}AFX_CODEJOCK_PRIVATE

private:
	void DrawThumbButton(CDC* pDC, CRect rcThumb, int nState, BOOL bHorz);
};

AFX_INLINE CString CXTPSkinManagerSchema::GetSchemaName() const {
	return m_strSchemaName;
}
AFX_INLINE CXTPSkinManager* CXTPSkinManagerSchema::GetSkinManager() const {
	return m_pManager;
}


#endif // !defined(AFX_SKINMANAGERSCHEMA_H__44A8B96F_B15B_43F7_A734_39C649784B29__INCLUDED_)
