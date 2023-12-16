/*
* index-tools.h
*
* Author: Taufik F. Abidin
*
*/

#include <stdio.h>

typedef struct treenode * TreeNodePtr;
typedef struct listnode * ListNodePtr;

typedef struct listhead
{
   ListNodePtr head;
   unsigned size;
}ListHead;


typedef struct treenode
{
  char *word;
  ListHead list;
  TreeNodePtr left,right;
}TreeNode;


typedef struct treeroot
{
  TreeNodePtr root;
  unsigned size;
}TreeRoot;


typedef struct listnode
{
  long int docno;
  long int freq;
  ListNodePtr next;
}ListNode;


/* functions prototype */
int initBST(TreeRoot *);
int insertBST(TreeRoot *, char *, long int);
int insertList(ListHead *, long int);
int readList(ListHead *, FILE *);
void treeTraversal(TreeRoot *, FILE *, FILE *, FILE *);
void inorder(TreeNode *, FILE *, FILE *);
void freeList(ListHead *);
void freeNode(TreeNode *);
void freeTree(TreeRoot *);
