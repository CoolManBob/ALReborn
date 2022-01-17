#pragma once

class CItemTemplateListDlg : public CDialog
{
public:
	CItemTemplateListDlg(INT32 *plTID, CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CItemTemplateListDlg)
	enum { IDD = IDD_ITEM_TEMPLATES_LIST };
	CListBox	m_csList;
	//}}AFX_DATA

protected:
	INT32 *m_plTID;
	INT32 m_lEquipType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void OnDblclkItemTemplateList();
	DECLARE_MESSAGE_MAP()

private :
	BOOL		 _IsAcceptFilter( AgpdItemTemplate* pcsAgpdItemTemplate );

public :
	inline	void SetListFilter( INT32 lEquipType )
	{
		m_lEquipType = lEquipType;
	}
};