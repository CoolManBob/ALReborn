// XTPRibbonQuickAccessControls.h: interface for the CXTPRibbonQuickAccessControls class.
//
// This file is a part of the XTREME RIBBON MFC class library.
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
#if !defined(__XTPRIBBONQUICKACCESSCONTROLS_H__)
#define __XTPRIBBONQUICKACCESSCONTROLS_H__
//}}AFX_CODEJOCK_PRIVATE


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommandBars/XTPControl.h"
#include "CommandBars/XTPControls.h"

//===========================================================================
// Summary:
//     CXTPRibbonControls is a CXTPControls derived class. It represents a collection
//     of the controls for Ribbon Bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonControls : public CXTPControls
{
	DECLARE_DYNCREATE(CXTPRibbonControls)
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonControls object
	//-----------------------------------------------------------------------
	CXTPRibbonControls();

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is removed from collection
	// Parameters:
	//     pControl - control that removed from collection
	//-----------------------------------------------------------------------
	virtual void OnControlRemoved(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is about to be removing from collection
	// Parameters:
	//     pControl - control that removing from collection
	// Returns:
	//     TRUE to cancel removing.
	//-----------------------------------------------------------------------
	virtual BOOL OnControlRemoving(CXTPControl* pControl);
};

//===========================================================================
// Summary:
//     CXTPRibbonQuickAccessControls is a CXTPControls derived class.
//     It represents a collection of the controls for quick access controls for Ribbon Bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPRibbonQuickAccessControls : public CXTPControls
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPRibbonQuickAccessControls object
	//-----------------------------------------------------------------------
	CXTPRibbonQuickAccessControls();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPRibbonQuickAccessControls object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPRibbonQuickAccessControls();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove a control.
	// Parameters:
	//     pControl - Control to be removed.
	//-----------------------------------------------------------------------
	void Remove(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to reset quick access controls
	//-----------------------------------------------------------------------
	void Reset();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to find Duplicate control
	// Parameters:
	//     pControl - Control to find
	// Returns: Pointer to Duplicate control.
	//-----------------------------------------------------------------------
	CXTPControl* FindDuplicate(CXTPControl* pControl);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is added to collection
	// Parameters:
	//     pControl - control that added to collection
	virtual void OnControlAdded(CXTPControl* pControl);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called when control is removed from collection
	// Parameters:
	//     pControl - control that removed from collection
	//-----------------------------------------------------------------------
	virtual void OnControlRemoved(CXTPControl* pControl);

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual void RefreshIndexes();
//}}AFX_CODEJOCK_PRIVATE
};


#endif // !defined(__XTPRIBBONQUICKACCESSCONTROLS_H__)
