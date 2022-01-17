#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "EventButton.h"

#define TFTB_NAME_SET_OFFSET			"Offset"
#define TFTB_NAME_SET_RENDER_OPTION		"Render option"
#define TFTB_NAME_SET_ANIMATION			"Animation"
#define TFTB_NAME_RESET_VIEW			"Reset view"

typedef enum
{
	TFTB_SET_OFFSET				= 1,
	TFTB_SET_RENDER_OPTION		= 2,
	TFTB_SET_ANIMATION			= 4,
	TFTB_RESET_VIEW				= 8
} eToolFormButtonFlag;

class CToolForm : public CFormView
{
protected:
	CToolForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CToolForm)

public:
	CToolForm(CWnd *pcsParent, RECT &stInitRect, HBRUSH hBackColor = NULL);

	VOID InitToolButton(INT16 nXTemp, INT16 nYTemp, INT16 nWidth, INT16 nHeight);
	VOID SetPropertyButton(INT32 lFlag);

protected:
	VOID InitializeMember();
	BOOL CreateMember();

	VOID AddToolButton(CEventButton *pcsButton);
	VOID HideAllToolButton();

	// callback
	static BOOL PressSetOffsetButton(PVOID pClass);
	static BOOL PressSetRenderOptionButton(PVOID pClass);
	static BOOL PressSetAnimationButton(PVOID pClass);
	static BOOL PressResetViewButton(PVOID pClass);

protected:
	HBRUSH			m_hBackBrush;

	INT16			m_nXTemp;
	INT16			m_nYTemp;
	INT16			m_nWidth;
	INT16			m_nHeight;

	INT16			m_nToolButtonIndex;

	// public
	CEventButton	*m_pcsOffset;
	CEventButton	*m_pcsRenderOption;
	CEventButton	*m_pcsAnimation;
	CEventButton	*m_pcsResetView;

// Form Data
public:
	//{{AFX_DATA(CToolForm)
	enum { IDD = IDD_TOOL_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolForm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolForm();
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CToolForm)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
