#ifndef _ARCHLORD_PRIORITYLIST_H
#define _ARCHLORD_PRIORITYLIST_H

#include <windows.h>
#include "ApBase.h"

template < class T, class U > class CPriorityList;

template < class T, class U > class CPriorityListNode {
  public:
    CPriorityListNode();
    CPriorityListNode(const T & KEY, const U & Data);

    ~CPriorityListNode();

    CPriorityListNode *NextNode;
    CPriorityListNode *PrevNode;
	T key;
    U data;
};

template < class T, class U > class CPriorityList {
private:
	ApMutualEx		m_Mutex;	
	bool bLock;

public:
    int count;
	CPriorityListNode < T, U > *start_node;
    CPriorityListNode < T, U > *end_node;

    CPriorityList();
    ~CPriorityList();

	void insertNode(const T & KEY, const U & DATA);
	void priorityinsertNode(const T & KEY, const U & DATA);
    void UniinsertNode(const T & KEY, const U & DATA);
    bool deleteNode(CPriorityListNode < T, U > *CurNode);
	bool deleteNodeByKey(const T &KEY);
	bool deleteNodeByData(const U  &DATA);
    void deleteAllNode();
	void removeAllNode() ;
	int getSize(void) {
		int cnt ;
		lock();
		cnt = count;
		unlock();
		return cnt;
	}
	void setUseLock(bool bUseLock)  {
			bLock=bUseLock;
			return;
	}

    CPriorityListNode<T, U> *SearchByKey(const T KEY);
    CPriorityListNode<T, U> *SearchByData(const U DATA);

    void lock(void);
	void unlock(void);
};

template < class T, class U >
void CPriorityList<T, U>::lock(void)
{
	m_Mutex.WLock();
}

template < class T, class U >
void CPriorityList<T, U>::unlock(void)
{
	m_Mutex.Release();
}

template < class T, class U > 
CPriorityListNode < T, U >::CPriorityListNode()
{
    NextNode = PrevNode = NULL;
}

template < class T, class U > 
CPriorityListNode < T, U >::CPriorityListNode(const T &KEY, const U &DATA)
{
	key = KEY;
    data = DATA;
    NextNode = PrevNode = NULL;
}

template < class T, class U > 
CPriorityListNode < T, U >::~CPriorityListNode()
{
}

template < class T, class U >
CPriorityList < T, U >::CPriorityList()
{
    // 처음에는 시작과 끝이 모두 t_node임
    count = 0;
	bLock=true;
    start_node = end_node = NULL;

	m_Mutex.Init();
}

template < class T, class U > 
CPriorityList < T, U >::~CPriorityList()
{
    deleteAllNode();

	m_Mutex.Destroy();
}

template < class T, class U > 
CPriorityListNode<T,U>*  CPriorityList < T,U >::SearchByData(const U data)
{
    CPriorityListNode < T,U > *tmpNode;
	lock();
    tmpNode = start_node;
    while (tmpNode != NULL) {
		if (tmpNode->data == data) break;
		tmpNode = tmpNode->NextNode;
    }
	unlock();
    return tmpNode;
}

template < class T, class U > 
CPriorityListNode<T,U>*  CPriorityList < T,U >::SearchByKey(const T key)
{
    CPriorityListNode < T,U > *tmpNode;
	lock();
    tmpNode = start_node;
    while (tmpNode != NULL) {
		if (tmpNode->key == key) break;
		tmpNode = tmpNode->NextNode;
    }
	unlock();
    return tmpNode;
}


template < class T, class U > 
void CPriorityList < T, U >::removeAllNode()
{
    lock();

    CPriorityListNode < T, U > *t_node = start_node;
    CPriorityListNode < T, U > *c_node;

    while (t_node) {
		c_node = t_node;
		t_node = t_node->NextNode;
		delete c_node->data;
		delete c_node;
    }

    start_node = end_node = 0;
	count = 0;

    unlock();
}

template < class T, class U > 
void CPriorityList < T,U >::deleteAllNode()
{
    lock();

    CPriorityListNode < T,U > *t_node = start_node;
    CPriorityListNode < T,U > *c_node;

    while (t_node) {
	c_node = t_node;
	t_node = t_node->NextNode;
	delete c_node;
    }

    start_node = end_node = 0;
	count = 0;

    unlock();
}

template < class T, class U > 
bool CPriorityList < T,U >::deleteNodeByData(const U  &DATA)
{
	lock();
	bool bResult=false;
	CPriorityListNode <T, U> * tmpNode = start_node;
   	while (tmpNode != NULL) {
		if (tmpNode->data == DATA ) {
		    //해당 노드가 시작 노드일 때.
		    if (tmpNode == start_node) {
		        start_node = tmpNode->NextNode;
		    }
		    // 데이터 노드가 맨끝 노드와 일치할때
		    else if (tmpNode == end_node) {
			    end_node = tmpNode->PrevNode;
			    tmpNode->PrevNode->NextNode = 0;
		    // 중간 노드일때
		    } else {
		        tmpNode->PrevNode->NextNode = tmpNode->NextNode;
		        tmpNode->NextNode->PrevNode = tmpNode->PrevNode;
		    }
		    count--;
		    delete tmpNode;

			bResult = true;
			break;	
		}
		tmpNode = tmpNode->NextNode;
	}	
	unlock();
	return bResult;
}

template < class T, class U > 
bool CPriorityList < T,U >::deleteNodeByKey(const T  &KEY)
{
	lock();
	bool bResult=false;
	CPriorityListNode <T, U> * tmpNode = start_node;
   	while (tmpNode != NULL) {
		if (tmpNode->key == KEY ) {
		    //해당 노드가 시작 노드일 때.
		    if (tmpNode == start_node) {
		        start_node = tmpNode->NextNode;
		    }
		    // 데이터 노드가 맨끝 노드와 일치할때
		    else if (tmpNode == end_node) {
			    end_node = tmpNode->PrevNode;
			    tmpNode->PrevNode->NextNode = 0;
		    // 중간 노드일때
		    } else {
		        tmpNode->PrevNode->NextNode = tmpNode->NextNode;
		        tmpNode->NextNode->PrevNode = tmpNode->PrevNode;
		    }
		    count--;
		    delete tmpNode;

			bResult = true;
			break;	
		}
		tmpNode = tmpNode->NextNode;
	}	
	unlock();
	return bResult;
}

template < class T, class U > 
bool CPriorityList < T,U >::deleteNode(CPriorityListNode < T,U > *CurNode)
{
    lock();
    if (!CurNode) {
		return false;
    }
    // 데이터가 하나 뿐일때
    if (CurNode == start_node) {
		start_node = CurNode->NextNode;
    }
    // 데이터 노드가 맨끝 노드와 일치할때
    else if (CurNode == end_node) {
	end_node = CurNode->PrevNode;
	CurNode->PrevNode->NextNode = 0;
	// 중간 노드일때
    } else {
		CurNode->PrevNode->NextNode = CurNode->NextNode;
		CurNode->NextNode->PrevNode = CurNode->PrevNode;
    }
    count--;
    delete CurNode;

    unlock();
    return true;
}

template < class T, class U > 
void CPriorityList < T,U >::UniinsertNode(const T & KEY, const U & DATA)
{
    if (SearchByKey(KEY) == NULL) insertNode(KEY, DATA);
}

template < class T, class U > 
void CPriorityList < T,U >::insertNode(const T & KEY, const U & DATA)
{
	lock();

    CPriorityListNode < T, U > *t_node;

    t_node = new CPriorityListNode < T, U > ( KEY, DATA);

    if (start_node) {
		end_node->NextNode = t_node;
		t_node->NextNode = 0;
	    t_node->PrevNode = end_node;
    } else {
		start_node = t_node;
    }

    end_node = t_node;
    count++;

	unlock();
}
template < class T, class U > 
void CPriorityList < T,U >::priorityinsertNode(const T & KEY, const U & DATA)
{
	lock();

    CPriorityListNode < T,U > *t_node;

    t_node = new CPriorityListNode < T,U > (KEY,DATA);

    if (start_node) 
	{
		bool		bInserted = false;

		for( CPriorityListNode< T,U > *pcTmpNode = start_node; pcTmpNode; pcTmpNode = pcTmpNode->NextNode )
		{
			if( t_node->key < pcTmpNode->key )
			{
				if( pcTmpNode == start_node )
				{
					start_node = t_node;
					t_node->NextNode = pcTmpNode;
					pcTmpNode->PrevNode = t_node;
					bInserted = true;

					break;
				}
				else
				{
					pcTmpNode->PrevNode->NextNode = t_node;
					t_node->PrevNode = pcTmpNode->PrevNode;
					t_node->NextNode = pcTmpNode;
					pcTmpNode->PrevNode = t_node;

					bInserted = true;
					break;
				}
			}
		}

		if( bInserted == false )
		{
			end_node->NextNode = t_node;
			t_node->NextNode = 0;
			t_node->PrevNode = end_node;
			end_node = t_node;
		}
    } 
	else 
	{
		start_node = t_node;
		end_node = t_node;
    }

    count++;
	unlock();
}

#endif
