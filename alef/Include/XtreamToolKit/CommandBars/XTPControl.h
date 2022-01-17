// XTPControl.h : interface for the CXTPControl class.
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
#if !defined(__XTPCONTROL_H__)
#define __XTPCONTROL_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common/XTPSystemHelpers.h"
#include "XTPCommandBarsDefines.h"

class CXTPControl;
class CXTPControls;
class CXTPCommandBar;
class CXTPImageManager;
class CXTPImageManagerIcon;
class CXTPPaintManager;
class CXTPPropExchange;
class CXTPRibbonGroup;
class CXTPCommandBarList;
class CXTPCommandBars;
struct XTP_COMMANDBARS_PROPEXCHANGE_PARAM;

class CXTPControlActions;

//===========================================================================
// Summary:
//     CXTPControlAction is a CCmdTarget derived class. It represents the single action of controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlAction : public CXTPCmdTarget
{
	DECLARE_DYNAMIC(CXTPControlAction)
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlAction object
	// Parameters:
	//     pActions - Owner actions collection.
	//-----------------------------------------------------------------------
	CXTPControlAction(CXTPControlActions* pActions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlAction object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPControlAction();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the caption of the action.
	// Parameters:
	//     lpszCaption - Caption to be set.
	//     nIDCaption - Caption resource identifier.
	// See Also: SetPrompt, SetDescription, SetTooltip
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);
	void SetCaption(UINT nIDCaption);// <combine CXTPControl::SetCaption@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's identifier.
	// Returns:
	//     The identifier of the control.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's caption.
	// Returns:
	//     The caption of the control.
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the description of the action.
	// Parameters:
	//     lpszDescription - Description to be set.
	// See Also: SetCaption, SetPrompt, SetTooltip
	//-----------------------------------------------------------------------
	void SetDescription(LPCTSTR lpszDescription);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to load a string from a STRINGTABLE resource
	//     to set the tooltip and description of a action.
	// Parameters:
	//     lpszPrompt -  String from resource file that contains the
	//                  tooltip and description of the item.  The Caption
	//                  will be set to use the tooltip text.  The tooltip
	//                  and description must be separated by "\n" if both
	//                  are included.  You can omit the description and
	//                  both the tooltip and description will be set to
	//                  the same value.
	// Remarks:
	//     Resource strings are in the format "Description\nToolTip".
	//
	//     Note: The caption is set to the value of the tooltip when this
	//           member is called.
	// See Also: SetCaption, SetDescription, SetTooltip
	//----------------------------------------------------------------------
	void SetPrompt(LPCTSTR lpszPrompt);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's description.
	// Returns:
	//     The description of the control.
	//-----------------------------------------------------------------------
	CString GetDescription() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the action icon's identifier.
	// Parameters:
	//     nId - Icon's identifier to be set.
	//-----------------------------------------------------------------------
	void SetIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon's identifier.
	// Returns:
	//     An icon's identifier of the action.
	//-----------------------------------------------------------------------
	int GetIconId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the action help identifier.
	// Parameters:
	//     nId - Help identifier to be set.
	//-----------------------------------------------------------------------
	void SetHelpId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the help identifier.
	// Returns:
	//     Help identifier of the action.
	//-----------------------------------------------------------------------
	int GetHelpId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable or disable the action.
	// Parameters:
	//     bEnabled - TRUE if the action is enabled.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	void SetEnabled(BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the action.
	// Returns:
	//     TRUE if the action is enabled; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check the action.
	// Parameters:
	//     bChecked - TRUE if the action is checked.
	// See Also: GetChecked, SetEnabled
	//-----------------------------------------------------------------------
	void SetChecked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the action.
	// Returns:
	//     TRUE if the action is checked; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetChecked() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the action is visible.
	// Returns:
	//     TRUE if the action is visible.
	//-----------------------------------------------------------------------
	BOOL IsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change the state of the action.
	// Parameters:
	//     bVisible - TRUE if the action is visible.
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the action
	// Parameters:
	//     dwTag - Contains the new value to associate with the action.
	//-----------------------------------------------------------------------
	void SetTag(DWORD_PTR dwTag);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the action.
	// Returns:
	//     The 32-bit value associated with the action.
	//-----------------------------------------------------------------------
	DWORD_PTR GetTag() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the category of the action.
	// Parameters:
	//     lpszCategory - Category of the action.
	//----------------------------------------------------------------------
	void SetCategory(LPCTSTR lpszCategory);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the category of the action.
	// Returns:
	//     Category of the action.
	//----------------------------------------------------------------------
	CString GetCategory() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the tooltip of the action.
	// Parameters:
	//     lpszTooltip - Tooltip to be set.
	// See Also: SetCaption, SetDescription, SetPrompt
	//-----------------------------------------------------------------------
	void SetTooltip(LPCTSTR lpszTooltip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's tooltip.
	// Returns:
	//     Tooltip of the control.
	//-----------------------------------------------------------------------
	CString GetTooltip() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the shortcut string to the action.
	// Parameters:
	//     lpszShortcutText - Shortcut to be set.
	//-----------------------------------------------------------------------
	void SetShortcutText(LPCTSTR lpszShortcutText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's shortcut.
	// Returns:
	//     The shortcut of the control.
	//-----------------------------------------------------------------------
	CString GetShortcutText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the keyboard tip to the action.
	// Parameters:
	//     lpszKeyboardTip - Keyboard tip to be set.
	//-----------------------------------------------------------------------
	void SetKeyboardTip(LPCTSTR lpszKeyboardTip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the action's keyboard tip.
	// Returns:
	//     The  keyboard tip of the control.
	//-----------------------------------------------------------------------
	CString GetKeyboardTip() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set grayed-out text displayed in the edit and combo controls
	//     that displayed a helpful description of what the control is used for.
	// Parameters:
	//     lpszEditHint - Edit hint to be set
	//-----------------------------------------------------------------------
	void SetEditHint(LPCTSTR lpszShortcutText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get grayed-out text displayed in the edit and combo controls
	//     that displayed a helpful description of what the control is used for.
	// Returns:
	//     Edit hint of the control
	//-----------------------------------------------------------------------
	CString GetEditHint() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get pointer to owner CommandBars
	// Returns:
	//     The pointer to CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get control associated with action
	// Parameters:
	//     nIndex - Index of control to retrieve
	// Returns:
	//     The pointer to CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPControl* GetControl(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get number of controls associated with the action
	// Returns:
	//     Total number of controls associated with action
	//-----------------------------------------------------------------------
	int GetCount() const;

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to redraw all controls associated with actions
	//-----------------------------------------------------------------------
	void RedrawControls();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to reposition all controls associated with actions
	//-----------------------------------------------------------------------
	void RepositionControls();


public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when new control attached to the Action
	// Parameters:
	//     pControl - Control to be attached
	// See Also: RemoveControl
	//-----------------------------------------------------------------------
	void AddControl(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to remove control from actions list
	// Parameters:
	//     pControl - Control to be removed
	// See Also: AddControl
	//-----------------------------------------------------------------------
	void RemoveControl(CXTPControl* pControl);

	//-------------------------------------------------------------------------
	// Summary:
	//     This method called before action removed
	//-------------------------------------------------------------------------
	void OnRemoved();

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when property of action was changed
	// Parameters:
	//     nProperty - Property identifier
	// See Also: OnChanging
	//-----------------------------------------------------------------------
	void OnChanged(int nProperty);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when property of action is about to be changed
	// Parameters:
	//     nProperty - Property identifier
	// See Also:OnChanged
	//-----------------------------------------------------------------------
	void OnChanging(int nProperty);



public:
	CCmdTarget* m_pTarget;          // Parameter of action

protected:
	DWORD_PTR m_nTag;               // Action tag.
	int m_nId;                      // Identifier of the action.
	int m_nIconId;                  // Identifier of the action's image.
	int m_nHelpId;                  // Help identifier
	BOOL m_bEnabled;                // TRUE if the action is enabled.
	BOOL m_bChecked;                // TRUE if the action is checked.
	BOOL m_bVisible;                // TRUE if action is visible
	CString m_strCaption;           // Caption of the action.
	CString m_strTooltipText;       // Tooltip text.
	CString m_strDescriptionText;   // Description text.
	CString m_strCategory;          // Category of the action.
	CString m_strShortcutText;      // Shortcut text
	CString m_strEditHint;          // Edit Hint for Combo and Edit Controls
	CString m_strKeyboardTip;       // Keyboard tip

	CArray<CXTPControl*, CXTPControl*> m_arrControls;   // Associated controls
	CXTPControlActions* m_pActions; // Parent actions object

private:
	friend class CXTPControl;
	friend class CXTPControls;
	friend class CXTPControlActions;
};


AFX_INLINE void CXTPControlAction::SetShortcutText(LPCTSTR lpszShortcutText) {
	m_strShortcutText = lpszShortcutText;
}
AFX_INLINE CString CXTPControlAction::GetShortcutText() const {
	return m_strShortcutText;
}
AFX_INLINE void CXTPControlAction::SetDescription(LPCTSTR lpszDescription) {
	m_strDescriptionText = lpszDescription;
}
AFX_INLINE CString CXTPControlAction::GetDescription() const {
	return m_strDescriptionText;
}
AFX_INLINE CString CXTPControlAction::GetTooltip() const {
	return m_strTooltipText;
}
AFX_INLINE void CXTPControlAction::SetTooltip(LPCTSTR lpszTooltip) {
	m_strTooltipText = lpszTooltip;
}
AFX_INLINE CString CXTPControlAction::GetCaption() const {
	return m_strCaption.IsEmpty() ? m_strTooltipText : m_strCaption;
}
AFX_INLINE CString CXTPControlAction::GetEditHint() const {
	return m_strEditHint;
}
AFX_INLINE int CXTPControlAction::GetID () const {
	return m_nId;
}
AFX_INLINE void CXTPControlAction::SetIconId(int nId) {
	if (m_nIconId != nId) {m_nIconId = nId; RedrawControls();}
}
AFX_INLINE int CXTPControlAction::GetIconId() const {
	return m_nIconId <= 0 ? m_nId : m_nIconId;
}
AFX_INLINE void CXTPControlAction::SetHelpId(int nId) {
	m_nHelpId = nId;
}
AFX_INLINE int CXTPControlAction::GetHelpId() const {
	return m_nHelpId <= 0 ? m_nId : m_nHelpId;
}

AFX_INLINE void CXTPControlAction::SetEnabled(BOOL bEnabled) {
	if (m_bEnabled != bEnabled) {m_bEnabled = bEnabled; OnChanged(0); RedrawControls();}
}
AFX_INLINE BOOL CXTPControlAction::GetChecked() const {
	return m_bChecked;
}
AFX_INLINE void CXTPControlAction::SetChecked(BOOL bChecked) {
	if (m_bChecked != bChecked) {m_bChecked = bChecked; OnChanged(1); RedrawControls();}
}
AFX_INLINE BOOL CXTPControlAction::IsVisible() const {
	return m_bVisible;
}
AFX_INLINE void CXTPControlAction::SetVisible(BOOL bVisible) {
	if (m_bVisible != bVisible) { m_bVisible = bVisible; OnChanged(2); RepositionControls();}
}
AFX_INLINE void CXTPControlAction::SetCategory(LPCTSTR lpszCategory) {
	m_strCategory = lpszCategory;
}
AFX_INLINE CString CXTPControlAction::GetCategory() const {
	return m_strCategory;
}
AFX_INLINE void CXTPControlAction::SetTag(DWORD_PTR dwTag) {
	m_nTag = dwTag;
}
AFX_INLINE DWORD_PTR CXTPControlAction::GetTag() const {
	return m_nTag;
}
AFX_INLINE void CXTPControlAction::SetKeyboardTip(LPCTSTR lpszKeyboardTip) {
	m_strKeyboardTip = lpszKeyboardTip;
}
AFX_INLINE CString CXTPControlAction::GetKeyboardTip() const {
	return m_strKeyboardTip;
}



//===========================================================================
// Summary:
//     CXTPControlActions is a CCmdTarget derived class. It represents a collection
//     of the actions.
//===========================================================================
class _XTP_EXT_CLASS CXTPControlActions : public CXTPCmdTarget
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControlActions object
	// Parameters:
	//     pCommandBars - Parent commandbars object
	//-----------------------------------------------------------------------
	CXTPControlActions(CXTPCommandBars* pCommandBars);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControlActions object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPControlActions();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return the action at the specified index.
	// Parameters:
	//     nIndex - An integer index.
	// Returns:
	//     The CXTPControlAction pointer currently at this index.
	//-----------------------------------------------------------------------
	CXTPControlAction* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the count of the actions
	// Returns:
	//     The count of the actions.
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to find the specified action.
	// Parameters:
	//     nId        - The action's identifier.
	// Returns:
	//     Pointer to the CXTPControlAction object if successful; otherwise returns NULL.
	//-----------------------------------------------------------------------
	CXTPControlAction* FindAction(int nId) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to add a new action.
	// Parameters:
	//     nId        - Identifier of the control to be added.
	//     pAction    - Action to add
	// Returns:
	//     A pointer to the added action.
	//-----------------------------------------------------------------------
	CXTPControlAction* Add(int nId);
	CXTPControlAction* Add(int nId, CXTPControlAction* pAction); // <combine CXTPControlActions::Add@int>

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	void DoPropExchange(CXTPPropExchange* pPX);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove all actions.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to create actions using menu commands
	//-----------------------------------------------------------------------
	void CreateFromMenu(CMenu* pMenu);


protected:


private:
	void Insert(CXTPControlAction* pAction);


protected:
	CArray<CXTPControlAction*, CXTPControlAction*> m_arrActions;    // Actions array
	CXTPCommandBars* m_pCommandBars;        // Parent CommandBars object

	friend class CXTPCommandBars;
	friend class CXTPControlAction;
};

//===========================================================================
// Summary:
//     CXTPControl is a CCmdTarget derived class. It represents the parent
//     class for the command bar's controls.
//===========================================================================
class _XTP_EXT_CLASS CXTPControl : public CXTPCmdTarget, public CXTPAccessible
{
private:
	class CDocTemplateMap : public CMap<UINT, UINT, BOOL, BOOL>
	{
	public:
		void Copy(CDocTemplateMap& map);
	};


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPControl object
	//-----------------------------------------------------------------------
	CXTPControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPControl object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPControl();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the style of the control.
	// Parameters:
	//     buttonStyle - The style to be set. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     buttonStyle parameter can be one of the following:
	//         * <b>xtpButtonAutomatic</b> Indicates the default style.
	//         * <b>xtpButtonCaption</b> Indicates caption drawing only.
	//         * <b>xtpButtonIcon</b> Indicates icon drawing only.
	//         * <b>xtpButtonIconAndCaption</b> Indicates icon and caption drawing.
	//-----------------------------------------------------------------------
	void SetStyle(XTPButtonStyle buttonStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's style.
	// Returns:
	//     The style of the control.
	//-----------------------------------------------------------------------
	XTPButtonStyle GetStyle()  const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the caption of the control.
	// Parameters:
	//     lpszCaption - Caption to be set.
	//     nIDCaption - Caption resource identifier.
	// See Also: SetPrompt, SetDescription, SetTooltip
	//-----------------------------------------------------------------------
	void SetCaption(LPCTSTR lpszCaption);
	void SetCaption(UINT nIDCaption);// <combine CXTPControl::SetCaption@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's caption.
	// Returns:
	//     The caption of the control.
	//-----------------------------------------------------------------------
	virtual CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the shortcut string to the control.
	// Parameters:
	//     lpszShortcutText - Shortcut to be set.
	//-----------------------------------------------------------------------
	void SetShortcutText(LPCTSTR lpszShortcutText);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's shortcut.
	// Returns:
	//     The shortcut of the control.
	//-----------------------------------------------------------------------
	CString GetShortcutText() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the description of the control.
	// Parameters:
	//     lpszDescription - Description to be set.
	// See Also: SetCaption, SetPrompt, SetTooltip
	//-----------------------------------------------------------------------
	void SetDescription(LPCTSTR lpszDescription);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's description.
	// Returns:
	//     The description of the control.
	//-----------------------------------------------------------------------
	virtual CString GetDescription() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the tooltip of the control.
	// Parameters:
	//     lpszTooltip - Tooltip to be set.
	// See Also: SetCaption, SetDescription, SetPrompt
	//-----------------------------------------------------------------------
	void SetTooltip(LPCTSTR lpszTooltip);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's tooltip.
	// Parameters:
	//     pPoint    - Points to a POINT structure to receive xy coordinates.
	//     lpRectTip - Points to a RECT structure to receive size information.
	//     nHit      - Specifies the hit-test area code.
	// Returns:
	//     Tooltip of the control.
	//-----------------------------------------------------------------------
	virtual CString GetTooltip(LPPOINT pPoint = 0, LPRECT lpRectTip = 0, INT_PTR* nHit = 0) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the parameter of the control.
	// Parameters:
	//     lpszParameter - Parameter to be set.
	// Remarks:
	//     This method sets CString value associated with the control, use SetTag to set numeric parameter.
	// See Also: SetTag, GetParameter
	//-----------------------------------------------------------------------
	void SetParameter(LPCTSTR lpszParameter);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's parameter.
	// Returns:
	//     The parameter of the control.
	// See Also: GetTag, SetParameter
	//-----------------------------------------------------------------------
	CString GetParameter() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the identifier of the control.
	// Parameters:
	//     nId - Identifier to be set.
	//-----------------------------------------------------------------------
	void SetID(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's identifier.
	// Returns:
	//     The identifier of the control.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the index of the control.
	// Returns:
	//     The index of the control.
	//-----------------------------------------------------------------------
	long GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the control icon's identifier.
	// Parameters:
	//     nId - Icon's identifier to be set.
	//-----------------------------------------------------------------------
	void SetIconId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the icon's identifier.
	// Returns:
	//     An icon's identifier of the control.
	//-----------------------------------------------------------------------
	int GetIconId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the control help identifier.
	// Parameters:
	//     nId - Help identifier to be set.
	//-----------------------------------------------------------------------
	void SetHelpId(int nId);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the help identifier.
	// Returns:
	//     Help identifier of the control.
	//-----------------------------------------------------------------------
	int GetHelpId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to begin a new group using a separator.
	// Parameters:
	//     bBeginGroup - TRUE if the control starts a new group.
	//-----------------------------------------------------------------------
	virtual void SetBeginGroup(BOOL bBeginGroup);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the control has a separator.
	// Returns:
	//     TRUE if the control starts a new group; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetBeginGroup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to enable or disable the control.
	//     If the control does not have xtpFlagManualUpdate flag, you must call
	//     the Enable member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// Parameters:
	//     bEnabled - TRUE if the control is enabled.
	// See Also: GetEnabled, SetChecked
	//-----------------------------------------------------------------------
	virtual void SetEnabled(BOOL bEnabled);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the control.
	// Returns:
	//     TRUE if the control is enabled; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check the control.
	//     If the control does not have xtpFlagManualUpdate flag, you must call
	//     the SetCheck member of CCmdUI in the ON_UPDATE_COMMAND_UI handler.
	// Parameters:
	//     bChecked - TRUE if the control is checked.
	// See Also: GetChecked, SetEnabled
	//-----------------------------------------------------------------------
	void SetChecked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the control.
	// Returns:
	//     TRUE if the control is checked; otherwise FALSE.
	//-----------------------------------------------------------------------
	BOOL GetChecked() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the control.
	// Returns:
	//     TRUE if the control is selected; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual int GetSelected() const;


	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set focus to the control.
	// Parameters:
	//     bFocused - TRUE to set focus
	//-----------------------------------------------------------------------
	virtual void SetFocused(BOOL bFocused);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the focused state of the control.
	// Returns:
	//     TRUE if the control has focus; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to press the control.
	// Parameters:
	//     bPressed - TRUE if the control is pressed.
	//-----------------------------------------------------------------------
	void SetPressed(BOOL bPressed);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the state of the control.
	// Returns:
	//     TRUE if the control is pressed; otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL GetPressed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to set the bounding rectangle of the control.
	// Parameters:
	//     rcControl - Bounding rectangle of the control.
	//-----------------------------------------------------------------------
	virtual void SetRect(CRect rcControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the bounding rectangle of the control.
	// Returns:
	//     The bounding rectangle of the control.
	//-----------------------------------------------------------------------
	CRect GetRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the parent command bar.
	// Returns:
	//     The parent command bar object.
	//-----------------------------------------------------------------------
	CXTPCommandBar* GetParent() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the control is visible.
	// Parameters:
	//     dwSkipFlags - combination of XTPControlHideFlags that can be ignored
	// Returns:
	//     TRUE if the control is visible.
	// See Also: XTPControlHideFlags, GetHideFlags
	//-----------------------------------------------------------------------
	virtual BOOL IsVisible(DWORD dwSkipFlags = 0) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the caption of the control is visible
	// Returns:
	//     TRUE if the caption is visible.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to change the state of the control.
	// Parameters:
	//     bVisible - TRUE if the control is visible.
	//-----------------------------------------------------------------------
	void SetVisible(BOOL bVisible);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the parent controls collection.
	// Returns:
	//     A CXTPControls pointer to the parent controls.
	//-----------------------------------------------------------------------
	CXTPControls* GetControls() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's type
	// Returns:
	//     The type of the control. It can be one of the following:
	//         * <b>xtpControlButton</b> Indicates the control is simple button (CXTPControlButton)
	//         * <b>xtpControlPopup</b> Indicates the control is simple popup button. (CXTPControlPopup)
	//         * <b>xtpControlButtonPopup</b> Indicates the control is popup button with icon (CXTPControlPopup)
	//         * <b>xtpControlSplitButtonPopup</b> Indicates the control is split button popup.(CXTPControlPopup)
	//         * <b>xtpControlComboBox</b> Indicates the control is combo box (CXTPControlComboBox)
	//         * <b>xtpControlEdit</b> Indicates the control is edit control (CXTPControlEdit)
	//         * <b>xtpControlLabel</b> Indicates the control is label (CXTPControlLabel)
	//-----------------------------------------------------------------------
	XTPControlType GetType() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the flags of the control.
	// Parameters:
	//     dwFlags - control flags.
	// Remarks:
	//     Flags to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:
	//         * <b>xtpFlagRightAlign</b> Indicates the control is right aligned.
	//         * <b>xtpFlagSkipFocus</b> Indicates the control does not have focus.
	//         * <b>xtpFlagLeftPopup</b> Indicates the child bar should pop-up on the left.
	//         * <b>xtpFlagManualUpdate</b> Indicates the control is manually updated.
	//         * <b>xtpFlagNoMovable</b> Indicates the control's customization is disabled.
	//         * <b>xtpFlagControlStretched</b> Indicates the control is stretched in the parent command bar.
	// See Also: GetFlags, XTPControlFlags
	//-----------------------------------------------------------------------
	void SetFlags(DWORD dwFlags);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve control special flags.
	// Returns:
	//     Controls special flags.
	// See Also: SetFlags, XTPControlFlags
	//----------------------------------------------------------------------
	DWORD GetFlags() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the 32-bit value associated with the control.
	// Parameters:
	//     dwTag - Contains the new value to associate with the control.
	//-----------------------------------------------------------------------
	void SetTag(DWORD_PTR dwTag);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the control.
	// Returns:
	//     The 32-bit value associated with the control.
	//-----------------------------------------------------------------------
	DWORD_PTR GetTag() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to compare controls.
	// Parameters:
	//     pOther - The control need compare with.
	// Returns:
	//     TRUE if the controls are identical.
	//-----------------------------------------------------------------------
	virtual BOOL Compare(CXTPControl* pOther);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the child command bar.
	// Returns:
	//     A pointer to the child command bar.
	//-----------------------------------------------------------------------
	virtual CXTPCommandBar* GetCommandBar() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member function to enable or disable closing of the
	//     sub menus when a control is clicked in the menu.
	// Parameters:
	//     bCloseOnClick - TRUE if the sub menu will close when a control
	//                     is clicked, FALSE if the menu will remain open
	//                     after a control is clicked.
	// See Also: GetCloseSubMenuOnClick
	//----------------------------------------------------------------------
	void SetCloseSubMenuOnClick(BOOL bCloseOnClick);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if sub menus will close
	//     as soon as a control is clicked.
	// Returns:
	//     TRUE if the sub menu will close when a control is clicked, FALSE
	//     if the menu will remain open after a control is clicked.
	// See Also: SetCloseSubMenuOnClick
	//----------------------------------------------------------------------
	BOOL GetCloseSubMenuOnClick() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to make button send WM_COMMAND messages while it pressed by user
	// Parameters:
	//     nExecuteOnPressInterval - Ms delay between each message.
	// See Also: GetExecuteOnPressInterval
	//-----------------------------------------------------------------------
	void SetExecuteOnPressInterval(int nExecuteOnPressInterval);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if SetExecuteOnPressInterval was called
	//-----------------------------------------------------------------------
	int GetExecuteOnPressInterval() const;

	//-----------------------------------------------------------------------
	// Summary: Returns the ribbon group the control belongs to.
	// Returns: If the control belongs to a ribbon group, then the CXTPRibbonGroup
	//          that the control belongs to is returned.  If the control does not
	//          belong to a ribbon group NULL is returned.
	// See Also: CXTPRibbonBar::RebuildControls
	//-----------------------------------------------------------------------
	CXTPRibbonGroup* GetRibbonGroup() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if control need to draw its background
	// Returns:
	//     TRUE if paint manager must skip filling background of control
	//-----------------------------------------------------------------------
	virtual BOOL IsTransparent() const { return FALSE;}

public:

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to hide the control.
	// Parameters:
	//     bHide - TRUE to set hide state.
	//----------------------------------------------------------------------
	void SetHideWrap(BOOL bHide);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to hide the control.
	// Parameters:
	//     dwFlags - Reasons to hide.
	// Remarks:
	//     HideFlags will tell how a control was hidden if it is hidden
	//     or why it is not visible.  Most of these flags are read only
	//     and should not be set be the developer.
	//     There is only one situation where you should set these flags
	//     manually.  The xtpHideCustomize is the only flag that should
	//     ever be manually set. you would do this when you want the
	//     control to initially be hidden but still exist in customize
	//     popup of toolbar.  This will cause the control to appear in
	//     the Add and Remove Buttons popup without a check mark indicating
	//     that it is currently hidden.  This flag will automatically
	//     be set when a command is hidden this way.
	// See Also: XTPControlHideFlags
	//----------------------------------------------------------------------
	virtual void SetHideFlags(DWORD dwFlags);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to set/remove hide flags.
	// Parameters:
	//     dwFlag - XTPControlHideFlags type of flag to be set/remove
	//     bSet   - TRUE to set flag; FALSE to remove.
	// Remarks:
	//     SetHideFlag will tell how a control was hidden if it is hidden
	//     or why it is not visible.  Most of these flags are read only
	//     and should not be set be the developer.
	// Returns:
	//     TRUE if flags was changed
	// See Also: XTPControlHideFlags, SetHideFlags
	//----------------------------------------------------------------------
	BOOL SetHideFlag(XTPControlHideFlags dwFlag, BOOL bSet);


	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve hidden flags.
	// Returns:
	//     Flags of the control visibility.
	// Remarks:
	//     HideFlags will tell how a control was hidden if it is hidden
	//     or why it is not visible.  Most of these flags are read only
	//     and should not be set be the developer.
	//     There is only one situation where you should set these flags
	//     manually.  The xtpHideCustomize is the only flag that should
	//     ever be manually set. you would do this when you want the
	//     control to initially be hidden but still exist in customize
	//     popup of toolbar.  This will cause the control to appear in
	//     the Add and Remove Buttons popup without a check mark indicating
	//     that it is currently hidden.  This flag will automatically
	//     be set when a command is hidden this way.
	// See Also: XTPControlHideFlags
	//----------------------------------------------------------------------
	DWORD GetHideFlags() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the position of the control's row.
	// Returns:
	//     Bounding rectangle of the control's row.
	//----------------------------------------------------------------------
	CRect GetRowRect() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve if the control is wrapped.
	// Returns:
	//     TRUE if the control is wrapped; otherwise returns FALSE
	// See Also: SetWrap
	//----------------------------------------------------------------------
	BOOL GetWrap() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the row of the control.
	// Parameters:
	//     rcRow - Rectangle of the control's row.
	//----------------------------------------------------------------------
	void SetRowRect(CRect rcRow);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the wrap.
	// Parameters:
	//     bWrap - TRUE to set wrap.
	// See Also: GetWrap
	//----------------------------------------------------------------------
	void SetWrap(BOOL bWrap);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve if the control is expanded.
	// Returns:
	//     TRUE if the control is expanded; otherwise returns FALSE
	//----------------------------------------------------------------------
	BOOL GetExpanded() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the mouse pointer is over
	//     the control.
	// Returns:
	//     TRUE if the mouse cursor is over the control; otherwise returns
	//     FALSE
	//----------------------------------------------------------------------
	BOOL IsCursorOver() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the control is executed.
	//----------------------------------------------------------------------
	virtual void OnExecute();

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine the size of the control.
	// Parameters:
	//     pDC - Pointer to a valid device context
	// Returns:
	//     Size of the control.
	//----------------------------------------------------------------------
	virtual CSize GetSize(CDC* pDC);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to draw the control.
	// Parameters:
	//     pDC - Pointer to a valid device context.
	//----------------------------------------------------------------------
	virtual void Draw(CDC* pDC);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called before recalculating the parent command
	//     bar size to calculate the dimensions of the control.
	// Parameters:
	//     dwMode - Flags used to determine the height and width of the
	//              dynamic command bar. See Remarks section for a list of
	//              values.
	// Remarks:
	//     The following predefined flags are used to determine the height and
	//     width of the dynamic command bar. Use the bitwise-OR (|) operator to
	//     combine the flags.<p/>
	//
	//     * <b>LM_STRETCH</b> Indicates whether the command bar should be
	//                stretched to the size of the frame. Set if the bar is
	//                not a docking bar (not available for docking). Not set
	//                when the bar is docked or floating (available for
	//                docking). If set, LM_STRETCH returns dimensions based
	//                on the LM_HORZ state. LM_STRETCH works similarly to
	//                the the bStretch parameter used in CalcFixedLayout;
	//                see that member function for more information about
	//                the relationship between stretching and orientation.
	//     * <b>LM_HORZ</b> Indicates that the bar is horizontally or
	//                vertically oriented. Set if the bar is horizontally
	//                oriented, and if it is vertically oriented, it is not
	//                set. LM_HORZ works similarly to the the bHorz
	//                parameter used in CalcFixedLayout; see that member
	//                function for more information about the relationship
	//                between stretching and orientation.
	//     * <b>LM_MRUWIDTH</b> Most Recently Used Dynamic Width. Uses the
	//                remembered most recently used width.
	//     * <b>LM_HORZDOCK</b> Horizontal Docked Dimensions. Returns the
	//                dynamic size with the largest width.
	//     * <b>LM_VERTDOCK</b> Vertical Docked Dimensions. Returns the dynamic
	//                size with the largest height.
	//     * <b>LM_COMMIT</b> Resets LM_MRUWIDTH to current width of
	//                floating command bar.
	//
	//     The framework calls this member function to calculate the dimensions
	//     of a dynamic command bar.<p/>
	//
	//     Override this member function to provide your own layout in classes
	//     you derive from CXTPControl. XTP classes derived from CXTPControl,
	//     such as CXTPControlComboBox, override this member function to provide
	//     their own implementation.
	// See Also:
	//     CXTPControlComboBox, CXTPControlCustom, CXTPControlEdit,
	//     CXTPControlWindowList, CXTPControlWorkspaceActions, CXTPControlToolbars,
	//     CXTPControlOleItems, CXTPControlRecentFileList, CXTPControlSelector,
	//     CXTPControlListBox
	//----------------------------------------------------------------------
	virtual void OnCalcDynamicSize(DWORD dwMode);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw the parent command bar.
	//----------------------------------------------------------------------
	void DelayRedrawParent();

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to recalculate the parent command bar size.
	//----------------------------------------------------------------------
	void DelayLayoutParent();

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the category of the control.
	// Parameters:
	//     lpszCategory - Category of the control.
	//----------------------------------------------------------------------
	void SetCategory(LPCTSTR lpszCategory);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the category of the control.
	// Returns:
	//     Category of the control.
	//----------------------------------------------------------------------
	CString GetCategory() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the image manager.
	// Returns:
	//     A pointer to a CXTPImageManager object.
	//----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the paint manager.
	// Returns:
	//     A pointer to a CXTPPaintManager object.
	//----------------------------------------------------------------------
	CXTPPaintManager* GetPaintManager() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve the image of the control.
	// Parameters:
	//     nWidth - Width of the image to retrieve.
	// Returns:
	//     A pointer to a CXTPImageManagerIcon object.
	//----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetImage(int nWidth = 16) const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to assign a document template to the control.
	//     Assigned control will be shown only when the child MDI window is activated
	//     with the assigned document template.
	// Parameters:
	//     nIDResource - document template identifier.
	// See Also: AssignDocTemplate
	//----------------------------------------------------------------------
	void AssignDocTemplate(UINT nIDResource);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to exclude the document template from control.
	//     Control will be shown only when the child MDI window is activated
	//     with the document template different from excluded.
	// Parameters:
	//     nIDResource - document template identifier.
	// See Also: ExcludeDocTemplate
	//----------------------------------------------------------------------
	void ExcludeDocTemplate(UINT nIDResource);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve if control is default menu item.
	// Returns:
	//     TRUE if control is default menu item.
	// See Also: SetItemDefault
	//----------------------------------------------------------------------
	BOOL IsItemDefault()  const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the default menu item.
	// Parameters:
	//     bDefault - TRUE to set control default menu item.
	// Remarks:
	//     A default menu item will appear in bold text.
	// See Also: IsItemDefault
	//----------------------------------------------------------------------
	void SetItemDefault(BOOL bDefault);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if control is temporary.
	//     Temporary controls will not be saved to the system registry when
	//     the application is closed (If the CommandBars layout is saved).
	// Returns:
	//     TRUE if control is temporary.
	//----------------------------------------------------------------------
	BOOL IsTemporary() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to reset the icon and caption of a control
	//     to its default settings before it was customized by the user.
	// Remarks:
	//     The caption and icon of a control is customized while in customization
	//     mode.
	// See Also:
	//     SetCustomIcon, m_strCustomCaption
	//----------------------------------------------------------------------
	void Reset();

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to load a string from a STRINGTABLE resource
	//     to set the tooltip and description of a control.
	// Parameters:
	//     lpszPrompt -  String from resource file that contains the
	//                  tooltip and description of the item.  The Caption
	//                  will be set to use the tooltip text.  The tooltip
	//                  and description must be separated by "\n" if both
	//                  are included.  You can omit the description and
	//                  both the tooltip and description will be set to
	//                  the same value.
	// Remarks:
	//     Resource strings are in the format "Description\nToolTip".
	//
	//     Note: The caption is set to the value of the tooltip when this
	//           member is called.
	// See Also: SetCaption, SetDescription, SetTooltip
	//----------------------------------------------------------------------
	void SetPrompt(LPCTSTR lpszPrompt);

	// -------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve CXTPControl from CCmdUI structure.
	// Parameters:
	//     pCmdUI :  Address of a CCmdUI structure.
	// Returns:
	//     CXTPControl from CCmdUI structure.
	// -------------------------------------------------------------------
	static CXTPControl* AFX_CDECL FromUI(CCmdUI* pCmdUI);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a custom icon to this control.
	// Parameters:
	//     hIcon - HICON object containing icon to set to control.
	// See Also: Reset
	//-----------------------------------------------------------------------
	void SetCustomIcon(HICON hIcon);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads or writes this object from or to an archive.
	// Parameters:
	//     pPX - A CXTPPropExchange object to serialize to or from.
	//----------------------------------------------------------------------
	virtual void DoPropExchange(CXTPPropExchange* pPX);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user activate control using its underline.
	//----------------------------------------------------------------------
	virtual void OnUnderlineActivate();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the control's popup state.
	// Returns:
	//     TRUE if the control is a popup.
	//-----------------------------------------------------------------------
	virtual BOOL GetPopuped() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to redraw the parent command bar.
	// Parameters:
	//     bAnimate - TRUE to animate changes
	//----------------------------------------------------------------------
	void RedrawParent(BOOL bAnimate = TRUE);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to sent message code to parent site window.
	// Parameters:
	//     code - Message to be sent.
	//----------------------------------------------------------------------
	LRESULT NotifySite(UINT code);
	LRESULT NotifySite(UINT code, NMXTPCONTROL* pNM);
	LRESULT NotifySite(CWnd* pSite, UINT code, NMXTPCONTROL* pNM);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to manually assign action for control
	// Parameters:
	//     pAction - Action to assign
	// See Also:
	//     CXTPControlAction, GetAction
	//----------------------------------------------------------------------
	virtual void SetAction(CXTPControlAction* pAction);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve action associated with control
	// See Also:
	//     CXTPControlAction, SetAction
	//----------------------------------------------------------------------
	CXTPControlAction* GetAction() const;


	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called when paint manager theme was changed
	//-------------------------------------------------------------------------
	virtual void OnThemeChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the width of the control
	// Parameters:
	//     nWidth - The width of the control
	//-----------------------------------------------------------------------
	virtual void SetWidth(int nWidth);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the height of the control
	// Parameters:
	//     nHeight - The height of the control
	//-----------------------------------------------------------------------
	virtual void SetHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the width of the control
	// Returns:
	//     Width of the control in pixels
	// See Also: GetHeight
	//-----------------------------------------------------------------------
	int GetWidth() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the height of the control
	// Returns:
	//     Height of the control in pixels
	// See Also: GetWidth
	//-----------------------------------------------------------------------
	int GetHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine if control has Vista DWM transparent area
	//-----------------------------------------------------------------------
	BOOL HasDwmCompositedRect() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to get the size of the command bar button.
	// Returns:
	//     The width and height values of the command bar button.
	//-----------------------------------------------------------------------
	virtual CSize GetButtonSize() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the size of a toolbar icon.
	// See Also:
	//     GetButtonSize
	//-----------------------------------------------------------------------
	virtual CSize GetIconSize() const;

protected:

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to check if control accept focus
	// See Also: SetFocused, IsFocused
	//----------------------------------------------------------------------
	virtual BOOL IsFocusable() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign the order of the control in
	//     the controls list.
	// Parameters:
	//     nIndex - Index of the control.
	//----------------------------------------------------------------------
	void SetIndex(int nIndex) { m_nIndex = nIndex; }

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign expanded state to the control.
	// Parameters:
	//     bExpanded - TRUE if the control is expanded.
	//----------------------------------------------------------------------
	void SetExpanded(BOOL bExpanded);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when a non-system key is pressed.
	// Parameters:
	//     nChar - Specifies the virtual key code of the given key.
	//     lParam   - Specifies additional message-dependent information.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnHookKeyDown(UINT nChar, LPARAM lParam);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when user rotates the mouse wheel.
	// Parameters:
	//     nFlags - Indicates whether various virtual keys are down.
	//     zDelta - Indicates distance rotated.
	//     point  - Specifies the x- and y-coordinate of the cursor.
	// Returns:
	//     TRUE if key handled, otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnHookMouseWheel(UINT nFlags, short zDelta, CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to assign a parent command bar object.
	// Parameters:
	//     pParent - Points to a CXTPCommandBar object
	//----------------------------------------------------------------------
	virtual void SetParent(CXTPCommandBar* pParent) { m_pParent = pParent;}

	//----------------------------------------------------------------------
	// Summary:
	//     The ScreenToClient function converts the screen coordinates
	//     of a specified point on the screen
	// Parameters:
	//     point - Pointer to a CPoint object that specifies the screen
	//     coordinates to be converted.
	//----------------------------------------------------------------------
	void ScreenToClient(CPoint* point);

	//----------------------------------------------------------------------
	// Summary:
	//     This helper method is called when the user clicks to the toolbar's
	//     button.
	// Parameters:
	//     rcActiveRect - Rectangle of the control.
	//----------------------------------------------------------------------
	void ClickToolBarButton(CRect rcActiveRect = CRect(0, 0, 0, 0));

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to copy the control.
	// Parameters:
	//     pControl   - Points to a source CXTPControl object
	//     bRecursive - TRUE to copy recursively.
	//----------------------------------------------------------------------
	virtual void Copy(CXTPControl* pControl, BOOL bRecursive = FALSE);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to popup the control.
	// Parameters:
	//     bPopup - TRUE to set popup.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnSetPopup(BOOL bPopup);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the user clicks the control.
	// Parameters:
	//     bKeyboard - TRUE if the control is selected using the keyboard.
	//     pt        - Mouse cursor position.
	//----------------------------------------------------------------------
	virtual void OnClick(BOOL bKeyboard = FALSE, CPoint pt = CPoint(0, 0));

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user releases the left mouse button.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	virtual void OnLButtonUp(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user press the right mouse button.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	virtual BOOL OnRButtonDown(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user releases the right mouse button.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	virtual void OnRButtonUp(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user double-clicks the left mouse button.
	// Parameters:
	//     point - Specifies the x- and y-coordinates of the cursor.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnLButtonDblClk(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the mouse cursor moves.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	virtual void OnMouseMove(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     Called after the mouse hovers over the control.
	//----------------------------------------------------------------------
	virtual void OnMouseHover();

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when the control becomes selected.
	// Parameters:
	//     bSelected - TRUE if the control becomes selected.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL OnSetSelected(int bSelected);

	//----------------------------------------------------------------------
	// Summary:
	//     Call this method to determine if command bars is in customize
	//     mode.
	// Returns:
	//     TRUE if command bars is in Customize mode; otherwise returns
	//     FALSE
	//----------------------------------------------------------------------
	BOOL IsCustomizeMode() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user starts to drag the control in customize mode.
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	// See Also: IsCustomizeMode, CustomizeStartResize
	//----------------------------------------------------------------------
	virtual void CustomizeStartDrag(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This method is called when user starts resize control in customize mode
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	// Returns:
	//     TRUE if control process this event.
	// See Also: IsCustomizeMode, CustomizeStartDrag, GetCustomizeMinWidth
	//----------------------------------------------------------------------
	virtual BOOL CustomizeStartResize(CPoint point);

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the user moves the mouse over the
	//     control.
	// Parameters:
	//     pDataObject - Points to a CXTPControl object
	//     point       - Mouse position.
	//     dropEffect  - DROPEFFECT enumerator.
	//----------------------------------------------------------------------
	virtual void OnCustomizeDragOver(CXTPControl* pDataObject, CPoint point, DROPEFFECT& dropEffect);

	//----------------------------------------------------------------------
	// Summary:
	//     This member checks if the user can drop the control.
	// Parameters:
	//     pCommandBar - Points to a CXTPCommandBar object
	//     point       - Mouse position.
	//     dropEffect  - DROPEFFECT enumerator.
	// Returns:
	//     TRUE if successful, otherwise returns FALSE
	//----------------------------------------------------------------------
	virtual BOOL IsCustomizeDragOverAvail(CXTPCommandBar* pCommandBar, CPoint point, DROPEFFECT& dropEffect);

	//----------------------------------------------------------------------
	// Summary:
	//     This member checks if the user can resize control.
	// Returns:
	//     TRUE if resize available.
	//----------------------------------------------------------------------
	virtual BOOL IsCustomizeResizeAllow() const;

	//----------------------------------------------------------------------
	// Summary:
	//     Retrieves available minimum width of control.
	// Remarks:
	//     This method is called in CustomizeStartResize to retrieve dimension of available rectangles
	//     of resized control.
	// Returns:
	//     Returns zero by default.
	// See Also: CXTPControlComboBox::GetCustomizeMinWidth, CXTPControlEdit::GetCustomizeMinWidth
	//----------------------------------------------------------------------
	virtual int GetCustomizeMinWidth() const;

	//----------------------------------------------------------------------
	// Summary:
	//     This member is called when the mouse cursor moves in customized mode
	// Parameters:
	//     point - Specifies the x- and y-coordinate of the cursor.
	//----------------------------------------------------------------------
	void OnCustomizeMouseMove(CPoint point);

	//-------------------------------------------------------------------------
	// Summary:
	//     This virtual method is called in idle-time processing
	//-------------------------------------------------------------------------
	virtual void OnIdleUpdate();

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called when control was removed from parent controls collection
	//-------------------------------------------------------------------------
	virtual void OnRemoved();

protected:
	//----------------------------------------------------------------------
	// Summary:
	//     This method is called to determine if control can be reposition in customize mode.
	// See Also:
	//     xtpFlagNoMovable
	//----------------------------------------------------------------------
	virtual BOOL IsCustomizeMovable() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called from LoadCommandBars
	//     to restore controls and its child commandbars
	// Parameters:
	//     pCommandBarList - CommandBars collection contains all commandbars was restored
	// See Also: GenerateCommandBarList
	//-----------------------------------------------------------------------
	virtual void RestoreCommandBarList(CXTPCommandBarList* pCommandBarList);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called from SaveCommandBars
	//     to save all child commandbars
	// Parameters:
	//      nID - Current Identifier counter
	//      pCommandBarList - List of saved commandbars
	//      pParam - Serialize parameters.
	// See Also: RestoreCommandBarList
	//-----------------------------------------------------------------------
	virtual void GenerateCommandBarList(DWORD& nID, CXTPCommandBarList* pCommandBarList, XTP_COMMANDBARS_PROPEXCHANGE_PARAM* pParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when action property was changed
	// Parameters:
	//     nProperty - Property of the action
	// See Also: OnActionChanging
	//-----------------------------------------------------------------------
	virtual void OnActionChanged(int nProperty);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when action property is about to be changed
	// Parameters:
	//     nProperty - Property of the action
	// See Also: OnActionChanged
	//-----------------------------------------------------------------------
	virtual void OnActionChanging(int nProperty);


protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription);
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual CCmdTarget* GetAccessible();
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);

	DECLARE_INTERFACE_MAP()
//}}AFX_CODEJOCK_PRIVATE

private:
	void OnInvertTracker(CDC* pDC, CRect rect);
	BOOL NeedPressOnExecute() const;

protected:
	DWORD_PTR m_nTag;               // Control tag.
	int m_nIndex;                   // Index of the control.
	int m_nId;                      // Identifier of the control.
	int m_nIconId;                  // Identifier of the control's image.
	int m_nHelpId;                  // Help identifier
	int m_nCustomIconId;            // Identifier of the control's custom image.
	BOOL m_bWrap;                   // TRUE if the control is wrapped.
	BOOL m_bBeginGroup;             // TRUE if the control starts new group.
	BOOL m_bEnabled;                // TRUE if the control is enabled.
	BOOL m_bChecked;                // TRUE if the control is checked.
	BOOL m_bSelected;               // TRUE if the control is selected.
	BOOL m_bPressed;                // TRUE if the control is pushed.
	BOOL m_bTemporary;              // TRUE if the control is temporary.
	CRect m_rcRow;                  // Bounding rectangle of the control's row.
	CRect m_rcControl;              // Bounding rectangle of the control.
	DWORD m_dwHideFlags;            // Hidden flags.
	DWORD m_dwFlags;                // Flags of the control.
	CString m_strCaption;           // Caption of the control.
	CString m_strCustomCaption;     // User defined caption.
	CString m_strShortcutText;      // Shortcut text.
	CString m_strShortcutTextAuto;  // Shortcut text.
	CString m_strTooltipText;       // Tooltip text.
	CString m_strDescriptionText;   // Description text.
	CString m_strParameter;         // Parameter text.
	CXTPControls* m_pControls;      // Parent control collection
	XTPControlType m_controlType;   // Type of the control.
	CXTPCommandBar* m_pParent;      // The Parent command bar.
	BOOL m_bExpanded;               // TRUE if the control is expanded.

	CString m_strCategory;          // Category of the control.
	BOOL m_bDefaultItem;            // TRUE if the item is default popup item;

	CDocTemplateMap m_mapDocTemplatesAssigned;  // Assigned templates.
	CDocTemplateMap m_mapDocTemplatesExcluded;  // Excluded templates.

	XTPButtonStyle m_buttonStyle;   // Button Style.
	XTPButtonStyle m_buttonCustomStyle;         // User defined style of button.
	XTPButtonStyle m_buttonRibbonStyle;         // User defined style of button.

	BOOL m_bCloseSubMenuOnClick;        // TRUE is sub-menus are closed as soon as a control is clicked.
	CXTPRibbonGroup* m_pRibbonGroup;    // Ribbon group of control

	CXTPControlAction* m_pAction;       // Action of the control.
	int m_nWidth;                       // Width of the control.
	int m_nHeight;                      // Height of the control.

	int m_nExecuteOnPressInterval;      // Delay between each message when control is pressed.


private:

	DECLARE_XTP_CONTROL(CXTPControl)

	friend class CXTPControls;
	friend class CXTPCommandBar;
	friend class CXTPToolBar;
	friend class CXTPPopupBar;
	friend class CXTPCustomizeSheet;
	friend class CXTPCustomizeDropSource;
	friend class CXTPRibbonGroup;
	friend class CXTPRibbonGroups;
	friend class CXTPRibbonBar;
	friend class CXTPRibbonControls;
	friend class CXTPControlAction;
};

//////////////////////////////////////////////////////////////////////////


AFX_INLINE long CXTPControl::GetIndex() const {
	return m_nIndex;
}
AFX_INLINE void CXTPControl::SetShortcutText(LPCTSTR lpszShortcutText) {
	m_strShortcutText = lpszShortcutText;
}
AFX_INLINE CString CXTPControl::GetShortcutText() const {
	return !m_strShortcutText.IsEmpty() ? m_strShortcutText :
	m_pAction && !m_pAction->GetShortcutText().IsEmpty()  ? m_pAction->GetShortcutText() : m_strShortcutTextAuto;
}
AFX_INLINE void CXTPControl::SetDescription(LPCTSTR lpszDescription) {
	m_strDescriptionText = lpszDescription;
}
AFX_INLINE CString CXTPControl::GetDescription() const {
	return !m_strDescriptionText.IsEmpty() ? m_strDescriptionText : m_pAction ? m_pAction->GetDescription() : _T("");
}
AFX_INLINE CString CXTPControl::GetTooltip(LPPOINT /*pPoint = 0*/, LPRECT /*lpRectTip = 0*/, INT_PTR* /*nHit = 0*/) const {
	return !m_strTooltipText.IsEmpty() ? m_strTooltipText : m_pAction ? m_pAction->GetTooltip() : _T("");
}
AFX_INLINE void CXTPControl::SetTooltip(LPCTSTR lpszTooltip) {
	m_strTooltipText = lpszTooltip;
}
AFX_INLINE CString CXTPControl::GetCaption() const {
	return !m_strCustomCaption.IsEmpty() ? m_strCustomCaption : !m_strCaption.IsEmpty() ? m_strCaption : m_pAction ? m_pAction->GetCaption() : _T("");
}
AFX_INLINE void CXTPControl::SetParameter(LPCTSTR lpszParameter) {
	m_strParameter = lpszParameter;
}
AFX_INLINE CString CXTPControl::GetParameter() const {
	return m_strParameter;
}
AFX_INLINE int CXTPControl::GetID () const {
	return m_nId;
}
AFX_INLINE void CXTPControl::SetIconId(int nId) {
	if (m_nIconId != nId) {m_nIconId = nId; RedrawParent();}
}
AFX_INLINE int CXTPControl::GetIconId() const {
	return m_nCustomIconId != 0 ? m_nCustomIconId : m_nIconId > 0 ? m_nIconId : m_pAction ? m_pAction->GetIconId() : m_nId;
}
AFX_INLINE void CXTPControl::SetHelpId(int nId) {
	m_nHelpId = nId;
}
AFX_INLINE int CXTPControl::GetHelpId() const {
	return m_nHelpId > 0 ? m_nHelpId : m_pAction ? m_pAction->GetHelpId() : m_nId;
}
AFX_INLINE void CXTPControl::SetBeginGroup(BOOL bBeginGroup) {
	if (m_bBeginGroup != bBeginGroup) {m_bBeginGroup = bBeginGroup; DelayLayoutParent();}
}
AFX_INLINE BOOL CXTPControl::GetBeginGroup() const {
	return m_bBeginGroup;
}
AFX_INLINE BOOL CXTPControl::GetEnabled() const {
	return m_bEnabled == -1 && m_pAction ? m_pAction->GetEnabled() : m_bEnabled;
}
AFX_INLINE void CXTPControl::SetEnabled(BOOL bEnabled) {
	if (m_bEnabled != bEnabled) {m_bEnabled = bEnabled; RedrawParent();}
}
AFX_INLINE BOOL CXTPControl::GetChecked() const {
	return m_bChecked == -1 && m_pAction ? m_pAction->GetChecked() : m_bChecked;
}
AFX_INLINE void CXTPControl::SetChecked(BOOL bChecked) {
	if (m_bChecked != bChecked) {m_bChecked = bChecked; RedrawParent();}
}
AFX_INLINE int CXTPControl::GetSelected() const {
	return m_bSelected;
}
AFX_INLINE BOOL CXTPControl::GetPressed() const {
	return m_bPressed;
}
AFX_INLINE void CXTPControl::SetPressed(BOOL bPressed) {
	m_bPressed = bPressed;
}
AFX_INLINE void CXTPControl::SetRect(CRect rcControl) {
	m_rcControl = rcControl;
}
AFX_INLINE CRect CXTPControl::GetRect() const {
	return m_rcControl;
}
AFX_INLINE CXTPCommandBar* CXTPControl::GetParent() const {
	return m_pParent;
}
AFX_INLINE BOOL CXTPControl::IsVisible(DWORD dwSkipFlags) const {
	if (m_pAction && !m_pAction->IsVisible()) return FALSE;
	return (m_dwHideFlags & ~dwSkipFlags) == xtpNoHide;
}
AFX_INLINE void CXTPControl::SetHideFlags(DWORD dwFlags) {
	m_dwHideFlags = dwFlags;
}
AFX_INLINE BOOL CXTPControl::SetHideFlag(XTPControlHideFlags dwFlag, BOOL bHide) {
	DWORD dwHideFlags = m_dwHideFlags;
	if (bHide) SetHideFlags (m_dwHideFlags | dwFlag); else SetHideFlags(m_dwHideFlags & ~dwFlag);
	return dwHideFlags != m_dwHideFlags;
}

AFX_INLINE void CXTPControl::SetHideWrap(BOOL bHide) {
	SetHideFlag(xtpHideWrap, bHide);
}
AFX_INLINE DWORD CXTPControl::GetHideFlags() const {
	return m_dwHideFlags;
}
AFX_INLINE CRect CXTPControl::GetRowRect() const {
	return m_rcRow;
}
AFX_INLINE void CXTPControl::SetRowRect(CRect rcRow) {
	m_rcRow = rcRow;
}
AFX_INLINE BOOL CXTPControl::GetWrap() const {
	return m_bWrap;
}
AFX_INLINE void CXTPControl::SetWrap(BOOL bWrap) {
	m_bWrap = bWrap;
}
AFX_INLINE CXTPControls* CXTPControl::GetControls() const {
	return m_pControls;
}

AFX_INLINE BOOL CXTPControl::IsTemporary() const {
	return m_bTemporary;
}
AFX_INLINE XTPControlType CXTPControl::GetType() const {
	return m_controlType;
}
AFX_INLINE BOOL CXTPControl::GetExpanded() const {
	return m_bExpanded;
}
AFX_INLINE void CXTPControl::SetCategory(LPCTSTR lpszCategory) {
	m_strCategory = lpszCategory;
}
AFX_INLINE CString CXTPControl::GetCategory() const {
	return !m_strCategory.IsEmpty() ? m_strCategory : m_pAction ? m_pAction->GetCategory() : _T("");
}
AFX_INLINE void CXTPControl::SetTag(DWORD_PTR dwTag) {
	m_nTag = dwTag;
}
AFX_INLINE DWORD_PTR CXTPControl::GetTag() const {
	return m_nTag;
}
AFX_INLINE void CXTPControl::AssignDocTemplate(UINT nIDResource) {
	m_mapDocTemplatesAssigned.SetAt(nIDResource, TRUE);
}
AFX_INLINE void CXTPControl::ExcludeDocTemplate(UINT nIDResource) {
	m_mapDocTemplatesExcluded.SetAt(nIDResource, TRUE);
}
AFX_INLINE void CXTPControl::OnClick(BOOL /*bKeyboard = FALSE*/, CPoint /*pt = CPoint(0, 0)*/) {
}
AFX_INLINE void CXTPControl::SetStyle(XTPButtonStyle buttonStyle) {
	if (m_buttonStyle != buttonStyle)
	{
		m_buttonStyle = buttonStyle;
		DelayLayoutParent();
	}
}
AFX_INLINE CXTPCommandBar* CXTPControl::GetCommandBar() const {
	return NULL;
}
AFX_INLINE void CXTPControl::SetCloseSubMenuOnClick(BOOL bCloseOnClick) {
	m_bCloseSubMenuOnClick = bCloseOnClick;
}

AFX_INLINE BOOL CXTPControl::GetCloseSubMenuOnClick() const {
	return m_bCloseSubMenuOnClick;
}
AFX_INLINE int CXTPControl::GetCustomizeMinWidth() const {
	return 0;
}
AFX_INLINE void CXTPControl::SetWidth(int nWidth) {
	if (m_nWidth != nWidth) {
		m_nWidth = nWidth;
		DelayLayoutParent();
	}
}
AFX_INLINE void CXTPControl::SetHeight(int nHeight) {
	if (m_nHeight != nHeight) {
		m_nHeight = nHeight;
		DelayLayoutParent();
	}
}
AFX_INLINE BOOL CXTPControl::OnHookKeyDown(UINT /*nChar*/, LPARAM /*lParam*/) {
	return FALSE;
}
AFX_INLINE BOOL CXTPControl::OnSetPopup(BOOL /*bPopup*/) {
	return FALSE;
}
AFX_INLINE void CXTPControl::OnLButtonUp(CPoint /*point*/) {
}
AFX_INLINE BOOL CXTPControl::OnRButtonDown(CPoint /*point*/) {
	return FALSE;
}
AFX_INLINE void CXTPControl::OnRButtonUp(CPoint /*point*/) {
}
AFX_INLINE void CXTPControl::OnMouseMove(CPoint /*point*/) {
}
AFX_INLINE BOOL CXTPControl::IsFocusable() const {
	return FALSE;
}
AFX_INLINE void CXTPControl::SetFocused(BOOL /*bFocused*/) {
}
AFX_INLINE BOOL CXTPControl::IsFocused() const{
	return FALSE;
}
AFX_INLINE void CXTPControl::OnCalcDynamicSize(DWORD /*dwMode*/) {
}

AFX_INLINE CXTPRibbonGroup* CXTPControl::GetRibbonGroup() const {
	return m_pRibbonGroup;
}
AFX_INLINE BOOL CXTPControl::OnHookMouseWheel(UINT /*nFlags*/, short /*zDelta*/, CPoint /*pt*/) {
	return FALSE;
}
AFX_INLINE BOOL CXTPControl::GetPopuped() const {
	return FALSE;
}
AFX_INLINE void CXTPControl::OnIdleUpdate() {

}
AFX_INLINE CXTPControlAction* CXTPControl::GetAction() const {
	return m_pAction;
}
AFX_INLINE void CXTPControl::OnThemeChanged() {

}
AFX_INLINE int CXTPControl::GetWidth() const {
	return m_nWidth;
}
AFX_INLINE int CXTPControl::GetHeight() const {
	return m_nHeight;
}
AFX_INLINE void CXTPControl::OnActionChanged(int /*nProperty*/) {

}
AFX_INLINE void CXTPControl::OnActionChanging(int /*nProperty*/) {

}
AFX_INLINE void CXTPControl::SetExecuteOnPressInterval(int nExecuteOnPressInterval) {
	m_nExecuteOnPressInterval = nExecuteOnPressInterval;
}
AFX_INLINE int CXTPControl::GetExecuteOnPressInterval() const {
	return m_nExecuteOnPressInterval;
}


#endif // #if !defined(__XTPCONTROL_H__)
