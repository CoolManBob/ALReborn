#pragma once
#include "afxwin.h"

#include <list>
#include "afxcmn.h"
#include "ServerListCtrl.h"

#define SERVER_FILE_NAME		_T("ServerList.txt")


struct	sServerInfo
{
	CHAR	Name[ 256 ];
	DWORD	dwIPAddress;
};

// CServerSettingDlg 대화 상자입니다.

class CServerSettingDlg : public CDialog
{
private:
	typedef std::list< sServerInfo >				ServerInfoList;
	typedef std::list< sServerInfo >::iterator		ServerInfoListIter;


	DECLARE_DYNAMIC(CServerSettingDlg)

public:
	CServerSettingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerSettingDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVER_SETTING_DIALOG };

	virtual BOOL	OnInitDialog();
	VOID			RefreshList( VOID );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


protected:
	afx_msg void OnBnClickedOkButton();


	ServerInfoList			m_ServerList;
	CServerListCtrl			m_ListCtrl;

	afx_msg void	OnMenuAddserver();
	afx_msg void	OnMenuEditserver();
	afx_msg void	OnMenuDeleteserver();
};
