//#include "stdafx.h"
#include "StringIntTree.h"

// ---------------------- CStringIntTreeNode ---------------------
CStringIntTreeNode::CStringIntTreeNode(LPCTSTR str,LONG i)
{
    mString = new TCHAR[strlen(str)+1];
	strcpy(mString,str);

	m_lData = i;
}

CStringIntTreeNode::~CStringIntTreeNode()
{
	delete [] mString;
}

// ---------------------- CStringIntTree --------------------------

int CStringIntTree::Compare(CBinTreeNode* p1,CBinTreeNode* p2) const
{
	return strcmp(
		     ((CStringIntTreeNode*)p1)->GetString(),
			 ((CStringIntTreeNode*)p2)->GetString()
			 );
}

void CStringIntTree::Add(LPCTSTR str,LONG i)
{
	Insert( new CStringIntTreeNode(str,i) );
}

BOOL CStringIntTree::DeleteByString(LPCTSTR str, LONG *pData)
{
  CStringIntTreeNode searchNode(str,0); // The int part of the node is
                                        // ignored in comparison

  BOOL bRetval = RemoveNode(&searchNode);

  *pData = searchNode.m_lData;

  return bRetval;
}

CStringIntTreeNode* CStringIntTree::FindByString(LPCTSTR str)
{
  CStringIntTreeNode searchNode(str,0); // The int part of the node is
                                        // ignored in comparison

  CStringIntTreeNode* pRes = (CStringIntTreeNode*) Find(&searchNode);
  return pRes;
}


// tcb_Clear : TraverseCallBack. Delete the node
void CStringIntTree::tcb_Clear(CBinTreeNode* p, void* pParam)
{
  delete (CStringIntTreeNode*)p;
}


void CStringIntTree::Clear()
{
	// ParentLast, so child is deleted prior its parent
	Traverse(ParentLast,tcb_Clear,this);
	CBinTree::Clear();
}

void tcb_Save(CBinTreeNode* p, void* pParam)
{
	/*
	CFile* pFile = (CFile*) pParam;
	CStringIntTreeNode* pNode = (CStringIntTreeNode*) p;

	int i = pNode->GetInt();
	pFile->Write(pNode->GetString(),strlen(pNode->GetString())+1);
	pFile->Write(&i,sizeof(int));
	*/
}

void CStringIntTree::Save(LPCTSTR fileName)
{
	/*
  CFile f;
  VERIFY(f.Open(fileName,CFile::modeCreate | CFile::modeWrite));

  // ParentFirst, so that when we read the file, the same tree
  // structure will be built.
  Traverse(ParentFirst,tcb_Save,&f);

  f.Close();
	*/
}

void CStringIntTree::Load(LPCTSTR fileName)
{
	/*
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

	*/
}



