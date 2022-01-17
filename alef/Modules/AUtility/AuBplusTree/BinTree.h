#ifndef _BINTREE_H_
#define _BINTREE_H_

#include "ApBase.h"

class CBinTreeNode;

// TraverseCallBack, a callback function, like
// void somefunction(CBinTreeNode*,void*);  
// see CBinTree::Traverse method below.
typedef void (*TraverseCallBack)(CBinTreeNode*,void*);  

// CBinTreeNode. The tree is built by CBinTreeNodes.
// To have any real use, you should subclass it to 
// a class that actually hold some data :-)
class CBinTreeNode
{
  CBinTreeNode* mLeftChild;
  CBinTreeNode* mRightChild;

  // The node also has a pointer to its parent (NULL for the
  // root node). This is to make deletion a bit easier, but
  // technically you could live without it since it is a bit redundant
  // information.
  CBinTreeNode* mParent;
public:
	LONG m_lData;

	// Constructor
	CBinTreeNode():mLeftChild(NULL),mRightChild(NULL),mParent(NULL){}

	// Get methods
	CBinTreeNode* GetLeftChild() const { return mLeftChild; }
	CBinTreeNode* GetRightChild() const { return mRightChild; }
	CBinTreeNode* GetParent() const { return mParent; }

	// Set methods
	void SetLeftChild(CBinTreeNode* p) { mLeftChild=p; }
	void SetRightChild(CBinTreeNode* p) { mRightChild=p; }
	void SetParent(CBinTreeNode* p) { mParent=p; }
};

// CBinTree. Holder of the tree structure. Must be subclassed,
// has a method, Compare, that's pure virtual and thus must 
// be defined elsewhere.
class CBinTree
{
	// The top node. NULL if empty.
	CBinTreeNode* mRoot;

	// Used in traversing
	TraverseCallBack mFunc;
	void* mParam;
	CBinTreeNode* mpSearchNode;

	int mComparisons;
	int mCount;
	int mHeight;
	int mHeightTmp;

public:
	// TraverseOrder. Input parameter to the Traverse function.
	// Specifies in what way the tree should be traversed.
    // Ascending   : 1,2,3,4,5....
	// Descedning  : 9,8,7,6,5....
	// ParentFirst : The parent node will be handeled before its children.
	//               Typically use when the structure is saved, so that
	//               the (possibly balanced) structure wont be altered.
	// ParentLast  : The parent node will be handeled after its children.
	//               Typically use when tree is deleted; got to delete the 
	//               children before deleting their parent.
	enum TraverseOrder { Ascending=0,Descending,ParentFirst,ParentLast };

	// Constructor.
	CBinTree():mRoot(NULL),mComparisons(0),mCount(0),mHeight(0){}

	// Insert. Adds a node to the tree at the right place.
	void Insert(CBinTreeNode* pNode);

	// Return the first CBinTreeNode in the tree where
	// Compare (node,pSearchNode)==0, or NULL if not found.
	CBinTreeNode* Find(CBinTreeNode* pSearchNode);

	// Remove a node.Return non-zero if the node could
	// be found in the tree.
	// The first node where Compare (node,pSearchNode)==0
	// gets zapped.
	BOOL RemoveNode(CBinTreeNode* pSearchNode);

	// Returns the number of comparisons required for the last
	// call to Find. Gives a hint on how balanced the tree is.
	int GetComparisons() const { return mComparisons; }

	// Traverse will call the supplied function, func,  for every node in the tree,
	// passing it a pointer to the node, so you can act opon it.
	// func: The callback function, like void somefunction(CBinTreeNode*,void*);
	// The pParam will also be passed to the function and is a pointer to something.
	// You decide to what, or ignore if you dont need it.
	void Traverse(TraverseOrder to, TraverseCallBack func, void* pParam=NULL);

	// Number of nodes in the tree.
	int GetCount() const { return mCount; }

	// The height of the tree, indicates how balanced it is.
	// The height is the maximum number of comparisons needed to be
	// made (worst case) when searching for an element.
	int GetHeight() const { return mHeight; }

	// Balance minimizes the height, optimizing it.
	void Balance();

	// These two thingies are temp. stuff used in balancing.
	CBinTreeNode** mBalArray; // Array of pointers to nodes
	int mBalArrayCount; 

protected:
	// Compare:
	// p1 < p2 shall return -1
	// p1 = p2 shall return  0
	// p1 > p2 shall return  1
	// You have to redefine it in a subclass, CBinTree can't know
	// what data is significant for comparison in your node 
	virtual int Compare(CBinTreeNode* p1,CBinTreeNode* p2) const = 0;

	// Remove all nodes without deleting them.
	// Not really hard now is it. 
	virtual void Clear() { mRoot = NULL; mCount=0;mHeight=0;}

	// Override if you want to take some special actions when a 
	// node gets removed from the tree.
	virtual void OnRemoveNode(CBinTreeNode* pNode) {};
	
	// Called by Insert.
	void InsertBelow(CBinTreeNode* pParent,CBinTreeNode* pNewNode);

	// Called by Traverse. All similar except for the order in which they call the children.
	void DoTraverse_Ascending(CBinTreeNode* pNode);
	void DoTraverse_Descending(CBinTreeNode* pNode);
	void DoTraverse_ParentFirst(CBinTreeNode* pNode);
	void DoTraverse_ParentLast(CBinTreeNode* pNode);


	// Called by Find. Does the real work.
	CBinTreeNode* DoTraverse_Find(CBinTreeNode* pNode);

	// Called by Balance. 
	void GetFromOrderedArray(int low, int hi);
};
#endif // _BINTREE_H_

