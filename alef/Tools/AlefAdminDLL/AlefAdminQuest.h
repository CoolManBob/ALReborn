#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "AgpdAdmin.h"
#include "AgpmQuest.h"

#include "AlefAdminXTExtension.h"

//캐릭터 퀘스트 정보를 관리.

class AlefAdminQuest : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminQuest)

protected:
	AlefAdminQuest();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminQuest();

public:
	enum { IDD = IDD_QUEST };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();

	void ClearContent();
	BOOL OnReceiveQuestInfo(stAgpdAdminCharQuest* pstQuest);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_csQuestList;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};
