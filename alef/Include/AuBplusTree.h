/******************************************************************************
Module:  AuBplusTree.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 12
******************************************************************************/

#if !defined(__AUBPLUSTREE_H__)
#define __AUBPLUSTREE_H__

#include "ApBase.h"
#include "IntIntTree.h"
#include "StringIntTree.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuBplusTreeD" )
#else
#pragma comment ( lib , "AuBplusTree" )
#endif
#endif


class AuBTree {
private:
	INT16				m_nFlag;

	CStringIntTree		m_csStringTree;
	CIntIntTree			m_csIntTree;

public:
	AuBTree();
	~AuBTree();

	BOOL Initialize(INT16 nOrder, INT16 nFlag);

	BOOL Add(PVOID pKey, INT32 lIndex);
	BOOL Remove(PVOID pKey, INT32* plIndex);
	BOOL Search(PVOID pKey, INT32* plIndex);

	BOOL Destroy();
};

#endif // __AUBPLUSTREE_H__
