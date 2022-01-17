// XTPPropertyGridInplaceButton.h interface for the CXTPPropertyGridInplaceButton class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#if !defined(__XTPPROPERTYGRIDINPLACEBUTTON_H__)
#define __XTPPROPERTYGRIDINPLACEBUTTON_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// class forwards

class CXTPPropertyGridView;
class CXTPPropertyGridItem;
class CXTPImageManagerIcon;

const UINT XTP_ID_PROPERTYGRID_EXPANDBUTTON = 100; //<ALIAS CXTPPropertyGridInplaceButton::CXTPPropertyGridInplaceButton@UINT>
const UINT XTP_ID_PROPERTYGRID_COMBOBUTTON = 101; //<ALIAS CXTPPropertyGridInplaceButton::CXTPPropertyGridInplaceButton@UINT>


//===========================================================================
// Summary:
//     CXTPPropertyGridInplaceButton is a CCmdTarget derived class.
//     It represents inplace button in Property Grid control.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridInplaceButton : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridInplaceButton object
	// Parameters:
	//     nID - Identifier of button
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridInplaceButton object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGridInplaceButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves width of the button
	// See Also: SetWidth
	//-----------------------------------------------------------------------
	int GetWidth();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set width of the button
	// Parameters:
	//     nWidth - New width to be set
	// See Also: GetWidth
	//-----------------------------------------------------------------------
	void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves caption of the button
	// See Also: SetCaption
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set caption text of the button
	// Parameters:
	//     lpszCaption - New caption to be set
	// See Also: GetCaption
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves image index of the button.
	// See Also: SetIconIndex
	//-----------------------------------------------------------------------
	int GetIconIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to assign icon index for the button. Add icons first to
	//     Image manager of Property Grid
	// Parameters:
	//     nIconIndex - Image index to be set.
	// See Also: GetImage, GetIconIndex, CXTPPropertyGrid::GetImageManager
	//-----------------------------------------------------------------------
	void SetIconIndex(int nIconIndex);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve image assigned for the button
	// See Also: SetIconIndex, CXTPPropertyGrid::GetImageManager
	//-----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetImage() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function retrieves the parent property grid item
	// Returns:
	//     A pointer to a CXTPPropertyGridItem object
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves identifier of the button
	// Remarks:
	//     Default identifier for expand button is XTP_ID_PROPERTYGRID_EXPANDBUTTON;
	//     Default identifier for combo button is XTP_ID_PROPERTYGRID_COMBOBUTTON
	//-----------------------------------------------------------------------
	UINT GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves bounding rectangle of the button.
	//-----------------------------------------------------------------------
	CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if button has focus
	//-----------------------------------------------------------------------
	BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if cursor under button
	//-----------------------------------------------------------------------
	BOOL IsHot()  const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if user push the button
	//-----------------------------------------------------------------------
	BOOL IsPressed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves index of button in array of item's buttons
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the button
	//     Override this method to draw custom button
	// Parameters:
	//     pDC - Pointer to valid device context
	//     rc  - Bounding rectangle of the button
	//-----------------------------------------------------------------------
	virtual void OnDraw(CDC* pDC, CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when user press the button
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down
	//     point  - Specifies the x- and y-coordinate of the cursor
	//-----------------------------------------------------------------------
	virtual void OnLButtonDown(UINT nFlags, CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when user press key while button has focus
	// Parameters:
	//     nChar - Specifies the virtual-key code of the given key.
	//-----------------------------------------------------------------------
	virtual void OnKeyDown(UINT nChar);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves parent grid object
	// Returns:
	//     Pointer to parent CXTPPropertyGridView object
	//-----------------------------------------------------------------------
	CXTPPropertyGridView* GetGrid() const;

private:

	BOOL  m_bPressed;    // TRUE if the button is pressed.
	int m_nIndex;

	CXTPPropertyGridView* m_pGrid;
	CXTPPropertyGridItem* m_pItem;
	UINT m_nID;
	CRect m_rcButton;
	int m_nWidth;
	CString m_strCaption;
	int m_nIconIndex;

protected:


private:
	friend class CXTPPropertyGridItem;
	friend class CXTPPropertyGridItems;
	friend class CXTPPropertyGridInplaceButtons;

};

//===========================================================================
// Summary:
//     CXTPPropertyGridInplaceButtons is a CCmdTarget derived class.
//     It represents collection of inplace buttons for specified grid item
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridInplaceButtons : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridInplaceButtons object
	// Parameters:
	//     pItem - Parent Item of buttons
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButtons(CXTPPropertyGridItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridInplaceButtons object, handles cleanup
	//     and deallocation.
	//-----------------------------------------------------------------------
	~CXTPPropertyGridInplaceButtons();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add standard combo button
	//-----------------------------------------------------------------------
	void AddComboButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add standard expand button
	//-----------------------------------------------------------------------
	void AddExpandButton();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add custom button:
	// Example:
	//     <code>pItem->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(1));</code>
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton* AddButton(CXTPPropertyGridInplaceButton* pButton);

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves count of buttons in collection
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves button by its index
	// Parameters:
	//     nIndex - Index to retrieve button
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove all buttons
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove specified button
	// Parameters:
	//     pButton - Button to remove
	//-----------------------------------------------------------------------
	void Remove(CXTPPropertyGridInplaceButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to remove button by its identifier
	// Parameters:
	//     nID - identifier of the button to remove
	//-----------------------------------------------------------------------
	void Remove(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines which button, if any, is at a specified position.
	// Parameters:
	//     point - Point to test.
	// Returns:
	//     The button at the specified position, if any,
	//     or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton* HitTest(CPoint point) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Finds button by its identifier
	// Parameters:
	//     nID - Identifier to find
	// Returns:
	//     The button with specified identifier, if any,
	//     or NULL otherwise.
	//-----------------------------------------------------------------------
	CXTPPropertyGridInplaceButton* Find(UINT nID) const;

private:
	void UpdateIndexes();

private:
	CArray<CXTPPropertyGridInplaceButton*, CXTPPropertyGridInplaceButton*> m_arrButtons;
	CXTPPropertyGridItem* m_pItem;

};

AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGridInplaceButton::GetItem() const {
	return m_pItem;
}
AFX_INLINE UINT CXTPPropertyGridInplaceButton::GetID() const {
	return m_nID;
}
AFX_INLINE CRect CXTPPropertyGridInplaceButton::GetRect() const {
	return m_rcButton;
}
AFX_INLINE CString CXTPPropertyGridInplaceButton::GetCaption() const {
	return m_strCaption;
}
AFX_INLINE int CXTPPropertyGridInplaceButton::GetIconIndex() const {
	return m_nIconIndex;
}
AFX_INLINE void CXTPPropertyGridInplaceButton::SetIconIndex(int nIconIndex) {
	m_nIconIndex = nIconIndex;
}
AFX_INLINE CXTPPropertyGridView* CXTPPropertyGridInplaceButton::GetGrid() const {
	return m_pGrid;
}


AFX_INLINE int CXTPPropertyGridInplaceButtons::GetCount() const {
	return (int)m_arrButtons.GetSize();
}
AFX_INLINE CXTPPropertyGridInplaceButton* CXTPPropertyGridInplaceButtons::GetAt(int nIndex) const {
	return nIndex >= 0 && nIndex < GetCount() ? m_arrButtons.GetAt(nIndex) : NULL;
}


#endif //#if !defined(__XTPPROPERTYGRIDINPLACEBUTTON_H__)
