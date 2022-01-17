//#include "stdafx.h"
#include "BinTree.h"
#include "assert.h"

void CBinTree::Insert(CBinTreeNode* pNode)
{
  if (mRoot==NULL)
  {
	  mRoot = pNode;
	  mCount = 1;
	  mHeight = 1;
	  mRoot->SetParent(NULL);
  }
  else
  {
	  mHeightTmp = 1;
      InsertBelow(mRoot,pNode);
	  mCount++;

	  if (mHeightTmp>mHeight)
		  mHeight = mHeightTmp;
  }

}

void CBinTree::InsertBelow(CBinTreeNode* mParent,CBinTreeNode* mNewNode)
{
  int i = Compare(mNewNode,mParent);
  mHeightTmp++;
  switch(i)
  {
  case -1: 
	  // mNewNode < mParent
	  if (mParent->GetLeftChild()==NULL)
	  {
		  // No left child? Okie then, mNewNode is the new left child 
		  mParent->SetLeftChild(mNewNode);
		  mNewNode->SetParent(mParent);
	  }
	  else
	  {
		  InsertBelow(mParent->GetLeftChild(),mNewNode);
	  };
	  break;
  case 0:
  case 1:
	  // mNewNode >= mParent
	  if (mParent->GetRightChild()==NULL)
	  {
		  // No right child? Okie then, mNewNode is the new right child 
		  mParent->SetRightChild(mNewNode);
		  mNewNode->SetParent(mParent);
	  }
	  else
	  {
		  InsertBelow(mParent->GetRightChild(),mNewNode);
	  };
	  break;
  default:
	  assert(FALSE);
  };
}

void CBinTree::Traverse(TraverseOrder to, TraverseCallBack func, void* pParam)
{
  mFunc = func;
  mParam = pParam;

  switch(to)
  {
  case Ascending:
    DoTraverse_Ascending(mRoot);
	break;
  case Descending:
    DoTraverse_Descending(mRoot);
	break;
  case ParentFirst:
    DoTraverse_ParentFirst(mRoot);
	break;
  case ParentLast:
    DoTraverse_ParentLast(mRoot);
	break;
  default:
	  assert(FALSE);
  }

}

void CBinTree::DoTraverse_Ascending(CBinTreeNode* pNode)
{
	if (!pNode)
		return;

	DoTraverse_Ascending(pNode->GetLeftChild());
	mFunc(pNode,mParam);
	DoTraverse_Ascending(pNode->GetRightChild());
}

void CBinTree::DoTraverse_Descending(CBinTreeNode* pNode)
{
	if (!pNode)
		return;

	DoTraverse_Descending(pNode->GetRightChild());
	mFunc(pNode,mParam);
	DoTraverse_Descending(pNode->GetLeftChild());
}

void CBinTree::DoTraverse_ParentFirst(CBinTreeNode* pNode)
{
	if (!pNode)
		return;

	mFunc(pNode,mParam);
	DoTraverse_ParentFirst(pNode->GetLeftChild());
	DoTraverse_ParentFirst(pNode->GetRightChild());
}

void CBinTree::DoTraverse_ParentLast(CBinTreeNode* pNode)
{
	if (!pNode)
		return;

	DoTraverse_ParentLast(pNode->GetLeftChild());
	DoTraverse_ParentLast(pNode->GetRightChild());
	mFunc(pNode,mParam);
}

CBinTreeNode* CBinTree::Find(CBinTreeNode* pSearchNode)
{
	mpSearchNode = pSearchNode;
	mComparisons = 0;
	return DoTraverse_Find(mRoot);
}

// DoTraverse_Find will, unlike the other DoTraverse_xxx, not 
// go through _all_ nodes, but stop when node is found or 
// is decided can't be found.

CBinTreeNode* CBinTree::DoTraverse_Find(CBinTreeNode* node)
{
  // Reached a dead end, node couldn't be found.
  if (!node)
	  return NULL;

  mComparisons++;
  int iComp = Compare(node,mpSearchNode);

  // Found the node we were looking for, return it.
  if (iComp == 0)
	  return node;

  // node > mpSearchNode, look if it is by the left 
  if (iComp > 0)
	  return DoTraverse_Find(node->GetLeftChild());
  
  // node < mpSearchNode, look if it is by the right
  // if (iComp < 0)
  return DoTraverse_Find(node->GetRightChild());
}

// tcb_Balance: TraverseCallBack
// Add the node into the array.
// pParam is the tree (so we can get the array)
void tcb_Balance(CBinTreeNode* pNode,void* pParam)
{
	CBinTree* pTree = (CBinTree*) pParam;
	pTree->mBalArray[pTree->mBalArrayCount] = pNode;
    pTree->mBalArrayCount++;
}

// Bring balance to the force.
void CBinTree::Balance()
{
    // Setup an array that will hold the nodes
	mBalArray = new CBinTreeNode*[mCount];
	mBalArrayCount=0;

	// Put the nodes into the array in ascending order (ie sorted)
    Traverse(Ascending,tcb_Balance,this);

    // Clarifying the array now holds all the elements
	assert(mCount == mBalArrayCount);

	// Remove the nodes from the tree (easily done).
	// We will put 'em back soon enough.
	CBinTree::Clear();


	// Reset the nodes so they don't have any children,
	// they will be given new as nodes get inserted back into to the tree.
	for (int i=0;i<mBalArrayCount;i++)
	{
		mBalArray[i]->SetLeftChild(NULL);
		mBalArray[i]->SetRightChild(NULL);
		mBalArray[i]->SetParent(NULL);
	}

	// Insert the nodes back to the tree in a balanced fashion.
	GetFromOrderedArray(0,mBalArrayCount-1);

    // Clarifying all elements have been inserted back from the array
	assert(mCount == mBalArrayCount);

	delete mBalArray;
}

// DoBalance.
// Insert the node in the middle position between 
// low and hi from the mBalArray array. 
// Recurse and the array elements < middlePos and > middlePos.
void CBinTree::GetFromOrderedArray(int low, int hi)
{

  if (hi<low)
	  return;

  int middlePos;
  middlePos = low+(hi-low)/2;

  Insert(mBalArray[middlePos]);

  GetFromOrderedArray(low,middlePos-1);
  GetFromOrderedArray(middlePos+1,hi);
}

BOOL CBinTree::RemoveNode(CBinTreeNode* pSearchNode)
{
  CBinTreeNode* pNode = Find(pSearchNode);
  if (!pNode)
	  return FALSE;

  int iCount = mCount;
  
  CBinTreeNode* pParent = pNode->GetParent();

  // Ok, so it has a parent, then we'll simply just disconnect it.
  if (pParent)
  {
	  if (pParent->GetLeftChild() == pNode)
	  {
		  pParent->SetLeftChild(NULL);
	  }
	  else
	  {
		  assert(pParent->GetRightChild() == pNode);
		  pParent->SetRightChild(NULL);
	  }
  }
  else
  {
	  // No parent? Then we're deleting the root node.
	  assert(pNode == mRoot);
	  mRoot = NULL;
  }

  // Disconnected, now we reconnect its children (if any)
  // just by adding them as we add any other node. Their
  // respective children will come along, since Insert doesnt
  // tamper with the inserted node's children.
  if (pNode->GetLeftChild())
	  Insert(pNode->GetLeftChild());
  if (pNode->GetRightChild())
	  Insert(pNode->GetRightChild());

  mCount = iCount-1;

  pSearchNode->m_lData = pNode->m_lData;

  // Give the subclass a chance to do stuff to the removed node.
  OnRemoveNode(pNode);
  return TRUE;

}