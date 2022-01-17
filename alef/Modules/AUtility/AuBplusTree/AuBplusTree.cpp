/******************************************************************************
Modle:  AuBplusTree.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 12
******************************************************************************/

#include "AuBplusTree.h"

AuBTree::AuBTree()
{
}

AuBTree::~AuBTree()
{
	Destroy();
}

BOOL AuBTree::Initialize(INT16 nOrder, INT16 nFlag)
{
	m_nFlag = nFlag;

	return TRUE;
}

BOOL AuBTree::Add(PVOID pKey, INT32 lIndex)
{
	if (Search(pKey, NULL))
		return FALSE;

	if (m_nFlag == AUBTREE_FLAG_KEY_STRING)
		m_csStringTree.Add((LPCTSTR) pKey, lIndex);
	else
		m_csIntTree.Add(*(LONG *) pKey, lIndex);
	
	return TRUE;
}

BOOL AuBTree::Remove(PVOID pKey, INT32* plIndex)
{
	if (m_nFlag == AUBTREE_FLAG_KEY_STRING)
	{
		return m_csStringTree.DeleteByString((LPCTSTR) pKey, (LONG *) plIndex);
	}
	else
	{
		return m_csIntTree.DeleteByLong(*(LONG *) pKey, (LONG *) plIndex);
	}

	return FALSE;
}

BOOL AuBTree::Search(PVOID pKey, INT32* plIndex)
{
	if (m_nFlag == AUBTREE_FLAG_KEY_STRING)
	{
		CStringIntTreeNode *pNode = m_csStringTree.FindByString((LPCTSTR) pKey);
		if (pNode)
		{
			if (plIndex != NULL)
				*plIndex = pNode->m_lData;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		CIntIntTreeNode *pNode = m_csIntTree.FindByLong(*(LONG *) pKey);
		if (pNode)
		{
			if (plIndex)
			{
				*plIndex = pNode->m_lData;
			}

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	return FALSE;
}

BOOL AuBTree::Destroy()
{
	if (m_nFlag == AUBTREE_FLAG_KEY_STRING)
		m_csStringTree.Clear();
	else
		m_csIntTree.Clear();

	return TRUE;
}