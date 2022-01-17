//#include "stdafx.h"
#include "IntIntTree.h"

// ---------------------- CStringIntTreeNode ---------------------
CIntIntTreeNode::CIntIntTreeNode(LONG lLong,LONG i)
{
    m_lLong = lLong;

	m_lData = i;
}

CIntIntTreeNode::~CIntIntTreeNode()
{
}

// ---------------------- CIntIntTree --------------------------

int CIntIntTree::Compare(CBinTreeNode* p1,CBinTreeNode* p2) const
{
	if (((CIntIntTreeNode*)p1)->GetKey() > ((CIntIntTreeNode*)p2)->GetKey())
		return 1;
	else if (((CIntIntTreeNode*)p1)->GetKey() < ((CIntIntTreeNode*)p2)->GetKey())
		return (-1);
	else
	{
		return 0;
	}
}

void CIntIntTree::Add(LONG lLong,LONG i)
{
	CIntIntTreeNode *pNewNode = new CIntIntTreeNode(lLong, i);

	Insert(pNewNode);
}

BOOL CIntIntTree::DeleteByLong(LONG lLong, LONG* pData)
{
  CIntIntTreeNode searchNode(lLong,0); // The int part of the node is
                                        // ignored in comparison

  BOOL bRetval = RemoveNode(&searchNode);

  *pData = searchNode.m_lData;

  return bRetval;
}

CIntIntTreeNode* CIntIntTree::FindByLong(LONG lLong)
{
  CIntIntTreeNode searchNode(lLong,0); // The int part of the node is
                                        // ignored in comparison

  CIntIntTreeNode* pRes = (CIntIntTreeNode*) Find(&searchNode);
  return pRes;
}


// tcb_Clear : TraverseCallBack. Delete the node
void CIntIntTree::tcb_Clear(CBinTreeNode* p, void* pParam)
{
  delete (CIntIntTreeNode*)p;
}


void CIntIntTree::Clear()
{
	// ParentLast, so child is deleted prior its parent
	Traverse(ParentLast,tcb_Clear,this);
	CBinTree::Clear();
}

	/*
void tcb_Save(CBinTreeNode* p, void* pParam)
{
	/*
	CFile* pFile = (CFile*) pParam;
	CStringIntTreeNode* pNode = (CStringIntTreeNode*) p;

	int i = pNode->GetInt();
	pFile->Write(pNode->GetString(),strlen(pNode->GetString())+1);
	pFile->Write(&i,sizeof(int));
}

void CStringIntTree::Save(LPCTSTR fileName)
{
  CFile f;
  VERIFY(f.Open(fileName,CFile::modeCreate | CFile::modeWrite));

  // ParentFirst, so that when we read the file, the same tree
  // structure will be built.
  Traverse(ParentFirst,tcb_Save,&f);

  f.Close();
}

void CStringIntTree::Load(LPCTSTR fileName)
{
  CFile f;
  VERIFY(f.Open(fileName,CFile::modeRead));

  Clear();

  int pos = 0;
  int size = f.GetLength();

  LPCTSTR str;
  int* pInt;

  char* buf = new char[size];

  f.Read(buf,size);  
  f.Close();

  while (pos<size)
  {
	  str = &buf[pos];
	  pInt= (int*)&buf[pos + strlen(str)+1];

	  Add(str,*pInt);

	  pos+=strlen(str)+sizeof(int)+1;
  }

  delete [] buf;

}
	*/



