#pragma once

#include <string>
#include <vector>
#include "afxcmn.h"
// COpenFilesDlg dialog

class COpenFilesDlg : public CDialog
{
	DECLARE_DYNAMIC(COpenFilesDlg)
public:
	struct CharTreeItem
	{
		HTREEITEM	m_hItem;
		char		szName[256];
	};

	void SetRootName( const char* szName );
	void SetDirectory( const char* szIniDir );
	int			GetFileNum() const;
	const char* GetFileName( int nIdx ) const;

private:
	void FindFile();

private:
	std::string										m_strRootName;
	std::string										m_strIniDirectory;
	std::vector< CharTreeItem >						m_vKindTreeItem;
	/*stdext::*/unordered_map< int, std::string >		m_hashFileName;
	/*stdext::*/hash_multimap< int, std::string >	m_hashKindFileName;

	CTreeCtrl										m_ctlFileTree;
	std::vector< std::string >						m_vstrFileName;

public:
	COpenFilesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COpenFilesDlg();

// Dialog Data
	enum { IDD = IDD_OPEN_FILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickOpenFilesTree(NMHDR *pNMHDR, LRESULT *pResult);
};
