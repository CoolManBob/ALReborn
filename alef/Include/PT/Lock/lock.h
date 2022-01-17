#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//	说明：	本程序用于多线程数据享中的互斥，实现了易用的资源锁、程序锁，实现方便的
//			资源管理、死锁检查、死锁恢复。
//
//	功能：	数据资源锁、程序代码段锁、资源管理、死锁检查、死锁恢复
//
//	特点：	1，函数返回后自动开锁，在多分枝控制中减少开锁语句，并避免忘记开锁
//			2，自动“锁－开锁”配对，避免多开或少开
//			3，带管理资源自动加入资源管理
//			4，资源管理器自动生成，无需另外的管理
//			5，完全面向对象
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////
//
//		CWithLock:	被共享实例的类
//		CLock:		用来锁CWithLock实例的类
//		CWithManagedLock:	被共享实例的类，并被资源管理系统管理自动管理
//		CResourceManage:	自动资源管理器，管理所有CWithManagedLock实例，并能检查死锁
//
//////////////////////////////////////////////////////////////////////////////////////
#include "sdkconfig.h"
//	锁、开锁，一个代码段只能使用一个，即在｛｝内第一层，只能LOCK一次
#define LOCK(object)		CLock Lock(object);
#define UNLOCK()			Lock.UnLock();

//	有名称锁，可以随时使用，如果在同一个代码段，请使用不同的名称
#define LOCK2(object,name)	CLock Lock_##name(object);
#define UNLOCK2(name)		Lock_##name.UnLock()

//	代码锁，锁后到开锁之间的代码是不会重入的
#define LOCK_CODE()			static CWithLock CodeWithLock; CLock CodeLock(CodeWithLock);
#define UNLOCK_CODE()		CodeLock.UnLock();

//	有名称代码锁，用于在同一代码段中多次使用的代码锁
#define LOCK_CODE2(name)	static CWithLock CodeWithLock_##name; CLock CodeLock_##name(CodeWithLock_##name);
#define UNLOCK_CODE2(name)	CodeLock##name.UnLock();

//	使用独立的命名空间
namespace CleverLock
{

	class CWithLock;
	class CLock;
	//class CResourceManage;
	//class CWithManagedLock;
	//class CGarbo;

	// 被共享实例的类
	class PTSDK_STUFF CWithLock
	{
	public:
		CWithLock();
		~CWithLock();
		HANDLE	GetOwnerThread();

	protected:
		CRITICAL_SECTION	m_csAccess;

		inline	void Lock();
		inline	void UnLock();
		inline	BOOL TryLock();

		friend class CLock;
	};

	// 用来锁CWithLock实例的类
	class PTSDK_STUFF CLock
	{
	public:
		CLock( CWithLock &door, bool bLook = true);
		~CLock();

		void Lock();
		void UnLock();
		BOOL TryLock();

		static void Lock(CWithLock &door);
		static void UnLock(CWithLock &door);
	private:
		bool m_bLocked;
		CWithLock * m_pDoor;
	};

	////自动资源管理器，管理所有CWithManagedLock实例，并能检查死锁 
	//class CResourceMgr: public CWithLock
	//{
	//public:
	//	~CResourceMgr();
	//	static CResourceMgr * GetInstance();

	//	CWithManagedLock * CheckDeadLock();

	//private:	
	//	CResourceMgr();
	//	static CResourceMgr * m_pInstance;
	//	CArray<CWithManagedLock *, CWithManagedLock *> m_aResourceArray;

	//protected:
	//	void AddResource(CWithManagedLock * p_resource);
	//	void RemoveResource(CWithManagedLock * p_resource);

	//	friend CWithManagedLock;

	//	class CGarbo
	//	{
	//	public:
	//		~CGarbo()
	//		{
	//			if (CResourceMgr::m_pInstance)
	//				delete CResourceMgr::m_pInstance;
	//		}
	//	};

	//	static CGarbo Garbo;
	//};

	//// 被共享实例的类，并被资源管理系统管理自动管理
	//class CWithManagedLock: public CWithLock
	//{
	//public:
	//	CWithManagedLock();
	//	~CWithManagedLock();

	//	void	ForceFree();
	//	HANDLE	GetWaitingThread(int index);

	//protected:
	//	CWithLock m_WaitQueueLock;	
	//	CArray<HANDLE, HANDLE> m_aWaitingThread;

	//	virtual	void Lock();

	//	friend CResourceMgr;
	//};

	//CResourceMgr::CGarbo CResourceMgr::Garbo;
};

