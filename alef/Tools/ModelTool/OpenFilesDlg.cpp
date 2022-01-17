// OpenFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModelTool.h"
#include "OpenFilesDlg.h"

typedef pair< int, std::string >									string_Pair;
typedef /*stdext::*/unordered_map< int, std::string >::iterator			string_Iterator;

typedef hash_multimap< int, std::string >::iterator					string_Iterator_Multimap;

// COpenFilesDlg dialog
IMPLEMENT_DYNAMIC(COpenFilesDlg, CDialog)
COpenFilesDlg::COpenFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenFilesDlg::IDD, pParent)
{
}

COpenFilesDlg::~COpenFilesDlg()
{
}

void COpenFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OPEN_FILES_TREE, m_ctlFileTree);
}

BEGIN_MESSAGE_MAP(COpenFilesDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_OPEN_FILES_TREE, OnNMClickOpenFilesTree)
END_MESSAGE_MAP()

BOOL COpenFilesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HTREEITEM hRootItem = m_ctlFileTree.InsertItem( m_strRootName.c_str(), TVI_ROOT );

	CharTreeItem Temp;
	Temp.m_hItem = m_ctlFileTree.InsertItem( "문엘프 여자법사",hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_MOONELF_SHA );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "문엘프 여자궁수",hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_MOONELF_HUN );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "문엘프 여자전사",hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_MOONELF_SB );
	m_vKindTreeItem.push_back( Temp );
	
	Temp.m_hItem = m_ctlFileTree.InsertItem( "오크 남자마법사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_ORC_SHA );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "오크 남자전사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_ORC_WAR );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "오크 여자궁수",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_ORC_HUN );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "휴먼 남자궁수",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_HUMAN_ARC );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "휴먼 남자전사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_HUMAN_KNI );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "휴먼 여자마법사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_HUMAN_WIZ );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "드레곤시온 시온",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_DRAGONSCION_SCION );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "드레곤시온 전사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_DRAGONSCION_SLA );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "드레곤시온 궁수",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_DRAGONSCION_OBI );
	m_vKindTreeItem.push_back( Temp );

	Temp.m_hItem = m_ctlFileTree.InsertItem( "드레곤시온 법사",	hRootItem );
	strcpy( Temp.szName, AMT_RESOURCE_TREE_NAME_DRAGONSCION_SUM );
	m_vKindTreeItem.push_back( Temp );

	FindFile();

	GetDlgItem( IDOK )->EnableWindow( FALSE );

	return TRUE;
}

// COpenFilesDlg message handlers
void COpenFilesDlg::SetRootName( const char* szName )
{
	m_strRootName = szName;
}

void COpenFilesDlg::SetDirectory( const char* szIniDir )
{
	m_strIniDirectory = szIniDir;
}

void COpenFilesDlg::FindFile()
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool bFinished = false;

	std::string strDest;
	char szBackupDir[256];
	_getcwd( szBackupDir, sizeof(szBackupDir) );
	strDest = szBackupDir;
	strDest += m_strIniDirectory;
	strDest += "*.rws";

	hFind = FindFirstFile( strDest.c_str(), &FindFileData );
	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	char* pFileNameUpper;
	while( bFinished == FALSE )
	{
		pFileNameUpper = strupr( FindFileData.cFileName );
		for( int i = 0; i < (int)m_vKindTreeItem.size(); i++ )
		{
			if( strncmp( pFileNameUpper, m_vKindTreeItem[i].szName, strlen(m_vKindTreeItem[i].szName) ) == 0 ) {
				HTREEITEM hItem = m_ctlFileTree.InsertItem( FindFileData.cFileName, m_vKindTreeItem[i].m_hItem );
				m_hashFileName.insert( string_Pair( (int)hItem, FindFileData.cFileName ) );

				m_hashKindFileName.insert( string_Pair( (int)m_vKindTreeItem[i].m_hItem, FindFileData.cFileName ) );
			}
		}

		if( !FindNextFile(hFind, &FindFileData) ) {
			if (GetLastError() == ERROR_NO_MORE_FILES) {
				bFinished = true;
			}
			else {
				break;
			}
		}
	}

	FindClose( hFind );
}

void COpenFilesDlg::OnBnClickedOk()
{
	OnOK();
}

void COpenFilesDlg::OnNMClickOpenFilesTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint ptPos;
	GetCursorPos(&ptPos);
	CPoint ptClient = ptPos;
	m_ctlFileTree.ScreenToClient( &ptClient );
	HTREEITEM htreeSelect = m_ctlFileTree.HitTest(ptClient);
	m_ctlFileTree.SelectItem( htreeSelect );

	string_Iterator_Multimap it = m_hashKindFileName.find( (int)htreeSelect );
	if( it != m_hashKindFileName.end() )
	{
		m_vstrFileName.clear();
		for( ; m_hashKindFileName.end() != it && it->first == (int)htreeSelect; it++ )
			m_vstrFileName.push_back( it->second );

		GetDlgItem( IDOK )->EnableWindow( TRUE );
		return;
	}

	string_Iterator it2 = m_hashFileName.find( (int)htreeSelect );
	if( it2 == m_hashFileName.end() )
	{
		m_vstrFileName.clear();
		GetDlgItem( IDOK )->EnableWindow( FALSE );
		return;
	}

	m_vstrFileName.clear();
	m_vstrFileName.push_back( it2->second );
	GetDlgItem( IDOK )->EnableWindow( TRUE );

	*pResult = 0;
}

int COpenFilesDlg::GetFileNum() const
{
	return (int)m_vstrFileName.size();
}

const char* COpenFilesDlg::GetFileName( int nIdx ) const
{
	if( nIdx >= (int)m_vstrFileName.size() )
		return NULL;

	return m_vstrFileName[nIdx].c_str();
}
