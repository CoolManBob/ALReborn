#pragma once

class AgcmEffectList : public CDialog
{
public:
	AgcmEffectList(UINT32 *pulIndex, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(AgcmEffectList)
	enum { IDD = IDD_EFFECT_LIST };
	CListCtrl	m_csEffectList;
	//}}AFX_DATA

protected:
	UINT32*	m_pulIndex;
	UINT32*	m_pulEID;

public:
	VOID	EnableApplyButton(BOOL bEnable = TRUE);
	VOID	UpdateEffectIndex(UINT32 *plIndex);

protected:
	BOOL LoadEffectList();
	VOID _ToUpper(CHAR *szDest, CHAR *szSrc);
	BOOL IsLowEqual(CHAR *szCmp1, CHAR *szCmp2);
	VOID SetEffectList(CHAR *szCmp);

public:
	virtual BOOL Create(CWnd* pParentWnd = NULL);

public:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	//{{AFX_MSG(AgcmEffectList)
	afx_msg void OnDblclkEffectList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnButtonApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};