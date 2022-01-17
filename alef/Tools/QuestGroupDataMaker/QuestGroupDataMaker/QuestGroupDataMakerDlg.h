// QuestGroupDataMakerDlg.h : 헤더 파일
//

#pragma once

#include "hashmapnpc.h"
#include "hashmaptemplate.h"
//#include "HashMapObject.h"

using namespace std;
class AuExcelTxtLib;

struct QuestNode
{
	BOOL	bUse;
	INT32	lGiveCount;
	INT32	lConfirmCount;
};

const int MAX_NODE_COUNT = 10000;

// CQuestGroupDataMakerDlg 대화 상자
class CQuestGroupDataMakerDlg : public CDialog
{
// 생성
public:
	CQuestGroupDataMakerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_QUESTGROUPDATAMAKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	typedef hash_map<INT32, CString> HashMapString;
	typedef HashMapString::iterator IterHashMapCheckPoint;

public:
	CHashMapQuestGroup	m_HashMapQuestGroup;
	HashMapTemplate		m_HashMapTemplate;
	HashMapString		m_HashMapCheckPoint;

	CString				m_strPathTemplateData;
	CString				m_strPathNPCData;
	CString				m_strPathObjectData;
	QuestNode			m_vtQusetNode[MAX_NODE_COUNT];

public:
	BOOL GetFilePath(CString &rResult);
	void MakeData();
	void WriteFile();
	void CheckStringLength(AuExcelTxtLib* pExcelTxt, INT32 lTID, INT32 lColumn, INT32 lRow, INT32 lMaxLength);

	void Check_QuestData();
	//void Check_NPCInfo();	// 퀘스트 부여자, 평가자 정보 확인
	//void Check_QuestNode();	// 퀘스트 노드 확인

	afx_msg void OnBnClickedLoadTemplateData();
	afx_msg void OnBnClickedLoadNpcData();
	afx_msg void OnBnClickedMakeQuestGroupData();
	afx_msg void OnBnClickedValidationCheck();
	afx_msg void OnBnClickedClearData();
	afx_msg void OnBnClickedLoadObjectData();
};
