// AlefAdminStringManager.h
// (C) NHN - ArchLord Development Team
// steeple, 2006.03.07
//
// 다국어 지원을 위해서 스트링 체계를 정리하였다.
// 게임과 어드민 툴에서 같이 쓰이는 스트링은 가져다가 사용하고,
// 아니라면 리소스에서 얻어온다.
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "resource.h"
#include "AgcmUIManager2.h"

class AlefAdminStringManager
{
protected:
	AgcmUIManager2* m_pcsAgcmUIManager2;

public:
	AlefAdminStringManager();
	virtual ~AlefAdminStringManager();

	void Init();

	CString GetUIMessage(TCHAR* szKey);
	CString GetResourceMessage(UINT uID);
};
