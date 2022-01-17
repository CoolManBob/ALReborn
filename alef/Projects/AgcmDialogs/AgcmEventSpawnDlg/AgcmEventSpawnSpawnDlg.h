#if !defined(AFX_AGCMEVENTSPAWNSPAWNDLG_H__9D2F29C9_6240_4815_ACC6_B757E6890F7D__INCLUDED_)
#define AFX_AGCMEVENTSPAWNSPAWNDLG_H__9D2F29C9_6240_4815_ACC6_B757E6890F7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AgcmEventSpawnSpawnDlg.h : header file
//

#include "../resource.h"
#include "AgpmItem.h"
#include "AgpmDropItem.h"
#include "AgpmAI2.h"
#include "AgpmEventSpawn.h"
//#include "AgcmDropItemDlg.h"
#include "AgcmDropTemplateDlg.h"

/////////////////////////////////////////////////////////////////////////////
// AgcmEventSpawnSpawnDlg dialog

#define AGCMEVENT_SPAWN_CHARACTERLIST_COLUMN1	"Character"
#define AGCMEVENT_SPAWN_CHARACTERLIST_COLUMN2	"Rate"

#define AGCMEVENT_SPAWN_ITEMLIST_COLUMN1		"Item"
#define AGCMEVENT_SPAWN_ITEMLIST_COLUMN2		"Rate"

class AgcmEventSpawnDlg;

class AgcmEventSpawnSpawnDlg : public CDialog
{
//	AgcmDropItemDlg			m_csDropItemDlg;
	AgcmDropTemplateDlg		m_csDropTemplateDlg;

// Construction
public:
	AgcmEventSpawnDlg *		m_pcsAgcmEventSpawnDlg;
	AgpmCharacter			*m_pcsAgpmCharacter;
	AgpmItem				*m_pcsAgpmItem;
	AgpmSkill				*m_pcsAgpmSkill;
	AgpmDropItem			*m_pcsAgpmDropItem;
	AgpmAI2					*m_pcsAgpmAI2;

	ApdEvent *				m_pstEvent;
	AgpdSpawn *				m_pstSpawn;
	INT32					m_lSpawnCharIndex;
	AgpdSpawnConfigChar *	m_pstSpawnChar;

	BOOL SetModule(AgcmEventSpawnDlg *pcsAgcmEventSpawnDlg, AgpmCharacter *pcsAgpmCharacter, AgpmItem *pcsAgpmItem, AgpmSkill *pcsAgpmSkill, AgpmDropItem *pcsAgpmDropItem, AgpmAI2 *pcsAgpmAI2 );
	BOOL SetAI2TemplateCombo();
	BOOL InitData(ApdEvent *pstEvent);
	BOOL InitSpawnData();
	VOID Init();
	AgcmEventSpawnSpawnDlg(CWnd* pParent = NULL);   // standard constructor

	static BOOL CBSelctAITemplate(PVOID pvData, PVOID pvClass, PVOID pvCustData);

// Dialog Data
	//{{AFX_DATA(AgcmEventSpawnSpawnDlg)
	enum { IDD = IDD_DIALOG_SPAWN };

	CListBox	m_cSelectedDropItemTemplateList;
	CComboBox	m_cDropItemCombo;
	CEdit		m_cAddDropItemNameEdit;
	CTreeCtrl	m_csSpawnTree;
	CComboBox	m_csCharacter;
	CListCtrl	m_csCharacterList;

	FLOAT		m_fPositionX;
	FLOAT		m_fPositionY;
	FLOAT		m_fPositionZ;
	FLOAT		m_fRadius;
	INT			m_lMaxChar;
	INT			m_lSpawnRate;
	LONG		m_lGroupMin;
	LONG		m_lGroupMax;
	LONG		m_lGroupInterval;
	CString		m_strGroupName;
	CString		m_strSpawnName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AgcmEventSpawnSpawnDlg)
	public:
	virtual BOOL Create();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AgcmEventSpawnSpawnDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnSpawnAddChar();
	afx_msg void OnSpawnDelChar();
	afx_msg void OnSpawnUpdChar();
	afx_msg void OnItemchangedSpawnCharacters(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpawnGroupAdd();
	afx_msg void OnSpawnGroupDelete();
	afx_msg void OnSpawnGroupUpdate();
	afx_msg void OnSelchangedSpawnGroupTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpawnSetGroup();
	afx_msg void OnSpawnEditCondition();
	afx_msg void OnItemchangedSpawnItems(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSpawnAddItem();
	afx_msg void OnSpawnDelItem();
	afx_msg void OnSpawnUpdItem();
	afx_msg void OnOpenDropTemplateEditButton();
	afx_msg void OnAddDropItemTemplateButton();
	afx_msg void OnUpdateDropItemTemplateButton();
	afx_msg void OnRemoveDropItemTemplateButton();
	afx_msg void OnSelectDropItemTemplateButton();
	afx_msg void OnDeleteSelectedDropItemTemplateButton();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGCMEVENTSPAWNSPAWNDLG_H__9D2F29C9_6240_4815_ACC6_B757E6890F7D__INCLUDED_)
