// PatchDataViwerDlg.h : 헤더 파일
//

#pragma once

#include "PatchCompress.h"
#include "AuPackingManager.h"
#include "afxwin.h"

// CPatchDataViwerDlg 대화 상자
class CPatchDataViwerDlg : public CDialog
{
	AuPackingManager		m_csAuPackingManager;
	CCompress				m_cCompress;

	char					m_strJZPSrcPath[255];
	char					m_strJZPDestPath[255];

	char					m_strPackingSrcPath[255];
	char					m_strPackingDestPath[255];

	char					m_strCompareSrcPath[255];
	char					m_strCompareDestPath[255];

	bool					ScanFolder( char *pstrFolder );
	bool					DepressJZP( char *pstrJZPFileName, char *pstrDestPath );
	bool					DepressDat( char *pstrSrcPath, char *pstrDestPath );

// 생성
public:
	CPatchDataViwerDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_PATCHDATAVIWER_DIALOG };

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
	CButton m_cPackingExtractSubDir;
	CButton m_cCompareSubDir;
	afx_msg void OnBnClickedButtonJzpextractorScanSrcfile();
	afx_msg void OnBnClickedButtonJzpextractorScanDestpath();
	afx_msg void OnBnClickedButtonJzpextractor();
	afx_msg void OnBnClickedButtonPackingextractorScanSrcpath();
	afx_msg void OnBnClickedButtonPackingextractorScanDestpath();
	afx_msg void OnBnClickedButtonPackingextractor();
	afx_msg void OnBnClickedButtonCompareScanSrcpath();
	afx_msg void OnBnClickedButtonCompareScanDestpath();
	afx_msg void OnBnClickedButtonCompare();
	afx_msg void OnBnClickedButtonExit();
	CStatic m_cJZPExtractorScrPath;
	CEdit m_cJZPExtractorDestPath;
	CStatic m_cDatExtractorScrPath;
	CEdit m_cDatExtractorDestPath;
	CStatic m_cCompareScrPath;
	CStatic m_cCompareDestPath;
};
