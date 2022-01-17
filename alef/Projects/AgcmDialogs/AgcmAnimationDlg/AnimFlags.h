#pragma once

#include "AgcdAnimation.h"

// CAnimFlags dialog

class CAnimFlags : public CDialog
{
	DECLARE_DYNAMIC(CAnimFlags)

public:
	CAnimFlags(AgcdAnimationFlag *pstFlags, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimFlags();

// Dialog Data
	enum { IDD = IDD_ANIM_FLAGS };

protected:
	AgcdAnimationFlag	*m_pstFlags;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
protected:
	BOOL m_bLoop;
	BOOL m_bBlend;
	BOOL m_bLink;
};
