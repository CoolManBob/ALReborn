// XTPTaskDialog.h: interface for the CXTPTaskDialog class.
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
#if !defined(__XTPTASKDIALOG_H__)
#define __XTPTASKDIALOG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPPropExchangeXMLNode;

#include "XTPTaskDialogFrame.h"

//===========================================================================
// Summary:
//       The CXTPTaskDialog class creates, displays, and operates a task
//       dialog. A task dialog is similar to, while much more flexible than,
//       a basic message box. The task dialog contains application-defined
//       messages, title, verification check box, command links and push
//       buttons, plus any combination of predefined icons and push buttons.
//===========================================================================
class _XTP_EXT_CLASS CXTPTaskDialog : public CXTPTaskDialogFrame
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//       Constructs a CXTPTaskDialog object.
	//-----------------------------------------------------------------------
	CXTPTaskDialog(CWnd* pWndParent = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPTaskDialog object, handles cleanup and
	//     deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPTaskDialog();

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the window title for the task
	//       dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used for the
	//                 task dialog title.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If this parameter is NULL, the filename of the executable
	//       program is used.
	// See Also:
	//       SetWidth, EnableCancellation, EnableRelativePosition,
	//       EnableRtlLayout, EnableMinimize, OnDialogConstructed
	//-----------------------------------------------------------------------
	void SetWindowTitle(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the label for the verification
	//       checkbox to be displayed in the task dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used to label
	//                 the verification checkbox.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If this parameter is NULL, the verification checkbox is not
	//       displayed in the task dialog.
	// See Also:
	//       OnVerificationClicked, SetVerifyCheckState, IsVerificiationChecked,
	//       ClickVerification
	//-----------------------------------------------------------------------
	void SetVerificationText(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the expanded control text for the
	//       task dialog. This function is ignored unless SetExpandedInformation
	//       has been called.
	// Parameters:
	//       pszText - Pointer that references the string to be used to label
	//                 the button for collapsing the expandable information.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If this parameter is NULL, then the value set with
	//       SetCollapsedControlText will be used for the expanded control text
	//       as well.
	// See Also:
	//       SetCollapsedControlText, SetExpandedInformation, ExpandedByDefault,
	//       ExpandFooterArea, OnExpandoButtonClicked
	//-----------------------------------------------------------------------
	void SetExpandedControlText(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the collapsed control text for the
	//       task dialog. This function is ignored unless SetExpandedInformation
	//       has been called.
	// Parameters:
	//       pszText - Pointer that references the string to be used to label
	//                 the button for expanding the expandable information.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If this parameter is NULL, then the value set with
	//       SetExpandedControlText will be used for the collapsed control text
	//       as well.
	// See Also:
	//       SetExpandedControlText, SetExpandedInformation, ExpandedByDefault,
	//       ExpandFooterArea, OnExpandoButtonClicked
	//-----------------------------------------------------------------------
	void SetCollapsedControlText(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the main instruction text for the
	//       task dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used for the main
	//                 instruction.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro.
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetMainInstruction(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the content text for the task
	//       dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used for the
	//                 dialog's primary content.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If EnableHyperlinks has been called, then this string may
	//       contain hyperlinks in the form:
	// <code>
	// <A HREF="executablestring">Hyperlink Text</A>.
	// </code>
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	// See Also:
	//       OnHyperlinkClicked, SetExpandedInformation, SetFooter, EnableHyperlinks
	//-----------------------------------------------------------------------
	BOOL SetContent(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the expanded information text for
	//       the task dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used for displaying
	//                 additional information.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. The additional information is displayed either immediately
	//       below the content or below the footer text depending on whether
	//       ExpandFooterArea has been called. If EnableHyperlinks has been called,
	//       then this string may contain hyperlinks in the form:
	// <code>
	// <A HREF="executablestring">Hyperlink Text</A>.
	// </code>
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	// See Also:
	//       SetCollapsedControlText, SetExpandedControlText, ExpandedByDefault,
	//       ExpandFooterArea, OnExpandoButtonClicked,
	//       SetContent, OnHyperlinkClicked, SetFooter, EnableHyperlinks
	//-----------------------------------------------------------------------
	BOOL SetExpandedInformation(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the footer text for the task dialog.
	// Parameters:
	//       pszText - Pointer that references the string to be used in the footer
	//                 area of the task dialog.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro. If EnableHyperlinks has been called, then this string may
	//       contain hyperlinks in the form:
	// <code>
	// <A HREF="executablestring">Hyperlink Text</A>.
	// </code>
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	// See Also:
	//       SetContent, SetExpandedInformation, OnHyperlinkClicked, EnableHyperlinks
	//-----------------------------------------------------------------------
	BOOL SetFooter(LPCTSTR pszText);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the icon for the Main Instruction
	//       area of the task dialog.
	// Parameters:
	//       hIcon       - A handle to an Icon that is to be displayed in the task
	//                     dialog Main Instruction area. If this member is NULL, no
	//                     icon will be displayed.
	//       pszResource - Pointer that references the icon to be displayed in
	//                     the task dialog footer area. If this parameter is
	//                     NULL no icon will be displayed.
	// Remarks:
	//       The parameter <i>pszResource</i> must be an integer resource
	//       identifier passed to the MAKEINTRESOURCE macro, or one of the
	//       following predefined values:
	//
	//       * <b>TD_ERROR_ICON</b>       A stop-sign icon appears in the task
	//                                    dialog.
	//       * <b>TD_WARNING_ICON</b>     An exclamation-point icon appears in
	//                                    the task dialog.
	//       * <b>TD_INFORMATION_ICON</b> An icon consisting of a lowercase
	//                                    letter <b>i</b> in a circle appears
	//                                    in the task dialog.
	//       * <b>TD_SHIELD_ICON</b>      A shield icon appears in the task
	//                                    dialog.
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetMainIcon(HICON hIcon);
	BOOL SetMainIcon(LPCWSTR pszResource); //<COMBINE CXTPTaskDialog::SetMainIcon@HICON>

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to use system icons
	// Parameters:
	//       bUseSysIcons - TRUE to use system icons when available for TD_ERROR_ICON,
	//                      TD_WARNING_ICON, TD_INFORMATION_ICON and TD_SHIELD_ICON
	//                      instead of Codejock Software icons. This parameter is
	//                      ignored when <i>bUseComCtl32</i> is set to TRUE.
	//-----------------------------------------------------------------------
	void SetUseSysIcons(BOOL bUseSysIcons = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the icon for the Footer
	//       area of the task dialog.
	// Parameters:
	//       hIcon       - A handle to an Icon that is to be displayed in the task
	//                     dialog footer area. If this member is NULL, no icon will
	//                     be displayed.
	//       pszResource - Pointer that references the icon to be displayed in
	//                     the task dialog footer area. If this parameter is
	//                     NULL no icon will be displayed.
	// Remarks:
	//       The parameter <i>pszResource</i> must be an integer resource
	//       identifier passed to the MAKEINTRESOURCE macro, or one of the
	//       following predefined values:
	//
	//       * <b>TD_ERROR_ICON</b>       A stop-sign icon appears in the task
	//                                    dialog.
	//       * <b>TD_WARNING_ICON</b>     An exclamation-point icon appears in
	//                                    the task dialog.
	//       * <b>TD_INFORMATION_ICON</b> An icon consisting of a lowercase
	//                                    letter <b>i</b> in a circle appears
	//                                    in the task dialog.
	//       * <b>TD_SHIELD_ICON</b>      A shield icon appears in the task
	//                                    dialog.
	// Returns:
	//       TRUE if successful, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL SetFooterIcon(HICON hIcon);
	BOOL SetFooterIcon(LPCWSTR pszResource); //<COMBINE CXTPTaskDialog::SetFooterIcon@HICON>

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the width of the task dialog's
	//       client area.
	// Parameters:
	//       cxWidth     - Specifies the width of the task dialog's client area
	//                     in dialog units. If 0, task dialog will calculate the
	//                     ideal width.
	//       bPixelToDLU - TRUE indicates the size specified by cxWidth are pixel
	//                     units and need to be converted into dialog units (DLU).
	// See Also:
	//       SetWindowTitle, EnableCancellation, EnableRelativePosition,
	//       EnableRtlLayout, EnableMinimize, OnDialogConstructed
	//-----------------------------------------------------------------------
	void SetWidth(int cxWidth, BOOL bPixelToDLU = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to add a user defined button to the
	//       task dialog's command area.
	// Parameters:
	//       pszText   - Pointer that references the string to be used to label
	//                   the button.
	//       nButtonID - Indicates the value to be returned when this button
	//                   is selected.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro.  When using Command Links, you delineate the command from
	//       the note by placing a new line character in the string.
	// See Also:
	//       OnButtonClicked, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void AddButton(LPCTSTR pszText, int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to add a user defined radio button to the
	//       task dialog's command area.
	// Parameters:
	//       pszText   - Pointer that references the string to be used to label
	//                   the radio button.
	//       nButtonID - Indicates the value to be returned when this button
	//                   is selected.
	// Remarks:
	//       The parameter <i>pszText</i> can be either a null-terminated string
	//       or an integer resource identifier passed to the MAKEINTRESOURCE
	//       macro.
	// See Also:
	//       OnRadioButtonClicked, SetDefaultRadioButton, NoDefaultRadioButton,
	//       GetSelectedRadioButtonId, ClickRadioButton, EnableRadioButton
	//-----------------------------------------------------------------------
	void AddRadioButton(LPCTSTR pszText, int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to indicate the buttons specified with
	//       AddButton should be displayed as command links (using a standard
	//       task dialog glyph) instead of push buttons. This function is ignored
	//       if AddButton has not been called.
	// Parameters:
	//       bEnable - TRUE to enable command links, FALSE to disable.
	//       bShowIcon - TRUE if the command links should display a glyph.
	// Remarks:
	//       When using command links, all characters up to the first new line
	//       character in AddButton's pszText argument will be treated as the
	//       command link's main text, and the remainder will be treated as the
	//       command link's note.
	// See Also:
	//       AddButton, OnButtonClicked, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void EnableCommandLinks(BOOL bEnable = TRUE, BOOL bShowIcon = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Enables hyperlink processing for the strings specified for the
	//       functions SetContent, SetExpandedInformation and SetFooter.
	// Parameters:
	//       bEnable - TRUE to enable hyperlinks, FALSE to disable.
	// Remarks:
	//       When enabled, Content, Expanded Information and Footer strings
	//       can contain hyperlinks in the form:
	// <code>
	// <A HREF="executablestring">Hyperlink Text</A>.
	// </code>
	// See Also:
	//       SetContent, SetExpandedInformation, SetFooter, OnHyperlinkClicked
	//-----------------------------------------------------------------------
	void EnableHyperlinks(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to allow dialog cancellation for the
	//       task dialog.
	// Parameters:
	//       bEnable - TRUE to allow cancellation, otherwise FALSE.
	// Remarks:
	//       When <i>bAllow</i> is TRUE, this indicates that the dialog should
	//       be able to be closed using Alt-F4, Escape and the title bar's close
	//       button even if no cancel button is specified by the
	//       SetCommonButtons or AddButton member functions.
	// See Also:
	//       SetWidth, SetWindowTitle, EnableRelativePosition,
	//       EnableRtlLayout, EnableMinimize, OnDialogConstructed
	//-----------------------------------------------------------------------
	void EnableCancellation(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to position the task dialog relative to the
	//       parent window.
	// Parameters:
	//       bEnable - TRUE to reposition dialog, otherwise FALSE.
	// Remarks:
	//       When <i>bEnable</i> is TRUE, this indicates that the task dialog
	//       should be positioned (centered) relative to the window specified by
	//       the hWndParent argument in the DoModal member function. If bEnable
	//       is FALSE, or hWndParent is NULL, the task dialog is positioned
	//       (centered) relative to the monitor.
	// See Also:
	//       SetWidth, EnableCancellation, SetWindowTitle,
	//       EnableRtlLayout, EnableMinimize, OnDialogConstructed
	//-----------------------------------------------------------------------
	void EnableRelativePosition(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to display a Progress Bar for the
	//       task dialog.
	// Parameters:
	//       bShow    - TRUE indicates that a Progress Bar should be displayed.
	//       bMarquee - TRUE indicates the progress bar should be Marquee style.
	// See Also:
	//       SetMarqueeProgressBar, SetProgressBarState, SetProgressBarRange,
	//       SetProgressBarPos, StartProgressBarMarquee
	//-----------------------------------------------------------------------
	void ShowProgressBar(BOOL bShow, BOOL bMarquee = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to enable right to left text display for
	//       the task dialog.
	// Parameters:
	//       bEnable - TRUE indicates that text should be displayed reading
	//                 right to left.
	// See Also:
	//       SetWidth, EnableCancellation, EnableRelativePosition,
	//       SetWindowTitle, EnableMinimize, OnDialogConstructed
	//-----------------------------------------------------------------------
	void EnableRtlLayout(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to display the text specified by the
	//       SetExpandedInformation member function when the dialog is initially
	//       displayed. This function is ignored if the SetExpandedInformation
	//       member function has not been called.
	// Parameters:
	//       bExpanded - TRUE indicates that the string specified by the
	//                   SetExpandedInformation member function should be displayed
	//                   when the dialog is initially displayed.
	// See Also:
	//       SetCollapsedControlText, SetExpandedInformation, SetExpandedControlText,
	//       ExpandFooterArea, OnExpandoButtonClicked
	//-----------------------------------------------------------------------
	void ExpandedByDefault(BOOL bExpanded = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member to display expanded information at the bottom of
	//       the task dialog's footer. This function is ignored if the
	//       SetExpandedInformation member function has not been called.
	// Parameters:
	//       bExpandFooter - TRUE indicates that the string specified by the
	//                       SetExpandedInformation member should be displayed
	//                       at the bottom of the dialog's footer area instead
	//                       of immediately after the dialog's content.
	// See Also:
	//       SetCollapsedControlText, SetExpandedInformation, ExpandedByDefault,
	//       SetExpandedControlText, OnExpandoButtonClicked
	//-----------------------------------------------------------------------
	void ExpandFooterArea(BOOL bExpandFooter = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the default button ID for the
	//       task dialog.
	// Parameters:
	//       nButtonID - Indicates the default button for the dialog.
	// Remarks:
	//       nButtonID may be any of the values specified when calling the
	///      AddButton member function, or one of the IDs corresponding to the
	//       buttons specified in the SetCommonButtons member function:
	//
	//      * <b>IDCANCEL</b> Make the Cancel button the default.
	//      * <b>IDNO</b> Make the No button the default.
	//      * <b>IDOK</b> Make the OK button the default.
	//      * <b>IDRETRY</b> Make the Retry button the default.
	//      * <b>IDYES</b> Make the Yes button the default.
	//      * <b>IDCLOSE</b> Make the Close button the default.
	//
	//      If this member is zero or its value does not correspond to any button ID
	//      in the dialog, then the first button in the dialog will be the default.
	// See Also:
	//       AddButton, EnableCommandLinks, OnButtonClicked, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void SetDefaultButton(int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the default radio button ID for the
	//       task dialog.
	// Parameters:
	//       nButtonID - Indicates the default radio button for the dialog.
	// Remarks:
	//       nButtonID may be any of the values specified when calling the
	//       AddRadioButton member function. If this member is zero or its value
	//       does not correspond to any radio button ID in the dialog, then the
	//       first button in the dialog will be the default.
	// See Also:
	//       AddRadioButton, OnRadioButtonClicked, NoDefaultRadioButton,
	//       GetSelectedRadioButtonId, ClickRadioButton, EnableRadioButton
	//-----------------------------------------------------------------------
	void SetDefaultRadioButton(int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to disable default radio button selection.
	// Parameters:
	//       bNoDefault - TRUE indicates that no default item will be selected.
	// See Also:
	//       AddRadioButton, SetDefaultRadioButton, OnRadioButtonClicked,
	//       GetSelectedRadioButtonId, ClickRadioButton, EnableRadioButton
	//-----------------------------------------------------------------------
	void NoDefaultRadioButton(BOOL bNoDefault);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the checked state for the verification
	//       checkbox when the dialog is initially displayed. This function is
	//       ignored if the SetVerificationText member function has not been called.
	// Parameters:
	//       bChecked - TRUE indicates that the verification checkbox in the
	//                  dialog should be checked when the dialog is initially
	//                  displayed.
	// See Also:
	//       OnVerificationClicked, OnVerificationClicked, IsVerificiationChecked,
	//       ClickVerification
	//-----------------------------------------------------------------------
	void SetVerifyCheckState(BOOL bChecked = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to enable the callback timer.
	// Parameters:
	//       bEnable - TRUE indicates that the task dialog's callback should
	//                  be called approximately every 200 milliseconds.
	// See Also:
	//       OnTimer, ModifyTaskStyle
	//-----------------------------------------------------------------------
	void EnableCallbackTimer(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to enable the the task dialog to be minimized.
	// Parameters:
	//       bEnable - TRUE indicates that the task dialog's title bar and system
	//                 menu contain the minimize command.
	// See Also:
	//       SetWidth, EnableCancellation, EnableRelativePosition,
	//       EnableRtlLayout, SetWindowTitle, OnDialogConstructed
	//-----------------------------------------------------------------------
	void EnableMinimize(BOOL bEnable);

	//-----------------------------------------------------------------------
	// Summary:
	//       Specifies the push buttons displayed in the task dialog. If no
	//       common buttons are specified and no custom buttons are specified using
	//       the AddButton member function, the task dialog will contain the OK
	//       button by default.
	// Parameters:
	//       bOk       - TRUE if the task dialog contains the push button: <b>OK</b>.
	//       bYes      - TRUE if the task dialog contains the push button: <b>Yes</b>.
	//       bNo       - TRUE if the task dialog contains the push button: <b>No</b>.
	//       bCancel   - TRUE if the task dialog contains the push button: <b>Cancel</b>.
	//       bRetry    - TRUE if the task dialog contains the push button: <b>Retry</b>.
	//       bClose    - TRUE if the task dialog contains the push button: <b>Close</b>.
	//       dwButtons - Can be any combination of the flags listed in the
	//                   remarks section.
	// Remarks:
	//       Any of the following flags can be used with the dwButtons parameter:
	//
	//       * <b>TDCBF_OK_BUTTON<b>     The task dialog contains the push button: <b>OK</b>.
	//       * <b>TDCBF_YES_BUTTON<b>    The task dialog contains the push button: <b>Yes</b>.
	//       * <b>TDCBF_NO_BUTTON<b>     The task dialog contains the push button: <b>No</b>.
	//       * <b>TDCBF_CANCEL_BUTTON<b> The task dialog contains the push button: <b>Cancel</b>.
	//       * <b>TDCBF_RETRY_BUTTON<b>  The task dialog contains the push button: <b>Retry</b>.
	//       * <b>TDCBF_CLOSE_BUTTON<b>  The task dialog contains the push button: <b>Close</b>.
	//
	//       If the Cancel button is specified, the task dialog will respond to
	//       typical cancel actions (Alt-F4 and Escape).
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, OnButtonClicked,
	//       GetSelectedButtonId, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void SetCommonButtons(BOOL bOk, BOOL bYes, BOOL bNo, BOOL bCancel, BOOL bRetry, BOOL bClose);
	void SetCommonButtons(TASKDIALOG_COMMON_BUTTON_FLAGS dwButtons);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to display the task dialog and return
	//       the selected button when done.
	// Parameters:
	//       bUseComCtl32 - TRUE to use ComCtl32.dll version of Task Dialog. This
	//                      parameter is ignored for operating systems older than
	//                      Windows Vista.
	// Returns:
	//       The return value is one of the button IDs specified in the AddButton
	//       member function or one of the following values:
	//
	//       * <b>0</b>        Function call failed.
	//       * <b>IDCANCEL</b> Cancel button was selected, Alt-F4 was pressed, Escape
	//                         was pressed or the user clicked on the close window button.
	//       * <b>IDNO</b>     No button was selected.
	//       * <b>IDOK</b>     OK button was selected.
	//       * <b>IDRETRY</b>  Retry button was selected.
	//       * <b>IDYES</b>    Yes button was selected.
	// Example:
	// <code>
	// CXTPTaskDialog taskDlg(this);
	// taskDlg.AddButton(_T("Change password"), IDOK);
	// taskDlg.SetCommonButtons(TDCBF_CANCEL_BUTTON);
	// taskDlg.SetMainIcon(TD_WARNING_ICON);
	// taskDlg.SetMainInstruction(_T("Change Password"));
	// taskDlg.SetContent(_T("Remember your changed password."));
	//
	// int nRet = taskDlg.DoModal();
	// switch (nRet)
	// {
	// case 0:
	//    AfxMessageBox(_T("Task Dialog could not be created!"));
	//    break;
	// case IDOK:
	//    // the user pressed the OK button, change password.
	//    break;
	// case IDCANCEL:
	//    // user canceled the dialog.
	//    break;
	// };
	// </code>
	//-----------------------------------------------------------------------
	INT_PTR DoModal(BOOL bUseComCtl32 = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to retrieve the ID of the button used
	//       to close the task dialog.
	// Returns:
	//       The return value is one of the button IDs specified in the AddButton
	//       member function or one of the following values:
	//
	//       * <b>0</b>        Function call failed.
	//       * <b>IDCANCEL</b> Cancel button was selected, Alt-F4 was pressed, Escape
	//                         was pressed or the user clicked on the close window button.
	//       * <b>IDNO</b>     No button was selected.
	//       * <b>IDOK</b>     OK button was selected.
	//       * <b>IDRETRY</b>  Retry button was selected.
	//       * <b>IDYES</b>    Yes button was selected.
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       OnButtonClicked, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	int GetSelectedButtonId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to retrieve the ID of the radio button
	//       that was selected when the task dialog was closed.
	// Returns:
	//       The return value is one of the button IDs specified in the
	//       AddRadioButton member function.
	// See Also:
	//       AddRadioButton, SetDefaultRadioButton, NoDefaultRadioButton,
	//       OnRadioButtonClicked, ClickRadioButton, EnableRadioButton
	//-----------------------------------------------------------------------
	int GetSelectedRadioButtonId() const;

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to retrieve the checked state of the
	//       verification checkbox when the dialog was closed.
	// Returns:
	//       TRUE if the verification check box was checked when the task
	//       dialog was closed, otherwise false.
	// See Also:
	//       SetVerificationText, SetVerifyCheckState, OnVerificationClicked,
	//       ClickVerification
	//-----------------------------------------------------------------------
	BOOL IsVerificiationChecked() const;

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to modify the style for the task dialog.
	// Parameters:
	//       dwRemove - Specifies the styles to be removed.
	//       dwAdd    - Specifies the styles to be added.
	// Remarks:
	//       Styles to be added or removed can be combined by using the bitwise
	//       OR (|) operator. The style can be any combination of the following
	//       values:
	//
	//       * <b>TDF_ENABLE_HYPERLINKS</b> See EnableHyperlinks.
	//       * <b>TDF_USE_HICON_MAIN</b> See SetMainIcon.
	//       * <b>TDF_USE_HICON_FOOTER</b> See SetFooterIcon.
	//       * <b>TDF_ALLOW_DIALOG_CANCELLATION</b> See EnableCancellation.
	//       * <b>TDF_USE_COMMAND_LINKS</b> See EnableCommandLinks.
	//       * <b>TDF_USE_COMMAND_LINKS_NO_ICON</b> See EnableCommandLinks.
	//       * <b>TDF_EXPAND_FOOTER_AREA</b> See ExpandFooterArea.
	//       * <b>TDF_EXPANDED_BY_DEFAULT</b> See ExpandedByDefault.
	//       * <b>TDF_VERIFICATION_FLAG_CHECKED</b> See SetVerifyCheckState.
	//       * <b>TDF_SHOW_PROGRESS_BAR</b> See ShowProgressBar.
	//       * <b>TDF_SHOW_MARQUEE_PROGRESS_BAR</b> See ShowProgressBar.
	//       * <b>TDF_CALLBACK_TIMER</b> See EnableCallbackTimer.
	//       * <b>TDF_POSITION_RELATIVE_TO_WINDOW</b> See EnableRelativePosition.
	//       * <b>TDF_RTL_LAYOUT</b> See EnableRtlLayout.
	//       * <b>TDF_NO_DEFAULT_RADIO_BUTTON</b> See NoDefaultRadioButton.
	//       * <b>TDF_CAN_BE_MINIMIZED</b> See EnableMinimize.
	// Returns:
	//       TRUE if style was successfully modified; otherwise, FALSE.
	//-----------------------------------------------------------------------
	BOOL ModifyTaskStyle(TASKDIALOG_FLAGS dwRemove, TASKDIALOG_FLAGS dwAdd);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to simulate the action of a button click
	//       in the task dialog.
	// Parameters:
	//       nButtonID - Indicates the button ID to be selected.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CSimulateClickDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          BOOL bClick = TRUE;
	//          if (bClick)
	//          {
	//              ClickButton(IDCANCEL);
	//          }
	//     }
	// };
	// </code>
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, OnButtonClicked, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void ClickButton(int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to simulate the action of a radio button click
	//       in the task dialog.
	// Parameters:
	//       nButtonID - Indicates the radio button ID to be selected.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CSimulateClickDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          BOOL bClick = TRUE;
	//          if (bClick)
	//          {
	//              ClickRadioButton(100);
	//          }
	//     }
	// };
	// </code>
	// See Also:
	//       AddRadioButton, SetDefaultRadioButton, NoDefaultRadioButton,
	//       GetSelectedRadioButtonId, OnRadioButtonClicked, EnableRadioButton
	//-----------------------------------------------------------------------
	void ClickRadioButton(int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to simulate the action of clicking the
	//       verification checkbox in the task dialog.
	// Parameters:
	//       bChecked     - TRUE to set the state of the checkbox to be checked,
	//                      and FALSE to set it to be unchecked.
	//       bSetKeyFocus - TRUE to set the keyboard focus to the checkbox, and
	//                      FALSE otherwise.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CSimulateClickDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          BOOL bClick = TRUE;
	//          if (bClick)
	//          {
	//              ClickVerification(TRUE, FALSE);
	//          }
	//     }
	// };
	// </code>
	// See Also:
	//       SetVerificationText, SetVerifyCheckState, IsVerificiationChecked,
	//       OnVerificationClicked
	//-----------------------------------------------------------------------
	void ClickVerification(BOOL bChecked, BOOL bSetKeyFocus);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to enable/disable a push button in
	//       the task dialog.
	// Parameters:
	//       nButtonID - Indicates the ID of the push button to be enabled/disabled.
	//       bEnable   - FALSE to disable the button, TRUE to enable the button.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CEnableButtonDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          EnableButton(IDOK, FALSE);
	//     }
	// };
	// </code>
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, OnButtonClicked, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	void EnableButton(int nButtonID, BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to enable/disable a radio button in
	//       the task dialog.
	// Parameters:
	//       nButtonID - Indicates the ID of the radio button to be enabled/disabled.
	//       bEnable   - FALSE to disable the radio button, TRUE to enable the radio button.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CEnableRadioButtonDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          EnableRadioButton(100, FALSE);
	//     }
	// };
	// </code>
	// See Also:
	//       AddRadioButton, SetDefaultRadioButton, NoDefaultRadioButton,
	//       GetSelectedRadioButtonId, ClickRadioButton, OnRadioButtonClicked
	//-----------------------------------------------------------------------
	void EnableRadioButton(int nButtonID, BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to indicate whether the progress bar should
	//       be displayed in marquee mode or not.
	// Parameters:
	//       bMarquee - Specifies whether the progress bar should be shown in
	//                  Marquee mode. A value of TRUE turns on Marquee mode.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CProgressDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          SetMarqueeProgressBar(TRUE);
	//          SetProgressBarState(PBST_NORMAL);
	//     }
	// };
	// </code>
	// See Also:
	//       ShowProgressBar, SetProgressBarState, SetProgressBarRange,
	//       SetProgressBarPos, StartProgressBarMarquee
	//-----------------------------------------------------------------------
	void SetMarqueeProgressBar(BOOL bMarquee = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the current state of the
	//       progress bar.
	// Parameters:
	//       nState - Specifies the bar state. The parameter can be one of the
	//                following values:
	//
	//                * <b>PBST_NORMAL</b> Sets the progress bar to the normal state.
	//                * <b>PBST_PAUSE</b> Sets the progress bar to the paused state.
	//                * <b>PBST_ERROR</b> Set the progress bar to the error state.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CProgressDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          SetProgressBarPos(25);
	//          SetProgressBarRange(0, 100);
	//          SetProgressBarState(PBST_PAUSE);
	//     }
	// };
	// </code>
	// See Also:
	//       SetMarqueeProgressBar, ShowProgressBar, SetProgressBarRange,
	//       SetProgressBarPos, StartProgressBarMarquee
	//-----------------------------------------------------------------------
	void SetProgressBarState(int nState);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to set the minimum and maximum values
	//       for the progress bar.
	// Parameters:
	//       wMinRange - Minimum range value. By default, the minimum value is zero.
	//       wMaxRange - Maximum range value. By default, the maximum value is 100.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CProgressDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          SetProgressBarPos(25);
	//          SetProgressBarRange(0, 100);
	//          SetProgressBarState(PBST_PAUSE);
	//     }
	// };
	// </code>
	// See Also:
	//       SetMarqueeProgressBar, SetProgressBarState, ShowProgressBar,
	//       SetProgressBarPos, StartProgressBarMarquee
	//-----------------------------------------------------------------------
	void SetProgressBarRange(WORD wMinRange = 0, WORD wMaxRange = 100);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to se the current position for the
	//       progress bar.
	// Parameters:
	//       nPosition - Signed integer that becomes the new position.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CProgressDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          SetProgressBarPos(25);
	//          SetProgressBarRange(0, 100);
	//          SetProgressBarState(PBST_PAUSE);
	//     }
	// };
	// </code>
	// See Also:
	//       SetMarqueeProgressBar, SetProgressBarState, SetProgressBarRange,
	//       ShowProgressBar, StartProgressBarMarquee
	//-----------------------------------------------------------------------
	void SetProgressBarPos(int nPosition);

	//-----------------------------------------------------------------------
	// Summary:
	//       Similar to SetMarqueeProgressBar, this member function is used to
	//       indicate whether the progress bar should be displayed in marquee
	//       mode or not with the ability to determine marquee speed as well.
	// Parameters:
	//       bStartMarquee  - Indicates whether to start marquee.
	//       dwMilliSeconds - Indicates the speed of the marquee in milliseconds.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CProgressDlg : public CXTPTaskDialog
	// {
	// protected:
	//     virtual void OnDialogConstructed()
	//     {
	//          StartProgressBarMarquee(TRUE, 10);
	//          SetProgressBarState(PBST_NORMAL);
	//     }
	// };
	// </code>
	// See Also:
	//       SetMarqueeProgressBar, SetProgressBarState, SetProgressBarRange,
	//       SetProgressBarPos, ShowProgressBar
	//-----------------------------------------------------------------------
	void StartProgressBarMarquee(BOOL bStartMarquee, DWORD dwMilliSeconds);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to designate whether a given Task Dialog
	//       button or command link should have a User Account Control (UAC)
	//       shield icon (whether the action invoked by the button requires
	//       elevation).
	// Parameters:
	//       nButtonID - Indicates ID of the push button or command link to be updated.
	//       bRequired - FALSE to designate that the action invoked by the button does
	//                   not require elevation; TRUE to designate that the action does
	//                   require elevation.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CElevationDlg : public CXTPTaskDialog
	// {
	// protected:
	//     void OnButtonClicked(int nButtonID, BOOL& bCloseDialog)
	//     {
	//          switch (nButtonID)
	//          {
	//          case IDOK:
	//              SetButtonElevationRequired(nButtonID, TRUE);
	//              bCloseDialog = FALSE;
	//              break;
	//          case IDCANCEL:
	//              SetButtonElevationRequired(nButtonID, FALSE);
	//              bCloseDialog = TRUE;
	//              break;
	//          }
	//     }
	// };
	// </code>
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, EnableButton, OnButtonClicked
	//-----------------------------------------------------------------------
	void SetButtonElevationRequired(int nButtonID, BOOL bRequired = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       This member function is used to simulate a wizard style task dialog.
	//       By using multiple CXTPTaskDialog objects that define the aspects
	//       of each page, you can incorporate Back and Next buttons that can
	//       be used to update the appearance and behavior of the task dialog.
	// Parameters:
	//       taskDlg - Reference to a CXTPTaskDialog object that contains
	//                 attributes for the appearance and behavior of the new
	//                 task page.
	// Remarks:
	//       This member function should only be called from a derived class,
	//       and only after the task dialog has been initialized.
	// Example:
	// <code>
	// class CNavigateDlg : public CXTPTaskDialog
	// {
	//     CXTPTaskDialog m_dlgBack;
	//     CXTPTaskDialog m_dlgNext;
	// protected:
	//     void OnButtonClicked(int nButtonID, BOOL& bCloseDialog)
	//     {
	//          switch (nButtonID)
	//          {
	//          case IDBACK:
	//              NavigatePage(m_dlgBack);
	//              bCloseDialog = FALSE;
	//              break;
	//          case IDNEXT:
	//              NavigatePage(m_dlgNext);
	//              bCloseDialog = FALSE;
	//              break;
	//          }
	//     }
	// };
	// </code>
	// See Also:
	//     OnNavigated
	//-----------------------------------------------------------------------
	void NavigatePage(CXTPTaskDialog& taskDlg);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this member function to reset the configuration attributes for
	//       the task dialog to the default state.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this method to show thed dialog even if standard window theme used
	//-----------------------------------------------------------------------
	void EnableMessageBoxStyle(BOOL bEnable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//       Call this method to create dialog from XML resource.
	// Parameters:
	//       hInstance   - Instance of resources
	//       nIDResource - Resource identifier
	//       lpszDialog  - Dialog xml section
	//-----------------------------------------------------------------------
	BOOL CreateFromResource(UINT nIDResource, LPCTSTR lpszDialog);
	BOOL CreateFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR lpszDialog); //<COMBINE CXTPTaskDialog::CreateFromResource@UINT@LPCTSTR>
	BOOL CreateFromResourceParam(HINSTANCE hInstance, UINT nIDResource, LPCTSTR lpszDialog, LPCTSTR const* rglpsz, int nString);    //<COMBINE CXTPTaskDialog::CreateFromResource@UINT@LPCTSTR>
	BOOL CreateFromResourceParam(CXTPPropExchangeXMLNode* pPX, LPCTSTR const* rglpsz, int nString);     //<COMBINE CXTPTaskDialog::CreateFromResource@UINT@LPCTSTR>


protected:

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog once the task dialog
	//       has been created and before it is displayed.
	// See Also:
	//       SetWidth, EnableCancellation, EnableRelativePosition,
	//       EnableRtlLayout, EnableMinimize, SetWindowTitle
	//-----------------------------------------------------------------------
	virtual void OnDialogConstructed();

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog once the task dialog
	//       has been destroyed.
	// See Also:
	//       OnDialogConstructed
	//-----------------------------------------------------------------------
	virtual void OnDialogDestroyed();

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when a user clicks
	//       on a hyperlink in the Task Dialog's content
	// Parameters:
	//       pszURL - Pointer to a wide-character string containing the URL
	//                of the hyperlink.
	// See Also:
	//       SetContent, SetExpandedInformation, SetFooter, EnableHyperlinks
	//-----------------------------------------------------------------------
	virtual void OnHyperlinkClicked(LPCTSTR pszURL);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when a user selects
	//       a button or command link in the task dialog.
	// Parameters:
	//       nButtonID    - The ID corresponding to the button selected.
	//       bCloseDialog - Set to TRUE if the task dialog should close.
	// See Also:
	//       AddButton, EnableCommandLinks, SetDefaultButton, SetCommonButtons,
	//       GetSelectedButtonId, ClickButton, EnableButton, SetButtonElevationRequired
	//-----------------------------------------------------------------------
	virtual void OnButtonClicked(int nButtonID, BOOL& bCloseDialog);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when a user selects
	//       a radio button in the task dialog.
	// Parameters:
	//       nButtonID    - The ID corresponding to the radio button that was clicked.
	// See Also:
	//       AddRadioButton, SetDefaultRadioButton, NoDefaultRadioButton,
	//       GetSelectedRadioButtonId, ClickRadioButton, EnableRadioButton
	//-----------------------------------------------------------------------
	virtual void OnRadioButtonClicked(int nButtonID);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when the user clicks
	//       on the task dialog's verification check box.
	// Parameters:
	//       bChecked - TRUE if the verification checkbox is checked, otherwise FALSE.
	// See Also:
	//       SetVerificationText, SetVerifyCheckState, IsVerificiationChecked,
	//       ClickVerification
	//-----------------------------------------------------------------------
	virtual void OnVerificationClicked(BOOL bChecked);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when the user presses
	//       F1 on the keyboard while the dialog has focus.
	//-----------------------------------------------------------------------
	virtual void OnHelp();

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when the user clicks
	//       on an expando in the Task Dialog.
	// Parameters:
	//       bExpanded - TRUE if the dialog is expanded, otherwise FALSE.
	// See Also:
	//       SetCollapsedControlText, SetExpandedInformation, ExpandedByDefault,
	//       ExpandFooterArea, SetExpandedControlText
	//-----------------------------------------------------------------------
	virtual void OnExpandoButtonClicked(BOOL bExpanded);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog approximately every
	//       200 milliseconds when the EnableCallbackTimer member function has
	//       been called.
	// Parameters:
	//       dwMilliSeconds - Number of milliseconds since the dialog was created
	//                        or bReset was set to TRUE.
	//       bReset         - TRUE to reset the tickcount, otherwise the tickcount
	//                        will continue to increment.
	// See Also:
	//       EnableCallbackTimer, ModifyTaskStyle
	//-----------------------------------------------------------------------
	virtual void OnTimer(DWORD dwMilliSeconds, BOOL& bReset);

	//-----------------------------------------------------------------------
	// Summary:
	//       This notification is sent by the Task Dialog when a navigation
	//       has occurred.
	// See Also:
	//       NavigatePage
	//-----------------------------------------------------------------------
	virtual void OnNavigated();

	virtual HRESULT OnDialogNotify(UINT uNotification, WPARAM wParam, LPARAM lParam);


protected:

	int                                          m_nSelButtonID;      // ID of the button that was selected when the task dialog was closed.
	HWND                                         m_hwndTaskDialog;              //
	TASKDIALOGCONFIG                             m_config;            // Structure contains information used to display a task dialog
	CArray<TASKDIALOG_BUTTON,TASKDIALOG_BUTTON&> m_arrButtons;         // Array of TASKDIALOG_BUTTON structures containing the definition of the custom buttons that are to be displayed in the dialog.
	CArray<TASKDIALOG_BUTTON,TASKDIALOG_BUTTON&> m_arrRadioButtons;    // Array of TASKDIALOG_BUTTON structures containing the definition of the custom radio buttons that are to be displayed in the dialog.

private:

	BOOL OnAttach(HWND hWndNew);
	HWND OnDetach();

	static HRESULT CALLBACK TaskDialogCallbackProc(
		HWND hwnd,
		UINT uNotification,
		WPARAM wParam,
		LPARAM lParam,
		DWORD_PTR dwRefData);
};

AFX_INLINE void CXTPTaskDialog::OnDialogConstructed() {

}
AFX_INLINE void CXTPTaskDialog::OnDialogDestroyed() {

}
AFX_INLINE void CXTPTaskDialog::OnHyperlinkClicked(LPCTSTR pszURL) {
	UNREFERENCED_PARAMETER(pszURL);
}
AFX_INLINE void CXTPTaskDialog::OnButtonClicked(int nButtonID, BOOL& bCloseDialog) {
	UNREFERENCED_PARAMETER(nButtonID); bCloseDialog = TRUE;
}
AFX_INLINE void CXTPTaskDialog::OnRadioButtonClicked(int nButtonID) {
	UNREFERENCED_PARAMETER(nButtonID);
}
AFX_INLINE void CXTPTaskDialog::OnVerificationClicked(BOOL bChecked) {
	UNREFERENCED_PARAMETER(bChecked);
}
AFX_INLINE void CXTPTaskDialog::OnHelp() {

}
AFX_INLINE void CXTPTaskDialog::OnExpandoButtonClicked(BOOL bExpanded) {
	UNREFERENCED_PARAMETER(bExpanded);
}
AFX_INLINE void CXTPTaskDialog::OnTimer(DWORD dwMilliSeconds, BOOL& bReset) {
	UNREFERENCED_PARAMETER(dwMilliSeconds); UNREFERENCED_PARAMETER(bReset);
}
AFX_INLINE void CXTPTaskDialog::OnNavigated() {

}
AFX_INLINE int CXTPTaskDialog::GetSelectedButtonId() const {
	return m_nSelButtonID;
}
AFX_INLINE int CXTPTaskDialog::GetSelectedRadioButtonId() const {
	return m_nSelRadioButtonID;
}
AFX_INLINE BOOL CXTPTaskDialog::IsVerificiationChecked() const {
	return m_bVerification;
}
AFX_INLINE void CXTPTaskDialog::ClickButton(int nButtonID) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_CLICK_BUTTON, nButtonID, 0);
}
AFX_INLINE void CXTPTaskDialog::ClickRadioButton(int nButtonID) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_CLICK_RADIO_BUTTON, nButtonID, 0);
}
AFX_INLINE void CXTPTaskDialog::ClickVerification(BOOL bChecked, BOOL bSetKeyFocus) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_CLICK_VERIFICATION, bChecked, bSetKeyFocus);
}
AFX_INLINE void CXTPTaskDialog::EnableButton(int nButtonID, BOOL bEnable) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_ENABLE_BUTTON, nButtonID, bEnable);
}
AFX_INLINE void CXTPTaskDialog::EnableRadioButton(int nButtonID, BOOL bEnable) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_ENABLE_RADIO_BUTTON, nButtonID, bEnable);
}
AFX_INLINE void CXTPTaskDialog::SetMarqueeProgressBar(BOOL bMarquee) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_MARQUEE_PROGRESS_BAR, bMarquee, 0);
}
AFX_INLINE void CXTPTaskDialog::StartProgressBarMarquee(BOOL bStartMarquee, DWORD dwMilliSeconds) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_PROGRESS_BAR_MARQUEE, bStartMarquee, dwMilliSeconds);
}
AFX_INLINE void CXTPTaskDialog::SetProgressBarState(int nState) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_PROGRESS_BAR_STATE, nState, 0);
}
AFX_INLINE void CXTPTaskDialog::SetProgressBarPos(int nPosition) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_PROGRESS_BAR_POS, nPosition, 0);
}
AFX_INLINE void CXTPTaskDialog::SetProgressBarRange(WORD wMinRange, WORD wMaxRange) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_PROGRESS_BAR_RANGE, 0, MAKELPARAM(wMinRange, wMaxRange));
}
AFX_INLINE void CXTPTaskDialog::SetButtonElevationRequired(int nButtonID, BOOL bRequired) {
	ASSERT(::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE, nButtonID, bRequired);
}
AFX_INLINE void CXTPTaskDialog::NavigatePage(CXTPTaskDialog& taskDlg) {
	ASSERT(taskDlg.m_hwndTaskDialog == NULL && ::IsWindow(m_hwndTaskDialog)); ::SendMessage(m_hwndTaskDialog, TDM_NAVIGATE_PAGE, 0, (LPARAM)&taskDlg.m_config);
}
AFX_INLINE void CXTPTaskDialog::SetUseSysIcons(BOOL bUseSysIcons) {
	m_bUseSysIcons = bUseSysIcons;
}
AFX_INLINE void CXTPTaskDialog::EnableMessageBoxStyle(BOOL bEnable) {
	m_bMessageBoxStyle = bEnable;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__XTPTASKDIALOG_H__)
