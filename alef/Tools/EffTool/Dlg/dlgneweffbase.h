#pragma once

#include "Dlg_EffBase.h"
#include "Dlg_EffRenderBase.h"

class CDlgNewEffBase : public CDialog
{
// Construction
public:
	CDlgNewEffBase(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DLG_NEWEFFBASE };
	int			m_nEffBaseType;
	BOOL		m_bDummyObj;
	CString		m_strFName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg void OnRadioBoard();
	afx_msg void OnRadioLight();
	afx_msg void OnRadioMfrm();
	afx_msg void OnRadioObj();
	afx_msg void OnRadioPsys();
	afx_msg void OnRadioPsysSbh();
	afx_msg void OnRadioSound();
	afx_msg void OnRadioTail();
	afx_msg void OnBnClickedRadioTerrainb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void	vLayOutWnd(void);

public:
	CDlg_EffBase		m_dlgEffBase;
	CDlg_EffRenderBase	m_dlgEffRenderBase;

private:
	CRect				m_rcThis;
	CRect				m_rcOk;
	CRect				m_rcCancel;
};
