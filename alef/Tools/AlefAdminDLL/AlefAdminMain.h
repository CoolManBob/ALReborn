#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "afxwin.h"

#include "AgpmAdmin.h"
#include "AgpmChatting.h"

#include "AlefAdminCharacter.h"
#include "AlefAdminItem.h"
#include "AlefAdminSkill.h"
#include "AlefAdminNotice.h"
#include "AlefAdminGuild.h"
#include "AlefAdminUser.h"
#include "AlefAdminOption.h"
#include "AlefAdminTitle.h"
#include "AlefAdminAPI.h"

// AlefAdminMain 대화 상자입니다.

class AlefAdminMain : public CXTResizeDialog
//class AlefAdminMain : public CDialog
{
	DECLARE_DYNAMIC(AlefAdminMain)

public:
	AlefAdminCharacter*			m_pCharView;
	AlefAdminItem*				m_pItemView;
	AlefAdminSkill*				m_pSkillView;
	AlefAdminNotice*			m_pNoticeView;
	AlefAdminGuild*				m_pGuildView;
	AlefAdminUser*				m_pUserView;
	AlefAdminTitle*				m_pTitleView;
	AlefAdminOption				m_cOptionDlg;
	

	ADMIN_CB m_pfCBSearch;
	stAgpdAdminSearch m_stLastSearch;
	
	// 캐릭터 하나를 선택하면 여기다가 저장한다.
	CHAR m_szCharName[AGPACHARACTER_MAX_ID_STRING+1];

	// Macro
	BOOL m_bSOS;

public:
	AlefAdminMain(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~AlefAdminMain();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAIN };

// 내가 만든 함수들
	BOOL SetCBSearch(ADMIN_CB pfCBSearch);

	BOOL Create();
	BOOL OpenDlg();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL IsDialogMessage(LPMSG lpMsg);

	BOOL InitSearchTypeCB();
	BOOL InitSearchResultList();

	BOOL InitMainTab();
	BOOL CreateViews();
	BOOL AddTabViews();

	BOOL InitMessageTab();
	BOOL CreateMessageViews();
	BOOL AddMessageTabViews();

	// Search
	BOOL SearchCharacter(LPCTSTR szCharName);

	BOOL OnReceiveSearchResult(stAgpdAdminSearchResult* pstResult);
	BOOL ClearSearchResultList();
	BOOL SetCharName(LPCTSTR szCharName);

	BOOL ResetAllCharData();

	BOOL IsSearchCharacter(CHAR* szCharName);

	// Message
	BOOL ProcessChatMessage(LPCTSTR szMessage);
	BOOL ProcessSystemMessage(LPCTSTR szMessage);
	BOOL ProcessAdminMessage(LPCTSTR szMessage);
	BOOL ProcessActionMessage(LPCTSTR szMessage);

	BOOL SaveWhisperToFile(AgpdChatData* pstChatData);

	// Current User
	BOOL OnReceiveCurrentUser(INT32 lCurrentUser);

	// Callback Function

	// From AgcmTargetting
	static BOOL CBOnLButtonDownPicking(PVOID pData, PVOID pClass, PVOID pCustData);

	// From AgcmChatting2
	static BOOL CBOnRecvChatting(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CTabCtrlEx			m_csMainTab;
	CXTButton			m_csSearchBtn;
	CXTFlatComboBox		m_csSearchTypeCB;
	CXTListCtrl			m_csSearchResult;
	CXTFlatHeaderCtrl	m_flatHeader;
	CEdit				m_csSearchEdit;

	CTabCtrlEx			m_csMessageTab;
	CEdit				m_csMessageChat;
	CEdit				m_csMessageSystem;
	CEdit				m_csMessageAdmin;
	CEdit				m_csMessageAction;

	CButton				m_csSOSBtn;

	afx_msg void OnBnClickedBSearch();
	afx_msg void OnNMDblclkLcSearchResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnOption();
	afx_msg void OnBnClickedChSos();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnAbout();
};
