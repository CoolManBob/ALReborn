#pragma once


// 2005.04.04. steeple
// 해당 캐릭터에게 빠른 매크로를 적용시키기 위해서 캐릭터용 매크로를 만듬.

#include "Resource.h"
#include "afxwin.h"

#include "AlefAdminXTExtension.h"

// AlefAdminCharMacro 폼 뷰입니다.

class AlefAdminCharMacro : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminCharMacro)

protected:
	AlefAdminCharMacro();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminCharMacro();

public:
	enum { IDD = IDD_CHARMACRO };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	vector<CAlefAdminMacro*> m_vectorMacro;

public:
	virtual void OnInitialUpdate();

	BOOL LoadMacro(LPCTSTR szFileName);
	BOOL SetMacroFunction(CAlefAdminMacro* pcsMacro);
	BOOL ClearMacro();
	BOOL SaveMacro();

	BOOL ShowMacro();

	BOOL ExecuteMacro(CAlefAdminMacro* pcsMacro);
	BOOL ExecuteMacroCommand(CAlefAdminMacro* pcsMacro);

	// Macro Function
	static BOOL MF_SOS(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_Disconnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_AllSkillLearn(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL MF_AllSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_csMacroList;
	CXTButton m_csExcuteBtn;
	CXTButton m_csLoadBtn;
	CXTButton m_csAddBtn;
	CXTButton m_csRemoveBtn;
	CXTButton m_csSaveBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBLoad();
	afx_msg void OnBnClickedBRunMacro();
	afx_msg void OnDestroy();
};


