// AlefAdminManager.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 02. 14.

// 개발 노트

// 2004.02.16.
// AlefAdminDLL 을 만들면서 예전보다는 조금 쉽게 개발하기 위해서
// AgcmAdmin Module 을 가져다가 사용하려고 했다.
// 컴파일은 무리없이 잘 되고, 각종 모듈도 다 가져올 수 있었다.
// 그러나 예상치 못했던 문제가 발생했다. 문제는 딱 하나.

// g_AuCircularBuffer 가 extern 으로 AuPacket.h 에 잡혀있는데,
// DLL 이 InitInstance 되는 시점에서 하나가 잡히고,
// AlefClient 가 실행되는 시점에서 또하나가 잡힌다. -_-;;
// 즉, global extern 변수는 공유가 안된다는 점이다.
// 앞으로 이런 문제가 더 나올 수도 있기 때문에,
// 그냥 독립적으로 개발하기로 다시 변경했다.
// 그러나 그리하기엔 너무 시간이 오래걸린다....
// 이를 어째야 하나...

// 2004.02.24.
// 그냥 독립적인 global extern 변수를 따로 초기화해주니깐 잘된다.............
// 일주일 날렸다.............................
// 미치겠다..................................

#pragma once

#include "AlefAdminMain.h"
#include "AlefAdminStringManager.h"
#include "CDataManager.h"
#include "AgcmAdmin.h"

class AlefAdminManager
{
private:
	static AlefAdminManager* _Instance;

protected:
	AlefAdminManager();

public:
	virtual ~AlefAdminManager();

	static AlefAdminManager* Instance()
	{	// inline 으로 해줌
		if(!_Instance)
			_Instance = new AlefAdminManager();

		return _Instance;
	}

	void DeleteInstance();

public:
	stAgpdAdminInfo m_stSelfAdminInfo;
	PVOID m_pAdminModule;

	CDataManager m_DataManager;

	AlefAdminMain* m_pMainDlg;

	AlefAdminStringManager m_csStringManager;

public:
	// Common
	BOOL SetAdminModule(PVOID pModule);
	BOOL SetAdminModule(AgcmAdmin* pcsAgcmAdmin);
	AgcmAdmin* GetAdminModule( void );
	CDataManager* GetAdminData( void );

	BOOL IsDialogMessage(LPMSG lpMsg);
	BOOL SetSelfAdminInfo(stAgpdAdminInfo* pstInfo);
	INT16 GetAdminLevel();
	BOOL OpenNotPrivilegeDlg();

	void OnInit();
	BOOL IsInitialized();

	UINT32 GetCurrentTimeStamp();
	BOOL ConvertTimeStampToString(UINT32 ulTimeStamp, CHAR* szTmp);
	BOOL ConvertTimeStampToDateString(UINT32 ulTimeStamp, CHAR* szTmp);

	// Main
	BOOL OpenMainDlg();
	BOOL OnIdle() { return m_DataManager.OnIdle(); }

	// Item
	BOOL AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate);
	BOOL ClearItemTemplateData();

	// String Manager
	AlefAdminStringManager& GetStringManager();
};