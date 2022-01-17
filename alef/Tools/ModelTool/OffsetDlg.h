#pragma once

class CCameraOffset
{
public:
	FLOAT m_fRotate;
	FLOAT m_fZoom;
	FLOAT m_fMove;

	CCameraOffset()
	{
		m_fRotate	= 1.f;
		m_fZoom		= 1.f;
		m_fMove		= 1.f;
	}
};

class CEditOffset
{
public:
	FLOAT m_fPosit;
	FLOAT m_fRotate;
	FLOAT m_fScale;

	CEditOffset()
	{
		m_fPosit	= 1.f;
		m_fRotate	= 1.f;
		m_fScale	= 1.f;
	}
};

class CCharacterOffset
{
public:
	BOOL m_bCharacterTarget;

	CCharacterOffset()
	{
		m_bCharacterTarget = FALSE;
	}
};

class COffsetDlg : public CDialog
{
public:
	COffsetDlg(CCharacterOffset *pCharacterOffset = NULL, CCameraOffset *pCameraOffset = NULL, CEditOffset *pEditOffset = NULL, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(COffsetDlg)
	enum { IDD = IDD_OFFSET };
	float	m_fCameraMove;
	float	m_fCameraRotate;
	float	m_fCameraZoom;
	float	m_fEditPosit;
	float	m_fEditScale;
	float	m_fEditRotate;
	BOOL	m_bCharacterTarget;
	//}}AFX_DATA

protected:
	CCharacterOffset	*m_pcsCharacterOffset;
	CCameraOffset		*m_pcsCameraOffset;
	CEditOffset			*m_pcsEditOffset;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
