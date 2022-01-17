#ifndef _AU_SPECIALIZE_LIST_FOR_PATHFIND
#define _AU_SPECIALIZE_LIST_FOR_PATHFIND

#include "PLinkedList.h"

//[길찾기용 특수 자료구조]
template<class T, class U>
class CSpecializeListPathFind : public CPriorityList<T, U>
{
public:
	int m_iIncreaseIndex;
	int m_iMaxSwapIndex;

    CPriorityListNode < T, U > *index_end_node;

	//실제 끝노드
    CPriorityListNode < T, U > *m_pcsRealEndNode; //Insert를 하며 만들어지는 EndNode;
	//현재 길찾기가 되고있는 노드.
	CPriorityListNode < T, U > *m_pcsCurrentPathNode;

//Static Priority Swap List
	CSpecializeListPathFind();
	~CSpecializeListPathFind();
	bool insert(const T & KEY, const U & DATA);
	bool Priorityinsert(const T & KEY, const U & DATA);
	CPriorityListNode < T, U > *swapHeadAndTail();
	void resetIndex();
	void allocSwapBuffer( int iMaxNum );
	void deleteSwapBuffer();
	void removeSwapBuffer();

	CPriorityListNode < T, U > *GetNextPath();
};

template < class T, class U > 
CSpecializeListPathFind < T, U >::CSpecializeListPathFind()
{
    // 처음에는 시작과 끝이 모두 t_node임
	m_iIncreaseIndex = 0;
	m_iMaxSwapIndex = 0;
	index_end_node = NULL;
    m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;
}

template < class T, class U > 
CSpecializeListPathFind < T, U >::~CSpecializeListPathFind()
{
    removeAllNode();
}

template < class T, class U >
void CSpecializeListPathFind < T,U >::allocSwapBuffer( int iMaxNum )
{
	lock();
	m_iMaxSwapIndex = iMaxNum;
	unlock();

	for( int i=0; i< iMaxNum; i++ )
	{
		U		data;

		data = new AgpdPathFindPoint;

		//단지 버퍼를 만드는 일이므로 그냥 insert를 쓰는게 맞다.
		insertNode( 0, data );
	}

}

template < class T, class U >
void CSpecializeListPathFind < T,U >::removeSwapBuffer()
{

	removeAllNode();

	lock();

	m_iIncreaseIndex = 0;
	m_iMaxSwapIndex = 0;
	index_end_node = NULL;
	m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;

	unlock();
}

template < class T, class U >
void CSpecializeListPathFind < T,U >::resetIndex()
{
	lock();
	m_iIncreaseIndex = 0;
	index_end_node = NULL;
	m_pcsRealEndNode = NULL;
	m_pcsCurrentPathNode = NULL;
	unlock();
}

template < class T, class U > 
CPriorityListNode < T, U > *CSpecializeListPathFind < T,U >::swapHeadAndTail()
{
	lock();

	CPriorityListNode < T, U > *pcResultNode;

	//아무것도 없는 상태에서 Drop and insert to tail은 의미가 없다.
	//최소한 1개라도 들어있어야한다. 그리고 인덱스 버퍼도 1개 이상이어야한다.
	if( (m_iIncreaseIndex < 1) || (m_iMaxSwapIndex < 1) )
	{
		pcResultNode = NULL;
	}
	else
	{
		//만약 1개의 노드로 이루어진 리스트라면?
		if( m_iMaxSwapIndex == 1 )
		{
			pcResultNode = start_node;
			m_iIncreaseIndex--;
		}
		//
		else
		{
			//현재 작업 노드가 시작노드면 다음 노드를 작업 노드로 정해놓는다.
			if( index_end_node == start_node )
			{
				index_end_node = start_node->NextNode;
			}

			CPriorityListNode < T, U > *pcTmpNode;
			pcTmpNode = start_node;

			//노드를 떼어낸다.
			start_node = pcTmpNode->NextNode;
			pcTmpNode->NextNode->PrevNode = NULL;

			//가장 마지막 노드에 붙여넣는다.
			pcTmpNode->PrevNode = end_node;
			pcTmpNode->NextNode = NULL;
			pcTmpNode->PrevNode->NextNode = pcTmpNode;

			end_node = pcTmpNode;

			pcResultNode = pcTmpNode;
			m_iIncreaseIndex--;
		}
	}

	unlock();

	return pcResultNode;
}

template < class T, class U > 
bool CSpecializeListPathFind < T,U >::insert(const T & KEY, const U & DATA)
{
	bool			bResult;

	bResult = false;

	lock();

	CPriorityListNode < T, U > *pcTmpNode;

	//버퍼에 넣을수 있는 최대 갯수를 넘어갔다.
	if( m_iIncreaseIndex >= m_iMaxSwapIndex )
	{
		unlock();
		return bResult;
	}

	//End 노드를 찾는다.( 값을 바꾸고 떼어내기 위해 )
	pcTmpNode = end_node;

	//pcTmpNode를 검사한다.
	if( pcTmpNode == NULL )
	{
		unlock();
		return bResult;
	}
	else
	{
		pcTmpNode->key = KEY;
		pcTmpNode->data->m_iX = DATA->m_iX;
		pcTmpNode->data->m_iY = DATA->m_iY;
		pcTmpNode->data->m_fX = DATA->m_fX;
		pcTmpNode->data->m_fY = DATA->m_fY;
	}

	//Swap 하기위해 EndNode를 떼어낸다.
	end_node = pcTmpNode->PrevNode;
	pcTmpNode->PrevNode->NextNode = NULL;

	//가장 앞 노드에 떼어낸 데이터를 넣는다.
	CPriorityListNode < T, U > *pcCurrentNode = start_node;

	pcTmpNode->PrevNode = NULL;
	pcTmpNode->NextNode = pcCurrentNode;
	pcCurrentNode->PrevNode = pcTmpNode;

	start_node = pcTmpNode;

	//성공적으로 Swap되었으니 index를 증가시킨다.

	if( m_iIncreaseIndex == 0 )
	{
		m_pcsRealEndNode = pcTmpNode;
	}

	m_iIncreaseIndex++;
	bResult = true;

	unlock();

	return bResult;
}

template < class T, class U > 
bool CSpecializeListPathFind < T,U >::Priorityinsert(const T & KEY, const U & DATA)
{
	bool			bResult;

	bResult = false;

	lock();

	CPriorityListNode < T, U > *pcTmpNode;

	//버퍼에 넣을수 있는 최대 갯수를 넘어갔다.
	if( m_iIncreaseIndex >= m_iMaxSwapIndex )
	{
		unlock();
		return bResult;
	}

	//Swap할 노드를 찾는다.
	if( index_end_node == NULL )
	{
		pcTmpNode = start_node;
		index_end_node = start_node->NextNode;
	}
	else
	{
		pcTmpNode = index_end_node;
		index_end_node = pcTmpNode->NextNode;
	}

	//pcTmpNode를 검사한다.
	if( pcTmpNode == NULL )
	{
		unlock();
		return bResult;
	}
	else
	{
		pcTmpNode->key = KEY;
		pcTmpNode->data->m_iX = DATA->m_iX;
		pcTmpNode->data->m_iY = DATA->m_iY;
	}

	//Swap 하기위해 노드를 떼어낸다.
	if( pcTmpNode == start_node )
	{
		start_node = pcTmpNode->NextNode;
		pcTmpNode->NextNode->PrevNode = NULL;
	}
	else if( pcTmpNode == end_node )
	{
		end_node = pcTmpNode->PrevNode;
		pcTmpNode->PrevNode->NextNode = NULL;
	}
	else
	{
		pcTmpNode->PrevNode->NextNode = pcTmpNode->NextNode;
		pcTmpNode->NextNode->PrevNode = pcTmpNode->PrevNode;
	}

	//가장 앞 노드에 넣는경우.
	if( m_iIncreaseIndex == 0 )
	{
		pcTmpNode->PrevNode = NULL;
		pcTmpNode->NextNode = start_node;
		start_node->PrevNode = pcTmpNode;

		start_node = pcTmpNode;
	}
	else
	{
		bool		bFound = false;
		CPriorityListNode < T, U > *pcCurrentNode = start_node;

		for( int iCounter = 0; iCounter < m_iIncreaseIndex; iCounter++ )
		{
			if( KEY < pcCurrentNode->key )
			{
				//가장 앞에 들어가는 경우
				if( pcCurrentNode == start_node )
				{
					//특정 위치에 넣는다.
					pcTmpNode->PrevNode = NULL;
					pcTmpNode->NextNode = pcCurrentNode;
					pcCurrentNode->PrevNode = pcTmpNode;

					start_node = pcTmpNode;
				}
				//가운데에 붙는경우
				else
				{
					pcCurrentNode->PrevNode->NextNode = pcTmpNode;
					pcTmpNode->PrevNode = pcCurrentNode->PrevNode;
					pcTmpNode->NextNode = pcCurrentNode;
					pcCurrentNode->PrevNode = pcTmpNode;
				}

				bFound = true;
				break;
			}

			pcCurrentNode = pcCurrentNode->NextNode;
		}

		//가장 끝에 붙는 경우
		if( bFound == false )
		{
			if( (pcCurrentNode == end_node) || (pcCurrentNode == NULL))
			{
				pcTmpNode->PrevNode = end_node;
				pcTmpNode->NextNode = NULL;
				pcTmpNode->PrevNode->NextNode = pcTmpNode;

				end_node = pcTmpNode;
			}
			else
			{
				//특정 위치에 넣는다.
				pcCurrentNode->PrevNode->NextNode = pcTmpNode;
				pcTmpNode->PrevNode = pcCurrentNode->PrevNode;
				pcTmpNode->NextNode = pcCurrentNode;
				pcCurrentNode->PrevNode = pcTmpNode;
			}
		}
	}

	//성공적으로 Swap되었으니 index를 증가시킨다.
	m_iIncreaseIndex++;
	bResult = true;

	unlock();

	return bResult;
}

template < class T, class U > 
CPriorityListNode < T, U > *CSpecializeListPathFind < T,U >::GetNextPath()
{
	CPriorityListNode < T, U > *pcsResultNode;

	lock();

	if( m_pcsCurrentPathNode == m_pcsRealEndNode )
	{
		pcsResultNode = NULL;
	}
	else
	{
		if( m_pcsCurrentPathNode == NULL )
		{
			m_pcsCurrentPathNode = start_node;
		}
		else 
		{
			m_pcsCurrentPathNode = m_pcsCurrentPathNode->NextNode;
		}

		pcsResultNode = m_pcsCurrentPathNode;
	}

	unlock();

	return pcsResultNode;
}

#endif