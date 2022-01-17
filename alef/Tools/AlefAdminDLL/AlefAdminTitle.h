#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class AlefAdminTitle :	public CFormView
{
	DECLARE_DYNCREATE(AlefAdminTitle)

protected:
	AlefAdminTitle();
	virtual ~AlefAdminTitle();

public:
	enum { IDD = IDD_CHARTITLE };

#ifdef _DEBUG
virtual void AssertValid() const;
virtual void Dump(CDumpContext& dc) const;
#endif

public:
	stAgpdAdminCharTitle m_stSelectedTitle;
	stAgpdAdminCharTitle m_stSelectedTitleQuest;
	stAgpdAdminCharTitle m_stSelectedHasTitle;


public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* msg);

	void InitTitleAllList();
	void InitTitleList();
	void InitTitleQuestList();

	void FillTitleAllList();

	void ClearTitleContent();
	void ClearTitleQuestContent();
	BOOL OnReceiveTitleInfo(stAgpdAdminCharTitle* pstTitle);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CXTListCtrl m_csTitleAllList;
	CXTListCtrl m_csTitleList;
	CXTListCtrl m_csTitleQuestList;

	CXTFlatHeaderCtrl m_flatHeader;
	CXTFlatHeaderCtrl m_flatHeader2;
	CXTFlatHeaderCtrl m_flatHeader3;

	CXTButton m_csTitleQuestAddBtn;
	CXTButton m_csTitleQuestEditBtn;
	CXTButton m_csTitleQuestCompleteBtn;
	CXTButton m_csTitleQuestDeleteBtn;
	CXTButton m_csTitleUseBtn;
	CXTButton m_csTitleDeleteBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	afx_msg void OnBnClickedBTitleQuestAdd();
	afx_msg void OnBnClickedBTitleQuestEdit();
	afx_msg void OnBnClickedBTitleQuestComplete();
	afx_msg void OnBnClickedBTitleQuestDelete();
	afx_msg void OnBnClickedBTitleUse();
	afx_msg void OnBnClickedBTitleDelete();

	afx_msg void OnNMClickLcTitle(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLcHasTitle(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLcTitleQuest(NMHDR *pNMHDR, LRESULT *pResult);
};
