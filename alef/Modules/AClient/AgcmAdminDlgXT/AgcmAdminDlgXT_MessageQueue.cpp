// AgcmAdminDlgXT_Manager.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 02.

#include "StdAfx.h"

#define MESSAGE_QUEUE_THREAD_NUM	1

//////////////////////////////////////////////////////////////////////////
// AgcmAdminDlgXT_MessageQueue

AgcmAdminDlgXT_MessageQueue::AgcmAdminDlgXT_MessageQueue()
{
	m_cThreadNum = 0;
	m_cStatus = MESSAGE_QUEUE_STATUS_NOT;
}

AgcmAdminDlgXT_MessageQueue::~AgcmAdminDlgXT_MessageQueue()
{
}

BOOL AgcmAdminDlgXT_MessageQueue::Init()
{
	m_cThreadNum = MESSAGE_QUEUE_THREAD_NUM;
	m_cStatus = MESSAGE_QUEUE_STATUS_INITIALIZED;
	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::Start()
{
	if(m_cThreadNum < 1)
		return FALSE;

	if(m_cStatus != MESSAGE_QUEUE_STATUS_INITIALIZED)
		return FALSE;

	m_csIOCP.Create(m_cThreadNum, ThreadFunc, (PVOID)this);

	m_cStatus = MESSAGE_QUEUE_STATUS_RUNNING;

	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::Stop()
{
	if(m_cStatus != MESSAGE_QUEUE_STATUS_RUNNING)
		return FALSE;

	m_csIOCP.Destroy();

	m_cStatus = MESSAGE_QUEUE_STATUS_INITIALIZED;

	return TRUE;
}

// Push Queue
BOOL AgcmAdminDlgXT_MessageQueue::PushQueue(MESSAGE_QUEUE_ITEM* pItem)
{
	if(!pItem)
		return FALSE;

	m_csIOCP.PostStatus((DWORD)pItem);

	return TRUE;
}

// Real Thread Function
BOOL AgcmAdminDlgXT_MessageQueue::ProcessQueue()
{
	MESSAGE_QUEUE_ITEM* pItem;
	LPOVERLAPPED lpOverlapped;
	DWORD dwBytes = 0;
	MSG stMsg;

	while(true)
	{
		m_csIOCP.GetStatus((PULONG_PTR)&pItem, &dwBytes, &lpOverlapped);

		std::auto_ptr<MESSAGE_QUEUE_ITEM> AutoPtr(pItem);

		ParseQueueItem(pItem);

		if(pItem->m_pData)
			delete pItem->m_pData;

		// Thread 종료 검사
		if(::PeekMessage(&stMsg, NULL, 0, 0, PM_REMOVE))
		{
			// 쓰레드 종료
			if(WM_QUIT == stMsg.message) return TRUE;
		}
	}

	return TRUE;
}

// Thread Wrapper
unsigned __stdcall AgcmAdminDlgXT_MessageQueue::ThreadFunc(LPVOID arg)
{
	AgcmAdminDlgXT_MessageQueue* pThis = (AgcmAdminDlgXT_MessageQueue*)arg;
	if(!pThis)
		return 0;

	pThis->ProcessQueue();
	return 1;
}







//////////////////////////////////////////////////////////////////////////
// Process Operation Fuction
BOOL AgcmAdminDlgXT_MessageQueue::ParseQueueItem(MESSAGE_QUEUE_ITEM* pItem)
{
	if(!pItem || !pItem->m_pData)
		return FALSE;

	switch(pItem->m_eID)
	{
		case MESSAGE_QUEUE_ITEM_ID_RECV_CHAR_EDIT:
			OnRecvCharEdit(pItem->m_pData);
			break;

		case MESSAGE_QUEUE_ITEM_ID_RECV_ITEM_RESULT:
			OnRecvItemResult(pItem->m_pData);
			break;

		case MESSAGE_QUEUE_ITEM_ID_RECV_HELP_RESULT:
			OnRecvHelpResult(pItem->m_pData);
			break;

		case MESSAGE_QUEUE_ITEM_ID_RECV_HELP_MEMO_RESULT:
			OnRecvHelpMemoResult(pItem->m_pData);
			break;
	}

	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::OnRecvCharEdit(PVOID pData)
{
	if(!pData)
		return FALSE;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->ReceiveEditResult((stAgpdAdminCharEdit*)pData);
	//AgcmAdminDlgXT_Manager::Instance()->GetCharDlg()->Unlock();

	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::OnRecvItemResult(PVOID pData)
{
	if(!pData)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->ReceiveResult((stAgpdAdminItemOperation*)pData);
	//AgcmAdminDlgXT_Manager::Instance()->GetItemDlg()->Unlock();

	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::OnRecvHelpResult(PVOID pData)
{
	if(!pData)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveHelpCompleteResultMessage((stAgpdAdminHelp*)pData);
	//AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();

	return TRUE;
}

BOOL AgcmAdminDlgXT_MessageQueue::OnRecvHelpMemoResult(PVOID pData)
{
	if(!pData)
		return FALSE;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	//AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Lock();
	AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->ReceiveHelpMemoResultMessage((stAgpdAdminHelp*)pData);
	//AgcmAdminDlgXT_Manager::Instance()->GetHelpDlg()->Unlock();

	return TRUE;
}







//////////////////////////////////////////////////////////////////////////
// User Call Function
BOOL AgcmAdminDlgXT_MessageQueue::PushQueueRecvCharEdit(stAgpdAdminCharEdit* pstCharEdit)
{
	if(!pstCharEdit)
		return FALSE;

	MESSAGE_QUEUE_ITEM* pItem = new MESSAGE_QUEUE_ITEM;
	memset(pItem, 0, sizeof(MESSAGE_QUEUE_ITEM));

	pItem->m_eID = MESSAGE_QUEUE_ITEM_ID_RECV_CHAR_EDIT;

	pItem->m_pData = new stAgpdAdminCharEdit;
	memcpy(pItem->m_pData, pstCharEdit, sizeof(stAgpdAdminCharEdit));

	return PushQueue(pItem);
}

BOOL AgcmAdminDlgXT_MessageQueue::PushQueueRecvItemResult(stAgpdAdminItemOperation* pstItemOperation)
{
	if(!pstItemOperation)
		return FALSE;

	MESSAGE_QUEUE_ITEM* pItem = new MESSAGE_QUEUE_ITEM;
	memset(pItem, 0, sizeof(MESSAGE_QUEUE_ITEM));

	pItem->m_eID = MESSAGE_QUEUE_ITEM_ID_RECV_ITEM_RESULT;

	pItem->m_pData = new stAgpdAdminItemOperation;
	memcpy(pItem->m_pData, pstItemOperation, sizeof(stAgpdAdminItemOperation));

	return PushQueue(pItem);
}

BOOL AgcmAdminDlgXT_MessageQueue::PushQueueRecvHelpResult(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp)
		return FALSE;

	MESSAGE_QUEUE_ITEM* pItem = new MESSAGE_QUEUE_ITEM;
	memset(pItem, 0, sizeof(MESSAGE_QUEUE_ITEM));

	pItem->m_eID = MESSAGE_QUEUE_ITEM_ID_RECV_HELP_RESULT;

	pItem->m_pData = new stAgpdAdminHelp;
	memcpy(pItem->m_pData, pstHelp, sizeof(stAgpdAdminHelp));

	return PushQueue(pItem);
}

BOOL AgcmAdminDlgXT_MessageQueue::PushQueueRecvHelpMemoResult(stAgpdAdminHelp* pstHelp)
{
	if(!pstHelp)
		return FALSE;

	MESSAGE_QUEUE_ITEM* pItem = new MESSAGE_QUEUE_ITEM;
	memset(pItem, 0, sizeof(MESSAGE_QUEUE_ITEM));

	pItem->m_eID = MESSAGE_QUEUE_ITEM_ID_RECV_HELP_MEMO_RESULT;

	pItem->m_pData = new stAgpdAdminHelp;
	memcpy(pItem->m_pData, pstHelp, sizeof(stAgpdAdminHelp));

	return PushQueue(pItem);
}
