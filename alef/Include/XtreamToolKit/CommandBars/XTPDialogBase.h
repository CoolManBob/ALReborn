// XTPFrameWnd.h : interface for the CXTPFrameWnd and CXTPMDIFrameWnd classes.
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
#if !defined(__XTPDIALOGBASE_H__)
#define __XTPDIALOGBASE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


//===========================================================================
// Summary:
//     CXTPDialogBase is a TBase derived class. It represents the parent
//     class for a  CXTPDialog class .
//===========================================================================
template <class TBase>
class CXTPDialogBase : public TBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDialog object
	// Parameters:
	//     nIDTemplate - Contains the ID number of a dialog-template resource
	//     lpszTemplateName - Contains a null-terminated string that is
	//                        the name of a dialog-template resource
	//     pParentWnd  - Points to the parent of the DialogBase control
	//-----------------------------------------------------------------------
	CXTPDialogBase()
	{
		m_pCommandBars = 0;
	}
	CXTPDialogBase(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		: TBase(nIDTemplate, pParentWnd)
	{
		m_pCommandBars = 0;
	}
	//<combine CXTPDialogBase::CXTPDialogBase>
	CXTPDialogBase(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		: TBase(lpszTemplateName, pParentWnd)
	{
		m_pCommandBars = 0;
	} //<combine CXTPDialogBase::CXTPDialogBase>

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDialogBase object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	~CXTPDialogBase()
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->InternalRelease();
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates command bars.
	// Parameters:
	//     pCommandBarsClass - Custom runtime class of CommandBars. It can be used if you want to override
	//                         some methods of CXTPCommandBars class.
	// Returns:
	//     Nonzero if successful; otherwise 0.
	//-----------------------------------------------------------------------
	BOOL InitCommandBars(CRuntimeClass* pCommandBarsClass = RUNTIME_CLASS(CXTPCommandBars))
	{
		ASSERT(pCommandBarsClass->IsDerivedFrom(RUNTIME_CLASS(CXTPCommandBars)));
		m_pCommandBars =  (CXTPCommandBars*) pCommandBarsClass->CreateObject();
		ASSERT(m_pCommandBars);
		m_pCommandBars->SetSite(this);
		m_pCommandBars->EnableDocking();
		return TRUE;
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function will re-dock a toolbar specified by 'pBarToDock'
	//     to the right of a newly docked toolbar specified by 'pBarOnLeft'.
	// Parameters:
	//     pBarToDock - A CXTPToolBar pointer to the toolbar to be docked.
	//     pBarOnLeft - A CXTPToolBar pointer to the already docked toolbar.
	//-----------------------------------------------------------------------
	void DockRightOf(CXTPToolBar* pBarToDock, CXTPToolBar* pBarOnLeft)
	{
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, AFX_IDW_PANE_FIRST);

		CXTPWindowRect rcBar(pBarOnLeft);

		if (IsVerticalPosition(pBarOnLeft->GetPosition())) rcBar.OffsetRect(0, rcBar.Height());
			else rcBar.OffsetRect(rcBar.Width(), 0);

		GetCommandBars()->DockCommandBar(pBarToDock, rcBar, pBarOnLeft->GetDockBar());
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to save the state information to the registry
	//     or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that specifies
	//                       the name of a section in the initialization file
	//                       or a key in the Windows registry where state
	//                       information is stored.
	//-----------------------------------------------------------------------
	void SaveCommandBars(LPCTSTR lpszProfileName)
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->SaveOptions(lpszProfileName);
			m_pCommandBars->SaveBarState(lpszProfileName);
			m_pCommandBars->GetShortcutManager()->SaveShortcuts(lpszProfileName);
		}
	}

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this function to retrieve state information from the registry
	//     or .INI file.
	// Parameters:
	//     lpszProfileName - Points to a null-terminated string that specifies
	//                       the name of a section in the initialization file
	//                       or a key in the Windows registry where state
	//                       information is stored.
	//     bSilent         - TRUE to disable user notifications when command bars are restore to their original state.
	//-----------------------------------------------------------------------
	void LoadCommandBars(LPCTSTR lpszProfileName, BOOL bSilent = FALSE)
	{
		if (m_pCommandBars)
		{
			m_pCommandBars->LoadOptions(lpszProfileName);
			m_pCommandBars->LoadBarState(lpszProfileName, bSilent);
			m_pCommandBars->GetShortcutManager()->LoadShortcuts(lpszProfileName);
		}
	}

	//----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve a pointer to the CommandBars object.
	// Returns:
	//     Retrieves Command Bars object.
	//----------------------------------------------------------------------
	CXTPCommandBars* GetCommandBars() const { return m_pCommandBars; }

//{{AFX_CODEJOCK_PRIVATE
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if ((pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
			&& (pMsg->wParam != VK_RETURN && pMsg->wParam != VK_TAB && pMsg->wParam != VK_ESCAPE))
		{
			CWnd* pWnd = CWnd::GetFocus();
			if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CXTPEdit)))
				return FALSE;
		}

		if (m_pCommandBars && m_pCommandBars->PreTranslateFrameMessage(pMsg))
			return TRUE;

		if (TBase::PreTranslateMessage(pMsg))
			return TRUE;

		return FALSE;
	}

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
	{
		if (m_pCommandBars && m_pCommandBars->OnFrameWndMsg(message, wParam, lParam, pResult))
			return TRUE;

		return TBase::OnWndMsg(message, wParam, lParam, pResult);
	}

//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPCommandBars* m_pCommandBars;
};

//===========================================================================
// Summary:
//     CXTPDialog is a CXTPDialogBase derived class. Use this class in your dialog base application.
//===========================================================================
class CXTPDialog : public CXTPDialogBase<CDialog>
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDialog object
	// Parameters:
	//     nIDTemplate - Contains the ID number of a dialog-template resource
	//     lpszTemplateName - Contains a null-terminated string that is
	//                        the name of a dialog-template resource
	//     pParentWnd  - Points to the parent of the Dialog control
	//-----------------------------------------------------------------------
	CXTPDialog()
	{

	}
	CXTPDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		: CXTPDialogBase<CDialog>(nIDTemplate, pParentWnd)
	{

	} //<combine CXTPDialog::CXTPDialog>
	CXTPDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		: CXTPDialogBase<CDialog>(lpszTemplateName, pParentWnd)
	{

	} //<combine CXTPDialog::CXTPDialog>

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDialog object
	//-----------------------------------------------------------------------

	//{{AFX_CODEJOCK_PRIVATE

	// deprecated
	void SetMenuBar(CXTPMenuBar* /*pMenuBar*/)
	{
		XT_ERROR_MSG(
			"WARNING: CXTPDialog::SetMenuBar(...) has been deprecated, use\n"\
			"CXTPCommandBars::SetMenu(...) instead, for example:\n\n"\
			"VERIFY(InitCommandBars());\n\n"\
			"CXTPCommandBars* pCommandBars = GetCommandBars();\n"\
			"pCommandBars->SetMenu(_T(\"Menu Bar\"), IDR_MENU);");
	}

	//}}AFX_CODEJOCK_PRIVATE
};

#endif // #if !defined(__XTPDIALOGBASE_H__)
