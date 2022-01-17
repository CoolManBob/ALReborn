#include "stdafx.h"
#include "../resource.h"
#include "AgcmFileListDlg.h"
#include "FileListDlg.h"
#include "FindFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int	CFileListDlg::lastsel = 0;
CFileListDlg::CFileListDlg(CHAR *szPathName1, CHAR *szPathName2, CHAR *szPathName3, CWnd* pParent /*=NULL*/) : CDialog(CFileListDlg::IDD, pParent)
{
	strcpy(m_szPathName1, szPathName1);
	strcpy(m_szPathName2, szPathName2 ? szPathName2 : "" );
	strcpy(m_szPathName3, szPathName3 ? szPathName3 : "" );
}


void CFileListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileListDlg)
	DDX_Control(pDX, IDC_FILE_LIST, m_csFileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileListDlg, CDialog)
	//{{AFX_MSG_MAP(CFileListDlg)
	ON_LBN_DBLCLK(IDC_FILE_LIST, OnDblclkFileList)
	ON_BN_CLICKED(ID_FILE_LIST_FIND, OnFileListFind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileListDlg message handlers

void CFileListDlg::OnDblclkFileList() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

VOID CFileListDlg::ReCalcStatusHorizon()
{
	CClientDC dc(this);
	
	CFont* pOldFont= (CFont*)dc.SelectObject(GetFont());

	int nMaxWidth = 0;
	CString szText;

	for(int i = 0; i < m_csFileList.GetCount(); i ++)
	{
		m_csFileList.GetText(i, szText);
		CSize sizText = dc.GetTextExtent(szText);
		
		if(sizText.cx > nMaxWidth)
		{
			nMaxWidth = sizText.cx;
		}
	}

	m_csFileList.SetHorizontalExtent(nMaxWidth);
	dc.SelectObject(pOldFont);
}

BOOL CFileListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CFileFind	csFind;
	BOOL		bFind;

	for(bFind = csFind.FindFile(m_szPathName1); bFind;	bFind = csFind.FindNextFile(),
														m_csFileList.AddString(csFind.GetFileName())	);
	csFind.Close();

	if(strcmp(m_szPathName2, ""))
	{
		for(bFind = csFind.FindFile(m_szPathName2); bFind;	bFind = csFind.FindNextFile(),
															m_csFileList.AddString(csFind.GetFileName())	);
		csFind.Close();
	}
	
	if(strcmp(m_szPathName3, ""))
	{
		for(bFind = csFind.FindFile(m_szPathName3); bFind;	bFind = csFind.FindNextFile(),
															m_csFileList.AddString(csFind.GetFileName())	);
		csFind.Close();
	}

	if( lastsel < m_csFileList.GetCount() )
		m_csFileList.SetCurSel(lastsel);

	ReCalcStatusHorizon();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileListDlg::OnOK() 
{
	// TODO: Add extra validation here
	INT16 nIndex = m_csFileList.GetCurSel();
	if(nIndex < 0)
		return;

	CHAR szTemp[256];

	if(m_csFileList.GetText(nIndex, szTemp) == LB_ERR)
		return;

	AgcmFileListDlg::GetInstance()->SetSelectedName(szTemp);
	
	lastsel	= nIndex;

	CDialog::OnOK();
}

void CFileListDlg::OnFileListFind() 
{
	// TODO: Add your control notification handler code here
	CHAR szTemp[256];

	CFindFileDlg dlg(szTemp);
	if(dlg.DoModal() == IDOK)
	{
		SetFindFile(szTemp);
	}
}

VOID CFileListDlg::_ToUpper(CHAR *szDest, CHAR *szSrc)
{
	strcpy(szDest, szSrc);
	INT16 nMax = strlen(szSrc);
	for(INT16 nCount = 0; nCount < nMax; szDest[nCount] = toupper(szSrc[nCount]), ++nCount);
}

BOOL CFileListDlg::IsLowEqual(CHAR *szCmp1, CHAR *szCmp2)
{
	CHAR szTemp1[256], szTemp2[256];

	_ToUpper(szTemp1, szCmp1);
	_ToUpper(szTemp2, szCmp2);

	INT16 nMax = strlen(szTemp2);
	if(nMax > (INT16)(strlen(szTemp1)))
		return FALSE;

	for(INT16 nCount = 0; nCount < nMax; ++nCount)
	{
		if(szTemp1[nCount] != szTemp2[nCount])
			return FALSE;
	}

	return TRUE;
}

VOID CFileListDlg::SetFindFile(CHAR *szCmp)
{
	CHAR	szCurList[256];
	INT16	nMax = m_csFileList.GetCount();
	for(INT16 nCount = 0; nCount < nMax; ++nCount)
	{
		m_csFileList.GetText(nCount, szCurList);

		if(IsLowEqual(szCurList, szCmp))
		{
			m_csFileList.SetCurSel(nCount);
			break;
		}
	}
}