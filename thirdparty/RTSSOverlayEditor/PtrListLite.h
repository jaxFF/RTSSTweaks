// PtrListLite.h: interface for the CPtrListLite class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _PTRLISTLITE_H_INCLUDED_
#define _PTRLISTLITE_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
typedef struct PTR_LIST_NODE
{
	PTR_LIST_NODE*	pPrev;
	PTR_LIST_NODE*	pNext;
	LPVOID			pData;
} PTR_LIST_NODE, *LPPTR_LIST_NODE;
//////////////////////////////////////////////////////////////////////
typedef int (*COMPARE_NODES_PROC) (LPPTR_LIST_NODE,LPPTR_LIST_NODE);
//////////////////////////////////////////////////////////////////////
class CPtrListLite  
{
public:
	LPPTR_LIST_NODE AddHead(LPVOID pData);
		//adds node to head
	LPPTR_LIST_NODE AddTail(LPVOID pData);
		//adds node to tail

	LPPTR_LIST_NODE GetTail();
		//returns ptr to tail node
	LPPTR_LIST_NODE GetHead();
		//returns ptr to head node

	LPVOID GetAt(LPPTR_LIST_NODE pNode);
		//returns ptr to the current node data
	LPVOID GetPrev(LPPTR_LIST_NODE& pNode);
		//returns ptr to the current node data and modifies current node ptr
	LPVOID GetNext(LPPTR_LIST_NODE& pNode);
		//returns ptr to the current node data and modifies current node ptr

	LPPTR_LIST_NODE Find(DWORD dwIndex);
		//finds nodeby index
	LPPTR_LIST_NODE Find(LPVOID pData);
		//finds node by data ptr

	void SwapWithPrev(LPPTR_LIST_NODE pNode);
		//swaps a node with the previous one
	void SwapWithNext(LPPTR_LIST_NODE pNode);
		//swaps the node with the next one

	void Sort(BOOL bAscending, COMPARE_NODES_PROC pCompareProc);
		//sorts the list using user specified node comparison procedure

	BOOL IsEmpty();
		//returns TRUE if the list is empty
	DWORD GetCount();
		//returns total count of items in the list

	void RemoveAt(LPPTR_LIST_NODE pNode);
		//removes specified node
	void RemoveAll();
		//removes all items in the list

	CPtrListLite();
	virtual ~CPtrListLite();
private:
	LPPTR_LIST_NODE m_pHead;
		//ptr to head node
	LPPTR_LIST_NODE m_pTail;
		//ptr to tail node
};
//////////////////////////////////////////////////////////////////////
#endif
