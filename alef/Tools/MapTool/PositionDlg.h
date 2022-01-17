#pragma once
#include "afxwin.h"


// CPositionDlg dialog

class CPositionDlg : public CDialog
{
	DECLARE_DYNAMIC(CPositionDlg)

public:
	CPositionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPositionDlg();

// Dialog Data
	enum { IDD = IDD_NPC_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_fX;
	float m_fY;
	float m_fZ;
	float m_fDegreeX;
	float m_fDegreeY;
};
