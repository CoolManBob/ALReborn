#pragma once
#include "afxwin.h"
#include "afxcmn.h"

///////////////////////////////////////////////////
//
// 2005.04.28. steeple
// 아 왜 XTPPropertyGrid 가 안보이지 -_-;;;
// 그래서 그냥 ListView 로 변경하려고 함. ㅜㅜ



// AlefAdminGuild 폼 뷰입니다.

class AlefAdminGuild : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminGuild)

protected:
	AlefAdminGuild();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~AlefAdminGuild();

public:
	enum { IDD = IDD_GUILD };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//CXTPPropertyGrid* m_pGuildGrid;
	INT32 m_lLastSearchType;
	CHAR m_szLastSearchField[255];
	
	CHAR m_szSearchedGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1];
	CHAR m_szSelectedMemberID[AGPACHARACTER_MAX_ID_STRING + 1];
	INT32 m_lSelectedMemberRank;

	INT32 m_lTotalGuildCount;
	INT32 m_lTotalMemberCount;

public:
	virtual void OnInitialUpdate();

	BOOL InitComboBox();
	BOOL InitGrid();
	BOOL InitListView();

	BOOL IsSearchGuild(CHAR* szGuildID, CHAR* szMasterID = NULL);
	BOOL InitGridData();

	BOOL ShowGuildData(AgpdGuild* pcsGuild);
	BOOL ShowMemberData(AgpdGuild* pcsGuild, CHAR* szMemberID);

	BOOL UpdateMemberInfo(AgpdGuild* pcsGuild);
	BOOL UpdatePassword(CHAR* szPassword);

	INT32 GetCurrentGuildID(CString& szGuildID);
	INT32 GetGuildDataIndex(const CHAR* szField);

	BOOL SendForcedMemberLeave(const CHAR* szGuildID, const CHAR* szMemberID);

	// Guild 서부터는 콜백을 여기서 직접해본다.
	// From AgpmGuild
	static BOOL CBGuildCreate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildJoin(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReceiveGuildOperation(PVOID pData, PVOID pClass, PVOID pCustData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	CXTFlatComboBox m_csSearchType;
	CXTButton m_csSearchBtn;
	CXTButton m_csRefreshBtn;
	CStatic m_csGuildDataPlace;
	CXTListCtrl m_csGuildDataLV;
	CXTFlatHeaderCtrl m_flatHeader;
	CXTListCtrl m_csMemberLV;
	CXTFlatHeaderCtrl m_flatHeader2;
	CXTButton m_csGuildDestroyBtn;
	CXTButton m_csMemberForcedLeaveBtn;
	CXTButton m_csChangeMasterBtn;
	CXTButton m_csChangeNoticeBtn;

	afx_msg void OnBnClickedBGuildSearch();
	afx_msg void OnBnClickedBGuildInfoRefresh();
	afx_msg void OnNMClickLvGuild(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickLvMember(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBGuildDestroy();
	afx_msg void OnBnClickedBMemberForcedLeave();
	afx_msg void OnBnClickedBChangeMaster();
	afx_msg void OnBnClickedBChangeNotice();
};


