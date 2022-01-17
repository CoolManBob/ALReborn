#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "AgcmUIManager2.h"

// UITWPAnimationDialog dialog

class UITWPAnimationDialog : public CPropertyPage
{
	DECLARE_DYNAMIC(UITWPAnimationDialog)

private:
	AcUIBase *		m_pcsBase;

public:
	UITWPAnimationDialog();
	virtual ~UITWPAnimationDialog();

// Dialog Data
	enum { IDD = IDD_UIT_WP_ANIMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_csAnimDataList;
	CComboBox m_csTexture;
	UINT32 m_ulClockCount;
	BOOL m_bRenderString;
	BOOL m_bRenderBaseTexture;
	afx_msg void OnBnClickedUitAnimAdd();
	afx_msg void OnBnClickedUitAnimRemove();
	afx_msg void OnBnClickedUitAnimUpdate();

	BOOL OnSetActive();

	BOOL UpdateDataToDialog(AcUIBase *pcsBase, AgcdUI *pcsUI, AgcdUIControl *pcsControl);
	BOOL UpdateDataFromDialog();
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedUitAnimList(NMHDR *pNMHDR, LRESULT *pResult);

	AcUIBaseAnimation *	GetSelectedAnimData();
	VOID RefreshAnimation();
	afx_msg void OnBnClickedUitAnimStart();
	afx_msg void OnBnClickedUitAnimStop();
	BOOL m_bAnimationDefault;
	afx_msg void OnBnClickedUitAnimDefault();
};
