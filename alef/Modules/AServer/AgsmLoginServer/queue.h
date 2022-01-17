#ifndef _AUOLEDB_QUEUE_H_
	#define _AUOLEDB_QUEUE_H_

#include "Winbase.h"


template <class T> class CQueueNode
{
public:
	T				m_Data;
	CQueueNode<T>	*m_pcNextNode;

	CQueueNode()
	{
		m_Data = NULL;
		m_pcNextNode = NULL;
	}

	T getData() { return m_Data; }
};

template <class T> class CQueue
{
private:
	CQueueNode<T>	*m_pcStartNode;
	CQueueNode<T>	*m_pcEndNode;

	CRITICAL_SECTION	m_cCriticalSection;

public:
	unsigned int	m_iCount;
	unsigned int	m_iMaxCount;

	CQueue()
	{
		m_iCount = 0;
		m_iMaxCount = 0xFFFFFFFF; //특별히 언급이 없으면 40억개까지 만든다.
		m_pcStartNode = NULL;
		m_pcEndNode = NULL;

		InitializeCriticalSection( &m_cCriticalSection );

	}
	CQueue( unsigned int iMaxCount )
	{
		m_iCount = 0;
		m_iMaxCount = iMaxCount;
		m_pcStartNode = NULL;
		m_pcEndNode = NULL;

		InitializeCriticalSection( &m_cCriticalSection );
	}
	~CQueue()
	{
		removeAll();

		DeleteCriticalSection( &m_cCriticalSection );
	}

	unsigned int getCount();
	bool push( T Data );
	bool push( T Data, char *pstrFileName, int iLine );
	T pop();
	void removeAll();

	void lock();
	void unlock();
};

template <class T> 
unsigned int CQueue<T>::getCount()
{
	int				iCount;

	lock();

	iCount = m_iCount;

	unlock();

	return iCount;
}

template <class T> 
bool CQueue<T>::push( T Data )
{
	bool			bResult;

	lock();

	//아무것도 리스트에 없다면?
	if( m_pcStartNode == NULL )
	{
		CQueueNode<T>	*pcTempQueueNode;
		
		pcTempQueueNode = new CQueueNode<T>;

		//printf( "[1][1]PushNode:%p, PushData:%p\n", pcTempQueueNode, Data );

		if( pcTempQueueNode != NULL )
		{
			m_iCount++;

			pcTempQueueNode->m_Data = Data;
			pcTempQueueNode->m_pcNextNode = NULL;
			m_pcStartNode = pcTempQueueNode;
			m_pcEndNode = pcTempQueueNode;
			bResult = true;
		}
		else
		{
			printf( "Push 실패! Memory 부족?\n" );
			bResult = false;
		}
	}
	else
	{
		//지정된 갯수보다 많이 넣으려고 하면 실패한다.
		if( m_iCount >= m_iMaxCount )
		{
			bResult = false;
		}
		//지정된 갯수보다 적다면 넣는다.
		else
		{
			CQueueNode<T>	*pcTempQueueNode;
			
			pcTempQueueNode = new CQueueNode<T>;

			if( pcTempQueueNode != NULL )
			{
				//printf( "[2][2]PushNode:%p, PushData:%p\n", pcTempQueueNode, Data );

				m_iCount++;

				pcTempQueueNode->m_Data = Data;
				pcTempQueueNode->m_pcNextNode = NULL;
				m_pcEndNode->m_pcNextNode = pcTempQueueNode;
				m_pcEndNode = pcTempQueueNode;
				bResult = true;
			}
			else
			{
				printf( "Push 실패! Memory 부족?\n" );
				bResult = false;
			}
		}
	}

	unlock();

	return bResult;
}

template <class T> 
bool CQueue<T>::push( T Data, char *pstrFileName, int iLine )
{
	bool			bResult;

	lock();

	//아무것도 리스트에 없다면?
	if( m_pcStartNode == NULL )
	{
		CQueueNode<T>	*pcTempQueueNode;
		
		pcTempQueueNode = new CQueueNode<T>;

		//printf( "[1][1]PushNode:%p, PushData:%p, FileName:%s, Line:%d\n", pcTempQueueNode, Data, pstrFileName, iLine );

		if( pcTempQueueNode != NULL )
		{
			m_iCount++;

			pcTempQueueNode->m_Data = Data;
			pcTempQueueNode->m_pcNextNode = NULL;
			m_pcStartNode = pcTempQueueNode;
			m_pcEndNode = pcTempQueueNode;
			bResult = true;
		}
		else
		{
			printf( "Push 실패! Memory 부족?\n" );
			bResult = false;
		}
	}
	else
	{
		//지정된 갯수보다 많이 넣으려고 하면 실패한다.
		if( m_iCount >= m_iMaxCount )
		{
			bResult = false;
		}
		//지정된 갯수보다 적다면 넣는다.
		else
		{
			CQueueNode<T>	*pcTempQueueNode;
			
			pcTempQueueNode = new CQueueNode<T>;

			if( pcTempQueueNode != NULL )
			{
				//printf( "[2][2]PushNode:%p, PushData:%p, FileName:%s, Line:%d\n", pcTempQueueNode, Data, pstrFileName, iLine );

				m_iCount++;

				pcTempQueueNode->m_Data = Data;
				pcTempQueueNode->m_pcNextNode = NULL;
				m_pcEndNode->m_pcNextNode = pcTempQueueNode;
				m_pcEndNode = pcTempQueueNode;
				bResult = true;
			}
			else
			{
				printf( "Push 실패! Memory 부족?\n" );
				bResult = false;
			}
		}
	}

	unlock();

	return bResult;
}

template <class T> 
T CQueue<T>::pop()
{
	T				tTempData = NULL;

	lock();

	if( m_pcStartNode != NULL )
	{
		CQueueNode<T>	*pcReturnNode;

		pcReturnNode = m_pcStartNode;
		m_pcStartNode = m_pcStartNode->m_pcNextNode;

		//printf( "^1^^1^PopNode : %p, PopData : %p\n", pcReturnNode, pcReturnNode->m_Data );

		if( pcReturnNode != NULL )
		{
			tTempData = pcReturnNode->m_Data;
			delete pcReturnNode;

			m_iCount--;
		}
		else
		{
			tTempData = NULL;
		}
	}
	else
	{
		tTempData = NULL;
	}

	unlock();

	return tTempData;
}

template <class T> 
void CQueue<T>::removeAll()
{
	lock();

	CQueueNode<T>	*pcNextNode;

	for( CQueueNode<T>	*pcTempNode = m_pcStartNode; pcTempNode != NULL; )
	{
		pcNextNode = pcTempNode->m_pcNextNode;

		if( pcTempNode != NULL )
			delete pcTempNode;

		pcTempNode = pcNextNode;
	}

	unlock();
}

template <class T> 
void CQueue<T>::lock()
{
	EnterCriticalSection( &m_cCriticalSection );
}

template <class T> 
void CQueue<T>::unlock()
{
	LeaveCriticalSection( &m_cCriticalSection );
}

#endif