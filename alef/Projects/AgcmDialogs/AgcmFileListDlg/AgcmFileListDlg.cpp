#include "stdafx.h"

#include "../resource.h"
#include "AgcmFileListDlg.h"
#include "FileListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AgcmFileListDlg *g_pcsAgcmFileListDlgInst = NULL;

AgcmFileListDlg *AgcmFileListDlg::GetInstance()
{
	return g_pcsAgcmFileListDlgInst;
}

AgcmFileListDlg::AgcmFileListDlg()
{
	g_pcsAgcmFileListDlgInst = this;
}

AgcmFileListDlg::~AgcmFileListDlg()
{
}

CHAR *AgcmFileListDlg::OpenFileList(CHAR *szPathName1, CHAR *szPathName2, CHAR *szPathName3)
{
	CFileListDlg dlg(szPathName1, szPathName2, szPathName3);
	if(dlg.DoModal() != IDOK)
		return NULL;

	return m_szSelectedName;
}

CHAR *AgcmFileListDlg::OpenFileList(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3)
{
	CHAR szPathName1[256], szPathName2[256], szPathName3[256];
	sprintf(szPathName1, "%s*.%s", szPath, szExt1);

	if(szExt2)
		sprintf(szPathName2, "%s*.%s", szPath, szExt2);

	if(szExt3)
		sprintf(szPathName3, "%s*.%s", szPath, szExt3);

	return OpenFileList(szPathName1, (szExt2) ? szPathName2 : NULL, (szExt3) ? szPathName3 : NULL);
}