// XTPCalendarViewPart.h: interface for the CXTPCalendarViewPart class.
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
#if !defined(_XTPCALENDARVIEWPART_H__)
#define _XTPCALENDARVIEWPART_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/XTPColorManager.h"
#include "Common/XTPPropExchange.h"

#pragma warning(disable: 4097)

class CXTPCalendarView;
class CXTPPropExchange;

//===========================================================================
// Summary:
//     Helper class provides functionality to manage font settings for
//     various graphical elements of control.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarViewPartFontValue
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarViewPartFontValue()
	{

	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the default font
	//     value is set.
	// Returns:
	//     A BOOL. TRUE if the default value is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsDefaultValue() {
		return (m_fntCustomValue.GetSafeHandle() == NULL) && (m_fntStandardValue.GetSafeHandle() == NULL);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the value for the standard
	//     font.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT structure that contains the
	//                standard font value.
	// Remarks:
	//     Call this member function to set the standard font. This font
	//     is used as the default font if there is not a custom font value
	//     set.
	//-----------------------------------------------------------------------
	void SetStandardValue(LOGFONT* pLogFont)
	{
		m_fntStandardValue.DeleteObject();
		m_fntStandardValue.CreateFontIndirect(pLogFont);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to delete the custom font and to
	//     set the default font value as the default font.
	// Remarks:
	//     Call this member function to ensure that the default font is
	//     used and not the custom font.
	//-----------------------------------------------------------------------
	void SetDefaultValue() {
		m_fntCustomValue.DeleteObject();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to overload the function call operator
	//     for the CXTPCalendarViewPartFontValue class.
	// Remarks:
	//     Use the default font if the custom font does not work. Otherwise,
	//     use a custom font.
	// Returns:
	//     A reference to a CFont object that contains either the standard
	//     font value or the custom font value.
	//-----------------------------------------------------------------------
	operator CFont& () {
		return  (m_fntCustomValue.GetSafeHandle() == 0) ? m_fntStandardValue : m_fntCustomValue;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to overload the function call operator
	//     for the CXTPCalendarViewPartFontValue class.
	// Remarks:
	//     Use the default font if the custom font does not work. Otherwise,
	//     use a custom font.
	// Returns:
	//     A pointer to a CFont object that contains either the standard
	//     font value or the custom font value.
	//-----------------------------------------------------------------------
	operator CFont* () {
		return  (m_fntCustomValue.GetSafeHandle() == 0) ? &m_fntStandardValue : &m_fntCustomValue;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function overloads the assignment operator for the
	//     CXTPCalendarViewPartFontValue class.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT structure.
	// Remarks:
	//     Creates a new custom font.
	// Returns:
	//     A reference to a CXTPCalendarViewPartFontValue.
	//-----------------------------------------------------------------------
	const CXTPCalendarViewPartFontValue& operator=(LOGFONT* pLogFont) {
		m_fntCustomValue.DeleteObject();
		if (pLogFont) m_fntCustomValue.CreateFontIndirect(pLogFont);
		return *this;
	}
	//-----------------------------------------------------------------------
	// Summary:
	//     Get a current value object.
	// Returns:
	//     Pointer to the current value object.
	//-----------------------------------------------------------------------
	CFont* operator->() {
		return (m_fntCustomValue.GetSafeHandle() == 0) ? &m_fntStandardValue : &m_fntCustomValue;
	};

protected:
	CFont m_fntStandardValue;   // Stores default font.
	CFont m_fntCustomValue;     // Stores custom font.

};

//===========================================================================
// Summary:
//      Helper class template provides functionality to manage customized
//      value for the specified type.
//===========================================================================
template<class _TValue, class _TValueRef = _TValue&>
class CXTPCalendarThemeCustomizableXValueT
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeCustomizableXValueT()
	{
		m_bIsStandardSet = FALSE;
		m_bIsCustomSet = FALSE;

		m_bAutoDestroy_Standard = FALSE;
		m_bAutoDestroy_Custom = FALSE;

		m_ValueStandard = _TValue();
		m_ValueCustom = _TValue();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarThemeCustomizableXValueT()
	{
		if (m_bAutoDestroy_Standard) {
			DoDestroy(m_ValueStandard);
		}
		if (m_bAutoDestroy_Custom) {
			DoDestroy(m_ValueCustom);
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to destroy objects of type _TValue if
	//     necessary (Close handles, free memory, ...).
	// Parameters:
	//      refValue - An object reference to destroy.
	//-----------------------------------------------------------------------
	virtual void DoDestroy(_TValue& refValue){UNREFERENCED_PARAMETER(refValue);};

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the default value is set.
	// Returns:
	//     A BOOL. TRUE if the default value is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsDefaultValue() const {
		return !m_bIsCustomSet && !m_bIsStandardSet;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the standard value is set and
	//     used.
	// Returns:
	//     TRUE if standard value is set and custom value is not set,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsStandardValue() const {
		return !m_bIsCustomSet && m_bIsStandardSet;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the custom value is set and
	//     used.
	// Returns:
	//     TRUE if custom value is set, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomValue() const {
		return m_bIsCustomSet;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to set the standard value.
	// Parameters:
	//      refValue    - A standard value.
	//      bAutoDestroy - This parameter indicates should be a value object
	//                    destroyed when destroy or other value is set.
	//                    If TRUE - DoDestroy will be called.
	// Remarks:
	//     Call this member function to set the standard value. This value
	//     is used as the default value if there is not a custom value set.
	// See Also:
	//      SetCustomValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual void SetStandardValue(_TValueRef refValue, BOOL bAutoDestroy)
	{
		if (m_bIsStandardSet && m_bAutoDestroy_Standard) {
			DoDestroy(m_ValueStandard);
		}
		m_ValueStandard = refValue;

		m_bAutoDestroy_Standard = bAutoDestroy;
		m_bIsStandardSet = TRUE;
	}

	//<COMBINE SetStandardValue@_TValueRef@BOOL>
	virtual void SetStandardValue(_TValueRef refValue)
	{
		SetStandardValue(refValue, m_bAutoDestroy_Standard);
	}


	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to set the custom value.
	// Parameters:
	//      refValue    - A custom value.
	//      bAutoDestroy - This parameter indicates should be a value object
	//                    destroyed when destroy or other value is set.
	//                    If TRUE - DoDestroy will be called.
	// Remarks:
	//     Call this member function to set the custom value. If set, this value
	//     is used instead of default value.
	// See Also:
	//      SetStandardValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual void SetCustomValue(_TValueRef refValue, BOOL bAutoDestroy)
	{
		if (m_bIsCustomSet && m_bAutoDestroy_Custom) {
			DoDestroy(m_ValueCustom);
		}
		m_ValueCustom = refValue;

		m_bAutoDestroy_Custom = bAutoDestroy;
		m_bIsCustomSet = TRUE;
	}

	//<COMBINE SetCustomValue@_TValueRef@BOOL>
	virtual void SetCustomValue(_TValueRef refValue)
	{
		SetCustomValue(refValue, m_bAutoDestroy_Custom);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to reset the custom value.
	// Remarks:
	//      Call this member function to ensure that the default value is
	//      used and not the custom value.
	//      If the default value is not set - method do nothing.
	// See Also:
	//      SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	virtual void SetDefaultValue()
	{
		if (m_bIsStandardSet)
		{
			if (m_bIsCustomSet && m_bAutoDestroy_Custom) {
				DoDestroy(m_ValueCustom);
			}
			m_bIsCustomSet = FALSE;
			m_bAutoDestroy_Custom = FALSE;
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to get current value.
	// Remarks:
	//     The default value is used if the custom value is not set. Otherwise,
	//     a custom value is used.
	// Returns:
	//     Current value.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual _TValueRef GetValue() const {
		return  m_bIsCustomSet ? m_ValueCustom : m_ValueStandard;
	}

	virtual const _TValue& GetValueX() const {
		return  m_bIsCustomSet ? m_ValueCustom : m_ValueStandard;
	}


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to get standard value.
	// Returns:
	//     Standard value.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual _TValueRef GetStandardValue() const {
		return  m_ValueStandard;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to get current value.
	// Remarks:
	//     The default value is used if the custom value is not set. Otherwise,
	//     a custom value is used.
	// Returns:
	//     Current value.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue
	//-----------------------------------------------------------------------
	operator _TValueRef () const {
		return GetValue();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to set standard value using current value
	//     from the specified object.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue,
	//      IsStandardValue, IsCustomValue, GetStandardValue.
	//-----------------------------------------------------------------------
	void CopySettings(const CXTPCalendarThemeCustomizableXValueT<_TValue, _TValueRef>& refSrc)
	{
		SetStandardValue(refSrc.GetValue());
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to set custom value.
	// Parameters:
	//      refValue    - A new custom value.
	// Returns:
	//      Reference to this object.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeCustomizableXValueT<_TValue, _TValueRef>& operator=(_TValueRef refValue) {
		SetCustomValue(refValue);
		return *this;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar)
	{
		// for simple types which can be easy serialized.
		// override for more complex cases

		if (ar.IsStoring())
		{
			ar << m_bIsStandardSet;
			ar << m_bIsCustomSet;

			ar << m_ValueStandard;
			ar << m_ValueCustom;
		}
		else
		{
			ASSERT(ar.IsLoading());

			ar >> m_bIsStandardSet;
			ar >> m_bIsCustomSet;

			ar >> m_ValueStandard;
			ar >> m_ValueCustom;

		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an storage.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	// See Also: Serialize(), DoPX_Value()
	//-----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX, LPCTSTR pcszPropName)
	{
		if (!pPX || !pcszPropName)
		{
			ASSERT(FALSE);
			return;
		}

		CXTPPropExchangeSection secData(pPX->GetSection(pcszPropName));
		if (pPX->IsStoring())
			secData->EmptySection();

		PX_Bool(&secData, _T("IsStandardSet"), m_bIsStandardSet);
		PX_Bool(&secData, _T("IsCustomSet"), m_bIsCustomSet);

		DoPX_Value(&secData, _T("Standard"), m_ValueStandard, TRUE);
		DoPX_Value(&secData, _T("Custom"), m_ValueCustom, FALSE);

	}

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes value data from or to an storage.
	// Parameters:
	//     pPX          - A CXTPPropExchange object to serialize to or from.
	//     pcszPropName - A value name.
	//     rXValue      - Reference to value.
	//     bStandard    - Standard or Custom value.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void DoPX_Value(CXTPPropExchange* pPX, LPCTSTR pcszPropName, _TValue& rXValue, BOOL bStandard)
	{
		ASSERT(FALSE);
		UNREFERENCED_PARAMETER(pPX); UNREFERENCED_PARAMETER(pcszPropName); UNREFERENCED_PARAMETER(rXValue);
		UNREFERENCED_PARAMETER(bStandard);
	}

protected:
	_TValue m_ValueStandard;            // Stores default value.
	BOOL    m_bAutoDestroy_Standard;    // Call DoDestroy for standard vale.
	BOOL    m_bIsStandardSet;           // Is standard value set.

	_TValue m_ValueCustom;              // Stores custom value.
	BOOL    m_bAutoDestroy_Custom;      // Call DoDestroy for custom vale.
	BOOL    m_bIsCustomSet;             // Is custom value set.
};

//===========================================================================
// Summary:
//      Helper class template provides functionality to manage customized
//      value for the specified class objects.
//===========================================================================
template<class _TValue>
class CXTPCalendarViewPartCustomizableValueT
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarViewPartCustomizableValueT()
	{
		m_pValueStandard = NULL;
		m_bAutoDelete_Standard = FALSE;

		m_pValueCustom = NULL;
		m_bAutoDelete_Custom = FALSE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewPartCustomizableValueT()
	{
		if (m_bAutoDelete_Standard) {
			SAFE_DELETE(m_pValueStandard);
		}
		if (m_bAutoDelete_Custom) {
			SAFE_DELETE(m_pValueCustom);
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to determine if the default value is set.
	// Returns:
	//     A BOOL. TRUE if the default value is set. FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsDefaultValue() const {
		return (m_pValueCustom == NULL) && (m_pValueStandard == NULL);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the standard value is set and
	//     used.
	// Returns:
	//     TRUE if standard value is set and custom value is not set,
	//     otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsStandardValue() const {
		return (m_pValueCustom == NULL) && (m_pValueStandard != NULL);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function determines if the custom value is set and
	//     used.
	// Returns:
	//     TRUE if custom value is set, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsCustomValue() const {
		return m_pValueCustom != NULL;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to set the standard value.
	// Parameters:
	//      pValue      - A pointer to a value object.
	//      bAutoDelete - This parameter indicates should be a value object
	//                    deleted when destroy or other value is set.
	//                    If TRUE - value object will be deleted automatically.
	// Remarks:
	//     Call this member function to set the standard value. This value
	//     is used as the default value if there is not a custom value set.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetCustomValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual void SetStandardValue(_TValue* pValue, BOOL bAutoDelete)
	{
		if (m_bAutoDelete_Standard) {
			SAFE_DELETE(m_pValueStandard);
		}
		m_pValueStandard = pValue;
		m_bAutoDelete_Standard = bAutoDelete;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to set the custom value.
	// Parameters:
	//      pValue      - A pointer to a value object.
	//      bAutoDelete - This parameter indicates should be a value object
	//                    deleted when destroy or other value is set.
	//                    If TRUE - value object will be deleted automatically.
	// Remarks:
	//     Call this member function to set the custom value. If set, this value
	//     is used instead of default value.
	// Example:
	// <code>
	//      class CMyClass
	//      {
	//      public:
	//          CMyClass();
	//
	//          void Draw(CDC* pDC, CRect& rcRect, BOOL bCustom);
	//          // ...
	//          CBrush m_brushBusy_Custom;
	//
	//          CXTPCalendarViewPartBrushValue m_brushVal_auto;
	//          CXTPCalendarViewPartBrushValue m_brushVal_static;
	//      };
	//
	//      CMyClass::CMyClass()
	//      {
	//          m_brushBusy_Custom.CreateSolidBrush(RGB(0, 0, 0xFF));
	//
	//          m_brushVal_auto.SetStandardValue(new CBrush(RGB(0xFF, 0xFF, 0xFF)), TRUE);
	//          m_brushVal_static.SetStandardValue(&m_brushBusy_Custom, FALSE);
	//      }
	//
	//      void CMyClass::Draw(CDC* pDC, CRect& rcRect, BOOL bCustom)
	//      {
	//          if (bCustom) {
	//              m_brushVal_auto.SetCustomValue(new CBrush(RGB(0, 0, 0)), TRUE);
	//              m_brushVal_static.SetCustomValue(new CBrush(RGB(255, 255, 255)), TRUE);
	//          }
	//          else {
	//              // Reset to standard value
	//              m_brushVal_auto.SetDefaultValue();
	//              m_brushVal_static.SetDefaultValue();
	//          }
	//
	//          pDC->FillRect(&rcRect, (CBrush*)m_brushVal_auto);
	//
	//          CBrush* pBrushOld = pDC->SelectObject(m_brushVal_static.GetValue());
	//          // ....
	//          pDC->SelectObject(pBrushOld);
	//      }
	//
	// </code>
	// See Also:
	//      SetStandardValue, SetDefaultValue
	//-----------------------------------------------------------------------
	virtual void SetCustomValue(_TValue* pValue, BOOL bAutoDelete)
	{
		if (m_bAutoDelete_Custom) {
			SAFE_DELETE(m_pValueCustom);
		}
		m_pValueCustom = pValue;
		m_bAutoDelete_Custom = bAutoDelete;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//      This member function is used to reset the custom value.
	// Remarks:
	//      Call this member function to ensure that the default value is
	//      used and not the custom value.
	//      If the default value is not set - method do nothing.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetStandardValue, SetCustomValue
	//-----------------------------------------------------------------------
	virtual void SetDefaultValue()
	{
		if (m_pValueStandard)
		{
			if (m_bAutoDelete_Custom) {
				SAFE_DELETE(m_pValueCustom);
			}
			m_pValueCustom = NULL;
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to get current value.
	// Remarks:
	//     The default value is used if the custom value is not set. Otherwise,
	//     a custom value is used.
	// Returns:
	//     A pointer to the current value.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, operator _TValue*
	//-----------------------------------------------------------------------
	virtual _TValue* GetValue() const {
		return  m_pValueCustom ? m_pValueCustom : m_pValueStandard;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to get standard value.
	// Returns:
	//     A pointer to the standard value.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, operator _TValue*
	//-----------------------------------------------------------------------
	virtual _TValue* GetStandardValue() const {
		return  m_pValueStandard;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to get current value.
	// Remarks:
	//     The default value is used if the custom value is not set. Otherwise,
	//     a custom value is used.
	// Returns:
	//     A pointer to the current value.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue
	//-----------------------------------------------------------------------
	operator _TValue* () const {
		return GetValue();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Get a current value object.
	// Returns:
	//     Pointer to the current value object.
	//-----------------------------------------------------------------------
	_TValue* operator->() {
		return GetValue();
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to set standard value using current value
	//     from the specified object.
	// Parameters:
	//     refSrc - Reference to a source object.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue,
	//      IsStandardValue, IsCustomValue, GetStandardValue.
	//-----------------------------------------------------------------------
	void CopySettings(const CXTPCalendarViewPartCustomizableValueT<_TValue>& refSrc)
	{
		SetStandardValue(refSrc.GetValue(), FALSE);
	}

protected:
	_TValue* m_pValueStandard;      // Stores default value.
	BOOL    m_bAutoDelete_Standard; // Call operator delete for the standard value.

	_TValue* m_pValueCustom;        // Stores custom value.
	BOOL    m_bAutoDelete_Custom;   // Call operator delete for the custom value.
};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage brush value objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarViewPartBrushValue : public
			CXTPCalendarViewPartCustomizableValueT<CBrush>
{
};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage font value objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarThemeFontValue : public
			CXTPCalendarViewPartCustomizableValueT<CFont>
{
	//{{AFX_CODEJOCK_PRIVATE
	typedef CXTPCalendarViewPartCustomizableValueT<CFont> TBase;
	//}}AFX_CODEJOCK_PRIVATE
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeFontValue()
	{
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the value for the standard
	//     font.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT structure that contains the
	//                standard font value.
	// Remarks:
	//     Call this member function to set the standard font. This font
	//     is used as the default font if there is not a custom font value
	//     set.
	//-----------------------------------------------------------------------
	virtual void SetStandardValue(LOGFONT* pLogFont)
	{
		CFont* pFont = new CFont();
		if (pFont) {
			VERIFY(pFont->CreateFontIndirect(pLogFont));
		}
		TBase::SetStandardValue(pFont, TRUE);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the value for the custom
	//     font.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT structure that contains the
	//                custom font value.
	// Remarks:
	//     Call this member function to set the custom font.
	//     If set, this font is used as the object value.
	//-----------------------------------------------------------------------
	virtual void SetCustomValue(LOGFONT* pLogFont)
	{
		CFont* pFont = new CFont();
		if (pFont) {
			VERIFY(pFont->CreateFontIndirect(pLogFont));
		}
		TBase::SetCustomValue(pFont, TRUE);
	}

	//<COMBINE CXTPCalendarViewPartCustomizableValueT::SetStandardValue>
	virtual void SetStandardValue(CFont* pFont)
	{
		//may be not safe when few CXTPCalendarThemeFontValue objects use the same CFont* pointer.
		//
		//TBase::SetStandardValue(pFont, FALSE);

		if (!pFont)
		{
			TBase::SetStandardValue(pFont, FALSE);
			return;
		}

		LOGFONT lfFont;
		int nRes = pFont->GetLogFont(&lfFont);
		ASSERT(nRes);
		if(nRes)
			SetStandardValue(&lfFont);
	}

	// <COMBINE CXTPCalendarViewPartCustomizableValueT::SetCustomValue>
	virtual void SetCustomValue(CFont* pFont)
	{
		//may be not safe when few CXTPCalendarThemeFontValue objects use the same CFont* pointer.
		//
		//TBase::SetCustomValue(pFont, FALSE);

		if (!pFont)
		{
			TBase::SetStandardValue(pFont, FALSE);
			return;
		}

		LOGFONT lfFont;
		int nRes = pFont->GetLogFont(&lfFont);
		ASSERT(nRes);
		if(nRes)
			SetCustomValue(&lfFont);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function overloads the assignment operator for the
	//     CXTPCalendarViewPartFontValue class.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT structure.
	// Remarks:
	//     Creates a new custom font.
	// Returns:
	//     A reference to a CXTPCalendarViewPartFontValue.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeFontValue& operator=(LOGFONT& rLogFont) {
		SetCustomValue(&rLogFont);
		return *this;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is use to set standard value using current value
	//     from the specified object.
	// Parameters:
	//     refSrc - Reference to a source object.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue,
	//      IsStandardValue, IsCustomValue, GetStandardValue.
	//-----------------------------------------------------------------------
	void CopySettings(const CXTPCalendarThemeFontValue& refSrc)
	{
		SetStandardValue(refSrc.GetValue());
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes data from or to an archive.
	// Parameters:
	//     ar - A CArchive object to serialize to or from.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void Serialize(CArchive& ar);
};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage customized string value
//     objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarThemeStringValue : public
			CXTPCalendarThemeCustomizableXValueT<CString, LPCTSTR>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeStringValue()
	{
		m_bAutoDestroy_Standard = m_bAutoDestroy_Custom = TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to get current value.
	// Remarks:
	//     The default value is used if the custom value is not set. Otherwise,
	//     a custom value is used.
	// Returns:
	//     A pointer to the current value.
	// Example:
	//      See example for SetCustomValue.
	// See Also:
	//      SetStandardValue, SetCustomValue, SetDefaultValue, GetValue
	//-----------------------------------------------------------------------
	operator const CString&() const {
		return GetValueX();
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to set custom value.
	// Parameters:
	//      pcszValue    - A new custom value.
	// Returns:
	//      Reference to this object.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeStringValue& operator=(LPCTSTR pcszValue) {
		SetCustomValue(pcszValue);
		return *this;
	}


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to destroy/clear value dada.
	// Parameters:
	//      refValue - A value reference to destroy/clear data.
	//-----------------------------------------------------------------------
	virtual void DoDestroy(CString& refValue) {
		refValue.Empty();
	};

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes value data from or to an storage.
	// Parameters:
	//     pPX          - A CXTPPropExchange object to serialize to or from.
	//     pcszPropName - A value name.
	//     rXValue      - Reference to value.
	//     bStandard    - Standard or Custom value.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void DoPX_Value(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CString& rXValue, BOOL bStandard)
	{
		if (pPX->IsStoring())
		{
			if (bStandard && !m_bIsStandardSet ||
				!bStandard && !m_bIsCustomSet)
			{
				rXValue.Empty();
			}
		}
		PX_String(pPX, pcszPropName, rXValue);
	}
};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage customized int value
//     objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarThemeIntValue : public CXTPCalendarThemeCustomizableXValueT<int, int>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeIntValue()
	{
		m_ValueStandard = m_ValueCustom = 0;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes value data from or to an storage.
	// Parameters:
	//     pPX          - A CXTPPropExchange object to serialize to or from.
	//     pcszPropName - A value name.
	//     rXValue      - Reference to value.
	//     bStandard    - Standard or Custom value.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void DoPX_Value(CXTPPropExchange* pPX, LPCTSTR pcszPropName, int& rXValue, BOOL bStandard) {
		if (pPX->IsStoring())
		{
			if (bStandard && !m_bIsStandardSet ||
				!bStandard && !m_bIsCustomSet)
			{
				rXValue = 0;
			}
		}
		PX_Int(pPX, pcszPropName, rXValue);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to set custom value.
	// Parameters:
	//      nValue    - A new custom value.
	// Returns:
	//      Reference to this object.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeIntValue& operator=(int nValue) {
		SetCustomValue(nValue);
		return *this;
	}
};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage customized BOOL value
//     objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarThemeBOOLValue : public CXTPCalendarThemeCustomizableXValueT<BOOL, BOOL>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeBOOLValue()
	{
		m_ValueStandard = m_ValueCustom = FALSE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes value data from or to an storage.
	// Parameters:
	//     pPX          - A CXTPPropExchange object to serialize to or from.
	//     pcszPropName - A value name.
	//     rXValue      - Reference to value.
	//     bStandard    - Standard or Custom value.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void DoPX_Value(CXTPPropExchange* pPX, LPCTSTR pcszPropName, BOOL& rXValue, BOOL bStandard)
	{
		if (pPX->IsStoring())
		{
			if (bStandard && !m_bIsStandardSet ||
				!bStandard && !m_bIsCustomSet)
			{
				rXValue = FALSE;
			}
		}
		PX_Bool(pPX, pcszPropName, rXValue);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to set custom value.
	// Parameters:
	//      bValue    - A new custom value.
	// Returns:
	//      Reference to this object.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeBOOLValue& operator=(BOOL bValue) {
		SetCustomValue(bValue);
		return *this;
	}

};

//===========================================================================
// Summary:
//     Helper class provides functionality to manage customized CRect value
//     objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarThemeRectValue :
		public CXTPCalendarThemeCustomizableXValueT<CRect, CRect>
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default object constructor.
	//-----------------------------------------------------------------------
	CXTPCalendarThemeRectValue()
	{
		m_ValueStandard = m_ValueCustom = CRect(0, 0, 0, 0);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This method reads or writes value data from or to an storage.
	// Parameters:
	//     pPX          - A CXTPPropExchange object to serialize to or from.
	//     pcszPropName - A value name.
	//     rXValue      - Reference to value.
	//     bStandard    - Standard or Custom value.
	// See Also: DoPropExchange()
	//-----------------------------------------------------------------------
	virtual void DoPX_Value(CXTPPropExchange* pPX, LPCTSTR pcszPropName, CRect& rXValue, BOOL bStandard) {
		const CRect crcZero(0, 0, 0, 0);
		if (pPX->IsStoring())
		{
			if (bStandard && !m_bIsStandardSet ||
				!bStandard && !m_bIsCustomSet)
			{
				rXValue = crcZero;
			}
		}

		PX_Rect(pPX, pcszPropName, rXValue, crcZero);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member operator is use to set custom value.
	// Parameters:
	//      nValue    - A new custom value.
	// Returns:
	//      Reference to this object.
	//-----------------------------------------------------------------------
	const CXTPCalendarThemeRectValue& operator=(const CRect& rcValue) {
		SetCustomValue((CRect&)rcValue);
		return *this;
	}
};

//===========================================================================
// Summary:
//     Helper base class  to implement parts for calendar paint manager.
//     objects.
//===========================================================================
class _XTP_EXT_CLASS CXTPCalendarViewPart : public CXTPCmdTarget
{
	//{{AFX_CODEJOCK_PRIVATE
	friend class CXTPCalendarPaintManager;
	//}}AFX_CODEJOCK_PRIVATE

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Default constructor.
	// Parameters:
	//     pParentPart - Pointer to parent class, can be NULL.
	//-----------------------------------------------------------------------
	CXTPCalendarViewPart(CXTPCalendarViewPart* pParentPart = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Default destructor.
	//-----------------------------------------------------------------------
	virtual ~CXTPCalendarViewPart();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set graphical related
	//     parameters equal to the system settings.
	//-----------------------------------------------------------------------
	virtual void RefreshMetrics();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the color used to fill
	//     the background of UI elements.
	// Returns:
	//     A COLORREF that contains the value of the background color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetBackgroundColor();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain the color used to display text.
	// Returns:
	//     A COLORREF that contains the value of text color.
	//-----------------------------------------------------------------------
	virtual COLORREF GetTextColor();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to obtain a pointer to a CFont
	//     object that contains the font that is used for displaying the
	//     text.
	// Returns:
	//     A reference to a CFont object that contains the font used to
	//     display the text.
	//-----------------------------------------------------------------------
	virtual CFont& GetTextFont();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to display the text using the
	//     custom font and color.
	// Parameters:
	//     pDC     - A pointer to a valid device context.
	//     str     - A CString that contains the text to display.
	//     lpRect  - An LPRECT that contains the rectangle coordinates
	//               used to display the text.
	//     nFormat - A UINT that contains additional format parameters.
	//-----------------------------------------------------------------------
	void DrawText(CDC* pDC, const CString& str, LPRECT lpRect, UINT nFormat) {
		CFont* pOldFont = pDC->SelectObject(&GetTextFont());
		pDC->SetTextColor(GetTextColor());
		nFormat |= DT_NOPREFIX;
		pDC->DrawText(str, lpRect, nFormat);
		pDC->SelectObject(pOldFont);
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to draw a single line text in the
	//     center of the rect. If rect width is not enough to draw all chars -
	//     text is aligned to left (or right, see nFormat) or the rect.
	// Parameters:
	//     pDC     - A pointer to a valid device context.
	//     str     - A CString that contains the text to display.
	//     lpRect  - An LPRECT that contains the rectangle coordinates
	//               used to display the text.
	//     nFormat - A UINT that contains additional format parameters as
	//               combination of flags: DT_VCENTER, DT_LEFT, DT_RIGHT or 0.
	//-----------------------------------------------------------------------
	void DrawLine_CenterLR(CDC* pDC, const CString& str, LPRECT lpRect, UINT nFormat)
	{
		CFont* pOldFont = pDC->SelectObject(&GetTextFont());
		pDC->SetTextColor(GetTextColor());
		nFormat |= DT_NOPREFIX | DT_SINGLELINE;

		int nLeftRight = nFormat & (DT_LEFT | DT_RIGHT);
		nFormat &= ~(DT_CENTER | DT_LEFT | DT_RIGHT);

		CSize sz = pDC->GetTextExtent(str);
		if (sz.cx < labs(lpRect->right - lpRect->left) ) {
			nFormat |= DT_CENTER;
		}
		else
		{
			nFormat |= nLeftRight ? nLeftRight : DT_LEFT;
		}

		pDC->DrawText(str, lpRect, nFormat);

		pDC->SelectObject(pOldFont);
	}


	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to calculate the size of the area
	//     required to display the given text.
	// Parameters:
	//     pDC - A pointer to a valid device context.
	//     str - A CString that contains the string of text to display.
	// Returns:
	//     A CSize object that contains the dimensions required to display
	//     the text.
	//-----------------------------------------------------------------------
	CSize GetTextExtent(CDC* pDC, const CString& str) {
		CFont* pOldFont = pDC->SelectObject(&GetTextFont());
		CSize sz = pDC->GetTextExtent(str);
		pDC->SelectObject(pOldFont);
		return sz;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new color used to fill
	//     the background.
	// Parameters:
	//     clr - A COLORREF that contains the new color value.
	//-----------------------------------------------------------------------
	void SetBackgroundColor(COLORREF clr) {
		m_clrBackground = clr;
	}
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new color used to display
	//     the text.
	// Parameters:
	//     clr - A COLORREF that contains the new color value.
	//-----------------------------------------------------------------------
	void SetTextColor(COLORREF clr) {
		m_clrTextColor = clr;
	}
	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is used to set the new font used to display
	//     the text.
	// Parameters:
	//     pLogFont - A pointer to a LOGFONT struct that contains the new
	//                font used to display the text.
	//-----------------------------------------------------------------------
	void SetTextFont(LOGFONT* pLogFont) {
		m_fntText = pLogFont;
	}


// Attributes
protected:
	CXTPPaintManagerColor m_clrTextColor;  // Stores color settings used to display text.
	CXTPPaintManagerColor m_clrBackground; // Stores color settings used to to fill background of UI item.
	CXTPCalendarViewPartFontValue m_fntText;        // Stores font settings used to display text.

	CXTPCalendarViewPart* m_pParentPart;            // Pointer to the parent CXTPCalendarViewPart object.
	CXTPCalendarPaintManager* m_pPaintManager;      // Pointer to containing CXTPCalendarPaintManager object
};

//---------------------------------------------------------------------------
// Summary:
//     Call this function within your class's DoPropExchange member function
//     to serialize or initialize a COLORREF property.
//     The property's value is read from or written to the variable referenced
//     by refColor, as appropriate.
// Parameters:
//     pPX          - Pointer to the CXTPPropExchange object
//                    (typically passed as a parameter to DoPropExchange).
//     pcszPropName - The name of the property being exchanged.
//     refColor     - Reference to the variable where the property is stored.
// Returns:
//     Nonzero if the exchange was successful; 0 if unsuccessful.
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Color(CXTPPropExchange* pPX, LPCTSTR pcszPropName,
											COLORREF& refColor);

//---------------------------------------------------------------------------
// Summary:
//     Call this function within your class's DoPropExchange member function
//     to serialize or initialize a customized Color property.
//     The property's value is read from or written to the variable referenced
//     by refColor, as appropriate.
// Parameters:
//     pPX          - Pointer to the CXTPPropExchange object
//                    (typically passed as a parameter to DoPropExchange).
//     pcszPropName - The name of the property being exchanged.
//     refColor     - Reference to the variable where the property is stored.
// Returns:
//     Nonzero if the exchange was successful; 0 if unsuccessful.
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Color(CXTPPropExchange* pPX, LPCTSTR pcszPropName,
											CXTPPaintManagerColor& refColor);

//---------------------------------------------------------------------------
// Summary:
//     Call this function within your class's DoPropExchange member function
//     to serialize or initialize a customized Gradient Color property.
//     The property's value is read from or written to the variable referenced
//     by refGrColor, as appropriate.
// Parameters:
//     pPX          - Pointer to the CXTPPropExchange object
//                    (typically passed as a parameter to DoPropExchange).
//     pcszPropName - The name of the property being exchanged.
//     refGrColor   - Reference to the variable where the property is stored.
// Returns:
//     Nonzero if the exchange was successful; 0 if unsuccessful.
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL PX_GrColor(CXTPPropExchange* pPX, LPCTSTR psczPropName,
											CXTPPaintManagerColorGradient& refGrColor);

//---------------------------------------------------------------------------
// Summary:
//     Call this function within your class's DoPropExchange member function
//     to serialize or initialize a LOGFONT property.
//     The property's value is read from or written to the variable referenced
//     by rLogFont, as appropriate.
// Parameters:
//     pPX          - Pointer to the CXTPPropExchange object
//                    (typically passed as a parameter to DoPropExchange).
//     pcszPropName - The name of the property being exchanged.
//     rLogFont     - Reference to the variable where the property is stored.
// Returns:
//     Nonzero if the exchange was successful; 0 if unsuccessful.
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Font(CXTPPropExchange* pPX, LPCTSTR pcszPropName,
											LOGFONT& rLogFont);

//---------------------------------------------------------------------------
// Summary:
//     Call this function within your class's DoPropExchange member function
//     to serialize or initialize a customized Font property.
//     The property's value is read from or written to the variable referenced
//     by refFont, as appropriate.
// Parameters:
//     pPX          - Pointer to the CXTPPropExchange object
//                    (typically passed as a parameter to DoPropExchange).
//     pcszPropName - The name of the property being exchanged.
//     refFont      - Reference to the variable where the property is stored.
// Returns:
//     Nonzero if the exchange was successful; 0 if unsuccessful.
//---------------------------------------------------------------------------
_XTP_EXT_CLASS BOOL AFX_CDECL PX_Font(CXTPPropExchange* pPX, LPCTSTR pcszPropName,
										CXTPCalendarThemeFontValue& refFont);

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_XTPCALENDARVIEWPART_H__)
