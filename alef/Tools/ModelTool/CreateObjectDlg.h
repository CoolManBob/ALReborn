#pragma once

enum
{
	COD_TYPE_NAME_CHARACTER_INDEX = 0,
	COD_TYPE_NAME_ITEM_INDEX,
	COD_TYPE_NAME_OBJECT_INDEX,
	COD_TYPE_NAME_SKILL_INDEX
};

#define COD_TYPE_NAME_CHARACTER		"CHARACTER"
#define COD_TYPE_NAME_ITEM			"ITEM"
#define COD_TYPE_NAME_OBJECT		"OBJECT"
#define COD_TYPE_NAME_SKILL			"SKILL"

class CCreateObjectDlg : public CDialog
{
public:
	CCreateObjectDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CCreateObjectDlg)
	enum { IDD = IDD_CREATE_OBJECT };
	CButton		m_csCOProperty;
	CComboBox	m_csCOType;
	CString		m_strCOName;
	CString		m_strID;
	BOOL		m_bMakeTemporaryTemplate;
	//}}AFX_DATA

protected:
	PVOID	m_pvAgpdCharacterTemplate;
	PVOID	m_pvAgcdCharacterTemplate;

	PVOID	m_pvAgpdItemTemplate;
	PVOID	m_pvAgcdItemTemplate;

	PVOID	m_pvApdObjectTemplate;
	PVOID	m_pvAgcdObjectTemplate;

	PVOID	m_pvAgpdSkillTemplate;
	PVOID	m_pvAgcdSkillTemplate;

protected:
	VOID InitializeMember();
	VOID ReleaseMember();
	BOOL SetBase(CHAR *szName, CHAR *szDFFName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	//{{AFX_MSG(CCreateObjectDlg)
	afx_msg void OnButtonCoMore();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeComboCoType();

	DECLARE_MESSAGE_MAP()
public:
	
};