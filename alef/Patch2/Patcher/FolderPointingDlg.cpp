// FolderPointingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "FolderPointingDlg.h"

IMPLEMENT_DYNAMIC(CFolderPointingDlg, CDialog)

CFolderPointingDlg::CFolderPointingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderPointingDlg::IDD, pParent)
	, m_strRealTimeFolder(_T(""))
{

}

CFolderPointingDlg::~CFolderPointingDlg()
{
}

void CFolderPointingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REALTIME_EDIT	, m_strRealTimeFolder	);
}

BEGIN_MESSAGE_MAP(CFolderPointingDlg, CDialog)
	ON_BN_CLICKED(IDC_SEARCHREALTIME, &CFolderPointingDlg::OnBnClickedSearchrealtime)
END_MESSAGE_MAP()

void CFolderPointingDlg::OnBnClickedSearchrealtime()
{
	UpdateData(	TRUE );


	LPITEMIDLIST	pidlRoot		= NULL;
	LPITEMIDLIST	pidlSelected	= NULL;
	BROWSEINFO		bi				= {0};
	LPMALLOC		pMalloc			= NULL;
	CHAR			pszDisplayName[256];
	CHAR			strSourceFolder[ 256 ];

	SHGetMalloc(&pMalloc);

	bi.hwndOwner		= this->m_hWnd;
	bi.pidlRoot			= pidlRoot;
	bi.pszDisplayName	= pszDisplayName;
	bi.lpszTitle		= "Choose a Patch Folder";
	bi.ulFlags			= 0;
	bi.lpfn				= NULL;
	bi.lParam			= 0;

	pidlSelected = SHBrowseForFolder(&bi);

	// NULL이면 Scan Folder에서 취소를 누른것이기 때문에
	// 바로 종료 시킨다
	if( pidlSelected == NULL )		
	{
		UpdateData( FALSE );
		return;
	}

	if(pidlRoot)
	{
		pMalloc->Free(pidlRoot);
	}

	pMalloc->Release();

	if( pidlSelected != NULL )
	{
		SHGetPathFromIDList(pidlSelected, strSourceFolder );

		m_strRealTimeFolder				=	strSourceFolder;
	}

	UpdateData( FALSE );
}