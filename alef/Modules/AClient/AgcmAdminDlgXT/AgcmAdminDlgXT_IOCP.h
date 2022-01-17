// AgcmAdminDlgXT_IOCP.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 02.

// 만들게 된 동기
// MessageQueue 를 만들면서 ApIOCPDispatcher 를 사용하려 했으나,
// ApIOCPDispatcher 가 Server 쪽 파일을 Include 함에 따라서
// 가져다 쓰질 못하게 되었음. 그래서 ApIOCP 만 띄어내서 붙여넣음.

typedef UINT (WINAPI *ThreadProc)(PVOID);
const INT32 MAX_THREAD_COUNT = 20;	// 최대 쓰레드 갯수정의

class AgcmAdminDlgXT_IOCP  
{
private:
	HANDLE	m_hIOCP;
	HANDLE	m_hWorkerThreadHandles[MAX_THREAD_COUNT];
	UINT	m_lWorkerThreadIDs[MAX_THREAD_COUNT];
	INT32	m_lThreadCount;
	
public:
	AgcmAdminDlgXT_IOCP();
	virtual ~AgcmAdminDlgXT_IOCP();
	
	VOID	Initialize();

	HANDLE	GetHandle();
	INT32	GetThreadCount();

	BOOL	Create(INT32 lThreadCount, ThreadProc WalkerThread, PVOID pvParam);
	VOID	Destroy();

	BOOL	AttachHandle(HANDLE hHandle, DWORD dwKey);
	BOOL	PostStatus(DWORD dwKey, DWORD dwBytes = 0, LPOVERLAPPED lpOverlapped = NULL);
	BOOL	GetStatus(DWORD *pdwKey, DWORD *pdwBytes, LPOVERLAPPED *plpOverlapped, DWORD dwTime = INFINITE);
};
