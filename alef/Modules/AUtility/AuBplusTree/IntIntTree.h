#ifndef _INTTREE_H_
#define _INTTREE_H_

#include "BinTree.h"


// CStringIntTreeNode. A node that holds a string and an int
// Since it inherits CBinTreeNode, can it be managed
// by the CBinTree class.
class CIntIntTreeNode : public CBinTreeNode
{
	LONG m_lLong;
public:
	CIntIntTreeNode(LONG lLong,LONG i);
	~CIntIntTreeNode();

	LONG GetKey() const { return m_lLong; }
};

// CStringIntTree. A binary tree of CStringIntTreeNode nodes.
class CIntIntTree : public CBinTree
{
protected:
  // Overrides the pure virtual compare function.
  // and compares the strings of the nodes.
  int Compare(CBinTreeNode* p1,CBinTreeNode* p2) const;

  virtual void OnRemoveNode(CBinTreeNode* pNode) { delete (CIntIntTreeNode*)pNode; };
public:
  // Destructor
	~CIntIntTree() { Clear(); };
  
  // Function to directly add CStringIntTreeNode:s to the tree.
  void Add(LONG lLong,LONG i);

  BOOL DeleteByLong(LONG lLong, LONG* pData);
  /*
  // Save the contents to a file.
  void Save(LPCTSTR fileName);

  // Load the contents from a file.
  void Load(LPCTSTR fileName);
  */

  // Delete all nodes (not only remove them from the tree)
  static void tcb_Clear(CBinTreeNode* p, void* pParam);
  void Clear();

  // FindByString. Returns the CStringIntTreeNode matching the
  // given string str or NULL if couldn't be found.
  CIntIntTreeNode* FindByLong(LONG lLong);
};

#endif // _INTTREE_H_

