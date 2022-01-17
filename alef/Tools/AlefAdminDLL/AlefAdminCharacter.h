#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "AlefAdminCharMacro.h"
#include "AlefAdminWC.h"
#include "AlefAdminQuest.h"

// AlefAdminCharacter 폼 뷰입니다.


class AlefAdminCharacter : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminCharacter)

public:
	CXTPPropertyGrid* m_pcsCharGrid;

	AlefAdminWC* m_pWCView;
	AlefAdminCharMacro* m_pCharMacroView;
	AlefAdminQuest* m_pCharQuest;

	stAgpdAdminCharData m_stCharData;
	stAgpdAdminCharDataSub m_stCharDataSub;

protected:
	AlefAdminCharacter();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminCharacter();

public:
	enum { IDD = IDD_CHARACTER };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* msg);

	BOOL InitCharData();
	BOOL InitCharDataSub();

	BOOL InitCharGrid();
	BOOL InitCharGridData();

	BOOL InitSubTab();
	BOOL CreateSubViews();
	BOOL AddSubTabViews();

	BOOL OnReceive(AgpdCharacter* pcsCharacter);
	BOOL OnReceive(stAgpdAdminCharData* pstCharData);
	BOOL OnReceiveBasic(stAgpdAdminCharDataBasic* pstBasic);
	BOOL OnReceiveStat(stAgpdAdminCharDataStat* pstStat);
	BOOL OnReceiveStatus(stAgpdAdminCharDataStatus* pstStatus);
	BOOL OnReceiveMoney(stAgpdAdminCharDataMoney* pstMoney);
	BOOL OnReceiveSub(stAgpdAdminCharDataSub* pstCharDataSub);

	BOOL SetCharData(AgpdCharacter* pcsCharacter);
	BOOL SetCharData(stAgpdAdminCharData* pstCharData);
	BOOL SetCharDataBasic(stAgpdAdminCharDataBasic* pstBasic);
	BOOL SetCharDataStat(stAgpdAdminCharDataStat* pstStat);
	BOOL SetCharDataStatus(stAgpdAdminCharDataStatus* pstStatus);
	BOOL SetCharDataMoney(stAgpdAdminCharDataMoney* pstMoney);
	BOOL SetCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub);

	BOOL ShowCharData();
	BOOL ShowCharDataBasic();
	BOOL ShowCharDataStat();
	BOOL ShowCharDataStatus();
	BOOL ShowCharDataMoney();
	BOOL ShowCharDataSub();
	
	BOOL SetFormatType1(CString& szValue, INT32 lCurrentValue, INT32 lOriginalValue);
	BOOL SetFormatType2(CString& szValue, INT32 lCurrentValue, INT32 lOriginalValue);
	BOOL SetFormatType3(CString& szValue, INT32 lValue1, INT32 lValue2, INT32 lValue3);

	BOOL EditChar(INT32 lCID, LPCTSTR szCharName, LPCTSTR szEditField, LPCTSTR szNewValue);
	BOOL EditCharToDB(LPCTSTR szCharName, LPCTSTR szEditField, LPCTSTR szNewValue);
	BOOL OnReceiveEditResult(stAgpdAdminCharEdit* pstCharEdit);

	static BOOL CBReceiveCharGuildData(PVOID pData, PVOID pClass, PVOID pCustData);
	BOOL SetGuildID(LPCTSTR szGuildID);
	BOOL SetServerName(LPCTSTR szServerName);

	LPCTSTR GetAccName() { return m_stCharData.m_stSub.m_szAccName; }
	BOOL IsOnline() { return (BOOL)m_stCharData.m_stBasic.m_lCID; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	LRESULT OnGridNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCharGridItemValueChanged(CXTPPropertyGridItem* pItem);

	DECLARE_MESSAGE_MAP()

public:
	CStatic m_csCharGridPlace;
	CTabCtrlEx m_csSubTab;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


