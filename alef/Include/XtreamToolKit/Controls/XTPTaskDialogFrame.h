// XTPTaskDialogFrame.h: interface for the CXTPTaskDialogFrame class.
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
#if !defined(__XTPTASKFRAMEWND_H__)
#define __XTPTASKFRAMEWND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPTaskDialogClient.h"

/////////////////////////////////////////////////////////////////////////////
// CXTPTaskDialogFrame window

class _XTP_EXT_CLASS CXTPTaskDialogFrame : public CXTPTaskDialogClient
{
public:
	CXTPTaskDialogFrame(TASKDIALOGCONFIG* pConfig);
	virtual ~CXTPTaskDialogFrame();

public:
	virtual INT_PTR DoModal();

protected:
	virtual CString GetFrameTitle() const;
	virtual DWORD GetFrameStyle() const;

protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPTaskDialogFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPTaskDialogFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	LRESULT OnDialogSetElementText(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogClickButton(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogClickRadioButton(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogEnableButton(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogEnableRadioButton(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogClickVerificationButton(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogUpdateIcon(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogSetButtonElevationState(WPARAM wParam, LPARAM lParam);

	LRESULT OnDialogSetMarqueeProgressBar(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogSetProgressBarState(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogSetProgressBarRange(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogSetProgressBarPos(WPARAM wParam, LPARAM lParam);
	LRESULT OnDialogStartProgressBarMarquee(WPARAM wParam, LPARAM lParam);


protected:
	LPDLGTEMPLATE m_lpDlgTemplate;
	DWORD m_dwTickCount;

private:
	friend class CXTPTaskCmdLinkButton;
};


#endif // !defined(__XTPTASKFRAMEWND_H__)
