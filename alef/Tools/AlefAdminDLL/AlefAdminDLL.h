// AlefAdminDLL.h : AlefAdminDLL DLL의 기본 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 주 기호입니다.


// CAlefAdminDLLApp
// 이 클래스의 구현을 보려면 AlefAdminDLL.cpp를 참조하십시오.
//

class CAlefAdminDLLApp : public CWinApp
{
public:
	CAlefAdminDLLApp();

// 재정의
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
