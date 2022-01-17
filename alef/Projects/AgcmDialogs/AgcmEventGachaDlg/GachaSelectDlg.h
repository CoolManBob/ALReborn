#pragma once

#include <map>
#include "afxwin.h"

using namespace std;

class	AgcmEventGachaDlg;

// CGachaSelectDlg 대화 상자입니다.

class CGachaSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CGachaSelectDlg)

public:
	AgcmEventGachaDlg	* m_pParentModule;
	CGachaSelectDlg( AgcmEventGachaDlg * pParent , INT32 nIndex = 0 );   // 표준 생성자입니다.
	virtual ~CGachaSelectDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GACHA };

	INT32	m_nIndex;
	map< INT32	, AgpdGachaType	>	* m_pMap;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listGacha;
	CString m_strSelectedGacha;
public:
	afx_msg void OnLbnSelchangeListGacha();
};
