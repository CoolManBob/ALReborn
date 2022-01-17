#pragma once

class AgcmLODDistanceDlg : public CDialog
{
public:
	AgcmLODDistanceDlg(UINT32 *pulDistance, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(AgcmLODDistanceDlg)
	enum { IDD = IDD_LOD_DISTANCE };
	DWORD	m_dwDistance0;
	DWORD	m_dwDistance1;
	DWORD	m_dwDistance2;
	DWORD	m_dwDistance3;
	DWORD	m_dwDistance4;
	//}}AFX_DATA
protected:
	UINT32 *m_pulDistance;

protected:
	// Overrides
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
