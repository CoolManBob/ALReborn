#pragma once


// CWorldMapEditDlg 대화 상자입니다.

class CWorldMapEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMapEditDlg)

public:
	CWorldMapEditDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorldMapEditDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORLDMAP_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectItemId();
	afx_msg void OnBnClickedCalccoodinate();
	
public:
	float		m_fStartX;
	float		m_fStartZ;
	float		m_fEndX;
	float		m_fEndZ;
	int			m_nIndex;
	CString		m_strComment;
	CString		m_strItemID;
	ApmMap::WorldMap*	m_pstWorldMap;
public:
	afx_msg void OnBnClickedCalccoodinate2();
};
