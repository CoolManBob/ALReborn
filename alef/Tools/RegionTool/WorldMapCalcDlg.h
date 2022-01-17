#pragma once


// CWorldMapCalcDlg 대화 상자입니다.

class CWorldMapCalcDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMapCalcDlg)

public:
	CWorldMapCalcDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorldMapCalcDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORLDCOODCALC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedOk();

public:
	int		nX1;
	int		nY1;
	int		nX2;
	int		nY2;
	int		nDivision1;
	int		nDivision2;

	FLOAT	m_fStartX;
	FLOAT	m_fStartZ;
	FLOAT	m_fEndX;
	FLOAT	m_fEndZ;
};
