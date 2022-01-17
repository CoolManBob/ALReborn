#pragma once

class CDlg_NewAnimItem : public CDialog
{
public:
	struct stMultUV
	{
		UINT tdst;
		UINT row;
		UINT col;
		UINT bgn;
		UINT end;
	};

public:
	CDlg_NewAnimItem(AgcdEffRenderBase*	pEffRenderBase, AgcdEffAnim* pEffAnim, CWnd* pParent = NULL);   // standard constructor

	//AFX_DATA
	enum { IDD = IDD_DLG_NEWANIMITEM };
	UINT		m_ulTime;
	CString		m_strVal;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel()		{	CDialog::OnCancel();	}

	// Message
	afx_msg void OnBtnDlg();
	afx_msg void OnChangeEditTime();
	afx_msg void OnChangeEditVal();
	DECLARE_MESSAGE_MAP()

private:
	void			vSetInitText();
	void			vGetValue();
	void			vToString();

public:
	UINT			bGetTime()const		{	return m_ulTime;	}
	LPVOID			bGetPtrVal();
	const stMultUV&	bGetStMultUV()const	{	return m_multiUV;	}
	BOOL			bGetMultUV()const	{	return m_bMultiUV;	}

private:
	AgcdEffRenderBase*	m_pEffRenderBase;
	AgcdEffAnim*		m_pEffAnim;

	RwRGBA			m_rgba;
	BOOL			m_bMultiUV;
	STUVRECT		m_rcTuTv;
	stMultUV		m_multiUV;
	RwV3d			m_v3dPos;
	AgcdEffAnim_Rev::STREVOLUTION	m_stRev;
	RwReal			m_angle;
	RwV3d			m_v3dScale;
};
