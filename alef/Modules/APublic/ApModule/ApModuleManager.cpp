/******************************************************************************
Module:  ApModuleManagerA.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 23
******************************************************************************/

#include "ApModuleManager.h"
#include "MagDebug.h"
#include "ApLockManager.h"
#include "ApMemoryTracker.h"

#define LOG_TIME 500

//		Constructor
//	Functions
//		- initialize variables
//	Arguments
//		- none
//	Return value
//		- none
///////////////////////////////////////////////////////////////////////////////
ApModuleManager::ApModuleManager()
{
	m_listModule = NULL;
	m_listModuleIdle = NULL;
	m_listModuleIdle2 = NULL;
	m_listModuleIdle3 = NULL;

	ZeroMemory(m_pModuleDisp, sizeof(ApModule *) * APPACKET_MAX_PACKET_TYPE);
	m_ulClockCount = 0;

//	m_csTickCounter.Initialize();

	m_pSlowIdleLogClass = NULL;
	m_pfSlowIdleLogCB = NULL;
	m_pSlowDispatchLogClass = NULL;
	m_pfSlowDispatchLogCB = NULL;

	m_csSystemPacket.SetFlagLength(sizeof(INT8));
	m_csSystemPacket.SetFieldType(
								  AUTYPE_INT8, 1,			// Client Termination Code (exit, crash)
								  AUTYPE_MEMORY_BLOCK, 1,	// Client Crash Reason
								  AUTYPE_END, 0
								  );
}

//		Destructor
///////////////////////////////////////////////////////////////////////////////
ApModuleManager::~ApModuleManager()
{
}

//		Initialize
//	Functions
//		- 모듈풀의 모듈들에 대한 OnInit()를 실행한다.
//	Arguments
//
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Initialize()
{
	m_csTickCounter.Initialize();

	ApModuleList*		cur_data = m_listModule;

	for (int i = 1; i < 20; ++i)
	{
		if (TIMERR_NOERROR == timeBeginPeriod(i))
			break;
	}

	while(cur_data)
	{
		TRACE( "ApModuleManager::Initialize() , %s Module Init...\n" , cur_data->pModule->GetModuleName() );

		if(cur_data->pModule)
		{
			if(!cur_data->pModule->OnInit())
			{
				MessageBox(NULL, cur_data->pModule->GetModuleName(), "Init Error", MB_OK);
				TRACE( "ApModuleManager::Initialize() , %s Module Init Failed !\n" , cur_data->pModule->GetModuleName() );
				return FALSE;
			}

			cur_data->pModule->m_eModuleStatus = APMODULE_STATUS_READY;
		}
		cur_data = cur_data->next;
	}
	
	return TRUE;
}

//		AddModule
//	Functions
//		- 모듈풀에 모듈을 등록한다.
//	Arguments
//		- pModule : 등록할 모듈 포인터
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::AddModule(ApModule* pModule)
{
	if (pModule->m_eModuleStatus != APMODULE_STATUS_INIT || GetModule(pModule->GetModuleName()))
	{
		// already added
		return FALSE;
	}

	ApModuleList*		nw_M = new ApModuleList;
	nw_M->pModule = pModule;
	nw_M->next = NULL;

	if(m_listModule == NULL)
	{
		m_listModule = nw_M;
	}
	else
	{
		ApModuleList*		find_data = m_listModule;
		while(find_data->next)
		{
			find_data = find_data->next;
		}

		find_data->next = nw_M;
	}
	
	// Packet Type이 있다면 등록시킨다.
	for (int i = 0; i < APMODULE_MAX_MODULEPACKET_TYPE; ++i)
	{
		if (pModule->m_ulPacketType[i])
		{
			// 이미 등록된 Packet에 대한 Type 이면, FALSE
			if (pModule->m_ulPacketType[i] >= APPACKET_MAX_PACKET_TYPE || m_pModuleDisp[pModule->m_ulPacketType[i]])
				return FALSE;

			m_pModuleDisp[pModule->m_ulPacketType[i]] = pModule;
		}
		else
			break;
	}

	if (pModule->m_bUseIdle)
	{
		ApModuleList*		nw_MI = new ApModuleList;
		nw_MI->pModule = pModule;
		nw_MI->next = NULL;

		if(m_listModuleIdle == NULL)
		{
			m_listModuleIdle = nw_MI;
		}
		else
		{
			ApModuleList*		find_data = m_listModuleIdle;
			while(find_data->next)
			{
				find_data = find_data->next;
			}

			find_data->next = nw_MI;
		}
	}

	if (pModule->m_bUseIdle2)
	{
		ApModuleList*		nw_MI = new ApModuleList;
		nw_MI->pModule = pModule;
		nw_MI->next = NULL;

		if(m_listModuleIdle2 == NULL)
		{
			m_listModuleIdle2 = nw_MI;
		}
		else
		{
			ApModuleList*		find_data = m_listModuleIdle2;
			while(find_data->next)
			{
				find_data = find_data->next;
			}

			find_data->next = nw_MI;
		}
	}

	if (pModule->m_bUseIdle3)
	{
		ApModuleList*		nw_MI = new ApModuleList;
		nw_MI->pModule = pModule;
		nw_MI->next = NULL;

		if(m_listModuleIdle3 == NULL)
		{
			m_listModuleIdle3 = nw_MI;
		}
		else
		{
			ApModuleList*		find_data = m_listModuleIdle3;
			while(find_data->next)
			{
				find_data = find_data->next;
			}

			find_data->next = nw_MI;
		}
	}

	pModule->m_eModuleStatus = APMODULE_STATUS_ADDED;

	OnAddModule();

#ifdef _DEBUG
	MEMORYSTATUS	stMemory;

	GlobalMemoryStatus(&stMemory);
	TRACE("Added Module (%s) - Allocation : %d\n", pModule->GetModuleName(), stMemory.dwTotalVirtual - stMemory.dwAvailVirtual);
#endif

//	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	return TRUE;
}

//		RemoveModule
//	Functions
//		- 모듈풀에 있는 모듈을 없앤다.
//	Arguments
//		- pModule : 삭제할 모듈 포인터
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::RemoveModule(CHAR* szModuleName)
{
	if (m_listModule == NULL)
	{
		// pool is empty
		return FALSE;
	}

	ApModule* pModule = GetModule(szModuleName);

	if(m_listModule->pModule == pModule)
	{
		ApModuleList*	remove_node = m_listModule;
		m_listModule = m_listModule->next;

		delete remove_node;
	}
	else
	{
		ApModuleList*		cur_node = m_listModule;
		ApModuleList*		bef_node = cur_node;

		while(cur_node)
		{
			if(cur_node->pModule == pModule)
			{
				ApModuleList*		remove_node = cur_node;
				delete remove_node;
				break;
			}

			bef_node = cur_node;
			cur_node = cur_node->next;
		}
	}
	
	return TRUE;
}

//		GetModule
//	Functions
//		- 모듈풀에 있는 모듈을 검색해서 모듈을 찾아 넘긴다.
//	Arguments
//		- szModuleName : 찾는 module name
//	Return value
//		- ApModule * : 찾은 모듈 포인터. 못찾음 NULL 을 리턴
///////////////////////////////////////////////////////////////////////////////
ApModule* ApModuleManager::GetModule(CHAR* szModuleName)
{
	ApModuleList*		find_node = m_listModule;

	while(find_node)
	{
		if(!strcmp(find_node->pModule->GetModuleName(),szModuleName))
		{
			return find_node->pModule;
		}

		find_node = find_node->next;
	}

	// search failed...
	return NULL;
}

//		Dispatch
//	Functions
//		- 들어오는 데이터를 등록된 해당 모듈로 (들어오는 데이타를 처리할 모듈로) 넘겨준다.
//	Arguments
//		- szData : 넘어온 데이타
//		- nSize : 데이타 크기
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Dispatch(CHAR* szData, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	// 데이타 타입 등등을 보고 해당 모듈을 얻어와 그 모듈에 데이타를 넘겨준다.
	// 즉, 그 모듈의 MessageHandler()를 호출한다.
	if (!szData || nSize < 1)
		return FALSE;

	//PacketMonitoring(szData, nSize, FALSE);

	PPACKET_HEADER pstPacket = (PPACKET_HEADER) szData;

	if (pstPacket->cType == ALEF_SYSTEM_PACKET_TYPE)
	{
		if (!OnSPReceive(pstPacket->cType, pstPacket, nSize, ulNID, pstCheckArg))
		{
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();

			return FALSE;
		}
	}
	else if (pstPacket->cType > 0 && pstPacket->cType < APPACKET_MAX_PACKET_TYPE && m_pModuleDisp[pstPacket->cType])
	{
		BOOL	bOnPreReceiveResult	= m_pModuleDisp[pstPacket->cType]->EnumPreRecieveModule(pstPacket->cType, pstPacket, nSize, ulNID, pstCheckArg);

		if (!bOnPreReceiveResult)
		{
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();

			return FALSE;
		}

		//INT32	lTick = timeGetTime();

		BOOL	bOnReceiveResult	= m_pModuleDisp[pstPacket->cType]->OnReceive(pstPacket->cType, pstPacket, nSize, ulNID, pstCheckArg);

		if (!bOnReceiveResult)
		{
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();
			
			return FALSE;
		}

		/*
		lTick = timeGetTime() - lTick;
#ifdef _DEBUG
		if (lTick >= 100)
			TRACEFILE(ALEF_ERROR_FILENAME, "ApModuleManager::Dispatch() Too slow %dms (%s)\n", lTick, m_pModuleDisp[pstPacket->cType]->GetModuleName());
#endif // _DEBUG

		if(lTick >= LOG_TIME)
			WriteSlowDispatchLog(m_pModuleDisp[pstPacket->cType]->GetModuleName(), lTick);
		*/
	}

	if (ApLockManager::Instance()->IsExistData())
		ApLockManager::Instance()->AllUnlock();

//	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	return TRUE;
}

BOOL ApModuleManager::OnSPReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	INT8	cClientCode = APMM_PACKET_CLIENT_CODE_UNKNOWN;
	CHAR *	szCrashReason = NULL;
	INT16	unReasonLength = 0;

	if (!pvPacket || nSize <= 0)
		return FALSE;

	m_csSystemPacket.GetField(TRUE, pvPacket, nSize, 
							  &cClientCode, 
							  &szCrashReason, &unReasonLength);

	if (cClientCode == APMM_PACKET_CLIENT_CODE_EXIT)
		return OnSPClientExit(ulNID);
	else if (cClientCode == APMM_PACKET_CLIENT_CODE_CRASH)
		return OnSPClientCrash(ulNID, szCrashReason);

	return TRUE;
}

PVOID ApModuleManager::MakeSPClientExit(INT16 *pnSize)
{
	INT8	cClientCode = APMM_PACKET_CLIENT_CODE_EXIT;

	return m_csSystemPacket.MakePacket(TRUE, pnSize, ALEF_SYSTEM_PACKET_TYPE,
									   &cClientCode,
									   NULL);
}

PVOID ApModuleManager::MakeSPClientCrash(INT16 *pnSize, CHAR *szReason)
{
	INT8	cClientCode = APMM_PACKET_CLIENT_CODE_CRASH;
	INT16	nReasonLength = 0;

	if (szReason)
		nReasonLength = (INT16)strlen(szReason);

	return m_csSystemPacket.MakePacket(TRUE, pnSize, ALEF_SYSTEM_PACKET_TYPE,
									   &cClientCode,
									   szReason, nReasonLength);
}

VOID ApModuleManager::EnableModuleIdle(ApModule* pModule, BOOL bUseIdle)
{
	if (pModule->m_bUseIdle == bUseIdle)
		return;

	pModule->m_bUseIdle = bUseIdle;

	if (pModule->m_eModuleStatus == APMODULE_STATUS_INIT)
		return;

	ApModuleList *		pcsCurList			= m_listModuleIdle;
	ApModuleList *		pcsPrevList			= NULL;

	if (pModule->m_bUseIdle)
	{
		pcsCurList = new ApModuleList;

		pcsCurList->pModule = pModule;
		pcsCurList->next = NULL;

		if(m_listModuleIdle == NULL)
		{
			m_listModuleIdle = pcsCurList;
		}
		else
		{
			pcsPrevList = m_listModuleIdle;
			while(pcsPrevList->next)
			{
				pcsPrevList = pcsPrevList->next;
			}

			pcsPrevList->next = pcsCurList;
		}
	}
	else
	{
		while(pcsCurList)
		{
			if(pcsCurList->pModule == pModule)
			{
				if (!pcsPrevList)
				{
					m_listModuleIdle = pcsCurList->next;
				}
				else
				{
					pcsPrevList->next = pcsCurList->next;
				}

				delete pcsCurList;

				return;
			}

			pcsPrevList = pcsCurList;

			pcsCurList = pcsCurList->next;
		}
	}
}

VOID ApModuleManager::EnableModuleIdle2(ApModule* pModule, BOOL bUseIdle)
{
	if (pModule->m_bUseIdle2 == bUseIdle)
		return;

	pModule->m_bUseIdle2 = bUseIdle;

	if (pModule->m_eModuleStatus == APMODULE_STATUS_INIT)
		return;

	ApModuleList *		pcsCurList			= m_listModuleIdle2;
	ApModuleList *		pcsPrevList			= NULL;

	if (pModule->m_bUseIdle2)
	{
		pcsCurList = new ApModuleList;

		pcsCurList->pModule = pModule;
		pcsCurList->next = NULL;

		if(m_listModuleIdle2 == NULL)
		{
			m_listModuleIdle2 = pcsCurList;
		}
		else
		{
			pcsPrevList = m_listModuleIdle2;
			while(pcsPrevList->next)
			{
				pcsPrevList = pcsPrevList->next;
			}

			pcsPrevList->next = pcsCurList;
		}
	}
	else
	{
		while(pcsCurList)
		{
			if(pcsCurList->pModule == pModule)
			{
				if (!pcsPrevList)
				{
					m_listModuleIdle2 = pcsCurList->next;
				}
				else
				{
					pcsPrevList->next = pcsCurList->next;
				}

				delete pcsCurList;

				return;
			}

			pcsPrevList = pcsCurList;

			pcsCurList = pcsCurList->next;
		}
	}
}

VOID ApModuleManager::EnableModuleIdle3(ApModule* pModule, BOOL bUseIdle)
{
	if (pModule->m_bUseIdle3 == bUseIdle)
		return;

	pModule->m_bUseIdle3 = bUseIdle;

	if (pModule->m_eModuleStatus == APMODULE_STATUS_INIT)
		return;

	ApModuleList *		pcsCurList			= m_listModuleIdle3;
	ApModuleList *		pcsPrevList			= NULL;

	if (pModule->m_bUseIdle3)
	{
		pcsCurList = new ApModuleList;

		pcsCurList->pModule = pModule;
		pcsCurList->next = NULL;

		if(m_listModuleIdle3 == NULL)
		{
			m_listModuleIdle3 = pcsCurList;
		}
		else
		{
			pcsPrevList = m_listModuleIdle3;
			while(pcsPrevList->next)
			{
				pcsPrevList = pcsPrevList->next;
			}

			pcsPrevList->next = pcsCurList;
		}
	}
	else
	{
		while(pcsCurList)
		{
			if(pcsCurList->pModule == pModule)
			{
				if (!pcsPrevList)
				{
					m_listModuleIdle3 = pcsCurList->next;
				}
				else
				{
					pcsPrevList->next = pcsCurList->next;
				}

				delete pcsCurList;

				return;
			}

			pcsPrevList = pcsCurList;

			pcsCurList = pcsCurList->next;
		}
	}
}

//		Idle
//	Functions
//		- 등록된 모듈들의 OnIdle() 함수를 호출한다.
//	Arguments
//		- ulClockCount : engine clock count
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Idle(UINT32 ulClockCount)
{
	PROFILE("ApModuleManager::Idle");

	m_ulClockCount = ulClockCount;

	ApModuleList*		cur_node= m_listModuleIdle;
//	INT32 lTick;

	while(cur_node)
	{
		//lTick = timeGetTime();
		//STOPWATCH2( cur_node->pModule->GetModuleName(), _T("OnIdle") , 10 );

		cur_node->pModule->Idle(ulClockCount);

		//lTick = timeGetTime() - lTick;

		//if(lTick >= LOG_TIME)
		//	WriteSlowIdleLog(cur_node->pModule->GetModuleName(), lTick);

		cur_node = cur_node->next;
	}

//	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	return TRUE;
}

//		Idle2
//	Functions
//		- 등록된 모듈들의 OnIdle2() 함수를 호출한다.
//	Arguments
//		- ulClockCount : engine clock count
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Idle2(UINT32 ulClockCount)
{
	PROFILE("ApModuleManager::Idle2");

	m_ulClockCount = ulClockCount;

	ApModuleList*		cur_node= m_listModuleIdle2;
//	INT32 lTick;

	while(cur_node)
	{
		//lTick = timeGetTime();

		cur_node->pModule->Idle2(ulClockCount);

		//lTick = timeGetTime() - lTick;

		//if(lTick >= LOG_TIME)
		//	WriteSlowIdleLog(cur_node->pModule->GetModuleName(), lTick);

		cur_node = cur_node->next;
	}

//	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	return TRUE;
}

//		Idle3
//	Functions
//		- 등록된 모듈들의 OnIdle3() 함수를 호출한다.
//	Arguments
//		- ulClockCount : engine clock count
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Idle3(UINT32 ulClockCount)
{
	PROFILE("ApModuleManager::Idle3");

	m_ulClockCount = ulClockCount;

	ApModuleList*		cur_node= m_listModuleIdle3;
//	INT32 lTick;

	while(cur_node)
	{
		//lTick = timeGetTime();

		cur_node->pModule->Idle3(ulClockCount);

		//lTick = timeGetTime() - lTick;

		//if(lTick >= LOG_TIME)
		//	WriteSlowIdleLog(cur_node->pModule->GetModuleName(), lTick);

		cur_node = cur_node->next;
	}

//	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	return TRUE;
}

//		Destroy
//	Functions
//		- 등록된 모듈들의 OnDestroy() 함수를 호출한다.
//	Arguments
//		- none
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL ApModuleManager::Destroy()
{
	INT32	nMax = GetModuleCount();
	if(nMax > 0)
	{
		for(int i = nMax-1;i>=0;--i)
		{
			ApModule*		pModule = GetModule(i);
			TRACE( "Module PreDestroy - %s\n" , pModule->GetModuleName() );
			if(!pModule->OnPreDestroy())
			{
#ifdef _DEBUG
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s 모듈 Destroy 실패!!\n" , pModule->GetModuleName());
				AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
#endif
			}

			pModule->m_eModuleStatus = APMODULE_STATUS_PREDESTROYED;
		}
		
		for(int i = nMax-1;i>=0;--i)
		{
			ApModule*		pModule = GetModule(i);
			TRACE( "Module Destroy - %s\n" , pModule->GetModuleName() );
			if(!pModule->OnDestroy())
			{
#ifdef _DEBUG
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "%s 모듈 Destroy 실패!!\n" , pModule->GetModuleName());
				AuLogFile_s(ALEF_ERROR_FILENAME, strCharBuff);
#endif
			}

			pModule->m_eModuleStatus = APMODULE_STATUS_DESTROYED;

//			ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);
		}
		
		// 마고자 (2004-03-25 오후 10:17:45) : 모듈 메모리에서 제거함..
		// 이전의 방식으로 하면 , 콜벡 등록한것들이 이상하게 꼬여버림..
		/*
		for( i = nMax-1;i>=0;--i)
		{
			ApModule*		pModule = GetModule(i);

			if (pModule->m_bDynamicAllocated)
				delete pModule;
		}
		*/

		for(int i = 0; i < nMax; ++i)
		{
			ApModule*		pModule = GetModule(i);

			if (pModule->m_bDynamicAllocated)
			{
				TRACE( "Module delete - %s\n" , pModule->GetModuleName() );
				delete pModule;
			}

//			ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);
		}
	}

	/*while(cur_node)
	{
		if(cur_node->pModule)
		{
			TRACE( "Module Destroy - %s\n" , cur_node->pModule->GetModuleName() );
			if(!cur_node->pModule->OnDestroy())
			{
				TRACE( "%s 모듈 Destroy 실패!!\n" , cur_node->pModule->GetModuleName() );
			}

		}

		cur_node = cur_node->next;
	}*/

	ApModuleList*		cur_node = m_listModule;
	ApModuleList*		remove_node;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	cur_node = m_listModuleIdle;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	cur_node = m_listModuleIdle2;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	cur_node = m_listModuleIdle3;
	while(cur_node)
	{
		remove_node = cur_node;
		cur_node = cur_node->next;

		delete remove_node;
	}

	m_listModule = NULL;
	m_listModuleIdle = NULL;
	m_listModuleIdle2 = NULL;

	return TRUE;
}

UINT32 ApModuleManager::GetPrevClockCount()
{
	return m_ulClockCount;
}

UINT32 ApModuleManager::GetClockCount()
{
	return m_csTickCounter.GetTickCount();
}

INT32	ApModuleManager::GetModuleCount()
{
	ApModuleList*		cur_node = m_listModule;
	INT32		count = 0;
	while(cur_node)
	{
		++count;
		cur_node =cur_node->next;
	}
	return count;
}

ApModule*	ApModuleManager::GetModule(INT32 index)
{
	ApModuleList*		cur_node = m_listModule;
	while(cur_node)
	{
		if(index-- == 0) return cur_node->pModule;
		cur_node =cur_node->next;
	}
	return NULL;
}

BOOL ApModuleManager::WriteSlowIdleLog(CHAR* szModuleName, INT32 lTick)
{
	if(!szModuleName)
		return FALSE;

	if(!m_pSlowIdleLogClass || !m_pfSlowIdleLogCB)
		return FALSE;

	m_pfSlowIdleLogCB(szModuleName, m_pSlowIdleLogClass, &lTick);
	return TRUE;
}

BOOL ApModuleManager::WriteSlowDispatchLog(CHAR* szModuleName, INT32 lTick)
{
	if(!szModuleName)
		return FALSE;

	if(!m_pSlowDispatchLogClass || !m_pfSlowDispatchLogCB)
		return FALSE;

	m_pfSlowDispatchLogCB(szModuleName, m_pSlowDispatchLogClass, &lTick);
	return TRUE;
}

// Report
BOOL ApModuleManager::ReportAll()
{
	CHAR szFile[_MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
		
	sprintf(szFile, "Report-%04d%02d%02d-%02d%02d%02d.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	
	FILE *fp = fopen(szFile, "a+");
	if (!fp)
		return FALSE;
	
	ApModuleList*		cur_data = m_listModule;

	while(cur_data)
	{
		if(cur_data->pModule)
		{
			cur_data->pModule->Report_Head(fp);
			cur_data->pModule->Report(fp);
		}
		
		cur_data = cur_data->next;
	}
	
	fclose(fp);
	
	return TRUE;
}

BOOL ApModuleManager::ReportModule(CHAR *pszModuleName, CHAR *pszFileName)
{
	if (!pszModuleName)
		return FALSE;
	
	ApModule *pModule = GetModule(pszModuleName);
	if (!pModule)
		return FALSE;

	CHAR szFile[_MAX_PATH];
	if (pszFileName)
	{	
		strcpy(szFile, pszFileName);
	}
	else
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		
		sprintf(szFile, "Report-%04d%02d%02d-%02d%02d%02d-%s.log", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, pszModuleName);
	}
	
	FILE *fp = fopen(szFile, "a+");
	if (!fp)
		return FALSE;
	
	pModule->Report_Head(fp);
	pModule->Report(fp);
	
	fclose(fp);
	
	return TRUE;
}