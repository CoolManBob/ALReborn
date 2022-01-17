#pragma once
#include "afxwin.h"

// CDefaultHeadRenderTypeDlg dialog

class CDefaultHeadRenderTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CDefaultHeadRenderTypeDlg)

public:
	enum
	{
		STATE_NONE,
		STATE_FACE,
		STATE_HAIR,
	};

	void	SetState( int nState );

private:
	int		m_nState;

public:
	CDefaultHeadRenderTypeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDefaultHeadRenderTypeDlg();

// Dialog Data
	enum { IDD = IDD_DEFAULT_HEAD_RENDER_TYPE };
	CComboBox	m_ctlRenderTypeCombo;
	CComboBox	m_ctlBlendModeCombo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
};
