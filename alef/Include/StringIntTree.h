#ifndef _STRINGTREE_H_
#define _STRINGTREE_H_

#include "BinTree.h"


// CStringIntTreeNode. A node that holds a string and an int
// Since it inherits CBinTreeNode, can it be managed
// by the CBinTree class.
class CStringIntTreeNode : public CBinTreeNode
{
	LPTSTR mString;
public:
	CStringIntTreeNode(LPCTSTR str,LONG i);
	~CStringIntTreeNode();

	LPCTSTR GetString() const { return mString; }
};

// CStringIntTree. A binary tree of CStringIntTreeNode nodes.
class CStringIntTree : public CBinTree
{
protected:
  // Overrides the pure virtual compare function.
  // and compares the strings of the nodes.
  int Compare(CBinTreeNode* p1,CBinTreeNode* p2) const;

  virtual void OnRemoveNode(CBinTreeNode* pNode) { delete (CStringIntTreeNode*)pNode; };
public:
  // Destructor
	~CStringIntTree() { Clear(); };
  
  // Function to directly add CStringIntTreeNode:s to the tree.
  void Add(LPCTSTR str,LONG i);

  BOOL DeleteByString(LPCTSTR str, LONG *pData);
  // Save the contents to a file.
  void Save(LPCTSTR fileName);

  // Load the contents from a file.
  void Load(LPCTSTR fileName);

  // Delete all nodes (not only remove them from the tree)
  static void tcb_Clear(CBinTreeNode* p, void* pParam);
  void Clear();

  // FindByString. Returns the CStringIntTreeNode matching the
  // given string str or NULL if couldn't be found.
  CStringIntTreeNode* FindByString(LPCTSTR str);
};

#endif // _STRINGTREE_H_

