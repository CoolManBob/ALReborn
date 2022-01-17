#pragma once

class AFX_EXT_CLASS AgcmFileListDlg
{
public:
	AgcmFileListDlg();
	virtual ~AgcmFileListDlg();

protected:
	CHAR m_szSelectedName[256];

public:
	static AgcmFileListDlg *GetInstance();

	CHAR	*OpenFileList(CHAR *szPathName1, CHAR *szPathName2, CHAR *szPathName3);
	CHAR	*OpenFileList(CHAR *szPath, CHAR *szExt1, CHAR *szExt2, CHAR *szExt3);

	// 내부적으로 쓰임...
	VOID	SetSelectedName(CHAR *szSet)	{
		strcpy(m_szSelectedName, szSet);
	}
};

/* Example

#include "AgcmFileListDlg.h"

VOID Test()
{
	CHAR			szDest[256];
	CHAR			*pszTemp;
	AgcmFileListDlg dlg;

	pszTemp = dlg.OpenFileList(".\\*.dff", ".\\exp\\*.rws", NULL);
	if(pszTemp) // pszTemp의 포인터는 클래스가 소멸되기 전까지 유효!
	{
		strcpy(szDest, pszTemp);
	}
} */
