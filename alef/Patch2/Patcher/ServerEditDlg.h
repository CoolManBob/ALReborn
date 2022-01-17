#pragma once
#include "afxcmn.h"


// CServerEditDlg 대화 상자입니다.

class CServerEditDlg : public CDialog
{


	DECLARE_DYNAMIC(CServerEditDlg)

public:
	CServerEditDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerEditDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADD_EDIT_DIALOG };

	VOID					SetName( const CString& str )	{ m_NameData	=	str;	}
	const CString&			GetName( VOID )					{ return m_NameData;		}

	VOID					SetIP( CString& strIP )			{ m_IPData		=	strIP;	}
	CString&				GetIP( VOID )					{ return m_IPData;			}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CString				m_NameData;
	CString				m_IPData;

public:
	afx_msg void OnBnClickedOk();
};
