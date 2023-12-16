/*
 * Index-tools.c 
 *
 * Author: Taufik F. Abidin
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "index-tools.h"


/* BST initialization */
int initBST(TreeRoot *pBST)
{
  pBST->root=NULL;
  pBST->size=0;
  return 1;
}


/* Inserting words into BST */
int insertBST(TreeRoot *pBST, char *word, long int docno)
{
  TreeNodePtr new;
  TreeNodePtr current=pBST->root;
  TreeNodePtr previous=NULL;
  char *term;

  while(current!=NULL)
  {
    previous=current;

    /* if the word is found in the tree then
       update the linked list on the current node */    

    if(strcmp(word,current->word)==0)
    {
      insertList(&(current->list),docno);
      return 1;
    }
    else if(strcmp(word,current->word)<0)
    {
      current=current->left;
    }
    else
    {
      current=current->right;
    }
  }

  /* Allocate new BST node for the word */
  new=(TreeNodePtr) malloc(sizeof(TreeNode));
  if(new==NULL)
  {
    printf("Memory allocation for BST node fails...\n");
    return 0;
  }

  term = (char *) malloc(strlen(word)+1);
  strcpy(term,word);

  new->word=term;
  new->list.head=NULL;
  new->list.size=0;
  new->left=NULL;
  new->right=NULL;
  pBST->size++;

  if(previous==NULL)
  {
    pBST->root=new;
    insertList(&(new->list),docno);
    return 1;
  }

  if(strcmp(word,previous->word)<0)
  {
    previous->left=new;
  }
  else
  {
    previous->right=new;
  }
  insertList(&(new->list),docno);
  return 1;
}


static int totalTerm=0;
void treeTraversal(TreeRoot *pBST, FILE *finv, FILE *fvoc, FILE *fpar)
{
  TreeNodePtr current=pBST->root;
  if(current!=NULL)
  {
    inorder(current, finv, fvoc);
    fwrite(&totalTerm, sizeof(long int),1,fpar);
  }
}


void inorder(TreeNodePtr tnode, FILE *finv, FILE *fvoc)
{
  if(tnode!=NULL)
  {
    long int offset;
    int totalList;

    inorder(tnode->left, finv, fvoc);  /* traverse to the left of BST */
    offset=ftell(finv);                /* get the offset of finv pointer */

    totalList = readList(&(tnode->list), finv);

    fprintf(fvoc,"%s %d %ld\n",tnode->word, totalList, offset);
    totalTerm++;

    inorder(tnode->right, finv, fvoc); /* traverse to the right of BST */
  }
}


int insertList(ListHead *ls, long int docno)
{
  ListNodePtr new;
     
  /* if it is the same document, then just update the frequency, without having to
     create new linked list node */

  if((ls->head!=NULL) && ((ls->head->docno) == docno))
  {
    (ls->head->freq)++;
  }
  else
  {
    new = (ListNodePtr) malloc(sizeof(ListNode));
    if(new==NULL)
    {
      printf("Memory allocation for linked list node fails...\n");
      return 0;
    }
    new->docno=docno;
    new->freq=1;
    new->next=ls->head;
    ls->head=new;
    ls->size++;
  }
  return 1;
}


int readList(ListHead *ls, FILE *finv)
{
  int totalvalue=0;
  ListNodePtr current=ls->head;

  while(current!=NULL)
  {
    fwrite(&(current->docno),sizeof(long int),1,finv);
    fwrite(&(current->freq),sizeof(long int),1,finv);
    current=current->next;
    totalvalue+=2;
  }
  return totalvalue;
}


void freeTree(TreeRoot *pBST)
{
  TreeNodePtr current;
  current=pBST->root;

  if(current!=NULL)
  {
    freeNode(current);
  }

  pBST->root=NULL;
  pBST->size=0;
}


void freeNode(TreeNodePtr tnode)
{
  if(tnode!=NULL)
  {
    freeNode(tnode->left);
    freeNode(tnode->right);
    freeList(&(tnode->list));
    free(tnode);
  }
}


void freeList(ListHead *ls)
{
  ListNodePtr next;
  ListNodePtr current=ls->head;

  while(current!=NULL)
  {
    next=current->next;
    current=NULL;
    current=next;
  }
}
