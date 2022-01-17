// Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "SDCltDynCode.h"
#include "SDSvrDynCode.h"

using namespace SGDP;

const char * GetRootPath(void)
{
    static char szPath[MAX_PATH];
    static bool bFirstTime = true;

    if(bFirstTime)
    {
        bFirstTime = false;
        GetModuleFileName(NULL, szPath, sizeof(szPath));
        char *p = strrchr(szPath, '\\');
        *p = '\0';
    }

    return szPath;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int nDataLen;
    char Data[256];
	const unsigned char *pCode;
	int nCodeIdx;
	int nCodeLen;

    ISDSvrDynCode * pSvrDynCode = SDCreateSvrDynCode();
    if(NULL == pSvrDynCode)
    {
        printf("Create SvrDynCode component failed\n");
        goto fail_ret;
    }

	ISDCltDynCode * pCltDynCode;
    pCltDynCode = SDCreateCltDynCode();
    if(NULL == pCltDynCode)
    {
        printf("Create CltDynCode component failed\n");
        goto fail_ret;
    }

    char szSvrDir[MAX_PATH];
    sprintf(szSvrDir, "%s\\DynCodeBin\\Server32", GetRootPath());

    char szCltDir[MAX_PATH];
    sprintf(szCltDir, "%s\\DynCodeBin\\Client32", GetRootPath());

	int nBinCount;
    nBinCount = pSvrDynCode->LoadBinary(szSvrDir, szCltDir);
    printf("Load Binary: %d binary are loaded\n", nBinCount);

    if(0 == nBinCount)
        goto fail_ret;

    pCode = NULL;
    nCodeIdx = pSvrDynCode->GetRandIdx();
    nCodeLen = pSvrDynCode->GetCltDynCode(nCodeIdx, &pCode);
    if(nCodeLen < 0)
        goto fail_ret;

    pCltDynCode->SetDynCode(pCode, nCodeLen);

    strcpy(Data, "Hello World!");
    nDataLen = (int)strlen(Data) + 1;
    
    printf("Data Before Encode: %s\n", Data);
    if(false == pCltDynCode->Encode((unsigned char*)Data, nDataLen))
    {
        printf("Encode fail\n");
        goto fail_ret;
    }
    printf("Encode Succeed\n");

    if(false == pSvrDynCode->Decode((unsigned char*)Data, nDataLen, nCodeIdx))
    {
        printf("Decode fail\n");
        goto fail_ret;
    }
    printf("Decode Succeed\n");
    printf("Data after decode: %s\n", Data);

    pSvrDynCode->Release();
    pCltDynCode->Release();

    return 0;

fail_ret:

    if(pSvrDynCode != NULL)
        pSvrDynCode->Release();

    if(pCltDynCode != NULL)
        pCltDynCode->Release();

	return 1;
}
