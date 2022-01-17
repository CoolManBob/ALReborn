#ifndef __ASHULAM_DLIST__
#define __ASHULAM_DLIST__

#include "ApBase.h"

template <class T> class CListNode
{
public:
	T				m_tData;
	CListNode<T>	*m_pcPrevNode;
	CListNode<T>	*m_pcNextNode;

	CListNode()
	{
		m_tData = NULL;
		m_pcNextNode = NULL;
		m_pcPrevNode = NULL;
	}

	T getData() { return m_tData; }
};

template <class T> class CDList
{
	CListNode<T>	*m_pcStartNode;
	CListNode<T>	*m_pcEndNode;

	HANDLE			m_hMutex;
	ApMutualEx		m_Mutex;

public:
	int				m_iCount;

	CDList()
	{
		m_iCount = 0;
		m_pcStartNode = NULL;
		m_pcEndNode = NULL;

		//m_hMutex = CreateMutex( NULL, FALSE, NULL );
		m_Mutex.Init();
	}
	~CDList()
	{
		Destroy();

		//CloseHandle( m_hMutex );
		m_Mutex.Destroy();
	}

	CListNode<T> *GetEndNode();
	CListNode<T> *GetStartNode();

	int	 getCount();
	bool Add( T tData );
	bool AddHead( T tData );
	CListNode<T> *Search( T tData );
	bool Remove( T tData );
	void Destroy();

	void lock();
	void unlock();

	void debug();
};

template <class T> 
CListNode<T> *CDList<T>::GetStartNode()
{
	CListNode<T> *pcTempNode;

	lock();

	pcTempNode = m_pcStartNode;

	unlock();

	return pcTempNode;
}

template <class T> 
CListNode<T> *CDList<T>::GetEndNode()
{
	CListNode<T> *pcTempNode;

	lock();

	pcTempNode = m_pcEndNode;

	unlock();

	return pcTempNode;
}

template <class T> 
int CDList<T>::getCount()
{
	int			iResult;

	lock();

	iResult = m_iCount;

	unlock();

	return iResult;
}

template <class T> 
bool CDList<T>::Add( T tData )
{
	CListNode<T>	*pcListNode;
	bool			bResult;

	bResult = false;

	lock();

	if( m_pcStartNode == NULL )
	{
		pcListNode = new CListNode<T>;

		if( pcListNode != NULL )
		{
			m_pcStartNode = pcListNode;
			m_pcEndNode = pcListNode;

			pcListNode->m_tData = tData;
			bResult = true;
		}
		else
		{
			bResult = false;
		}
	}
	else
	{
		pcListNode = new CListNode<T>;

		if( pcListNode != NULL )
		{
			pcListNode->m_pcPrevNode = m_pcEndNode;
			m_pcEndNode->m_pcNextNode = pcListNode;
			m_pcEndNode = pcListNode;

			pcListNode->m_tData = tData;
			bResult = true;
		}
		else
		{
			bResult = false;
		}
	}

	m_iCount++;

	unlock();

	return bResult;
}

template <class T> 
bool CDList<T>::AddHead( T tData )
{
	CListNode<T>	*pcListNode;
	bool			bResult;

	bResult = false;

	lock();

	if( m_pcStartNode == NULL )
	{
		pcListNode = new CListNode<T>;

		if( pcListNode != NULL )
		{
			m_pcStartNode = pcListNode;
			m_pcEndNode = pcListNode;

			pcListNode->m_tData = tData;
			bResult = true;
		}
		else
		{
			bResult = false;
		}
	}
	else
	{
		pcListNode = new CListNode<T>;

		if( pcListNode != NULL )
		{
			pcListNode->m_pcNextNode = m_pcStartNode;
			m_pcStartNode->m_pcPrevNode = pcListNode;
			m_pcStartNode = pcListNode;

			pcListNode->m_tData = tData;
			bResult = true;
		}
		else
		{
			bResult = false;
		}
	}

	m_iCount++;

	unlock();

	return bResult;
}

template <class T> 
CListNode<T> *CDList<T>::Search( T tData )
{
	CListNode<T>	*pcResultNode;

	pcResultNode = NULL;

	lock();

	for( CListNode<T> *pcTempNode = m_pcStartNode; pcTempNode; pcTempNode=pcTempNode->m_pcNextNode )
	{
		if( pcTempNode->m_tData == tData )
		{
			pcResultNode = pcTempNode;
			break;
		}
	}

	unlock();

	return pcResultNode;
}

template <class T> 
bool CDList<T>::Remove( T tData )
{
	CListNode<T>	*pcListNode;
	bool			bResult;

	bResult = false;

	lock();

	pcListNode = Search( tData );

	if( pcListNode != NULL )
	{
		//StartNode를 날리는 경우.
		if( pcListNode == m_pcStartNode )
		{
			m_pcStartNode = pcListNode->m_pcNextNode; //pcListNode->m_pcNextNode=NULL

			if( m_pcEndNode == pcListNode )
			{
				m_pcEndNode = pcListNode->m_pcNextNode; //pcListNode->m_pcNextNode=NULL
			}
			else
			{
				pcListNode->m_pcNextNode->m_pcPrevNode = NULL;
			}

			delete pcListNode->m_tData;
			delete pcListNode;
		}
		//End Node를 날리는 경우.
		else if( pcListNode == m_pcEndNode )
		{
			pcListNode->m_pcPrevNode->m_pcNextNode = NULL;
			m_pcEndNode = pcListNode->m_pcPrevNode;

			delete pcListNode->m_tData;
			delete pcListNode;
		}
		//그냥 가운데 노드를 날리는경우.
		else
		{
			pcListNode->m_pcPrevNode->m_pcNextNode = pcListNode->m_pcNextNode;
			pcListNode->m_pcNextNode->m_pcPrevNode = pcListNode->m_pcPrevNode;

			delete pcListNode->m_tData;
			delete pcListNode;
		}

		m_iCount--;
	}
	else
	{
		bResult = false;
	}

	unlock();

	return bResult;
}

template <class T> 
void CDList<T>::Destroy()
{
	CListNode<T> *pcNextNode;

	lock();

	for( CListNode<T> *pcTempNode = m_pcStartNode; pcTempNode; )
	{
		pcNextNode = pcTempNode->m_pcNextNode;

		//노드와 그 안의 Data를 지운다.
		delete pcTempNode->m_tData;
		delete pcTempNode;

		pcTempNode = pcNextNode;
	}

	m_iCount = 0;
	m_pcStartNode = NULL;
	m_pcEndNode = NULL;

	unlock();
}

template <class T> 
void CDList<T>::lock()
{
//	WaitForSingleObject( m_hMutex, INFINITE );
	m_Mutex.WLock();
}

template <class T> 
void CDList<T>::unlock()
{
//	ReleaseMutex( m_hMutex );
	m_Mutex.Release();
}

template <class T> 
void CDList<T>::debug()
{
	lock();

	for( CListNode<T> *pcTempNode = m_pcStartNode; pcTempNode; pcTempNode = pcTempNode->m_pcNextNode )
	{
		printf( "Prev:%p, Node:%p, Next:%p, tData:%d\n",pcTempNode->m_pcPrevNode,pcTempNode,pcTempNode->m_pcNextNode, *pcTempNode->m_tData );
	}

	printf( "---------\n" );

	unlock();
}

#endif