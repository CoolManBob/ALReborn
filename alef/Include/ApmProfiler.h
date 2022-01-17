
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the APMPROFILER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// APMPROFILER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef APMPROFILER_EXPORTS
#define APMPROFILER_API __declspec(dllexport)
#else
#define APMPROFILER_API __declspec(dllimport)
#endif

#include "AuProfileManager.h"
#include "ApModule.h"

// This class is exported from the ApmProfiler.dll
class APMPROFILER_API ApmProfiler : public ApModule
{
private:
//	AuProfileManager *	m_pcsProfile;

	INT32			m_ulLastTick;
	BOOL			m_bInit;

	HANDLE			m_hThread;
	DWORD			m_ulThreadID;

	static BOOL		m_bDestroy;

	static CProfileNode *	m_pRoot;
	static CProfileNode *	m_pCurNode;

	static HWND		m_hDialog;
	static HWND		m_hList;
	static HWND		m_hEntry;

	static CHAR		m_szEntry[1024];

public:
	static HINSTANCE		m_hInstance;

public:
	ApmProfiler();
	~ApmProfiler();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnIdle(UINT32 ulClockCount);
	BOOL	OnDestroy();

	VOID	SetProfileManager(CProfileNode **ppRoots);

	static VOID				UpdateStatistics(BOOL bChangeEntry);

	static VOID				SetCurrentEntry();

	static BOOL CALLBACK	ProfilerProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI		DialogThread(PVOID pvParam);
	static BOOL				SetupDialog(HWND hDlg);
};
