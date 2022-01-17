// XTPCalendarCustomProperties.h: interface for the CXTPCalendarCustomProperties class.
//
// This file is a part of the XTREME CALENDAR MFC class library.
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
#if !defined(_XTP_CALENDAR_CUSTOM_PROPERTIES_H__)
#define _XTP_CALENDAR_CUSTOM_PROPERTIES_H__
//}}AFX_CODEJOCK_PRIVATE

//#include "Common/XTPPropExchange.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////
class CXTPPropExchange;

//===========================================================================
// Summary:
//      This class is used to store properties collection.
// Remarks:
//      It contains methods to Get, Set, Remove and Iterate on properties.
// See Also:
//      CXTPCalendarEvent::GetCustomProperties().
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarCustomProperties : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	DECLARE_DYNAMIC(CXTPCalendarCustomProperties)
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	// Parameters:
	//      bNameIgnoreCase - [in] if TRUE - properties names are not
	//                        case sensitive.
	//-----------------------------------------------------------------------
	CXTPCalendarCustomProperties(BOOL bNameIgnoreCase = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarCustomProperties();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to get property value using the specified
	//      name.
	// Parameters:
	//      pcszName    - [in] A pointer to property name string.
	//      rVarValue   - [out] A reference to COleVariant object to receive
	//                    property value.
	// Remarks:
	//      The corresponding method for ActiveX returns VT_EMPTY variant value
	//      instead of FALSE.
	// Returns:
	//      TRUE if successful, FALSE if specified property is not present
	//      in the collection.
	// See Also:
	//      SetProperty, RemoveProperty.
	//-----------------------------------------------------------------------
	virtual BOOL GetProperty(LPCTSTR pcszName, COleVariant& rVarValue) const;

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to set property value with the specified
	//      name.
	// Parameters:
	//      pcszName    - [in] A pointer to property name string.
	//      varValue    - [in] A const reference to COleVariant object with a
	//                    property value.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also:
	//      GetProperty, RemoveProperty.
	//-----------------------------------------------------------------------
	virtual BOOL SetProperty(LPCTSTR pcszName, const COleVariant& varValue);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to remove property with the specified
	//      name.
	// Parameters:
	//      pcszName    - [in] A pointer to property name string.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also:
	//      GetProperty, SetProperty.
	//-----------------------------------------------------------------------
	virtual BOOL RemoveProperty(LPCTSTR pcszName);

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to remove all properties.
	// See Also:
	//      RemoveProperty, GetProperty, SetProperty.
	//-----------------------------------------------------------------------
	virtual void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is to get properties count in the collection.
	// Returns:
	//      Properties amount in the collection.
	// See Also:
	//      GetProperty, SetProperty, RemoveProperty, RemoveAll.
	//-----------------------------------------------------------------------
	virtual int GetCount();

	//-----------------------------------------------------------------------
	// Summary:
	//      Starts a properties collection iteration by returning a POSITION
	//      value that can be passed to a GetNextProperty call.
	// Returns:
	//      A POSITION value with start position.
	// See Also:
	//      GetNextProperty, CMap.
	//-----------------------------------------------------------------------
	POSITION GetStartPosition() const;

	//-----------------------------------------------------------------------
	// Summary:
	//      Retrieves the properties collection element at rPos, then updates
	//      rPos to refer to the next element in the collection.
	// Parameters:
	//      rPos        - [in out] Specifies a reference to a POSITION value
	//                    returned by a previous GetNextProperty or
	//                    GetStartPosition call.
	//      rStrName    - [out] A reference to CString object to receive
	//                    property name value.
	//      rVarValue   - [out] A reference to COleVariant object to receive
	//                    property value.
	// See Also:
	//      GetStartPosition, CMap.
	//-----------------------------------------------------------------------
	void GetNextProperty(POSITION& rPos, CString& rStrName, COleVariant& rVarValue) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store/Load a properties collection
	//     using the specified data object.
	// Parameters:
	//     pPX - Source or destination CXTPPropExchange data object reference.
	// Remarks:
	//     This member function is used to store or load properties collection
	//     data to or form an storage.
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//      This helper function is used to load properties collection from XML
	//      string source.
	// Parameters:
	//      pcszXMLData - [in] A pointer to XML string data source.
	// Remarks:
	//      RemoveAll method is called for collection before loading.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also:
	//      SaveToXML.
	//-----------------------------------------------------------------------
	BOOL LoadFromXML(LPCTSTR pcszXMLData);

	//-----------------------------------------------------------------------
	// Summary:
	//      This helper function is used to store properties collection to XML
	//      string.
	// Parameters:
	//      rstrXMLData - [out] A XML string object reference.
	// Returns:
	//      TRUE if successful, FALSE otherwise.
	// See Also:
	//      LoadFromXML.
	//-----------------------------------------------------------------------
	BOOL SaveToXML(CString& rstrXMLData);

	//-----------------------------------------------------------------------
	// Summary:
	//      This helper function is used to copy properties from source collection
	//      object to destination collection.
	// Parameters:
	//      pDest   - [out] A pointer to destination properties collection object.
	// Remarks:
	//      RemoveAll method is called for destination collection before copying.
	// See Also:
	//      LoadFromXML, SaveToXML.
	//-----------------------------------------------------------------------
	void CopyTo(CXTPCalendarCustomProperties* pDest);



protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Load a properties collection
	//     using the specified data object.
	// Parameters:
	//     pPX - Source CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	void _Load(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to Store a properties collection
	//     using the specified data object.
	// Parameters:
	//     pPX - Destination CXTPPropExchange data object reference.
	//-----------------------------------------------------------------------
	void _Save(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//      Call this member function to prepare property name before pass it
	//      in the map access functions.
	// Parameters:
	//      pcszName    - [in] A pointer to property name string.
	// Remarks:
	//      If m_bNameIgnoreCase is TRUE, properties names are converted to
	//      lower case.
	// Returns:
	//      Prepared property name string to pass it in the map access functions.
	//-----------------------------------------------------------------------
	CString PrepareName(LPCTSTR pcszName) const;

	//{{AFX_CODEJOCK_PRIVATE
	typedef CMap<CString, LPCTSTR, COleVariant, COleVariant&> CXTPOleVariantMap;
	//}}AFX_CODEJOCK_PRIVATE

	CXTPOleVariantMap   m_mapProperties;    // Properties storage map.
	BOOL                m_bNameIgnoreCase;  // if TRUE - properties names are not case sensitive.

};



#endif // !defined(_XTP_CALENDAR_CUSTOM_PROPERTIES_H__)
