// AgcmAdminDlgXT_MessageQueue.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 02.

// 만들게 된 동기
// Lock 매니저의 도입과 함께, 클라이언트 메인 프로시저의 Idle 에서 락이 안풀린 경우에는 ASSERT 가 뜨게 된다.
// 문제는, MFC DLL 에서 MessageBox 를 띄우게 되면, 메인 프로시저가 한번 더 호출이 되면서 락이 풀리지 않은 상태로 
// Idle 을 한번 더 돌게 된다. - 자세히는 잘 모르겠음.... TT
//
// 그래서 Thread 를 분리하는 게 낫다고 판단! 해서 도입하게 되었다.

typedef enum _eMESSAGE_QUEUE_ITEM_ID
{
	MESSAGE_QUEUE_ITEM_ID_RECV_CHAR_EDIT = 0,
	MESSAGE_QUEUE_ITEM_ID_RECV_ITEM_RESULT,
	MESSAGE_QUEUE_ITEM_ID_RECV_HELP_RESULT,
	MESSAGE_QUEUE_ITEM_ID_RECV_HELP_MEMO_RESULT,
} eMESSAGE_QUEUE_ITEM_ID;

typedef struct _stMESSAGE_QUEUE_ITEM
{
	eMESSAGE_QUEUE_ITEM_ID m_eID;
	PVOID m_pData;
} MESSAGE_QUEUE_ITEM;

typedef enum _eMESSAGE_QUEUE_STATUS
{
	MESSAGE_QUEUE_STATUS_NOT = 0,
	MESSAGE_QUEUE_STATUS_INITIALIZED,
	MESSAGE_QUEUE_STATUS_RUNNING,
} eMESSAGE_QUEUE_STATUS;

class AgcmAdminDlgXT_MessageQueue
{
protected:
	AgcmAdminDlgXT_IOCP m_csIOCP;
	
	INT8 m_cThreadNum;
	INT8 m_cStatus;

public:
	AgcmAdminDlgXT_MessageQueue();
	virtual ~AgcmAdminDlgXT_MessageQueue();

	// System Function
	BOOL Init();
	BOOL Start();
	BOOL Stop();

	BOOL PushQueue(MESSAGE_QUEUE_ITEM* pItem);
	BOOL ProcessQueue();

	static unsigned __stdcall ThreadFunc(LPVOID arg);	// Thread Wrapping Function

protected:
	// Process Operation Fnction
	BOOL ParseQueueItem(MESSAGE_QUEUE_ITEM* pItem);

	BOOL OnRecvCharEdit(PVOID pData);
	BOOL OnRecvItemResult(PVOID pData);
	BOOL OnRecvHelpResult(PVOID pData);
	BOOL OnRecvHelpMemoResult(PVOID pData);

public:
	// User Call Function
	BOOL PushQueueRecvCharEdit(stAgpdAdminCharEdit* pstCharEdit);
	BOOL PushQueueRecvItemResult(stAgpdAdminItemOperation* pstItemOperation);
	BOOL PushQueueRecvHelpResult(stAgpdAdminHelp* pstHelp);
	BOOL PushQueueRecvHelpMemoResult(stAgpdAdminHelp* pstHelp);
};
