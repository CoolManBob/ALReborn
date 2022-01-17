// Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include "SDCltDynCode.h"
#include "SDSvrDynCode.h"

#define BIT64

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
    ISDSvrDynCode * pSvrDynCode = SDCreateSvrDynCode();
    if(NULL == pSvrDynCode)
    {
        printf("Create SvrDynCode component failed\n");
        goto fail_ret;
    }

    //ISDCltDynCode * pCltDynCode = SDCreateCltDynCode();
    //if(NULL == pCltDynCode)
    //{
    //    printf("Create CltDynCode component failed\n");
    //    goto fail_ret;
    //}

    char szSvrDir[MAX_PATH];
    char szCltDir[MAX_PATH];

#ifdef BIT64
    sprintf_s(szSvrDir, "%s\\DynCodeBin\\Server64", GetRootPath());
	sprintf_s(szCltDir, "%s\\DynCodeBin\\Client32", GetRootPath());
#else
    sprintf(szSvrDir, "%s\\DynCodeBin\\Server32", GetRootPath());
	sprintf(szCltDir, "%s\\DynCodeBin\\Client32", GetRootPath());
#endif

    int nBinCount = pSvrDynCode->LoadBinary(szSvrDir, szCltDir);
    printf("Load Binary: %d binary are loaded\n", nBinCount);

    if(0 == nBinCount)
        goto fail_ret;

    const unsigned char *pCode = NULL;
    int nCodeIdx = pSvrDynCode->GetRandIdx();
    int nCodeLen = pSvrDynCode->GetCltDynCode(nCodeIdx, &pCode);
    if(nCodeLen < 0)
        goto fail_ret;

    //pCltDynCode->SetDynCode(pCode, nCodeLen);

    //char Data[256];
    //strcpy(Data, "Hello World!");
    //int nDataLen = (int)strlen(Data) + 1;
    //
    //printf("Data Before Encode: %s\n", Data);
    //if(false == pCltDynCode->Encode((unsigned char*)Data, nDataLen))
    //{
    //    printf("Encode fail\n");
    //    goto fail_ret;
    //}
    //printf("Encode Succeed\n");

	BYTE Data[256] = {0x64,0x0F,0x30,0xE2,0x76,0x30,0x66,0xF4,0x76,0x01,0x37,0xF4,0x45,0x24,0x09,0x9F,
        0x9D,0xB8,0xD1,0x17,0x71,0x68,0xE5,0x7E,0xA2,0xC9,0x99,0x2B,0xCB,0xAE,0x7E,0x66,
        0x20,0x34,0x60,0xA8,0xAB,0x5B,0xDC,0x01,0x1C,0x64,0x6F,0x67,0x20,0x04,0x60,0xEC};

    if(false == pSvrDynCode->Decode((unsigned char*)Data, 48, nCodeIdx))
    {
        printf("Decode fail\n");
        goto fail_ret;
    }
    printf("Decode Succeed\n");
    printf("Data after decode: %s\n", Data);

    pSvrDynCode->Release();
    //pCltDynCode->Release();

    return 0;

fail_ret:

    if(pSvrDynCode != NULL)
        pSvrDynCode->Release();

    //if(pCltDynCode != NULL)
    //    pCltDynCode->Release();

	return 1;
}
